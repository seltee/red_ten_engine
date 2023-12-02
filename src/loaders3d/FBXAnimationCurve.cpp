// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "FBXAnimationCurve.h"
#include <string>

FBXAnimationCurve::FBXAnimationCurve(FBXNode *node)
{
    id = *reinterpret_cast<unsigned long long *>(node->bindedData.at(0).data);

    unsigned long long *keyTimeData = nullptr;
    float *keyValueData = nullptr;

    for (auto &property : node->children)
    {
        if (property->isName("KeyTime"))
        {
            auto subValue = property->bindedData.at(0);
            keysCount = subValue.numElements;
            keyTimeData = reinterpret_cast<unsigned long long *>(subValue.data);
        }
        if (property->isName("KeyValueFloat"))
        {
            auto subValue = property->bindedData.at(0);
            keyValueData = reinterpret_cast<float *>(subValue.data);
        }
        if (property->isName("KeyAttrRefCount"))
        {
            auto subValue = property->bindedData.at(0);
            refsAmount = subValue.numElements;
            refs = new unsigned long long[refsAmount];
            for (int i = 0; i < refsAmount; i++)
            {
                refs[i] = reinterpret_cast<unsigned long long *>(subValue.data)[i];
            }
        }
    }

    if (keyTimeData && keyValueData && keysCount > 0)
    {
        keys = new FBXAnimationCurveKey[keysCount];
        for (int i = 0; i < keysCount; i++)
        {
            keys[i].keyTime = keyTimeData[i];
            keys[i].keyValue = keyValueData[i];
        }
    }
}

/*
Animation Curve
 AnimationCurve
 * 153232986(L)
 * (S)
  Default
  * 0.565464(D)
  KeyVer
  * 0(I)
  KeyTime
  * (l)
  KeyValueFloat
  * Float array (num 2)(f)
  KeyAttrFlags
  * Long long array (num 1)(i)
  KeyAttrDataFloat
  * Float array (num 4)(f)
  KeyAttrRefCount
  * Long long array (num 1)(i)

*/