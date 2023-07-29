// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "math/math.h"
#include <vector>

class PhysicsBody;

const int MAX_POINTS = 8;

/// Collision information between 2 bodies
class CollisionManifold
{
public:
    // Point of collision on the surface of object A
    Vector3 pointsOnA[MAX_POINTS];

    // Point of collision on the surface of object B
    Vector3 pointsOnB[MAX_POINTS];

    // Normal directing the shortest way body B can escape collision
    Vector3 normal[MAX_POINTS];

    // Shortest distance to escape collision by normal
    float depth[MAX_POINTS];

    // Amount of points of collision
    int collisionAmount = 0;

    inline bool addCollisionPoint(Vector3 onA, Vector3 onB, float depth, Vector3 normal)
    {
        if (collisionAmount < MAX_POINTS)
        {
            this->pointsOnA[collisionAmount] = onA;
            this->pointsOnB[collisionAmount] = onB;
            this->normal[collisionAmount] = normal;
            this->depth[collisionAmount] = depth;
            collisionAmount++;
            return true;
        }
        return false;
    }

    inline void combineIntoOne()
    {
        if (collisionAmount <= 1)
            return;

        Vector3 combinedNormal(0.0f);
        Vector3 combineOnA(0.0f), combineOnB(0.0f);
        for (int i = 0; i < collisionAmount; i++)
        {
            combineOnA += pointsOnA[i];
            combineOnB += pointsOnB[i];
            combinedNormal += normal[i] * depth[i];
        }
        combineOnA /= (float)collisionAmount;
        combineOnB /= (float)collisionAmount;
        pointsOnA[0] = combineOnA;
        pointsOnB[0] = combineOnB;
        depth[0] = glm::length(combinedNormal);
        normal[0] = glm::normalize(combinedNormal);
        collisionAmount = 1;
        
    }
};
