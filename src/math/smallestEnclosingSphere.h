// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "math/sphere.h"
#include <algorithm>

Sphere makeSmallestSphere(const float *vData, int vertexAmount, int floatsPerVertex)
{
    Sphere sphere;
    if (vertexAmount <= 0)
    {
        return sphere;
    }

    // Find the point with the maximum extent along the x-axis
    Vector3 minPoint = Vector3(vData[0], vData[1], vData[2]);
    Vector3 maxPoint = minPoint;

    for (int i = 1; i < vertexAmount; ++i)
    {
        int shift = i * floatsPerVertex;
        const Vector3 point = Vector3(vData[shift + 0], vData[shift + 1], vData[shift + 2]);
        minPoint = glm::min(minPoint, point);
        maxPoint = glm::max(maxPoint, point);
    }

    sphere.setup((minPoint + maxPoint) * 0.5f, glm::distance(minPoint, maxPoint) * 0.5f);

    for (int i = 1; i < vertexAmount; ++i)
    {
        int shift = i * floatsPerVertex;
        const Vector3 point = Vector3(vData[shift + 0], vData[shift + 1], vData[shift + 2]);
        if (glm::distance(point, sphere.center) > sphere.radius)
        {
            // Calculate a new bounding sphere using the new point
            Vector3 d = glm::normalize(point - sphere.center);
            Vector3 newCenter = sphere.center + d * (glm::distance(sphere.center, point) - sphere.radius) * 0.5f;
            sphere.radius = (sphere.radius + glm::distance(sphere.center, point)) * 0.5f;
            sphere.center = newCenter;
        }
    }

    return sphere;
}
