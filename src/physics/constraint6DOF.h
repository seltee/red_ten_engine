// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "constraint.h"
#include "common/utils.h"

struct Constraint6DOFDescriptor
{
    bool blockXRotation;
    bool blockYRotation;
    bool blockZRotation;
    bool blockXMoving;
    bool blockYMoving;
    bool blockZMoving;
};

class Constraint6DOF : public Constraint
{
public:
    EXPORT Constraint6DOF(const Constraint6DOFDescriptor &descriptor);
    EXPORT void processMotion(Motion *motion);
    EXPORT Vector3 processTranslation(Vector3 &translation);

protected:
    Constraint6DOFDescriptor descriptor;
};
