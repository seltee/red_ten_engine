#pragma once
#include "math/math.h"
#include "physics/hull.h"
#include "physics/collisionManifold.h"
#include "math/reduceContacts.h"
#include "connector/withDebug.h"

#define MAX_VERTS 1024

class HullCliping : public WithDebug
{
public:
    static int clipFace(const Vector3 *pVtxIn, int numVertsIn, Vector3 &planeNormalWS, float planeEqWS, Vector3 *ppVtxOut);

    static int clipFaceAgainstHull(
        const Vector3 &separatingNormal,
        const Hull *hullA,
        Vector3 *verticies,
        int verticiesAmount,
        const float minDist,
        const float maxDist,
        Vector4 *contactsOut,
        int contactCapacity);

    static int clipHullAgainstHull(const Vector3 &separatingNormal,
                                   const Hull *hullA,
                                   const Hull *hullB,
                                   const float minDist,
                                   float maxDist,
                                   Vector4 *contactsOut,
                                   int contactCapacity);

    static void clipHullAgainstHull(
        Hull *hullA,
        Hull *hullB,
        Vector3 sepNormal,
        CollisionManifold *manifold);
};
