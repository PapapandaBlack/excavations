#include <array>
#include <random>

#include "excavations/game/UnderwaterExcavation.hpp"
#include "excavations/game/ExcavationUtils.hpp"

namespace
{
    const std::array<LootEntry, 3> UnderwaterLoot{
    LootEntry{"Seashell", 30, 45, LootType::Seashell},
    LootEntry{"Pearl", 75, 35, LootType::Pearl},
    LootEntry{"Sunken idol", 160, 20, LootType::SunkenIdol}
    };
}

std::string UnderwaterExcavation::getName() const
{
    return "Underwater";
}

std::string UnderwaterExcavation::getDescription() const
{
    return "Explore the seabed.\nUse a pickaxe on rocks.";
}

void UnderwaterExcavation::generateField(
    std::mt19937& randomEngine
)
{
    for (auto& row : field_)
    {
        for (auto& cell : row)
        {
            cell.terrain = TerrainType::Sand;
            cell.excavated = false;
        }
    }

    std::uniform_int_distribution<std::size_t> rowDistribution(
        0,
        FieldRows - 2
    );

    std::uniform_int_distribution<std::size_t> columnDistribution(
        0,
        FieldColumns - 2
    );

    for (int group = 0; group < 2; ++group)
    {
        const auto startRow = rowDistribution(randomEngine);
        const auto startColumn = columnDistribution(randomEngine);

        for (std::size_t dy = 0; dy < 2; ++dy)
        {
            for (std::size_t dx = 0; dx < 2; ++dx)
            {
                field_[startRow + dy][startColumn + dx].terrain =
                    TerrainType::Rock;
            }
        }
    }
}

ExcavationResult UnderwaterExcavation::excavate(
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
            "This cell is outside the field.",
            "",
            0
        };
    }

    auto& cell = field_[row][column];

    if (cell.excavated)
    {
        return {
            ExcavationOutcome::InvalidAction,
            "This cell has already been excavated.",
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

        findChance = 65;
        damageChance = 20;
    }
    else if (cell.terrain == TerrainType::Sand)
    {
        switch (tool)
        {
        case ToolType::Pickaxe:
            findChance = 40;
            damageChance = 35;
            break;

        case ToolType::Shovel:
            findChance = 70;
            damageChance = 10;
            break;

        case ToolType::Brush:
            findChance = 60;
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
        UnderwaterLoot,
        randomEngine
    );
}