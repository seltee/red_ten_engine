// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "constraint6DOF.h"

Constraint6DOF::Constraint6DOF(const Constraint6DOFDescriptor &descriptor)
{
    this->descriptor = descriptor;
}

void Constraint6DOF::processMotion(Motion *motion)
{
    if (descriptor.blockXRotation)
        motion->angularVelocity.x = 0.0f;
    if (descriptor.blockYRotation)
        motion->angularVelocity.y = 0.0f;
    if (descriptor.blockZRotation)
        motion->angularVelocity.z = 0.0f;
    if (descriptor.blockXMoving)
        motion->linearVelocity.x = 0.0f;
    if (descriptor.blockYMoving)
        motion->linearVelocity.y = 0.0f;
    if (descriptor.blockZMoving)
        motion->linearVelocity.z = 0.0f;
}

Vector3 Constraint6DOF::processTranslation(Vector3 &translation)
{
    if (descriptor.blockXMoving)
        translation.x = 0.0f;
    if (descriptor.blockYMoving)
        translation.y = 0.0f;
    if (descriptor.blockZMoving)
        translation.z = 0.0f;
    return translation;
}