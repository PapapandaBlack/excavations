#pragma once

#include <vector>
#include <string>
#include <array>

#include "excavations/game/ProductType.hpp"
#include "excavations/game/LootItem.hpp"
#include "excavations/game/ToolType.hpp"

class Player
{
public:
    int getMoney() const;
    int getFood() const;

    int getPickaxeCount() const;
    int getShovelCount() const;
    int getBrushCount() const;

    bool addMoney(int amount);
    bool spendMoney(int amount);

    bool addFood(int amount);
    bool consumeFood(int amount);

    bool buyProduct(ProductType type, int price);
    bool consumeTool(ToolType tool);

    void addLoot(const LootItem& item);
    const std::vector<LootItem>& getLoot() const;
    bool sellAllLoot();

    bool isLootDiscovered(LootType type) const;

    bool saveToFile(const std::string& filename) const;
    bool loadFromFile(const std::string& filename);

private:
    static constexpr int StartingMoney = 300;

    int money_ = StartingMoney;
    int food_ = 0;

    int pickaxeCount_ = 0;
    int shovelCount_ = 0;
    int brushCount_ = 0;

    std::vector<LootItem> loot_;

    static constexpr std::size_t MuseumItemCount = 12;

    std::array<bool, MuseumItemCount> discoveredLoot_{};
};