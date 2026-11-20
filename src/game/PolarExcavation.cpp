#include <array>
#include <random>

#include "excavations/game/PolarExcavation.hpp"
#include "excavations/game/ExcavationUtils.hpp"

namespace
{
    const std::array<LootEntry, 3> PolarLoot{
    LootEntry{"Fossil", 40, 30, LootType::Fossil},
    LootEntry{"Ancient amulet", 90, 45, LootType::AncientAmulet},
    LootEntry{"Frozen meteorite", 180, 25, LootType::FrozenMeteorite}
    };
}

std::string PolarExcavation::getName() const
{
    return "Polar";
}

std::string PolarExcavation::getDescription() const
{
    return "Explore the polar ice.\nUse a pickaxe on ice.";
}

void PolarExcavation::generateField(
    std::mt19937& randomEngine
)
{
    for (auto& row : field_)
    {
        for (auto& cell : row)
        {
            cell.terrain = TerrainType::Snow;
            cell.excavated = false;
        }
    }

    std::uniform_int_distribution<std::size_t> rowDistribution(
        0,
        FieldRows - 1
    );

    const auto firstIceRow = rowDistribution(randomEngine);

    std::uniform_int_distribution<std::size_t> otherRowDistribution(
        0,
        FieldRows - 2
    );

    auto secondIceRow = otherRowDistribution(randomEngine);

    if (secondIceRow >= firstIceRow)
    {
        ++secondIceRow;
    }

    for (std::size_t column = 0; column < FieldColumns; ++column)
    {
        field_[firstIceRow][column].terrain = TerrainType::Ice;
        field_[secondIceRow][column].terrain = TerrainType::Ice;
    }
}

ExcavationResult PolarExcavation::excavate(
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

    if (cell.terrain == TerrainType::Ice)
    {
        if (tool != ToolType::Pickaxe)
        {
            return {
                ExcavationOutcome::InvalidAction,
                "Use a pickaxe\nto break the ice.",
                "",
                0
            };
        }

        findChance = 75;
        damageChance = 15;
    }
    else if (cell.terrain == TerrainType::Snow)
    {
        switch (tool)
        {
        case ToolType::Pickaxe:
            findChance = 45;
            damageChance = 35;
            break;

        case ToolType::Shovel:
            findChance = 70;
            damageChance = 10;
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
    }
    else
    {
        return {
            ExcavationOutcome::InvalidAction,
            "Unsupported terrain.",
            "",
            0
        };
    }

    cell.excavated = true;

    return ExcavationUtils::rollLoot(
        findChance,
        damageChance,
        PolarLoot,
        randomEngine
    );
}