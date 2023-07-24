#include "collisionMesh.h"
#include "string"

CollisionMesh::CollisionMesh()
{
}

void CollisionMesh::setPoints(Vector3 *points, int amount)
{
    if (this->points)
        delete this->points;
    this->points = new Vector4[amount];

    for (int i = 0; i < amount; i++)
    {
        this->points[i] = Vector4({points[i].x, points[i].y, points[i].z, 1.0f});
    }

    pointsAmount = amount;
}

void CollisionMesh::setIndexes(PolygonIndexs *indexes, int amount)
{
    if (this->indexes)
        delete this->indexes;
    this->indexes = new PolygonIndexs[amount];
    memcpy(this->indexes, indexes, sizeof(PolygonIndexs) * amount);
    indexesAmount = amount;
}

void CollisionMesh::update(Matrix4 *m)
{
    polygons.clear();
    for (int i = 0; i < indexesAmount; i++)
    {
        Vector4 p1 = (*m) * points[indexes[i].a];
        Vector4 p2 = (*m) * points[indexes[i].b];
        Vector4 p3 = (*m) * points[indexes[i].c];

        Vector3 normal = glm::normalize(glm::cross((Vector3(p2.x, p2.y, p2.z) - Vector3(p1.x, p1.y, p1.z)), (Vector3(p3.x, p3.y, p3.z) - Vector3(p1.x, p1.y, p1.z))));
        polygons.push_back({p1, p2, p3, normal});
    }
}

std::vector<Triangle> *CollisionMesh::getPolygons()
{
    return &polygons;
}

bool CollisionMesh::testCollision(CollisionMesh *foreignMesh)
{
    std::vector<Triangle> *foreignPolygons = foreignMesh->getPolygons();

    Vector3 summ;

    for (auto pol1 = polygons.begin(); pol1 != polygons.end(); pol1++)
    {
        Vector3 triPoint = (pol1->a + pol1->b + pol1->c) / 3.0f;

        float A = pol1->n.x;
        float B = pol1->n.y;
        float C = pol1->n.z;
        float D = -A * triPoint.x - B * triPoint.y - C * triPoint.z;

        for (auto pol2 = foreignPolygons->begin(); pol2 != foreignPolygons->end(); pol2++)
        {
            float t1 = -(A * pol2->b.x + B * pol2->b.y + C * pol2->b.z + D) / (A * (pol2->a.x - pol2->b.x) + B * (pol2->a.y - pol2->b.y) + C * (pol2->a.z - pol2->b.z));
            if (t1 >= 0.0f && t1 <= 1.0f)
            {
                Vector3 pointInPlane = Vector3({pol2->b.x * t1 + pol2->a.x * (1 - t1),
                                                pol2->b.y * t1 + pol2->a.y * (1 - t1),
                                                pol2->b.z * t1 + pol2->a.z * (1 - t1)});
                if (pointInTriangle(pol1->a, pol1->b, pol1->c, pointInPlane))
                {
                    float sw = abs(t1 - 0.5f);
                    Vector3 p = (pol2->a - pol2->b) * t1;
                    summ += p;
                    printf("INTERSECTION %f %f %f %f %f\n", p.x, p.y, p.z, glm::length(p), sw);
                }
            }

            float t2 = -(A * pol2->c.x + B * pol2->c.y + C * pol2->c.z + D) / (A * (pol2->b.x - pol2->c.x) + B * (pol2->b.y - pol2->c.y) + C * (pol2->b.z - pol2->c.z));
            if (t2 >= 0.0f && t2 <= 1.0f)
            {
                Vector3 pointInPlane = Vector3({pol2->c.x * t2 + pol2->b.x * (1 - t2),
                                                pol2->c.y * t2 + pol2->b.y * (1 - t2),
                                                pol2->c.z * t2 + pol2->b.z * (1 - t2)});
                if (pointInTriangle(pol1->a, pol1->b, pol1->c, pointInPlane))
                {
                    float sw = abs(t2 - 0.5f);
                    Vector3 p = (pol2->b - pol2->c) * t2;
                    summ += p;
                    printf("INTERSECTION %f %f %f %f %f\n", p.x, p.y, p.z, glm::length(p), sw);
                }
            }

            float t3 = -(A * pol2->a.x + B * pol2->a.y + C * pol2->a.z + D) / (A * (pol2->c.x - pol2->a.x) + B * (pol2->c.y - pol2->a.y) + C * (pol2->c.z - pol2->a.z));
            if (t3 >= 0.0f && t3 <= 1.0f)
            {
                Vector3 pointInPlane = Vector3({pol2->a.x * t2 + pol2->c.x * (1 - t2),
                                                pol2->a.y * t2 + pol2->c.y * (1 - t2),
                                                pol2->a.z * t2 + pol2->c.z * (1 - t2)});
                if (pointInTriangle(pol1->a, pol1->b, pol1->c, pointInPlane))
                {
                    float sw = abs(t3 - 0.5f);
                    Vector3 p = (pol2->c - pol2->a) * t3;
                    summ += p;
                    printf("INTERSECTION %f %f %f %f %f\n", p.x, p.y, p.z, glm::length(p), sw);
                }
            }
        }
    }
    return false;
}
