// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "../src/rtengine.h"
#include <math.h>

// To create actor's logic you need to inherit actor into your class then you will get an access to Actor's life cycle
class Bowl : public Actor
{
public:
    // Constuctor usually being used to create all of the components, but design of engine doesn't allow to
    // pass parameters here to your class. The usual way is to create setup function that will be called after the creation by your code
    Bowl() : Actor()
    {
        moveDirection = randf(0.0f, CONST_PI2);
        moveSpeed = randf(0.0f, 100.0f);
        life = 4.0f + randf(0.0f, 4.0f);

        sprite = createComponent<ComponentSprite>();
        sprite->setTexture(ballTexture);

        // All actors has transform property inside them that provides ability to set position, scale and rotation in 3d or 2d space
        // Z axis (Vector3(x, y, z)) controls the order in which sprites are rendered and represents depth if look from the camera
        // Sprites are rendered on the blending phase (everything that is not of Lit mixing type are on the blending phase)
        // If sorting is disabled - for blending phase engine sorts all entities by their depth
        // That's more an example, it's better to enable sorting for 2d layers - sorting works with disabled depth buffer
        // Without sorting if you have 2 sprites on the same depth they will glith through each other cause of depth buffer
        // Sorting shown in the future examples
        transform.setScale(0.5f);
        transform.setPosition(randf(-200.0f, 200.0f), randf(-200.0f, 200.0f), randf(0.0f, 50.0f));
    }

    // This is the standart actor lifecycle function called on all stage updates once per frame
    void onProcess(float delta)
    {
        Actor::onProcess(delta);
        transform.translate(sinf(moveDirection) * moveSpeed * delta, cosf(moveDirection) * moveSpeed * delta);

        // Actors don't have transparancy because they are abstraction, they don't determine how components should be drawn,
        // They only provide transformation to their components. But components can have a lot of parameters to control their presentation
        // Such as opacity
        sprite->setOpacity(life * 0.8f);

        // Life is for how long our actor will exist
        life -= delta;

        if (life < 0)
        {
            // Never delete actors manually, because Engine should handle creation and removing actors properly in specific stage of pipeline
            // After calling destroy your actor wont be immidietely removed, but will stop existing before rendering stage
            destroy();
        }
    }

    // Common way of providing textures to actors is just to use static functions
    // Or to provide resourceManager and load textures inside the constructor
    static Texture *ballTexture;

    // Other way exist - just use RTEngine::getInstance()->getResourceController();
    // From the resource controller actor can load it's resources by itself

protected:
    float moveDirection, moveSpeed, life;
    ComponentSprite *sprite;
};
Texture *Bowl::ballTexture = nullptr;

APPMAIN
{
    // Engine setup
    auto engine = RTEngine::getInstance();

    // Set windowed mode through configuration
    auto configController = engine->getConfigController();
    auto config = configController->getConfig();
    config->setWindowWidth(1280);
    config->setWindowHeight(800);
    config->setFullscreenState(false);

    // Enable antialiasing
    config->setAnialiasing(AntiAliasing::SSAA);

    // View setup
    auto viewController = engine->getViewController();
    auto view = viewController->createView("Example \"2. Hello Actors\"");

    // Stage setup
    auto stageController = engine->getStageController();
    auto stage = stageController->createStage("Hello Actors");

    // Layers and camera setup
    auto layerActors = stage->createLayerActors("Hello Actors", 0);
    auto camera = layerActors->createActor<ActorCamera>();
    camera->setupOrtoCamera()->setWidthBasedResolution(1280);

    // Textures setup
    auto resourceController = engine->getResourceController();
    Bowl::ballTexture = resourceController->addImage("./data/plate.png")->getAsTexture();

    float creationCounter = 0;
    while (!engine->isTerminationIntended())
    {
        float delta = engine->syncFrame();

        // This way we will create 300 sprites per second
        creationCounter += delta * 300.0f;
        while (creationCounter > 0.0f)
        {
            creationCounter -= 1.0f;
            layerActors->createActor<Bowl>();
        }

        viewController->processEvents();
        stage->process(delta);
        stage->present(view);
    }

    engine->destroy();
    return 0;
}