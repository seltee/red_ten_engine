// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "debugController.h"
#include "math/transformation.h"

void DebugController::print(std::string str)
{
    print(defaultColor, defaultShowTimeMS, str);
}

void DebugController::print(Color color, std::string str)
{
    print(color, defaultShowTimeMS, str);
}

void DebugController::print(int ms, std::string str)
{
    print(defaultColor, ms, str);
}

void DebugController::print(Color color, int ms, std::string str)
{
    DebugEntry *entry = new DebugEntry();
    entry->index = index;
    entry->msLeft = ms;
    entry->color = color;
    entry->str = str;
    index++;
    entries.push_back(entry);
}

void DebugController::process(float delta)
{
    auto entry = entries.begin();
    while (entry != entries.end())
    {
        (*entry)->msLeft -= (int)(delta * 1000);
        if ((*entry)->msLeft <= 0)
        {
            delete (*entry);
            entry = entries.erase(entry);
        }
        else
            ++entry;
    }

    if (!debugFigures.empty())
    {
        auto figure = debugFigures.begin();
        while (figure != debugFigures.end())
        {
            (*figure)->timer -= delta;
            if ((*figure)->timer <= 0.0f)
            {
                delete *figure;
                figure = debugFigures.erase(figure);
            }
            else
                ++figure;
        }
    }
}

std::vector<DebugEntry *> *DebugController::getEntries()
{
    return &entries;
}

bool DebugController::hasIndex(int index)
{
    for (auto it = entries.begin(); it != entries.end(); it++)
    {
        if ((*it)->index == index)
            return true;
    }
    return false;
}

bool DebugController::isVisible()
{
    return bVisible;
}

void DebugController::enableVisibility()
{
    bVisible = true;
}

void DebugController::disableVisibility()
{
    bVisible = false;
}

void DebugController::addDebugLine(Vector3 a, Vector3 b, float time, float thickness, Vector3 color)
{
    Vector3 p = (a + b) / 2.0f;
    Vector3 dif = glm::normalize(b - a);
    float y = atan2f(dif.z, dif.x);
    float len = sqrtf(dif.x * dif.x + dif.z * dif.z);
    float x = atan2(len, dif.y);
    float lineLength = glm::length(a - b);

    Transformation tf;
    tf.setPosition(p);
    tf.setRotation(Vector3(CONST_PI / 2 - x, -y - CONST_PI / 2.0f, 0.0f));
    tf.setScale(Vector3(thickness, thickness, lineLength));
    Matrix4 *mOut = tf.getModelMatrix();

    DebugBox *newDebugBox = new DebugBox({*mOut, time, color});
    debugFigures.push_back(newDebugBox);
}

void DebugController::addDebugBox(Vector3 p, float size, float time, Vector3 color)
{
    Transformation tf;
    tf.setPosition(p);
    tf.setScale(Vector3(size, size, size));
    Matrix4 *mOut = tf.getModelMatrix();
    DebugBox *newDebugBox = new DebugBox({*mOut, time, color});
    debugFigures.push_back(newDebugBox);
}

void DebugController::renderAll(Matrix4 *mProjectionView)
{
    if (!debugFigures.empty())
    {
        for (auto figure = debugFigures.begin(); figure != debugFigures.end(); ++figure)
        {
            renderLine(&(*figure)->m, mProjectionView, (*figure)->color);
        }
    }
}

void DebugController::renderBoundingBox(AABB aabb, Matrix4 *mProjectionView, float f, float thickness, Vector3 color)
{
    Vector3 start = aabb.start / f;
    Vector3 end = aabb.end / f;

    renderLine(Vector3(start.x, end.y, start.z), Vector3(end.x, end.y, start.z), mProjectionView, thickness, color);
    renderLine(Vector3(start.x, end.y, start.z), Vector3(start.x, end.y, end.z), mProjectionView, thickness, color);
    renderLine(Vector3(end.x, end.y, end.z), Vector3(end.x, end.y, start.z), mProjectionView, thickness, color);
    renderLine(Vector3(end.x, end.y, end.z), Vector3(start.x, end.y, end.z), mProjectionView, thickness, color);

    renderLine(Vector3(start.x, start.y, start.z), Vector3(end.x, start.y, start.z), mProjectionView, thickness, color);
    renderLine(Vector3(start.x, start.y, start.z), Vector3(start.x, start.y, end.z), mProjectionView, thickness, color);
    renderLine(Vector3(end.x, start.y, end.z), Vector3(end.x, start.y, start.z), mProjectionView, thickness, color);
    renderLine(Vector3(end.x, start.y, end.z), Vector3(start.x, start.y, end.z), mProjectionView, thickness, color);

    renderLine(Vector3(start.x, start.y, start.z), Vector3(start.x, end.y, start.z), mProjectionView, thickness, color);
    renderLine(Vector3(end.x, start.y, start.z), Vector3(end.x, end.y, start.z), mProjectionView, thickness, color);
    renderLine(Vector3(start.x, start.y, end.z), Vector3(start.x, end.y, end.z), mProjectionView, thickness, color);
    renderLine(Vector3(end.x, start.y, end.z), Vector3(end.x, end.y, end.z), mProjectionView, thickness, color);
}

void DebugController::renderLine(Vector3 a, Vector3 b, Matrix4 *mProjectionView, float thickness, Vector3 color)
{
    if (getRenderer())
        getRenderer()->renderDebugLine(a, b, mProjectionView, thickness, color);
}

void DebugController::renderLine(Matrix4 *model, Matrix4 *mProjectionView, Vector3 color)
{
    if (getRenderer())
        getRenderer()->renderDebugLine(model, mProjectionView, color);
}