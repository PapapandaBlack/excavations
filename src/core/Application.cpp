#include <string>
#include <cstdint>
#include <stdexcept>
#include <utility>

#include "excavations/core/Application.hpp"
#include "excavations/game/DesertExcavation.hpp"
#include "excavations/game/UnderwaterExcavation.hpp"
#include "excavations/game/PolarExcavation.hpp"
#include "excavations/game/SpaceExcavation.hpp"

namespace
{
    const sf::Color BackgroundColor{ 30, 35, 45 };
    const sf::Color ButtonColor{ 60, 70, 90 };
    const sf::Color ButtonHoverColor{ 85, 105, 135 };

    void drawTextOnBlack(sf::RenderWindow& window, const sf::Text& text)
    {
        if (text.getString().isEmpty())
            return;

        const auto bounds = text.getGlobalBounds();
        sf::RectangleShape panel({ bounds.size.x + 24.f, bounds.size.y + 16.f });
        panel.setPosition({ bounds.position.x - 12.f, bounds.position.y - 8.f });
        panel.setFillColor(sf::Color::Black);
        window.draw(panel);
        window.draw(text);
    }
}

Application::MenuButton::MenuButton(
    const sf::Font& font,
    const sf::String& text
)
    : label(font, text, Application::ButtonSize)
{
    background.setSize({ 250.f, 80.f });
    background.setFillColor(sf::Color::White);

    label.setFillColor(sf::Color::White);
}

Application::Application()
    : window_(sf::VideoMode({ 1500, 1050 }), "Excavations"),
    titleFont_("assets/fonts/title.ttf"),
    buttonFont_("assets/fonts/gen2_font.ttf"),
    infoFont_("assets/fonts/gen2_font.ttf"),
    baseTexture_("assets/textures/base.png"),
    expeditionsTexture_("assets/textures/expeditions.png"),
    shopTexture_("assets/textures/shop.png"),
    museumTexture_("assets/textures/museum.png"),
    desertTexture_("assets/textures/desert.png"),
    underwaterTexture_("assets/textures/underwater.png"),
    polarTexture_("assets/textures/polar.png"),
    spaceTexture_("assets/textures/space.png"),
    buttonTexture_("assets/textures/button.png"),
    panelTexture_("assets/textures/panel.png"),
    buyButtonTexture_("assets/textures/buy_button.png"),
    inventoryPanelTexture_("assets/textures/inventory_panel.png"),
    inventorySlotTexture_("assets/textures/inventory_slot.png"),
    productPanelTexture_("assets/textures/product_panel.png"),
    pickaxeIconTexture_("assets/icons/pickaxe_icon.png"),
    shovelIconTexture_("assets/icons/shovel_icon.png"),
    brushIconTexture_("assets/icons/brush_icon.png"),
    moneyIconTexture_("assets/icons/coin_icon.png"),
    foodIconTexture_("assets/icons/food_icon.png"),
    sandTexture_("assets/textures/sand.png"),
    rockTexture_("assets/textures/rock.png"),
    snowTexture_("assets/textures/snow.png"),
    iceTexture_("assets/textures/ice.png"),
    spaceSoilTexture_("assets/textures/space_soil.png"),
    vaseTexture_("assets/icons/vase_icon.png"),
    silverTexture_("assets/icons/silver_icon.png"),
    goldTexture_("assets/icons/gold_icon.png"),
    buttons_{
        MenuButton{buttonFont_, U"Expeditions"},
        MenuButton{buttonFont_, U"Market"},
        MenuButton{buttonFont_, U"Museum"},
        MenuButton{buttonFont_, U"Exit"}
    },
    backButton_(buttonFont_, U"Back"),
    saveButton_(buttonFont_, U"Save"),
    loadButton_(buttonFont_, U"Load"),
    sellLootButton_(buttonFont_, U"Sell all loot"),
    shopCards_{
        ProductCard{
            ProductType::Food,
            U"Food",
            FoodPrice,
            infoFont_,
            buttonFont_
        },
        ProductCard{
            ProductType::Pickaxe,
            U"Pickaxe",
            PickaxePrice,
            infoFont_,
            buttonFont_
        },
        ProductCard{
            ProductType::Shovel,
            U"Shovel",
            ShovelPrice,
            infoFont_,
            buttonFont_
        },
        ProductCard{
            ProductType::Brush,
            U"Brush",
            BrushPrice,
            infoFont_,
            buttonFont_
        }
    },
    expeditionCards_{
        ExpeditionCard{
            U"Desert",
            U"Explore the\nancient sands.",
            DesertTravelPrice,
            infoFont_,
            buttonFont_
        },
        ExpeditionCard{
            U"Underwater",
            U"Discover the\nocean depths.",
            UnderwaterTravelPrice,
            infoFont_,
            buttonFont_
        },
        ExpeditionCard{
            U"Polar",
            U"Search beneath\nthe polar ice.",
            PolarTravelPrice,
            infoFont_,
            buttonFont_
        },
        ExpeditionCard{
            U"Space",
            U"Explore distant\nalien worlds.",
            SpaceTravelPrice,
            infoFont_,
            buttonFont_
        }
    },
    toolButtons_{
        MenuButton{buttonFont_, U"Pickaxe"},
        MenuButton{buttonFont_, U"Shovel"},
        MenuButton{buttonFont_, U"Brush"}
    },
    pageTitle_(titleFont_, U"Base", TitleSize),
    resourcesTitle_(infoFont_, U"Resources available:", InfoSize),
    moneyText_(infoFont_, U"", InfoSize),
    foodText_(infoFont_, U"", InfoSize),
    inventorySlots_{
        InventorySlot{infoFont_},
        InventorySlot{infoFont_},
        InventorySlot{infoFont_}
    },
    lootSlots_{
        InventorySlot{infoFont_},
        InventorySlot{infoFont_},
        InventorySlot{infoFont_}
    },
    shopMessage_(infoFont_, U"", InfoSize),
    expeditionMessage_(infoFont_, U"", InfoSize)
{
    window_.setFramerateLimit(60);

    window_.setView(
        sf::View(sf::FloatRect({ 0.f, 0.f }, { 1000.f, 700.f }))
    );

    background_.setSize({ 1000.f, 700.f });

    loadExtraLootTextures();

    setupMuseum();

    prepareButtonTextures();

    for (std::size_t i = 0; i < buttons_.size(); ++i)
    {
        auto& button = buttons_[i];

        button.background.setTexture(&buttonTexture_, true);
        button.background.setFillColor(sf::Color::White);
        button.label.setFillColor(sf::Color::White);

        positionButton(
            button,
            { 375.f, 240.f + static_cast<float>(i) * 90.f }
        );
    }

    backButton_.background.setTexture(&buttonTexture_, true);
    backButton_.background.setFillColor(sf::Color::White);
    backButton_.label.setFillColor(sf::Color::White);

    positionButton(backButton_, { 40.f, 580.f });

    const auto exitPosition = buttons_[3].background.getPosition();
    const auto exitSize = buttons_[3].background.getSize();

    const float gap = 10.f;
    const float buttonWidth = (exitSize.x - gap) / 2.f;
    const float buttonHeight = 55.f;
    const float rowTop = exitPosition.y + exitSize.y + 15.f;

    saveButton_.background.setTexture(&buttonTexture_, true);
    saveButton_.background.setFillColor(sf::Color::White);
    saveButton_.background.setSize({ buttonWidth, buttonHeight });
    saveButton_.label.setCharacterSize(ButtonSize);
    saveButton_.label.setFillColor(sf::Color::White);

    positionButton(
        saveButton_,
        { exitPosition.x, rowTop }
    );

    loadButton_.background.setTexture(&buttonTexture_, true);
    loadButton_.background.setFillColor(sf::Color::White);
    loadButton_.background.setSize({ buttonWidth, buttonHeight });
    loadButton_.label.setCharacterSize(ButtonSize);
    loadButton_.label.setFillColor(sf::Color::White);

    positionButton(
        loadButton_,
        { exitPosition.x + buttonWidth + gap, rowTop }
    );

    setupShopCards();
    setupExpeditionCards();
    setupExcavationScreen();
    setupInventory();

    shopMessage_.setFillColor(sf::Color::Black);
    expeditionMessage_.setFillColor(sf::Color::White);

    updatePlayerStats();
    changeScreen(Screen::Base);
}

void Application::positionButton(
    MenuButton& button,
    sf::Vector2f position
)
{
    button.background.setPosition(position);

    const auto bounds = button.label.getLocalBounds();

    button.label.setOrigin({
        bounds.position.x + bounds.size.x / 2.f,
        bounds.position.y + bounds.size.y / 2.f
        });

    button.label.setPosition({
        position.x + button.background.getSize().x / 2.f,
        position.y + button.background.getSize().y / 2.f
        });
}

void Application::run()
{
    while (window_.isOpen())
    {
        processEvents();

        if (!window_.isOpen())
        {
            break;
        }


        updateExcavation();
        updatePlayerStats();
        updateHover();
        updateMessageLayout();
        render();
    }
}

void Application::processEvents()
{
    while (const auto event = window_.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window_.close();
        }

        if (const auto* click =
            event->getIf<sf::Event::MouseButtonPressed>())
        {
            if (click->button == sf::Mouse::Button::Left)
            {
                const auto mousePosition =
                    window_.mapPixelToCoords(click->position);

                handleClick(mousePosition);
            }
        }
    }
}

void Application::updateHover()
{
    const auto mousePosition = window_.mapPixelToCoords(
        sf::Mouse::getPosition(window_)
    );

    const auto updateButton = [&](
        MenuButton& button,
        const sf::Texture& normalTexture,
        const sf::Texture& hoverTexture,
        sf::Color normalTextColor,
        sf::Color hoverTextColor
        )
        {
            const bool hovered =
                button.background.getGlobalBounds().contains(
                    mousePosition
                );

            button.background.setTexture(
                hovered ? &hoverTexture : &normalTexture,
                true
            );

            button.label.setFillColor(
                hovered ? hoverTextColor : normalTextColor
            );
        };

    if (currentScreen_ == Screen::Base)
    {
        for (auto& button : buttons_)
        {
            updateButton(
                button,
                buttonTexture_,
                buttonHoverTexture_,
                sf::Color::White,
                sf::Color::Black
            );
        }

        updateButton(
            saveButton_,
            buttonTexture_,
            buttonHoverTexture_,
            sf::Color::White,
            sf::Color::Black
        );

        updateButton(
            loadButton_,
            buttonTexture_,
            buttonHoverTexture_,
            sf::Color::White,
            sf::Color::Black
        );
    }
    else
    {
        updateButton(
            backButton_,
            buttonTexture_,
            buttonHoverTexture_,
            sf::Color::White,
            sf::Color::Black
        );

        if (currentScreen_ == Screen::Shop)
        {
            for (auto& card : shopCards_)
            {
                updateButton(
                    card.buyButton,
                    buyButtonTexture_,
                    buyButtonHoverTexture_,
                    sf::Color::Black,
                    sf::Color::White
                );
            }

            updateButton(
                sellLootButton_,
                buyButtonTexture_,
                buyButtonHoverTexture_,
                sf::Color::Black,
                sf::Color::White
            );
        }

        if (currentScreen_ == Screen::Expeditions)
        {
            for (auto& card : expeditionCards_)
            {
                updateButton(
                    card.travelButton,
                    buyButtonTexture_,
                    buyButtonHoverTexture_,
                    sf::Color::Black,
                    sf::Color::White
                );
            }
        }

        if (currentScreen_ == Screen::Excavation)
        {
            const std::array<ToolType, 3> tools{
                ToolType::Pickaxe,
                ToolType::Shovel,
                ToolType::Brush
            };

            for (std::size_t i = 0; i < toolButtons_.size(); ++i)
            {
                auto& button = toolButtons_[i];

                updateButton(
                    button,
                    buttonTexture_,
                    buttonHoverTexture_,
                    sf::Color::White,
                    sf::Color::Black
                );

                button.background.setOutlineColor(
                    sf::Color(230, 180, 40)
                );

                button.background.setOutlineThickness(
                    tools[i] == selectedTool_ ? 3.f : 0.f
                );
            }
        }
    }
}

void Application::handleClick(sf::Vector2f mousePosition)
{
    if (!window_.isOpen())
    {
        return;
    }

    if (currentScreen_ == Screen::Base)
    {
        if (saveButton_.background
            .getGlobalBounds()
            .contains(mousePosition))
        {
            const bool saved = player_.saveToFile("save.txt");

            showShopMessage(
                saved
                ? U"Game saved."
                : U"Unable to save the game."
            );

            return;
        }

        if (loadButton_.background
            .getGlobalBounds()
            .contains(mousePosition))
        {
            if (!player_.loadFromFile("save.txt"))
            {
                showShopMessage(
                    U"Save file is missing or invalid."
                );

                return;
            }

            activeExcavation_.reset();
            cellEffectActive_ = false;
            selectedExpedition_ = 0;
            selectedTool_ = ToolType::Shovel;

            updatePlayerStats();

            showShopMessage(U"Game loaded.");

            return;
        }

        if (buttons_[0].background.getGlobalBounds().contains(mousePosition))
        {
            changeScreen(Screen::Expeditions);
        }
        else if (buttons_[1].background.getGlobalBounds().contains(mousePosition))
        {
            changeScreen(Screen::Shop);
        }
        else if (buttons_[2].background.getGlobalBounds().contains(mousePosition))
        {
            changeScreen(Screen::Museum);
        }
        else if (buttons_[3].background.getGlobalBounds().contains(mousePosition))
        {
            window_.close();
        }

        return;
    }

    if (backButton_.background.getGlobalBounds().contains(mousePosition))
    {
        changeScreen(
            currentScreen_ == Screen::Excavation
            ? Screen::Expeditions
            : Screen::Base
        );

        return;
    }

    if (currentScreen_ == Screen::Excavation)
    {
        handleExcavationClick(mousePosition);
        return;
    }

    if (currentScreen_ == Screen::Expeditions)
    {
        for (std::size_t i = 0; i < expeditionCards_.size(); ++i)
        {
            const auto& card = expeditionCards_[i];

            if (card.travelButton.background
                .getGlobalBounds()
                .contains(mousePosition))
            {
                departExpedition(i);
                return;
            }
        }

        return;
    }

    if (currentScreen_ == Screen::Shop)
    {
        if (sellLootButton_.background
            .getGlobalBounds()
            .contains(mousePosition))
        {
            sellLoot();
            return;
        }

        for (const auto& card : shopCards_)
        {
            if (card.buyButton.background
                .getGlobalBounds()
                .contains(mousePosition))
            {
                purchaseProduct(card);
                return;
            }
        }
    }
}

void Application::changeScreen(Screen screen)
{
    currentScreen_ = screen;

    expeditionMessage_.setString(U"");

    if (currentScreen_ == Screen::Shop ||
        currentScreen_ == Screen::Expeditions ||
        currentScreen_ == Screen::Excavation)
    {
        sf::Vector2f inventoryPosition{ 720.f, 475.f };

        if (currentScreen_ == Screen::Shop)
        {
            inventoryPosition = { 620.f, 150.f };
        }
        else if (currentScreen_ == Screen::Excavation)
        {
            inventoryPosition = { 620.f, 150.f };
        }

        setupInventory(inventoryPosition);
        setupLootArea();

        updatePlayerStats();
    }

    shopMessage_.setString(U"");

    const bool isBase = currentScreen_ == Screen::Base;

    const float baseTitleY = 90.f;
    const float otherTitleY = 65.f;

    pageTitle_.setCharacterSize(
        isBase ? BaseTitleSize : TitleSize
    );

    pageTitle_.setFillColor(
        currentScreen_ == Screen::Shop
        ? sf::Color::Black
        : sf::Color::White
    );

    resourcesTitle_.setFillColor(sf::Color::Black);
    moneyText_.setFillColor(sf::Color::Black);
    foodText_.setFillColor(sf::Color::Black);

    switch (currentScreen_)
    {
    case Screen::Base:
        pageTitle_.setString(U"Archaeologists Base");
        background_.setTexture(&baseTexture_, true);
        break;

    case Screen::Expeditions:
        pageTitle_.setString(U"Expeditions");
        background_.setTexture(&expeditionsTexture_, true);
        break;

    case Screen::Shop:
        pageTitle_.setString(U"Market");
        background_.setTexture(&shopTexture_, true);
        break;

    case Screen::Museum:
        pageTitle_.setString(U"Museum");
        background_.setTexture(&museumTexture_, true);
        break;

    case Screen::Excavation:
        {
            if (activeExcavation_)
            {
                pageTitle_.setString(activeExcavation_->getName());
            }
            else
            {
                pageTitle_.setString(U"Excavation");
            }

            const std::array<const sf::Texture*, 4> backgrounds{
                &desertTexture_,
                &underwaterTexture_,
                &polarTexture_,
                &spaceTexture_
            };

            if (selectedExpedition_ < backgrounds.size())
            {
                background_.setTexture(
                    backgrounds[selectedExpedition_],
                    true
                );
            }
            else
            {
                background_.setTexture(&expeditionsTexture_, true);
            }

            break;
        }
    }

    const auto bounds = pageTitle_.getLocalBounds();

    pageTitle_.setOrigin({
        bounds.position.x + bounds.size.x / 2.f,
        bounds.position.y + bounds.size.y / 2.f
        });

    pageTitle_.setPosition({
        500.f,
        isBase ? baseTitleY : otherTitleY
        });
}

void Application::render()
{
    window_.clear(BackgroundColor);

    window_.draw(background_);
    
    if (currentScreen_ == Screen::Excavation)
        drawTextOnBlack(window_, pageTitle_);
    else
        window_.draw(pageTitle_);

    if (currentScreen_ == Screen::Base)
    {
        for (const auto& button : buttons_)
        {
            window_.draw(button.background);
            window_.draw(button.label);
        }

        window_.draw(saveButton_.background);
        window_.draw(saveButton_.label);

        window_.draw(loadButton_.background);
        window_.draw(loadButton_.label);

        drawTextOnBlack(window_, shopMessage_);
    }

    if (currentScreen_ == Screen::Shop)
    {
        for (const auto& card : shopCards_)
        {
            window_.draw(card.panel);
            window_.draw(card.title);
            window_.draw(card.priceText);

            window_.draw(card.buyButton.background);
            window_.draw(card.buyButton.label);
        }

        window_.draw(shopMessage_);
    }

    if (currentScreen_ == Screen::Expeditions)
    {
        for (const auto& card : expeditionCards_)
        {
            window_.draw(card.panel);

            window_.draw(card.title);
            window_.draw(card.description);
            window_.draw(card.costText);

            window_.draw(card.travelButton.background);
            window_.draw(card.travelButton.label);
        }
        drawTextOnBlack(window_, expeditionMessage_);
    }

    if (currentScreen_ == Screen::Excavation)
    {
        drawExcavation();
    }

    const bool showInventory =
        currentScreen_ == Screen::Shop ||
        currentScreen_ == Screen::Expeditions ||
        currentScreen_ == Screen::Excavation;

    if (showInventory)
    {
        sf::RenderStates inventoryStates;

        if (currentScreen_ == Screen::Expeditions)
        {
            const float inventoryScale = 0.55f;
            const auto position = resourcesPanel_.getPosition();

            inventoryStates.transform.translate(position);

            inventoryStates.transform.scale({
                inventoryScale,
                inventoryScale
                });

            inventoryStates.transform.translate({
                -position.x,
                -position.y
                });
        }

        window_.draw(resourcesPanel_, inventoryStates);
        window_.draw(resourcesTitle_, inventoryStates);

        window_.draw(moneyIcon_, inventoryStates);
        window_.draw(moneyText_, inventoryStates);

        window_.draw(foodIcon_, inventoryStates);
        window_.draw(foodText_, inventoryStates);

        for (const auto& slot : inventorySlots_)
        {
            window_.draw(slot.background, inventoryStates);
            window_.draw(slot.icon, inventoryStates);
            window_.draw(slot.quantity, inventoryStates);
        }
    }

    if (currentScreen_ == Screen::Shop)
    {
        window_.draw(sellLootButton_.background);
        window_.draw(sellLootButton_.label);
    }
    else if (currentScreen_ == Screen::Excavation)
    {
        window_.draw(lootPanel_);

        for (const auto& slot : lootSlots_)
        {
            window_.draw(slot.icon);
            window_.draw(slot.quantity);
        }
    }

    if (currentScreen_ == Screen::Museum)
    {
        drawMuseum();
    }

    if (currentScreen_ != Screen::Base)
    {
        window_.draw(backButton_.background);
        window_.draw(backButton_.label);
    }

    window_.display();
}

void Application::updatePlayerStats()
{
    moneyText_.setString(std::to_string(player_.getMoney()));
    foodText_.setString(std::to_string(player_.getFood()));

    const auto alignResourceText = [](
        sf::Text& text,
        const sf::RectangleShape& icon
        )
        {
            const auto textBounds = text.getLocalBounds();
            const auto iconBounds = icon.getGlobalBounds();

            text.setOrigin({
                textBounds.position.x,
                textBounds.position.y + textBounds.size.y / 2.f
                });

            text.setPosition({
                iconBounds.position.x + iconBounds.size.x + 10.f,
                iconBounds.position.y + iconBounds.size.y / 2.f
                });
        };

    alignResourceText(moneyText_, moneyIcon_);
    alignResourceText(foodText_, foodIcon_);

    const std::array<int, 3> quantities{
        player_.getPickaxeCount(),
        player_.getShovelCount(),
        player_.getBrushCount()
    };

    for (std::size_t i = 0; i < inventorySlots_.size(); ++i)
    {
        auto& slot = inventorySlots_[i];

        slot.quantity.setString(
            std::to_string(quantities[i])
        );

        const auto textBounds = slot.quantity.getLocalBounds();
        const auto slotBounds = slot.background.getGlobalBounds();

        slot.quantity.setOrigin({
            textBounds.position.x + textBounds.size.x,
            textBounds.position.y + textBounds.size.y
            });

        slot.quantity.setPosition({
            slotBounds.position.x + slotBounds.size.x - 14.f,
            slotBounds.position.y + slotBounds.size.y - 14.f
            });
    }

    const auto lootTypes = getExpeditionLootTypes();

    std::array<std::size_t, 3> lootCounts{};

    for (const auto& item : player_.getLoot())
    {
        for (std::size_t i = 0; i < lootTypes.size(); ++i)
        {
            if (item.type == lootTypes[i])
            {
                ++lootCounts[i];
                break;
            }
        }
    }

    for (std::size_t i = 0; i < lootSlots_.size(); ++i)
    {
        auto& slot = lootSlots_[i];

        slot.icon.setTexture(
            getLootTexture(lootTypes[i]),
            true
        );

        slot.quantity.setString(
            std::to_string(lootCounts[i])
        );

        const auto textBounds = slot.quantity.getLocalBounds();
        const auto iconBounds = slot.icon.getGlobalBounds();

        slot.quantity.setOrigin({
            textBounds.position.x,
            textBounds.position.y + textBounds.size.y / 2.f
            });

        slot.quantity.setPosition({
            iconBounds.position.x + iconBounds.size.x + 8.f,
            iconBounds.position.y + iconBounds.size.y / 2.f
            });
    }
}

void Application::prepareButtonTextures()
{
    const auto createInvertedTexture = [](
        const sf::Texture& source,
        sf::Texture& destination
        )
        {
            auto image = source.copyToImage();
            const auto size = image.getSize();

            for (unsigned int y = 0; y < size.y; ++y)
            {
                for (unsigned int x = 0; x < size.x; ++x)
                {
                    auto color = image.getPixel({ x, y });

                    color.r = static_cast<std::uint8_t>(255 - color.r);
                    color.g = static_cast<std::uint8_t>(255 - color.g);
                    color.b = static_cast<std::uint8_t>(255 - color.b);

                    image.setPixel({ x, y }, color);
                }
            }

            if (!destination.loadFromImage(image))
            {
                throw std::runtime_error(
                    "Failed to create inverted button texture"
                );
            }
        };

    createInvertedTexture(
        buttonTexture_,
        buttonHoverTexture_
    );

    createInvertedTexture(
        buyButtonTexture_,
        buyButtonHoverTexture_
    );
}

Application::ProductCard::ProductCard(
    ProductType productType,
    const sf::String& name,
    int productPrice,
    const sf::Font& infoFont,
    const sf::Font& buttonFont
)
    : type(productType),
    price(productPrice),
    title(infoFont, name, Application::InfoSize),
    priceText(infoFont, U"", Application::InfoSize),
    buyButton(buttonFont, U"Buy")
{
    title.setFillColor(sf::Color::White);
    priceText.setFillColor(sf::Color::White);

    const std::wstring priceLabel =
        std::wstring(L"Price: ")
        + std::to_wstring(price)
        + L" coins";

    priceText.setString(priceLabel);
}

void Application::setupShopCards()
{
    const std::array<sf::Vector2f, 4> positions{
        sf::Vector2f{40.f, 160.f},
        sf::Vector2f{320.f, 160.f},
        sf::Vector2f{40.f, 350.f},
        sf::Vector2f{320.f, 350.f}
    };

    for (std::size_t i = 0; i < shopCards_.size(); ++i)
    {
        auto& card = shopCards_[i];
        const auto position = positions[i];

        card.panel.setTexture(&productPanelTexture_, true);
        card.panel.setFillColor(sf::Color::White);
        card.panel.setSize({ 200.f, 150.f });
        card.panel.setPosition(position);

        const auto centerText = [&](
            sf::Text& text,
            float top
            )
            {
                const auto bounds = text.getLocalBounds();

                text.setOrigin({
                    bounds.position.x + bounds.size.x / 2.f,
                    bounds.position.y
                    });

                text.setPosition({
                    position.x + 100.f,
                    position.y + top
                    });
            };

        card.title.setFillColor(sf::Color::White);
        card.title.setOutlineColor(sf::Color::Black);
        card.title.setOutlineThickness(1.0f);

        card.priceText.setFillColor(sf::Color::White);
        card.priceText.setOutlineColor(sf::Color::Black);
        card.priceText.setOutlineThickness(1.0f);

        centerText(card.title, 20.f);
        centerText(card.priceText, 65.f);

        card.buyButton.background.setTexture(
            &buyButtonTexture_,
            true
        );

        card.buyButton.background.setFillColor(sf::Color::White);
        card.buyButton.background.setSize({ 170.f, 48.f });
        card.buyButton.label.setFillColor(sf::Color::Black);

        positionButton(
            card.buyButton,
            { position.x + 15.f, position.y + 107.f }
        );
    }
}

void Application::setupInventory(sf::Vector2f panelPosition)
{
    resourcesPanel_.setTexture(&inventoryPanelTexture_, true);
    resourcesPanel_.setFillColor(sf::Color::White);
    resourcesPanel_.setSize({ 350.f, 290.f });
    resourcesPanel_.setPosition(panelPosition);

    resourcesTitle_.setString(U"Resources and Tools: ");
    resourcesTitle_.setFillColor(sf::Color::Black);
    resourcesTitle_.setPosition({
        panelPosition.x + 20.f,
        panelPosition.y + 20.f
        });

    moneyIcon_.setTexture(&moneyIconTexture_, true);
    moneyIcon_.setFillColor(sf::Color::White);
    moneyIcon_.setSize({ 32.f, 32.f });
    moneyIcon_.setPosition({
        panelPosition.x + 25.f,
        panelPosition.y + 75.f
        });

    foodIcon_.setTexture(&foodIconTexture_, true);
    foodIcon_.setFillColor(sf::Color::White);
    foodIcon_.setSize({ 32.f, 32.f });
    foodIcon_.setPosition({
        panelPosition.x + 190.f,
        panelPosition.y + 75.f
        });

    moneyText_.setFillColor(sf::Color::Black);
    foodText_.setFillColor(sf::Color::Black);

    const std::array<const sf::Texture*, 3> toolTextures{
        &pickaxeIconTexture_,
        &shovelIconTexture_,
        &brushIconTexture_
    };

    for (std::size_t i = 0; i < inventorySlots_.size(); ++i)
    {
        auto& slot = inventorySlots_[i];

        const sf::Vector2f slotPosition{
            panelPosition.x + 22.f + static_cast<float>(i) * 105.f,
            panelPosition.y + 155.f
        };

        slot.background.setTexture(&inventorySlotTexture_, true);
        slot.background.setFillColor(sf::Color::White);
        slot.background.setSize({ 96.f, 96.f });
        slot.background.setPosition(slotPosition);

        slot.icon.setTexture(toolTextures[i], true);
        slot.icon.setFillColor(sf::Color::White);
        slot.icon.setSize({ 58.f, 58.f });
        slot.icon.setPosition({
            slotPosition.x + 10.f,
            slotPosition.y + 10.f
            });

        slot.quantity.setFillColor(sf::Color::Black);
        slot.quantity.setOutlineColor(sf::Color::White);
        slot.quantity.setOutlineThickness(1.5f);
    }
}

void Application::purchaseProduct(const ProductCard& card)
{
    if (player_.getMoney() < card.price)
    {
        showShopMessage(U"Insufficient budget!");
        return;
    }

    if (!player_.buyProduct(card.type, card.price))
    {
        showShopMessage(U"Could not\nprocess the purchase.");
        return;
    }

    shopMessage_.setString(U"");
}

Application::ExpeditionCard::ExpeditionCard(
    const sf::String& name,
    const sf::String& descriptionText,
    int price,
    const sf::Font& infoFont,
    const sf::Font& buttonFont
)
    : travelPrice(price),
    title(infoFont, name, Application::InfoSize),
    description(infoFont, descriptionText, Application::InfoSize),
    costText(infoFont, U"", Application::InfoSize),
    travelButton(buttonFont, U"Depart")
{
    title.setFillColor(sf::Color::Black);
    description.setFillColor(sf::Color::Black);
    costText.setFillColor(sf::Color::Black);

    const std::wstring priceLabel =
        std::wstring(L"Cost: ")
        + std::to_wstring(travelPrice)
        + L" coins";

    costText.setString(priceLabel);
}

void Application::setupExpeditionCards()
{
    const float cardScale = 0.70f;

    const float screenWidth = 1000.f;
    const float screenHeight = 700.f;
    const float sideMargin = 90.f;

    const float cardWidth = 200.f * cardScale;
    const float cardHeight = 176.f * cardScale;

    const float verticalOffset = 25.f;

    const float cardTop =
        (screenHeight - cardHeight) / 2.f + verticalOffset;

    const float cardCount =
        static_cast<float>(expeditionCards_.size());

    const float gap =
        (screenWidth - 2.f * sideMargin - cardCount * cardWidth)
        / (cardCount - 1.f);

    for (std::size_t i = 0; i < expeditionCards_.size(); ++i)
    {
        auto& card = expeditionCards_[i];

        const sf::Vector2f position{
            sideMargin + static_cast<float>(i) * (cardWidth + gap),
            cardTop
        };

        card.panel.setTexture(&productPanelTexture_, true);
        card.panel.setFillColor(sf::Color::White);
        card.panel.setSize({ cardWidth, cardHeight });
        card.panel.setPosition(position);

        card.title.setCharacterSize(22);
        card.description.setCharacterSize(20);
        card.costText.setCharacterSize(22);

        card.title.setScale({ cardScale, cardScale });
        card.description.setScale({ cardScale, cardScale });
        card.costText.setScale({ cardScale, cardScale });

        const auto applyOutline = [](sf::Text& text)
            {
                text.setFillColor(sf::Color::Black);
                text.setOutlineColor(sf::Color::White);
                text.setOutlineThickness(1.f);
            };

        applyOutline(card.title);
        applyOutline(card.description);
        applyOutline(card.costText);

        const auto positionText = [&](
            sf::Text& text,
            float top
            )
            {
                const auto bounds = text.getLocalBounds();

                text.setOrigin({
                    bounds.position.x + bounds.size.x / 2.f,
                    bounds.position.y
                    });

                text.setPosition({
                    position.x + cardWidth / 2.f,
                    position.y + top * cardScale
                    });
            };

        positionText(card.title, 12.f);
        positionText(card.description, 42.f);
        positionText(card.costText, 98.f);

        card.travelButton.background.setTexture(
            &buyButtonTexture_,
            true
        );

        card.travelButton.background.setFillColor(sf::Color::White);

        card.travelButton.background.setSize({
            170.f * cardScale,
            38.f * cardScale
            });

        card.travelButton.label.setCharacterSize(24);
        card.travelButton.label.setScale({ cardScale, cardScale });
        card.travelButton.label.setFillColor(sf::Color::Black);

        positionButton(
            card.travelButton,
            {
                position.x + 15.f * cardScale,
                position.y + 128.f * cardScale
            }
        );
    }
}

void Application::departExpedition(std::size_t index)
{
    if (currentScreen_ != Screen::Expeditions ||
        index >= expeditionCards_.size())
    {
        return;
    }

    if (player_.getFood() <= 0)
    {
        showExpeditionMessage(
            U"You need food\nbefore departure."
        );

        return;
    }

    const bool hasTools =
        player_.getPickaxeCount() > 0 ||
        player_.getShovelCount() > 0 ||
        player_.getBrushCount() > 0;

    if (!hasTools)
    {
        showExpeditionMessage(
            U"You need at least\none tool."
        );

        return;
    }

    const auto& card = expeditionCards_[index];

    if (player_.getMoney() < card.travelPrice)
    {
        showExpeditionMessage(U"Not enough coins.");
        return;
    }

    std::unique_ptr<Excavation> expedition;

    switch (index)
    {
    case 0:
        expedition = std::make_unique<DesertExcavation>();
        break;

    case 1:
        expedition = std::make_unique<UnderwaterExcavation>();
        break;

    case 2:
        expedition = std::make_unique<PolarExcavation>();
        break;

    case 3:
        expedition = std::make_unique<SpaceExcavation>();
        break;

    default:
        return;
    }

    expedition->generateField(randomEngine_);

    if (!player_.spendMoney(card.travelPrice))
    {
        showExpeditionMessage(
            U"Unable to pay\nfor this expedition."
        );

        return;
    }

    activeExcavation_ = std::move(expedition);
    selectedExpedition_ = index;

    if (player_.getShovelCount() > 0)
    {
        selectedTool_ = ToolType::Shovel;
    }
    else if (player_.getPickaxeCount() > 0)
    {
        selectedTool_ = ToolType::Pickaxe;
    }
    else
    {
        selectedTool_ = ToolType::Brush;
    }

    cellEffectActive_ = false;

    changeScreen(Screen::Excavation);

    showExpeditionMessage(
        U"Select a tool,\nthen click a cell."
    );
}

void Application::setupExcavationScreen()
{
    const float cellSize = 62.f;
    const float cellGap = 6.f;

    const sf::Vector2f fieldPosition{ 40.f, 150.f };

    for (std::size_t row = 0; row < Excavation::FieldRows; ++row)
    {
        for (
            std::size_t column = 0;
            column < Excavation::FieldColumns;
            ++column
            )
        {
            auto& shape = fieldShapes_[row][column];

            shape.setSize({ cellSize, cellSize });

            shape.setPosition({
                fieldPosition.x
                    + static_cast<float>(column) * (cellSize + cellGap),

                fieldPosition.y
                    + static_cast<float>(row) * (cellSize + cellGap)
                });
        }
    }

    const auto firstPosition =
        fieldShapes_[0][0].getPosition();

    const auto& lastCell =
        fieldShapes_[Excavation::FieldRows - 1]
        [Excavation::FieldColumns - 1];

    const auto lastPosition = lastCell.getPosition();
    const auto lastSize = lastCell.getSize();

    const float padding = 8.f;

    fieldBackground_.setFillColor(sf::Color::Black);

    fieldBackground_.setPosition({
        firstPosition.x - padding,
        firstPosition.y - padding
        });

    fieldBackground_.setSize({
        lastPosition.x + lastSize.x - firstPosition.x + padding * 2.f,
        lastPosition.y + lastSize.y - firstPosition.y + padding * 2.f
        });

    for (std::size_t i = 0; i < toolButtons_.size(); ++i)
    {
        auto& button = toolButtons_[i];

        button.background.setTexture(&buttonTexture_, true);
        button.background.setFillColor(sf::Color::White);
        button.background.setSize({ 110.f, 45.f });

        button.label.setCharacterSize(20);

        positionButton(
            button,
            { 40.f + static_cast<float>(i) * 118.f, 510.f }
        );
    }

    cellEffect_.setSize({ 46.f, 46.f });
}

int Application::getToolCount(ToolType tool) const
{
    switch (tool)
    {
    case ToolType::Pickaxe:
        return player_.getPickaxeCount();

    case ToolType::Shovel:
        return player_.getShovelCount();

    case ToolType::Brush:
        return player_.getBrushCount();
    }

    return 0;
}

void Application::handleExcavationClick(
    sf::Vector2f mousePosition
)
{
    if (!activeExcavation_)
    {
        return;
    }

    if (cellEffectActive_)
    {
        return;
    }

    const std::array<ToolType, 3> tools{
        ToolType::Pickaxe,
        ToolType::Shovel,
        ToolType::Brush
    };

    for (std::size_t i = 0; i < toolButtons_.size(); ++i)
    {
        if (toolButtons_[i].background
            .getGlobalBounds()
            .contains(mousePosition))
        {
            if (getToolCount(tools[i]) <= 0)
            {
                showExpeditionMessage(
                    U"You do not have this tool."
                );

                return;
            }

            selectedTool_ = tools[i];

            showExpeditionMessage(
                U"Tool selected.\nClick a cell."
            );

            return;
        }
    }

    for (std::size_t row = 0; row < Excavation::FieldRows; ++row)
    {
        for (
            std::size_t column = 0;
            column < Excavation::FieldColumns;
            ++column
            )
        {
            const auto& shape = fieldShapes_[row][column];

            if (!shape.getGlobalBounds().contains(mousePosition))
            {
                continue;
            }

            if (player_.getFood() <= 0)
            {
                showExpeditionMessage(U"No food left.");
                return;
            }

            if (getToolCount(selectedTool_) <= 0)
            {
                showExpeditionMessage(
                    U"You do not have the selected tool."
                );

                return;
            }

            const auto result = activeExcavation_->excavate(
                row,
                column,
                selectedTool_,
                randomEngine_
            );

            showExpeditionMessage(result.message);

            if (result.outcome == ExcavationOutcome::InvalidAction)
            {
                return;
            }

            player_.consumeFood(1);
            player_.consumeTool(selectedTool_);

            if (getToolCount(selectedTool_) == 0)
            {
                if (player_.getShovelCount() > 0)
                {
                    selectedTool_ = ToolType::Shovel;
                }
                else if (player_.getPickaxeCount() > 0)
                {
                    selectedTool_ = ToolType::Pickaxe;
                }
                else if (player_.getBrushCount() > 0)
                {
                    selectedTool_ = ToolType::Brush;
                }
            }

            effectRow_ = row;
            effectColumn_ = column;

            cellEffect_.setTexture(nullptr);
            cellEffect_.setFillColor(sf::Color::Transparent);

            if (result.outcome == ExcavationOutcome::Found ||
                result.outcome == ExcavationOutcome::Damaged)
            {
                const sf::Texture* lootTexture =
                    getLootTexture(result.lootType);

                if (lootTexture != nullptr)
                {
                    cellEffect_.setTexture(lootTexture, true);
                }

                const bool damaged =
                    result.outcome == ExcavationOutcome::Damaged;

                cellEffect_.setFillColor(
                    damaged
                    ? sf::Color(255, 70, 70)
                    : sf::Color::White
                );

                if (!damaged)
                {
                    player_.addLoot({
                        result.lootName,
                        result.lootValue,
                        result.lootType
                        });
                }
            }

            const auto cellPosition = shape.getPosition();

            cellEffect_.setPosition({
                cellPosition.x + 8.f,
                cellPosition.y + 8.f
                });

            cellEffectActive_ = true;
            (void)cellEffectClock_.restart();

            return;
        }
    }
}

void Application::updateExcavation()
{
    if (currentScreen_ != Screen::Excavation ||
        !activeExcavation_)
    {
        return;
    }

    if (cellEffectActive_)
    {
        if (cellEffectClock_.getElapsedTime().asSeconds()
            < CellEffectDuration)
        {
            return;
        }

        cellEffectActive_ = false;
    }

    bool fieldFinished = true;

    for (const auto& row : activeExcavation_->getField())
    {
        for (const auto& cell : row)
        {
            if (!cell.excavated)
            {
                fieldFinished = false;
            }
        }
    }

    const bool noFood = player_.getFood() <= 0;

    const bool noTools =
        player_.getPickaxeCount() <= 0 &&
        player_.getShovelCount() <= 0 &&
        player_.getBrushCount() <= 0;

    if (noFood || noTools || fieldFinished)
    {
        changeScreen(Screen::Expeditions);

        if (noFood)
        {
            showExpeditionMessage(
                U"No food left.\nYou returned with your loot."
            );
        }
        else if (noTools)
        {
            showExpeditionMessage(
                U"No tools left.\nYou returned with your loot."
            );
        }
        else
        {
            showExpeditionMessage(
                U"All cells excavated.\nYou returned with your loot."
            );
        }
    }
}

void Application::drawExcavation()
{
    if (!activeExcavation_)
    {
        return;
    }

    window_.draw(fieldBackground_);
    const auto& field = activeExcavation_->getField();

    for (std::size_t row = 0; row < Excavation::FieldRows; ++row)
    {
        for (
            std::size_t column = 0;
            column < Excavation::FieldColumns;
            ++column
            )
        {
            auto& shape = fieldShapes_[row][column];

            if (field[row][column].excavated)
            {
                shape.setTexture(nullptr);
                shape.setFillColor(sf::Color::Black);
            }
            else
            {
                switch (field[row][column].terrain)
                {
                case TerrainType::Sand:
                    shape.setTexture(&sandTexture_, true);
                    break;

                case TerrainType::Rock:
                    shape.setTexture(&rockTexture_, true);
                    break;

                case TerrainType::Snow:
                    shape.setTexture(&snowTexture_, true);
                    break;

                case TerrainType::Ice:
                    shape.setTexture(&iceTexture_, true);
                    break;

                case TerrainType::SpaceSoil:
                    shape.setTexture(&spaceSoilTexture_, true);
                    break;

                default:
                    shape.setTexture(nullptr);
                    break;
                }

                shape.setFillColor(sf::Color::White);
            }

            window_.draw(shape);
        }
    }

    if (cellEffectActive_)
    {
        window_.draw(cellEffect_);
    }

    for (const auto& button : toolButtons_)
    {
        window_.draw(button.background);
        window_.draw(button.label);
    }

    drawTextOnBlack(window_, expeditionMessage_);
}

void Application::setupLootArea()
{
    const auto panelPosition = resourcesPanel_.getPosition();
    const auto panelSize = resourcesPanel_.getSize();

    const sf::Vector2f areaPosition{
        panelPosition.x,
        panelPosition.y + panelSize.y + 12.f
    };

    const sf::Vector2f areaSize{ 350.f, 65.f };

    sellLootButton_.background.setTexture(
        &buyButtonTexture_,
        true
    );

    sellLootButton_.background.setFillColor(sf::Color::White);
    sellLootButton_.background.setSize(areaSize);

    sellLootButton_.label.setCharacterSize(ButtonSize);
    sellLootButton_.label.setFillColor(sf::Color::Black);

    positionButton(sellLootButton_, areaPosition);

    lootPanel_.setTexture(&inventoryPanelTexture_, true);
    lootPanel_.setFillColor(sf::Color::White);
    lootPanel_.setSize(areaSize);
    lootPanel_.setPosition(areaPosition);

    const auto lootTypes = getExpeditionLootTypes();

    for (std::size_t i = 0; i < lootSlots_.size(); ++i)
    {
        auto& slot = lootSlots_[i];

        const float offsetX =
            15.f + static_cast<float>(i) * 110.f;

        slot.icon.setTexture(getLootTexture(lootTypes[i]),true);
        slot.icon.setFillColor(sf::Color::White);
        slot.icon.setSize({ 40.f, 40.f });

        slot.icon.setPosition({
            areaPosition.x + offsetX,
            areaPosition.y + 12.f
            });

        slot.quantity.setFillColor(sf::Color::Black);
        slot.quantity.setOutlineThickness(0.f);
    }
}

void Application::sellLoot()
{
    if (player_.getLoot().empty())
    {
        showShopMessage(U"No loot to sell.");
        return;
    }

    if (!player_.sellAllLoot())
    {
        showShopMessage(
            U"Unable to sell loot:\nbalance limit."
        );

        return;
    }

    showShopMessage(U"All loot sold!");
}

void Application::updateMessageLayout()
{
    if (shopMessageClock_.getElapsedTime().asSeconds()
        >= MessageDuration)
    {
        if (!shopMessage_.getString().isEmpty())
        {
            shopMessage_.setString(U"");
        }
    }

    if (expeditionMessageClock_.getElapsedTime().asSeconds()
        >= MessageDuration)
    {
        if (!expeditionMessage_.getString().isEmpty())
        {
            expeditionMessage_.setString(U"");
        }
    }

    const auto shopBounds = shopMessage_.getLocalBounds();

    shopMessage_.setOrigin({
        shopBounds.position.x + shopBounds.size.x / 2.f,
        shopBounds.position.y
        });

    if (currentScreen_ == Screen::Base)
    {
        shopMessage_.setFillColor(sf::Color::White);

        const auto buttonPosition =
            saveButton_.background.getPosition();

        const auto buttonSize =
            saveButton_.background.getSize();

        shopMessage_.setPosition({
            500.f,
            buttonPosition.y + buttonSize.y + 20.f
            });
    }
    else
    {
        shopMessage_.setFillColor(sf::Color::Black);
        shopMessage_.setPosition({ 500.f, 560.f });
    }

    expeditionMessage_.setOrigin({ 0.f, 0.f });
    expeditionMessage_.setCharacterSize(18);

    if (currentScreen_ == Screen::Excavation)
    {
        const auto lootPosition = lootPanel_.getPosition();
        const auto lootSize = lootPanel_.getSize();

        expeditionMessage_.setPosition({
            lootPosition.x,
            lootPosition.y + lootSize.y + 15.f
            });
    }
    else if (currentScreen_ == Screen::Expeditions)
    {
        const auto panel = resourcesPanel_.getPosition();
        const auto bounds = expeditionMessage_.getLocalBounds();

        expeditionMessage_.setOrigin({
            bounds.position.x + bounds.size.x, bounds.position.y
            });
        expeditionMessage_.setPosition({ panel.x - 24.f, panel.y + 12.f });
    }
}

void Application::showShopMessage(const sf::String& text)
{
    shopMessage_.setString(text);
    (void)shopMessageClock_.restart();
}

void Application::showExpeditionMessage(const sf::String& text)
{
    expeditionMessage_.setString(text);
    (void)expeditionMessageClock_.restart();
}

void Application::loadExtraLootTextures()
{
    const std::array<const char*, 9> paths{
        "assets/icons/seashell_icon.png",
        "assets/icons/pearl_icon.png",
        "assets/icons/sunken_idol_icon.png",

        "assets/icons/fossil_icon.png",
        "assets/icons/ancient_amulet_icon.png",
        "assets/icons/frozen_meteorite_icon.png",

        "assets/icons/mineral_sample_icon.png",
        "assets/icons/alien_mechanism_icon.png",
        "assets/icons/energy_crystal_icon.png"
    };

    for (std::size_t i = 0; i < paths.size(); ++i)
    {
        if (!extraLootTextures_[i].loadFromFile(paths[i]))
        {
            throw std::runtime_error(
                std::string{ "Failed to load loot texture: " } + paths[i]
            );
        }
    }
}

const sf::Texture* Application::getLootTexture(
    LootType type
) const
{
    switch (type)
    {
    case LootType::CeramicVase:
        return &vaseTexture_;

    case LootType::SilverBar:
        return &silverTexture_;

    case LootType::GoldBar:
        return &goldTexture_;

    case LootType::Seashell:
        return &extraLootTextures_[0];

    case LootType::Pearl:
        return &extraLootTextures_[1];

    case LootType::SunkenIdol:
        return &extraLootTextures_[2];

    case LootType::Fossil:
        return &extraLootTextures_[3];

    case LootType::AncientAmulet:
        return &extraLootTextures_[4];

    case LootType::FrozenMeteorite:
        return &extraLootTextures_[5];

    case LootType::MineralSample:
        return &extraLootTextures_[6];

    case LootType::AlienMechanism:
        return &extraLootTextures_[7];

    case LootType::EnergyCrystal:
        return &extraLootTextures_[8];

    default:
        return nullptr;
    }
}

std::array<LootType, 3> Application::getExpeditionLootTypes() const
{
    switch (selectedExpedition_)
    {
    case 0:
        return {
            LootType::CeramicVase,
            LootType::SilverBar,
            LootType::GoldBar
        };

    case 1:
        return {
            LootType::Seashell,
            LootType::Pearl,
            LootType::SunkenIdol
        };

    case 2:
        return {
            LootType::Fossil,
            LootType::AncientAmulet,
            LootType::FrozenMeteorite
        };

    case 3:
        return {
            LootType::MineralSample,
            LootType::AlienMechanism,
            LootType::EnergyCrystal
        };

    default:
        return {
            LootType::None,
            LootType::None,
            LootType::None
        };
    }
}

void Application::setupMuseum()
{
    const float shelfLeft = 170.f;
    const float shelfWidth = 660.f;

    const std::array<float, 4> shelfBottoms{
        220.f,
        335.f,
        450.f,
        560.f
    };

    const float iconSize = 60.f;

    for (std::size_t row = 0; row < shelfBottoms.size(); ++row)
    {
        for (std::size_t column = 0; column < 3; ++column)
        {
            const std::size_t index = row * 3 + column;
            const auto type = static_cast<LootType>(index + 1);

            auto& icon = museumItems_[index];

            icon.setTexture(getLootTexture(type), true);
            icon.setFillColor(sf::Color::White);
            icon.setSize({ iconSize, iconSize });

            const float centerX =
                shelfLeft
                + (static_cast<float>(column) + 0.5f)
                * (shelfWidth / 3.f);

            icon.setPosition({
                centerX - iconSize / 2.f,
                shelfBottoms[row] - iconSize
                });
        }
    }
}

void Application::drawMuseum()
{
    for (std::size_t i = 0; i < museumItems_.size(); ++i)
    {
        const auto type = static_cast<LootType>(i + 1);

        if (player_.isLootDiscovered(type))
        {
            window_.draw(museumItems_[i]);
        }
    }
}