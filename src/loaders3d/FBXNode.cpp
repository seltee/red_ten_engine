// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "loaders3d/FBXNode.h"
#include "loaders/stb_image.h"

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
    float *fData;
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

        case 'C':
            iValue = *((char *)&property[p + 1]);
            p += 2;
            iData = (int *)malloc(sizeof(char));
            *iData = dValue;
            bindedData.push_back(FBXNodeDataBinding({'C', (void *)iData, 1}));
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

        case 'f':
            arrayLength = *((unsigned int *)&property[p + 1]);
            encoding = *((unsigned int *)&property[p + 5]);
            compressedLength = *((unsigned int *)&property[p + 9]);
            destLen = arrayLength * sizeof(float);
            sourceLen = compressedLength;

            p += 13;
            fData = (float *)malloc(destLen);
            dataLength = arrayLength;

            if (encoding)
                b = stbi_zlib_decode_buffer((char *)fData, destLen, &property[p], compressedLength);
            else
                memcpy(fData, &property[p], destLen);

            bindedData.push_back(FBXNodeDataBinding({'f', (void *)fData, arrayLength}));
            break;

        case 'i':
            arrayLength = *((unsigned int *)&property[p + 1]);
            encoding = *((unsigned int *)&property[p + 5]);
            compressedLength = *((unsigned int *)&property[p + 9]);
            destLen = arrayLength * sizeof(unsigned int);
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

        case 'l':
            arrayLength = *((unsigned long long *)&property[p + 1]);
            encoding = *((unsigned long long *)&property[p + 5]);
            compressedLength = *((unsigned long long *)&property[p + 9]);
            destLen = arrayLength * sizeof(unsigned long long);
            sourceLen = compressedLength;

            p += 13;
            lData = (unsigned long long *)malloc(destLen);
            dataLength = arrayLength;

            if (encoding)
                b = stbi_zlib_decode_buffer((char *)lData, destLen, &property[p], compressedLength);
            else
                memcpy(lData, &property[p], destLen);

            bindedData.push_back(FBXNodeDataBinding({'l', (void *)lData, arrayLength}));
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
        if (it.type == 'C')
            printf("%s", *static_cast<char *>(it.data) ? "true" : false);
        if (it.type == 'F')
            printf("%f", *static_cast<double *>(it.data));
        if (it.type == 'L')
            printf("%llu", *static_cast<unsigned long long *>(it.data));
        if (it.type == 'd')
            printf("Double array (num %i)", it.numElements);
        if (it.type == 'f')
            printf("Float array (num %i)", it.numElements);
        if (it.type == 'i')
            printf("int array (num %i)", it.numElements);
        if (it.type == 'l')
            printf("long long array (num %i)", it.numElements);

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
        (strcmp(name, "Documents") == 0) ||
        (strcmp(name, "References") == 0) ||
        (strcmp(name, "Definitions") == 0) ||
        (strcmp(name, "Video") == 0) ||
        (strcmp(name, "Material") == 0) ||
        (strcmp(name, "Texture") == 0));
}