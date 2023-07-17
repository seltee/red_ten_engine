#pragma once
#include "primitives.h"

class Line
{
public:
    Line() {}
    Line(Vector3 a, Vector3 b)
    {
        this->a = a;
        this->b = b;
    }
    Vector3 getDirection()
    {
        return glm::normalize(this->a - this->b);
    }
    Vector3 a = Vector3(0.0f), b = Vector3(0.0f);
};