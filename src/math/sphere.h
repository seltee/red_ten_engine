// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "math/math.h"

class Sphere
{
public:
    inline void setup(Vector3 center, float radius)
    {
        this->radius = radius;
        this->center = center;
    }

    inline bool contains(const Vector3 &p)
    {
        return glm::length(center - p) <= radius;
    }

    inline bool isSphereInFrustum(Matrix4 *mModelView, const Vector4 *planes)
    {
        Vector3 centerClip = Vector3(*mModelView * Vector4(center, 1.0f));
        float dRadius = recalcRadius(mModelView);
        for (int i = 0; i < 6; ++i)
        {
            float distance = glm::dot(Vector3(planes[i]), Vector3(centerClip)) + planes[i].w;
            if (distance < -dRadius)
                return false; // Sphere is entirely outside the frustum
        }
        return true; // Sphere is inside or intersecting the frustum
    }

    inline float recalcRadius(Matrix4 *mModelViewProjection)
    {
        Vector3 scalingFactors = Vector3(glm::length(Vector3((*mModelViewProjection)[0])),
                                         glm::length(Vector3((*mModelViewProjection)[1])),
                                         glm::length(Vector3((*mModelViewProjection)[2])));

        return radius * glm::compMax(scalingFactors);
    }

    float radius = 0.01f;
    Vector3 center = Vector3(0.0f, 0.0f, 0.0f);
};