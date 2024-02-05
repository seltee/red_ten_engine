// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "renderer/opengl/meshStaticOpenGL.h"
#include "renderer/renderer.h"
#include "math/math.h"
#include "math/smallestEnclosingSphere.h"
#include "renderer/opengl/glew.h"
#include <string.h>

MeshStaticOpenGL::MeshStaticOpenGL()
{
}

MeshStaticOpenGL::~MeshStaticOpenGL()
{
    if (!isInstance)
        clear();
}

void MeshStaticOpenGL::render()
{
    if (!vertexAmount)
        return;

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertexAmount);
}

Mesh *MeshStaticOpenGL::createInstance()
{
    MeshStaticOpenGL *newMesh = new MeshStaticOpenGL();
    newMesh->setupInstance(vao, vbo, vertexAmount, floatsPerVertex, attributesAmount, vertexData, boundVolume);
    newMesh->setDefaultShader(this->getDefaultShader());
    return newMesh;
}

Geometry *MeshStaticOpenGL::getGeometry()
{
    return geometry;
}

void MeshStaticOpenGL::setupFloatsArray(const float *data, int vertexAmount, int attributesAmount, int *attributeSize, bool buildTangents)
{
    clear();

    floatsPerVertex = 0;
    for (int i = 0; i < attributesAmount; i++)
        floatsPerVertex += attributeSize[i];

    this->vertexAmount = vertexAmount;

    Sphere volumeSphere = makeSmallestSphere(data, vertexAmount, floatsPerVertex);
    setBoundVolumeSphere(volumeSphere.center, volumeSphere.radius);

    if (buildTangents)
    {
        int newFloatsPerVertex = floatsPerVertex + 6;
        vertexData = new float[newFloatsPerVertex * vertexAmount];
        memset(vertexData, 0, newFloatsPerVertex * vertexAmount * sizeof(float));
        int limit = vertexAmount - (vertexAmount % 3);

        for (int i = 0; i < limit; i += 3)
        {
            const float *oldV1Addr = &data[(i + 0) * floatsPerVertex];
            const float *oldV2Addr = &data[(i + 1) * floatsPerVertex];
            const float *oldV3Addr = &data[(i + 2) * floatsPerVertex];

            float *newV1Addr = &vertexData[(i + 0) * newFloatsPerVertex];
            float *newV2Addr = &vertexData[(i + 1) * newFloatsPerVertex];
            float *newV3Addr = &vertexData[(i + 2) * newFloatsPerVertex];

            memcpy(newV1Addr, oldV1Addr, floatsPerVertex * sizeof(float));
            memcpy(newV2Addr, oldV2Addr, floatsPerVertex * sizeof(float));
            memcpy(newV3Addr, oldV3Addr, floatsPerVertex * sizeof(float));

            float tg[6];
            calcTangets(newV1Addr, newV2Addr, newV3Addr, tg);

            memcpy(&newV1Addr[floatsPerVertex], tg, 6 * sizeof(float));
            memcpy(&newV2Addr[floatsPerVertex], tg, 6 * sizeof(float));
            memcpy(&newV3Addr[floatsPerVertex], tg, 6 * sizeof(float));
        }

        int newAttributeAmount = attributesAmount + 2;
        int *newAttributeSize = new int[newAttributeAmount];
        for (int i = 0; i < attributesAmount; i++)
        {
            newAttributeSize[i] = attributeSize[i];
        }
        newAttributeSize[attributesAmount] = 3;
        newAttributeSize[attributesAmount + 1] = 3;

        floatsPerVertex = newFloatsPerVertex;
        makeVBO(vertexData, vertexAmount * floatsPerVertex);
        makeVAO(newAttributeAmount, newAttributeSize);

        this->attributesAmount = newAttributeAmount;

        geometry = new Geometry(vertexData, vertexAmount, floatsPerVertex, 0);

        delete[] newAttributeSize;
    }
    else
    {
        this->attributesAmount = attributesAmount;

        vertexData = new float[floatsPerVertex * vertexAmount];
        memset(vertexData, 0, floatsPerVertex * vertexAmount * sizeof(float));
        memcpy(vertexData, data, floatsPerVertex * vertexAmount * sizeof(float));

        makeVBO(vertexData, vertexAmount * floatsPerVertex);
        makeVAO(attributesAmount, attributeSize);

        geometry = new Geometry(data, vertexAmount, floatsPerVertex, 0);
    }
}

void MeshStaticOpenGL::setupInstance(unsigned int vao, unsigned int vbo, int vertexAmount, int floatsPerVertex, int attributesAmount, float *vertexData, Sphere &boundVolume)
{
    isInstance = true;
    this->vao = vao;
    this->vbo = vbo;
    this->vertexAmount = vertexAmount;
    this->floatsPerVertex = floatsPerVertex;
    this->attributesAmount = attributesAmount;
    this->vertexData = vertexData;
    this->boundVolume = boundVolume;
}

EXPORT void MeshStaticOpenGL::setOtherMeshAsInstanceOfThis(MeshStatic *mesh)
{
    if (mesh)
    {
        reinterpret_cast<MeshStaticOpenGL *>(mesh)->setupInstance(vao, vbo, vertexAmount, floatsPerVertex, attributesAmount, vertexData, boundVolume);
    }
}

void MeshStaticOpenGL::clear()
{
    if (vbo)
        glDeleteBuffers(1, &vbo);
    if (vao)
        glDeleteVertexArrays(1, &vao);
    vbo = 0;
    vao = 0;
    if (vertexData)
        delete[] vertexData;
    vertexData = nullptr;
    if (geometry)
        delete geometry;
    geometry = nullptr;
}

void MeshStaticOpenGL::useVertexArray()
{
    if (vao)
        glBindVertexArray(vao);
}

MeshStatic *MeshStaticOpenGL::getAsStatic()
{
    return this;
}

void MeshStaticOpenGL::makeVBO(const float *data, int amount)
{
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(float), data, GL_STATIC_DRAW);
}

void MeshStaticOpenGL::makeVAO(int attributesAmount, int *attributeSize)
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    for (int i = 0; i < attributesAmount; i++)
        glEnableVertexAttribArray(i);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    int shift = 0;
    for (int i = 0; i < attributesAmount; i++)
    {
        glVertexAttribPointer(i, attributeSize[i], GL_FLOAT, GL_FALSE, floatsPerVertex * sizeof(float), (void *)(shift * sizeof(float)));
        shift += attributeSize[i];
    }
}
