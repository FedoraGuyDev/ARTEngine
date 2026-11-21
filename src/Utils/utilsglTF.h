#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <cstring>
#include <map>
#include <algorithm>

#include "glad/glad.h"
#include "TinyglTF/tiny_gltf_v3.h"
#include "TinyglTF/stb_image.h"
#include "JsonNlohmann/json.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "engineDefinitionsAssets.h"

inline bool ModelReadFloats(const tg3_model& m, int ai, int comps, std::vector<float>& out){
    if(ai < 0 || ai >= (int)m.accessors_count) return false;
    const tg3_accessor& acc = m.accessors[ai];
    if(acc.component_type != TG3_COMPONENT_TYPE_FLOAT || acc.buffer_view < 0) return false;
    if(tg3_num_components(acc.type) != comps) return false;

    const tg3_buffer_view& bv = m.buffer_views[acc.buffer_view];
    const tg3_buffer& buf = m.buffers[bv.buffer];
    const int stride = tg3_accessor_byte_stride(&acc, &bv);
    const uint64_t start = bv.byte_offset + acc.byte_offset;
    if(stride <= 0 || acc.count == 0 || start + (acc.count - 1) * stride + comps * 4 > buf.data.count) return false;

    out.resize(acc.count * comps);
    for(uint64_t i = 0; i < acc.count; i++)
        std::memcpy(&out[i * comps], buf.data.data + start + i * stride, comps * 4);
    return true;
}

inline bool ModelReadIndices(const tg3_model& m, int ai, std::vector<unsigned int>& out){
    if(ai < 0 || ai >= (int)m.accessors_count) return false;
    const tg3_accessor& acc = m.accessors[ai];
    if(acc.buffer_view < 0) return false;

    const tg3_buffer_view& bv = m.buffer_views[acc.buffer_view];
    const tg3_buffer& buf = m.buffers[bv.buffer];
    const int stride = tg3_accessor_byte_stride(&acc, &bv);
    const uint64_t start = bv.byte_offset + acc.byte_offset;
    if(stride <= 0 || acc.count == 0 || start + (acc.count - 1) * stride + tg3_component_size(acc.component_type) > buf.data.count) return false;

    out.resize(acc.count);
    for(uint64_t i = 0; i < acc.count; i++){
        const uint8_t* p = buf.data.data + start + i * stride;
        switch(acc.component_type){
            case TG3_COMPONENT_TYPE_UNSIGNED_BYTE:  out[i] = *p; break;
            case TG3_COMPONENT_TYPE_UNSIGNED_SHORT: { uint16_t v; std::memcpy(&v, p, 2); out[i] = v; break; }
            case TG3_COMPONENT_TYPE_UNSIGNED_INT:   { uint32_t v; std::memcpy(&v, p, 4); out[i] = v; break; }
            default: return false;
        }
    }
    return true;
}

inline std::vector<unsigned char> ModelBase64(const char* s, size_t n){
    std::vector<unsigned char> out;
    unsigned int acc = 0; int bits = 0;
    for(size_t i = 0; i < n; i++){
        char c = s[i]; int v;
        if(c >= 'A' && c <= 'Z') v = c - 'A';
        else if(c >= 'a' && c <= 'z') v = c - 'a' + 26;
        else if(c >= '0' && c <= '9') v = c - '0' + 52;
        else if(c == '+' || c == '-') v = 62;
        else if(c == '/' || c == '_') v = 63;
        else continue;
        acc = (acc << 6) | v; bits += 6;
        if(bits >= 8){ bits -= 8; out.push_back((unsigned char)((acc >> bits) & 0xFF)); }
    }
    return out;
}

inline std::string ModelUriDecode(const std::string& uri){
    std::string r;
    for(size_t i = 0; i < uri.size(); i++){
        if(uri[i] == '%' && i + 2 < uri.size()){
            r += (char)std::stoi(uri.substr(i + 1, 2), nullptr, 16);
            i += 2;
        } else r += uri[i];
    }
    return r;
}

inline bool ModelImageBytes(const tg3_model& m, const tg3_image& img, const std::string& baseDir, std::vector<unsigned char>& out){
    if(img.buffer_view >= 0 && img.buffer_view < (int)m.buffer_views_count){
        const tg3_buffer_view& bv = m.buffer_views[img.buffer_view];
        const tg3_buffer& buf = m.buffers[bv.buffer];
        if(bv.byte_offset + bv.byte_length > buf.data.count) return false;
        out.assign(buf.data.data + bv.byte_offset, buf.data.data + bv.byte_offset + bv.byte_length);
        return true;
    }
    if(img.uri.len == 0) return false;
    std::string uri(img.uri.data, img.uri.len);

    if(uri.rfind("data:", 0) == 0){
        size_t comma = uri.find(',');
        if(comma == std::string::npos) return false;
        out = ModelBase64(uri.c_str() + comma + 1, uri.size() - comma - 1);
        return !out.empty();
    }

    std::string path = baseDir + ModelUriDecode(uri);
    FILE* f = std::fopen(path.c_str(), "rb");
    if(!f) return false;
    std::fseek(f, 0, SEEK_END);
    long size = std::ftell(f);
    std::fseek(f, 0, SEEK_SET);
    out.resize(size > 0 ? (size_t)size : 0);
    size_t read = out.empty() ? 0 : std::fread(out.data(), 1, out.size(), f);
    std::fclose(f);
    return read == out.size() && !out.empty();
}

inline unsigned int ModelLoadTexture(const tg3_model& m, int ti, const std::string& baseDir, std::vector<unsigned int>& cache){
    if(ti < 0 || ti >= (int)m.textures_count) return 0;
    if(cache[ti]) return cache[ti] == ~0u ? 0 : cache[ti];
    cache[ti] = ~0u;

    const tg3_texture& tex = m.textures[ti];
    if(tex.source < 0 || tex.source >= (int)m.images_count) return 0;

    std::vector<unsigned char> bytes;
    if(!ModelImageBytes(m, m.images[tex.source], baseDir, bytes)){
        std::cout << "[Model] no se pudo leer la imagen de la textura " << ti << std::endl;
        return 0;
    }

    int w, h, c;
    unsigned char* px = stbi_load_from_memory(bytes.data(), (int)bytes.size(), &w, &h, &c, 4);
    if(!px){
        std::cout << "[Model] stb_image: " << stbi_failure_reason() << std::endl;
        return 0;
    }

    GLint wrapS = GL_REPEAT, wrapT = GL_REPEAT;
    bool nearest = false;
    if(tex.sampler >= 0 && tex.sampler < (int)m.samplers_count){
        const tg3_sampler& s = m.samplers[tex.sampler];
        wrapS = s.wrap_s; wrapT = s.wrap_t;
        nearest = (s.mag_filter == TG3_TEXTURE_FILTER_NEAREST);
    }

    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, px);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, nearest ? GL_NEAREST_MIPMAP_LINEAR : GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, nearest ? GL_NEAREST : GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(px);

    cache[ti] = id;
    return id;
}

struct ModelPartData{
    std::vector<float> data;
    std::vector<unsigned int> indices;
};
static const int MODEL_VERTEX_FLOATS = 16;

inline std::string ModelStr(tg3_str s){ return s.data ? std::string(s.data, s.len) : std::string(); }

inline glm::mat4 ModelNodeLocal(const tg3_node& node){
    if(node.has_matrix){
        glm::mat4 m;
        for(int i = 0; i < 16; i++) m[i / 4][i % 4] = (float)node.matrix[i];
        return m;
    }
    return glm::translate(glm::mat4(1.0f), glm::vec3(node.translation[0], node.translation[1], node.translation[2]))
         * glm::mat4_cast(glm::quat((float)node.rotation[3], (float)node.rotation[0], (float)node.rotation[1], (float)node.rotation[2]))
         * glm::scale(glm::mat4(1.0f), glm::vec3(node.scale[0], node.scale[1], node.scale[2]));
}

// JOINTS_0 (bytes o shorts sin signo) y WEIGHTS_0 (float, o bytes/shorts normalizados) -> 4 floats por vertice
inline bool ModelReadVec4(const tg3_model& m, int ai, std::vector<float>& out){
    if(ai < 0 || ai >= (int)m.accessors_count) return false;
    const tg3_accessor& acc = m.accessors[ai];
    if(acc.buffer_view < 0 || tg3_num_components(acc.type) != 4) return false;
    const int ct = acc.component_type;
    if(ct != TG3_COMPONENT_TYPE_FLOAT && ct != TG3_COMPONENT_TYPE_UNSIGNED_BYTE && ct != TG3_COMPONENT_TYPE_UNSIGNED_SHORT) return false;

    const tg3_buffer_view& bv = m.buffer_views[acc.buffer_view];
    const tg3_buffer& buf = m.buffers[bv.buffer];
    const int stride = tg3_accessor_byte_stride(&acc, &bv);
    const uint64_t start = bv.byte_offset + acc.byte_offset;
    if(stride <= 0 || acc.count == 0 || start + (acc.count - 1) * stride + 4 * tg3_component_size(ct) > buf.data.count) return false;

    out.resize(acc.count * 4);
    for(uint64_t i = 0; i < acc.count; i++){
        const uint8_t* p = buf.data.data + start + i * stride;
        for(int k = 0; k < 4; k++){
            float v;
            if(ct == TG3_COMPONENT_TYPE_FLOAT){ std::memcpy(&v, p + k * 4, 4); }
            else if(ct == TG3_COMPONENT_TYPE_UNSIGNED_BYTE){ v = p[k]; if(acc.normalized) v /= 255.0f; }
            else { uint16_t u; std::memcpy(&u, p + k * 2, 2); v = u; if(acc.normalized) v /= 65535.0f; }
            out[i * 4 + k] = v;
        }
    }
    return true;
}

// Recorre el nodo y sus hijos. Cada primitive se agrega al grupo (material, con huesos?).
// Las mallas con skin NO se transforman aca: las mueven los huesos en el shader.
inline void ModelAddNode(const tg3_model& model, int nodeIndex, const glm::mat4& parent,
                         std::map<std::pair<int,int>, ModelPartData>& parts, int depth = 0){
    if(nodeIndex < 0 || nodeIndex >= (int)model.nodes_count || depth > 64) return;
    const tg3_node& node = model.nodes[nodeIndex];

    glm::mat4 world = parent * ModelNodeLocal(node);

    const bool skinned = node.skin == 0 && model.skins_count > 0 && model.skins[0].joints_count > 0;
    const unsigned int boneCount = skinned ? model.skins[0].joints_count : 0;

    if(node.mesh >= 0 && node.mesh < (int)model.meshes_count){
        const tg3_mesh& mesh = model.meshes[node.mesh];
        for(uint32_t pi = 0; pi < mesh.primitives_count; pi++){
            const tg3_primitive& prim = mesh.primitives[pi];
            if(prim.mode != -1 && prim.mode != TG3_MODE_TRIANGLES) continue;

            int posAcc = -1, norAcc = -1, uvAcc = -1, jointAcc = -1, weightAcc = -1;
            for(uint32_t a = 0; a < prim.attributes_count; a++){
                if(tg3_str_equals_cstr(prim.attributes[a].key, "POSITION"))   posAcc = prim.attributes[a].value;
                if(tg3_str_equals_cstr(prim.attributes[a].key, "NORMAL"))     norAcc = prim.attributes[a].value;
                if(tg3_str_equals_cstr(prim.attributes[a].key, "TEXCOORD_0")) uvAcc  = prim.attributes[a].value;
                if(tg3_str_equals_cstr(prim.attributes[a].key, "JOINTS_0"))   jointAcc  = prim.attributes[a].value;
                if(tg3_str_equals_cstr(prim.attributes[a].key, "WEIGHTS_0"))  weightAcc = prim.attributes[a].value;
            }
            std::vector<float> pos, nor, uv, joints, weights;
            std::vector<unsigned int> idx;
            if(!ModelReadFloats(model, posAcc, 3, pos)) continue;
            const size_t vcount = pos.size() / 3;
            if(!ModelReadFloats(model, norAcc, 3, nor) || nor.size() != pos.size()) nor.assign(pos.size(), 0.0f);
            if(!ModelReadFloats(model, uvAcc, 2, uv) || uv.size() != vcount * 2)    uv.assign(vcount * 2, 0.0f);
            if(prim.indices >= 0){
                if(!ModelReadIndices(model, prim.indices, idx)) continue;
            } else {
                idx.resize(vcount);
                for(size_t i = 0; i < idx.size(); i++) idx[i] = (unsigned int)i;
            }

            // Sin joints/weights validos, una malla con skin se dibuja como estatica
            bool primSkinned = skinned;
            if(primSkinned){
                if(!ModelReadVec4(model, jointAcc, joints) || joints.size() != vcount * 4 ||
                   !ModelReadVec4(model, weightAcc, weights) || weights.size() != vcount * 4) primSkinned = false;
            }
            const glm::mat4 vm  = primSkinned ? glm::mat4(1.0f) : world;
            const glm::mat3 nm  = primSkinned ? glm::mat3(1.0f) : glm::transpose(glm::inverse(glm::mat3(world)));

            ModelPartData& part = parts[std::make_pair(prim.material, primSkinned ? 1 : 0)];
            const unsigned int base = (unsigned int)(part.data.size() / MODEL_VERTEX_FLOATS);
            for(size_t i = 0; i < vcount; i++){
                glm::vec3 p = glm::vec3(vm * glm::vec4(pos[i*3], pos[i*3+1], pos[i*3+2], 1.0f));
                glm::vec3 n = nm * glm::vec3(nor[i*3], nor[i*3+1], nor[i*3+2]);
                if(glm::length(n) > 0.0f) n = glm::normalize(n);

                float j[4] = {0, 0, 0, 0}, w[4] = {0, 0, 0, 0};
                if(primSkinned){
                    float sum = 0.0f;
                    for(int k = 0; k < 4; k++){
                        j[k] = joints[i*4+k]; w[k] = weights[i*4+k];
                        if(j[k] >= (float)boneCount) w[k] = 0.0f;       // hueso inexistente
                        sum += w[k];
                    }
                    if(sum > 0.0f) for(int k = 0; k < 4; k++) w[k] /= sum;   // los pesos suman 1
                    else { w[0] = 1.0f; j[0] = 0.0f; }
                }
                part.data.insert(part.data.end(), { p.x, p.y, p.z, n.x, n.y, n.z, uv[i*2], uv[i*2+1],
                                                    j[0], j[1], j[2], j[3], w[0], w[1], w[2], w[3] });
            }
            for(unsigned int i : idx) part.indices.push_back(base + i);
        }
    }

    for(uint32_t c = 0; c < node.children_count; c++)
        ModelAddNode(model, node.children[c], world, parts, depth + 1);
}

inline bool ModelLoadSkeleton(const tg3_model& m, AssetSkeleton& sk, std::vector<int>& nodeToBone){
    if(m.skins_count == 0) return false;
    const tg3_skin& skin = m.skins[0];
    const int n = (int)skin.joints_count;
    if(n == 0) return false;

    std::vector<int> parentOf(m.nodes_count, -1);
    for(uint32_t i = 0; i < m.nodes_count; i++)
        for(uint32_t c = 0; c < m.nodes[i].children_count; c++)
            if(m.nodes[i].children[c] >= 0 && m.nodes[i].children[c] < (int)m.nodes_count)
                parentOf[m.nodes[i].children[c]] = (int)i;

    nodeToBone.assign(m.nodes_count, -1);
    for(int i = 0; i < n; i++){
        if(skin.joints[i] < 0 || skin.joints[i] >= (int)m.nodes_count) return false;
        nodeToBone[skin.joints[i]] = i;
    }

    std::vector<float> ibm;
    const bool hasIbm = ModelReadFloats(m, skin.inverse_bind_matrices, 16, ibm) && ibm.size() == (size_t)n * 16;

    sk.bones.assign(n, AssetBone());
    for(int i = 0; i < n; i++){
        const int nodeIndex = skin.joints[i];
        const tg3_node& nd = m.nodes[nodeIndex];
        AssetBone& b = sk.bones[i];

        b.name   = ModelStr(nd.name);
        b.parent = parentOf[nodeIndex] >= 0 ? nodeToBone[parentOf[nodeIndex]] : -1;

        if(nd.has_matrix){          // descomponer la matriz en T, R, S
            glm::mat4 lm = ModelNodeLocal(nd);
            b.translation = glm::vec3(lm[3]);
            b.scale = glm::vec3(glm::length(glm::vec3(lm[0])), glm::length(glm::vec3(lm[1])), glm::length(glm::vec3(lm[2])));
            glm::mat3 r(glm::vec3(lm[0]) / b.scale.x, glm::vec3(lm[1]) / b.scale.y, glm::vec3(lm[2]) / b.scale.z);
            b.rotation = glm::quat_cast(r);
        } else {
            b.translation = glm::vec3(nd.translation[0], nd.translation[1], nd.translation[2]);
            b.rotation    = glm::quat((float)nd.rotation[3], (float)nd.rotation[0], (float)nd.rotation[1], (float)nd.rotation[2]);
            b.scale       = glm::vec3(nd.scale[0], nd.scale[1], nd.scale[2]);
        }
        b.inverseBind = glm::mat4(1.0f);
        if(hasIbm) std::memcpy(&b.inverseBind[0][0], &ibm[i * 16], 16 * sizeof(float));
    }

    std::vector<bool> placed(n, false);
    sk.order.clear();
    while((int)sk.order.size() < n){
        bool progress = false;
        for(int i = 0; i < n; i++){
            if(placed[i]) continue;
            if(sk.bones[i].parent < 0 || placed[sk.bones[i].parent]){
                placed[i] = true; sk.order.push_back(i); progress = true;
            }
        }
        if(!progress) return false;     // ciclo: glTF invalido
    }

    sk.rootMatrix = glm::mat4(1.0f);
    for(int i : sk.order){
        if(sk.bones[i].parent >= 0) continue;
        std::vector<int> chain;
        for(int p = parentOf[skin.joints[i]]; p >= 0; p = parentOf[p]) chain.push_back(p);
        for(auto it = chain.rbegin(); it != chain.rend(); ++it) sk.rootMatrix = sk.rootMatrix * ModelNodeLocal(m.nodes[*it]);
        break;
    }
    return true;
}

inline void ModelLoadAnimations(const tg3_model& m, const std::vector<int>& nodeToBone, std::vector<AssetAnimation>& out){
    for(uint32_t ai = 0; ai < m.animations_count; ai++){
        const tg3_animation& an = m.animations[ai];
        AssetAnimation anim;
        anim.name = ModelStr(an.name);
        if(anim.name.empty()) anim.name = "animation" + std::to_string(ai);

        for(uint32_t ci = 0; ci < an.channels_count; ci++){
            const tg3_animation_channel& ch = an.channels[ci];
            if(ch.target.node < 0 || ch.target.node >= (int)nodeToBone.size()) continue;
            const int bone = nodeToBone[ch.target.node];
            if(bone < 0 || ch.sampler < 0 || ch.sampler >= (int)an.samplers_count) continue;

            int path = -1;
            if(tg3_str_equals_cstr(ch.target.path, "translation")) path = 0;
            else if(tg3_str_equals_cstr(ch.target.path, "rotation")) path = 1;
            else if(tg3_str_equals_cstr(ch.target.path, "scale")) path = 2;
            if(path < 0) continue;                       // "weights" (morph targets) no soportado

            const tg3_animation_sampler& sa = an.samplers[ch.sampler];
            int interp = 0;                              // 0 = LINEAR, 1 = STEP
            const bool cubic = tg3_str_equals_cstr(sa.interpolation, "CUBICSPLINE");
            if(tg3_str_equals_cstr(sa.interpolation, "STEP")) interp = 1;

            AnimChannel c;
            c.bone = bone; c.path = path; c.interp = interp;
            const int comps = (path == 1) ? 4 : 3;
            std::vector<float> raw;
            if(!ModelReadFloats(m, sa.input, 1, c.times)) continue;
            if(!ModelReadFloats(m, sa.output, comps, raw)) continue;

            if(cubic){
                if(raw.size() != c.times.size() * comps * 3) continue;
                c.values.resize(c.times.size() * comps);
                for(size_t k = 0; k < c.times.size(); k++)
                    for(int j = 0; j < comps; j++) c.values[k * comps + j] = raw[(k * 3 + 1) * comps + j];
            } else {
                if(raw.size() != c.times.size() * comps) continue;
                c.values = std::move(raw);
            }
            if(c.times.empty()) continue;

            anim.duration = std::max(anim.duration, c.times.back());
            anim.channels.push_back(std::move(c));
        }
        out.push_back(std::move(anim));
    }
}
