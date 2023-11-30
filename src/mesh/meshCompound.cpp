#include "mesh/meshCompound.h"
#include <algorithm>

MeshCompound::MeshCompound()
{
}

MeshCompound::~MeshCompound()
{
}

void MeshCompound::render(Shader *shader, Matrix4 &vpMatrix, Matrix4 &modelMatrix)
{
    printf("COMPOUND RENDER\n");
}

Mesh *MeshCompound::createInstance()
{
    MeshCompound *newMesh = new MeshCompound;
    for (auto &it : nodes)
    {
        newMesh->addMesh(it->mesh);
    }
    return newMesh;
}

MeshCompoundNode *MeshCompound::addMesh(MeshStatic *mesh)
{
    auto newNode = new MeshCompoundNode(mesh);
    nodes.push_back(newNode);
    return newNode;
}

bool MeshCompound::setParent(Mesh *child, Mesh *parent)
{
    MeshCompoundNode *childNode = getNodeByMesh(child);
    MeshCompoundNode *parentNode = getNodeByMesh(parent);

    if (childNode && parentNode)
    {
        childNode->parent = parentNode;
        return true;
    }
    return false;
}

MeshStatic *MeshCompound::getAsStatic()
{
    if (meshStatic)
        return meshStatic;

    if (nodes.size() == 0)
        return nullptr;

    meshStatic = new MeshStatic();

    // Collect amount of verticies
    int amountOfVerticies = 0;
    for (auto &node : nodes)
    {
        amountOfVerticies += node->mesh->getVertexAmount();
    }

    // vertex - 3, normals - 3, UVs - 2
    const int floatPerVertex = 8;
    float *data = new float[floatPerVertex * amountOfVerticies];
    memset(data, 0, floatPerVertex * amountOfVerticies * sizeof(float));

    // copying data
    int dataShift = 0;
    for (auto &node : nodes)
    {
        int meshFloatsPerVertex = node->mesh->getFloatsPerVertex();
        int meshVertexAmount = node->mesh->getVertexAmount();
        float *meshVertexData = node->mesh->getVertexData();

        // Ignore old tangents cause they will be outdated with apply of transformation
        auto modelMatrix = getTransformationMatrix(node);
        auto rq = node->transform.getRotation();

        for (int i = 0; i < meshVertexAmount; i++)
        {
            int fromShift = i * meshFloatsPerVertex;
            int toShift = i * floatPerVertex + dataShift;

            Vector4 vp = Vector4(meshVertexData[fromShift], meshVertexData[fromShift + 1], meshVertexData[fromShift + 2], 1.0f);
            Vector3 vn = Vector3(meshVertexData[fromShift + 3], meshVertexData[fromShift + 4], meshVertexData[fromShift + 5]);

            Vector4 mvp = modelMatrix * vp;
            Vector3 mvr = glm::normalize(glm::rotate(rq, vn));

            data[toShift] = mvp.x;
            data[toShift + 1] = mvp.y;
            data[toShift + 2] = mvp.z;
            data[toShift + 3] = mvr.x;
            data[toShift + 4] = mvr.y;
            data[toShift + 5] = mvr.z;
            data[toShift + 6] = meshVertexData[fromShift + 6];
            data[toShift + 7] = meshVertexData[fromShift + 7];
        }

        dataShift += meshVertexAmount * floatPerVertex;
    }

    int attributeSizes[3] = {3, 3, 2};
    meshStatic->setupFloatsArray(data, amountOfVerticies, 3, attributeSizes, true);

    delete[] data;
    return meshStatic;
}

MeshCompoundNode *MeshCompound::getNodeByMesh(Mesh *mesh)
{
    for (auto &it : nodes)
    {
        if (it->mesh == mesh)
        {
            return it;
        }
    }
    return nullptr;
}

Matrix4 MeshCompound::getTransformationMatrix(MeshCompoundNode *node)
{
    if (!node->parent)
    {
        return *node->transform.getModelMatrix();
    }
    else
    {
        Matrix4 out;
        std::vector<Matrix4 *> list;
        MeshCompoundNode *current = node;
        while (current)
        {
            list.push_back(current->transform.getModelMatrix());
            current = current->parent;
        }

        for (auto it = list.rbegin(); it != list.rend(); ++it)
        {
            out = out * **it;
        }

        return out;
    }
}