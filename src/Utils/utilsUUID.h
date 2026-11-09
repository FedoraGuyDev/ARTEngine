#pragma once

#include <algorithm>
#include <array>
#include <functional>
#include <random>
#include "stdUUID/uuid.h"

inline std::string GenerateUUID(){
    std::random_device rd;
    std::array<int, std::mt19937::state_size> seedData{};
    std::generate(seedData.begin(), seedData.end(), std::ref(rd));
    std::seed_seq seq(seedData.begin(), seedData.end());
    std::mt19937 engine(seq);

    uuids::uuid_random_generator gen(engine);
    return uuids::to_string(gen());
}
