// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "FBXAnimationLayer.h"

FBXAnimationLayer::FBXAnimationLayer(FBXNode *node)
{
    node->print();
    id = *reinterpret_cast<unsigned long long *>(node->bindedData.at(0).data);
    const char *charName = reinterpret_cast<char *>(node->bindedData.at(1).data);
    name = std::string(charName);
}

std::string targetAnimationName;

void FBXAnimationLayer::linkCurveNode(FBXAnimationCurveNode *node)
{
    curveNodes.push_back(node);
}