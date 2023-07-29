// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "../src/rtengine.h"
#include "helpers.h"
#include <math.h>

class Viewer : public Actor, public WithRepository
{
public:
    Viewer() : Actor()
    {
        registerClassName("Viewer");
    }

    void onSpawned()
    {
        auto cameraComponent = createComponent<ComponentCameraPerspective>();
        cameraComponent->getCamera()->setWidthBasedResolution(1280);
        cameraComponent->transform.setPosition(0.0f, 0.4f, 0.0f);

        Component *collisionComponent = createComponent<Component>();
        collisionComponent->addShapeCapsule(1.0f, 0.4f);

        setFrictionAndRestitution(0.7f, 0.3f);
        setPhysicsMotionType(MotionType::Dynamic);
    }

    static PhongShader *sphereShader;
    bool autoremove = true;
};

int main()
{
    // Engine setup
    auto engine = RTEngine::createInstance();

    // We need view controller to get resolution
    auto viewController = engine->getViewController();

    auto configController = engine->getConfigController();
    auto config = configController->getConfig();

    // Setup configuration
    config->setWindowWidth(viewController->getPrimaryScreenWidth() * 0.8f);
    config->setWindowHeight(viewController->getPrimaryScreenHeight() * 0.8f);
    config->setFullscreenState(false);

    // View setup
    auto view = viewController->createView("Example \"16. Hello FPV\"");

    // Stage setup
    auto stageController = engine->getStageController();
    auto stage = stageController->createStage("Hello FPV");

    // Resource controller
    auto resourceController = engine->getResourceController();

    // Actors' layer and physics
    auto layerActors = stage->createLayerActors("Hello 3D Layer", 0);
    layerActors->enablePhisics(Vector3(0.0f, -4.0f, 0.0f), 0.1f, 120);

    // Sun with shadow casting
    auto sun = layerActors->createActor<Actor>();
    auto sunComponent = sun->createComponent<ComponentLight>();
    sunComponent->setupSunLight(Vector3(-1.0f, 2.0f, 1.0f), Vector3(0.9f, 0.9f, 0.9f), true);

    ComponentMesh *componentMesh;

    // Create plain
    Vector3 plainNormal = glm::normalize(Vector3(0.0f, 1.0f, 0.0f));
    float plainDistance = -5.0f;
    auto plainBody = layerActors->getPhysicsWorld()->createPhysicsBody(new ShapePlain(plainNormal, plainDistance, layerActors->getPhysicsWorld()));
    plainBody->setStaticMotionType();
    plainBody->setRestitution(0.2f);
    plainBody->setFriction(0.7f);

    auto plainMesh = engine->getMeshMaker()->createPlane({25.0f, 25.0f}, 4.0f);
    auto plainTextureAlbedo = engine->getResourceController()->addTexture("./data/3d/pavement_albedo.jpg");
    auto plainTextureNormal = engine->getResourceController()->addTexture("./data/3d/pavement_normal.jpg");
    auto plainShader = new PhongShader();
    plainShader->setTexture(TextureType::Albedo, plainTextureAlbedo);
    plainShader->setTexture(TextureType::Normal, plainTextureNormal);

    auto plain = layerActors->createActor<Actor>();
    componentMesh = plain->createComponent<ComponentMesh>();
    componentMesh->setMesh(plainMesh);
    componentMesh->setShader(plainShader);
    plain->transform.setPosition(plainNormal * plainDistance);

    // Platform - temporary
    Actor *platform = layerActors->createActor<Actor>();
    auto platformShader = new PhongShader();

    auto platformAlbedoTexture = resourceController->addTexture("./data/3d/platform_albedo.jpg");
    platformShader->setTexture(TextureType::Albedo, platformAlbedoTexture);

    auto platformComponent = platform->createComponent<ComponentMesh>();
    auto platformMesh = resourceController->addMesh("./data/3d/platform.fbx");
    platformComponent->setMesh(platformMesh);
    platformComponent->setShader(platformShader);
    platformComponent->addShapeGeometry(platformMesh->getGeometry());

    platform->setFrictionAndRestitution(0.6f, 0.1f);
    platform->setPhysicsMotionType(MotionType::Static);
    platform->transform.setPosition(0.0f, -2.0f, 0.0f);

    Actor *spawn = layerActors->createActor<Viewer>();
    spawn->transform.setPosition(Vector3(0.0f, 4.0f, 0.0f));

    for (int i = 0; i < 60; i++)
    {
        Actor *spawn = nullptr;
        int sp = i % 4;
        if (sp == 0)
            spawn = layerActors->createActor<Sphere>();
        if (sp == 1 || sp == 2)
            spawn = layerActors->createActor<Crate>();
        if (sp == 3)
            spawn = layerActors->createActor<Crate>();

        if (spawn)
        {
            float x = randf(-6.0f, 6.0f);
            float y = randf(1.0f, 3.0f);
            float z = randf(-6.0f, 6.0f);

            spawn->transform.setPosition(x, y, z);
            spawn->transform.setRotation(Vector3(randf(-M_PI, M_PI), randf(-M_PI, M_PI), randf(-M_PI, M_PI)));
        }
    }

    while (!engine->isTerminationIntended())
    {
        float delta = engine->syncFrame();
        viewController->processEvents();

        stage->process(delta);
        stage->present(view);
    }
}