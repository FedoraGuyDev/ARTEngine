// udp.hpp - Header-only UDP (IPv4) socket wrapper para Windows y POSIX.
// Requiere C++17.  En Windows con MinGW linkear con -lws2_32 (MSVC lo hace solo).
//
// Uso:
//   udp::Socket s;
//   s.bind(9000);                                   // opcional para el que envia
//   s.send_to("hola", 4, "127.0.0.1", 9000);
//   char buf[1500]; udp::Endpoint from;
//   if (auto n = s.recv_from(buf, sizeof buf, from, 1000))  // timeout 1000 ms
//       std::printf("%zu bytes de %s:%u\n", *n, from.host.c_str(), from.port);
#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>

#ifdef _WIN32
  #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
  #endif
  #ifndef NOMINMAX
    #define NOMINMAX
  #endif
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #ifdef _MSC_VER
    #pragma comment(lib, "ws2_32.lib")
  #endif
#else
  #include <arpa/inet.h>
  #include <cerrno>
  #include <fcntl.h>
  #include <netdb.h>
  #include <netinet/in.h>
  #include <poll.h>
  #include <sys/socket.h>
  #include <sys/types.h>
  #include <unistd.h>
#endif

namespace udp {

struct Endpoint {
    std::string host;   // IPv4 en texto, ej. "192.168.1.10"
    std::uint16_t port = 0;
};

namespace detail {

#ifdef _WIN32
using native_socket = SOCKET;
inline const native_socket invalid_socket = INVALID_SOCKET;
inline int last_error() { return ::WSAGetLastError(); }
inline void close_socket(native_socket s) { ::closesocket(s); }
inline bool is_would_block(int e) { return e == WSAEWOULDBLOCK; }
inline bool is_timeout(int e) { return e == WSAETIMEDOUT || e == WSAEWOULDBLOCK; }
// Windows reporta WSAECONNRESET en recvfrom si un envio previo recibio ICMP unreachable.
inline bool is_ignorable(int e) { return e == WSAECONNRESET || e == WSAEMSGSIZE; }

// Inicializa Winsock una vez por proceso.
struct WsaInit {
    WsaInit() {
        WSADATA d;
        int r = ::WSAStartup(MAKEWORD(2, 2), &d);
        if (r != 0) throw std::system_error(r, std::system_category(), "WSAStartup");
    }
    ~WsaInit() { ::WSACleanup(); }
};
inline void ensure_init() { static WsaInit init; }
#else
using native_socket = int;
inline const native_socket invalid_socket = -1;
inline int last_error() { return errno; }
inline void close_socket(native_socket s) { ::close(s); }
inline bool is_would_block(int e) { return e == EAGAIN || e == EWOULDBLOCK; }
inline bool is_timeout(int e) { return e == EAGAIN || e == EWOULDBLOCK; }
inline bool is_ignorable(int e) { return e == ECONNREFUSED; }
inline void ensure_init() {}
#endif

[[noreturn]] inline void fail(const char* what) {
    throw std::system_error(last_error(), std::system_category(), what);
}

inline sockaddr_in make_addr(const std::string& host, std::uint16_t port) {
    sockaddr_in a{};
    a.sin_family = AF_INET;
    a.sin_port = htons(port);
    if (host.empty() || host == "0.0.0.0") {
        a.sin_addr.s_addr = htonl(INADDR_ANY);
        return a;
    }
    if (::inet_pton(AF_INET, host.c_str(), &a.sin_addr) == 1) return a;

    // No es una IP literal: resolver nombre (DNS).
    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    addrinfo* res = nullptr;
    if (::getaddrinfo(host.c_str(), nullptr, &hints, &res) != 0 || !res)
        throw std::runtime_error("udp: no se pudo resolver el host: " + host);
    a.sin_addr = reinterpret_cast<sockaddr_in*>(res->ai_addr)->sin_addr;
    ::freeaddrinfo(res);
    return a;
}

inline Endpoint to_endpoint(const sockaddr_in& a) {
    char buf[INET_ADDRSTRLEN] = {};
    ::inet_ntop(AF_INET, &a.sin_addr, buf, sizeof buf);
    return Endpoint{buf, ntohs(a.sin_port)};
}

} // namespace detail

class Socket {
public:
    Socket() {
        detail::ensure_init();
        fd_ = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (fd_ == detail::invalid_socket) detail::fail("socket");
    }
    ~Socket() { close(); }

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;
    Socket(Socket&& o) noexcept : fd_(std::exchange(o.fd_, detail::invalid_socket)) {}
    Socket& operator=(Socket&& o) noexcept {
        if (this != &o) { close(); fd_ = std::exchange(o.fd_, detail::invalid_socket); }
        return *this;
    }

    bool is_open() const { return fd_ != detail::invalid_socket; }
    detail::native_socket native_handle() const { return fd_; }

    void close() {
        if (fd_ != detail::invalid_socket) {
            detail::close_socket(fd_);
            fd_ = detail::invalid_socket;
        }
    }

    // Enlaza a un puerto local (0 = puerto efimero). Devuelve el puerto real.
    std::uint16_t bind(std::uint16_t port, const std::string& address = "0.0.0.0",
                       bool reuse_addr = true) {
        if (reuse_addr) {
            int one = 1;
            ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR,
                         reinterpret_cast<const char*>(&one), sizeof one);
        }
        sockaddr_in a = detail::make_addr(address, port);
        if (::bind(fd_, reinterpret_cast<sockaddr*>(&a), sizeof a) != 0) detail::fail("bind");
        return local_port();
    }

    std::uint16_t local_port() const {
        sockaddr_in a{};
        socklen_t len = sizeof a;
        if (::getsockname(fd_, reinterpret_cast<sockaddr*>(&a), &len) != 0)
            detail::fail("getsockname");
        return ntohs(a.sin_port);
    }

    void set_broadcast(bool on) {
        int v = on ? 1 : 0;
        if (::setsockopt(fd_, SOL_SOCKET, SO_BROADCAST,
                         reinterpret_cast<const char*>(&v), sizeof v) != 0)
            detail::fail("setsockopt(SO_BROADCAST)");
    }

    void set_nonblocking(bool on) {
#ifdef _WIN32
        u_long m = on ? 1 : 0;
        if (::ioctlsocket(fd_, FIONBIO, &m) != 0) detail::fail("ioctlsocket");
#else
        int fl = ::fcntl(fd_, F_GETFL, 0);
        if (fl < 0 || ::fcntl(fd_, F_SETFL, on ? (fl | O_NONBLOCK) : (fl & ~O_NONBLOCK)) < 0)
            detail::fail("fcntl");
#endif
    }

    // Envia un datagrama. Devuelve bytes enviados, o nullopt si el socket es
    // no bloqueante y el buffer de envio esta lleno.
    std::optional<std::size_t> send_to(const void* data, std::size_t len,
                                       const std::string& host, std::uint16_t port) {
        return send_to(data, len, detail::make_addr(host, port));
    }
    std::optional<std::size_t> send_to(const void* data, std::size_t len, const Endpoint& to) {
        return send_to(data, len, to.host, to.port);
    }
    std::optional<std::size_t> send_to(const std::string& s, const Endpoint& to) {
        return send_to(s.data(), s.size(), to.host, to.port);
    }

    // Recibe un datagrama.
    //  timeout_ms < 0 : bloquea indefinidamente
    //  timeout_ms = 0 : no espera (poll)
    //  timeout_ms > 0 : espera hasta ese tiempo
    // Devuelve nullopt si vencio el timeout / no hay datos. Lanza en errores reales.
    // Si el datagrama es mayor que el buffer, se trunca.
    std::optional<std::size_t> recv_from(void* buf, std::size_t len, Endpoint& from,
                                         int timeout_ms = -1) {
        if (timeout_ms >= 0 && !wait_readable(timeout_ms)) return std::nullopt;

        sockaddr_in a{};
        socklen_t alen = sizeof a;
#ifdef _WIN32
        int n = ::recvfrom(fd_, static_cast<char*>(buf), static_cast<int>(len), 0,
                           reinterpret_cast<sockaddr*>(&a), &alen);
#else
        ssize_t n = ::recvfrom(fd_, buf, len, 0, reinterpret_cast<sockaddr*>(&a), &alen);
#endif
        if (n < 0) {
            int e = detail::last_error();
            if (detail::is_timeout(e)) return std::nullopt;
            if (detail::is_ignorable(e)) return std::nullopt;
            detail::fail("recvfrom");
        }
        from = detail::to_endpoint(a);
        return static_cast<std::size_t>(n);
    }

    // Variante que devuelve el paquete como string.
    std::optional<std::string> recv_string(Endpoint& from, int timeout_ms = -1,
                                           std::size_t max_size = 65507) {
        std::string s(max_size, '\0');
        auto n = recv_from(&s[0], s.size(), from, timeout_ms);
        if (!n) return std::nullopt;
        s.resize(*n);
        return s;
    }

    // Espera hasta que haya datos para leer. true si hay datos.
    bool wait_readable(int timeout_ms) {
#ifdef _WIN32
        WSAPOLLFD p{};
        p.fd = fd_;
        p.events = POLLRDNORM;
        int r = ::WSAPoll(&p, 1, timeout_ms);
#else
        pollfd p{};
        p.fd = fd_;
        p.events = POLLIN;
        int r;
        do { r = ::poll(&p, 1, timeout_ms); } while (r < 0 && errno == EINTR);
#endif
        if (r < 0) detail::fail("poll");
        return r > 0;
    }

private:
    std::optional<std::size_t> send_to(const void* data, std::size_t len, const sockaddr_in& a) {
#ifdef _WIN32
        int n = ::sendto(fd_, static_cast<const char*>(data), static_cast<int>(len), 0,
                         reinterpret_cast<const sockaddr*>(&a), sizeof a);
#else
        ssize_t n = ::sendto(fd_, data, len, 0, reinterpret_cast<const sockaddr*>(&a), sizeof a);
#endif
        if (n < 0) {
            if (detail::is_would_block(detail::last_error())) return std::nullopt;
            detail::fail("sendto");
        }
        return static_cast<std::size_t>(n);
    }

    detail::native_socket fd_ = detail::invalid_socket;
};

} // namespace udp
