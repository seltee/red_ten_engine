// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/meshMaker.h"

class WithMeshMaker
{
public:
    EXPORT WithMeshMaker();
    static void setMeshMaker(MeshMaker *meshMaker);

protected:
    MeshMaker *meshMaker;

private:
    static MeshMaker *meshMakerStatic;
};
