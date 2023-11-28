#include "physicsWorld.h"
#include <future>
#include <thread>
#include <mutex>
#include <chrono>

std::mutex lock;

void _prepareBody(std::vector<PhysicsBody *>::iterator bodyStart, std::vector<PhysicsBody *>::iterator bodyEnd)
{
    for (auto body = bodyStart; body < bodyEnd; body++)
        (*body)->prepareSteps();
}

void _processBody(std::vector<PhysicsBody *>::iterator bodyStart, std::vector<PhysicsBody *>::iterator bodyEnd, float subStep, Vector3 localGravity)
{
    for (auto body = bodyStart; body < bodyEnd; body++)
        (*body)->process(subStep, localGravity);
}

void _finishBody(std::vector<PhysicsBody *>::iterator bodyStart, std::vector<PhysicsBody *>::iterator bodyEnd, float subStep)
{
    for (auto body = bodyStart; body < bodyEnd; body++)
        (*body)->finishStep(subStep);
}

void _collectPairs(
    std::vector<PhysicsBody *>::iterator bodyStart,
    std::vector<PhysicsBody *>::iterator bodyEnd,
    std::vector<PhysicsBody *> *bodyList,
    std::vector<BodyPair> *list)
{
    for (auto a = bodyStart; a < bodyEnd; a++)
    {
        if (!(*a)->isEnabled())
            continue;
        for (auto b = bodyList->begin(); b != bodyList->end(); b++)
        {
            if (!(*b)->isEnabled())
                continue;

            if (a == b)
                break;

            if ((*a)->getMotionType() == MotionType::Static && (*b)->getMotionType() == MotionType::Static)
                continue;

            if ((*a)->isSleeping() && (*b)->isSleeping())
                continue;

            if ((*a)->checkAABB((*b)->getAABB()))
            {
                lock.lock();
                list->push_back({*a, *b});
                lock.unlock();
            }
        }
    }
}

void _collide(
    std::vector<BodyPair>::iterator pairStart,
    std::vector<BodyPair>::iterator pairEnd,
    CollisionDispatcher *collisionDispatcher,
    CollisionCollector *collisionCollector)
{
    for (auto pair = pairStart; pair < pairEnd; pair++)
    {
        collisionDispatcher->collide(pair->a, pair->b, collisionCollector);
    }
}

void _solve(
    std::vector<CollisionPair>::iterator pairStart,
    std::vector<CollisionPair>::iterator pairEnd,
    float simScale,
    float subStep)
{
    CollisionSolver collisionSolver(simScale);
    for (auto pair = pairStart; pair < pairEnd; pair++)
    {
        collisionSolver.solve(pair->a, pair->b, pair->manifold, subStep);
    }
}

void _ray(
    std::vector<PhysicsBody *>::iterator bodyStart,
    std::vector<PhysicsBody *>::iterator bodyEnd,
    const Segment &rayLocal,
    std::vector<PhysicsBodyPoint> *points)
{
    for (auto body = bodyStart; body < bodyEnd; body++)
    {
        if (!(*body)->checkAABB(rayLocal))
            continue;

        lock.lock();
        (*body)->castRay(rayLocal, points);
        lock.unlock();
    }
}

PhysicsWorld::PhysicsWorld(const Vector3 &gravity, float simScale, int stepsPerSecond)
{
    setBasicParameters(gravity, simScale, stepsPerSecond);

    maxThreads = std::thread::hardware_concurrency() - 1;
    if (maxThreads <= 0)
        maxThreads = 1;

    logger->logff("Max threads supported: %i (%i)", maxThreads, std::thread::hardware_concurrency());
}

void PhysicsWorld::setBasicParameters(const Vector3 &gravity, float simScale, int stepsPerSecond)
{
    this->gravity = gravity;
    this->simScale = simScale;
    this->subStep = 1.0f / (float)stepsPerSecond;
}

float PhysicsWorld::getSimScale()
{
    return simScale;
}

PhysicsBody *PhysicsWorld::createPhysicsBody(Shape *shape, Actor *actor)
{
    if (!shape)
        return nullptr;
    auto newBody = new PhysicsBody(shape, simScale);
    newBody->setActor(actor);
    bodies.push_back(newBody);
    return newBody;
}

void PhysicsWorld::process(float delta)
{
    deltaAccumulator += delta;
    prepareBodies();
    while (deltaAccumulator > subStep)
    {
        deltaAccumulator -= subStep;

        CollisionCollector collisionCollector;
        std::vector<BodyPair> pairs;

        applyForces();
        findCollisionPairs(&pairs);
        findCollisions(&pairs, &collisionCollector);
        solveSollisions(&collisionCollector);
        finishStep();
        triggerCollisionEvents(&collisionCollector);
        removeNotPersistedCollisions();
    }
}

void PhysicsWorld::removeDestroyed()
{
    auto body = bodies.begin();
    while (body != bodies.end())
        if ((*body)->isDestroyed())
        {
            delete (*body);
            body = bodies.erase(body);
        }
        else
            ++body;
}

std::vector<PhysicsBodyPoint> PhysicsWorld::castRay(const Segment &ray)
{
    std::vector<PhysicsBodyPoint> points;
    std::vector<std::future<void>> futures;
    futures.reserve(maxThreads);
    int bodiesPerThread = bodies.size() / maxThreads;
    Segment rayLocal = Segment(ray.a * simScale, ray.b * simScale);
    std::vector<PhysicsBody *>::iterator currentBody = bodies.begin();

    for (int i = 0; i < maxThreads; i++)
    {
        if (i == maxThreads - 1)
            futures.push_back(std::async(_ray, currentBody, bodies.end(), rayLocal, &points));
        else
        {
            futures.push_back(std::async(_ray, currentBody, currentBody + bodiesPerThread, rayLocal, &points));
            currentBody += bodiesPerThread;
        }
    }
    for (auto &ft : futures)
        ft.wait();
    return points;
}

// Prepare global before multiple physics steps
void PhysicsWorld::prepareBodies()
{
    std::vector<std::future<void>> futures;
    futures.reserve(maxThreads);
    int bodiesPerThread = bodies.size() / maxThreads;
    std::vector<PhysicsBody *>::iterator currentBody = bodies.begin();
    for (int i = 0; i < maxThreads; i++)
    {
        if (i == maxThreads - 1)
            futures.push_back(std::async(_prepareBody, currentBody, bodies.end()));
        else
        {
            futures.push_back(std::async(_prepareBody, currentBody, currentBody + bodiesPerThread));
            currentBody += bodiesPerThread;
        }
    }
    for (auto &ft : futures)
        ft.wait();
}

// Process gravitation and forces on each body
void PhysicsWorld::applyForces()
{
    Vector3 localGravity = gravity * simScale;
    std::vector<std::future<void>> futures;
    futures.reserve(maxThreads);
    int bodiesPerThread = bodies.size() / maxThreads;
    std::vector<PhysicsBody *>::iterator currentBody = bodies.begin();
    for (int i = 0; i < maxThreads; i++)
    {
        if (i == maxThreads - 1)
            futures.push_back(std::async(_processBody, currentBody, bodies.end(), subStep, localGravity));
        else
        {
            futures.push_back(std::async(_processBody, currentBody, currentBody + bodiesPerThread, subStep, localGravity));
            currentBody += bodiesPerThread;
        }
    }
    for (auto &ft : futures)
        ft.wait();
}

void PhysicsWorld::findCollisionPairs(std::vector<BodyPair> *pairs)
{
    // find possible collision pairs
    std::vector<std::future<void>> futures;
    futures.reserve(maxThreads);
    std::vector<PhysicsBody *>::iterator currentBody = bodies.begin();
    int bodiesPerThread = bodies.size() / maxThreads;
    for (int i = 0; i < maxThreads; i++)
    {
        if (i == maxThreads - 1)
            futures.push_back(std::async(_collectPairs, currentBody, bodies.end(), &bodies, pairs));
        else
        {
            futures.push_back(std::async(_collectPairs, currentBody, currentBody + bodiesPerThread, &bodies, pairs));
            currentBody += bodiesPerThread;
        }
    }
    for (auto &ft : futures)
        ft.wait();
}

void PhysicsWorld::findCollisions(std::vector<BodyPair> *pairs, CollisionCollector *collisionCollector)
{
    // find exact collisions
    std::vector<std::future<void>> futures;
    futures.reserve(maxThreads);
    std::vector<BodyPair>::iterator currentPair = pairs->begin();
    int pairsPerThread = pairs->size() / maxThreads;
    for (int i = 0; i < maxThreads; i++)
    {
        if (i == maxThreads - 1)
            futures.push_back(
                std::async(_collide, currentPair, pairs->end(), &collisionDispatcher, collisionCollector));
        else
        {
            futures.push_back(
                std::async(_collide, currentPair, currentPair + pairsPerThread, &collisionDispatcher, collisionCollector));
            currentPair += pairsPerThread;
        }
    }
    for (auto &ft : futures)
        ft.wait();
}

void PhysicsWorld::solveSollisions(CollisionCollector *collisionCollector)
{
    // solve collision pairs
    std::vector<std::future<void>> futures;
    futures.reserve(maxThreads);
    if (collisionCollector->pairs.size() > 0)
    {
        futures.clear();
        std::vector<CollisionPair>::iterator currentCollisionPair = collisionCollector->pairs.begin();
        int pairsPerThread = collisionCollector->pairs.size() / maxThreads;
        for (int i = 0; i < maxThreads; i++)
        {
            if (i == maxThreads - 1)
                futures.push_back(
                    std::async(_solve, currentCollisionPair, collisionCollector->pairs.end(), simScale, subStep));
            else
            {
                futures.push_back(
                    std::async(_solve, currentCollisionPair, currentCollisionPair + pairsPerThread, simScale, subStep));
                currentCollisionPair += pairsPerThread;
            }
        }
        for (auto &ft : futures)
            ft.wait();
    }
}

void PhysicsWorld::finishStep()
{
    std::vector<std::future<void>> futures;
    futures.reserve(maxThreads);
    int bodiesPerThread = bodies.size() / maxThreads;
    std::vector<PhysicsBody *>::iterator currentBody = bodies.begin();
    for (int i = 0; i < maxThreads; i++)
    {
        if (i == maxThreads - 1)
            futures.push_back(std::async(_finishBody, currentBody, bodies.end(), subStep));
        else
        {
            futures.push_back(std::async(_finishBody, currentBody, currentBody + bodiesPerThread, subStep));
            currentBody += bodiesPerThread;
        }
    }
    for (auto &ft : futures)
        ft.wait();
}

void PhysicsWorld::triggerCollisionEvents(CollisionCollector *collisionCollector)
{
    for (auto &pair : collisionCollector->pairs)
    {
        pair.a->triggerPostCollisionEvent(pair.b, pair.manifold.pointsOnA[0]);
        pair.b->triggerPostCollisionEvent(pair.a, pair.manifold.pointsOnB[0]);
    }
}

void PhysicsWorld::removeNotPersistedCollisions()
{
    for (auto &body : bodies)
        body->removeNotPersistedCollisions();
}