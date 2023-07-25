#include "shapeConvex.h"

Hull *ShapeConvex::setNewHull(Vector3 *verticies, int amount)
{
    if (hull)
        delete hull;
    hull = new Hull(verticies, amount);
    return hull;
}

void ShapeConvex::provideTransformation(Matrix4 *transformation)
{
    this->transformation = *transformation;
    isDirty = true;
    updateTransformation();
}

void ShapeConvex::updateTransformation()
{
    if (isDirty)
    {
        isDirty = false;
        Hull *hull = getHull();
        aabb = AABB(Vector3(FLT_MAX), Vector3(-FLT_MAX));

        if (hull)
        {
            hull->updateAbsoluteForm(&this->transformation, &aabb);
        }
    }
}

ShapeCollisionType ShapeConvex::getType()
{
    return ShapeCollisionType::Convex;
}

// From Christer Ericson - Real-Time Collision Detection
bool ShapeConvex::testRay(Line line, std::vector<RayCollisionPoint> *points)
{
    // Compute direction vector for the segment
    Hull *hull = getHull();
    Vector3 d = line.b - line.a;

    // Set initial interval to being the whole segment.
    float pointfirst = 0.0f;
    float pointlast = 1.0f;

    // Intersect segment against each polygon
    for (auto polygonIt = hull->polies.begin(); polygonIt != hull->polies.end(); polygonIt++)
    {
        // Normal of the polygon = normal of it's plain
        Vector3 normal = polygonIt->absoluteNormal;
        float denom = glm::dot(normal, d);

        // Distance from center to polygon plane
        float plainDistance = glm::dot(normal, hull->absoluteVerticies[polygonIt->points[0]]);
        float lineOriginDistance = plainDistance - glm::dot(normal, line.a);

        // Test if segment runs parallel to the plane
        if (denom == 0.0f)
        {
            if (lineOriginDistance > 0.0f)
                return false;
        }
        else
        {
            // Compute parameterized t value for intersection with current plane
            float t = lineOriginDistance / denom;
            if (denom < 0.0f)
            {
                // Entering halfspace
                if (t > pointfirst)
                    pointfirst = t;
            }
            else
            {
                // Exiting halfspace
                if (t < pointlast)
                    pointlast = t;
            }
            // Exit with “no intersection” if intersection becomes empty
            if (pointfirst > pointlast)
                return false;
        }
    }

    // The segment intersects the polyhedron, return first and last point
    // For polygedron there are always will be only 2 points
    float length = glm::length(d);
    points->push_back({line.a + pointfirst * d, pointfirst * length});
    points->push_back({line.a + pointlast * d, pointlast * length});
    return true;
}

AABB ShapeConvex::getAABB()
{
    return aabb;
}

FaceQuery ShapeConvex::queryFaceDirection(ShapeConvex *foreignShape)
{
    Hull *hull = getHull();
    float maxDistance = -FLT_MAX;
    HullPolygon *outPolygon = nullptr;
    Vector3 outNormal(0.0f);

    for (auto polygonIt = hull->polies.begin(); polygonIt != hull->polies.end(); polygonIt++)
    {
        Vector3 plainPointA = hull->absoluteVerticies[polygonIt->points[0]];
        Vector3 plainNormalA = polygonIt->absoluteNormal;

        Vector3 vertexFar = foreignShape->findFurthestPoint(-plainNormalA);

        float distance = glm::dot(plainNormalA, vertexFar - plainPointA);

        if (distance > maxDistance)
        {
            outPolygon = &(*polygonIt);
            outNormal = plainNormalA;
            maxDistance = distance;
        }
    }

    // printf("SELECTED %f\n", maxDistance);
    return FaceQuery({maxDistance, outPolygon, outNormal});
}

EdgeQuery ShapeConvex::queryEdgeDirection(ShapeConvex *foreignShape)
{
    Hull *hull = getHull();
    Hull *foreignHull = foreignShape->getHull();

    HullEdge *maxIndexA = nullptr;
    HullEdge *maxIndexB = nullptr;

    float maxSeparation = -FLT_MAX;
    Vector3 axis(0.0f);

    for (auto edgeIt = hull->edges.begin(); edgeIt != hull->edges.end(); edgeIt += 2)
    {
        HullEdge edge = *edgeIt;
        HullEdge twin = *(edgeIt + 1);

        Vector3 P1 = hull->absoluteVerticies[edge.a];
        Vector3 Q1 = hull->absoluteVerticies[twin.a];
        Vector3 E1 = Q1 - P1;

        Vector3 U1 = edge.polygon->absoluteNormal;
        Vector3 V1 = twin.polygon->absoluteNormal;

        for (auto foreignEdgeIt = foreignHull->edges.begin(); foreignEdgeIt != foreignHull->edges.end(); foreignEdgeIt += 2)
        {
            HullEdge edgeForeign = *foreignEdgeIt;
            HullEdge twinForeign = *(foreignEdgeIt + 1);

            Vector3 P2 = foreignHull->absoluteVerticies[edgeForeign.a];
            Vector3 Q2 = foreignHull->absoluteVerticies[twinForeign.a];
            Vector3 E2 = Q2 - P2;

            Vector3 U2 = edgeForeign.polygon->absoluteNormal;
            Vector3 V2 = twinForeign.polygon->absoluteNormal;

            if (isMinkowskiFace(U1, V1, -E1, -U2, -V2, -E2))
            {
                Vector3 newAxis;
                float separation = project(P1, E1, P2, E2, hull->hullCenter, newAxis);
                if (separation > maxSeparation)
                {
                    maxIndexA = &(*edgeIt);
                    maxIndexB = &(*foreignEdgeIt);
                    maxSeparation = separation;
                    axis = newAxis;
                }
            }
        }
    }

    return EdgeQuery({maxSeparation,
                      maxIndexA,
                      maxIndexB,
                      axis});
}

Vector3 ShapeConvex::findFurthestPoint(Vector3 direction)
{

    Hull *hull = getHull();
    if (hull)
    {
        int outVertex;
        float maxDistance = -FLT_MAX;

        for (int i = 0; i < hull->amountOfVertices; i++)
        {
            float distance = glm::dot(hull->absoluteVerticies[i], direction);
            if (distance > maxDistance)
            {
                maxDistance = distance;
                outVertex = i;
            }
        }

        return hull->absoluteVerticies[outVertex];
    }
    return Vector3(0.0f, 0.0f, 0.0f);
}

Vector3 ShapeConvex::getClosestPointToHull(Vector3 point)
{
    float minDistance = MAXFLOAT;
    Vector3 closestPoint(0);
    Hull *hull = getHull();
    if (!hull)
        return closestPoint;

    for (auto polygonIt = hull->polies.begin(); polygonIt != hull->polies.end(); polygonIt++)
    {
        Vector3 polyPoints[8];
        for (int i = 0; i < (*polygonIt).pointsAmount;i++){
            polyPoints[i] = hull->absoluteVerticies[(*polygonIt).points[i]];
        }

        Vector3 closestCurrent = getClosestPointOnPolygon(polyPoints, (*polygonIt).pointsAmount, (*polygonIt).absoluteNormal, point);
        float distance = glm::length(point - closestCurrent);
        if (distance < minDistance){
            minDistance = distance;
            closestPoint = closestCurrent;
        }
    }
    return closestPoint;
}

void ShapeConvex::renderDebug(Matrix4 *projectionView, Matrix4 *model, float scale, float thickness)
{
    Hull *hull = getHull();
    if (hull)
    {
        updateTransformation();

        for (auto edgeIt = hull->edges.begin(); edgeIt != hull->edges.end(); edgeIt++)
        {
            Vector3 vA = hull->absoluteVerticies[edgeIt->a] * scale;
            Vector3 vB = hull->absoluteVerticies[edgeIt->b] * scale;
            debug->renderLine(vA, vB, projectionView, thickness, Vector3(0.9f, 0.9f, 0.9f));

            if (edgeIt->polygon)
            {
                Vector3 center = (vA + vB) / 2.0f;
                Vector3 normal = edgeIt->polygon->absoluteNormal;
                debug->renderLine(center, center + normal * 0.2f, projectionView, thickness, Vector3(0.2f, 0.2f, 0.9f));
            }
        }
    }
}
