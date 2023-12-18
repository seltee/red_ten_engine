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

        createTower(0.0f, 0.0f, 0.0f);
        createTower(0.0f, 0.8f, CONST_PI);
        createTower(0.0f, -0.8f, CONST_PI);

        for (int i = 0; i < LIGHT_COUNT; i++)
        {
            light[i] = createComponent<ComponentLight>();
            light[i]->setupOmniLight(0.1f + randf(0.0f, 0.1f), Vector3(0.2f + randf(0.0f, 0.7f), 0.2f + randf(0.0f, 0.7f), 0.3f + randf(0.0f, 0.7f)));
        }
    }

    void createTower(float positionX, float positionZ, float rotation)
    {
        // FBX export from Blender sets 100.0f scale on meshes
        const float FBXSizeFix = 0.01f;
        auto towerComponent = createComponent<ComponentMesh>();
        towerComponent->setMesh(towerMesh);
        towerComponent->setShader(towerShader);
        towerComponent->transform.setScale(Vector3(0.2f * FBXSizeFix));
        towerComponent->transform.setPosition(positionX, 0.0f, positionZ);
        towerComponent->transform.setRotation(Vector3(0.0f, rotation, 0.0f));
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
    auto view = viewController->createView("Example \"12. Hello 3D\"");
    auto renderer = view->getRenderer();

    // Stage setup
    auto stageController = engine->getStageController();
    auto stage = stageController->createStage("Hello 3D");

    // Layers and camera setup
    auto layerActors = stage->createLayerActors("Hello 3D Layer", 0);

    // Gamma control helps lights to pop up and night to be more visually readable
    layerActors->setGamma(1.6f);

    auto camera = layerActors->createActor<ActorCamera>();
    camera->setupPerspectiveCamera()->setWidthBasedResolution(1280);

    auto layerGUI = stage->createLayerActors("Hello Gui Layer", 1);
    auto guiCamera = layerGUI->createActor<ActorCamera>();
    guiCamera->setupOrtoCamera()->setWidthBasedResolution(1280);

    // Resources
    auto resourceController = engine->getResourceController();
    auto concreteAlbedoTexture = resourceController->addImage("./data/3d/concrete_albedo.jpg")->getAsTexture();
    auto concreteNormalTexture = resourceController->addImage("./data/3d/concrete_normal.jpg")->getAsTexture();

    auto towerAlbedoTexture = resourceController->addImage("./data/3d/tower_albedo.png")->getAsTexture();
    auto towerEmissionTexture = resourceController->addImage("./data/3d/tower_emission.png")->getAsTexture();
    auto towerNormalTexture = resourceController->addImage("./data/3d/tower_normal.png")->getAsTexture();

    // our floor
    auto plainMesh = engine->getMeshMaker()->createPlane({1.0f, 1.0f}, 3.0f);

    // our tower
    auto towerMesh = resourceController->addMesh("./data/3d/tower.fbx")->getAsMeshStatic();

    Town::floorMesh = plainMesh;
    Town::towerMesh = towerMesh;

    Town::floorShader = renderer->createPhongShader();
    Town::floorShader->setTexture(TextureType::Albedo, concreteAlbedoTexture);
    Town::floorShader->setTexture(TextureType::Normal, concreteNormalTexture);

    Town::towerShader = renderer->createPhongShader();
    Town::towerShader->setTexture(TextureType::Albedo, towerAlbedoTexture);
    Town::towerShader->setTexture(TextureType::Emission, towerEmissionTexture);
    Town::towerShader->setTexture(TextureType::Normal, towerNormalTexture);

    // town
    auto town = layerActors->createActor<Town>();
    town->transform.setScale(2.0f, 2.0f, 2.0f);

    // Sun with shadow casting
    auto sun = layerActors->createActor<Actor>();
    auto sunComponent = sun->createComponent<ComponentLight>();
    sunComponent->setupSunLight(Vector3(0.3f, 0.3f, 0.6f), true);
    sunComponent->transform.setPosition(Vector3(-1.0f, 1.0f, -0.5f));

    float cameraRotation = 0.0f;
    float sunRotation = 0.0f;

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
        sunRotation += delta * 0.1f;

        const float cameraDistance = 4.8f;
        const float cameraHeight = 3.6f;
        camera->transform.setPosition(sinf(cameraRotation) * cameraDistance, cameraHeight, cosf(cameraRotation) * cameraDistance);
        camera->lookAt(0.0f, 0.1f, 0.0f);

        float effectiveLight = fmaxf(sinf(sunRotation), 0.0f);
        layerActors->setAmbientColor(0.03f + effectiveLight * 0.42f, 0.03f + effectiveLight * 0.42f, 0.045f + effectiveLight * 0.4f);
        sunComponent->setupSunLight(
            Vector3(0.7f + (1.0f - effectiveLight) * 0.4f, 0.7f, 0.7f) * effectiveLight * 3.0f,
            true);
        sunComponent->transform.setPosition(Vector3(cosf(sunRotation) + 0.5f, sinf(sunRotation), cosf(sunRotation)));

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