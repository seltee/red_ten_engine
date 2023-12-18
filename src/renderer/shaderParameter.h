// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"

class Shader;

enum class ShaderParameterType
{
    Float,
    Float2,
    Float3,
    Float4,
    Int,
    Int2,
    Int3,
    Int4
};

class ShaderParameter
{
protected:
    EXPORT ShaderParameter(ShaderParameterType type);

public:
    EXPORT virtual ~ShaderParameter();
    EXPORT void set(int amount, void *data);

    EXPORT virtual void apply();

    EXPORT inline ShaderParameterType getType() { return type; }
    EXPORT inline int getAmount() { return amount; }
    EXPORT inline void *getData() { return data; }

protected:
    void *data = nullptr;
    int amount = 0;
    ShaderParameterType type;
};