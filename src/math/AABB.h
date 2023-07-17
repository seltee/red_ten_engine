#pragma once
#include "primitives.h"
#include "line.h"

class AABB
{
public:
    AABB(){};
    AABB(Vector3 start, Vector3 end)
    {
        this->start = start;
        this->end = end;
    }

    inline void extend(AABB aabb)
    {
        if (aabb.start.x < start.x)
            start.x = aabb.start.x;
        if (aabb.start.y < start.y)
            start.y = aabb.start.y;
        if (aabb.start.z < start.z)
            start.z = aabb.start.z;
        if (aabb.end.x > end.x)
            end.x = aabb.end.x;
        if (aabb.end.y > end.y)
            end.y = aabb.end.y;
        if (aabb.end.z > end.z)
            end.z = aabb.end.z;
    }

    inline bool test(AABB aabb)
    {
        return aabb.start.x < end.x && aabb.end.x > start.x && aabb.start.y < end.y && aabb.end.y > start.y && aabb.start.z < end.z && aabb.end.z > start.z;
    }

    inline bool test(Line line)
    {
        // Point p0, Point p1, AABB b
        Vector3 AABBcenter = (start + end) / 2.0f;     // Box center-point
        Vector3 AABBhalfLength = end - AABBcenter;     // Box halflength extents
        Vector3 lineMiddle = (line.a + line.b) / 2.0f; // Segment midpoint

        Vector3 lineHalfLength = line.b - lineMiddle; // Segment halflength vector
        lineMiddle = lineMiddle - AABBcenter;         // Translate box and segment to origin

        // Try world coordinate axes as separating axes
        float adx = fabsf(lineHalfLength.x);
        if (fabsf(lineMiddle.x) > AABBhalfLength.x + adx)
            return false;
        float ady = fabsf(lineHalfLength.y);
        if (fabsf(lineMiddle.y) > AABBhalfLength.y + ady)
            return false;
        float adz = fabsf(lineHalfLength.z);
        if (fabsf(lineMiddle.z) > AABBhalfLength.z + adz)
            return false;

        // Add in an epsilon term to counteract arithmetic errors when line is
        // (near) parallel to a coordinate axis
        const float e = 0.0001f;
        adx += e;
        ady += e;
        adz += e;

        // Try cross products of line direction vector with coordinate axes
        if (fabsf(lineMiddle.y * lineHalfLength.z - lineMiddle.z * lineHalfLength.y) > AABBhalfLength.y * adz + AABBhalfLength.z * ady)
            return false;
        if (fabsf(lineMiddle.z * lineHalfLength.x - lineMiddle.x * lineHalfLength.z) > AABBhalfLength.x * adz + AABBhalfLength.z * adx)
            return false;
        if (fabsf(lineMiddle.x * lineHalfLength.y - lineMiddle.y * lineHalfLength.x) > AABBhalfLength.x * ady + AABBhalfLength.y * adx)
            return false;
        // No separating axis found; line must be overlapping AABB
        return true;
    }

    inline Vector3 getCenter()
    {
        return (start + end) / 2.0f;
    }

    Vector3 start = Vector3(0.0f);
    Vector3 end = Vector3(0.0f);
};
