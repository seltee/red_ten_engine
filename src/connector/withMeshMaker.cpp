#include "withMeshMaker.h"

MeshMaker *WithMeshMaker::meshMakerStatic = nullptr;

WithMeshMaker::WithMeshMaker()
{
    meshMaker = meshMakerStatic;
}

void WithMeshMaker::setMeshMaker(MeshMaker *meshMaker)
{
    meshMakerStatic = meshMaker;
}