// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "../src/rtengine.h"
#include <math.h>

class CrateCollection : public Actor, public WithMeshMaker, public WithRepository
{
public:
    CrateCollection() : Actor()
    {
        registerClassName("CrateCollection");
    }

    void setup(int amount, float distance)
    {
        if (!crateMesh)
        {
            crateMesh = meshMaker->createBox({1.0f, 1.0f, 1.0f});
        }

        if (!crateShader)
        {
            crateShader = new PhongShader();
            auto crateAlbedoTexture = repository->addTexture("./data/crate.jpg");
            crateShader->setTexture(TextureType::Albedo, crateAlbedoTexture);
        }

        for (int i = 0; i < amount; i++)
        {
            auto step = CONST_PI2 / (float)amount;
            auto crateComponent = createComponent<ComponentMesh>();
            crateComponent->setMesh(crateMesh);
            crateComponent->setShader(crateShader);

            crateComponent->transform.setPosition(sin(float(i) * step) * distance, 0.0f, cos(float(i) * step) * distance);
        }
    }

    void onProcess(float delta)
    {
    }

    static PhongShader *crateShader;
    static Mesh *crateMesh;
};
PhongShader *CrateCollection::crateShader = nullptr;
Mesh *CrateCollection::crateMesh = nullptr;

void recreate(LayerActors *layer, int layers)
{
    for (auto &it : *layer->getActorsList())
    {
        if (it->implements("CrateCollection"))
            it->destroy();
    }

    for (int i = 0; i < layers; i++)
    {

        auto collection = layer->createActor<CrateCollection>();
        collection->setup((i == 0) ? 1 : i * 9, i * 1.8f);
    }
}

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

    // View setup
    auto viewController = engine->getViewController();
    auto view = viewController->createView("Example \"2. Hello Actors\"");

    // Stage setup
    auto stageController = engine->getStageController();
    auto stage = stageController->createStage("Hello Actors");

    // Layers and camera setup
    auto layerActors = stage->createLayerActors("Hello Actors", 0);

    auto camera = layerActors->createActor<ActorCamera>();
    camera->setupPerspectiveCamera()->setWidthBasedResolution(1280);

    // Profiler setup
    // Profiler is tool to measure different systems for how much time they take to process
    // Engine has it's own entries in different places, but you also can add your own
    // Check log.txt or console after running your app, profiler will write all the info there
    auto profilerController = engine->getProfilerController();
    profilerController->enableTrackingAvgSecond(3);

    // This app tracks every profiler new entry and increases skin difficulty
    int currentEntry = profilerController->getRecordNumber();

    auto logController = engine->getLogController();
    logController->logff("Entry %i\n", currentEntry);

    int layer = 1;
    recreate(layerActors, layer);

    camera->transform.setPosition(1.0f * layer, 2.0f * layer, 2.0f * layer);
    camera->lookAt(0.0f, 0.0f, 0.0f);
    while (!engine->isTerminationIntended())
    {
        float delta = engine->syncFrame();

        if (currentEntry != profilerController->getRecordNumber())
        {
            currentEntry = profilerController->getRecordNumber();
            logController->logff("Entry %i\n", currentEntry);

            layer++;
            recreate(layerActors, layer);

            camera->transform.setPosition(1.0f * layer, 1.6f * layer, 2.0f * layer);
            camera->lookAt(0.0f, -1.0f, 0.0f);
        }

        viewController->processEvents();
        stage->process(delta);
        stage->present(view);
    }

    return 0;
}