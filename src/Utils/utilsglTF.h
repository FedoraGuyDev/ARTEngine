#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <cstring>

#include "glad/glad.h"
#include "TinyglTF/tiny_gltf_v3.h"
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

inline void ModelAddNode(const tg3_model& model, int nodeIndex, const glm::mat4& parent,
                         std::vector<float>& data, std::vector<unsigned int>& indices, int depth = 0){
    if(nodeIndex < 0 || nodeIndex >= (int)model.nodes_count || depth > 64) return;
    const tg3_node& node = model.nodes[nodeIndex];

    glm::mat4 local(1.0f);
    if(node.has_matrix){
        for(int i = 0; i < 16; i++) local[i / 4][i % 4] = (float)node.matrix[i];
    } else {
        local = glm::translate(glm::mat4(1.0f), glm::vec3(node.translation[0], node.translation[1], node.translation[2]))
              * glm::mat4_cast(glm::quat((float)node.rotation[3], (float)node.rotation[0], (float)node.rotation[1], (float)node.rotation[2]))
              * glm::scale(glm::mat4(1.0f), glm::vec3(node.scale[0], node.scale[1], node.scale[2]));
    }
    glm::mat4 world = parent * local;
    glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(world)));

    if(node.mesh >= 0 && node.mesh < (int)model.meshes_count){
        const tg3_mesh& mesh = model.meshes[node.mesh];
        for(uint32_t pi = 0; pi < mesh.primitives_count; pi++){
            const tg3_primitive& prim = mesh.primitives[pi];
            if(prim.mode != -1 && prim.mode != TG3_MODE_TRIANGLES) continue;

            int posAcc = -1, norAcc = -1;
            for(uint32_t a = 0; a < prim.attributes_count; a++){
                if(tg3_str_equals_cstr(prim.attributes[a].key, "POSITION")) posAcc = prim.attributes[a].value;
                if(tg3_str_equals_cstr(prim.attributes[a].key, "NORMAL"))   norAcc = prim.attributes[a].value;
            }
            std::vector<float> pos, nor;
            std::vector<unsigned int> idx;
            if(!ModelReadFloats(model, posAcc, 3, pos)) continue;
            if(!ModelReadFloats(model, norAcc, 3, nor)) nor.assign(pos.size(), 0.0f);
            if(prim.indices >= 0){
                if(!ModelReadIndices(model, prim.indices, idx)) continue;
            } else {
                idx.resize(pos.size() / 3);
                for(size_t i = 0; i < idx.size(); i++) idx[i] = (unsigned int)i;
            }

            const unsigned int base = (unsigned int)(data.size() / 6);
            for(size_t i = 0; i < pos.size() / 3; i++){
                glm::vec3 p = glm::vec3(world * glm::vec4(pos[i*3], pos[i*3+1], pos[i*3+2], 1.0f));
                glm::vec3 n = normalMat * glm::vec3(nor[i*3], nor[i*3+1], nor[i*3+2]);
                if(glm::length(n) > 0.0f) n = glm::normalize(n);
                data.insert(data.end(), { p.x, p.y, p.z, n.x, n.y, n.z });
            }
            for(unsigned int i : idx) indices.push_back(base + i);
        }
    }

    for(uint32_t c = 0; c < node.children_count; c++)
        ModelAddNode(model, node.children[c], world, data, indices, depth + 1);
}
