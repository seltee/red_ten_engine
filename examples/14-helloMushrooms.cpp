// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "../src/rtengine.h"
#include <math.h>

class Land : public Actor
{
public:
    Land()
    {
        registerClassName("Land");
    }

    void onSpawned()
    {
        auto landComponent = createComponent<ComponentMesh>();
        landComponent->setMesh(landMesh);
        landComponent->setShader(landShader);
        landComponent->addShapeGeometry(landMesh->getGeometry());
        landComponent->transform.setScale(Vector3(0.01f));
    }

    static MeshStatic *landMesh;
    static PhongShader *landShader;
};
PhongShader *Land::landShader = nullptr;
MeshStatic *Land::landMesh = nullptr;

class Mushroom : public Actor
{
public:
    Mushroom()
    {
        registerClassName("Mushroom");
    }

    void onSpawned()
    {
        mushroomComponent = createComponent<ComponentMesh>();
        mushroomComponent->setMesh(mushroomMesh);
        mushroomComponent->setShader(mushroomShader);
        mushroomComponent->transform.setScale(Vector3(0.005f));
        mushroomComponent->addShapeGeometry(mushroomMesh->getGeometry());
    }

    void onProcess(float delta)
    {
        size = fminf(size + delta * 0.5f, 1.0f);
        mushroomComponent->transform.setScale(Vector3(size * 0.01f));
        if (flyingAway)
        {
            flyingSpeed += delta;
            Vector3 v = transform.getPosition();
            transform.setPosition(v.x, v.y + flyingSpeed, v.z);
            if (v.y > 40.0f)
                destroy();
        }
    }

    void shootIt()
    {
        flyingAway = true;
    }

    float size = 0.0f;
    float flyingSpeed = 0.0f;
    ComponentMesh *mushroomComponent;
    bool flyingAway = false;

    static MeshStatic *mushroomMesh;
    static PhongShader *mushroomShader;
};
PhongShader *Mushroom::mushroomShader = nullptr;
MeshStatic *Mushroom::mushroomMesh = nullptr;

class Pointer : public ActorPawn
{
public:
    Pointer() : ActorPawn(this)
    {
        auto mousePressInput = registerButtonCallback(&Pointer::mousePress);
        mousePressInput->addMouseButtonBinding(InputTypeMouse::LEFT_BUTTON, 1.0f);

        auto mouseMoveInput = registerAxisCallback(&Pointer::mouseMove);
        mouseMoveInput->addMouseMoveBinding(1.0f);
    }

    void setup(Camera *camera)
    {
        this->camera = camera;
    }

    void mousePress(InputType type, int deviceIndex, int index, bool state)
    {
        if (state && camera)
        {
            PointWithDirection pointWithDirection = camera->screenToWorld(mouseX, mouseY);
            PhysicsBodyPoint point;
            if ((layer)->castRaySingleCollision(Segment(pointWithDirection.vPosition, pointWithDirection.vPosition + pointWithDirection.vDirection * 100.0f), point))
            {
                if (point.actor->implements("Land"))
                {
                    auto mushroom = (layer)->createActor<Mushroom>();
                    mushroom->transform.setPosition(point.point.x, point.point.y, point.point.z);
                    mushroom->transform.setRotation(Vector3(0.0f, randf(0.0f, CONST_PI * 4.0f), 0.0f));
                }
                if (point.actor->implements("Mushroom"))
                {
                    ((Mushroom *)point.actor)->shootIt();
                }
            }
        }
    }

    void mouseMove(InputType type, int deviceIndex, int index, float state)
    {
        if (index == (int)InputTypeMouseMove::MOVE_HORIZONTAL)
            mouseX = state;
        if (index == (int)InputTypeMouseMove::MOVE_VERTICAL)
            mouseY = state;
    }

    Camera *camera = nullptr;
    float mouseX = 0.0f, mouseY = 0.0f;
};

APPMAIN
{
    // Engine setup
    auto engine = RTEngine::getInstance();

    // We need view controller to get resolution
    auto viewController = engine->getViewController();

    // Set fullscreen through configuration controller
    auto configController = engine->getConfigController();
    auto config = configController->getConfig();
    if (!config->isLoaded())
    {
        // If configuration is new we setup high quality in window
        config->setupByQuality(RenderQuality::High);
        config->setWindowWidth(viewController->getPrimaryScreenWidth() * 0.8f);
        config->setWindowHeight(viewController->getPrimaryScreenHeight() * 0.8f);
        config->setFullscreenState(false);
    }

    // View setup
    auto view = viewController->createView("Example \"14. Hello Mushroom\"");

    // Stage setup
    auto stageController = engine->getStageController();
    auto stage = stageController->createStage("Hello Mushroom");

    // Layers and camera setup
    auto layerActors = stage->createLayerActors("Hello Mushroom", 0);
    layerActors->setAmbientColor(0.8f, 0.8f, 0.8f);
    layerActors->enablePhisics(Vector3(0.0f, 0.0f, 0.0f), 0.1f, 200);

    auto cameraActor = layerActors->createActor<ActorCamera>();
    auto camera = cameraActor->setupPerspectiveCamera();
    camera->setWidthBasedResolution(1280);
    cameraActor->transform.setPosition(6.0f, 10.0f, 6.0f);
    cameraActor->lookAt(0.0f, -2.0f, 0.0f);

    // Resources
    auto resourceController = engine->getResourceController();
    auto landAlbedoTexture = resourceController->addImage("./data/3d/land_albedo.jpg")->getAsTexture();
    auto landNormalTexture = resourceController->addImage("./data/3d/land_normal.jpg")->getAsTexture();
    auto mushroomAlbedoTexture = resourceController->addImage("./data/3d/mushroom_albedo.jpg")->getAsTexture();

    // our tower
    auto landMesh = resourceController->addMesh("./data/3d/land.fbx")->getAsMeshStatic();
    auto mushroomMesh = resourceController->addMesh("./data/3d/mushroom.fbx")->getAsMeshStatic();

    Land::landMesh = landMesh;
    Land::landShader = new PhongShader();
    Land::landShader->setTexture(TextureType::Albedo, landAlbedoTexture);
    Land::landShader->setTexture(TextureType::Normal, landNormalTexture);

    Mushroom::mushroomMesh = mushroomMesh;
    Mushroom::mushroomShader = new PhongShader();
    Mushroom::mushroomShader->setTexture(TextureType::Albedo, mushroomAlbedoTexture);

    // land
    layerActors->createActor<Land>();

    // Sun with shadow casting
    auto sun = layerActors->createActor<Actor>();
    auto sunComponent = sun->createComponent<ComponentLight>();
    sunComponent->setupSunLight(Vector3(-1.0f, 2.0f, 1.0f), Vector3(2.5f, 2.5f, 2.5f), true);

    // Pointer to receive input and apply actions
    auto pointer = layerActors->createActor<Pointer>();
    pointer->setup(camera);

    int created = 0;
    while (created < 40)
    {
        float radius = randf(1.0f, 10.0f);
        float angle = randf(0.0f, CONST_PI * 4.0f);
        float x = sin(angle) * radius;
        float z = cos(angle) * radius;

        PhysicsBodyPoint point;
        if (layerActors->castRaySingleCollision(Segment(Vector3(x, 10.0f, z), Vector3(x, -20.0f, z)), point))
        {
            if (point.actor->implements("Land"))
            {
                auto mushroom = layerActors->createActor<Mushroom>();
                mushroom->transform.setPosition(x, point.point.y, z);
                mushroom->transform.setRotation(Vector3(0.0f, randf(0.0f, CONST_PI * 4.0f), 0.0f));
            }
        }
        created++;
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