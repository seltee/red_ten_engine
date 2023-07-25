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
    inline Vector3 getDirection()
    {
        return glm::normalize(this->a - this->b);
    }

    // From Christer Ericson - Real-Time Collision Detection
    inline Vector3 getClosestPoint(Vector3 p)
    {
        Vector3 ab = b - a;
        // Project c onto ab, computing parameterized position d(t)=a+ t*(b – a)
        float t = glm::dot(p - a, ab) / glm::dot(ab, ab);

        // If outside segment, clamp t (and therefore d) to the closest endpoint
        if (t < 0.0f)
            t = 0.0f;
        if (t > 1.0f)
            t = 1.0f;

        // Compute projected position from the clamped t
        return a + t * ab;
    }

    Vector3 a = Vector3(0.0f), b = Vector3(0.0f);
};