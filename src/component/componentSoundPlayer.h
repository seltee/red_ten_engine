// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "common/soundPlayer.h"
#include "component/component.h"
#include "resource/resourceSound.h"

class ComponentSoundPlayer : public Component, public SoundPlayer
{
public:
    EXPORT void onRenderQueue(RenderQueue *renderQueue) override;
};
