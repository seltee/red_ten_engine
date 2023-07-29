// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "collisionDispatcher.h"
#include "physics/shapes/shapePlain.h"
#include "physics/shapes/shapeSphere.h"
#include "physics/shapes/shapeBox.h"
#include "physics/shapes/shapeConvex.h"
#include "physics/shapes/shapeGeometry.h"
#include "physics/shapes/shapeCapsule.h"
#include "math/hullCliping.h"
#include <vector>

CollisionDispatcher::CollisionDispatcher()
{
    int amountOfTypes = (int)ShapeCollisionType::Amount;
    for (int a = 0; a < amountOfTypes; a++)
        for (int b = 0; b < amountOfTypes; b++)
            collectCollisions[a][b] = [](PhysicsBody *a, PhysicsBody *b, CollisionCollector *collector)
            {
                printf("Collision of %s with %s is not implemented\n", Shape::getTypeName(a->getType()).c_str(), Shape::getTypeName(b->getType()).c_str());
            };

    collectCollisions[(int)ShapeCollisionType::Plain][(int)ShapeCollisionType::Sphere] = [](PhysicsBody *plain, PhysicsBody *sphere, CollisionCollector *collector)
    {
        CollisionDispatcher::collideSphereVsPlain(sphere, plain, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Sphere][(int)ShapeCollisionType::Plain] = [](PhysicsBody *sphere, PhysicsBody *plain, CollisionCollector *collector)
    {
        CollisionDispatcher::collideSphereVsPlain(sphere, plain, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Sphere][(int)ShapeCollisionType::Sphere] = [](PhysicsBody *sphere1, PhysicsBody *sphere2, CollisionCollector *collector)
    {
        CollisionDispatcher::collideSphereVsSphere(sphere1, sphere2, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Sphere][(int)ShapeCollisionType::Geometry] = [](PhysicsBody *sphere, PhysicsBody *geometry, CollisionCollector *collector)
    {
        CollisionDispatcher::collideSphereVsGeometry(sphere, geometry, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Geometry][(int)ShapeCollisionType::Sphere] = [](PhysicsBody *geometry, PhysicsBody *sphere, CollisionCollector *collector)
    {
        CollisionDispatcher::collideSphereVsGeometry(sphere, geometry, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Sphere][(int)ShapeCollisionType::Convex] = [](PhysicsBody *sphere, PhysicsBody *convex, CollisionCollector *collector)
    {
        CollisionDispatcher::collideSphereVsConvex(sphere, convex, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Convex][(int)ShapeCollisionType::Sphere] = [](PhysicsBody *convex, PhysicsBody *sphere, CollisionCollector *collector)
    {
        CollisionDispatcher::collideSphereVsConvex(sphere, convex, collector);
    };

    collectCollisions[(int)ShapeCollisionType::OBB][(int)ShapeCollisionType::Plain] = [](PhysicsBody *OBB, PhysicsBody *plain, CollisionCollector *collector)
    {
        CollisionDispatcher::collideOBBVsPlain(OBB, plain, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Plain][(int)ShapeCollisionType::OBB] = [](PhysicsBody *plain, PhysicsBody *OBB, CollisionCollector *collector)
    {
        CollisionDispatcher::collideOBBVsPlain(OBB, plain, collector);
    };

    collectCollisions[(int)ShapeCollisionType::OBB][(int)ShapeCollisionType::Sphere] = [](PhysicsBody *OBB, PhysicsBody *Sphere, CollisionCollector *collector)
    {
        CollisionDispatcher::collideOBBVsSphere(OBB, Sphere, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Sphere][(int)ShapeCollisionType::OBB] = [](PhysicsBody *Sphere, PhysicsBody *OBB, CollisionCollector *collector)
    {
        CollisionDispatcher::collideOBBVsSphere(OBB, Sphere, collector);
    };

    collectCollisions[(int)ShapeCollisionType::OBB][(int)ShapeCollisionType::OBB] = [](PhysicsBody *OBB_A, PhysicsBody *OBB_B, CollisionCollector *collector)
    {
        CollisionDispatcher::collideConvexVsConvex(OBB_A, OBB_B, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Convex][(int)ShapeCollisionType::Convex] = [](PhysicsBody *convexA, PhysicsBody *convexB, CollisionCollector *collector)
    {
        CollisionDispatcher::collideConvexVsConvex(convexA, convexB, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Convex][(int)ShapeCollisionType::Plain] = [](PhysicsBody *convex, PhysicsBody *plain, CollisionCollector *collector)
    {
        CollisionDispatcher::collideConvexVsPlain(convex, plain, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Plain][(int)ShapeCollisionType::Convex] = [](PhysicsBody *plain, PhysicsBody *convex, CollisionCollector *collector)
    {
        CollisionDispatcher::collideConvexVsPlain(convex, plain, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Convex][(int)ShapeCollisionType::Geometry] = [](PhysicsBody *convex, PhysicsBody *geometry, CollisionCollector *collector)
    {
        CollisionDispatcher::collideConvexVsGeometry(convex, geometry, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Geometry][(int)ShapeCollisionType::Convex] = [](PhysicsBody *geometry, PhysicsBody *convex, CollisionCollector *collector)
    {
        CollisionDispatcher::collideConvexVsGeometry(convex, geometry, collector);
    };

    collectCollisions[(int)ShapeCollisionType::OBB][(int)ShapeCollisionType::Geometry] = [](PhysicsBody *OBB, PhysicsBody *geometry, CollisionCollector *collector)
    {
        CollisionDispatcher::collideConvexVsGeometry(OBB, geometry, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Geometry][(int)ShapeCollisionType::OBB] = [](PhysicsBody *geometry, PhysicsBody *OBB, CollisionCollector *collector)
    {
        CollisionDispatcher::collideConvexVsGeometry(OBB, geometry, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Capsule][(int)ShapeCollisionType::Plain] = [](PhysicsBody *capsule, PhysicsBody *plain, CollisionCollector *collector)
    {
        CollisionDispatcher::collideCapsuleVsPlain(capsule, plain, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Plain][(int)ShapeCollisionType::Capsule] = [](PhysicsBody *capsule, PhysicsBody *plain, CollisionCollector *collector)
    {
        CollisionDispatcher::collideCapsuleVsPlain(capsule, plain, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Capsule][(int)ShapeCollisionType::Capsule] = [](PhysicsBody *capsuleA, PhysicsBody *capsuleB, CollisionCollector *collector)
    {
        CollisionDispatcher::collideCapsuleVsCapsule(capsuleA, capsuleB, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Capsule][(int)ShapeCollisionType::Sphere] = [](PhysicsBody *capsule, PhysicsBody *sphere, CollisionCollector *collector)
    {
        CollisionDispatcher::collideCapsuleVsSphere(capsule, sphere, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Sphere][(int)ShapeCollisionType::Capsule] = [](PhysicsBody *sphere, PhysicsBody *capsule, CollisionCollector *collector)
    {
        CollisionDispatcher::collideCapsuleVsSphere(capsule, sphere, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Capsule][(int)ShapeCollisionType::OBB] = [](PhysicsBody *capsule, PhysicsBody *OBB, CollisionCollector *collector)
    {
        CollisionDispatcher::collideCapsuleVsConvex(capsule, OBB, collector);
    };

    collectCollisions[(int)ShapeCollisionType::OBB][(int)ShapeCollisionType::Capsule] = [](PhysicsBody *OBB, PhysicsBody *capsule, CollisionCollector *collector)
    {
        CollisionDispatcher::collideCapsuleVsConvex(capsule, OBB, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Capsule][(int)ShapeCollisionType::Convex] = [](PhysicsBody *capsule, PhysicsBody *convex, CollisionCollector *collector)
    {
        CollisionDispatcher::collideCapsuleVsConvex(capsule, convex, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Convex][(int)ShapeCollisionType::Capsule] = [](PhysicsBody *convex, PhysicsBody *capsule, CollisionCollector *collector)
    {
        CollisionDispatcher::collideCapsuleVsConvex(capsule, convex, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Capsule][(int)ShapeCollisionType::Geometry] = [](PhysicsBody *capsule, PhysicsBody *geometry, CollisionCollector *collector)
    {
        CollisionDispatcher::collideCapsuleVsGeometry(capsule, geometry, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Geometry][(int)ShapeCollisionType::Capsule] = [](PhysicsBody *geometry, PhysicsBody *capsule, CollisionCollector *collector)
    {
        CollisionDispatcher::collideCapsuleVsGeometry(capsule, geometry, collector);
    };
}

void CollisionDispatcher::collideSphereVsPlain(PhysicsBody *sphere, PhysicsBody *plain, CollisionCollector *collector)
{
    Vector3 sphereCenter = sphere->getCenterOfMass();
    ShapeSphere *sphereShape = (ShapeSphere *)sphere->getShape();
    ShapePlain *plainShape = (ShapePlain *)plain->getShape();
    float radius = sphereShape->getRadius();

    Vector3 point = plainShape->getClosestPoint(sphereCenter);
    Vector3 difference = point - sphereCenter;
    float distance = glm::length(difference);

    if (distance < radius)
    {
        CollisionManifold manifold;
        Vector3 normal = difference / distance;
        manifold.addCollisionPoint(Vector3(sphereCenter + normal * radius), point, radius - distance, normal);

        collector->addBodyPair(sphere, plain, manifold);
    }
}

void CollisionDispatcher::collideSphereVsSphere(PhysicsBody *sphereA, PhysicsBody *sphereB, CollisionCollector *collector)
{
    ShapeSphere *sphereShapeA = (ShapeSphere *)sphereA->getShape();
    ShapeSphere *sphereShapeB = (ShapeSphere *)sphereB->getShape();

    Vector3 sphereAPosition = sphereA->getCenterOfMass();
    Vector3 sphereBPosition = sphereB->getCenterOfMass();

    float shapeARadius = sphereShapeA->getRadius();
    float shapeBRadius = sphereShapeB->getRadius();

    Vector3 difference = sphereBPosition - sphereAPosition;
    float distance = glm::length(difference);
    float radiusSumm = shapeARadius + shapeBRadius;

    if (distance < radiusSumm)
    {
        CollisionManifold manifold;
        Vector3 normal = difference / distance;
        manifold.addCollisionPoint(Vector3(sphereAPosition + normal * shapeARadius), Vector3(sphereBPosition - normal * shapeBRadius), radiusSumm - distance, normal);

        collector->addBodyPair(sphereA, sphereB, manifold);
    }
}

void CollisionDispatcher::collideSphereVsConvex(PhysicsBody *sphere, PhysicsBody *convex, CollisionCollector *collector)
{
    ShapeSphere *sphereShape = (ShapeSphere *)sphere->getShape();
    ShapeConvex *geometryShape = (ShapeConvex *)convex->getShape();

    Vector3 center = sphere->getCenterOfMass();
    Vector3 closest = geometryShape->getClosestPointToHull(center);

    Vector3 difference = closest - center;
    float distance = glm::length(difference);
    if (distance < sphereShape->getRadius())
    {
        CollisionManifold manifold;
        Vector3 normal = difference / distance;
        Vector3 collisionPoint = Vector3(center + normal * sphereShape->getRadius());
        manifold.addCollisionPoint(collisionPoint, closest, sphereShape->getRadius() - distance, normal);

        collector->addBodyPair(sphere, convex, manifold);
    }
}

void CollisionDispatcher::collideSphereVsGeometry(PhysicsBody *sphere, PhysicsBody *geometry, CollisionCollector *collector)
{
    ShapeSphere *sphereShape = (ShapeSphere *)sphere->getShape();
    ShapeGeometry *geometryShape = (ShapeGeometry *)geometry->getShape();

    Vector3 center = sphere->getCenterOfMass();
    Vector3 closest = geometryShape->getClosestPoint(center);

    Vector3 difference = closest - center;
    float distance = glm::length(difference);
    if (distance < sphereShape->getRadius())
    {
        CollisionManifold manifold;
        Vector3 normal = difference / distance;
        Vector3 collisionPoint = Vector3(center + normal * sphereShape->getRadius());
        manifold.addCollisionPoint(collisionPoint, closest, sphereShape->getRadius() - distance, normal);

        collector->addBodyPair(sphere, geometry, manifold);
    }
}

void CollisionDispatcher::collideOBBVsPlain(PhysicsBody *OBB, PhysicsBody *plain, CollisionCollector *collector)
{
    ShapeBox *OBBShape = (ShapeBox *)OBB->getShape();
    ShapePlain *plainShape = (ShapePlain *)plain->getShape();

    Matrix4 m = glm::toMat4(OBB->getOrientation());
    Vector4 axisX4 = m * Vector4(1.0f, 0.0f, 0.0f, 1.0f);
    Vector4 axisY4 = m * Vector4(0.0f, 1.0f, 0.0f, 1.0f);
    Vector4 axisZ4 = m * Vector4(0.0f, 0.0f, 1.0f, 1.0f);

    Vector3 axisX = Vector3(axisX4.x, axisX4.y, axisX4.z);
    Vector3 axisY = Vector3(axisY4.x, axisY4.y, axisY4.z);
    Vector3 axisZ = Vector3(axisZ4.x, axisZ4.y, axisZ4.z);

    Vector3 plainNormal = plainShape->normal;

    Vector3 size = OBBShape->getSize() * 0.5f;
    float r = size.x * abs(glm::dot(plainNormal, axisX)) +
              size.y * abs(glm::dot(plainNormal, axisY)) +
              size.z * abs(glm::dot(plainNormal, axisZ));

    Vector3 OBBCenter = OBB->getCenterOfMass();

    // Compute distance of box center from plane
    float s = glm::dot(plainNormal, OBBCenter) - plainShape->distance;

    // Intersection occurs when distance s falls within [-r,+r] interval
    if (abs(s) <= r)
    {
        Vector3 closestToCenter = plainShape->getClosestPoint(OBBCenter);
        Vector3 sideNormal = glm::normalize(OBBCenter - closestToCenter);

        Matrix4 transformation;
        transformation = glm::translate(Matrix4(1.0f), Vector3(OBBCenter.x, OBBCenter.y, OBBCenter.z));
        transformation *= m;

        Vector4 points[8];
        points[0] = transformation * Vector4(size.x, size.y, size.z, 1.0f);
        points[1] = transformation * Vector4(size.x, size.y, -size.z, 1.0f);
        points[2] = transformation * Vector4(-size.x, size.y, size.z, 1.0f);
        points[3] = transformation * Vector4(-size.x, size.y, -size.z, 1.0f);
        points[4] = transformation * Vector4(size.x, -size.y, size.z, 1.0f);
        points[5] = transformation * Vector4(size.x, -size.y, -size.z, 1.0f);
        points[6] = transformation * Vector4(-size.x, -size.y, size.z, 1.0f);
        points[7] = transformation * Vector4(-size.x, -size.y, -size.z, 1.0f);

        float n = 0.0f;
        Vector3 c = Vector3(0.0f);
        float depth = 0.0f;
        for (int i = 0; i < 8; i++)
        {
            Vector3 p = Vector3(points[i].x, points[i].y, points[i].z);
            Vector3 closest = plainShape->getClosestPoint(p);
            Vector3 difference = p - closest;

            if (glm::dot(sideNormal, difference) < 0.0f)
            {
                c += p;
                n += 1.0f;
                depth = fmaxf(glm::length(difference), depth);
            }
        }
        if (n > 0.0f)
        {
            c /= n;

            CollisionManifold manifold;
            manifold.addCollisionPoint(c, plainShape->getClosestPoint(c), depth, -sideNormal);
            collector->addBodyPair(OBB, plain, manifold);
        }
    }
}

void CollisionDispatcher::collideOBBVsSphere(PhysicsBody *OBB, PhysicsBody *sphere, CollisionCollector *collector)
{
    ShapeBox *OBBShape = (ShapeBox *)OBB->getShape();
    ShapeSphere *sphereShape = (ShapeSphere *)sphere->getShape();

    Matrix4 m = glm::toMat4(OBB->getOrientation());

    Vector3 OBBCenter = OBB->getCenterOfMass();
    Vector3 sphereCenter = sphere->getCenterOfMass();
    float sphereRadius = sphereShape->getRadius();

    Matrix4 transformation;
    transformation = glm::translate(Matrix4(1.0f), Vector3(OBBCenter.x, OBBCenter.y, OBBCenter.z));
    transformation *= m;

    Vector3 closestPoint = OBBShape->getClosestPoint(transformation, sphereCenter);
    Vector3 difference = sphereCenter - closestPoint;

    float distance = glm::length(difference);
    if (distance < sphereRadius)
    {
        CollisionManifold manifold;
        Vector3 normal = difference / distance;
        manifold.addCollisionPoint(sphereCenter + normal * sphereRadius, closestPoint, sphereRadius - distance, normal);
        collector->addBodyPair(OBB, sphere, manifold);
    }
}

// SAT Hull vs Hull
// Dirk Gregorius - Robust Contact Creation for Physics Simulations, Valve Software
void CollisionDispatcher::collideConvexVsConvex(PhysicsBody *convexA, PhysicsBody *convexB, CollisionCollector *collector)
{
    ShapeConvex *convexShapeA = (ShapeConvex *)convexA->getShape();
    ShapeConvex *convexShapeB = (ShapeConvex *)convexB->getShape();

    convexShapeA->updateTransformation();
    convexShapeB->updateTransformation();

    if (!convexShapeA->getHull() || !convexShapeB->getHull())
        return;

    FaceQuery faceQueryA = convexShapeA->queryFaceDirection(convexShapeB);
    if (faceQueryA.separation > 0.0f)
        return;

    FaceQuery faceQueryB = convexShapeB->queryFaceDirection(convexShapeA);
    if (faceQueryB.separation > 0.0f)
        return;

    EdgeQuery edgeQuery = convexShapeA->queryEdgeDirection(convexShapeB);
    if (edgeQuery.separation > 0.0f)
    {
        return;
    }

    bool bIsFaceContactA = faceQueryA.separation > edgeQuery.separation;
    bool bIsFaceContactB = faceQueryB.separation > edgeQuery.separation;

    Hull *hullA = convexShapeA->getHull();
    Hull *hullB = convexShapeB->getHull();

    CollisionManifold manifold;
    if (bIsFaceContactA && bIsFaceContactB)
    {
        if (faceQueryA.separation > faceQueryB.separation)
        {
            HullCliping::clipHullAgainstHull(hullA, hullB, -faceQueryA.axis, &manifold);
        }
        else
        {
            HullCliping::clipHullAgainstHull(hullA, hullB, faceQueryB.axis, &manifold);
        }
    }
    else
    {
        HullCliping::clipHullAgainstHull(hullA, hullB, -edgeQuery.axis, &manifold);
    }

    if (manifold.collisionAmount > 0)
    {
        collector->addBodyPair(convexA, convexB, manifold);
    }
}

void CollisionDispatcher::collideConvexVsPlain(PhysicsBody *convex, PhysicsBody *plain, CollisionCollector *collector)
{
    ShapeConvex *convexShape = (ShapeConvex *)convex->getShape();
    ShapePlain *plainShape = (ShapePlain *)plain->getShape();

    Hull *hull = convexShape->getHull();
    if (!hull)
        return;

    Vector3 convexCenter = convex->getCenterOfMass();
    Vector3 closestToCenter = plainShape->getClosestPoint(convexCenter);
    Vector3 sideNormal = glm::normalize(convexCenter - closestToCenter);

    float n = 0.0f;
    Vector3 c = Vector3(0.0f);
    float depth = 0.0f;
    for (int i = 0; i < hull->amountOfVertices; i++)
    {
        Vector3 p = Vector3(hull->absoluteVerticies[i]);
        Vector3 closest = plainShape->getClosestPoint(p);
        Vector3 difference = p - closest;

        if (glm::dot(sideNormal, difference) < 0.0f)
        {
            c += p;
            n += 1.0f;
            depth = fmaxf(glm::length(difference), depth);
        }
    }
    if (n > 0.0f)
    {
        c /= n;
        CollisionManifold manifold;
        manifold.addCollisionPoint(c, plainShape->getClosestPoint(c), depth, -sideNormal);
        collector->addBodyPair(convex, plain, manifold);
    }
}

void CollisionDispatcher::collideConvexVsGeometry(PhysicsBody *convex, PhysicsBody *geometry, CollisionCollector *collector)
{
    ShapeConvex *convexShape = (ShapeConvex *)convex->getShape();
    ShapeGeometry *geometryShape = (ShapeGeometry *)geometry->getShape();
    Hull *convexHull = convexShape->getHull();
    if (!convexHull)
        return;

    ShapeConvex geometryToConvex(Vector3(0.0f));
    Vector3 verticies[3] = {Vector3(0.0f), Vector3(1.0f), Vector3(2.0f)};
    Hull *hull = geometryToConvex.setNewHull(verticies, 3);
    int pointsA[3] = {0, 1, 2};
    hull->addPolygon(pointsA, 3);

    int pointsB[3] = {0, 2, 1};
    hull->addPolygon(pointsB, 3);

    hull->hullCenter = geometry->getCenterOfMass();

    auto amountVerticies = geometryShape->getVerticiesAmount();
    auto amountTries = amountVerticies / 3;
    auto tri = geometryShape->getAbsoluteVerticies();

    CollisionManifold manifold;
    for (int i = 0; i < amountTries; i++)
    {
        int polyInside = i * 3;

        hull->absoluteVerticies[0] = tri[polyInside];
        hull->absoluteVerticies[1] = tri[polyInside + 1];
        hull->absoluteVerticies[2] = tri[polyInside + 2];
        hull->rebuildEdges();
        hull->rebuildNormals();

        Vector3 center = (hull->absoluteVerticies[0] + hull->absoluteVerticies[1] + hull->absoluteVerticies[2]) / 3.0f;
        hull->hullCenter = center;

        FaceQuery faceQueryA = geometryToConvex.queryFaceDirection(convexShape);
        if (faceQueryA.separation > 0.0f)
            continue;

        FaceQuery faceQueryB = convexShape->queryFaceDirection(&geometryToConvex);
        if (faceQueryB.separation > 0.0f)
            continue;

        EdgeQuery edgeQuery = geometryToConvex.queryEdgeDirection(convexShape);
        if (edgeQuery.separation > 0.0f)
            continue;

        bool bIsFaceContactA = faceQueryA.separation > edgeQuery.separation;
        bool bIsFaceContactB = faceQueryB.separation > edgeQuery.separation;

        if (bIsFaceContactA && bIsFaceContactB)
        {
            if (faceQueryA.separation > faceQueryB.separation)
                HullCliping::clipHullAgainstHull(convexHull, hull, faceQueryA.axis, &manifold);
            else
                HullCliping::clipHullAgainstHull(convexHull, hull, -faceQueryB.axis, &manifold);
        }
        else
        {
            HullCliping::clipHullAgainstHull(convexHull, hull, edgeQuery.axis, &manifold);
        }
    }
    if (manifold.collisionAmount > 0)
    {
        // printf("Collisions: %i\n", manifold.collisionAmount);
        manifold.combineIntoOne();
        // printf("%f %f %f, %f\n", manifold.normal.x, manifold.normal.y, manifold.normal.z, manifold.depth);
        // printf("%f %f %f - %f %f %f\n", manifold.pointsOnA[0].x, manifold.pointsOnA[0].y, manifold.pointsOnA[0].z, manifold.pointsOnB[0].x, manifold.pointsOnB[0].y, manifold.pointsOnB[0].z);
        if (manifold.depth[0] > 0.0f)
            collector->addBodyPair(convex, geometry, manifold);
    }
}

void CollisionDispatcher::collideCapsuleVsPlain(PhysicsBody *capsule, PhysicsBody *plain, CollisionCollector *collector)
{
    ShapeCapsule *capsuleShape = (ShapeCapsule *)capsule->getShape();
    ShapePlain *plainShape = (ShapePlain *)plain->getShape();
    float radius = capsuleShape->getRadius();

    Vector3 pointA = capsuleShape->getAbsoluteCapsule().a;
    Vector3 pointB = capsuleShape->getAbsoluteCapsule().b;

    Vector3 closestA = plainShape->getClosestPoint(pointA);
    Vector3 closestB = plainShape->getClosestPoint(pointB);

    float distA = glm::length(pointA - closestA);
    float distB = glm::length(pointB - closestB);

    Vector3 point, segmentPoint;
    float distance;
    if (fabsf(distA - distB) < 0.00001f)
    {
        point = (closestA + closestB) / 2.0f;
        segmentPoint = (pointA + pointB) / 2.0f;
        distance = distA;
    }
    else
    {
        if (distA < distB)
        {
            point = closestA;
            segmentPoint = pointA;
            distance = distA;
        }
        else
        {
            point = closestB;
            segmentPoint = pointB;
            distance = distB;
        }
    }

    if (distance < radius)
    {
        CollisionManifold manifold;
        manifold.addCollisionPoint(point, point, radius - distance, plainShape->normal);
        collector->addBodyPair(plain, capsule, manifold);
    }
}

void CollisionDispatcher::collideCapsuleVsCapsule(PhysicsBody *capsuleA, PhysicsBody *capsuleB, CollisionCollector *collector)
{
    ShapeCapsule *capsuleShapeA = (ShapeCapsule *)capsuleA->getShape();
    ShapeCapsule *capsuleShapeB = (ShapeCapsule *)capsuleB->getShape();

    Segment segmentA = capsuleShapeA->getAbsoluteCapsule();
    Segment segmentB = capsuleShapeB->getAbsoluteCapsule();

    float s, t;
    Vector3 onA, onB;
    getClosestPointSegmentSegment(segmentA.a, segmentA.b, segmentB.a, segmentB.b, s, t, onA, onB);
    float distance = glm::length(onA - onB);

    float rSumm = capsuleShapeA->getRadius() + capsuleShapeB->getRadius();
    if (distance < rSumm)
    {
        CollisionManifold manifold;
        Vector3 normal = glm::normalize(onB - onA);
        manifold.addCollisionPoint(onA + normal * capsuleShapeA->getRadius(), onB - normal * capsuleShapeB->getRadius(), rSumm - distance, normal);
        collector->addBodyPair(capsuleA, capsuleB, manifold);
    }
}

void CollisionDispatcher::collideCapsuleVsSphere(PhysicsBody *capsule, PhysicsBody *sphere, CollisionCollector *collector)
{
    ShapeCapsule *capsuleShape = (ShapeCapsule *)capsule->getShape();
    ShapeSphere *sphereShape = (ShapeSphere *)sphere->getShape();

    Segment segment = capsuleShape->getAbsoluteCapsule();
    Vector3 sphereCenter = sphere->getCenterOfMass();

    Vector3 closest = segment.getClosestPoint(sphereCenter);
    Vector3 difference = sphereCenter - closest;
    float distance = glm::length(difference);
    float rSumm = sphereShape->getRadius() + capsuleShape->getRadius();
    if (distance < rSumm)
    {
        CollisionManifold manifold;
        Vector3 normal = difference / distance;
        manifold.addCollisionPoint(closest + normal * capsuleShape->getRadius(), sphereCenter - normal * sphereShape->getRadius(), rSumm - distance, normal);
        collector->addBodyPair(capsule, sphere, manifold);
    }
}

void CollisionDispatcher::collideCapsuleVsConvex(PhysicsBody *capsule, PhysicsBody *convex, CollisionCollector *collector)
{
    ShapeCapsule *capsuleShape = (ShapeCapsule *)capsule->getShape();
    ShapeConvex *convexShape = (ShapeConvex *)convex->getShape();
    Segment segment = capsuleShape->getAbsoluteCapsule();

    float radius = capsuleShape->getRadius();

    FaceQuery faceQuery = convexShape->queryFaceDirection(capsuleShape->getAsConvex());
    if (faceQuery.separation > radius)
        return;

    CollisionManifold manifold;

    if (faceQuery.separation > 0.0f)
    {
        // Outside
        Vector3 poly[16];
        int amount = faceQuery.polygon->pointsAmount;
        for (int i = 0; i < amount; i++)
        {
            poly[i] = convexShape->getHull()->absoluteVerticies[faceQuery.polygon->points[i]];
        }

        Vector3 onPoly, onSegment;

        float distance = segment.getClosestPointToPoly(poly, amount, onPoly, onSegment);
        Vector3 difference = onSegment - onPoly;
        if (distance < radius && distance > 0.0f)
        {
            Vector3 normal = difference / distance;
            manifold.addCollisionPoint(onPoly, onSegment + normal * radius, radius - distance, normal);
            collector->addBodyPair(convex, capsule, manifold);
        }
    }
    else
    {
        // Inside
        Vector3 normal = faceQuery.axis;
        Vector3 closest = segment.getClosestPoint(convex->getCenterOfMass());
        manifold.addCollisionPoint(closest + normal * radius, closest + normal * radius, radius - faceQuery.separation, normal);
        collector->addBodyPair(convex, capsule, manifold);
    }
}

void CollisionDispatcher::collideCapsuleVsGeometry(PhysicsBody *capsule, PhysicsBody *geometry, CollisionCollector *collector)
{
    ShapeCapsule *capsuleShape = (ShapeCapsule *)capsule->getShape();
    ShapeGeometry *geometryShape = (ShapeGeometry *)geometry->getShape();
    Segment segment = capsuleShape->getAbsoluteCapsule();

    float radius = capsuleShape->getRadius();

    Vector3 onSegment, onGeometry;
    float distance = geometryShape->getClosestPoint(segment, onSegment, onGeometry);

    if (distance < radius)
    {
        CollisionManifold manifold;

        Vector3 normal = glm::normalize(onGeometry - onSegment);
        manifold.addCollisionPoint(onSegment + normal * capsuleShape->getRadius(), onGeometry, radius - distance, normal);
        collector->addBodyPair(capsule, geometry, manifold);
    }
}
