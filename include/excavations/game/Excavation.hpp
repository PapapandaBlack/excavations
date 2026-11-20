#pragma once

#include <array>
#include <cstddef>
#include <random>
#include <string>

#include "excavations/game/Cell.hpp"
#include "excavations/game/ToolType.hpp"
#include "excavations/game/LootType.hpp"

enum class ExcavationOutcome
{
    InvalidAction,
    Nothing,
    Found,
    Damaged
};

struct ExcavationResult
{
    ExcavationOutcome outcome = ExcavationOutcome::InvalidAction;

    std::string message;

    std::string lootName;
    int lootValue = 0;

    LootType lootType = LootType::None;
};

struct LootEntry
{
    std::string name;
    int value;
    int weight;

    LootType type;
};

class Excavation
{
public:
    static constexpr std::size_t FieldRows = 5;
    static constexpr std::size_t FieldColumns = 5;

    using Field = std::array<
        std::array<Cell, FieldColumns>,
        FieldRows
    >;

    Excavation() = default;
    virtual ~Excavation() = default;

    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;

    virtual void generateField(
        std::mt19937& randomEngine
    ) = 0;

    virtual ExcavationResult excavate(
        std::size_t row,
        std::size_t column,
        ToolType tool,
        std::mt19937& randomEngine
    ) = 0;

    const Field& getField() const
    {
        return field_;
    }

protected:
    Field field_{};
};