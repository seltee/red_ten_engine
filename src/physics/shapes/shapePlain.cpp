#include "shapePlain.h"
#include "physics/physicsWorld.h"

ShapePlain::ShapePlain(const Vector3 &normal, float distance, PhysicsWorld *world) : Shape(Vector3(0.0f, 0.0f, 0.0f))
{
    this->normal = glm::normalize(normal);
    this->distance = distance * world->getSimScale();
    mass = 99999999.0f;
}

ShapeCollisionType ShapePlain::getType()
{
    return ShapeCollisionType::Plain;
}

bool ShapePlain::testRay(const Segment &line, std::vector<RayCollisionPoint> *points)
{
    // Compute the t value for the directed line ab intersecting the plane
    Vector3 ab = line.b - line.a;
    float t = (distance - glm::dot(normal, line.a)) / glm::dot(normal, ab);
    // If t in [0..1] compute and return intersection point
    if (t >= 0.0f && t <= 1.0f)
    {
        points->push_back({line.a + t * ab, t * glm::length(ab)});
        return true;
    }
    // Else no intersection
    return false;
}

AABB ShapePlain::getAABB()
{
    return AABB(Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX), Vector3(FLT_MAX, FLT_MAX, FLT_MAX));
}