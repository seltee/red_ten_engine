// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "../src/rtengine.h"
#include <math.h>

class Sphere : public Actor, public WithRepository
{
public:
    Sphere() : Actor()
    {
        registerClassName("Sphere");
    }

    void onSpawned()
    {
        auto sphereMesh = repository->addMesh("./data/3d/concrete_ball.fbx");
        auto sphereAlbedoTexture = repository->addTexture("./data/3d/concrete_ball_albedo.jpg");
        auto sphereNormalTexture = repository->addTexture("./data/3d/concrete_ball_normal.jpg");

        if (!sphereShader)
        {
            sphereShader = new PhongShader();
            sphereShader->setTexture(TextureType::Albedo, sphereAlbedoTexture);
            sphereShader->setTexture(TextureType::Normal, sphereNormalTexture);
        }

        auto sphereComponent = createComponent<ComponentMesh>();
        sphereComponent->setMesh(sphereMesh);
        sphereComponent->setShader(sphereShader);
        sphereComponent->addShapeSphere(0.4f);

        setFrictionAndRestitution(0.7f, 0.5f);
        setPhysicsMotionType(MotionType::Dynamic);
    }

    void onProcess(float delta)
    {
        if (autoremove)
        {
            auto position = transform.getPosition();
            if (position.y < -10.0f)
                destroy();
        }
    }

    static PhongShader *sphereShader;
    bool autoremove = true;
};
PhongShader *Sphere::sphereShader = nullptr;

class Crate : public Actor, public WithMeshMaker, public WithRepository
{
public:
    Crate() : Actor()
    {
        registerClassName("Crate");
    }

    void onSpawned()
    {
        crateMesh = meshMaker->createBox({1.0f, 1.0f, 1.0f});
        auto crateAlbedoTexture = repository->addTexture("./data/crate.jpg");

        if (!crateShader)
        {
            crateShader = new PhongShader();
            crateShader->setTexture(TextureType::Albedo, crateAlbedoTexture);
        }

        auto crateComponent = createComponent<ComponentMesh>();
        crateComponent->setMesh(crateMesh);
        crateComponent->setShader(crateShader);
        crateComponent->addShape3dBox(Vector3(0.0f), Vector3(1.0f, 1.0f, 1.0f));

        setFrictionAndRestitution(0.7f, 0.1f);
        setPhysicsMotionType(MotionType::Dynamic);
    }

    void onProcess(float delta)
    {
        if (autoremove)
        {
            auto position = transform.getPosition();
            if (position.y < -10.0f)
                destroy();
        }
    }

    static PhongShader *crateShader;
    static Mesh *crateMesh;
    bool autoremove = true;
};
PhongShader *Crate::crateShader = nullptr;
Mesh *Crate::crateMesh = nullptr;


class Platform : public Actor, public WithMeshMaker, public WithRepository
{
public:
    Platform() : Actor()
    {
        registerClassName("Platform");
    }

    void onSpawned()
    {
        platformMesh = meshMaker->createBox({10.0f, 1.0f, 10.0f});
        auto platformAlbedoTexture = repository->addTexture("./data/3d/concrete_albedo.jpg");

        if (!platformShader)
        {
            platformShader = new PhongShader();
            platformShader->setTexture(TextureType::Albedo, platformAlbedoTexture);
        }

        auto platformComponent = createComponent<ComponentMesh>();
        platformComponent->setMesh(platformMesh);
        platformComponent->setShader(platformShader);
        platformComponent->addShape3dBox(Vector3(0.0f), Vector3(10.0f, 1.0f, 10.0f));

        setFrictionAndRestitution(0.7f, 0.1f);
        setPhysicsMotionType(MotionType::Static);
    }

    static PhongShader *platformShader;
    static Mesh *platformMesh;
};
PhongShader *Platform::platformShader = nullptr;
Mesh *Platform::platformMesh = nullptr;


class Capsule : public Actor, public WithRepository
{
public:
    Capsule() : Actor()
    {
        registerClassName("Capsule");
    }

    void onSpawned()
    {
        auto sphereMesh = repository->addMesh("./data/3d/concrete_ball.fbx");
        auto sphereAlbedoTexture = repository->addTexture("./data/3d/concrete_ball_albedo.jpg");
        auto sphereNormalTexture = repository->addTexture("./data/3d/concrete_ball_normal.jpg");

        if (!sphereShader)
        {
            sphereShader = new PhongShader();
            sphereShader->setTexture(TextureType::Albedo, sphereAlbedoTexture);
            sphereShader->setTexture(TextureType::Normal, sphereNormalTexture);
        }

        auto sphereComponent = createComponent<ComponentMesh>();
        sphereComponent->setMesh(sphereMesh);
        sphereComponent->setShader(sphereShader);
        sphereComponent->addShapeCapsule(1.0f, 0.4f);

        sphereComponent = createComponent<ComponentMesh>();
        sphereComponent->setMesh(sphereMesh);
        sphereComponent->setShader(sphereShader);
        sphereComponent->transform.setPosition(Vector3(0.0f, -0.5f, 0.0f));

        sphereComponent = createComponent<ComponentMesh>();
        sphereComponent->setMesh(sphereMesh);
        sphereComponent->setShader(sphereShader);
        sphereComponent->transform.setPosition(Vector3(0.0f, 0.5f, 0.0f));

        sphereComponent = createComponent<ComponentMesh>();
        sphereComponent->setMesh(sphereMesh);
        sphereComponent->setShader(sphereShader);
        sphereComponent->transform.setPosition(Vector3(0.0f, -0.25f, 0.0f));

        sphereComponent = createComponent<ComponentMesh>();
        sphereComponent->setMesh(sphereMesh);
        sphereComponent->setShader(sphereShader);
        sphereComponent->transform.setPosition(Vector3(0.0f, 0.25f, 0.0f));

        setFrictionAndRestitution(0.7f, 0.3f);
        setPhysicsMotionType(MotionType::Dynamic);
    }

    static PhongShader *sphereShader;
    bool autoremove = true;
};
PhongShader *Capsule::sphereShader = nullptr;
