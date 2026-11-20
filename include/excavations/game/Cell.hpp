#pragma once

enum class TerrainType
{
    Sand,
    Rock,
    Snow,
    Ice,
    SpaceSoil
};

struct Cell
{
    TerrainType terrain = TerrainType::Sand;
    bool excavated = false;
};