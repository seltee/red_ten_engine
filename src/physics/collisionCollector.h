#pragma once
#include "collisionManifold.h"
#include <vector>
#include <thread>
#include <mutex>

class PhysicsBody;

struct CollisionPair
{
    PhysicsBody *a;
    PhysicsBody *b;
    CollisionManifold manifold;
};

class CollisionCollector
{
public:
    EXPORT inline void addBodyPair(PhysicsBody *a, PhysicsBody *b, CollisionManifold manifold)
    {
        lock.lock();
        pairs.push_back({a, b, manifold});
        lock.unlock();
    }

    std::vector<CollisionPair> pairs;

protected:
    std::mutex lock;
};