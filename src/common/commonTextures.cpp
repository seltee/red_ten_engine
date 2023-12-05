#include "commonTextures.h"
#include "opengl/glew.h"

Texture *CommonTextures::blackTexture = nullptr;
Texture *CommonTextures::whiteTexture = nullptr;
Texture *CommonTextures::greyTexture = nullptr;
Texture *CommonTextures::zeroNormalTexture = nullptr;

void CommonTextures::build()
{
    logger->logff("compiling common textures ...");

    // Black
    const unsigned char blackData[4] = {0x00, 0x00, 0x00, 0xff};
    unsigned int tBlackID;
    glGenTextures(1, &tBlackID);
    glBindTexture(GL_TEXTURE_2D, tBlackID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, blackData);

    blackTexture = new Texture(tBlackID);

    // White
    const unsigned char whiteData[4] = {0xe0, 0xe0, 0xe0, 0xff};
    unsigned int tWhiteID;
    glGenTextures(1, &tWhiteID);
    glBindTexture(GL_TEXTURE_2D, tWhiteID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whiteData);

    whiteTexture = new Texture(tWhiteID);

    // Grey
    const unsigned char greyData[4] = {0xAA, 0xAA, 0xAA, 0xff};
    unsigned int tGreyID;
    glGenTextures(1, &tGreyID);
    glBindTexture(GL_TEXTURE_2D, tGreyID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, greyData);

    greyTexture = new Texture(tGreyID);

    // Zero Normal
    unsigned int tZeroNormalID;
    const unsigned char normalData[4] = {0x80, 0x80, 0xff, 0xff};
    glGenTextures(1, &tZeroNormalID);
    glBindTexture(GL_TEXTURE_2D, tZeroNormalID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, normalData);

    zeroNormalTexture = new Texture(tZeroNormalID);
    
    logger->logff("common textures compiled");
}

Texture *CommonTextures::getBlackTexture()
{
    return CommonTextures::blackTexture;
}
Texture *CommonTextures::getWhiteTexture()
{
    return CommonTextures::whiteTexture;
}
Texture *CommonTextures::getGreyTexture()
{
    return CommonTextures::greyTexture;
}
Texture *CommonTextures::getZeroNormalTexture()
{
    return CommonTextures::zeroNormalTexture;
}