#pragma once
#include "math/math.h"
#include "physics/physicsBody.h"
#include "physics/collisionSolver.h"
#include "physics/collisionDispatcher.h"
#include "physics/shapes/shape.h"
#include <vector>

struct BodyPair
{
    PhysicsBody *a;
    PhysicsBody *b;
};


class Actor;

class PhysicsWorld
{
public:
    EXPORT PhysicsWorld(Vector3 gravity, float simScale, int stepsPerSecond);
    EXPORT void setBasicParameters(Vector3 gravity, float simScale, int stepsPerSecond);

    EXPORT float getSimScale();

    EXPORT PhysicsBody *createPhysicsBody(Shape *shape, Actor *actor = nullptr);
    EXPORT void process(float delta);
    EXPORT void removeDestroyed();

    EXPORT std::vector<PhysicsBodyPoint> castRay(Line ray);

protected:
    void prepareBodies();
    void applyForces();
    void findCollisionPairs(std::vector<BodyPair> *pairs);
    void findCollisions(std::vector<BodyPair> *pairs, CollisionCollector *collisionCollector);
    void solveSollisions(CollisionCollector *collisionCollector);
    void finishStep();

    std::vector<PhysicsBody *> bodies;
    CollisionDispatcher collisionDispatcher;

    Vector3 gravity;
    float deltaAccumulator = 0.0f;
    float subStep = 0.0f;

    float simScale = 0.01f;
    int maxThreads;
};