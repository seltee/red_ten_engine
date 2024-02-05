// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once

#include "loaders3d/loader3d.h"
#include "loaders3d/FBX/FBXNode.h"
#include "loaders3d/FBX/FBXAnimationStack.h"
#include "loaders3d/FBX/FBXAnimationCurveNode.h"
#include "loaders3d/FBX/FBXAnimationCurve.h"
#include "loaders3d/FBX/FBXAnimationLayer.h"
#include "connector/withLogger.h"
#include "connector/withRenderer.h"
#include "animation/animation.h"

const unsigned long long FBXTimeToMs = 46186158;

struct FBXMeshWithIndex
{
    MeshStatic *mesh;
    unsigned long long index;
};

struct FBXModelWithIndex
{
    unsigned long long index;
    unsigned long long parentIndex;
    Vector3 position, rotation, scale;
    std::string name;
    MeshStatic *mesh;
    std::vector<FBXAnimationCurveNode *> curveNodes;
    MeshCompoundNode *meshCompoundNode;
};

struct FBXAnimationBinding
{
    AnimationKeyTranform keyTransform;
    std::string modelName;
};

class LoaderFBX : public Loader3d,
                  public WithLogger,
                  public WithRenderer
{
public:
    EXPORT LoaderFBX(std::string path);
    EXPORT ~LoaderFBX();

    MeshCompound *getMeshCompound();

    std::vector<Animation *> getAnimations();

protected:
    void reload();

    void printAnimationStructure();

    FBXModelWithIndex *getModelByIndex(unsigned long long id);
    FBXMeshWithIndex *getMeshByIndex(unsigned long long id);
    FBXAnimationStack *getStackByIndex(unsigned long long id);
    FBXAnimationLayer *getLayerByIndex(unsigned long long id);
    FBXAnimationCurveNode *getCurveNodeByIndex(unsigned long long id);
    FBXAnimationCurve *getCurveByIndex(unsigned long long id);

    std::vector<std::string> getAnimationNames();

    void gatherTimeStamps(FBXAnimationLayer *layer, std::vector<unsigned long long> *timestamps);
    float getCurveLerped(FBXAnimationCurve *curve, float time);

    std::vector<FBXMeshWithIndex> meshes;
    std::vector<FBXModelWithIndex> models;
    std::vector<FBXAnimationStack *> animStacks;
    std::vector<FBXAnimationLayer *> animLayers;
    std::vector<FBXAnimationCurveNode *> animCurveNodes;
    std::vector<FBXAnimationCurve *> animCurves;

    MeshCompound *meshCompound = nullptr;

    std::vector<Animation *> animations;

    std::vector<FBXNode *> nodes;
    FBXNodeRecordHeader header;
};
