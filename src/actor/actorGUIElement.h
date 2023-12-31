// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "actor/actorPawn.h"
#include "camera/camera.h"
#include <list>

class ActorGUIElement : public ActorPawn
{
public:
    EXPORT ActorGUIElement();
    EXPORT ~ActorGUIElement();

    EXPORT void setActiveArea(float width, float height);
    EXPORT void setActiveArea(float l, float r, float t, float b);
    EXPORT bool isFocues();

    EXPORT void onProcess(float delta);

    EXPORT virtual void onHover();
    EXPORT virtual void onFocus();
    EXPORT virtual void onBlur();
    EXPORT virtual void onPress();
    EXPORT virtual void onRelease();

    EXPORT void setSelectiveIndex(int selectiveIndex);

    EXPORT inline void setPressID(int pressID) { this->pressID = pressID; }
    EXPORT inline int getPressID() { return pressID; }
    EXPORT static void clearPressIDStack();
    EXPORT static void addPressID(int id);
    EXPORT static int getFirstPressID();

protected:
    void onPressCheck(InputType type, int deviceIndex, int index, bool state);
    void sortSelectives();

    float activeAreaLeft = 0, activeAreaRight = 0, activeAreaTop = 0, activeAreaBottom = 0;
    int selectiveIndex = 0;
    bool bIsHovered = false;
    bool bIsFocused = false;
    int pressID = 0;

    bool bAddedToSelectives = false;
    static std::list<ActorGUIElement *> selectives;
    static std::list<int> pressIDStack;
};
