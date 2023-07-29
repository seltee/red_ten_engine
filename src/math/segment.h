// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "primitives.h"
#include "math/algorithms.h"

class Segment
{
public:
    Segment() {}
    Segment(Vector3 a, Vector3 b)
    {
        this->a = a;
        this->b = b;
    }
    inline const Vector3 getDirection()
    {
        return glm::normalize(this->a - this->b);
    }

    // From Christer Ericson - Real-Time Collision Detection
    inline Vector3 getClosestPoint(Vector3 p) const
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

    // Returns distance
    inline float getClosestPointToPoly(Vector3 *points, int amount, Vector3 &onPoly, Vector3 &onSegment) const
    {
        Vector3 closestAOnPoly = getClosestPointOnPolygon(points, amount, a);
        Vector3 closestBOnPoly = getClosestPointOnPolygon(points, amount, b);

        Vector3 closestOnSegmentA = getClosestPoint(closestAOnPoly);
        Vector3 closestOnSegmentB = getClosestPoint(closestBOnPoly);

        float distA = glm::length(closestAOnPoly - closestOnSegmentA);
        float distB = glm::length(closestBOnPoly - closestOnSegmentB);

        float minDistance = distA;
        onPoly = closestAOnPoly;
        onSegment = closestOnSegmentA;
        if (distB < minDistance)
        {
            minDistance = distB;
            onPoly = closestBOnPoly;
            onSegment = closestOnSegmentB;
        }
        float s, c;
        Vector3 newOnPoly, newOnSegment;
        for (int i = 0; i < amount; i++)
        {
            Vector3 p1 = points[i];
            Vector3 p2 = points[(i + 1) % amount];
            getClosestPointSegmentSegment(p1, p2, a, b, s, c, newOnPoly, newOnSegment);
            float distance = glm::length(newOnPoly - newOnSegment);
            if (distance < minDistance)
            {
                distance = minDistance;
                onPoly = newOnPoly;
                onSegment = newOnSegment;
            }
        }
        return minDistance;
    }

    inline float getClosestPointToTriangle(const Vector3 *triangle, Vector3 &onTri, Vector3 &onSegment) const
    {
        Vector3 closestAOnTri = getClosestPointOnTriangle(triangle, a);
        Vector3 closestBOnTri = getClosestPointOnTriangle(triangle, b);

        Vector3 closestOnSegmentA = getClosestPoint(closestAOnTri);
        Vector3 closestOnSegmentB = getClosestPoint(closestBOnTri);

        float distA = glm::length(closestAOnTri - closestOnSegmentA);
        float distB = glm::length(closestBOnTri - closestOnSegmentB);

        float maxDistance = distA;
        onTri = closestAOnTri;
        onSegment = closestOnSegmentA;
        if (distB < maxDistance)
        {
            maxDistance = distB;
            onTri = closestBOnTri;
            onSegment = closestOnSegmentB;
        }
        float s, c;
        Vector3 newOnPoly, newOnSegment;
        for (int i = 0; i < 3; i++)
        {
            Vector3 p1 = triangle[i];
            Vector3 p2 = triangle[(i + 1) % 3];
            getClosestPointSegmentSegment(p1, p2, a, b, s, c, newOnPoly, newOnSegment);
            float distance = glm::length(newOnPoly - newOnSegment);
            if (distance < maxDistance)
            {
                distance = maxDistance;
                onTri = newOnPoly;
                onSegment = newOnSegment;
            }
        }
        return maxDistance;
    }

    Vector3 a = Vector3(0.0f), b = Vector3(0.0f);
};