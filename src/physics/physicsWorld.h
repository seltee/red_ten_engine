#pragma once
#include "math/math.h"
#include "physics/physicsBody.h"
#include "physics/collisionSolver.h"
#include "physics/collisionDispatcher.h"
#include "physics/shapes/shape.h"
#include "connector/withLogger.h"
#include "connector/withCore.h"
#include <vector>

struct BodyPair
{
    PhysicsBody *a;
    PhysicsBody *b;
};

class Actor;

class PhysicsWorld : public WithLogger, public WithCore
{
public:
    EXPORT PhysicsWorld(const Vector3 &gravity, float simScale, int stepsPerSecond);
    EXPORT void setBasicParameters(const Vector3 &gravity, float simScale, int stepsPerSecond);

    EXPORT float getSimScale();

    EXPORT PhysicsBody *createPhysicsBody(Shape *shape, Actor *actor = nullptr);
    EXPORT void process(float delta);
    EXPORT void removeDestroyed();

    EXPORT std::vector<PhysicsBodyPoint> castRay(const Segment &ray);

protected:
    void prepareBodies();
    void applyForces();
    void findCollisionPairs(std::vector<BodyPair> *pairs);
    void findCollisions(std::vector<BodyPair> *pairs, CollisionCollector *collisionCollector);
    void solveSollisions(CollisionCollector *collisionCollector);
    void finishStep();
    void triggerCollisionEvents(CollisionCollector *collisionCollector);
    void removeNotPersistedCollisions();

    std::vector<PhysicsBody *> bodies;
    CollisionDispatcher collisionDispatcher;

    Vector3 gravity;
    float deltaAccumulator = 0.0f;
    float subStep = 0.0f;

    float simScale = 0.01f;
    int maxThreads;
    std::vector<BodyPair> pairs;
    CollisionCollector collisionCollector;
};