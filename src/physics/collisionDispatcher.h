#pragma once
#include "physics/shapes/shape.h"
#include "physics/collisionCollector.h"
#include "physics/physicsBody.h"
#include "connector/withDebug.h"

typedef void (*CollectCollisions)(PhysicsBody *a, PhysicsBody *b, CollisionCollector *collector);

class CollisionDispatcher : public WithDebug
{
public:
    CollisionDispatcher();
    EXPORT inline void collide(PhysicsBody *a, PhysicsBody *b, CollisionCollector *collector) { collectCollisions[(int)a->getType()][(int)b->getType()](a, b, collector); }

    static void collideSphereVsPlain(PhysicsBody *sphere, PhysicsBody *plain, CollisionCollector *collector);
    static void collideSphereVsSphere(PhysicsBody *sphereA, PhysicsBody *sphereB, CollisionCollector *collector);
    static void collideOBBVsPlain(PhysicsBody *OBB, PhysicsBody *plain, CollisionCollector *collector);
    static void collideOBBVsSphere(PhysicsBody *OBB, PhysicsBody *sphere, CollisionCollector *collector);
    static void collideConvexVsConvex(PhysicsBody *convexA, PhysicsBody *convexB, CollisionCollector *collector);


protected:
    CollectCollisions collectCollisions[(int)ShapeCollisionType::Amount][(int)ShapeCollisionType::Amount];
};