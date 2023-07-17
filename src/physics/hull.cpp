#include "hull.h"

Hull::Hull(Vector3 *verticies, int amount)
{
    this->amountOfVertices = amount;
    this->verticies = new Vector3[amount];
    this->absoluteVerticies = new Vector3[amount];

    for (int i = 0; i < amount; i++)
    {
        this->verticies[i] = verticies[i];
        this->absoluteVerticies[i] = verticies[i];
    }
}

HullPolygon Hull::addPolygon(int *points, int numOfVerticies)
{
    HullPolygon polygon;
    if (numOfVerticies > 7)
        numOfVerticies = 7;
    for (int i = 0; i < numOfVerticies; i++)
        polygon.points[i] = points[i];
    polygon.pointsAmount = numOfVerticies;
    polygon.normal = getPolygonNormal(
        verticies[points[0]],
        verticies[points[1]],
        verticies[points[2]]);
    polygon.absoluteNormal = polygon.normal;
    polies.push_back(polygon);
    return polygon;
}

void Hull::updateAbsoluteForm(Matrix4 *transformation, AABB *aabb)
{
    for (int i = 0; i < amountOfVertices; i++)
    {
        Vector3 aPosition = Vector3(*transformation * Vector4(verticies[i], 1.0f));
        absoluteVerticies[i] = aPosition;
        aabb->start.x = fminf(aPosition.x, aabb->start.x);
        aabb->start.y = fminf(aPosition.y, aabb->start.y);
        aabb->start.z = fminf(aPosition.z, aabb->start.z);
        aabb->end.x = fmaxf(aPosition.x, aabb->end.x);
        aabb->end.y = fmaxf(aPosition.y, aabb->end.y);
        aabb->end.z = fmaxf(aPosition.z, aabb->end.z);
    }

    for (auto it = polies.begin(); it != polies.end(); it++)
    {
        it->absoluteNormal = getPolygonNormal(
            absoluteVerticies[it->points[0]],
            absoluteVerticies[it->points[1]],
            absoluteVerticies[it->points[2]]);
   }

    hullCenter = Vector3(*transformation * Vector4(0.0f, 0.0f, 0.0f, 1.0f));
}

void Hull::rebuildEdges()
{
    edges.clear();
    int indexp = 0;
    for (auto it = polies.begin(); it != polies.end(); it++)
    {
        for (int edgeIndex = 0; edgeIndex < it->pointsAmount; edgeIndex++)
        {
            int nextIndex = (edgeIndex + 1) % it->pointsAmount;
            HullEdge edge = HullEdge({it->points[edgeIndex], it->points[nextIndex], &(*it)});

            bool found = false;
            for (auto edgeIt = edges.begin(); edgeIt != edges.end(); edgeIt++)
            {
                if ((edgeIt->a == edge.a && edgeIt->b == edge.b) || (edgeIt->a == edge.b && edgeIt->b == edge.a))
                {
                    if (edgeIt->polygon == nullptr)
                        edgeIt->polygon = &(*it);
                    found = true;
                }
            }
            if (!found)
            {
                edges.push_back(edge);
                edges.push_back(HullEdge({edge.b, edge.a, nullptr}));
            }
        }
        indexp++;
    }
}