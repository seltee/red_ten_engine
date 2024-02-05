// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "FBXAnimationStack.h"
#include <string>

FBXAnimationStack::FBXAnimationStack(FBXNode *node)
{
    id = *reinterpret_cast<unsigned long long *>(node->bindedData.at(0).data);
    auto props = node->findNode("Properties70");
    if (props)
    {
        for (auto &property : props->children)
        {
            char type = 'n';
            for (auto &p : property->bindedData)
            {
                if (p.type == 'S' && strcmp(reinterpret_cast<char *>(p.data), "LocalStop") == 0)
                    type = 'l';
                if (p.type == 'S' && strcmp(reinterpret_cast<char *>(p.data), "ReferenceStop") == 0)
                    type = 'r';

                if (p.type == 'L')
                {
                    if (type == 'l')
                        localTime = static_cast<float>(*reinterpret_cast<unsigned long long *>(p.data));
                    if (type == 'r')
                        referenceTime = static_cast<float>(*reinterpret_cast<unsigned long long *>(p.data));
                }
            }
        }
    }
}

void FBXAnimationStack::linkLayer(FBXAnimationLayer *layer)
{
    layers.push_back(layer);
}

/*
 AnimationStack
 * 68970039(L)
 * Icosphere.001|IcosphereAction.001(S)
  Properties70
   P
   * LocalStop(S)
   * KTime(S)
   * Time(S)
   * 57732697500(L)
   P
   * ReferenceStop(S)
   * KTime(S)
   * Time(S)
   * 57732697500(L)
*/