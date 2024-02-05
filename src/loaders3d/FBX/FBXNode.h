// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once

#include <vector>
#include <stdio.h>

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
