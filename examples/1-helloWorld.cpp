// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "../src/rtengine.h"

APPMAIN
{
    // Engine instance is the source of controllers and settings of the engine
    // This instance is a singleton and the same one can be obtained from anywhere in the code
    auto engine = RTEngine::getInstance();

    // Set fullscreen through configuration controller
    auto configController = engine->getConfigController();
    auto config = configController->getConfig();
    config->setFullscreenState(true);

    // Provides ability to create views, also stores them and update
    // Size of the view and fullscreen is being taken from config. If config has 0 width and height the maximum of the screen will be used
    auto viewController = engine->getViewController();
    auto view = viewController->createView("Example \"1. Hello World\"");

    // Stage contains layers, layers contain actors
    auto stageController = engine->getStageController();
    auto stage = stageController->createStage("Hello World");
    stage->setClearColor(Color(0.0f, 0.0f, 0.0f));

    // Layer with actors contain sprites and camera to view them
    // To make 2d games you should use ortographic camera that has no perspective distorsion
    // Second parameter is index and being used to sort the layers.
    // Higher the index more on top the layer is
    auto layerActors = stage->createLayerActors("Hello Actors", 0);
    auto camera = layerActors->createActor<ActorCamera>();

    // There are physical and logical resolutions exist in the engine.
    // Physical is the width and height of your view in pixels - the real screen user is seeing
    // Logical is dimensions where you draw. 
    // For example 640 width will mean that right side of your screen for this camera will always be at the position 640 regradless of the view resolution.
    // Height is calculated proportaionally to renderTarget of the camera (view has default render target representing the screen)
    // If you set height based resolution of 480 your logical screen height will always be 480
    camera->setupOrtoCamera()->setHeightBasedResolution(480);

    // To put sprite onto our stage layer we first need to load image for this sprite
    // Before you you use it for the first time it won't be loaded into the memory, but once you need it - image will be loaded and texture made
    auto resourceController = engine->getResourceController();
    auto helloImage = resourceController->addImage("./data/hello.png");

    // First we need to create Actor. Actor is representation of some affecting environment entity in the scene
    // It may contain to graphics and it's representation fully relying on the components inside
    auto sprite = layerActors->createActor<Actor>();

    // Entity without components rendering nothing. Components are the bridges between actor and various APIs, including graphics, sound, physics
    // ComponentSprite represents a texture with position that being rendered if visible
    // ResourceImage generates and caches the texture when you call getAsTexture, you don't need to actually store the texture separatelly - it will always return the same texture
    auto component = sprite->createComponent<ComponentSprite>();
    component->setTexture(helloImage->getAsTexture());

    // We want to put picture on the bottom of the screen and to avoid calculating shift depending on the size of the texture
    // We can simply move anchor drawing point from center to bottom
    // (second parameter is vertical axis Y, first is horizontal - 0.5 means sprite will stay in the center)
    component->setAnchor(0.5f, 0.0f);

    // Now we will set the position of our actor in the center-bottom of the screen. Our screen height is alaways 480,
    // but 0-0 position is actually the center of the screen, not the upper left corner as in many game engines redners in 2d space
    // So the bottom will be height / -2, bottom of Y axis is negative
    sprite->transform.setPosition(0.0f, -320.0f);

    while (!engine->isTerminationIntended())
    {
        // Sync frame calculates the delta between frames
        // You can use your own counter if you prefer, it wont break anything
        float delta = engine->syncFrame();
        // Process operating system events coming to all created windows
        viewController->processEvents();
        // Processes all layers, actors and physics if enabled
        stage->process(delta);
        // Renders all actor components on all layers
        stage->present(view);
    }

    engine->destroy();
    return 0;
}