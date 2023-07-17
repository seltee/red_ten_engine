#pragma once
#include "math/glm/mat4x4.hpp"
#include "math/glm/vec4.hpp"
#include "math/glm/gtc/quaternion.hpp"
#include "math/glm/gtx/quaternion.hpp"
#include <math.h>

typedef glm::vec4 Vector4;
typedef glm::vec3 Vector3;
typedef glm::vec2 Vector2;

typedef glm::mat3x3 Matrix3;
typedef glm::mat4x4 Matrix4;

typedef glm::quat Quat;

struct PointWithDirection
{
    Vector3 vPosition;
    Vector3 vDirection;
};

struct Triangle
{
    Vector3 a;
    Vector3 b;
    Vector3 c;
    Vector3 n;
};

struct HullPolygon
{
    unsigned short int points[7];
    unsigned short int pointsAmount;
    Vector3 normal;
    Vector3 absoluteNormal;
};

struct HullEdge
{
    int a;
    int b;
    HullPolygon *polygon;
};


inline Vector3 getNormalizedPerpendicular(Vector3 v)
{
    if (fabsf(v.x) > fabsf(v.y))
    {
        float len = sqrtf(v.x * v.x + v.z * v.z);
        return Vector3(v.z, 0.0f, -v.x) / len;
    }
    else
    {
        float len = sqrtf(v.y * v.y + v.z * v.z);
        return Vector3(0.0f, v.z, -v.y) / len;
    }
}