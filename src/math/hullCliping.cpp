#include "hullCliping.h"

// Clips a face to the back of a plane, return the number of vertices out, stored in ppVtxOut
int HullCliping::clipFace(const Vector3 *pVtxIn, int numVertsIn, Vector3 &planeNormalWS, float planeEqWS, Vector3 *ppVtxOut)
{
    int ve;
    float ds, de;
    int numVertsOut = 0;
    if (numVertsIn < 2)
        return 0;

    Vector3 firstVertex = pVtxIn[numVertsIn - 1];
    Vector3 endVertex = pVtxIn[0];

    ds = glm::dot(planeNormalWS, firstVertex) + planeEqWS;

    for (ve = 0; ve < numVertsIn; ve++)
    {
        endVertex = pVtxIn[ve];

        de = glm::dot(planeNormalWS, endVertex) + planeEqWS;

        if (ds < 0)
        {
            if (de < 0)
            {
                // Start < 0, end < 0, so output endVertex
                ppVtxOut[numVertsOut++] = endVertex;
            }
            else
            {
                // Start < 0, end >= 0, so output intersection
                ppVtxOut[numVertsOut++] = lerp(firstVertex, endVertex, (ds * 1.0f / (ds - de)));
            }
        }
        else
        {
            if (de < 0)
            {
                // Start >= 0, end < 0 so output intersection and end
                ppVtxOut[numVertsOut++] = lerp(firstVertex, endVertex, (ds * 1.0f / (ds - de)));
                ppVtxOut[numVertsOut++] = endVertex;
            }
        }
        firstVertex = endVertex;
        ds = de;
    }
    return numVertsOut;
}

int HullCliping::clipFaceAgainstHull(
    const Vector3 &separatingNormal,
    const Hull *hullA,
    Vector3 *verticies,
    int verticiesAmount,
    const float minDist,
    const float maxDist,
    Vector4 *contactsOut,
    int contactCapacity)
{
    int numContactsOut = 0;
    int numVertsIn = verticiesAmount;
    int numVertsOut = 0;

    Vector3 *pVtxIn = verticies;
    Vector3 vBuff[MAX_POINTS];
    Vector3 *pVtxOut = vBuff;

    const HullPolygon *closestFaceA = nullptr;

    float dmin = FLT_MAX;
    for (auto it = hullA->polies.begin(); it != hullA->polies.end(); it++)
    {
        float d = glm::dot(it->absoluteNormal, separatingNormal);
        if (d < dmin)
        {
            dmin = d;
            closestFaceA = &(*it);
        }
    }

    if (closestFaceA == nullptr)
        return numContactsOut;

    int numVerticesA = closestFaceA->pointsAmount;
    for (int e0 = 0; e0 < numVerticesA; e0++)
    {
        const Vector3 a = hullA->absoluteVerticies[closestFaceA->points[e0]];
        const Vector3 b = hullA->absoluteVerticies[closestFaceA->points[(e0 + 1) % numVerticesA]];

        debugController->addDebugLine(a * 10.0f, b * 10.0f, 0.5f, 0.02f, Vector3(0.1f, 1.0f, 0.1f));

        const Vector3 worldEdge0 = a - b;
        const Vector3 worldPlaneAnormal1 = closestFaceA->absoluteNormal;

        Vector3 planeNormalWS = -glm::cross(worldEdge0, worldPlaneAnormal1);
        float planeEqWS = -glm::dot(a, planeNormalWS);

        numVertsOut = clipFace(pVtxIn, numVertsIn, planeNormalWS, planeEqWS, pVtxOut);

        Vector3 *tmp = pVtxOut;
        pVtxOut = pVtxIn;
        pVtxIn = tmp;
        numVertsIn = numVertsOut;
        numVertsOut = 0;
    }

    // Keep points that are behind the witness face
    Vector3 planeNormalWS = closestFaceA->absoluteNormal;
    float planeEqWS = glm::dot(planeNormalWS, hullA->absoluteVerticies[closestFaceA->points[0]]);

    for (int i = 0; i < numVertsIn; i++)
    {
        // debugController->addDebugLine(pVtxIn[i] * 10.0f, pVtxIn[(i + 1) % numVertsIn] * 10.0f, 2.5f, 0.02f, Vector3(1.0f, 1.0f, 0.1f));

        float depth = 0.5f + (glm::dot(planeNormalWS, pVtxIn[i]) + planeEqWS);
        // printf("VERT %i : %f, %f\n", i, depth, planeEqWS);

        if (depth <= minDist)
        {
            depth = minDist;
        }
        if (numContactsOut < contactCapacity)
        {
            if (depth <= maxDist)
            {
                Vector3 pointInWorld = pVtxIn[i];
                contactsOut[numContactsOut++] = Vector4(pointInWorld, depth);
            }
        }
        else
        {
            printf("exceeding contact capacity (%d,%df)\n", numContactsOut, contactCapacity);
        }
    }

    return numContactsOut;
}

int HullCliping::clipHullAgainstHull(const Vector3 &separatingNormal,
                                     const Hull *hullA,
                                     const Hull *hullB,
                                     const float minDist,
                                     float maxDist,
                                     Vector4 *contactsOut,
                                     int contactCapacity)
{
    int numContactsOut = 0;
    const HullPolygon *closestFaceB = nullptr;
    float dmax = -FLT_MAX;

    for (auto it = hullB->polies.begin(); it != hullB->polies.end(); it++)
    {
        Vector3 polyWorldNormal = it->absoluteNormal;
        float d = glm::dot(polyWorldNormal, separatingNormal);
        if (d > dmax)
        {
            dmax = d;
            closestFaceB = &(*it);
        }
    }

    if (closestFaceB != nullptr)
    {
        Vector3 verticies[MAX_POINTS];
        int verticiesAmount = closestFaceB->pointsAmount;
        for (int i = 0; i < verticiesAmount; i++)
        {
            verticies[i] = hullB->absoluteVerticies[closestFaceB->points[i]];

            debugController->addDebugLine(
                hullB->absoluteVerticies[closestFaceB->points[i]] * 10.0f,
                hullB->absoluteVerticies[closestFaceB->points[(i + 1) % verticiesAmount]] * 10.0f,
                0.5f,
                0.02f,
                Vector3(0.1f, 1.0f, 0.1f));
        }

        numContactsOut = clipFaceAgainstHull(separatingNormal,
                                             hullA,
                                             verticies,
                                             verticiesAmount,
                                             minDist,
                                             maxDist,
                                             contactsOut,
                                             contactCapacity);
    }

    return numContactsOut;
}

void HullCliping::clipHullAgainstHull(
    Hull *hullA,
    Hull *hullB,
    Vector3 sepNormal,
    CollisionManifold *manifold)
{
    // printf("SEPARATION %f %f %f\n", sepNormal.x, sepNormal.y, sepNormal.z);

    Vector4 contactsOut[MAX_POINTS];
    const int contactCapacity = MAX_POINTS;

    const float minDist = -1.0f;
    const float maxDist = 0.0f;

    int numContactsOut = clipHullAgainstHull(sepNormal, hullA, hullB, minDist, maxDist, contactsOut, contactCapacity);

    if (numContactsOut > 0)
    {
        Vector3 normalOnSurfaceB = -sepNormal;

        int contacts4[4] = {0, 1, 2, 3};
        int numPoints = reduceContacts(contactsOut, numContactsOut, normalOnSurfaceB, contacts4);

        Vector3 middle(0.0f);
        float depth = 0.0f;
        manifold->normal = normalOnSurfaceB;
        for (int p = 0; p < numPoints; p++)
        {
            depth = fminf(contactsOut[contacts4[p]].w, depth);
            middle += Vector3(contactsOut[contacts4[p]]) / (float)numPoints;
        }
        manifold->depth = -depth;
        manifold->addCollisionPoint(middle, middle);

        // debugController->addDebugLine(middle * 10.0f, middle * 10.0f + manifold->normal, 2.0f, 0.02f, Vector3(1.0f, 1.0f, 1.0f));
    }
}