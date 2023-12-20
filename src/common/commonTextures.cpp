#include "commonTextures.h"
#include "renderer/renderer.h"

Texture *CommonTextures::blackTexture = nullptr;
Texture *CommonTextures::whiteTexture = nullptr;
Texture *CommonTextures::greyTexture = nullptr;
Texture *CommonTextures::zeroNormalTexture = nullptr;

void CommonTextures::build(Renderer *renderer)
{
    logger->logff("compiling common textures ...");

    // Black
    const unsigned char blackData[4] = {0x00, 0x00, 0x00, 0xff};
    blackTexture = renderer->createTexture(1, 1, 4, blackData, false);

    // White
    const unsigned char whiteData[4] = {0xe0, 0xe0, 0xe0, 0xff};
    whiteTexture = renderer->createTexture(1, 1, 4, whiteData, false);

    // Grey
    const unsigned char greyData[4] = {0xAA, 0xAA, 0xAA, 0xff};
    greyTexture = renderer->createTexture(1, 1, 4, greyData, false);

    // Zero Normal
    const unsigned char normalData[4] = {0x80, 0x80, 0xff, 0xff};
    zeroNormalTexture = renderer->createTexture(1, 1, 4, normalData, false);

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