// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "meshMaker.h"
#include "renderer/renderer.h"
#include <string>

extern const float meshMakerCubeData[];
extern const float meshMakerPlainData[];

MeshStatic *MeshMaker::createBox(Vector3 dimensions)
{
    float data[8 * 36];
    memcpy(data, meshMakerCubeData, (8 * 36) * sizeof(float));

    for (int i = 0; i < 36; i++)
    {
        int index = i * 8;
        data[index] *= dimensions.x;
        data[index + 1] *= dimensions.y;
        data[index + 2] *= dimensions.z;
    }
    auto newMesh = getRenderer()->createStaticMesh();
    int attributeSizes[3] = {3, 3, 2};
    newMesh->setupFloatsArray(data, 36, 3, attributeSizes, true);
    return newMesh;
}

MeshStatic *MeshMaker::createPlane(Vector2 dimensions, float UVScale, int subdivision)
{
    int dataAmount = 8 * 6 * subdivision * subdivision;
    float data[dataAmount];

    float plotSizeX = (dimensions.x / static_cast<float>(subdivision));
    float plotSizeY = (dimensions.y / static_cast<float>(subdivision));

    for (int iy = 0; iy < subdivision; iy++)
    {
        for (int ix = 0; ix < subdivision; ix++)
        {
            int polyShift = ix * 6 * 8 + iy * 6 * 8 * subdivision;
            memcpy(&data[polyShift], meshMakerPlainData, 8 * 6 * sizeof(float));
            for (int i = 0; i < 6; i++)
            {
                int index = polyShift + i * 8;
                data[index] *= plotSizeX;
                data[index] += plotSizeX * static_cast<float>(ix);
                data[index] -= dimensions.x / 2.0f;
                data[index + 2] *= plotSizeY;
                data[index + 2] += plotSizeY * static_cast<float>(iy);
                data[index + 2] -= dimensions.y / 2.0f;
                data[index + 6] *= UVScale / static_cast<float>(subdivision);
                data[index + 7] *= UVScale / static_cast<float>(subdivision);
                data[index + 6] += UVScale / static_cast<float>(subdivision) * static_cast<float>(ix);
                data[index + 7] += UVScale / static_cast<float>(subdivision) * static_cast<float>(iy);
            }
        }
    }

    auto newMesh = getRenderer()->createStaticMesh();
    int attributeSizes[3] = {3, 3, 2};
    newMesh->setupFloatsArray(data, 6 * subdivision * subdivision, 3, attributeSizes, true);
    return newMesh;
}

const float meshMakerCubeData[] = {
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,

    -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

    -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
    0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,

    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f};

const float meshMakerPlainData[] = {
    0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f};