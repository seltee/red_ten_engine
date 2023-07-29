#include "physicsWorld.h"
#include <thread>
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
        for (auto b = bodyList->begin(); b != bodyList->end(); b++)
        {
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
    maxThreads = std::thread::hardware_concurrency();
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
    std::vector<std::thread> threads;
    int bodiesPerThread = bodies.size() / maxThreads;
    Segment rayLocal = Segment(ray.a * simScale, ray.b * simScale);
    std::vector<PhysicsBody *>::iterator currentBody = bodies.begin();

    for (int i = 0; i < maxThreads; i++)
    {
        if (i == maxThreads - 1)
            threads.push_back(std::thread(_ray, currentBody, bodies.end(), rayLocal, &points));
        else
        {
            threads.push_back(std::thread(_ray, currentBody, currentBody + bodiesPerThread, rayLocal, &points));
            currentBody += bodiesPerThread;
        }
    }
    for (auto &th : threads)
        th.join();

    return points;
}

// Prepare global before multiple physics steps
void PhysicsWorld::prepareBodies()
{
    std::vector<std::thread> threads;
    int bodiesPerThread = bodies.size() / maxThreads;
    std::vector<PhysicsBody *>::iterator currentBody = bodies.begin();
    for (int i = 0; i < maxThreads; i++)
    {
        if (i == maxThreads - 1)
            threads.push_back(std::thread(_prepareBody, currentBody, bodies.end()));
        else
        {
            threads.push_back(std::thread(_prepareBody, currentBody, currentBody + bodiesPerThread));
            currentBody += bodiesPerThread;
        }
    }
    for (auto &th : threads)
        th.join();
}

// Process gravitation and forces on each body
void PhysicsWorld::applyForces()
{
    Vector3 localGravity = gravity * simScale;
    std::vector<std::thread> threads;
    int bodiesPerThread = bodies.size() / maxThreads;
    std::vector<PhysicsBody *>::iterator currentBody = bodies.begin();
    for (int i = 0; i < maxThreads; i++)
    {
        if (i == maxThreads - 1)
            threads.push_back(std::thread(_processBody, currentBody, bodies.end(), subStep, localGravity));
        else
        {
            threads.push_back(std::thread(_processBody, currentBody, currentBody + bodiesPerThread, subStep, localGravity));
            currentBody += bodiesPerThread;
        }
    }
    for (auto &th : threads)
        th.join();
}

void PhysicsWorld::findCollisionPairs(std::vector<BodyPair> *pairs)
{
    // find possible collision pairs
    std::vector<std::thread> threads;
    std::vector<PhysicsBody *>::iterator currentBody = bodies.begin();
    int bodiesPerThread = bodies.size() / maxThreads;
    for (int i = 0; i < maxThreads; i++)
    {
        if (i == maxThreads - 1)
            threads.push_back(std::thread(_collectPairs, currentBody, bodies.end(), &bodies, pairs));
        else
        {
            threads.push_back(std::thread(_collectPairs, currentBody, currentBody + bodiesPerThread, &bodies, pairs));
            currentBody += bodiesPerThread;
        }
    }
    for (auto &th : threads)
        th.join();
}

void PhysicsWorld::findCollisions(std::vector<BodyPair> *pairs, CollisionCollector *collisionCollector)
{

    // find exact collisions
    std::vector<std::thread> threads;
    std::vector<BodyPair>::iterator currentPair = pairs->begin();
    int pairsPerThread = pairs->size() / maxThreads;
    for (int i = 0; i < maxThreads; i++)
    {
        if (i == maxThreads - 1)
            threads.push_back(
                std::thread(_collide, currentPair, pairs->end(), &collisionDispatcher, collisionCollector));
        else
        {
            threads.push_back(
                std::thread(_collide, currentPair, currentPair + pairsPerThread, &collisionDispatcher, collisionCollector));
            currentPair += pairsPerThread;
        }
    }
    for (auto &th : threads)
        th.join();
}

void PhysicsWorld::solveSollisions(CollisionCollector *collisionCollector)
{
    // solve collision pairs
    std::vector<std::thread> threads;
    if (collisionCollector->pairs.size() > 0)
    {
        threads.clear();
        std::vector<CollisionPair>::iterator currentCollisionPair = collisionCollector->pairs.begin();
        int pairsPerThread = collisionCollector->pairs.size() / maxThreads;
        for (int i = 0; i < maxThreads; i++)
        {
            if (i == maxThreads - 1)
                threads.push_back(
                    std::thread(_solve, currentCollisionPair, collisionCollector->pairs.end(), simScale, subStep));
            else
            {
                threads.push_back(
                    std::thread(_solve, currentCollisionPair, currentCollisionPair + pairsPerThread, simScale, subStep));
                currentCollisionPair += pairsPerThread;
            }
        }
        for (auto &th : threads)
            th.join();
    }
}

void PhysicsWorld::finishStep()
{
    std::vector<std::thread> threads;
    int bodiesPerThread = bodies.size() / maxThreads;
    std::vector<PhysicsBody *>::iterator currentBody = bodies.begin();
    for (int i = 0; i < maxThreads; i++)
    {
        if (i == maxThreads - 1)
            threads.push_back(std::thread(_finishBody, currentBody, bodies.end(), subStep));
        else
        {
            threads.push_back(std::thread(_finishBody, currentBody, currentBody + bodiesPerThread, subStep));
            currentBody += bodiesPerThread;
        }
    }
    for (auto &th : threads)
        th.join();
}