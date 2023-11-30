// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once

#include "loaders3d/loader3d.h"
#include "connector/withLogger.h"

class FBXNode;

struct FBXMeshWithIndex
{
    MeshStatic *mesh;
    unsigned long long index;
};

struct FBXModelWithIndex
{
    unsigned long long index;
    Vector3 position, rotation, scale;
    std::string name;
};

struct FBXNodeDataBinding
{
    char type;
    void *data;
    unsigned int numElements;
};

struct FBXNodeRecordHeader
{
    unsigned int endOffset;
    unsigned int numProperties;
    unsigned int propertyListLen;
};

class LoaderFBX : public Loader3d, public WithLogger
{
public:
    EXPORT LoaderFBX(std::string path);
    EXPORT ~LoaderFBX();

    MeshCompound *getMeshCompound();

protected:
    void reload();

    FBXModelWithIndex *getModelByIndex(unsigned long long index);
    FBXMeshWithIndex *getMeshByIndex(unsigned long long index);

    std::vector<FBXMeshWithIndex> meshes;
    std::vector<FBXModelWithIndex> models;

    MeshCompound *meshCompound = nullptr;

    std::vector<FBXNode *> nodes;
    FBXNodeRecordHeader header;
};

class FBXNode
{
public:
    FBXNode(int level, FILE *file);

    void process();
    void readHead();
    void readValues();
    void print();
    bool isName(const char *name);
    FBXNode *findNode(const char *name);
    bool doSkip(const char *name);

    std::vector<FBXNode *> children;

    unsigned int endOffset = 0;
    unsigned int numProperties = 0;
    unsigned int propertyListLen = 0;
    char *name = nullptr;
    bool bIsZero = false;
    bool bMarkedToRemove = false;
    int level = 0;
    FILE *file = nullptr;
    std::vector<FBXNodeDataBinding> bindedData;
};
