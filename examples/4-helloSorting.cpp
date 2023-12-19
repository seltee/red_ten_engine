// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "../src/rtengine.h"
#include <math.h>

// Actor just draws ball or crate depending on how it was setted up and moves it in circle on every update
class Figure : public Actor
{
public:
    void setup(float circlePosition, bool useBall)
    {
        this->circlePosition = circlePosition;
        transform.setScale(0.5f);
        auto sprite = createComponent<ComponentSprite>();
        sprite->setTexture(useBall ? ballTexture : crateTexture);
    }

    void onProcess(float delta)
    {
        circlePosition += delta;
        float x = sinf(circlePosition) * 500.0f;
        float y = cosf(circlePosition) * 200.0f;
        transform.setPosition(x, y);

        // This parameter determines how the actor will be sorted on the screen before rendering
        zDepth = (int)(-y * 2.0f);
    }

    static Texture *ballTexture;
    static Texture *crateTexture;

protected:
    float circlePosition;
};
Texture *Figure::ballTexture = nullptr;
Texture *Figure::crateTexture = nullptr;

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

    // Enable antialiasing
    config->setAnialiasing(AntiAliasing::SSAA);

    // View setup
    auto viewController = engine->getViewController();
    auto view = viewController->createView("Example \"4. Hello Sorting\"");

    // Stage setup
    auto stageController = engine->getStageController();
    auto stage = stageController->createStage("Hello Sorting");

    // Ortographic camera in layer disables Z test in OpenGL. It means Z axis doesn't effect which sprite is on the top
    // But you can still sort sprites by enabling manual sort. This way actors will be sorted depending on their inner zIndex before rendering
    auto layerActors = stage->createLayerActors("Hello Sorting", 0);
    layerActors->enableSorting();

    auto camera = layerActors->createActor<ActorCamera>();
    camera->setupOrtoCamera()->setWidthBasedResolution(1280);

    // Textures setup
    auto resourceController = engine->getResourceController();
    Figure::ballTexture = resourceController->addImage("./data/plate.png")->getAsTexture();
    Figure::crateTexture = resourceController->addImage("./data/crate.jpg")->getAsTexture();

    // We will use fixed 20 actors
    // We also pass their starting angle to shape a circle from this actors
    const int figuresAmount = 20;
    float period = (CONST_PI * 2.0f) / static_cast<float>(figuresAmount);
    for (int i = 0; i < figuresAmount; i++)
    {
        auto figure = layerActors->createActor<Figure>();
        figure->setup((float)i * period, (i % 2) == 0);
    }

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