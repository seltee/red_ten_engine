// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "loaders3d/loaderFBX.h"
#include "loaders/stb_image.h"
#include "common/meshDescriptor.h"
#include "mesh/meshStatic.h"
#include <stdio.h>

LoaderFBX::LoaderFBX(std::string path) : Loader3d(path)
{
}

LoaderFBX::~LoaderFBX()
{
}

MeshCompound *LoaderFBX::getMeshCompound()
{
    if (!meshCompound)
        reload();
    return meshCompound;
}

void LoaderFBX::reload()
{
    if (meshCompound)
        delete meshCompound;
    meshCompound = nullptr;

    FILE *file = fopen(path.c_str(), "rb");
    if (file == nullptr)
        return;

    // Checking header
    char buff[21];
    fread(buff, 21, 1, file);
    if (std::strcmp(buff, "Kaydara FBX Binary  ") != 0)
    {
        logger->logff("File isn't FBX %s\n", path.c_str());
        return;
    }

    fread(buff, 2, 1, file);
    if (buff[0] != 0x1A || buff[1] != 0x00)
    {
        logger->logff("Wrong FBX code %s\n", path.c_str());
        return;
    }

    unsigned int version;
    fread(&version, 4, 1, file);
    if (version != 7400)
    {
        if (version < 7400)
        {
            logger->logff("Warning! FBX version is less than 7.4, errors of reading may happen, %s\n", path.c_str());
        }
        else
        {
            logger->logff("Versions of FBX higher than 7.4 are unsupported, failed to load %s\n", path.c_str());
            return;
        }
    }

    // Passed the headers check
    meshCompound = new MeshCompound();

    // loop through the nodes
    while (1)
    {
        FBXNode *node = new FBXNode(0, file);
        node->process();
        if (node->bIsZero)
            break;
        if (node->bMarkedToRemove)
            delete node;
        else
            nodes.push_back(node);
    }

    FBXNode *objects = nullptr;
    FBXNode *connections = nullptr;

    for (auto &it : nodes)
    {
        // it->print();
        if (it->isName("Objects"))
            objects = it;
        if (it->isName("Connections"))
            connections = it;
    }

    if (!objects || !connections)
    {
        logger->logff("Objects or connections weren't found for %s\n", path.c_str());
        return;
    }

    // collecting geometry and models info
    for (auto &it : objects->children)
    {
        if (it->isName("Geometry"))
        {
            MeshDescriptor descriptor;

            FBXNode *vertices = it->findNode("Vertices");
            FBXNode *polygonVertexIndex = it->findNode("PolygonVertexIndex");
            FBXNode *elementUVs = it->findNode("LayerElementUV");
            FBXNode *elementNormals = it->findNode("LayerElementNormal");

            if (vertices && vertices->bindedData.size() > 0)
            {
                auto nodeData = vertices->bindedData.at(0);
                descriptor.provideVertex((double *)nodeData.data, nodeData.numElements);
            }
            if (polygonVertexIndex && polygonVertexIndex->bindedData.size() > 0)
            {
                auto nodeData = polygonVertexIndex->bindedData.at(0);
                descriptor.providePolygonIndexes((int *)nodeData.data, nodeData.numElements);
            }
            if (elementUVs)
            {
                FBXNode *UVs = elementUVs->findNode("UV");
                FBXNode *UVIndexes = elementUVs->findNode("UVIndex");
                if (UVs && UVs->bindedData.size() > 0)
                {
                    auto nodeData = UVs->bindedData.at(0);
                    descriptor.provideUVData((double *)nodeData.data, nodeData.numElements);
                }
                if (UVIndexes && UVIndexes->bindedData.size() > 0)
                {
                    auto nodeData = UVIndexes->bindedData.at(0);
                    descriptor.provideUVIndexes((int *)nodeData.data, nodeData.numElements);
                }
            }
            if (elementNormals)
            {
                FBXNode *normals = elementNormals->findNode("Normals");
                if (normals && normals->bindedData.size())
                {
                    auto nodeData = normals->bindedData.at(0);
                    descriptor.provideNormals((double *)nodeData.data, nodeData.numElements);
                }
            }

            unsigned long long index = *static_cast<unsigned long *>(it->bindedData.at(0).data);

            int amountOfFloats;
            float *data = descriptor.getAsFloatArray8f(&amountOfFloats);

            // create static mesh with gathered data
            MeshStatic *mesh = new MeshStatic();

            int amountOfVertexes = amountOfFloats / 8;
            int attributeSizes[3] = {3, 3, 2};
            mesh->setupFloatsArray(data, amountOfVertexes, 3, attributeSizes, true);

            delete[] data;

            meshes.push_back({mesh, index});
            continue;
        }

        if (it->isName("Model"))
        {
            FBXModelWithIndex mWithIndex;
            mWithIndex.index = *static_cast<unsigned long *>(it->bindedData.at(0).data);
            mWithIndex.name = std::string(reinterpret_cast<const char *>(it->bindedData.at(1).data));
            mWithIndex.position = Vector3(0.0f, 0.0f, 0.0f);
            mWithIndex.rotation = Vector3(0.0f, 0.0f, 0.0f);
            mWithIndex.scale = Vector3(0.0f, 0.0f, 0.0f);

            FBXNode *properties = it->findNode("Properties70");
            if (properties)
            {
                for (auto &property : properties->children)
                {
                    // n - none, p - position, r - rotation, s - scale
                    int entryIndex = 0;
                    char transformationType = 'n';
                    Vector3 out;
                    for (auto &p : property->bindedData)
                    {
                        if (p.type == 'S' && strcmp(reinterpret_cast<char *>(p.data), "Lcl Translation") == 0)
                            transformationType = 'p';
                        if (p.type == 'S' && strcmp(reinterpret_cast<char *>(p.data), "Lcl Rotation") == 0)
                            transformationType = 'r';
                        if (p.type == 'S' && strcmp(reinterpret_cast<char *>(p.data), "Lcl Scaling") == 0)
                            transformationType = 's';
                        if (p.type == 'D' && entryIndex < 3)
                        {
                            out[entryIndex] = static_cast<float>(*reinterpret_cast<double *>(p.data));
                            entryIndex++;
                        }
                    }
                    if (transformationType == 'p')
                        mWithIndex.position = out / 100.0f;

                    if (transformationType == 's')
                        mWithIndex.scale = out / 100.0f;

                    if (transformationType == 'r')
                        mWithIndex.rotation = out * CONST_PI / 180.0f;
                }
            }

            models.push_back(mWithIndex);
            continue;
        }
    }

    // connecting models and geometry

    float n = 0.0f;
    for (auto &it : connections->children)
    {
        unsigned long long indexFrom = *static_cast<unsigned long *>(it->bindedData.at(1).data);
        unsigned long long indexTo = *static_cast<unsigned long *>(it->bindedData.at(2).data);

        FBXModelWithIndex *foundModel = getModelByIndex(indexFrom);
        if (foundModel)
        {
            if (indexTo)
            {
                printf("TODO: IMPLEMENT PARENTING\n");
            }
            continue;
        }

        FBXMeshWithIndex *foundMesh = getMeshByIndex(indexFrom);
        if (foundMesh)
        {
            foundModel = getModelByIndex(indexTo);
            if (foundModel)
            {
                MeshStatic *instance = reinterpret_cast<MeshStatic *>(foundMesh->mesh->createInstance());
                MeshCompoundNode *meshCompoundNode = meshCompound->addMesh(instance);
                meshCompoundNode->transform.setPosition(foundModel->position);
                meshCompoundNode->transform.setRotation(foundModel->rotation);
                meshCompoundNode->transform.setScale(foundModel->scale);

                n += 2.0f;
            }
        }
    }
}

FBXModelWithIndex *LoaderFBX::getModelByIndex(unsigned long long index)
{
    for (auto &it : models)
    {
        if (it.index == index)
            return &it;
    }
    return nullptr;
}

FBXMeshWithIndex *LoaderFBX::getMeshByIndex(unsigned long long index)
{
    for (auto &it : meshes)
    {
        if (it.index == index)
            return &it;
    }
    return nullptr;
}

FBXNode::FBXNode(int level, FILE *file)
{
    this->level = level;
    this->file = file;
}

void FBXNode::process()
{
    readHead();
    if (name == 0)
    {
        bIsZero = true;
        return;
    }
    else
    {
        if (doSkip(name))
        {
            fseek(file, endOffset, SEEK_SET);
            bMarkedToRemove = true;
            return;
        }

        if (numProperties > 0)
            readValues();

        int p = ftell(file);
        while (p < endOffset)
        {
            FBXNode *child = new FBXNode(level + 1, file);
            if (!child)
                break;

            child->process();

            if (child->bIsZero)
            {
                delete child;
                break;
            }
            else if (child->bMarkedToRemove)
            {
                delete child;
                p = ftell(file);
                continue;
            }
            else
            {
                p = ftell(file);
                children.push_back(child);
            }
        }
    }
}

void FBXNode::readHead()
{
    FBXNodeRecordHeader header;
    fread(&header, sizeof(FBXNodeRecordHeader), 1, file);
    unsigned char nameLength;
    if (feof(file))
        return;

    fread(&nameLength, 1, 1, file);
    if (nameLength == 0)
        return;

    endOffset = header.endOffset;
    numProperties = header.numProperties;
    propertyListLen = header.propertyListLen;

    name = (char *)malloc(nameLength + 1);
    fread(name, nameLength, 1, file);
    name[nameLength] = 0;
    // printf("Entry %s, offset: %i, num: %i, listLen: %i\n", name, header.endOffset, header.numProperties, header.propertyListLen);
}

void FBXNode::readValues()
{
    if (!propertyListLen)
        return;

    int rawLength = 0, p = 0, b = 0, iValue;
    unsigned int stringLength = 0, arrayLength, encoding, compressedLength, destLen, sourceLen, dataLength;
    double dValue;
    unsigned long long longValue;

    char *cData;
    int *iData;
    double *dData;
    unsigned long long *lData;

    char *property = (char *)malloc(propertyListLen + 1);
    if (!property)
        return;

    fread(property, propertyListLen, 1, file);

    for (int i = 0; i < numProperties; i++)
    {
        switch (property[p])
        {
        case 'S':
            stringLength = *((int *)&property[p + 1]);
            p += 5;
            if (stringLength)
            {
                cData = (char *)malloc(stringLength + 1);
                memcpy(cData, property + p, stringLength);
                cData[stringLength] = 0;
                bindedData.push_back(FBXNodeDataBinding({'S', (void *)cData, stringLength}));

                p += stringLength;
            }
            break;

        case 'I':
            iValue = *((int *)&property[p + 1]);
            p += 5;
            iData = (int *)malloc(sizeof(int));
            *iData = dValue;
            bindedData.push_back(FBXNodeDataBinding({'I', (void *)iData, 1}));
            break;

        case 'R':
            rawLength = *((int *)&property[p + 1]);
            p += 5 + rawLength;
            bindedData.push_back(FBXNodeDataBinding({'R', (void *)0, 0}));
            break;

        case 'D':
            dValue = *((double *)&property[p + 1]);
            p += 9;
            dData = (double *)malloc(sizeof(double));
            *dData = dValue;
            bindedData.push_back(FBXNodeDataBinding({'D', (void *)dData, 1}));
            break;

        case 'L':
            longValue = *(reinterpret_cast<unsigned long long *>(&property[p + 1]));
            p += 9;
            lData = reinterpret_cast<unsigned long long *>(malloc(sizeof(unsigned long long)));
            *lData = longValue;
            bindedData.push_back(FBXNodeDataBinding({'L', (void *)lData, 1}));
            break;

        case 'd':
            arrayLength = *((unsigned int *)&property[p + 1]);
            encoding = *((unsigned int *)&property[p + 5]);
            compressedLength = *((unsigned int *)&property[p + 9]);
            destLen = arrayLength * sizeof(double);
            sourceLen = compressedLength;

            p += 13;
            dData = (double *)malloc(destLen);
            dataLength = arrayLength;

            if (encoding)
                b = stbi_zlib_decode_buffer((char *)dData, destLen, &property[p], compressedLength);
            else
                memcpy(dData, &property[p], destLen);

            bindedData.push_back(FBXNodeDataBinding({'d', (void *)dData, arrayLength}));
            break;

        case 'i':
            arrayLength = *((unsigned int *)&property[p + 1]);
            encoding = *((unsigned int *)&property[p + 5]);
            compressedLength = *((unsigned int *)&property[p + 9]);
            destLen = arrayLength * sizeof(int);
            sourceLen = compressedLength;

            p += 13;
            iData = (int *)malloc(destLen);
            dataLength = arrayLength;

            if (encoding)
                b = stbi_zlib_decode_buffer((char *)iData, destLen, &property[p], compressedLength);
            else
                memcpy(iData, &property[p], destLen);

            bindedData.push_back(FBXNodeDataBinding({'i', (void *)iData, arrayLength}));
            break;

        default:
            printf("Unknown type %c\n", property[p]);
            return;
        }
    }
    free(property);
}

void FBXNode::print()
{
    for (int i = 0; i < level; i++)
        printf(" ");

    printf("%s\n", name);
    for (auto &it : bindedData)
    {
        for (int i = 0; i < level; i++)
            printf(" ");
        printf("* ");

        if (it.type == 'S')
            printf("%s", static_cast<char *>(it.data));
        if (it.type == 'I')
            printf("%i", *static_cast<int *>(it.data));
        if (it.type == 'D')
            printf("%f", *static_cast<double *>(it.data));
        if (it.type == 'L')
            printf("%llu", *static_cast<unsigned long long *>(it.data));
        if (it.type == 'd')
            printf("Double array (num %i)", it.numElements);
        if (it.type == 'i')
            printf("Long long array (num %i)", it.numElements);

        printf("(%c)\n", it.type);
    }

    for (auto &it : children)
    {
        it->print();
    }
}

bool FBXNode::isName(const char *name)
{
    return strcmp(this->name, name) == 0;
}

FBXNode *FBXNode::findNode(const char *name)
{
    for (auto it = children.begin(); it != children.end(); it++)
        if ((*it)->isName(name))
            return (*it);
    return nullptr;
}

bool FBXNode::doSkip(const char *name)
{
    return (
        (strcmp(name, "FBXHeaderExtension") == 0) ||
        (strcmp(name, "Takes") == 0) ||
        (strcmp(name, "Document") == 0) ||
        (strcmp(name, "Definitions") == 0) ||
        (strcmp(name, "Video") == 0) ||
        (strcmp(name, "Material") == 0) ||
        (strcmp(name, "Texture") == 0));
}