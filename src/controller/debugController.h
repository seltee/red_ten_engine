// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "common/color.h"
#include "math/primitives.h"
#include <vector>
#include <string>

class AABB;

struct DebugEntry
{
    int index;
    int msLeft;
    std::string str;
    Color color;
};

struct DebugBox
{
    Matrix4 m;
    float timer;
    Vector3 color;
};

class DebugController
{
public:
    EXPORT void print(std::string str);
    EXPORT void print(Color color, std::string str);
    EXPORT void print(int ms, std::string str);
    EXPORT void print(Color color, int ms, std::string str);

    EXPORT void process(float delta);
    EXPORT std::vector<DebugEntry *> *getEntries();
    EXPORT bool hasIndex(int index);

    EXPORT bool isVisible();
    EXPORT void enableVisibility();
    EXPORT void disableVisibility();

    EXPORT void addDebugLine(Vector3 a, Vector3 b, float time, float thickness, Vector3 color);
    EXPORT void addDebugBox(Vector3 p, float size, float time, Vector3 color);
    EXPORT void renderAll(Matrix4 *mProjectionView);
    EXPORT void renderBoundingBox(AABB aabb, Matrix4 *mProjectionView, float f, float thickness, Vector3 color);

    const int defaultShowTimeMS = 3000;
    const Color defaultColor{0.5f, 0.9f, 0.5f};

    EXPORT void renderLine(Vector3 a, Vector3 b, Matrix4 *mProjectionView, float thickness, Vector3 color);
    EXPORT void renderLine(Matrix4 *model, Matrix4 *mProjectionView, Vector3 color);

protected:
    std::vector<DebugBox *> debugFigures;

    int index = 0;
    bool bVisible = true;
    std::vector<DebugEntry *> entries;
};
