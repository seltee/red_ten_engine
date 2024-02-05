// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "common/geometry.h"
#include "renderer/shader.h"
#include "math/math.h"
#include <string>

class MeshStatic;

class Mesh
{
protected:
    EXPORT Mesh();
    EXPORT virtual ~Mesh();

public:
    EXPORT virtual void render();

    // Instance can have it's own default shader
    // Basically that's the only difference with main mesh
    // They share same geometry
    // Instances allows you to create compound mesh with different shaders on internal meshes
    EXPORT virtual Mesh *createInstance();

    EXPORT virtual Geometry *getGeometry();

    EXPORT virtual MeshStatic *getAsStatic();
    
    EXPORT virtual bool isRendarable();

    inline void setDefaultShader(Shader *defaultShader) { this->defaultShader = defaultShader; };
    inline Shader *getDefaultShader() { return defaultShader; };

    inline void setName(std::string name) { this->name = name; }
    inline std::string getName() { return this->name; }

protected:
    Shader *defaultShader;
    std::string name = "unnamed";
};
