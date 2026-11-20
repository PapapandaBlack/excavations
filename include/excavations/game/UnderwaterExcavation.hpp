#pragma once

#include "excavations/game/Excavation.hpp"

class UnderwaterExcavation : public Excavation
{
public:
    UnderwaterExcavation() = default;
    ~UnderwaterExcavation() override = default;

    std::string getName() const override;
    std::string getDescription() const override;

    void generateField(
        std::mt19937& randomEngine
    ) override;

    ExcavationResult excavate(
        std::size_t row,
        std::size_t column,
        ToolType tool,
        std::mt19937& randomEngine
    ) override;
};