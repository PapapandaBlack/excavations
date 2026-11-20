#include <array>
#include <random>

#include "excavations/game/SpaceExcavation.hpp"
#include "excavations/game/ExcavationUtils.hpp"

namespace
{
    const std::array<LootEntry, 3> SpaceLoot{
    LootEntry{"Mineral sample", 50, 20, LootType::MineralSample},
    LootEntry{"Alien mechanism", 120, 45, LootType::AlienMechanism},
    LootEntry{"Energy crystal", 240, 35, LootType::EnergyCrystal}
    };
}

std::string SpaceExcavation::getName() const
{
    return "Space";
}

std::string SpaceExcavation::getDescription() const
{
    return "Explore alien soil.\nUse a pickaxe on rocks.";
}

void SpaceExcavation::generateField(
    std::mt19937& randomEngine
)
{
    for (auto& row : field_)
    {
        for (auto& cell : row)
        {
            cell.terrain = TerrainType::SpaceSoil;
            cell.excavated = false;
        }
    }

    std::uniform_int_distribution<std::size_t> rowDistribution(
        1,
        FieldRows - 2
    );

    std::uniform_int_distribution<std::size_t> columnDistribution(
        1,
        FieldColumns - 2
    );

    const auto centerRow = rowDistribution(randomEngine);
    const auto centerColumn = columnDistribution(randomEngine);

    for (std::size_t row = centerRow - 1; row <= centerRow + 1; ++row)
    {
        for (
            std::size_t column = centerColumn - 1;
            column <= centerColumn + 1;
            ++column
            )
        {
            if (row != centerRow || column != centerColumn)
            {
                field_[row][column].terrain = TerrainType::Rock;
            }
        }
    }
}

ExcavationResult SpaceExcavation::excavate(
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

    if (cell.terrain == TerrainType::Rock)
    {
        if (tool != ToolType::Pickaxe)
        {
            return {
                ExcavationOutcome::InvalidAction,
                "Use a pickaxe\non this rock.",
                "",
                0
            };
        }

        findChance = 80;
        damageChance = 15;
    }
    else if (cell.terrain == TerrainType::SpaceSoil)
    {
        switch (tool)
        {
        case ToolType::Pickaxe:
            findChance = 55;
            damageChance = 30;
            break;

        case ToolType::Shovel:
            findChance = 75;
            damageChance = 10;
            break;

        case ToolType::Brush:
            findChance = 65;
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
        SpaceLoot,
        randomEngine
    );
}