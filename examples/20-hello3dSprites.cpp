// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "../src/rtengine.h"
#include "helpers.h"
#include <math.h>

class Explosion : public Actor, public WithRepository, public WithRenderer
{
public:
    Explosion()
    {
        if (!shader)
        {
            auto spriteTextureAlbedo = repository->addImage("./data/explosion.png")->getAsTexture();

            shader = getRenderer()->createPhongShader();
            shader->setTexture(TextureType::Albedo, spriteTextureAlbedo);
            shader->setTexture(TextureType::Emission, spriteTextureAlbedo);
            shader->setTexture(TextureType::Shadow, spriteTextureAlbedo);
        }

        if (!spriteMesh)
        {
            spriteMesh = RTEngine::getInstance()->getMeshMaker()->createPlane({4.0f, 4.0f}, 1.0f);
        }

        expComponent = createComponent<ComponentMesh>();
        expComponent->setMesh(spriteMesh);
        expComponent->setShader(shader);
        expComponent->transform.setRotation(Vector3(CONST_PI / 2.0f, CONST_PI, 0.0f));
        expComponent->colorMode = ComponentColorMode::Alpha;
        expComponent->disableShadowCasting();

        uvParameter = shader->createShaderUVParameter();
        uvParameterData = Vector4(0.0f, 0.0f, frameSize, frameSize);
        uvParameter->set(1, value_ptr(uvParameterData));
        uvParameters[0] = uvParameter;

        expComponent->setShaderParameters(uvParameters, 1);

        shadowExpComponent = createComponent<ComponentMesh>();
        shadowExpComponent->setMesh(spriteMesh);
        shadowExpComponent->setShader(shader);
        shadowExpComponent->colorMode = ComponentColorMode::Alpha;

        shadowExpComponent->setShaderParameters(uvParameters, 1);
        shadowExpComponent->disableShapeCasting();

        omni = createComponent<ComponentLight>();
        omniLife = 1.0f;
    }

    ~Explosion()
    {
        shader->destroyShaderParameter(uvParameter);
    }

    void setSunPosition(Vector3 sunPosition)
    {
        this->sunPosition = sunPosition;
    }

    void onProcess(float delta)
    {
        frame += delta * 25.0f;
        omniLife = fmaxf(omniLife - delta * 4.0f, 0.0f);

        while (frame > 64.0f)
        {
            destroy();
            return;
        }

        int intFrame = static_cast<int>(frame);

        uvParameterData = Vector4(static_cast<float>(intFrame % 8) / 8.0f, static_cast<float>(intFrame / 8) / 8.0f, frameSize, frameSize);
        uvParameter->set(1, value_ptr(uvParameterData));

        auto camera = layer->getActiveCamera();
        Vector3 cameraPos = Vector3(0.0f);
        if (camera)
        {
            auto m = camera->getWorldModelMatrix();
            cameraPos = Vector3(m * Vector4(0.0f, 0.0f, 0.0f, 1.0f));
        }

        shadowExpComponent->lookAt(sunPosition, false);
        expComponent->lookAt(cameraPos, true);

        shadowExpComponent->setShadowShiftSize(uvParameterData);

        omni->setupOmniLight(4.0f, Vector3(fminf(omniLife * 6.0f, 3.0f), omniLife * 3.0f, omniLife * 3.0f), false);
    }

    const float frameSize = 1.0f / 8.0f;
    Vector3 sunPosition;
    Vector4 uvParameterData;
    ShaderParameter *uvParameter;
    ShaderParameter *uvParameters[1];
    ShaderParameter *uvParameterShadow;
    ShaderParameter *uvParametersShadow[1];
    ComponentMesh *expComponent;
    ComponentMesh *shadowExpComponent;
    ComponentLight *omni;
    float frame = 0.0f;
    float omniLife = 0.0f;

    static PhongShader *shader;
    static MeshStatic *spriteMesh;
};
PhongShader *Explosion::shader = nullptr;
MeshStatic *Explosion::spriteMesh = nullptr;

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
    auto view = viewController->createView("Example \"20. Hello 3d Sprites\"");

    // Resources
    auto resourceController = engine->getResourceController();

    // Stage setup
    auto stageController = engine->getStageController();
    auto stage = stageController->createStage("Hello 3d Sprites");

    // Actors layer
    auto layerActors = stage->createLayerActors("Hello 3D Layer", 0);

    // Main viewing camera
    auto mainCamera = layerActors->createActor<ActorCamera>();
    mainCamera->setupPerspectiveCamera()->setWidthBasedResolution(1280);
    mainCamera->transform.setPosition(2.0f, 4.0f, 2.0f);
    mainCamera->lookAt(0.0f, 0.0f, 0.0f);

    // Sun with shadow casting
    const Vector3 sunPosition = Vector3(-1.0f, 2.0f, 1.0f);
    auto sun = layerActors->createActor<Actor>();
    auto sunComponent = sun->createComponent<ComponentLight>();
    sunComponent->setupSunLight(Vector3(1.8f, 1.8f, 1.8f), true);
    sunComponent->transform.setPosition(sunPosition);

    ComponentMesh *componentMesh;

    // Create plain for ground
    auto plainMesh = engine->getMeshMaker()->createPlane({20.0f, 20.0f}, 4.0f);
    auto plainTextureAlbedo = resourceController->addImage("./data/3d/pavement_albedo.jpg")->getAsTexture();
    auto plainTextureNormal = resourceController->addImage("./data/3d/pavement_normal.jpg")->getAsTexture();
    auto plainShader = view->getRenderer()->createPhongShader();
    plainShader->setTexture(TextureType::Albedo, plainTextureAlbedo);
    plainShader->setTexture(TextureType::Normal, plainTextureNormal);

    auto plain = layerActors->createActor<Actor>();
    componentMesh = plain->createComponent<ComponentMesh>();
    componentMesh->setMesh(plainMesh);
    componentMesh->setShader(plainShader);
    plain->transform.setPosition(Vector3(0.0f, 0.0f, 0.0f));

    // Main loop
    float rotationCounter = 0.0f;
    float expLife = 0.0f;
    while (!engine->isTerminationIntended())
    {
        float delta = engine->syncFrame();
        viewController->processEvents();

        expLife -= delta;
        if (expLife < 0.0f)
        {
            expLife = randf(0.1f, 0.3f);
            auto explosion = layerActors->createActor<Explosion>();
            explosion->setSunPosition(sunPosition);
            explosion->transform.setPosition(Vector3(randf(-10.0f, 10.0f), 2.0f, randf(-10.0f, 10.0f)));
        }

        rotationCounter += delta * 0.12f;

        // Update camera position
        const float cameraDistance = 8.0f;
        mainCamera->transform.setPosition(Vector3(sinf(-rotationCounter) * cameraDistance, 3.8f, cosf(-rotationCounter) * cameraDistance));
        mainCamera->lookAt(0.0f, 0.5f, 0.0f);

        stage->process(delta);
        stage->present(view);
    }

    engine->destroy();
    return 0;
}
