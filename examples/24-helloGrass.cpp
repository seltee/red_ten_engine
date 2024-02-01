// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
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
    auto view = viewController->createView("Example \"24. Hello Grass\"");

    // Resources
    auto resourceController = engine->getResourceController();

    // Stage setup
    auto stageController = engine->getStageController();
    auto stage = stageController->createStage("Hello Grass");

    // Actors layer
    auto layerActors = stage->createLayerActors("Hello 3D Layer", 0);

    // Main viewing camera
    auto mainCamera = layerActors->createActor<ActorCamera>();
    mainCamera->setupPerspectiveCamera()->setWidthBasedResolution(1280);
    mainCamera->transform.setPosition(1.5f, 2.2f, 1.5f);
    mainCamera->lookAt(0.0f, 0.0f, 0.0f);

    // Sun with shadow casting
    auto sun = layerActors->createActor<Actor>();
    auto sunComponent = sun->createComponent<ComponentLight>();
    sunComponent->setupSunLight(Vector3(0.4f, 0.4f, 0.4f), true);
    sunComponent->transform.setPosition(Vector3(-1.0f, 2.0f, 1.0f));

    // Ground with grass
    auto groundMesh = engine->getMeshMaker()->createPlane({7.0f, 7.0f}, 1.0f, 10);

    const int cellSize = 2048;
    unsigned char *grassData = new unsigned char[cellSize * cellSize * 4];
    for (int i = 0; i < cellSize * cellSize; i++)
    {
        grassData[i * 4 + 0] = 255;
        grassData[i * 4 + 1] = 255;
        grassData[i * 4 + 2] = 255;
        grassData[i * 4 + 3] = 0;
    }

    // r - height
    // g - cut off
    // b - color shade
    for (int i = 0; i < 400000; i++)
    {
        int x = rand() % cellSize;
        int y = rand() % cellSize;
        grassData[(x + y * cellSize) * 4 + 0] = (rand() % 128) + 128;
        grassData[(x + y * cellSize) * 4 + 2] = (rand() % 86) + 170;
        grassData[(x + y * cellSize) * 4 + 3] = 255;
    }

    auto groundTexture = resourceController->addImage("./data/grass_root.jpg")->getAsTexture();
    auto grassTexture = view->getRenderer()->createTexture(cellSize, cellSize, 4, grassData, false);

    auto planeShader = view->getRenderer()->createPhongShader();
    planeShader->setTexture(TextureType::Albedo, groundTexture);

    auto plane = layerActors->createActor<Actor>();
    plane->transform.setPosition(Vector3(-1.4f, 0, -1.4f));
    auto componentMesh = plane->createComponent<ComponentMesh>();
    componentMesh->setMesh(groundMesh);
    componentMesh->setShader(planeShader);

    auto componentShell = plane->createComponent<ComponentMeshShell>();
    componentShell->setMesh(groundMesh);
    componentShell->setDensityTexture(grassTexture);
    componentShell->setDistorsionTexture(resourceController->addImage("./data/distorsion.jpg")->getAsTexture());
    componentShell->setRootTexture(resourceController->addImage("./data/grass_root.jpg")->getAsTexture());
    componentShell->setTipTexture(resourceController->addImage("./data/grass_tip.jpg")->getAsTexture());
    componentShell->setProperties(12, 0.0035f, 0.03f);

    while (!engine->isTerminationIntended())
    {
        float delta = engine->syncFrame();
        viewController->processEvents();

        stage->process(delta);
        stage->present(view);
    }

    engine->destroy();
    return 0;
}
