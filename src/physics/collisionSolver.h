#pragma once
#include "physics/physicsBody.h"
#include "connector/withDebug.h"
#include "physics/collisionManifold.h"

struct Collision
{
    PhysicsBody *a;
    PhysicsBody *b;
    CollisionPoint point;
};

class CollisionSolver : public WithDebug
{
public:
    CollisionSolver(float simScale);

    void solve(PhysicsBody *a, PhysicsBody *b, CollisionManifold &manifold, float delta);

    float solveAxis(
        PhysicsBody *a,
        PhysicsBody *b,
        Vector3 pointA,
        Vector3 pointB,
        Vector3 velocityBodyA,
        Vector3 velocityBodyB,
        Vector3 angularVelocityBodyA,
        Vector3 angularVelocityBodyB,
        Vector3 axis,
        float fMin,
        float fMax,
        float bias);

protected:
    float simScale;
    float totalLambda;
};
