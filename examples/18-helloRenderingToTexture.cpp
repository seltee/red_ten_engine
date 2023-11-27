// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "../src/rtengine.h"
#include "helpers.h"
#include <math.h>

extern const char *fragmentBWShader;

// Black and wight effect class for screens
// Fragment shader code is in the end of this file
class BWEffect : public Effect
{
public:
    BWEffect(const char *fragmentShader) : Effect(fragmentShader)
    {
        timeUniform = getUniformLocation("fTime");
    }

    void process(float delta)
    {
        time += delta * 0.5f;
        use();
        provideFloatValue(timeUniform, 1, &time);
    }

    float time = 0.0f;
    int timeUniform = -1;
};

APPMAIN
{
    // Engine setup
    auto engine = RTEngine::getInstance();

    // We need view controller to get resolution
    auto viewController = engine->getViewController();

    auto configController = engine->getConfigController();
    auto config = configController->getConfig();

    // Setup configuration
    config->setWindowWidth(viewController->getPrimaryScreenWidth() * 0.8f);
    config->setWindowHeight(viewController->getPrimaryScreenHeight() * 0.8f);
    config->setFullscreenState(false);

    // View setup
    auto view = viewController->createView("Example \"18. Hello Rendering to texture\"");

    // Stage setup
    auto stageController = engine->getStageController();
    auto stage = stageController->createStage("Hello Rendering to texture");

    // Actors layer
    auto layerActors = stage->createLayerActors("Hello 3D Layer", 0);

    // BW effect layer
    auto effectLayer = stage->createLayerEffects("BW Effect Layer", 1);
    effectLayer->addEffect(new BWEffect(fragmentBWShader));

    // Main viewing camera
    auto mainCamera = layerActors->createActor<ActorCamera>();
    mainCamera->setupPerspectiveCamera()->setWidthBasedResolution(1280);
    mainCamera->transform.setPosition(2.0f, 4.0f, 2.0f);
    mainCamera->lookAt(0.0f, 0.0f, 0.0f);

    // Screens viewing camera
    auto screenCamera = layerActors->createActor<ActorCamera>();
    screenCamera->setupPerspectiveCamera()->setWidthBasedResolution(800);
    screenCamera->transform.setPosition(0.5f, 6.0f, 0.5f);
    screenCamera->lookAt(0.0f, 0.0f, 0.0f);

    // Render to texture camera
    auto renderToTextureCamera = layerActors->createActor<ActorCamera>();
    renderToTextureCamera->setupPerspectiveCamera()->setWidthBasedResolution(1280);

    // Sun with shadow casting
    auto sun = layerActors->createActor<Actor>();
    auto sunComponent = sun->createComponent<ComponentLight>();
    sunComponent->setupSunLight(Vector3(-1.0f, 2.0f, 1.0f), Vector3(0.9f, 0.9f, 0.9f), true);

    ComponentMesh *componentMesh;

    // Render target for texture rendering
    auto screenRenderTarget = viewController->createRenderTarget(800, 600, RenderQuality::SuperFast);

    // Create plain
    auto plainMesh = engine->getMeshMaker()->createPlane({20.0f, 20.0f}, 4.0f);
    auto plainTextureAlbedo = engine->getResourceController()->addTexture("./data/3d/pavement_albedo.jpg");
    auto plainTextureNormal = engine->getResourceController()->addTexture("./data/3d/pavement_normal.jpg");
    auto plainShader = new PhongShader();
    plainShader->setTexture(TextureType::Albedo, plainTextureAlbedo);
    plainShader->setTexture(TextureType::Normal, plainTextureNormal);

    auto plain = layerActors->createActor<Actor>();
    componentMesh = plain->createComponent<ComponentMesh>();
    componentMesh->setMesh(plainMesh);
    componentMesh->setShader(plainShader);
    plain->transform.setPosition(Vector3(0.0f, 0.0f, 0.0f));

    // Rotating boxes
    auto boxMesh = engine->getMeshMaker()->createBox({0.8f, 0.8f, 0.8f});
    auto boxTexture = engine->getResourceController()->addTexture("./data/crate.jpg");
    auto boxShader = new PhongShader();
    boxShader->setTexture(TextureType::Albedo, boxTexture);

    std::vector<Actor *> boxes;
    for (int i = 0; i < 7; i++)
    {
        auto box = layerActors->createActor<Actor>();
        componentMesh = box->createComponent<ComponentMesh>();
        componentMesh->setMesh(boxMesh);
        componentMesh->setShader(boxShader);
        boxes.push_back(box);
    }

    // Screens
    const float scale = 0.008f;
    auto screenMesh = engine->getMeshMaker()->createPlane({800.0f * scale, 600.0f * scale});
    auto screenShader = new PhongShader();

    screenShader->setTexture(TextureType::Albedo, screenRenderTarget->getResultTextureAsClass());

    const int screensAmount = 5;
    float angleBetween = CONST_PI2 / (float)screensAmount;
    float screenDistance = 8.5f;
    std::vector<Actor *> screens;
    for (int i = 0; i < 5; i++)
    {
        auto screen = layerActors->createActor<Actor>();
        componentMesh = screen->createComponent<ComponentMesh>();
        componentMesh->setMesh(screenMesh);
        componentMesh->setShader(screenShader);

        float angle = (float)i * angleBetween;
        screen->transform.setPosition(Vector3(sin(angle) * screenDistance, 2.0f, cos(angle) * screenDistance));
        screen->transform.setRotation(Vector3(CONST_PI / -2.0f, angle, 0));
    }

    // Main loop
    float rotationCounter = 0.0f;
    while (!engine->isTerminationIntended())
    {
        float delta = engine->syncFrame();
        viewController->processEvents();

        // Set our rotating boxes positions
        const float distance = 3.0f;
        float angleBetween = CONST_PI2 / boxes.size();
        float angle = rotationCounter;

        for (auto &it : boxes)
        {
            it->transform.setPosition(Vector3(sinf(angle) * distance, 0.5f, cosf(angle) * distance));
            angle += angleBetween;
        }

        // Update camera position
        const float cameraDistance = 6.0f;
        const float speed = 1.1f;
        mainCamera->transform.setPosition(Vector3(sinf(-angle * speed) * cameraDistance, 2.2f, cosf(-angle * speed) * cameraDistance));
        mainCamera->lookAt(0.0f, 0.9f, 0.0f);

        rotationCounter += delta * 0.15f;

        // First rendering our scene from the screen camera into the texture
        // Rendering pipeline is the same as with usual rendering
        // Just rendering target is different from the one that used by View
        screenCamera->setActive();
        effectLayer->setVisible(true);
        stage->present(screenRenderTarget);

        // Now with main camera render the scene as usual
        // resulting texture from the screenRenderTarget is already insude the material for plains
        mainCamera->setActive();
        effectLayer->setVisible(false);
        stage->process(delta);
        stage->present(view);
    }

    return 0;
}

const char *fragmentBWShader = "#version 410 core\n"
                               "out vec4 fragColor;\n"
                               "in vec2 texCoord;\n"
                               "uniform sampler2D t;\n"
                               "uniform float fTime;\n"
                               "void main() {\n"
                               "  vec4 color = texture(t, texCoord);\n"
                               "  float m = (color.x + color.y + color.z) / 3.0;\n"
                               "  float addIn = min(max(sin(texCoord.y * 12.0 + fTime), 0) * 10, 1.0) * 0.08;\n"
                               "  float cm = ((m * 1.4) * (m * 1.4)) / 1.2 + addIn;\n"
                               "  fragColor = vec4(cm, cm, cm, 1.0);\n"
                               "}\n";