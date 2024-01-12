// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "ui/uiStyle.h"
#include "ui/uiRenderData.h"
#include "ui/uiChierarchyElement.h"
#include "common/destroyable.h"
#include "renderer/renderer.h"
#include <vector>

class UINode;
class UINodeInput;

enum class UIPointerEvent
{
    CursorHover,
    CursorBlur,
    CursorMove,
    CursorClickLeftPress,
    CursorClickRightPress,
    CursorClickLeftRelease,
    CursorClickRightRelease,
    Scroll,
};

struct PointerCallbackData
{
    UIPointerEvent event;
    void *userData;
    float relativeX, relativeY;
    float globalX, globalY;
    float value;
    float zoom;
};

// return true if should propagate farther
using UIPointerCallback = bool (*)(PointerCallbackData *data);

struct UINodeAbsolutePosition
{
    UINode *node;
    UIRenderData renderData;
};

struct UINodePositioning
{
    float sx;
    float sy;
    float betweenH;
    float betweenV;
};

class UINode : public Destroyable
{
public:
    EXPORT UINode(UINode *parent);
    EXPORT virtual ~UINode();

    EXPORT void process(float delta, float zoom);
    EXPORT void recalcSize(float zoom);
    EXPORT void buildChierarchy(UIChierarchyElement *current, UIChierarchyElement *root);

    EXPORT virtual void onProcess(float delta, float zoom);

    EXPORT UINode *createChildNode();
    EXPORT UINodeInput *createChildInputNode();
    EXPORT void clearChildren();

    EXPORT void setText(std::string text);
    EXPORT std::string getText();

    EXPORT void setPointerCallback(UIPointerCallback callback);
    EXPORT UIPointerCallback getPointerCallback();
    EXPORT void setUserCallbackData(void *data);
    EXPORT void *getUserCallbackData();

    EXPORT float getScroll();
    EXPORT void doScroll(float scroll, float maxScroll);

    EXPORT virtual void beforeRender();
    EXPORT virtual void afterRender();

    EXPORT inline float getContentWidth() { return contentWidth; }
    EXPORT inline float getContentHeight() { return contentHeight; }
    EXPORT inline float getContentDimensionsWidth() { return contentDimensionsWidth; }
    EXPORT inline float getContentDimensionsHeight() { return contentDimensionsHeight; }

    EXPORT inline float getImageX() { return imageX; }
    EXPORT inline float getImageY() { return imageY; }
    EXPORT inline float getImageWidth() { return imageWidth; }
    EXPORT inline float getImageHeight() { return imageHeight; }

    UIStyle style;
    bool isHover = false;

protected:
    void recalcFixedSizes();
    void recalcAdaptiveSizes(float zoom);
    void recalcPercentageSizes(float proposedWidth, float proposedHeight);
    void recalcImageSize(float zoom);
    void recalcContentDimensions(float zoom);

    UINodePositioning calcPositioning(UIRenderData *renderData);
    UINodePositioning correctChildPositioning(UINode *child, UINodePositioning &positioning, UIRenderData *renderData);
    UINodePositioning shiftInlinePositioning(UINode *child, UINodePositioning &positioning);

    void fillChildRenderData(float posX, float posY, UINode *child, UIRenderData *ownData, UIRenderData *childData);

    UINode *parent = nullptr;
    std::vector<UINode *> children;
    std::string text;

    UIPointerCallback pointerCallback = nullptr;

    float contentWidth = 0.0f;
    float contentHeight = 0.0f;
    float imageX = 0.0f;
    float imageY = 0.0f;
    float imageWidth = 0.0f;
    float imageHeight = 0.0f;

    // real
    float contentDimensionsWidth = 0.0f;
    float contentDimensionsHeight = 0.0f;

    void *userCallbackData = nullptr;

    float scroll = 0.0f;
    float maxScroll = 0.0f;
};