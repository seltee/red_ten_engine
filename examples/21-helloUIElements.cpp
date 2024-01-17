// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "../src/rtengine.h"
#include <math.h>
#include <vector>

#define TransparentColor Color(0.0f, 0.0f, 0.0f, 0.0f)
#define TopBarColor Color(0.2f, 0.2f, 0.22f, 1.0f)
#define ModalColor Color(0.22f, 0.22f, 0.24f, 1.0f)
#define ModalBackgroundColor Color(0.2, 0.2f, 0.2f, 0.4f)
#define HoverColor Color(0.4f, 0.4f, 0.42f, 1.0f)
#define BorderColor Color(0.85f, 0.25f, 0.25f, 1.0f)
#define EditorColor Color(0.15f, 0.15f, 0.2f, 1.0f)
#define TextColor Color(0.9f, 0.9f, 0.9f, 1.0f)

class UIController;

enum class ButtonAction
{
    None,
    TopMenuFile,
    TopMenuEdit,
    TopMenuHelp,
    MenuNew,
    MenuOpen,
    MenuExit,
    MenuCopy,
    MenuPaste,
    MenuAbout
};

enum class ModalType
{
    None,
    About
};

struct ButtonUserData
{
    UINode *node;
    UINode *menuNode;
    UIController *controller;
    ButtonAction action;
};

struct ModalUserData
{
    UINode *node;
    ModalType type;
};

struct ButtonDescriptor
{
    std::string name;
    ButtonAction action;
};

class UIController : public WithLogger
{
public:
    UIController(LayerUI *layer)
    {
        auto engine = RTEngine::getInstance();
        auto resourceController = engine->getResourceController();
        auto image = resourceController->addImage("./data/colors.jpg")->getAsTexture();

        this->layer = layer;

        // Left side
        auto leftPanel = this->layer->getRootNode()->createChildNode();
        leftPanel->style.setWidthPercentage(50);
        leftPanel->style.setHeightPercentage(100);
        leftPanel->style.setBackgroundColor(Color(0.92f, 0.92f, 0.92f, 1.0f));
        leftPanel->style.setLayoutDirection(UILayoutDirection::Vertical);

        createTitle(leftPanel, "Left");
        UINode *panel1 = createPanel(leftPanel);
        panel1->style.setContentHorizontalAlign(UIContentAlign::Start);
        createTitle(panel1, "[]");
        createTitle(panel1, "[]");
        createTitle(panel1, "[]");

        createTitle(leftPanel, "Middle");
        UINode *panel2 = createPanel(leftPanel);
        panel2->style.setContentHorizontalAlign(UIContentAlign::Middle);
        createTitle(panel2, "[]");
        createTitle(panel2, "[]");
        createTitle(panel2, "[]");

        createTitle(leftPanel, "Right");
        UINode *panel3 = createPanel(leftPanel);
        panel3->style.setContentHorizontalAlign(UIContentAlign::End);
        createTitle(panel3, "[]");
        createTitle(panel3, "[]");
        createTitle(panel3, "[]");

        createTitle(leftPanel, "Between");
        UINode *panel4 = createPanel(leftPanel);
        panel4->style.setContentHorizontalAlign(UIContentAlign::SpaceBetween);
        createTitle(panel4, "[]");
        createTitle(panel4, "[]");
        createTitle(panel4, "[]");

        createTitle(leftPanel, "Around");
        UINode *panel5 = createPanel(leftPanel);
        panel5->style.setContentHorizontalAlign(UIContentAlign::SpaceAround);
        createTitle(panel5, "[]");
        createTitle(panel5, "[]");
        createTitle(panel5, "[]");

        UINode *quads = createPanel(leftPanel);
        quads->style.setHeightPoints(20);
        UINode *quad1 = createQuad(quads, 40, 10, Color(0.6f, 0.1f, 0.1f, 1.0f));
        quad1->style.setZIndex(10);
        UINode *quad2 = createQuad(quad1, 40, 10, Color(0.1f, 0.6f, 0.1f, 1.0f));
        UINode *quad3 = createQuad(quad2, 40, 10, Color(0.1f, 0.1f, 0.6f, 1.0f));
        UINode *quad4 = createQuad(quad3, 40, 10, Color(0.7f, 0.7f, 0.7f, 1.0f));

        quads = createPanel(leftPanel);
        quads->style.setHeightPoints(20);
        quad1 = createQuad(quads, 40, 10, Color(0.8f, 0.1f, 0.1f, 1.0f));
        quad2 = createQuad(quad1, 40, 10, Color(0.1f, 0.8f, 0.1f, 1.0f));
        quad3 = createQuad(quad2, 40, 10, Color(0.1f, 0.1f, 0.8f, 1.0f));
        quad3->style.setZIndex(10);
        quad4 = createQuad(quad3, 40, 10, Color(0.8f, 0.8f, 0.8f, 1.0f));

        // Right side
        auto rightPanel = this->layer->getRootNode()->createChildNode();
        rightPanel->style.setWidthPercentage(50);
        rightPanel->style.setHeightPercentage(100);
        rightPanel->style.setBackgroundColor(Color(0.86f, 0.86f, 0.86f, 1.0f));
        rightPanel->style.setLayoutDirection(UILayoutDirection::Vertical);

        createTitle(rightPanel, "Place");
        panel1 = createPanel(rightPanel);
        panel1->style.setImage(image);
        panel1->style.setImagePosition(UIImagePositioning::Place);
        panel1->style.setHeightPoints(80);
        panel1->style.setOverflowVisibility(UIVisibility::Hidden);

        createTitle(rightPanel, "Place background");
        panel2 = createPanel(rightPanel);
        panel2->style.setImage(image);
        panel2->style.setImagePosition(UIImagePositioning::PlaceBackground);
        panel2->style.setHeightPoints(80);
        panel2->style.setOverflowVisibility(UIVisibility::Hidden);

        createTitle(rightPanel, "Cover");
        panel3 = createPanel(rightPanel);
        panel3->style.setImage(image);
        panel3->style.setImagePosition(UIImagePositioning::Cover);
        panel3->style.setHeightPoints(80);
        panel3->style.setOverflowVisibility(UIVisibility::Hidden);

        createTitle(rightPanel, "Contain");
        panel4 = createPanel(rightPanel);
        panel4->style.setImage(image);
        panel4->style.setImagePosition(UIImagePositioning::Contain);
        panel4->style.setHeightPoints(80);
        panel4->style.setOverflowVisibility(UIVisibility::Hidden);
    }

    void process()
    {
    }

protected:
    void setupMenuButton(UINode *node, std::string name, std::vector<ButtonDescriptor> menuItems, ButtonAction action)
    {
    }

    UINode *createPanel(UINode *parent)
    {
        auto panel = parent->createChildNode();
        panel->style.setMarginPoints(4, 4, 4, 4);
        panel->style.setPaddingPoints(4, 4, 4, 4);
        panel->style.setWidthPercentage(100);
        panel->style.setBackgroundColor(Color(0.8f, 0.8f, 0.8f, 1.0f));
        panel->style.setTextColor(Color(0.9f, 0.9f, 0.9f, 1.0f));
        return panel;
    }

    UINode *createTitle(UINode *parent, std::string title)
    {
        auto text = parent->createChildNode();
        text->style.setFontSize(16);
        text->style.setTextColor(Color(0.2f, 0.2f, 0.2f, 1.0f));
        text->style.setMarginLeftPoints(4);
        text->setText(title);
        return text;
    }

    UINode *createQuad(UINode *parent, float size, float margin, Color color)
    {
        auto quad = parent->createChildNode();
        quad->style.setWidthPoints(size);
        quad->style.setHeightPoints(size);
        quad->style.setPositioning(UIPosition::Absolute);
        quad->style.setBackgroundColor(color);
        quad->style.setMarginPoints(margin, margin, margin, margin);
        return quad;
    }

    LayerUI *layer;
    std::vector<ButtonUserData *> buttons;
    std::vector<ModalUserData *> modals;
    bool showTopBarMenu = false;
    ButtonAction shownMenu = ButtonAction::TopMenuFile;
    ModalType modalShownType = ModalType::None;
};

APPMAIN
{
    // Engine setup
    auto engine = RTEngine::getInstance();

    // Set fullscreen through configuration controller
    auto configController = engine->getConfigController();
    auto config = configController->getConfig();
    config->setWindowWidth(2560);
    config->setWindowHeight(1440);
    config->setFullscreenState(false);

    // View setup
    auto viewController = engine->getViewController();
    auto view = viewController->createView("Example \"21. Hello UI Elements\"", VIEW_CREATION_FLAG_RESIZABLE);
    int screenWidth = viewController->getPrimaryScreenWidth();
    int screenHeight = viewController->getPrimaryScreenHeight();

    // Stage setup
    auto stageController = engine->getStageController();
    auto stage = stageController->createStage("Hello UI Elements");

    // UI setup
    auto uiLayer = stage->createLayerUI("Elements UI", 1);
    uiLayer->setZoom(viewController->getDPIZoom());
    auto uiController = new UIController(uiLayer);

    while (!engine->isTerminationIntended())
    {
        float delta = engine->syncFrame();
        viewController->processEvents();
        uiController->process();
        stage->process(delta);
        stage->present(view);
    }

    engine->destroy();
    return 0;
}