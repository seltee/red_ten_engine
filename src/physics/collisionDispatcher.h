// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

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
    static void collideSphereVsConvex(PhysicsBody *sphere, PhysicsBody *convex, CollisionCollector *collector);
    static void collideSphereVsGeometry(PhysicsBody *sphere, PhysicsBody *geometry, CollisionCollector *collector);
    static void collideOBBVsPlain(PhysicsBody *OBB, PhysicsBody *plain, CollisionCollector *collector);
    static void collideOBBVsSphere(PhysicsBody *OBB, PhysicsBody *sphere, CollisionCollector *collector);
    static void collideConvexVsConvex(PhysicsBody *convexA, PhysicsBody *convexB, CollisionCollector *collector);
    static void collideConvexVsPlain(PhysicsBody *convex, PhysicsBody *plain, CollisionCollector *collector);
    static void collideConvexVsGeometry(PhysicsBody *convex, PhysicsBody *geometry, CollisionCollector *collector);
    static void collideCapsuleVsPlain(PhysicsBody *capsule, PhysicsBody *plain, CollisionCollector *collector);
    static void collideCapsuleVsCapsule(PhysicsBody *capsuleA, PhysicsBody *capsuleB, CollisionCollector *collector);
    static void collideCapsuleVsSphere(PhysicsBody *capsule, PhysicsBody *sphere, CollisionCollector *collector);
    static void collideCapsuleVsConvex(PhysicsBody *capsule, PhysicsBody *convex, CollisionCollector *collector);
    static void collideCapsuleVsGeometry(PhysicsBody *capsule, PhysicsBody *geometry, CollisionCollector *collector);

protected:
    CollectCollisions collectCollisions[(int)ShapeCollisionType::Amount][(int)ShapeCollisionType::Amount];
};