// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "loaders3d/FBXAnimationCurveNode.h"
#include "loaders3d/loaderFBX.h"

FBXAnimationCurveNode::FBXAnimationCurveNode(FBXNode *node)
{
    id = *reinterpret_cast<unsigned long long *>(node->bindedData.at(0).data);
    auto typeData = node->bindedData.at(1);
    if (typeData.type == 'S' && strcmp(reinterpret_cast<char *>(typeData.data), "S") == 0)
        type = FBXAnimationCurveNodeType::Scale;
    if (typeData.type == 'S' && strcmp(reinterpret_cast<char *>(typeData.data), "T") == 0)
        type = FBXAnimationCurveNodeType::Position;
    if (typeData.type == 'S' && strcmp(reinterpret_cast<char *>(typeData.data), "R") == 0)
        type = FBXAnimationCurveNodeType::Rotation;

    auto props = node->findNode("Properties70");
    if (props)
    {
        for (auto &property : props->children)
        {
            char type = 'n';
            for (auto &p : property->bindedData)
            {
                if (p.type == 'S' && strcmp(reinterpret_cast<char *>(p.data), "d|X") == 0)
                    type = 'x';
                if (p.type == 'S' && strcmp(reinterpret_cast<char *>(p.data), "d|Y") == 0)
                    type = 'y';
                if (p.type == 'S' && strcmp(reinterpret_cast<char *>(p.data), "d|Z") == 0)
                    type = 'z';

                if (p.type == 'D')
                {
                    if (type == 'x')
                        defaultValue.x = static_cast<float>(*reinterpret_cast<double *>(p.data));
                    if (type == 'y')
                        defaultValue.y = static_cast<float>(*reinterpret_cast<double *>(p.data));
                    if (type == 'z')
                        defaultValue.z = static_cast<float>(*reinterpret_cast<double *>(p.data));
                }
            }
        }
    }
}

void FBXAnimationCurveNode::linkCurve(FBXAnimationCurve *curve, FBXNode *node)
{
    const char *targetName = node->numProperties >= 4 && node->bindedData.at(3).type == 'S' ? reinterpret_cast<char *>(node->bindedData.at(3).data) : nullptr;
    if (targetName)
    {
        char type = 'n';
        if (strcmp(targetName, "d|X") == 0)
            type = 'x';
        if (strcmp(targetName, "d|Y") == 0)
            type = 'y';
        if (strcmp(targetName, "d|Z") == 0)
            type = 'z';

        curves.push_back({curve, type});
    }
    else
    {
        printf("LINK WITHOUT PROPERTY NAME\n");
    }
}

void FBXAnimationCurveNode::addAffectedModel(FBXModelWithIndex *model)
{
    affectedModels.push_back(model);
}

const char *FBXAnimationCurveNode::getTypeName()
{
    switch (type)
    {
    case FBXAnimationCurveNodeType::Scale:
        return "scale";

    case FBXAnimationCurveNodeType::Position:
        return "position";

    case FBXAnimationCurveNodeType::Rotation:
        return "rotation";

    default:
        return "unknown";
        break;
    }
}

bool FBXAnimationCurveNode::hasModelName(std::string modelName)
{
    for (auto &model : affectedModels)
    {
        if (model->name == modelName)
            return true;
    }

    return false;
}

FBXAnimationCurve *FBXAnimationCurveNode::getXCurve()
{
    for (auto &curve : curves)
    {
        if (curve.axis == 'x')
            return curve.curve;
    }
    return nullptr;
}

FBXAnimationCurve *FBXAnimationCurveNode::getYCurve()
{
    for (auto &curve : curves)
    {
        if (curve.axis == 'y')
            return curve.curve;
    }
    return nullptr;
}

FBXAnimationCurve *FBXAnimationCurveNode::getZCurve()
{
    for (auto &curve : curves)
    {
        if (curve.axis == 'z')
            return curve.curve;
    }
    return nullptr;
}

/*
AnimationCurveNode
* 601678870(L)
* S(S)
 Properties70
  P
  * d|X(S)
  * Number(S)
  * A(S)
  * 1.752912(D)
  P
  * d|Y(S)
  * Number(S)
  * A(S)
  * 0.565464(D)
  P
  * d|Z(S)
  * Number(S)
  * A(S)
  * 0.565464(D)
  */