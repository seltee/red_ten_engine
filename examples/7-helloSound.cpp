// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "../src/rtengine.h"
#include <math.h>

// Sound player could be component for an actor, same is sprite component
// It allows sound to be panned depending on actor's position
// Btw, Jojo belongs to it's owner and has no relation to Red Ten Roads
class JoJo : public ActorPawn
{
public:
    JoJo() : ActorPawn(this)
    {
        registerClassName("JoJo");
        transform.setScale(0.8);
        // Note: here we use z position of an actor, because that's the way to control sound pan in 3d space
        transform.setPosition(0.0f, -50.0f, frontDistance);
        jojoTexture->setMakeBytemapAlpha(true);
        jojoTexture->setBytemapScale(4);
        jojoTexture->reload();

        sprite = createComponent<ComponentSprite>();
        sprite->setTexture(jojoTexture);

        auto inputX = registerAxisCallback(&JoJo::controlX);
        inputX->addKeyboardBinding(KeyboardCodes::D, 400.0f);
        inputX->addKeyboardBinding(KeyboardCodes::A, -400.0f);
        inputX->addKeyboardBinding(KeyboardCodes::RIGHT, 400.0f);
        inputX->addKeyboardBinding(KeyboardCodes::LEFT, -400.0f);

        // This is our sound player that playes the song
        ComponentSoundPlayer *player = createComponent<ComponentSoundPlayer>();
        player->setSound(jojoSong);
        player->playLoop();
    }

    void onProcess(float delta)
    {
        transform.translate(moveX * delta, 0, 0);
        auto position = transform.getPosition();
        if (position.x > 600.0f)
            transform.setPosition(600.0f, position.y, frontDistance);

        if (position.x < -600.0f)
            transform.setPosition(-600.0f, position.y, frontDistance);
    }

    void controlX(InputType type, int deviceIndex, int index, float axis)
    {
        moveX = axis;
    }

    static Texture *jojoTexture;
    static Sound *jojoSong;

    float counter = 0.0f;
    float moveX = 0.0f;
    const float frontDistance = -90;

protected:
    ComponentSprite *sprite;
};
Texture *JoJo::jojoTexture = nullptr;
Sound *JoJo::jojoSong = nullptr;

int main()
{
    // Engine setup
    auto engine = RTEngine::createInstance();

    // Set fullscreen through configuration controller
    auto configController = engine->getConfigController();
    auto config = configController->getConfig();
    config->setWindowWidth(1280);
    config->setWindowHeight(800);
    config->setFullscreenState(false);

    // View setup
    auto viewController = engine->getViewController();
    auto view = viewController->createView("Example \"7. Hello Sound\"");

    // Stage setup
    auto stageController = engine->getStageController();
    auto stage = stageController->createStage("Hello Sound");

    // Layers and camera setup
    auto layerActors = stage->createLayerActors("Hello Sound", 0);
    auto camera = layerActors->createActor<ActorCamera>();
    camera->setupOrtoCamera()->setWidthBasedResolution(1280);

    // Textures and sound setup
    auto resourceController = engine->getResourceController();
    JoJo::jojoTexture = resourceController->addTexture("./data/jojo.png");
    auto background = resourceController->addTexture("./data/background.jpg");

    JoJo::jojoSong = resourceController->addSound("./data/sound.wav");
    // Note that only mono sounds can be panned. Stereo sounds always being played as is
    // But we can force mono
    JoJo::jojoSong->setForceMono(true);

    // Just a background for better look
    auto backgroundActor = layerActors->createActor<Actor>();
    backgroundActor->transform.setScale(1.5f);
    auto backgroundActorSprite = backgroundActor->createComponent<ComponentSprite>();
    backgroundActorSprite->setTexture(background);

    layerActors->createActor<JoJo>();

    while (!engine->isTerminationIntended())
    {
        float delta = engine->syncFrame();
        viewController->processEvents();
        stage->process(delta);
        stage->present(view);
    }
}