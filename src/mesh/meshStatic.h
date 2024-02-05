// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once

#include "mesh/mesh.h"
#include "math/sphere.h"

// Don't delete the main mesh if you have instances of it
class MeshStatic : public Mesh
{
protected:
    EXPORT MeshStatic();

public:
    EXPORT ~MeshStatic();

    EXPORT virtual Geometry *getGeometry() override;

    EXPORT float *getVertexData();

    EXPORT int getVertexAmount();
    EXPORT int getFloatsPerVertex();
    EXPORT int getAttributesAmount();
    EXPORT bool isRendarable() override;

    // Make original mesh by transfering geometry data to it
    EXPORT virtual void setupFloatsArray(const float *data, int vertexAmount, int attributesAmount, int *attributeSize, bool buildTangents = false);

    // Set VAO, VBO vertex data to the mesh.
    // Target mesh becoming an instance of this mesh
    // Name stays unique
    EXPORT virtual void setOtherMeshAsInstanceOfThis(MeshStatic *mesh);

    // Be carefull with this one. If you have instances and clear one of the instances it may crush the engine
    // Instances doesn't clear VAO and VBO, only main mesh upon deletion
    EXPORT virtual void clear();

    EXPORT virtual void useVertexArray();

    EXPORT virtual MeshStatic *getAsStatic() override;

    EXPORT void setBoundVolumeSphere(Vector3 &center, float radius);
    EXPORT inline Sphere *getBoundVolumeSphere() { return &boundVolume; }

protected:
    Sphere boundVolume;

    int vertexAmount = 0;
    int floatsPerVertex = 0;
    int attributesAmount = 0;
    bool isInstance = false;
    float *vertexData = nullptr;
};