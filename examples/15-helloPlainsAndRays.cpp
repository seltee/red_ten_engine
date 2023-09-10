// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "../src/rtengine.h"
#include "helpers.h"
#include <math.h>

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
    auto view = viewController->createView("Example \"15. Hello Plains and Rays\"");

    // Stage setup
    auto stageController = engine->getStageController();
    auto stage = stageController->createStage("Hello Plains and Rays");

    // Layers and camera setup
    // Also enabling physics for the layer
    auto layerActors = stage->createLayerActors("Hello 3D Layer", 0);
    layerActors->enablePhisics(Vector3(0.0f, -4.0f, 0.0f), 0.1f, 160);

    auto camera = layerActors->createActor<ActorCamera>();
    camera->setupPerspectiveCamera()->setWidthBasedResolution(1280);
    camera->transform.setPosition(2.0f, 7.0f, 8.0f);
    camera->lookAt(0.0f, 0.0f, 0.0f);

    // Sun with shadow casting
    auto sun = layerActors->createActor<Actor>();
    auto sunComponent = sun->createComponent<ComponentLight>();
    sunComponent->setupSunLight(Vector3(-1.0f, 2.0f, 1.0f), Vector3(0.9f, 0.9f, 0.9f), true);

    ComponentMesh *componentMesh;

    // Create plain
    Vector3 plainNormal = glm::normalize(Vector3(0.0f, 1.0f, 0.0f));
    float plainDistance = -3.0f;
    auto plainBody = layerActors->getPhysicsWorld()->createPhysicsBody(new ShapePlain(plainNormal, plainDistance, layerActors->getPhysicsWorld()));
    plainBody->setStaticMotionType();
    plainBody->setRestitution(0.2f);
    plainBody->setFriction(0.7f);

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
    plain->transform.setPosition(plainNormal * plainDistance + Vector3(0.0f, 0.0f, -4.0f));

    // Rotating boxes
    auto boxMesh = engine->getMeshMaker()->createBox({0.4f, 0.4f, 0.4f});
    auto boxTexture = engine->getResourceController()->addTexture("./data/crate.jpg");
    auto boxShader = new PhongShader();
    boxShader->setTexture(TextureType::Albedo, boxTexture);

    auto box1 = layerActors->createActor<Actor>();
    componentMesh = box1->createComponent<ComponentMesh>();
    componentMesh->setMesh(boxMesh);
    componentMesh->setShader(boxShader);

    auto box2 = layerActors->createActor<Actor>();
    componentMesh = box2->createComponent<ComponentMesh>();
    componentMesh->setMesh(boxMesh);
    componentMesh->setShader(boxShader);

    auto platform = layerActors->createActor<Platform>();
    platform->transform.setPosition(Vector3(0.0f, -3.0f, 0.0f));

    // spawn 1000 objects
    for (int i = 0; i < 1000; i++)
    {
        Actor *spawn = nullptr;
        int sp = i % 3;
        if (sp == 0)
            spawn = layerActors->createActor<Sphere>();
        if (sp == 1 || sp == 2)
            spawn = layerActors->createActor<Crate>();

        float x = (float)((i % 10) - 5) * 1.2f;
        float z = (float)(((i / 10) % 10) - 5) * 1.2f;
        float y = (float)(i / 100) * 1.2f + 1.0f;

        spawn->transform.setPosition(x, y, z);
    }

    float rotationCounter = 0.0f;
    float impulseCounter = 0.0f;
    while (!engine->isTerminationIntended())
    {
        float delta = engine->syncFrame();
        viewController->processEvents();

        // Set our rotating boxes positions && calc ray end and ray start vectors
        const float distance = 5.0f;
        rotationCounter += delta * 0.2f;
        Vector3 p1 = Vector3(sinf(rotationCounter) * distance, -2.0f, cosf(rotationCounter) * distance);
        Vector3 p2 = Vector3(-sinf(rotationCounter) * distance, -2.0f, -cosf(rotationCounter) * distance);
        box1->transform.setPosition(p1);
        box2->transform.setPosition(p2);

        // Cast ray 2 times per second
        impulseCounter += delta * 2.0f;
        if (impulseCounter > 1.0f)
        {
            impulseCounter -= 1.0f;
            auto result = layerActors->castRayCollision(Segment(p1, p2), true);
        }

        stage->process(delta);
        stage->present(view);
    }

    return 0;
}