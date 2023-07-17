#pragma once
#include "common/mesh.h"
#include "math/math.h"

class MeshMaker
{
public:
    EXPORT Mesh *createBox(Vector3 dimensions);
    EXPORT Mesh *createPlane(Vector2 dimensions, float UVScale = 1.0f);
};