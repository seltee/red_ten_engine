#pragma once
#include "collisionManifold.h"
#include <vector>

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
    EXPORT inline void addBodyPair(PhysicsBody *a, PhysicsBody *b, CollisionManifold manifold) { pairs.push_back({a, b, manifold}); }

    std::vector<CollisionPair> pairs;
};