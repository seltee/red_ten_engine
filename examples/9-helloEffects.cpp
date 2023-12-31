// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "../src/rtengine.h"
#include <math.h>

enum GUI_BUTTON
{
    NONE = 0,
    TOGGLE_LOCAL_COLOR_INVERSE,
    TOGGLE_LOCAL_COLOR_SHIFT,
    TOGGLE_LOCAL_COLOR_BW,
    TOGGLE_LOCAL_COLOR_JOJO
};

// Btw, Jojo belongs to it's owner and has no relation to Red Ten Roads
class JoJo : public Actor
{
public:
    JoJo() : Actor()
    {
        registerClassName("JoJo");
        transform.setScale(0.8);
        transform.setPosition(80.0f, -100.0f);

        sprite = createComponent<ComponentSprite>();
        sprite->setTexture(jojoTexture);
    }

    static Texture *jojoTexture;

protected:
    ComponentSprite *sprite;
};
Texture *JoJo::jojoTexture = nullptr;

class GUIButton : public ActorGUIElement
{
public:
    GUIButton() : ActorGUIElement()
    {
        registerClassName("Gui Button");
        setActiveArea(400.0f, 48.0f);
        setSelectiveIndex(index++);

        textShadowSprite = createComponent<ComponentText>();
        textShadowSprite->setFont(font);
        textShadowSprite->setText("Unknown Button");
        textShadowSprite->setColor(40, 40, 40);
        textShadowSprite->setOpacity(0.8f);
        textShadowSprite->transform.setPosition(5.0f, -1.0f);

        textSprite = createComponent<ComponentText>();
        textSprite->setFont(font);
        textSprite->setText("Unknown Button");
        textSprite->setColor(170, 170, 170);
    }

    void setup(float x, float y, std::string text, int fireEvent)
    {
        transform.setPosition(x, y);
        setPressID(fireEvent);
        setText(text);
    }

    void setText(std::string text)
    {
        textSprite->setText(text);
        textShadowSprite->setText(text);
    }

    void onProcess(float delta)
    {
        ActorGUIElement::onProcess(delta);

        if (bIsHovered)
            counter = fminf(counter + delta * 5.0f, 1.0f);
        if (!bIsHovered)
            counter = fmaxf(counter - delta * 5.0f, 0.0f);

        textSprite->transform.setPosition(0, sinf(counter * CONST_PI / 2.0f) * 8.0f);
    }

    void onHover()
    {
        textSprite->setColor(255, 255, 255);
    }

    void onBlur()
    {
        textSprite->setColor(170, 170, 170);
    }

    static ResourceFont *font;

protected:
    float counter;
    ComponentText *textSprite, *textShadowSprite;
    static int index;
    int color = 170;
};
ResourceFont *GUIButton::font = nullptr;
int GUIButton::index = 0;

class CratePlate : public Actor
{
public:
    CratePlate() : Actor()
    {
        registerClassName("CratePlate");
        transform.setScale(0.8);
        sprite = createComponent<ComponentSprite>();
    }

    void onProcess(float delta)
    {
        transform.rotate((bIsPlate ? 2.0f : -2.0f) * delta);
    }

    void setAsCrate()
    {
        sprite->setTexture(crateTexture);
        bIsPlate = false;
    }

    void setAsPlate()
    {
        sprite->setTexture(plateTexture);
        bIsPlate = true;
    }

    static Texture *crateTexture;
    static Texture *plateTexture;

protected:
    bool bIsPlate = false;
    ComponentSprite *sprite;
};
Texture *CratePlate::crateTexture = nullptr;
Texture *CratePlate::plateTexture = nullptr;

extern const std::string fragmentInverseShader;
extern const std::string fragmentShiftShader;
extern const std::string fragmentBWShader;
extern const std::string fragmentJoJoShader;

// Effects are technically shaders applied to the whole rendered layer
APPMAIN
{
    // Engine setup
    auto engine = RTEngine::getInstance();

    // Set fullscreen through configuration controller
    auto configController = engine->getConfigController();
    auto config = configController->getConfig();
    config->setWindowWidth(1280);
    config->setWindowHeight(800);
    config->setFullscreenState(false);

    // View setup
    auto viewController = engine->getViewController();
    auto view = viewController->createView("Example \"9. Hello Effects\"");

    // Stage setup
    auto stageController = engine->getStageController();
    auto stage = stageController->createStage("Hello Effects");

    // We have now 4 layers. 3 to draw actors and 1 to apply effects
    // Note the position of effect layer. It will be below gui layer and will effect only first 2 layers with background and jojo
    auto bottomLayerActors = stage->createLayerActors("Hello Bottom Actor Layer", 0);
    bottomLayerActors->enableSorting();
    auto bottomLayerCamera = bottomLayerActors->createActor<ActorCamera>();
    bottomLayerCamera->setupOrtoCamera()->setWidthBasedResolution(1280);

    auto topLayerActors = stage->createLayerActors("Hello Top Actor Layer", 1);
    topLayerActors->enableSorting();
    auto topLayerCamera = topLayerActors->createActor<ActorCamera>();
    topLayerCamera->setupOrtoCamera()->setWidthBasedResolution(1280);

    // Effects is a plane that is rendered with specific fragment shader you create
    // It takes previous render result and renders it to a buffer
    // Then it renders it back to the main buffer
    // Since renderer may use different graphic APIs be sure to check the current API and use shader for this API
    auto effectLayer = stage->createLayerEffects("Hello Effects Layer", 2);
    auto effectJoJo = view->getRenderer()->createOpenGLShader(fragmentJoJoShader);
    effectJoJo->setOpacity(0.0f);
    effectLayer->addEffect(effectJoJo);
    auto effectInverse = view->getRenderer()->createOpenGLShader(fragmentInverseShader);
    effectInverse->setOpacity(0.0f);
    effectLayer->addEffect(effectInverse);
    auto effectBW = view->getRenderer()->createOpenGLShader(fragmentBWShader);
    effectBW->setOpacity(0.0f);
    effectLayer->addEffect(effectBW);
    auto effectShift = view->getRenderer()->createOpenGLShader(fragmentShiftShader);
    effectShift->setOpacity(0.0f);
    effectLayer->addEffect(effectShift);

    auto layerGUI = stage->createLayerActors("Hello GUI Layer", 3);
    layerGUI->enableSorting();

    auto guiCamera = layerGUI->createActor<ActorCamera>();
    guiCamera->setupOrtoCamera()->setWidthBasedResolution(1280);

    // Textures, fonts setup
    auto resourceController = engine->getResourceController();
    JoJo::jojoTexture = resourceController->addImage("./data/jojo.png")->getAsTexture();
    auto background = resourceController->addImage("./data/background.jpg")->getAsTexture();
    CratePlate::crateTexture = resourceController->addImage("./data/crate.jpg")->getAsTexture();
    CratePlate::plateTexture = resourceController->addImage("./data/plate.png")->getAsTexture();

    auto font = resourceController->addFont("./data/BebasNeue-Regular.ttf", 72);
    GUIButton::font = resourceController->addFont("./data/BebasNeue-Regular.ttf", 48);

    // Just a background for better look
    auto backgroundActor = bottomLayerActors->createActor<Actor>();
    backgroundActor->transform.setScale(1.5f);
    auto backgroundActorSprite = backgroundActor->createComponent<ComponentSprite>();
    backgroundActorSprite->setTexture(background);

    auto dio = topLayerActors->createActor<Actor>();
    dio->transform.setScale(1.1f);
    dio->transform.setPosition(-100.0f, 0.0f);
    auto dioSprite = dio->createComponent<ComponentSprite>();
    dioSprite->setTexture(resourceController->addImage("./data/dio.jpg")->getAsTexture());
    dioSprite->setOpacity(0);

    auto plate = topLayerActors->createActor<CratePlate>();
    plate->setAsPlate();
    plate->transform.setPosition(-400.0f, 0);

    auto crate = topLayerActors->createActor<CratePlate>();
    crate->setAsCrate();
    crate->transform.setPosition(400.0f, 0);

    topLayerActors->createActor<JoJo>();

    auto headerLeft = layerGUI->createActor<Actor>();
    headerLeft->transform.setPosition(-400, 0.0f);
    auto textLeft = headerLeft->createComponent<ComponentText>();
    textLeft->setFont(font);
    textLeft->setText("Local Effects");
    textLeft->setColor(255, 255, 255);

    auto headerRight = layerGUI->createActor<Actor>();
    headerRight->transform.setPosition(400, 0.0f);
    auto textRight = headerRight->createComponent<ComponentText>();
    textRight->setFont(font);
    textRight->setText("Global Effects");
    textRight->setColor(255, 255, 255);

    GUIButton *button;
    button = layerGUI->createActor<GUIButton>();
    button->setup(400.0f, -70.0f, "Toggle Color Inverse", GUI_BUTTON::TOGGLE_LOCAL_COLOR_INVERSE);

    button = layerGUI->createActor<GUIButton>();
    button->setup(400.0f, -140.0f, "Toggle Color Shift", GUI_BUTTON::TOGGLE_LOCAL_COLOR_SHIFT);

    button = layerGUI->createActor<GUIButton>();
    button->setup(400.0f, -210.0f, "Toggle black/white", GUI_BUTTON::TOGGLE_LOCAL_COLOR_BW);

    button = layerGUI->createActor<GUIButton>();
    button->setup(400.0f, -280.0f, "Toggle JoJo battle", GUI_BUTTON::TOGGLE_LOCAL_COLOR_JOJO);

    bool bColorJoJo = false;
    bool bColorInverse = false;
    bool bColorBW = false;
    bool bColorShift = false;

    while (!engine->isTerminationIntended())
    {
        float delta = engine->syncFrame();
        viewController->processEvents();
        stage->process(delta);

        if (effectJoJo->getOpacity() > 0.0f && effectInverse->getOpacity() > 0.0f)
            dioSprite->setOpacity(effectInverse->getOpacity());
        else
            dioSprite->setOpacity(0);

        stage->present(view);

        // GUI enabling/disabling of the effects
        int firstPressID;
        while ((firstPressID = ActorGUIElement::getFirstPressID()))
        {
            if (firstPressID == GUI_BUTTON::TOGGLE_LOCAL_COLOR_JOJO)
                bColorJoJo = !bColorJoJo;
            if (firstPressID == GUI_BUTTON::TOGGLE_LOCAL_COLOR_INVERSE)
                bColorInverse = !bColorInverse;
            if (firstPressID == GUI_BUTTON::TOGGLE_LOCAL_COLOR_SHIFT)
                bColorShift = !bColorShift;
            if (firstPressID == GUI_BUTTON::TOGGLE_LOCAL_COLOR_BW)
                bColorBW = !bColorBW;
        }

        // Opacity control over shader effects
        effectJoJo->setOpacity(bColorJoJo ? fmin(effectJoJo->getOpacity() + delta, 1.0f) : fmax(effectJoJo->getOpacity() - delta, 0.0f));
        effectInverse->setOpacity(bColorInverse ? fmin(effectInverse->getOpacity() + delta, 1.0f) : fmax(effectInverse->getOpacity() - delta, 0.0f));
        effectShift->setOpacity(bColorShift ? fmin(effectShift->getOpacity() + delta, 1.0f) : fmax(effectShift->getOpacity() - delta, 0.0f));
        effectBW->setOpacity(bColorBW ? fmin(effectBW->getOpacity() + delta, 1.0f) : fmax(effectBW->getOpacity() - delta, 0.0f));
    }

    engine->destroy();
    return 0;
}

// Our fragment OpenGL shaders for effects.
// Screen here is represented in 0-1 on each axis inside texCoord
// There is no vertex shaders because we use the default one

const std::string fragmentInverseShader = "#version 410 core\n"
                                    "out vec4 fragColor;\n"
                                    "in vec2 texCoord;\n"
                                    "uniform sampler2D t;\n"
                                    "void main() {\n"
                                    "  vec4 color = texture(t, texCoord);\n"
                                    "  fragColor = vec4(1.0 - color.x, 1.0 - color.y, 1.0 - color.z, color.w);\n"
                                    "}\n";

const std::string fragmentShiftShader = "#version 410 core\n"
                                  "out vec4 fragColor;\n"
                                  "in vec2 texCoord;\n"
                                  "uniform sampler2D t;\n"
                                  "void main() {\n"
                                  "  vec4 color = texture(t, texCoord);\n"
                                  "  float m = 1.0 - (abs(0.5 - texCoord.x) + abs(0.5 - texCoord.y));\n"
                                  "  vec4 leftColor = texture(t, texCoord + vec2(-0.003 * m, 0));\n"
                                  "  vec4 rightColor = texture(t, texCoord + vec2(0.003 * m, 0));\n"
                                  "  fragColor = vec4(rightColor.x, leftColor.y, color.z, color.w);\n"
                                  "}\n";

const std::string fragmentBWShader = "#version 410 core\n"
                               "out vec4 fragColor;\n"
                               "in vec2 texCoord;\n"
                               "uniform sampler2D t;\n"
                               "void main() {\n"
                               "  vec4 color = texture(t, texCoord);\n"
                               "  float m = (color.x + color.y + color.z) / 3.0;\n"
                               "  fragColor = vec4(m, m, m, 1.0);\n"
                               "}\n";

const std::string fragmentJoJoShader = "#version 410 core\n"
                                 "out vec4 fragColor;\n"
                                 "in vec2 texCoord;\n"
                                 "uniform sampler2D t;\n"
                                 "void main() {\n"
                                 "  vec4 color = texture(t, texCoord);\n"
                                 "  fragColor = vec4(color.z * 1.1, color.x * 0.8, color.y * 1.2, 1.0);\n"
                                 "}\n";
