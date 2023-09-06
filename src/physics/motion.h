// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "math/math.h"
#include "common/utils.h"

class Motion
{
public:
    EXPORT Motion();
    EXPORT void setProperties(float linearDamping = 0.2f, float angularDamping = 0.25f, float gravityFactor = 1.0f);
    EXPORT void setInertia(Matrix3 &interia);
    EXPORT void setMass(float mass);

    EXPORT inline float getInvMass() { return invMass; }

    EXPORT inline Vector3 getPointVelocity(Vector3 pointLocal)
    {
        return linearVelocity + glm::cross(angularVelocity, pointLocal);
    }
    EXPORT inline Matrix3 getInvertedInteria() { return invertedInteria; }

    void checkLimits();

    EXPORT void process(float delta, Vector3 gravity, Quat &orientation);

    Vector3 linearVelocity = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 angularVelocity = Vector3(0.0f, 0.0f, 0.0f);

    // Physics space force, reseted to zero on process
    Vector3 force = Vector3(0.0f, 0.0f, 0.0f);

    // Physics space torque, reseted to zero on process
    Vector3 torque = Vector3(0.0f, 0.0f, 0.0f);

protected:
    Matrix3 invertedInteria;

    // inversed mass 1 / mass
    float invMass = 1.0f;

    float linearDamping = 0.4f;
    float angularDamping = 0.4f;

    // allows object to fall slower
    float gravityFactor = 1.0f;
};