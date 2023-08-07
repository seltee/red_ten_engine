// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "primitives.h"

inline bool pointInTriangle(Vector3 triV1, Vector3 triV2, Vector3 triV3, Vector3 point)
{
    float distX = triV1.x - triV2.x;
    float distY = triV1.y - triV2.y;
    float distZ = triV1.z - triV2.z;

    float edgeLength1 = sqrt(distX * distX + distY * distY + distZ * distZ);

    distX = triV1.x - triV3.x;
    distY = triV1.y - triV3.y;
    distZ = triV1.z - triV3.z;

    float edgeLength2 = sqrt(distX * distX + distY * distY + distZ * distZ);

    distX = triV2.x - triV3.x;
    distY = triV2.y - triV3.y;
    distZ = triV2.z - triV3.z;

    float edgeLength3 = sqrt(distX * distX + distY * distY + distZ * distZ);

    float s = (edgeLength1 + edgeLength2 + edgeLength3) / 2.0f;

    float mainTriArea = sqrt(s * (s - edgeLength1) * (s - edgeLength2) * (s - edgeLength3));

    float smallTriArea[3] = {0.0f, 0.0f, 0.0f};
    Vector3 triVert[4];
    triVert[0] = triV1;
    triVert[1] = triV2;
    triVert[2] = triV3;
    triVert[3] = triV1; // When i=2, i+1 will be triV1

    // Find 3 triangle areas using the plane intersecting point
    for (int i = 0; i < 3; i++)
    {
        distX = point.x - triVert[i].x;
        distY = point.y - triVert[i].y;
        distZ = point.z - triVert[i].z;

        edgeLength1 = sqrt(distX * distX + distY * distY + distZ * distZ);

        distX = point.x - triVert[i + 1].x;
        distY = point.y - triVert[i + 1].y;
        distZ = point.z - triVert[i + 1].z;

        edgeLength2 = sqrt(distX * distX + distY * distY + distZ * distZ);

        distX = triVert[i].x - triVert[i + 1].x;
        distY = triVert[i].y - triVert[i + 1].y;
        distZ = triVert[i].z - triVert[i + 1].z;

        edgeLength3 = sqrt(distX * distX + distY * distY + distZ * distZ);

        s = (edgeLength1 + edgeLength2 + edgeLength3) / 2.0f;

        smallTriArea[i] = sqrt(s * (s - edgeLength1) * (s - edgeLength2) * (s - edgeLength3));
    }

    float totalSmallTriArea = smallTriArea[0] + smallTriArea[1] + smallTriArea[2];

    // Compare the three smaller triangle areas with the main triangles area
    // Subtract a small value from totalSmallTriArea to make up for inacuracy
    if (mainTriArea >= (totalSmallTriArea - 0.001f))
    {
        return true;
    }

    return false;
}

inline Vector3 getPolygonNormal(Vector3 pA, Vector3 pB, Vector3 pC)
{
    Vector3 vA = (pB - pA);
    Vector3 vB = (pC - pA);

    Vector3 n;
    n.x = (vA.y * vB.z) - (vA.z * vB.y);
    n.y = -((vB.z * vA.x) - (vB.x * vA.z));
    n.z = (vA.x * vB.y) - (vA.y * vB.x);

    return glm::normalize(n);
}

// From Gregorius Dirk - The Separating Axis Test
inline bool isMinkowskiFace(const Vector3 &A, const Vector3 &B, const Vector3 &B_x_A, const Vector3 &C, const Vector3 &D, const Vector3 &D_x_C)
{
    // Test if arcs AB and CD intersect on the unit sphere
    float CBA = glm::dot(C, B_x_A);
    float DBA = glm::dot(D, B_x_A);
    float ADC = glm::dot(A, D_x_C);
    float BDC = glm::dot(B, D_x_C);

    return CBA * DBA < 0.0f && ADC * BDC < 0.0f && CBA * BDC > 0.0f;
}

// From Gregorius Dirk - The Separating Axis Test
inline float project(const Vector3 &P1, const Vector3 &E1, const Vector3 &P2, const Vector3 &E2, const Vector3 &C1, Vector3 &Axis)
{
    // Build search direction
    Vector3 E1_x_E2 = glm::cross(E1, E2);

    // Skip near parallel edges: |e1 x e2| = sin(alpha) * |e1| * |e2|
    const float kTolerance = 0.05f;

    float L = glm::length(E1_x_E2);
    if (L < kTolerance * sqrtf(glm::length2(E1) * glm::length2(E2)))
    {
        return -FLT_MAX;
    }

    // Assure consistent normal orientation (here: Hull1 -> Hull2)
    Axis = E1_x_E2 / L;
    if (glm::dot(Axis, P1 - C1) < 0.0f)
    {
        Axis = -Axis;
    }

    // s = Dot(n, p2) - d = Dot(n, p2) - Dot(n, p1) = Dot(n, p2 - p1)
    return glm::dot(Axis, P2 - P1);
}

// from Christofer Ericson - real time collision detection
// Computes closest points C1 and C2 of S1(s)=P1+s*(Q1-P1) and
// S2(t)=P2+t*(Q2-P2), returning s and t. Function result is squared
// distance between between S1(s) and S2(t)
inline float getClosestPointSegmentSegment(const Vector3 &p1, const Vector3 &q1, const Vector3 &p2, const Vector3 &q2,
                                           float &s, float &t, Vector3 &c1, Vector3 &c2)
{
    const float EPSILON = 0.001f;

    Vector3 d1 = q1 - p1; // Direction vector of segment S1
    Vector3 d2 = q2 - p2; // Direction vector of segment S2

    Vector3 r = p1 - p2;
    float a = glm::dot(d1, d1); // Squared length of segment S1, always nonnegative
    float e = glm::dot(d2, d2); // Squared length of segment S2, always nonnegative
    float f = glm::dot(d2, r);

    // Check if either or both segments degenerate into points
    if (a <= EPSILON && e <= EPSILON)
    {
        // Both segments degenerate into points
        s = t = 0.0f;
        c1 = p1;
        c2 = p2;
    }
    if (a <= EPSILON)
    {
        // First segment degenerates into a point
        s = 0.0f;
        t = f / e; // s = 0 => t = (b*s + f) / e = f / e
        t = glm::clamp(t, 0.0f, 1.0f);
    }
    else
    {
        float c = glm::dot(d1, r);
        if (e <= EPSILON)
        {
            // Second segment degenerates into a point
            t = 0.0f;
            s = glm::clamp(-c / a, 0.0f, 1.0f); // t = 0 => s = (b*t - c) / a = -c / a
        }
        else
        {
            // The general nondegenerate case starts here
            float b = glm::dot(d1, d2);
            float denom = a * e - b * b; // Always nonnegative
            // If segments not parallel, compute closest point on L1 to L2 and
            // clamp to segment S1. Else pick arbitrary s (here 0)
            if (denom != 0.0f)
                s = glm::clamp((b * f - c * e) / denom, 0.0f, 1.0f);
            else
                s = 0.0f;

            // Compute point on L2 closest to S1(s) using
            // t = Dot((P1 + D1*s) - P2,D2) / Dot(D2,D2) = (b*s + f) / e
            t = (b * s + f) / e;
            // If t in [0,1] done. Else clamp t, recompute s for the new value
            // of t using s = Dot((P2 + D2*t) - P1,D1) / Dot(D1,D1)= (t*b - c) / a
            // and clamp s to [0, 1]
            if (t < 0.0f)
            {
                t = 0.0f;
                s = glm::clamp(-c / a, 0.0f, 1.0f);
            }
            else if (t > 1.0f)
            {
                t = 1.0f;
                s = glm::clamp((b - c) / a, 0.0f, 1.0f);
            }
        }
    }
    c1 = p1 + d1 * s;
    c2 = p2 + d2 * t;
    return glm::dot(c1 - c2, c1 - c2);
}

inline Vector3 getClosestPointOnTriangle(const Vector3 *tri, const Vector3 &point)
{
    Vector3 a = tri[0];
    Vector3 b = tri[1];
    Vector3 c = tri[2];

    const Vector3 ab = b - a;
    const Vector3 ac = c - a;
    const Vector3 ap = point - a;

    const float d1 = glm::dot(ab, ap);
    const float d2 = glm::dot(ac, ap);
    if (d1 <= 0.f && d2 <= 0.f)
        return a;

    const Vector3 bp = point - b;
    const float d3 = glm::dot(ab, bp);
    const float d4 = glm::dot(ac, bp);
    if (d3 >= 0.f && d4 <= d3)
        return b;

    const Vector3 cp = point - c;
    const float d5 = glm::dot(ab, cp);
    const float d6 = glm::dot(ac, cp);
    if (d6 >= 0.f && d5 <= d6)
        return c;

    const float vc = d1 * d4 - d3 * d2;
    if (vc <= 0.f && d1 >= 0.f && d3 <= 0.f)
    {
        const float v = d1 / (d1 - d3);
        return a + v * ab;
    }

    const float vb = d5 * d2 - d1 * d6;
    if (vb <= 0.f && d2 >= 0.f && d6 <= 0.f)
    {
        const float v = d2 / (d2 - d6);
        return a + v * ac;
    }

    const float va = d3 * d6 - d5 * d4;
    if (va <= 0.f && (d4 - d3) >= 0.f && (d5 - d6) >= 0.f)
    {
        const float v = (d4 - d3) / ((d4 - d3) + (d5 - d6));
        return b + v * (c - b);
    }

    const float denom = 1.f / (va + vb + vc);
    const float v = vb * denom;
    const float w = vc * denom;

    return a + v * ab + w * ac;
}

inline Vector3 getClosestPointOnPolygon(const Vector3 *verticies, const int amount, const Vector3 &point)
{
    float minDist = FLT_MAX;
    Vector3 out(0.0f);
    for (int i = 0; i < amount - 2; i++)
    {
        Vector3 closest = getClosestPointOnTriangle(&verticies[i], point);
        float distance = glm::length(closest - point);
        if (distance < minDist)
        {
            minDist = distance;
            out = closest;
        }
    }
    return out;
}

inline Vector3 lerp(const Vector3 &a, const Vector3 &b, float t)
{
    return Vector3(a.x + (b.x - a.x) * t,
                   a.y + (b.y - a.y) * t,
                   a.z + (b.z - a.z) * t);
}

inline bool isAlmostZero(Vector3 v)
{
    if (abs(v.x) > 1e-6 || abs(v.y) > 1e-6 || abs(v.z) > 1e-6)
        return false;
    return true;
}
