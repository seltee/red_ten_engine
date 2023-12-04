// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "loaders3d/loaderFBX.h"
#include "loaders/stb_image.h"
#include "common/meshDescriptor.h"
#include "mesh/meshStatic.h"
#include <stdio.h>
#include <algorithm>

LoaderFBX::LoaderFBX(std::string path) : Loader3d(path)
{
}

LoaderFBX::~LoaderFBX()
{
}

MeshCompound *LoaderFBX::getMeshCompound()
{
    if (!meshCompound)
        reload();
    return meshCompound;
}

void LoaderFBX::reload()
{
    if (meshCompound)
        delete meshCompound;
    meshCompound = nullptr;

    FILE *file = fopen(path.c_str(), "rb");
    if (file == nullptr)
        return;

    // Checking header
    char buff[21];
    fread(buff, 21, 1, file);
    if (std::strcmp(buff, "Kaydara FBX Binary  ") != 0)
    {
        logger->logff("File isn't FBX %s\n", path.c_str());
        return;
    }

    fread(buff, 2, 1, file);
    if (buff[0] != 0x1A || buff[1] != 0x00)
    {
        logger->logff("Wrong FBX code %s\n", path.c_str());
        return;
    }

    unsigned int version;
    fread(&version, 4, 1, file);
    if (version != 7400)
    {
        if (version < 7400)
        {
            logger->logff("Warning! FBX version is less than 7.4, errors of reading may happen, %s\n", path.c_str());
        }
        else
        {
            logger->logff("Versions of FBX higher than 7.4 are unsupported, failed to load %s\n", path.c_str());
            return;
        }
    }

    // Passed the headers check
    meshCompound = new MeshCompound();

    // loop through the nodes
    while (1)
    {
        FBXNode *node = new FBXNode(0, file);
        node->process();
        if (node->bIsZero)
            break;
        if (node->bMarkedToRemove)
            delete node;
        else
            nodes.push_back(node);
    }

    FBXNode *objects = nullptr;
    FBXNode *connections = nullptr;

    for (auto &it : nodes)
    {
        // it->print();
        if (it->isName("Objects"))
            objects = it;
        if (it->isName("Connections"))
            connections = it;
    }

    if (!objects || !connections)
    {
        logger->logff("Objects or connections weren't found for %s\n", path.c_str());
        return;
    }

    // collecting geometry and models info
    for (auto &it : objects->children)
    {
        if (it->isName("Geometry"))
        {
            MeshDescriptor descriptor;

            FBXNode *vertices = it->findNode("Vertices");
            FBXNode *polygonVertexIndex = it->findNode("PolygonVertexIndex");
            FBXNode *elementUVs = it->findNode("LayerElementUV");
            FBXNode *elementNormals = it->findNode("LayerElementNormal");

            if (vertices && vertices->bindedData.size() > 0)
            {
                auto nodeData = vertices->bindedData.at(0);
                descriptor.provideVertex((double *)nodeData.data, nodeData.numElements);
            }
            if (polygonVertexIndex && polygonVertexIndex->bindedData.size() > 0)
            {
                auto nodeData = polygonVertexIndex->bindedData.at(0);
                descriptor.providePolygonIndexes((int *)nodeData.data, nodeData.numElements);
            }
            if (elementUVs)
            {
                FBXNode *UVs = elementUVs->findNode("UV");
                FBXNode *UVIndexes = elementUVs->findNode("UVIndex");
                if (UVs && UVs->bindedData.size() > 0)
                {
                    auto nodeData = UVs->bindedData.at(0);
                    descriptor.provideUVData((double *)nodeData.data, nodeData.numElements);
                }
                if (UVIndexes && UVIndexes->bindedData.size() > 0)
                {
                    auto nodeData = UVIndexes->bindedData.at(0);
                    descriptor.provideUVIndexes((int *)nodeData.data, nodeData.numElements);
                }
            }
            if (elementNormals)
            {
                FBXNode *normals = elementNormals->findNode("Normals");
                if (normals && normals->bindedData.size())
                {
                    auto nodeData = normals->bindedData.at(0);
                    descriptor.provideNormals((double *)nodeData.data, nodeData.numElements);
                }
            }

            unsigned long long index = *static_cast<unsigned long *>(it->bindedData.at(0).data);

            int amountOfFloats;
            float *data = descriptor.getAsFloatArray8f(&amountOfFloats);

            // create static mesh with gathered data
            MeshStatic *mesh = new MeshStatic();

            int amountOfVertexes = amountOfFloats / 8;
            int attributeSizes[3] = {3, 3, 2};
            mesh->setupFloatsArray(data, amountOfVertexes, 3, attributeSizes, true);

            delete[] data;

            meshes.push_back({mesh, index});
            continue;
        }

        if (it->isName("Model"))
        {
            FBXModelWithIndex mWithIndex;
            mWithIndex.index = *static_cast<unsigned long *>(it->bindedData.at(0).data);
            mWithIndex.name = std::string(reinterpret_cast<const char *>(it->bindedData.at(1).data));
            mWithIndex.parentIndex = 0;
            mWithIndex.position = Vector3(0.0f, 0.0f, 0.0f);
            mWithIndex.rotation = Vector3(0.0f, 0.0f, 0.0f);
            mWithIndex.scale = Vector3(1.0f, 1.0f, 1.0f);
            mWithIndex.mesh = nullptr;

            FBXNode *properties = it->findNode("Properties70");
            if (properties)
            {
                for (auto &property : properties->children)
                {
                    // n - none, p - position, r - rotation, s - scale
                    int entryIndex = 0;
                    char transformationType = 'n';
                    Vector3 out;
                    for (auto &p : property->bindedData)
                    {
                        if (p.type == 'S' && strcmp(reinterpret_cast<char *>(p.data), "Lcl Translation") == 0)
                            transformationType = 'p';
                        if (p.type == 'S' && strcmp(reinterpret_cast<char *>(p.data), "Lcl Rotation") == 0)
                            transformationType = 'r';
                        if (p.type == 'S' && strcmp(reinterpret_cast<char *>(p.data), "Lcl Scaling") == 0)
                            transformationType = 's';
                        if (p.type == 'D' && entryIndex < 3)
                        {
                            out[entryIndex] = static_cast<float>(*reinterpret_cast<double *>(p.data));
                            entryIndex++;
                        }
                    }
                    if (transformationType == 'p')
                        mWithIndex.position = out;

                    if (transformationType == 's')
                        mWithIndex.scale = out;

                    if (transformationType == 'r')
                        mWithIndex.rotation = out * CONST_PI / 180.0f;
                }
            }

            models.push_back(mWithIndex);
            continue;
        }

        if (it->isName("AnimationStack"))
        {
            auto newAnimStack = new FBXAnimationStack(it);
            animStacks.push_back(newAnimStack);
            printf("Animation Stack id %llu, l %llu, r %llu\n", newAnimStack->id, newAnimStack->localTime, newAnimStack->referenceTime);
            continue;
        }

        if (it->isName("AnimationLayer"))
        {
            auto newAnimLayer = new FBXAnimationLayer(it);
            animLayers.push_back(newAnimLayer);
            printf(
                "Animation Layer id %llu, %s\n",
                newAnimLayer->id,
                newAnimLayer->name.c_str());
            continue;
        }

        if (it->isName("AnimationCurveNode"))
        {
            auto newAnimCurveNode = new FBXAnimationCurveNode(it);
            animCurveNodes.push_back(newAnimCurveNode);
            printf("Animation Curve Node id %llu, type %s\n", newAnimCurveNode->id, newAnimCurveNode->getTypeName());
            continue;
        }

        if (it->isName("AnimationCurve"))
        {
            auto newAnimCurve = new FBXAnimationCurve(it);
            animCurves.push_back(newAnimCurve);
            printf("Animation Curve id %llu, num %i\n", newAnimCurve->id, newAnimCurve->keysCount);
            continue;
        }
    }

    // Connecting models, geometry and animations
    for (auto &it : connections->children)
    {
        unsigned long long indexFrom = *reinterpret_cast<unsigned long long *>(it->bindedData.at(1).data);
        unsigned long long indexTo = *reinterpret_cast<unsigned long long *>(it->bindedData.at(2).data);

        FBXModelWithIndex *foundModel = getModelByIndex(indexFrom);
        if (foundModel)
        {
            if (indexTo)
                foundModel->parentIndex = indexTo;
            continue;
        }

        FBXMeshWithIndex *foundMesh = getMeshByIndex(indexFrom);
        if (foundMesh)
        {
            foundModel = getModelByIndex(indexTo);
            if (foundModel)
            {
                MeshStatic *instance = reinterpret_cast<MeshStatic *>(foundMesh->mesh->createInstance());
                MeshCompoundNode *meshCompoundNode = meshCompound->addMesh(instance);
                if (foundModel->parentIndex)
                {
                    meshCompoundNode->transform.setPosition(foundModel->position);
                }
                else
                {
                    meshCompoundNode->transform.setPosition(foundModel->position);
                }

                meshCompoundNode->transform.setRotation(foundModel->rotation);
                meshCompoundNode->transform.setScale(foundModel->scale);

                foundModel->mesh = instance;
                instance->setName(foundModel->name);
            }
            continue;
        }

        FBXAnimationLayer *layer = getLayerByIndex(indexFrom);
        if (layer)
        {
            FBXAnimationStack *stack = getStackByIndex(indexTo);
            if (stack)
            {
                stack->linkLayer(layer);
            }
            continue;
        }

        FBXAnimationCurveNode *curveNode = getCurveNodeByIndex(indexFrom);
        if (curveNode)
        {
            FBXModelWithIndex *targetModel = getModelByIndex(indexTo);
            FBXAnimationLayer *targetLayer = getLayerByIndex(indexTo);
            if (targetModel)
            {
                targetModel->curveNodes.push_back(curveNode);
                curveNode->addAffectedModel(targetModel);
            }
            if (targetLayer)
            {
                targetLayer->linkCurveNode(curveNode);
            }
            continue;
        }

        FBXAnimationCurve *curve = getCurveByIndex(indexFrom);
        if (curve)
        {
            FBXAnimationCurveNode *curveNode = getCurveNodeByIndex(indexTo);
            if (curveNode)
            {
                curveNode->linkCurve(curve, it);
            }
            continue;
        }

        printf("Unknown link %llu to %llu\n", indexFrom, indexTo);
    }

    // printAnimationStructure();

    // Setting up parent links
    for (auto &model : models)
    {
        if (model.parentIndex)
        {
            auto parentModel = getModelByIndex(model.parentIndex);
            if (parentModel)
            {
                if (!meshCompound->setParent(model.mesh, parentModel->mesh))
                    logger->logff("Failed to set parent during FBX load");
            }
        }
    }

    // setting up animation
    if (animStacks.size() > 0)
    {
        auto animNames = getAnimationNames();
        for (auto &animName : animNames)
        {
            Animation *newAnimation = new Animation(animName);
            animations.push_back(newAnimation);

            for (auto &layer : animLayers)
            {
                if (layer->name == animName)
                {
                    std::vector<unsigned long long> timestamps;
                    gatherTimeStamps(layer, &timestamps);

                    for (auto &timeStamp : timestamps)
                    {
                        std::vector<FBXAnimationBinding> animBindings;
                        float floatTimeStamp = static_cast<float>(timeStamp / FBXTimeToMs) / 1000.0f;

                        for (auto &curveNode : layer->curveNodes)
                        {
                            Vector3 def = curveNode->defaultValue;
                            FBXAnimationCurve *curveX = curveNode->getXCurve();
                            FBXAnimationCurve *curveY = curveNode->getYCurve();
                            FBXAnimationCurve *curveZ = curveNode->getZCurve();

                            Vector3 out = def;

                            if (curveX)
                                out.x = getCurveLerped(curveX, floatTimeStamp);
                            if (curveY)
                                out.y = getCurveLerped(curveY, floatTimeStamp);
                            if (curveZ)
                                out.z = getCurveLerped(curveZ, floatTimeStamp);

                            // Make sure we have all have names in the list of transforms
                            for (auto &model : curveNode->affectedModels)
                            {
                                bool bFound = false;
                                for (auto &binding : animBindings)
                                {
                                    if (binding.modelName == model->name)
                                    {
                                        bFound = true;
                                        break;
                                    }
                                }
                                if (!bFound)
                                {
                                    FBXAnimationBinding binding;
                                    binding.modelName = model->name;
                                    binding.keyTransform = AnimationKeyTranform({floatTimeStamp,
                                                                                 model->position,
                                                                                 model->rotation,
                                                                                 model->scale});
                                    animBindings.push_back(binding);
                                }
                            }

                            // affect transforms in list for this curve
                            for (auto &anim : animBindings)
                            {
                                if (curveNode->hasModelName(anim.modelName))
                                {
                                    if (curveNode->type == FBXAnimationCurveNodeType::Position)
                                        anim.keyTransform.position = out;
                                    if (curveNode->type == FBXAnimationCurveNodeType::Scale)
                                        anim.keyTransform.scale = out;
                                    if (curveNode->type == FBXAnimationCurveNodeType::Rotation)
                                        anim.keyTransform.rotation = (out / 180.0f) * CONST_PI;
                                }
                            }
                        }

                        // each timestamp we add transformation to anim bindings
                        for (auto &animBinding : animBindings)
                        {
                            // printf("ANIM %f - %s\n", animBinding.keyTransform.timeStamp, animBinding.modelName.c_str());
                            // printf("p %f %f %f\n", animBinding.keyTransform.position.x, animBinding.keyTransform.position.y, animBinding.keyTransform.position.z);
                            // printf("r %f %f %f\n", animBinding.keyTransform.rotation.x, animBinding.keyTransform.rotation.y, animBinding.keyTransform.rotation.z);
                            // printf("s %f %f %f\n", animBinding.keyTransform.scale.x, animBinding.keyTransform.scale.y, animBinding.keyTransform.scale.z);
                            auto animTarget = newAnimation->createAnimationTarget(animBinding.modelName);
                            animTarget->addKey(animBinding.keyTransform);
                        }
                    }
                }

                newAnimation->recalcAnimationLength();
            }
        }
    }
}

std::vector<Animation *> LoaderFBX::getAnimations()
{
    return animations;
}

void LoaderFBX::printAnimationStructure()
{
    for (auto &stack : animStacks)
    {
        printf("Stack %llu\n", stack->id);
        for (auto &layer : stack->layers)
        {
            printf("-Layer %s\n", layer->name.c_str());
            for (auto &node : layer->curveNodes)
            {
                printf("--Node %s\n", node->getTypeName());
                for (auto &curve : node->curves)
                {
                    printf("---Curve %llu, %c\n", curve.curve->id, curve.axis);
                    for (int i = 0; i < curve.curve->keysCount; i++)
                    {
                        unsigned long long ms = curve.curve->keys[i].keyTime / FBXTimeToMs;
                        printf("----Key %llums - %f\n", ms, curve.curve->keys[i].keyValue);
                    }
                }
            }
        }
    }
}

FBXModelWithIndex *LoaderFBX::getModelByIndex(unsigned long long id)
{
    for (auto &it : models)
    {
        if (it.index == id)
            return &it;
    }
    return nullptr;
}

FBXMeshWithIndex *LoaderFBX::getMeshByIndex(unsigned long long id)
{
    for (auto &it : meshes)
    {
        if (it.index == id)
            return &it;
    }
    return nullptr;
}

FBXAnimationCurveNode *LoaderFBX::getCurveNodeByIndex(unsigned long long id)
{
    for (auto &it : animCurveNodes)
    {
        if (it->id == id)
            return it;
    }
    return nullptr;
}

FBXAnimationCurve *LoaderFBX::getCurveByIndex(unsigned long long id)
{
    for (auto &it : animCurves)
    {
        if (it->id == id)
            return it;
    }
    return nullptr;
}

FBXAnimationStack *LoaderFBX::getStackByIndex(unsigned long long id)
{
    for (auto &it : animStacks)
    {
        if (it->id == id)
            return it;
    }
    return nullptr;
}

FBXAnimationLayer *LoaderFBX::getLayerByIndex(unsigned long long id)
{
    for (auto &it : animLayers)
    {
        if (it->id == id)
            return it;
    }
    return nullptr;
}

std::vector<std::string> LoaderFBX::getAnimationNames()
{
    std::vector<std::string> animNames;
    for (auto &it : animLayers)
    {
        bool found = false;
        for (auto &animName : animNames)
        {
            if (animName == it->name)
                found = true;
        }
        if (!found)
        {
            animNames.push_back(it->name);
        }
    }
    return animNames;
}

void LoaderFBX::gatherTimeStamps(FBXAnimationLayer *layer, std::vector<unsigned long long> *timestamps)
{
    for (auto &curveNode : layer->curveNodes)
    {
        for (auto &curve : curveNode->curves)
        {
            int keysCount = curve.curve->keysCount;
            auto keys = curve.curve->keys;

            for (int i = 0; i < keysCount; i++)
            {
                auto keyTime = keys[i].keyTime;
                bool found = false;
                for (auto &time : *timestamps)
                {
                    if (time == keyTime)
                        found = true;
                }
                if (!found)
                    timestamps->push_back(keyTime);
            }
        }
    }
    std::sort(timestamps->begin(), timestamps->end());
}

float LoaderFBX::getCurveLerped(FBXAnimationCurve *curve, float time)
{
    int keysCount = curve->keysCount;
    auto keys = curve->keys;

    for (int i = 0; i < keysCount; i++)
    {
        float keyTime = static_cast<float>(keys[i].keyTime / FBXTimeToMs) / 1000.0f;
        float prevKeyTime = i > 0 ? static_cast<float>(keys[i - 1].keyTime / FBXTimeToMs) / 1000.0f : 0.0f;

        if (time < keyTime)
        {
            if (i == 0)
            {
                return keys[i].keyValue;
            }
            else
            {
                float cValue = keys[i].keyValue;
                float pValue = keys[i - 1].keyValue;
                float normalTime = (time - prevKeyTime) / (keyTime - prevKeyTime); // 0 - 1

                return pValue * (1.0f - normalTime) + cValue * normalTime;
            }
        }
    }

    return keys[keysCount - 1].keyValue;
}