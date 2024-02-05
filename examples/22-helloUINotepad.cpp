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

        this->layer = layer;

        auto root = layer->getRootNode();
        root->style.setLayoutDirection(UILayoutDirection::Vertical);
        root->setUserCallbackData(this);

        auto modalWindowAboutContainer = root->createChildNode();
        modalWindowAboutContainer->style.setPositioning(UIPosition::Absolute);
        modalWindowAboutContainer->style.setWidthPercentage(100);
        modalWindowAboutContainer->style.setHeightPercentage(100);
        modalWindowAboutContainer->style.setContentHorizontalAlign(UIContentAlign::Middle);
        modalWindowAboutContainer->style.setContentVerticalAlign(UIContentAlign::Middle);
        modalWindowAboutContainer->style.setBackgroundColor(ModalBackgroundColor);

        auto modalWindowAboutBorder = modalWindowAboutContainer->createChildNode();
        modalWindowAboutBorder->style.setPaddingPoints(1, 1, 1, 1);
        modalWindowAboutBorder->style.setBackgroundColor(BorderColor);

        auto modalWindowAbout = modalWindowAboutBorder->createChildNode();
        modalWindowAbout->style.setWidthPoints(340);
        modalWindowAbout->style.setHeightPoints(180);
        modalWindowAbout->style.setLayoutDirection(UILayoutDirection::Vertical);
        modalWindowAbout->style.setBackgroundColor(ModalColor);
        modalWindowAbout->style.setContentHorizontalAlign(UIContentAlign::Middle);
        modalWindowAbout->style.setContentVerticalAlign(UIContentAlign::Middle);

        auto str0 = modalWindowAbout->createChildNode();
        str0->setText("ABOUT");
        str0->style.setTextColor(BorderColor);

        auto str1 = modalWindowAbout->createChildNode();
        str1->setText("This app is created as part of Tutorials!");
        str1->style.setTextColor(TextColor);

        auto str2 = modalWindowAbout->createChildNode();
        str2->setText("By Red Ten Games");
        str2->style.setTextColor(TextColor);

        ModalUserData *aboutModalData = new ModalUserData();
        aboutModalData->node = modalWindowAboutContainer;
        aboutModalData->type = ModalType::About;
        modals.push_back(aboutModalData);

        auto topBar = root->createChildNode();
        topBar->style.setLayoutDirection(UILayoutDirection::Horizontal);
        topBar->style.setHeightPoints(24.0f);
        topBar->style.setWidthPercentage(100.0f);
        topBar->style.setBackgroundColor(TopBarColor);

        auto topBarDivider = root->createChildNode();
        topBarDivider->style.setHeightPoints(1.0f);
        topBarDivider->style.setWidthPercentage(100.0f);
        topBarDivider->style.setBackgroundColor(BorderColor);

        auto fileMenu = topBar->createChildNode();
        setupMenuButton(fileMenu, "File", {
                                              {"New", ButtonAction::MenuNew},
                                              {"Open", ButtonAction::MenuOpen},
                                              {"Exit", ButtonAction::MenuExit},
                                          },
                        ButtonAction::TopMenuFile);

        auto editMenu = topBar->createChildNode();
        setupMenuButton(editMenu, "Edit", {{"Copy", ButtonAction::MenuCopy}, {"Paste", ButtonAction::MenuPaste}}, ButtonAction::TopMenuEdit);

        auto helpMenu = topBar->createChildNode();
        setupMenuButton(helpMenu, "Help", {{"About", ButtonAction::MenuAbout}}, ButtonAction::TopMenuHelp);

        auto textContainer = root->createChildNode();
        textContainer->style.setLayoutDirection(UILayoutDirection::Vertical);
        textContainer->style.setHeightPercentage(100.0f);
        textContainer->style.setWidthPercentage(100.0f);
        textContainer->style.setBackgroundColor(EditorColor);
        textContainer->style.setImage(resourceController->addImage("./data/colors.jpg")->getAsTexture());
        textContainer->style.setImagePosition(UIImagePositioning::Cover);
        textContainer->style.setImageAlpha(0.05f);
        textContainer->style.setOverflowVisibility(UIVisibility::Hidden);

        auto text = textContainer->createChildInputNode();
        text->style.setPaddingPoints(8, 8, 8, 8);
        text->style.setWidthPercentage(100.0f);
        text->style.setHeightPercentage(100.0f);
        text->style.setTextColor(TextColor);
        text->style.setFontSize(16);
        text->setMultilineEnabled(true);
        text->setSuperFocus(true);

        root->setPointerCallback([](PointerCallbackData *data) -> bool
                                 {
                    UIController *userData = reinterpret_cast<UIController *>(data->userData);
                    if (data->event == UIPointerEvent::CursorClickLeftPress)
                        userData->processButtonPress(ButtonAction::None);
                    return true; });
    }

    void process()
    {
        for (auto &button : buttons)
        {
            if (isTopMenuAction(button->action))
            {
                if (showTopBarMenu && button->menuNode && button->action == shownMenu)
                    button->menuNode->style.setVisibility(UIVisibility::Visible);
                else
                    button->menuNode->style.setVisibility(UIVisibility::Hidden);
            }
        }
        for (auto &modal : modals)
        {
            if (modal->type == modalShownType)
                modal->node->style.setVisibility(UIVisibility::Visible);
            else
                modal->node->style.setVisibility(UIVisibility::Hidden);
        }
    }

    void processButtonPress(ButtonAction action)
    {
        this->modalShownType = ModalType::None;
        if (isTopMenuAction(action))
        {
            showTopBarMenu = !showTopBarMenu;
        }
        else
        {
            if (action == ButtonAction::MenuAbout)
            {
                this->modalShownType = ModalType::About;
            }
            showTopBarMenu = false;
        }
    }

    void processMouseHover(ButtonAction action)
    {
        if (isTopMenuAction(action))
        {
            shownMenu = action;
        }
    }

    bool isTopMenuAction(ButtonAction action)
    {
        return action == ButtonAction::TopMenuFile || action == ButtonAction::TopMenuEdit || action == ButtonAction::TopMenuHelp;
    }

protected:
    void setupMenuButton(UINode *node, std::string name, std::vector<ButtonDescriptor> menuItems, ButtonAction action)
    {
        ButtonUserData *buttonUserData = new ButtonUserData();
        buttonUserData->node = node;
        buttonUserData->action = action;
        buttonUserData->menuNode = nullptr;
        buttonUserData->controller = this;
        buttons.push_back(buttonUserData);

        node->style.setHeightPoints(24.0f);
        node->style.setAdaptiveWidth();
        node->style.setBackgroundColor(TopBarColor);
        node->style.setTextColor(TextColor);
        node->style.setTextHorizontalAlign(UITextAlign::Middle);
        node->style.setTextVerticalAlign(UITextAlign::Middle);
        node->style.setPaddingLeftPoints(8);
        node->style.setPaddingRightPoints(8);
        node->style.setHoverCursor(UICursor::Pointer);
        node->setUserCallbackData(buttonUserData);
        node->setText(name);

        if (menuItems.size() > 0)
        {
            auto menu = node->createChildNode();
            buttonUserData->menuNode = menu;
            menu->style.setBackgroundColor(BorderColor);
            menu->style.setPaddingLeftPoints(1);
            menu->style.setPaddingRightPoints(1);
            menu->style.setPaddingTopPoints(1);
            menu->style.setPaddingBottomPoints(1);
            menu->style.setMarginTopPoints(24.0f);
            menu->style.setMarginLeftPoints(-8.0f);
            menu->style.setPositioning(UIPosition::Absolute);
            menu->style.setVisibility(UIVisibility::Hidden);
            menu->style.setZIndex(1);

            auto menuInner = menu->createChildNode();
            menuInner->style.setBackgroundColor(TopBarColor);
            menuInner->style.setLayoutDirection(UILayoutDirection::Vertical);
            menuInner->style.setPaddingLeftPoints(0);
            menuInner->style.setPaddingRightPoints(0);
            menuInner->style.setPaddingTopPoints(0);
            menuInner->style.setPaddingBottomPoints(0);
            menuInner->style.setWidthPoints(120);

            for (auto &item : menuItems)
            {
                auto menuItem = menuInner->createChildNode();
                menuItem->setText(item.name);
                menuItem->style.setLayoutDirection(UILayoutDirection::Vertical);
                // menuItem->style.setBackgroundColor(TopBarColor);
                menuItem->style.setTextColor(TextColor);
                menuItem->style.setTextHorizontalAlign(UITextAlign::Start);
                menuItem->style.setTextVerticalAlign(UITextAlign::Middle);
                menuItem->style.setHeightPoints(24.0f);
                menuItem->style.setPaddingLeftPoints(6);
                menuItem->style.setPaddingRightPoints(6);
                menuItem->style.setWidthPoints(120 - 12);
                menuItem->style.setHoverCursor(UICursor::Pointer);

                ButtonUserData *menuButtonUserData = new ButtonUserData();
                menuButtonUserData->node = menuItem;
                menuButtonUserData->action = item.action;
                menuButtonUserData->menuNode = nullptr;
                menuButtonUserData->controller = this;
                buttons.push_back(menuButtonUserData);
                menuItem->setUserCallbackData(menuButtonUserData);

                menuItem->setPointerCallback([](PointerCallbackData *data) -> bool
                                             {
                    ButtonUserData *userData = reinterpret_cast<ButtonUserData *>(data->userData);
                    if (data->event == UIPointerEvent::CursorHover)
                    {
                        userData->node->style.setBackgroundColor(HoverColor);
                        userData->controller->processMouseHover(userData->action);
                    }

                    if (data->event == UIPointerEvent::CursorBlur)
                        userData->node->style.setBackgroundColor(TransparentColor);

                    if (data->event == UIPointerEvent::CursorClickLeftPress)
                    {
                        userData->controller->processButtonPress(userData->action);
                    } 
                    return false; });
            }

            node->setPointerCallback([](PointerCallbackData *data) -> bool
                                     {
        ButtonUserData *userData = reinterpret_cast<ButtonUserData *>(data->userData);
        if (data->event == UIPointerEvent::CursorHover){
            userData->node->style.setBackgroundColor(HoverColor);
            userData->controller->processMouseHover(userData->action);
        }
        
        if (data->event == UIPointerEvent::CursorBlur)
            userData->node->style.setBackgroundColor(TopBarColor);
        
        if (data->event == UIPointerEvent::CursorClickLeftPress){
            userData->controller->processButtonPress(userData->action);
        }
        if (data->event == UIPointerEvent::CursorClickLeftRelease){
        }
        return false; });
        }
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
    config->setWindowWidth(1920);
    config->setWindowHeight(1080);
    config->setFullscreenState(false);

    // View setup
    auto viewController = engine->getViewController();
    auto view = viewController->createView("Example \"22. Hello UI Notepad\"", VIEW_CREATION_FLAG_RESIZABLE);

    // Stage setup
    auto stageController = engine->getStageController();
    auto stage = stageController->createStage("Hello UI Notepad");

    // UI setup
    auto uiLayer = stage->createLayerUI("Notepad UI", 1);
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