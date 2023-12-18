// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "../src/rtengine.h"
#include <math.h>

class Star : public Actor
{
public:
    Star() : Actor()
    {
        registerClassName("Star");
        sprite = createComponent<ComponentSprite>();
        sprite->setTexture(starTexture);
        sprite->colorMode = ComponentColorMode::Addition;
    }
    void onProcess(float delta)
    {
        life -= delta;
        if (life < 0.0f)
            destroy();
        sprite->setOpacity(fmaxf(0.0f, life));
    }

    static Texture *starTexture;

protected:
    ComponentSprite *sprite;
    float life = 1.0f;
};
Texture *Star::starTexture = nullptr;

// All the bytemap magic happens in the Actor
// Btw, Jojo belongs to it's owner and has no relation to Red Ten Roads
class JoJo : public Actor
{
public:
    JoJo() : Actor()
    {
        registerClassName("JoJo");
        transform.setScale(0.8);
        transform.setPosition(0.0f, -50.0f);
        sprite = createComponent<ComponentSprite>();
        sprite->setTexture(jojoImage->getAsTexture());
    }

    void onProcess(float delta)
    {
        counter += delta;
        while (counter > 0.02f && layerActors)
        {
            // Here the magic goes
            counter -= 0.02f;
            unsigned char probe = 0;
            int randomX = 0, randomY = 0;

            // first we need to take random point inside the image, that actually has pixel.
            // So we will use this position to put a star on it. This way stars will appear only on JoJo
            while (probe < 128)
            {
                randomX = rand() % jojoImage->getWidth();
                randomY = rand() % jojoImage->getHeight();
                probe = jojoImage->getProbeBytemapAlpha(randomX, randomY);
            }

            auto star = layerActors->createActor<Star>();

            // local space operates points position between 0 and 1, it's already stores texture size transofrmation
            // We need to create vector that holds 0 to 1 positions of our randoms. So just divide it on texture sizes
            Vector4 v(
                (float)randomX / (float)jojoImage->getWidth(),
                (float)randomY / (float)jojoImage->getHeight(),
                0.0f,
                1.0f);

            // Now we need to move the vector into local space of the component
            v = sprite->getLocalspaceMatrix() * v;

            // And then to local space of the actor
            v = (*transform.getModelMatrix()) * v;

            // That's it. v now holds the position on the actual scene and we can put our star there!
            star->transform.setPosition(v.x, v.y);
        }
    }

    static ResourceImage *jojoImage;
    float counter = 0.0f;
    LayerActors *layerActors = nullptr;

protected:
    ComponentSprite *sprite;
};
ResourceImage *JoJo::jojoImage = nullptr;

// Bytemap is an interesting way of creating effects, where you can detect existance of pixels on certain points of your actor
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
    auto view = viewController->createView("Example \"6. Hello Bytemap\"");

    // Stage setup
    auto stageController = engine->getStageController();
    auto stage = stageController->createStage("Hello Bytemap");

    // Layers setup
    auto layerActors = stage->createLayerActors("Hello Bytemap", 0);
    layerActors->enableSorting();

    // Camera
    auto camera = layerActors->createActor<ActorCamera>();
    camera->setupOrtoCamera()->setWidthBasedResolution(1280);

    // Textures and images setup
    auto resourceController = engine->getResourceController();

    // Second parameter makes image to create bytemap, Alpha means it will store only alpha per pixel
    // Third parameter allows you to reduce the size of the bytemap: width / scale, height / scale
    // This helps to save up some ram if needed and lower accuracy if acceptable
    JoJo::jojoImage = resourceController->addImage("./data/jojo.png", ByteMap::Alpha, 4);
    
    // Other images are used as textures
    Star::starTexture = resourceController->addImage("./data/star.png")->getAsTexture();
    auto background = resourceController->addImage("./data/background.jpg")->getAsTexture();

    // Just a background for better look
    auto backgroundActor = layerActors->createActor<Actor>();
    backgroundActor->transform.setScale(1.5f);
    auto backgroundActorSprite = backgroundActor->createComponent<ComponentSprite>();
    backgroundActorSprite->setTexture(background);

    // Sparks creation happens in the JoJo actor class
    auto jojo = layerActors->createActor<JoJo>();
    jojo->layerActors = layerActors;

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