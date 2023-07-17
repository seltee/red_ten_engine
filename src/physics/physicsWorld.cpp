#include "physicsWorld.h"

PhysicsWorld::PhysicsWorld(Vector3 gravity, float simScale, int stepsPerSecond)
{
    setBasicParameters(gravity, simScale, stepsPerSecond);
}

void PhysicsWorld::setBasicParameters(Vector3 gravity, float simScale, int stepsPerSecond)
{
    this->gravity = gravity;
    subStep = 1.0f / (float)stepsPerSecond;
    this->simScale = simScale;
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
    Vector3 localGravity = gravity * simScale;
    CollisionCollector collisionCollector;

    for (auto it = bodies.begin(); it != bodies.end(); it++)
        (*it)->prepareSteps();

    while (deltaAccumulator > subStep)
    {
        deltaAccumulator -= subStep;

        for (auto it = bodies.begin(); it != bodies.end(); it++)
        {
            (*it)->process(subStep, localGravity);
        }

        for (auto a = bodies.begin(); a != bodies.end(); a++)
        {
            for (auto b = bodies.begin(); b != bodies.end(); b++)
            {
                if (a == b)
                    break;

                if ((*a)->getMotionType() == MotionType::Static && (*b)->getMotionType() == MotionType::Static)
                    continue;

                if ((*a)->checkAABB((*b)->getAABB()))
                {
                    collisionDispatcher.collide(*a, *b, &collisionCollector);
                }
            }
        }

        if (collisionCollector.pairs.size() > 0)
        {
            CollisionSolver collisionSolver(simScale);
            for (auto pair = collisionCollector.pairs.begin(); pair != collisionCollector.pairs.end(); pair++)
            {
                collisionSolver.solve(pair->a, pair->b, pair->manifold, subStep);
            }
        }
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

std::vector<PhysicsBodyPoint> PhysicsWorld::castRay(Line ray)
{

    std::vector<PhysicsBodyPoint> points;

    Line rayLocal = Line(ray.a * simScale, ray.b * simScale);
    for (auto body = bodies.begin(); body != bodies.end(); body++)
    {
        if (!(*body)->checkAABB(rayLocal))
            continue;
        (*body)->castRay(rayLocal, &points);
    }

    return points;
}