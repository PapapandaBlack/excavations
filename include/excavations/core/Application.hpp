#pragma once

#include <SFML/Graphics.hpp>
#include <array>
#include <memory>
#include <random>

#include "excavations/game/Excavation.hpp"
#include "excavations/game/Player.hpp"
#include "excavations/game/ProductType.hpp"

class Application
{
public:
    Application();

    void run();

private:
    static constexpr unsigned int TitleSize = 70;
    static constexpr unsigned int BaseTitleSize = 70;
    static constexpr unsigned int ButtonSize = 30;
    static constexpr unsigned int InfoSize = 23;

    static constexpr int FoodPrice = 10;
    static constexpr int PickaxePrice = 30;
    static constexpr int ShovelPrice = 50;
    static constexpr int BrushPrice = 20;

    static constexpr int DesertTravelPrice = 20;
    static constexpr int UnderwaterTravelPrice = 40;
    static constexpr int PolarTravelPrice = 60;
    static constexpr int SpaceTravelPrice = 100;

    enum class Screen
    {
        Base,
        Expeditions,
        Shop,
        Museum,
        Excavation
    };

    struct MenuButton
    {
        sf::RectangleShape background;
        sf::Text label;

        explicit MenuButton(
            const sf::Font& font,
            const sf::String& text
        );
    };

    struct ExpeditionCard
    {
        int travelPrice;

        sf::RectangleShape panel;

        sf::Text title;
        sf::Text description;
        sf::Text costText;

        MenuButton travelButton;

        ExpeditionCard(
            const sf::String& name,
            const sf::String& descriptionText,
            int price,
            const sf::Font& infoFont,
            const sf::Font& buttonFont
        );
    };

    struct InventorySlot
    {
        sf::RectangleShape background;
        sf::RectangleShape icon;
        sf::Text quantity;

        explicit InventorySlot(const sf::Font& font)
            : quantity(font, U"0", Application::InfoSize)
        {
        }
    };

    struct ProductCard
    {
        ProductType type;
        int price;

        sf::RectangleShape panel;
        sf::Text title;
        sf::Text priceText;
        MenuButton buyButton;

        ProductCard(
            ProductType productType,
            const sf::String& name,
            int productPrice,
            const sf::Font& infoFont,
            const sf::Font& buttonFont
        );
    };

    void prepareButtonTextures();

    void processEvents();
    void updateHover();
    void render();

    void handleClick(sf::Vector2f mousePosition);
    void changeScreen(Screen screen);

    void positionButton(
        MenuButton& button,
        sf::Vector2f position
    );

    void setupInventory(sf::Vector2f panelPosition = { 620.f, 180.f });
    void updatePlayerStats();

    void setupShopCards();
    void purchaseProduct(const ProductCard& card);

    void setupExpeditionCards();
    void departExpedition(std::size_t index);

    void setupExcavationScreen();
    void handleExcavationClick(sf::Vector2f mousePosition);

    void updateExcavation();
    void drawExcavation();

    int getToolCount(ToolType tool) const;

    void setupLootArea();
    void sellLoot();

    sf::RenderWindow window_;
    
    sf::Font titleFont_;
    sf::Font buttonFont_;
    sf::Font infoFont_;

    sf::Texture baseTexture_;
    sf::Texture expeditionsTexture_;
    sf::Texture shopTexture_;
    sf::Texture museumTexture_;

    sf::Texture desertTexture_;
    sf::Texture underwaterTexture_;
    sf::Texture polarTexture_;
    sf::Texture spaceTexture_;

    sf::RectangleShape background_;

    sf::Texture buyButtonTexture_;
    sf::Texture buyButtonHoverTexture_;

    sf::Texture inventoryPanelTexture_;
    sf::Texture inventorySlotTexture_;
    sf::Texture productPanelTexture_;

    sf::Texture pickaxeIconTexture_;
    sf::Texture shovelIconTexture_;
    sf::Texture brushIconTexture_;

    sf::Texture moneyIconTexture_;
    sf::Texture foodIconTexture_;

    sf::RectangleShape moneyIcon_;
    sf::RectangleShape foodIcon_;

    sf::Texture sandTexture_;
    sf::Texture rockTexture_;
    sf::Texture snowTexture_;
    sf::Texture iceTexture_;
    sf::Texture spaceSoilTexture_;

    sf::Texture vaseTexture_;
    sf::Texture silverTexture_;
    sf::Texture goldTexture_;
    std::array<sf::Texture, 9> extraLootTextures_;

    void loadExtraLootTextures();

    const sf::Texture* getLootTexture(LootType type) const;

    std::array<LootType, 3> getExpeditionLootTypes() const;

    sf::Texture buttonTexture_;
    sf::Texture buttonHoverTexture_;
    sf::Texture panelTexture_;

    sf::RectangleShape resourcesPanel_;

    std::array<MenuButton, 4> buttons_;
    MenuButton backButton_;
    MenuButton saveButton_;
    MenuButton loadButton_;
    MenuButton sellLootButton_;

    std::array<ProductCard, 4> shopCards_;

    std::array<ExpeditionCard, 4> expeditionCards_;
    std::size_t selectedExpedition_ = 0;
    std::array<MenuButton, 3> toolButtons_;

    sf::Text pageTitle_;

    Player player_;
    sf::Text resourcesTitle_;
    sf::Text moneyText_;
    sf::Text foodText_;
    std::array<InventorySlot, 3> inventorySlots_;

    sf::RectangleShape lootPanel_;

    std::array<InventorySlot, 3> lootSlots_;

    sf::Text shopMessage_;

    sf::Text expeditionMessage_;

    Screen currentScreen_ = Screen::Base;

    std::unique_ptr<Excavation> activeExcavation_;

    std::mt19937 randomEngine_{ std::random_device{}() };

    std::array<
        std::array<sf::RectangleShape, Excavation::FieldColumns>,
        Excavation::FieldRows
    > fieldShapes_;

    sf::RectangleShape fieldBackground_;

    ToolType selectedTool_ = ToolType::Shovel;

    bool cellEffectActive_ = false;

    std::size_t effectRow_ = 0;
    std::size_t effectColumn_ = 0;

    sf::RectangleShape cellEffect_;
    sf::Clock cellEffectClock_;

    static constexpr float CellEffectDuration = 0.9f;

    static constexpr float MessageDuration = 3.f;

    sf::Clock shopMessageClock_;
    sf::Clock expeditionMessageClock_;

    void showShopMessage(const sf::String& text);
    void showExpeditionMessage(const sf::String& text);
    void updateMessageLayout();

    void setupMuseum();
    void drawMuseum();

    std::array<sf::RectangleShape, 12> museumItems_;
};