#include "collisionDispatcher.h"
#include "physics/shapes/shapePlain.h"
#include "physics/shapes/shapeSphere.h"
#include "physics/shapes/shapeBox.h"
#include "physics/shapes/shapeConvex.h"
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
        manifold.normal = difference / distance;
        manifold.depth = radius - distance;
        manifold.addCollisionPoint(Vector3(sphereCenter + manifold.normal * radius), point);

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
        manifold.normal = difference / distance;
        manifold.depth = radiusSumm - distance;
        manifold.addCollisionPoint(Vector3(sphereAPosition + manifold.normal * shapeARadius), Vector3(sphereBPosition - manifold.normal * shapeBRadius));

        collector->addBodyPair(sphereA, sphereB, manifold);
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
    float r = size.x * fabsf(glm::dot(plainNormal, axisX)) +
              size.y * fabsf(glm::dot(plainNormal, axisY)) +
              size.z * fabsf(glm::dot(plainNormal, axisZ));

    Vector3 OBBCenter = OBB->getCenterOfMass();

    // Compute distance of box center from plane
    float s = glm::dot(plainNormal, OBBCenter) - plainShape->distance;

    // Intersection occurs when distance s falls within [-r,+r] interval
    if (fabsf(s) <= r)
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
            // debug->addDebugBox(points[i] / 0.1f, 0.1f, 0.04f, Vector3(0.6f, 0.0f, 0.0f));

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
            manifold.normal = -sideNormal;
            manifold.depth = depth;
            manifold.addCollisionPoint(c, plainShape->getClosestPoint(c));

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
    Vector3 difference = closestPoint - sphereCenter;

    float distance = glm::length(difference);
    if (distance < sphereRadius)
    {
        CollisionManifold manifold;
        manifold.normal = -difference / distance;
        manifold.depth = sphereRadius - distance;
        manifold.addCollisionPoint(sphereCenter + manifold.normal * sphereRadius, closestPoint);

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