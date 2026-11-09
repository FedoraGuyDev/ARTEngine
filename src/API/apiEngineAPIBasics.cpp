#include "apiEngineAPIBasics.h"

#include <string>
#include <iostream>
#include "JsonNlohmann/json.hpp"
#include "utilsUDP.hpp"

void APILogEngine(std::string message){
    std::cout << message << std::endl;
}
extern udp::Socket loggerSock;
extern std::string loggerUUID;

void APILogLogger(std::string message,int logLevel){
    nlohmann::json j = {
        {"level", logLevel},
        {"text", message},
        {"uuid", loggerUUID},
        {"timestamp", ""}
    };

    std::string msg = j.dump();
    loggerSock.send_to(msg.data(),msg.size(),"127.0.0.1",7777);

};
