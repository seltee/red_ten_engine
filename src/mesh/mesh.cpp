#include "mesh/mesh.h"
#include "mesh/meshStatic.h"

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

void Mesh::render(Shader *shader, Matrix4 &vpMatrix, Matrix4 &modelMatrix)
{
}

Mesh *Mesh::createInstance()
{
    return nullptr;
}

Geometry *Mesh::getGeometry()
{
    return nullptr;
}

MeshStatic *Mesh::getAsStatic()
{
    return nullptr;
}