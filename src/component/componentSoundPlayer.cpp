// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "component/componentSoundPlayer.h"
#include "math/math.h"

void ComponentSoundPlayer::onRender(Matrix4 &vpMatrix, Transformation *tf)
{
    Vector4 v(0, 0, 0, 1.0f);
    Vector4 position = *tf->getModelMatrix() * v;
    setPosition(Vector3(position.x, position.y, position.z));
}