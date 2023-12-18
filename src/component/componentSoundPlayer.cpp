// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "component/componentSoundPlayer.h"
#include "math/math.h"

void ComponentSoundPlayer::onRenderQueue(RenderQueue *renderQueue)
{
    Vector4 v(0, 0, 0, 1.0f);
    Matrix4 mModel = *owner->transform.getModelMatrix() * *transform.getModelMatrix();
    Vector4 position = mModel * v;
    setPosition(Vector3(position.x, position.y, position.z));
}