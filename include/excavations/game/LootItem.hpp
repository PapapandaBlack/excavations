#pragma once

#include <string>

#include "excavations/game/LootType.hpp"

struct LootItem
{
    std::string name;
    int value = 0;

    LootType type = LootType::None;
};