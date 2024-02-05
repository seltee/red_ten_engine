// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once

#include "mesh/meshStatic.h"
#include "connector/withRenderer.h"

// Don't delete the main mesh if you have instances of it
class MeshStaticOpenGL : public MeshStatic, WithRenderer
{
public:
    EXPORT MeshStaticOpenGL();
    EXPORT ~MeshStaticOpenGL();

    EXPORT void render() override;

    // Instance can have it's own default shader
    // Basically that's the only difference with main mesh
    // They share same geometry
    // Instances allows you to create compound mesh with different shaders on internal meshes
    EXPORT Mesh *createInstance() override;

    // Pure geometry used for physics
    EXPORT Geometry *getGeometry() override;

    // Make original mesh by transfering geometry data to it
    EXPORT void setupFloatsArray(const float *data, int vertexAmount, int attributesAmount, int *attributeSize, bool buildTangents = false) override;

    // Trasfer data to this mesh, this mesh becoming an instance
    // Doesn't change the name
    EXPORT void setupInstance(unsigned int vao, unsigned int vbo, int vertexAmount, int floatsPerVertex, int attributesAmount, float *vertexData, Sphere &boundVolume);

    // Set VAO, VBO vertex data to the mesh.
    // Target mesh becoming an instance of this mesh
    // Name stays unique
    EXPORT void setOtherMeshAsInstanceOfThis(MeshStatic *mesh) override;

    // Be carefull with this one. If you have instances and clear one of the instances it may crush the engine
    // Instances doesn't clear VAO and VBO, only main mesh upon deletion
    EXPORT void clear() override;

    EXPORT void useVertexArray() override;

    EXPORT MeshStatic *getAsStatic() override;

protected:
    void makeVBO(const float *data, int amount);
    void makeVAO(int attributesAmount, int *attributeSize);

    unsigned int vbo = 0;
    unsigned int vao = 0;

    Geometry *geometry = nullptr;
};