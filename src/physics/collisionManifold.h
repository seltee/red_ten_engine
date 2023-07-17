#pragma once
#include "math/math.h"
#include <vector>

class PhysicsBody;

const int MAX_POINTS = 16;

/// Collision information between 2 bodies
class CollisionManifold
{
public:
    // Normal directing the shortest way body B can escape collision
    Vector3 normal;

    // Shortest distance to escape collision by normal
    float depth;

    // Point of collision on the surface of object A
    Vector3 pointsOnA[MAX_POINTS];

    // Point of collision on the surface of object B
    Vector3 pointsOnB[MAX_POINTS];

    // Amount of points of collision
    int collisionAmount = 0;

    inline bool addCollisionPoint(Vector3 onA, Vector3 onB)
    {
        if (collisionAmount < MAX_POINTS)
        {
            pointsOnA[collisionAmount] = onA;
            pointsOnB[collisionAmount] = onB;
            collisionAmount++;
            return true;
        }
        return false;
    }
};
