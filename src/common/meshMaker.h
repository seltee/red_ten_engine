#pragma once
#include "mesh/mesh.h"
#include "mesh/meshStatic.h"
#include "math/math.h"

class MeshMaker
{
public:
    EXPORT MeshStatic *createBox(Vector3 dimensions);
    EXPORT MeshStatic *createPlane(Vector2 dimensions, float UVScale = 1.0f);
};