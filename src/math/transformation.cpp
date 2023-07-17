// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "math/transformation.h"
#include "math/glm/gtc/matrix_transform.hpp"
#include <stdio.h>

void Transformation::setPosition(Vector3 v)
{
    this->position = v;
    bIsDirty = true;
}

void Transformation::setPosition(Vector2 v)
{
    this->position = Vector3(v.x, v.y, 0.0f);
    bIsDirty = true;
}

void Transformation::setPosition(float x, float y, float z)
{
    this->position = Vector3(x, y, z);
    bIsDirty = true;
}

void Transformation::setPosition(float x, float y)
{
    this->position = Vector3(x, y, 0.0f);
    bIsDirty = true;
}

void Transformation::translate(float x, float y, float z)
{
    this->position += Vector3(x, y, z);
    bIsDirty = true;
}

void Transformation::translate(float x, float y)
{
    this->position += Vector3(x, y, 0.0f);
    bIsDirty = true;
}

void Transformation::translate(Vector3 v)
{
    this->position += v;
    bIsDirty = true;
}

void Transformation::translate(Vector2 v)
{
    this->position += Vector3(v.x, v.y, 0.0f);
    bIsDirty = true;
}

Vector3 Transformation::getPosition()
{
    return position;
}

void Transformation::setRotation(Vector3 r)
{
    this->rotation = Quat(r);
    bIsDirty = true;
}

void Transformation::setRotation(Quat r)
{
    this->rotation = r;
    bIsDirty = true;
}

void Transformation::setRotation(float z)
{

    this->rotation = Quat(Vector3(0.0f, 0.0f, z));
    bIsDirty = true;
}

void Transformation::rotate(float z)
{
    this->rotation *= Quat(Vector3(0.0f, 0.0f, z));
    bIsDirty = true;
}

void Transformation::rotate(Vector3 r)
{
    this->rotation *= Quat(r);
    bIsDirty = true;
}

void Transformation::rotate(Quat r)
{
    this->rotation *= r;
    bIsDirty = true;
}

Quat Transformation::getRotation()
{
    return rotation;
}

Vector3 Transformation::getRotationEuler()
{
    return glm::eulerAngles(rotation);
}

float Transformation::getZRotation()
{
    return rotation.z;
}

void Transformation::setScale(Vector3 v)
{
    this->scale = v;
    bIsDirty = true;
}
void Transformation::setScale(Vector2 v)
{
    this->scale = Vector3(v.x, v.y, 1.0f);
    bIsDirty = true;
}

void Transformation::setScale(float x, float y, float z)
{
    this->scale = Vector3(x, y, z);
    bIsDirty = true;
}

void Transformation::setScale(float x, float y)
{
    this->scale = Vector3(x, y, 1.0f);
    bIsDirty = true;
}

void Transformation::setScale(float xy)
{
    this->scale = Vector3(xy, xy, 1.0f);
    bIsDirty = true;
}

Vector3 Transformation::getScale()
{
    return scale;
}

void Transformation::update()
{
    bIsDirty = false;

    mModel = glm::translate(Matrix4(1.0f), Vector3(position.x, position.y, position.z));
    mModel *= glm::toMat4(rotation);
    if (scale.x != 1.0f || scale.y != 1.0f || scale.z != 1.0f)
    {
        mModel *= glm::scale(Matrix4(1.0f), Vector3(scale.x, scale.y, scale.z));
    }
}

bool Transformation::isDirty()
{
    return bIsDirty;
}

Matrix4 *Transformation::getModelMatrix()
{
    if (bIsDirty)
        update();

    return &mModel;
}