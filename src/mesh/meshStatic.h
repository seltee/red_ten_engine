// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once

#include "mesh/mesh.h"

// Don't delete the main mesh if you have instances of it
class MeshStatic : public Mesh
{
public:
    EXPORT MeshStatic();
    EXPORT ~MeshStatic();

    EXPORT void render(Shader *shader, Matrix4 &vpMatrix, Matrix4 &modelMatrix);

    // Instance can have it's own default shader
    // Basically that's the only difference with main mesh
    // They share same geometry
    // Instances allows you to create compound mesh with different shaders on internal meshes
    EXPORT Mesh *createInstance();
    
    // Pure geometry used for physics
    EXPORT Geometry *getGeometry();
    EXPORT float *getVertexData();

    EXPORT int getVertexAmount();
    EXPORT int getFloatsPerVertex();
    EXPORT int getAttributesAmount();

    // Make original mesh by transfering geometry data to it
    EXPORT void setupFloatsArray(const float *data, int vertexAmount, int attributesAmount, int *attributeSize, bool buildTangents = false);

    // Trasfer data to this mesh, this mesh becoming an instance
    // Doesn't change the name
    EXPORT void setupInstance(unsigned int vao, unsigned int vbo, int vertexAmount, int floatsPerVertex, int attributesAmount, float *vertexData);

    // Set VAO, VBO vertex data to the mesh.
    // Target mesh becoming an instance of this mesh
    // Name stays unique
    EXPORT void setOtherMeshAsInstanceOfThis(MeshStatic *mesh);

    // Be carefull with this one. If you have instances and clear one of the instances it may crush the engine
    // Instances doesn't clear VAO and VBO, only main mesh upon deletion
    EXPORT virtual void clear();

    EXPORT void useVertexArray();
    
    EXPORT MeshStatic *getAsStatic();

protected:
    void makeVBO(const float *data, int amount);
    void makeVAO(int attributesAmount, int *attributeSize);

    int vertexAmount = 0;
    int floatsPerVertex = 0;
    int attributesAmount = 0;

    unsigned int vbo = 0;
    unsigned int vao = 0;

    bool isInstance = false;

    Geometry *geometry = nullptr;
    float *vertexData = nullptr;
};