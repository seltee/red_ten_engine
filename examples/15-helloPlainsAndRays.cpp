// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "../src/rtengine.h"
#include "helpers.h"
#include <math.h>

const int spawnPerSecond = 2;

int main()
{
    // Engine setup
    auto engine = RTEngine::createInstance();

    // We need view controller to get resolution
    auto viewController = engine->getViewController();

    auto configController = engine->getConfigController();
    auto config = configController->getConfig();

    // Setup configuration
    config->setWindowWidth(viewController->getPrimaryScreenWidth() * 0.8f);
    config->setWindowHeight(viewController->getPrimaryScreenHeight() * 0.8f);
    config->setFullscreenState(false);

    // View setup
    auto view = viewController->createView("Example \"13. Hello 3D Physics\"");

    // Stage setup
    auto stageController = engine->getStageController();
    auto stage = stageController->createStage("Hello 3D Physics");

    // Layers and camera setup
    // Also enabling physics for the layer
    auto layerActors = stage->createLayerActors("Hello 3D Layer", 0);
    layerActors->enablePhisics(Vector3(0.0f, -4.0f, 0.0f), 0.1f, 100);

    auto camera = layerActors->createActor<CameraPerspective>();
    camera->setWidthBasedResolution(1280);
    camera->transform.setPosition(2.0f, 7.0f, 8.0f);
    camera->lookAt(0.0f, 0.0f, 0.0f);

    // Sun with shadow casting
    auto sun = layerActors->createActor<Actor>();
    auto sunComponent = sun->createComponent<ComponentLight>();
    sunComponent->setupSunLight(Vector3(-1.0f, 2.0f, 1.0f), Vector3(0.9f, 0.9f, 0.9f), true);

    ComponentMesh *componentMesh;

    // Create plain
    Vector3 plainNormal = glm::normalize(Vector3(0.0f, 1.0f, 0.0f));
    float plainDistance = -3.0f;
    auto plainBody = layerActors->getPhysicsWorld()->createPhysicsBody(new ShapePlain(plainNormal, plainDistance, layerActors->getPhysicsWorld()));
    plainBody->setStaticMotionType();
    plainBody->setRestitution(0.2f);
    plainBody->setFriction(0.7f);

    auto plainMesh = engine->getMeshMaker()->createPlane({20.0f, 20.0f}, 4.0f);
    auto plainTextureAlbedo = engine->getResourceController()->addTexture("./data/3d/pavement_albedo.jpg");
    auto plainTextureNormal = engine->getResourceController()->addTexture("./data/3d/pavement_normal.jpg");
    auto plainShader = new PhongShader();
    plainShader->setTexture(TextureType::Albedo, plainTextureAlbedo);
    plainShader->setTexture(TextureType::Normal, plainTextureNormal);

    auto plain = layerActors->createActor<Actor>();
    componentMesh = plain->createComponent<ComponentMesh>();
    componentMesh->setMesh(plainMesh);
    componentMesh->setShader(plainShader);
    plain->transform.setPosition(plainNormal * plainDistance + Vector3(0.0f, 0.0f, -4.0f));
    // plain->transform.setRotation(Vector3(0.0f, 0.0f, -CONST_PI / 30.0f));

    // Rotating boxes
    auto boxMesh = engine->getMeshMaker()->createBox({0.4f, 0.4f, 0.4f});
    auto boxTexture = engine->getResourceController()->addTexture("./data/crate.jpg");
    auto boxShader = new PhongShader();
    boxShader->setTexture(TextureType::Albedo, boxTexture);

    auto box1 = layerActors->createActor<Actor>();
    componentMesh = box1->createComponent<ComponentMesh>();
    componentMesh->setMesh(boxMesh);
    componentMesh->setShader(boxShader);

    auto box2 = layerActors->createActor<Actor>();
    componentMesh = box2->createComponent<ComponentMesh>();
    componentMesh->setMesh(boxMesh);
    componentMesh->setShader(boxShader);

    auto platform = layerActors->createActor<Platform>();
    platform->transform.setPosition(Vector3(0.0f, -3.0f, 0.0f));
    // platform->showBoundingBox(true);

    auto sphere = layerActors->createActor<Sphere>();
    sphere->transform.setPosition(-10.0f, 4.0f, 0.0f);
    // sphere->getPhysicsBody()->addLinearVelocity(Vector3(0.18f, 0.0f, -0.05f));
    // sphere->getPhysicsBody()->addAngularVelocity(Vector3(0.0f, 0.015f, 0.0f));
    //  sphere->showBoundingBox(true);

    auto box = layerActors->createActor<Crate>();
    box->transform.setPosition(-1.8f, 0.0f, 1.8f);
    //box->transform.setRotation(glm::angleAxis(-0.6f, Vector3(1.0f, 0.01f, 0.0f)));
    box->getPhysicsBody()->addLinearVelocity(Vector3(0.1f, 0.0f, 0.0f));

    /*
        auto box3 = layerActors->createActor<Crate>();
        box3->transform.setPosition(1.8f, 6.0f, -4.8f);
        box3->transform.setRotation(glm::angleAxis(-0.6f, Vector3(0.4f, 1.0f, 0.0f)));
        box3->getPhysicsBody()->addLinearVelocity(Vector3(0.1f, 0.0f, 0.0f));
        */

    // sphere->transform.setRotation(Vector3(1.0f, 0.0f, 0.0f));
    // Quat quat = glm::angleAxis(-0.3f, Vector3(0.0f, 0.0f, 1.0f));
    // sphere->getPhysicsBody()->setAngularVelocity(quat);

    //float spawnCounter = (float)spawnPerSecond;
    float spawnCounter = -1000.0;
    float rotationCounter = 0.0f;
    float impulseCounter = 0.0f;
    int spawnedCounter = 0;
    while (!engine->isTerminationIntended())
    {
        float delta = engine->syncFrame();

        // sphere->getPhysicsBody()->setAngularVelocity(Vector3(-CONST_PI * 2, 0.0f, 0.0f));
        // sphere->getPhysicsBody()->addLinearVelocity(Vector3(0.18f * delta, 0.0f, -0.1f* delta));

        viewController->processEvents();

        // Spawn new spheres
        spawnCounter += delta * (float)spawnPerSecond;
        while (spawnCounter > 1.0f)
        {
            spawnCounter -= 1.0f;
            int sp = rand() % 10;
            Actor *spawn = nullptr;
            if (sp > 4)
                spawn = layerActors->createActor<Sphere>();
            else
                spawn = layerActors->createActor<Crate>();

            spawn->transform.setPosition(randf(-4.0f, 4.0f), randf(6.0f, 9.0f), randf(-4.0f, 4.0f));
            spawn->transform.setRotation(glm::angleAxis(-0.01f, Vector3(1.0f, 0.01f, 0.0f)));
            spawn->getPhysicsBody()->addLinearVelocity(Vector3(0.1f, 0.0f, 0.0f));
            spawnedCounter++;
            printf("Spawned %i\n", spawnedCounter);
        }

        // Set our rotating boxes positions && calc ray end and ray start vectors
        const float distance = 5.0f;
        rotationCounter += delta * 0.2f;
        Vector3 p1 = Vector3(sinf(rotationCounter) * distance, -2.0f, cosf(rotationCounter) * distance);
        Vector3 p2 = Vector3(-sinf(rotationCounter) * distance, -2.0f, -cosf(rotationCounter) * distance);
        box1->transform.setPosition(p1);
        box2->transform.setPosition(p2);

        // Cast ray 2 times per second
        impulseCounter += delta * 2.0f;
        if (impulseCounter > 1.0f)
        {
            impulseCounter -= 1.0f;
            auto result = layerActors->castRayCollision(Line(p1, p2), true);
            // for (auto it = result.begin(); it != result.end(); it++)
            //{
            //  if (it->actor)
            //      it->actor->destroy();
            //}
        }

        stage->process(delta);
        stage->present(view);
    }
}