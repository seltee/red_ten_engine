// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "common/geometry.h"
#include "shaders/shader.h"
#include "math/math.h"
#include <string>

class MeshStatic;

class Mesh
{
protected:
    EXPORT Mesh();
    EXPORT virtual ~Mesh();

public:
    EXPORT virtual void render(Shader *shader, Matrix4 &vpMatrix, Matrix4 &modelMatrix);
    EXPORT virtual Mesh *createInstance();
    EXPORT virtual Geometry *getGeometry();

    EXPORT virtual MeshStatic *getAsStatic();

    inline void setDefaultShader(Shader *defaultShader) { this->defaultShader = defaultShader; };
    inline Shader *getDefaultShader() { return defaultShader; };

    inline void setName(std::string name) { this->name = name; }
    inline std::string getName() { return this->name; }

protected:
    Shader *defaultShader;
    std::string name = "unnamed";
};
