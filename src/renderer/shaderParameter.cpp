// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "renderer/shaderParameter.h"

ShaderParameter::ShaderParameter(ShaderParameterType type)
{
    this->type = type;
}

ShaderParameter::~ShaderParameter()
{
}

void ShaderParameter::apply()
{
}

void ShaderParameter::set(int amount, void *data)
{
    this->amount = amount;
    this->data = data;
}