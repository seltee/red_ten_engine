// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "FBXNode.h"

// KFbxAnimCurve: an animation curve (often called a function curve or FCurve) affects how an animatable attribute of an FBX object varies with time. In FBX, attributes are implemented as FBX properties.
// An animation curve can be connected to many curve nodes. Accordingly, one animation curve can animate many properties of many FBX objects.

struct FBXAnimationCurveKey
{
    long long keyTime;
    float keyValue;
};

class FBXAnimationCurve
{
public:
    FBXAnimationCurve(FBXNode *node);

    unsigned long long id = 0;
    unsigned int keyVer = 0;

    int keysCount = 0;
    FBXAnimationCurveKey *keys = nullptr;

    unsigned long long *refs = nullptr;
    int refsAmount = 0;
};
