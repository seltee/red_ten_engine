// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "motion.h"

Motion::Motion()
{
}

void Motion::setProperties(float linearDamping, float angularDamping, float gravityFactor)
{
    this->linearDamping = linearDamping;
    this->angularDamping = angularDamping;
    this->gravityFactor = gravityFactor;
}

void Motion::setInertia(Matrix3 &interia)
{
    invertedInteria = glm::inverse(interia);
}

void Motion::setMass(float mass)
{
    this->invMass = 1.0f / mass;
}

void Motion::checkLimits()
{
    if (glm::length(linearVelocity) > 10.0f)
    {
        linearVelocity = glm::normalize(linearVelocity) * 10.0f;
    }
}

void Motion::process(float delta, Vector3 gravity, Quat &orientation)
{
    Matrix3 r = glm::toMat3(orientation);

    linearVelocity += (gravity * gravityFactor + invMass * force) * delta;
    angularVelocity += (r * (invertedInteria * glm::transpose(r) * torque)) * delta;

    linearVelocity -= linearVelocity * linearDamping * delta;
    angularVelocity -= angularVelocity * angularDamping * delta;

    force = Vector3(0.0f, 0.0f, 0.0f);
    torque = Vector3(0.0f, 0.0f, 0.0f);
}