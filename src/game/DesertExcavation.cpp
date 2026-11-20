#include <array>
#include <random>

#include "excavations/game/DesertExcavation.hpp"

namespace
{
    const std::array<LootEntry, 3> DesertLoot{
    LootEntry{"Ceramic vase", 20, 60, LootType::CeramicVase},
    LootEntry{"Silver bar", 50, 30, LootType::SilverBar},
    LootEntry{"Gold bar", 120, 10, LootType::GoldBar}
    };
}

std::string DesertExcavation::getName() const
{
    return "Desert";
}

std::string DesertExcavation::getDescription() const
{
    return "Explore the ancient sands.";
}

void DesertExcavation::generateField(
    std::mt19937& randomEngine
)
{
    (void)randomEngine;

    for (auto& row : field_)
    {
        for (auto& cell : row)
        {
            cell.terrain = TerrainType::Sand;
            cell.excavated = false;
        }
    }
}

ExcavationResult DesertExcavation::excavate(
    std::size_t row,
    std::size_t column,
    ToolType tool,
    std::mt19937& randomEngine
)
{
    if (row >= FieldRows || column >= FieldColumns)
    {
        return {
            ExcavationOutcome::InvalidAction,
            "This cell is\noutside the field.",
            "",
            0
        };
    }

    auto& cell = field_[row][column];

    if (cell.excavated)
    {
        return {
            ExcavationOutcome::InvalidAction,
            "This cell has\nalready been excavated.",
            "",
            0
        };
    }

    int findChance = 0;
    int damageChance = 0;

    switch (tool)
    {
    case ToolType::Pickaxe:
        findChance = 45;
        damageChance = 35;
        break;

    case ToolType::Shovel:
        findChance = 65;
        damageChance = 15;
        break;

    case ToolType::Brush:
        findChance = 55;
        damageChance = 5;
        break;

    default:
        return {
            ExcavationOutcome::InvalidAction,
            "Unknown tool.",
            "",
            0
        };
    }

    cell.excavated = true;

    std::uniform_int_distribution<int> outcomeDistribution(1, 100);
    const int roll = outcomeDistribution(randomEngine);

    if (roll > findChance + damageChance)
    {
        return {
            ExcavationOutcome::Nothing,
            "Nothing was found.",
            "",
            0
        };
    }

    std::array<int, DesertLoot.size()> weights{};

    for (std::size_t i = 0; i < DesertLoot.size(); ++i)
    {
        weights[i] = DesertLoot[i].weight;
    }

    std::discrete_distribution<std::size_t> lootDistribution(
        weights.begin(),
        weights.end()
    );

    const auto& loot =
        DesertLoot[lootDistribution(randomEngine)];

    if (roll <= findChance)
    {
        return {
            ExcavationOutcome::Found,
            std::string{"Found: "} + loot.name,
            loot.name,
            loot.value,
            loot.type
        };
    }

    return {
        ExcavationOutcome::Damaged,
        std::string{"Destroyed: "} + loot.name,
        loot.name,
        0,
        loot.type
    };
}