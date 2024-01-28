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
    auto view = viewController->createView("Example \"23. Hello Texture Drawing\"");

    // Stage setup
    auto stageController = engine->getStageController();
    auto stage = stageController->createStage("Hello Texture Drawing");

    // Actors layer
    auto layerActors = stage->createLayerActors("Hello 3D Layer", 0);

    // Main viewing camera
    auto mainCamera = layerActors->createActor<ActorCamera>();
    mainCamera->setupPerspectiveCamera()->setWidthBasedResolution(1280);
    mainCamera->transform.setPosition(2.0f, 1.5f, 2.0f);
    mainCamera->lookAt(0.0f, 0.0f, 0.0f);

    auto colors = engine->getResourceController()->addImage("./data/colors.jpg")->getAsTexture();
    auto crate = engine->getResourceController()->addImage("./data/crate.jpg")->getAsTexture();
    auto plate = engine->getResourceController()->addImage("./data/plate.png")->getAsTexture();
    auto jojo = engine->getResourceController()->addImage("./data/jojo.png")->getAsTexture();

    auto boxMesh = engine->getMeshMaker()->createBox({1.6f, 1.6f, 1.6f});

    auto boxTexture = view->getRenderer()->createTextureEditable(512, 512);
    boxTexture->drawImage(colors, Vector2(0.0f, 0.0f));
    boxTexture->drawImage(crate, Vector2(0.0f, 0.0f));
    boxTexture->drawImage(crate, Vector2(0.5f, 0.5f));
    boxTexture->drawImage(plate, Vector2(0.0f, 0.5f));
    boxTexture->drawImage(plate, Vector2(0.5f, 0.0f));
    boxTexture->drawImage(jojo, Vector2(0.5f, 0.45f), Vector2(0.55f, 0.55f), Vector2(0.5f, 0.5f), -0.3f);

    auto boxShader = view->getRenderer()->createPhongShader();
    boxShader->setTexture(TextureType::Albedo, boxTexture);

    auto box = layerActors->createActor<Actor>();
    auto componentMesh = box->createComponent<ComponentMesh>();
    componentMesh->setMesh(boxMesh);
    componentMesh->setShader(boxShader);

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
