// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "math/primitives.h"
#include "math/AABB.h"
#include "math/plain.h"
#include "math/segment.h"
#include "math/algorithms.h"
#include <vector>

// Möller–Trumbore intersection algorithm
inline bool testRayAgainstTriangle(const Vector3 *tri, const Segment &line, float &distance, Vector3 &p)
{
    const float epsilon = 0.000001f;
    Vector3 e0 = tri[1] - tri[0];
    Vector3 e1 = tri[2] - tri[0];

    Vector3 dir = line.b - line.a;
    Vector3 dir_norm = glm::normalize(dir);

    Vector3 h = glm::cross(dir_norm, e1);
    const float a = glm::dot(e0, h);

    if (a > -epsilon && a < epsilon)
    {
        return false;
    }

    Vector3 s = line.a - tri[0];
    const float f = 1.0f / a;
    const float u = f * glm::dot(s, h);

    if (u < 0.0f || u > 1.0f)
    {
        return false;
    }

    Vector3 q = glm::cross(s, e0);
    const float v = f * glm::dot(dir_norm, q);

    if (v < 0.0f || u + v > 1.0f)
    {
        return false;
    }

    const float t = f * glm::dot(e1, q);
    const float len = glm::length(dir);
    if (t > epsilon && t < len)
    {
        p = line.a + dir_norm * t;
        distance = t / len;
        return true;
    }
    return false;
}

// from Christofer Ericson - real time collision detection
// Sutherland-Hodgman algorithm
inline void clipPolygon(std::vector<Vector3> *out, const Vector3 *polygon, const Vector3 &plainNormal, const Vector3 &plainPoint)
{
    Vector3 vertex1 = polygon[0];
    float distance1 = glm::dot(plainNormal, vertex1 - plainPoint);

    for (int index = 0; index < 3; ++index)
    {
        Vector3 vertex2 = polygon[index];
        float distance2 = glm::dot(plainNormal, vertex2 - plainPoint);

        if (distance1 <= 0.0f && distance2 <= 0.0f)
        {
            // Both vertices are behind the plane - keep vertex2
            out->push_back(vertex2);
        }
        else if (distance1 <= 0.0f && distance2 > 0.0f)
        {
            // Vertex1 is behind the plane, vertex2 is in front -> intersection point
            float fraction = distance1 / (distance1 - distance2);
            Vector3 position = vertex1 + fraction * (vertex2 - vertex1);

            // Keep intersection point
            out->push_back(position);
        }
        else if (distance2 <= 0.0f && distance1 > 0)
        {
            // Vertex2 is behind the plane, vertex1 is in front -> intersection point
            float fraction = distance1 / (distance1 - distance2);
            Vector3 position = vertex1 + fraction * (vertex2 - vertex1);

            out->push_back(position);

            // And also keep vertex2
            out->push_back(vertex2);
        }

        // Keep vertex2 as starting vertex for next edge
        vertex1 = vertex2;
        distance1 = distance2;
    }
}
