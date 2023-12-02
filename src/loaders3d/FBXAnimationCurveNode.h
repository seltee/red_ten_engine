// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "FBXNode.h"
#include "loaders3d/FBXAnimationCurve.h"
#include "math/math.h"
#include <vector>
#include <string>

// An animation curve node is the connection point between animation curves and FBX properties.
// A curve node can be connected to only one property of one FBX object.
// Properties such as KFbxNode::LclTranslation contain more than one value (X, Y, Z).

class FBXModelWithIndex;

// type n - none, x - x, y - y, z - z
struct FBXCurveBinding
{
    FBXAnimationCurve *curve;
    char axis;
};

enum class FBXAnimationCurveNodeType
{
    Unknown,
    Position,
    Scale,
    Rotation
};

class FBXAnimationCurveNode
{
public:
    FBXAnimationCurveNode(FBXNode *node);
    void linkCurve(FBXAnimationCurve *curve, FBXNode *node);
    void addAffectedModel(FBXModelWithIndex *model);
    const char *getTypeName();

    bool hasModelName(std::string modelName);

    FBXAnimationCurve *getXCurve();
    FBXAnimationCurve *getYCurve();
    FBXAnimationCurve *getZCurve();

    FBXAnimationCurveNodeType type;
    unsigned long long id;

    Vector3 defaultValue;

    std::vector<FBXCurveBinding> curves;
    std::vector<FBXModelWithIndex *> affectedModels;
};
