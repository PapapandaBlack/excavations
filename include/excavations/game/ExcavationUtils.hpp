#pragma once

#include <array>
#include <random>

#include "excavations/game/Excavation.hpp"

namespace ExcavationUtils
{
    template<std::size_t N>
    ExcavationResult rollLoot(
        int findChance,
        int damageChance,
        const std::array<LootEntry, N>& lootTable,
        std::mt19937& randomEngine
    )
    {
        static_assert(N > 0, "Loot table must\nnot be empty.");

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

        std::array<int, N> weights{};

        for (std::size_t i = 0; i < N; ++i)
        {
            weights[i] = lootTable[i].weight;
        }

        std::discrete_distribution<std::size_t> lootDistribution(
            weights.begin(),
            weights.end()
        );

        const auto& loot =
            lootTable[lootDistribution(randomEngine)];

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
}