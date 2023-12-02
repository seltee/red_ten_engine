// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "../src/rtengine.h"
#include <math.h>

#define LIGHT_COUNT 5
#define BUTTON_SWITCH_SHADOW_QUALITY 1
class Town : public Actor
{
public:
    Town()
    {
        auto floorComponent = createComponent<ComponentMesh>();
        floorComponent->setMesh(floorMesh);
        floorComponent->setShader(floorShader);
        floorComponent->transform.setScale(2.5f, 2.5f, 2.5f);

        auto towerComponent = createComponent<ComponentMesh>();
        towerComponent->setMesh(towerMesh);
        towerComponent->setShader(towerShader);
        towerComponent->transform.setScale(0.2f, 0.2f, 0.2f);

        for (int i = 0; i < LIGHT_COUNT; i++)
        {
            light[i] = createComponent<ComponentLight>();
            light[i]->setupOmniLight(0.2f + randf(0.0f, 0.1f), Vector3(0.3f + randf(0.0f, 0.7f), 0.3f + randf(0.0f, 0.7f), 0.3f + randf(0.0f, 0.7f)));
        }
    }

    void onProcess(float delta)
    {
        counter += delta * 0.3f;
        float step = (CONST_PI * 2.0f) / (float)LIGHT_COUNT;
        for (int i = 0; i < LIGHT_COUNT; i++)
        {
            float rotation = (float)i * step + counter;
            light[i]->transform.setPosition(Vector3(sinf(rotation) * 1.2f, 0.10f, cosf(rotation) * 1.2f));
        }
    }

    float counter = 0.0f;
    ComponentLight *light[LIGHT_COUNT];

    static Mesh *floorMesh;
    static MeshStatic *towerMesh;
    static PhongShader *towerShader;
    static PhongShader *floorShader;
};
PhongShader *Town::towerShader = nullptr;
PhongShader *Town::floorShader = nullptr;
Mesh *Town::floorMesh = nullptr;
MeshStatic *Town::towerMesh = nullptr;

class GUISimpleButton : public ActorGUIElement
{
public:
    GUISimpleButton() : ActorGUIElement()
    {
        registerClassName("Gui Button");
        setActiveArea(0.0f, 320.0f, -32.0f, 32.0f);

        textSprite = createComponent<ComponentText>();
        textSprite->setFont(font);
        textSprite->setText("Unknown Button");
        textSprite->setColor(200, 200, 200);
        textSprite->setAnchor(0, 0.5f);
    }

    void setText(std::string text)
    {
        textSprite->setText(text);
    }

    void onHover()
    {
        textSprite->setColor(255, 255, 255);
    }

    void onBlur()
    {
        textSprite->setColor(200, 200, 200);
    }

    static ResourceFont *font;

protected:
    ComponentText *textSprite;
    int color = 170;
};
ResourceFont *GUISimpleButton::font = nullptr;

APPMAIN
{
    // Engine setup
    auto engine = RTEngine::getInstance("ex12cfg.ini");

    // We need view controller to get resolution
    auto viewController = engine->getViewController();

    // Set fullscreen through configuration controller
    auto configController = engine->getConfigController();
    auto config = configController->getConfig();
    if (!config->isLoaded())
    {
        // If configuration is new we setup high quality in window
        config->setupByQuality(RenderQuality::High);
        config->setWindowWidth(viewController->getPrimaryScreenWidth() * 0.8f);
        config->setWindowHeight(viewController->getPrimaryScreenHeight() * 0.8f);
        config->setFullscreenState(false);
    }

    // View setup
    auto view = viewController->createView("Example \"19. Hello Animation\"");

    // Stage setup
    auto stageController = engine->getStageController();
    auto stage = stageController->createStage("Hello Animation");

    // Layers and camera setup
    auto layerActors = stage->createLayerActors("Hello Animation Layer", 0);
    layerActors->setAmbientColor(0.75f, 0.75f, 0.75f);
    auto camera = layerActors->createActor<ActorCamera>();
    camera->setupPerspectiveCamera()->setWidthBasedResolution(1280);

    auto layerGUI = stage->createLayerActors("Hello Gui Layer", 1);
    auto guiCamera = layerGUI->createActor<ActorCamera>();
    guiCamera->setupOrtoCamera()->setWidthBasedResolution(1280);

    // Resources
    auto resourceController = engine->getResourceController();
    auto concreteAlbedoTexture = resourceController->addImage("./data/3d/concrete_albedo.jpg")->getAsTexture();
    auto concreteNormalTexture = resourceController->addImage("./data/3d/concrete_normal.jpg")->getAsTexture();

    // our floor
    auto plainMesh = engine->getMeshMaker()->createPlane({1.0f, 1.0f}, 3.0f);

    auto floorShader = new PhongShader();
    floorShader->setTexture(TextureType::Albedo, concreteAlbedoTexture);
    floorShader->setTexture(TextureType::Normal, concreteNormalTexture);

    auto floorActor = layerActors->createActor<Actor>();
    auto floorComponent = floorActor->createComponent<ComponentMesh>();
    floorComponent->setMesh(plainMesh);
    floorComponent->setShader(floorShader);
    floorComponent->transform.setScale(5.0f, 5.0f, 5.0f);

    // our tower
    auto animatedMeshResource = resourceController->addMesh("./data/3d/figures.fbx");
    auto animatedMesh = animatedMeshResource->getAsMeshCompound();

    auto animMeshShader = new PhongShader();
    animMeshShader->setTexture(TextureType::Albedo, concreteAlbedoTexture);
    animMeshShader->setTexture(TextureType::Normal, concreteNormalTexture);

    auto animActor = layerActors->createActor<Actor>();
    animActor->transform.setScale(0.5f, 0.5f, 0.5f);
    auto animComponent = animActor->createComponent<ComponentAnimatedMesh>();
    animComponent->setMesh(animatedMesh);
    animComponent->setShader(animMeshShader);
    auto animator = animComponent->createAnimator(animatedMeshResource->getFirstAnimation());
    animator->play();

    // Sun with shadow casting
    auto sun = layerActors->createActor<Actor>();
    auto sunComponent = sun->createComponent<ComponentLight>();
    sunComponent->setupSunLight(Vector3(-1.0f, 1.0f, -0.5f), Vector3(0.55f, 0.55f, 0.5f), true);

    float cameraRotation = 0.0f;

    // Shadow quality switch GUI
    GUISimpleButton::font = resourceController->addFont("./data/BebasNeue-Regular.ttf", 32);
    auto newButton = layerGUI->createActor<GUISimpleButton>();
    newButton->transform.setPosition(-config->getWindowWidth() / 2 + 60, -config->getWindowHeight() / 2 + 94);
    newButton->setPressID(BUTTON_SWITCH_SHADOW_QUALITY);
    newButton->setText("Toggle shadow quality");

    auto shadowQualityActor = layerGUI->createActor<Actor>();
    shadowQualityActor->transform.setPosition(-config->getWindowWidth() / 2 + 60, -config->getWindowHeight() / 2 + 62);
    auto shadowQualityText = shadowQualityActor->createComponent<ComponentText>();
    shadowQualityText->setFont(GUISimpleButton::font);
    shadowQualityText->setText(std::string("Shadow quality: ") + Config::qualityToString(config->getShadowQuality()));
    shadowQualityText->setColor(170, 170, 170);
    shadowQualityText->setAnchor(0, 0.5f);

    // Update config file if something changed from the initial configuration during setup
    config->saveConfig();

    float cameraMovementDelta = 0.0f;

    while (!engine->isTerminationIntended())
    {
        float delta = engine->syncFrame();
        viewController->processEvents();

        cameraRotation += delta * 0.3f;

        const float cameraDistance = 4.8f;
        const float cameraHeight = 3.6f;
        camera->transform.setPosition(sinf(cameraRotation) * cameraDistance, cameraHeight, cosf(cameraRotation) * cameraDistance);
        camera->lookAt(0.0f, 0.0f, 0.0f);

        // Camera flying effect
        cameraMovementDelta += delta * 0.25f;
        camera->getCameraComponent()->transform.setPosition(Vector3(0.0f, sinf(cameraMovementDelta) * 0.4f, 0.0f));

        // Controlling the shadow quality
        int firstPressID;
        while ((firstPressID = ActorGUIElement::getFirstPressID()))
        {
            if (firstPressID == BUTTON_SWITCH_SHADOW_QUALITY)
            {
                if (config->getShadowQuality() == RenderQuality::SuperFast)
                    config->setShadowQuality(RenderQuality::Fast);
                else if (config->getShadowQuality() == RenderQuality::Fast)
                    config->setShadowQuality(RenderQuality::Balanced);
                else if (config->getShadowQuality() == RenderQuality::Balanced)
                    config->setShadowQuality(RenderQuality::High);
                else if (config->getShadowQuality() == RenderQuality::High)
                    config->setShadowQuality(RenderQuality::SuperFast);

                shadowQualityText->setText(std::string("Shadow quality: ") + Config::qualityToString(config->getShadowQuality()));
                configController->applyConfig();

                // Saved configuration will be automatically loaded on next run of the engine
                // In this case it will load the quality of shadows
                config->saveConfig();
            }
        }

        stage->process(delta);
        stage->present(view);
    }

    engine->destroy();
    return 0;
}