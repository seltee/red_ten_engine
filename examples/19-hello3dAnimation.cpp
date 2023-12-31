// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "../src/rtengine.h"
#include <math.h>

class Ground : public Actor, WithRenderer
{
public:
    Ground()
    {
        auto resourceController = RTEngine::getInstance()->getResourceController();

        floorMesh = resourceController->addMesh("./data/3d/ground.fbx")->getAsMeshStatic();
        treeMesh = resourceController->addMesh("./data/3d/tree.fbx")->getAsMeshStatic();
        treeMeshLod = resourceController->addMesh("./data/3d/tree_lod.fbx")->getAsMeshStatic();

        floorShader = getRenderer()->createPhongShader();
        auto floorAlbedoTexture = resourceController->addImage("./data/3d/ground_albedo.jpg")->getAsTexture();
        floorShader->setTexture(TextureType::Albedo, floorAlbedoTexture);

        treeShader = getRenderer()->createPhongShader();
        auto treeAlbedoTexture = resourceController->addImage("./data/3d/tree_albedo.jpg")->getAsTexture();
        treeShader->setTexture(TextureType::Albedo, treeAlbedoTexture);

        auto floorComponent = createComponent<ComponentMesh>();
        floorComponent->setMesh(floorMesh);
        floorComponent->setShader(floorShader);
        floorComponent->transform.setScale(0.01f, 0.01f, 0.01f);

        // Left
        spawnTree(0.0f, 0.96f, -5.9f, 71.0f, 1.1f);
        spawnTree(-7.85603f, 1.51206f, -10.2466f, 243.0f, 1.5f);
        spawnTree(2.62099f, 1.61022f, -11.8371f, 243.0f, 1.2f);
        spawnTree(9.69429f, 1.20674f, -7.01909f, 463.0f, 1.8f);
        spawnTree(-1.93082f, 1.59953f, -11.9317f, 293.0f, 1.4f);

        // Left far
        spawnTree(-4.85603f, 1.51206f, -17.2466, 243.0f, 1.1f);
        spawnTree(-8.85603f, 1.61206f, -22.2466, 88.0f, 1.2f);
        spawnTree(-13.85603f, 1.61206f, -17.2466, 22.0f, 1.4f);
        spawnTree(6.85603f, 1.41206f, -19.2466, 0.0f, 1.4f);
        spawnTree(12.85603f, 1.61206f, -20.2466, 55.0f, 1.3f);
        spawnTree(14.85603f, 1.51206f, -22.2466, 259.0f, 1.6f);
        spawnTree(-5.85603f, 1.51206f, -25.2466, 44.0f, 1.1f);
        spawnTree(-6.85603f, 1.61206f, -26.2466, 77.0f, 1.2f);
        spawnTree(-11.85603f, 1.61206f, -28.2466, 377.0f, 1.4f);
        spawnTree(2.85603f, 1.41206f, -29.2466, 40.0f, 1.4f);
        spawnTree(6.85603f, 1.61206f, -27.2466, 66.0f, 1.3f);
        spawnTree(10.85603f, 1.51206f, -26.2466, 243.0f, 1.6f);

        // Right
        spawnTree(9.69429f, 1.13718f, 6.47371f, 579.0f, 1.1f);
        spawnTree(5.39769f, 1.50266f, 9.65971f, 818.0f, 1.5f);
        spawnTree(-0.440189f, 1.32333f, 8.33341f, 993.0f, 1.2f);
        spawnTree(-5.62584f, 1.67503f, 13.0492f, 894.0f, 1.3f);
        spawnTree(-10.8634f, 1.37381f, 9.11185f, 1026.0f, 1.4f);
        spawnTree(-13.4929f, 1.68867f, 12.7425f, 1201.0f, 1.3f);

        // Right far
        spawnTree(-4.85603, 1.51206, 17.2466, 243.0f, 1.1f);
        spawnTree(-8.85603, 1.61206, 22.2466, 88.0f, 1.2f);
        spawnTree(-13.85603, 1.61206, 17.2466, 22.0f, 1.4f);
        spawnTree(6.85603, 1.41206, 19.2466, 0.0f, 1.4f);
        spawnTree(12.85603, 1.61206, 20.2466, 55.0f, 1.3f);
        spawnTree(14.85603, 1.51206, 22.2466, 259.0f, 1.6f);

        spawnTree(-5.85603, 1.51206, 25.2466, 44.0f, 1.1f);
        spawnTree(-6.85603, 1.61206, 26.2466, 77.0f, 1.2f);
        spawnTree(-11.85603, 1.61206, 28.2466, 377.0f, 1.4f);
        spawnTree(2.85603, 1.41206, 29.2466, 40.0f, 1.4f);
        spawnTree(6.85603, 1.61206, 27.2466, 66.0f, 1.3f);
        spawnTree(10.85603, 1.51206, 26.2466, 243.0f, 1.6f);
    }

    void spawnTree(float x, float y, float z, float r, float s)
    {
        auto treeComponent = createComponent<ComponentMesh>();
        treeComponent->setMesh(treeMesh);
        treeComponent->setShader(treeShader);

        treeComponent->transform.setPosition(Vector3(x, y - 0.4f, z));
        treeComponent->transform.setRotation(Vector3(0.0f, r / 180.0f * CONST_PI, 0.0f));
        treeComponent->transform.setScale(Vector3(s * 0.01f, s * 0.01f, s * 0.01f));

        treeComponent->addLod(treeMeshLod, 32.0f);
    }

    MeshStatic *floorMesh = nullptr;
    MeshStatic *treeMesh = nullptr;
    MeshStatic *treeMeshLod = nullptr;
    PhongShader *treeShader = nullptr;
    PhongShader *floorShader = nullptr;
};

class Robot : public ActorPawn, WithRenderer
{
public:
    Robot() : ActorPawn(this)
    {
        auto resourceController = RTEngine::getInstance()->getResourceController();

        // Animated mesh is loaded as MeshCompound.
        // MeshCompound has a set of static meshes with each having an own transformation
        auto animatedMeshResource = resourceController->addMesh("./data/3d/robot.fbx");
        auto animatedMesh = animatedMeshResource->getAsMeshCompound();

        transform.setScale(0.25f, 0.25f, 0.25f);
        transform.setRotation(Vector3(0.0f, CONST_PI / -2.0f, 0.0f));
        transform.setPosition(0.0f, -0.01f, 0.0f);

        auto animMeshShader = getRenderer()->createPhongShader();
        auto animMeshAlbedoTexture = resourceController->addImage("./data/3d/robot_texture_base.jpg")->getAsTexture();
        auto animMeshNormalTexture = resourceController->addImage("./data/3d/robot_texture_nor.jpg")->getAsTexture();
        auto animMeshRoughTexture = resourceController->addImage("./data/3d/robot_texture_rough.jpg")->getAsTexture();

        animMeshShader->setTexture(TextureType::Albedo, animMeshAlbedoTexture);
        animMeshShader->setTexture(TextureType::Normal, animMeshNormalTexture);
        animMeshShader->setTexture(TextureType::Roughness, animMeshRoughTexture);

        // Animation and MeshCompound are static data shared between meshes
        auto animComponent = createComponent<ComponentAnimatedMesh>();
        animComponent->setMesh(animatedMesh);
        animComponent->setShader(animMeshShader);
        animComponent->transform.setScale(0.01f, 0.01f, 0.01f);
        bodyTransformation = animComponent->getNodeTransformation("Body");
        handLTransformation = animComponent->getNodeTransformation("Hand.L");
        handRTransformation = animComponent->getNodeTransformation("Hand.R");

        // To animate a MeshCompound you need to create an individual animation descriptor called Animator
        // Animator holds such parameters as speed, current time moment and current state
        // Animator bond to Component so each component you want to animate should contain it's own Animator
        // Animator is being updated inside ComponentAnimatedMesh on each Actor's update
        auto animator = animComponent->createAnimator(animatedMeshResource->getFirstAnimation());
        animator->speed = 1.4f;
        animator->play();

        // First we create an input type with function to receive the input itself and make an action base on it
        auto inputX = registerAxisCallback(&Robot::controlX);
        // Second we will add the dependences of this input type around your gamepad, mouse, keyboard and other input devices
        // Second parameter is a multiplier of device input making it here in range from -400 to 400
        // Note that keyboard usually have input of 0 to 1, but axises have input from -1 to 1.
        // Also some gamepad axises may have -1 in their rest position, like triggers
        inputX->addKeyboardBinding(KeyboardCodes::D, 1.0f);
        inputX->addKeyboardBinding(KeyboardCodes::A, -1.0f);
        inputX->addKeyboardBinding(KeyboardCodes::RIGHT, 1.0f);
        inputX->addKeyboardBinding(KeyboardCodes::LEFT, -1.0f);
        inputX->addGamepadAxisBinding(GamepadAxisCode::LEFT_AXIS_X, 1.0f);

        auto inputY = registerAxisCallback(&Robot::controlY);
        inputY->addKeyboardBinding(KeyboardCodes::S, -1.0f);
        inputY->addKeyboardBinding(KeyboardCodes::W, 1.0f);
        inputY->addKeyboardBinding(KeyboardCodes::DOWN, -1.0f);
        inputY->addKeyboardBinding(KeyboardCodes::UP, 1.0f);
        inputY->addGamepadAxisBinding(GamepadAxisCode::LEFT_AXIS_Y, -1.0f);
    }

    void onProcess(float delta)
    {
        bodyRotation = warpAngle(bodyRotation + moveX * delta);
        handRotation = warpAngle(handRotation - moveY * delta);

        if (handRotation > 0.8f)
            handRotation = 0.8f;
        if (handRotation < -0.5f)
            handRotation = -0.5f;

        bodyTransformation->setRotation(Vector3(0.0f, 0.0f, bodyRotation));

        handLTransformation->setRotation(Vector3(handRotation, 0.0f, 0.0f));
        handRTransformation->setRotation(Vector3(handRotation, 0.0f, 0.0f));
    }

    void controlX(InputType type, int deviceIndex, int index, float axis)
    {
        moveX = axis;
    }

    void controlY(InputType type, int deviceIndex, int index, float axis)
    {
        moveY = axis;
    }

    Transformation *bodyTransformation;
    Transformation *handRTransformation;
    Transformation *handLTransformation;
    float bodyRotation = 0.0f;
    float handRotation = 0.0f;
    float moveX = 0.0f;
    float moveY = 0.0f;
};

APPMAIN
{
    // Engine setup
    auto engine = RTEngine::getInstance("ex19cfg.ini");

    // We need view controller to get resolution
    auto viewController = engine->getViewController();

    // Set fullscreen through configuration controller
    auto configController = engine->getConfigController();
    auto config = configController->getConfig();

    config->setupByQuality(RenderQuality::High);
    config->setWindowWidth(viewController->getPrimaryScreenWidth() * 0.8f);
    config->setWindowHeight(viewController->getPrimaryScreenHeight() * 0.8f);
    config->setFullscreenState(false);

    // Enable antialiasing
    config->setAnialiasing(AntiAliasing::SSAA);

    // View setup
    auto view = viewController->createView("Example \"19. Hello Animation\"");

    // Stage setup
    auto stageController = engine->getStageController();
    auto stage = stageController->createStage("Hello Animation");

    // Layers and camera setup
    auto layerActors = stage->createLayerActors("Hello Animation Layer", 0);
    layerActors->setAmbientColor(0.25f, 0.25f, 0.1f);
    layerActors->setGamma(1.2f);

    auto camera = layerActors->createActor<ActorCamera>();
    camera->setupPerspectiveCamera()->setWidthBasedResolution(1280);

    auto layerGUI = stage->createLayerActors("Hello Gui Layer", 1);
    auto guiCamera = layerGUI->createActor<ActorCamera>();
    guiCamera->setupOrtoCamera()->setWidthBasedResolution(1280);

    // Resources
    auto resourceController = engine->getResourceController();

    // HDR
    auto HDR = resourceController->addHDRImage("./data/3d/meadow.hdr", 1.0f, 0.8f);
    layerActors->setHDRTextures(HDR->getAsTexture(), HDR->getAsRadianceTexture());
    layerActors->setHDRRotation(CONST_PI / 2.0f);

    // Floor is repeated to feel as endless road
    std::vector<Ground *> grounds;
    for (int i = 0; i < 16; i++)
    {
        auto ground = layerActors->createActor<Ground>();
        grounds.push_back(ground);
    }

    // Animated mesh
    layerActors->createActor<Robot>();

    // Sun with shadow casting
    auto sun = layerActors->createActor<Actor>();
    auto sunComponent = sun->createComponent<ComponentLight>();
    sunComponent->setupSunLight(Vector3(1.45f, 1.45f, 1.19f), true);
    sunComponent->transform.setPosition(Vector3(-0.8f, 1.0f, 0.7f));

    // Settings used to move our ground
    // Each time shift exceeding groundSize it will be moved back to groundSize
    // to create a seamless feel of endless movement
    float groundMove = 0.0f;
    const float groundMoveSpeed = 1.2f;
    const float groundSize = 30.0f;
    while (!engine->isTerminationIntended())
    {
        float delta = engine->syncFrame();
        groundMove += delta * groundMoveSpeed;
        while (groundMove > groundSize)
            groundMove -= groundSize;

        viewController->processEvents();

        // Ground shift update
        float s = 0.0f;
        for (auto &ground : grounds)
        {
            ground->transform.setPosition(groundSize * s + groundMove - groundSize, 0.0f, 0.0f);
            s += 1.0f;
        }

        camera->transform.setPosition(-3.0f, 2.0f, 1.5f);
        camera->lookAt(0.0f, 0.8f, 0.0f);

        stage->process(delta);
        stage->present(view);
    }

    engine->destroy();
    return 0;
}