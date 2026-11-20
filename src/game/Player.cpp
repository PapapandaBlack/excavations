#include <limits>
#include <fstream>
#include <iomanip>
#include <utility>

#include "excavations/game/Player.hpp"

int Player::getMoney() const
{
    return money_;
}

int Player::getFood() const
{
    return food_;
}

int Player::getPickaxeCount() const
{
    return pickaxeCount_;
}

int Player::getShovelCount() const
{
    return shovelCount_;
}

int Player::getBrushCount() const
{
    return brushCount_;
}

bool Player::addMoney(int amount)
{
    if (amount <= 0)
    {
        return false;
    }

    if (amount > std::numeric_limits<int>::max() - money_)
    {
        return false;
    }

    money_ += amount;
    return true;
}

bool Player::spendMoney(int amount)
{
    if (amount <= 0 || amount > money_)
    {
        return false;
    }

    money_ -= amount;
    return true;
}

bool Player::addFood(int amount)
{
    if (amount <= 0)
    {
        return false;
    }

    if (amount > std::numeric_limits<int>::max() - food_)
    {
        return false;
    }

    food_ += amount;
    return true;
}

bool Player::consumeFood(int amount)
{
    if (amount <= 0 || amount > food_)
    {
        return false;
    }

    food_ -= amount;
    return true;
}

bool Player::buyProduct(ProductType type, int price)
{
    if (price <= 0 || money_ < price)
    {
        return false;
    }

    int* quantity = nullptr;

    switch (type)
    {
    case ProductType::Food:
        quantity = &food_;
        break;

    case ProductType::Pickaxe:
        quantity = &pickaxeCount_;
        break;

    case ProductType::Shovel:
        quantity = &shovelCount_;
        break;

    case ProductType::Brush:
        quantity = &brushCount_;
        break;

    default:
        return false;
    }

    if (*quantity == std::numeric_limits<int>::max())
    {
        return false;
    }

    money_ -= price;
    ++(*quantity);

    return true;
}

bool Player::consumeTool(ToolType tool)
{
    int* quantity = nullptr;

    switch (tool)
    {
    case ToolType::Pickaxe:
        quantity = &pickaxeCount_;
        break;

    case ToolType::Shovel:
        quantity = &shovelCount_;
        break;

    case ToolType::Brush:
        quantity = &brushCount_;
        break;

    default:
        return false;
    }

    if (*quantity <= 0)
    {
        return false;
    }

    --(*quantity);

    return true;
}

void Player::addLoot(const LootItem& item)
{
    const int typeNumber = static_cast<int>(item.type);

    if (typeNumber < 1 ||
        typeNumber > static_cast<int>(MuseumItemCount))
    {
        return;
    }

    loot_.push_back(item);

    const auto index =
        static_cast<std::size_t>(typeNumber - 1);

    discoveredLoot_[index] = true;
}

const std::vector<LootItem>& Player::getLoot() const
{
    return loot_;
}

bool Player::sellAllLoot()
{
    if (loot_.empty())
    {
        return false;
    }

    int totalValue = 0;

    for (const auto& item : loot_)
    {
        if (item.value < 0)
        {
            return false;
        }

        if (item.value > std::numeric_limits<int>::max() - totalValue)
        {
            return false;
        }

        totalValue += item.value;
    }

    if (totalValue > std::numeric_limits<int>::max() - money_)
    {
        return false;
    }

    money_ += totalValue;
    loot_.clear();

    return true;
}

bool Player::isLootDiscovered(LootType type) const
{
    const int typeNumber = static_cast<int>(type);

    if (typeNumber < 1 ||
        typeNumber > static_cast<int>(MuseumItemCount))
    {
        return false;
    }

    const auto index =
        static_cast<std::size_t>(typeNumber - 1);

    return discoveredLoot_[index];
}

bool Player::saveToFile(const std::string& filename) const
{
    if (loot_.size() > 100000)
    {
        return false;
    }

    std::ofstream file(filename, std::ios::trunc);

    if (!file)
    {
        return false;
    }

    file << money_ << ' '
        << food_ << ' '
        << pickaxeCount_ << ' '
        << shovelCount_ << ' '
        << brushCount_ << '\n';

    file << loot_.size() << '\n';

    for (const auto& item : loot_)
    {
        file << static_cast<int>(item.type) << ' '
            << std::quoted(item.name) << ' '
            << item.value << '\n';
    }

    for (const bool discovered : discoveredLoot_)
    {
        file << (discovered ? 1 : 0) << ' ';
    }

    file << '\n';

    file.close();

    return !file.fail();
}

bool Player::loadFromFile(const std::string& filename)
{
    std::ifstream file(filename);

    if (!file)
    {
        return false;
    }

    int loadedMoney = 0;
    int loadedFood = 0;
    int loadedPickaxes = 0;
    int loadedShovels = 0;
    int loadedBrushes = 0;

    if (!(file >> loadedMoney
        >> loadedFood
        >> loadedPickaxes
        >> loadedShovels
        >> loadedBrushes))
    {
        return false;
    }

    if (loadedMoney < 0 ||
        loadedFood < 0 ||
        loadedPickaxes < 0 ||
        loadedShovels < 0 ||
        loadedBrushes < 0)
    {
        return false;
    }

    constexpr int MaxSavedLoot = 100000;

    int lootCount = 0;

    if (!(file >> lootCount) ||
        lootCount < 0 ||
        lootCount > MaxSavedLoot)
    {
        return false;
    }

    std::vector<LootItem> loadedLoot;
    loadedLoot.reserve(static_cast<std::size_t>(lootCount));

    std::array<bool, MuseumItemCount> loadedDiscoveries{};

    for (int i = 0; i < lootCount; ++i)
    {
        int typeNumber = 0;
        LootItem item;

        if (!(file >> typeNumber
            >> std::quoted(item.name)
            >> item.value))
        {
            return false;
        }

        if (typeNumber < 1 ||
            typeNumber > static_cast<int>(MuseumItemCount))
        {
            return false;
        }

        if (item.name.empty() || item.value < 0)
        {
            return false;
        }

        item.type = static_cast<LootType>(typeNumber);

        const auto index =
            static_cast<std::size_t>(typeNumber - 1);

        loadedDiscoveries[index] = true;

        loadedLoot.push_back(std::move(item));
    }

    for (std::size_t i = 0; i < MuseumItemCount; ++i)
    {
        int flag = 0;

        if (!(file >> flag) || (flag != 0 && flag != 1))
        {
            return false;
        }

        loadedDiscoveries[i] = loadedDiscoveries[i] || (flag == 1);
    }

    file >> std::ws;

    if (!file.eof() || file.bad())
    {
        return false;
    }

    money_ = loadedMoney;
    food_ = loadedFood;

    pickaxeCount_ = loadedPickaxes;
    shovelCount_ = loadedShovels;
    brushCount_ = loadedBrushes;

    loot_ = std::move(loadedLoot);
    discoveredLoot_ = loadedDiscoveries;

    return true;
}