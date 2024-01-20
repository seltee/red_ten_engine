// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "ui/uiNode.h"
#include "common/pawn.h"
#include "common/text.h"

class UINodeInput : public UINode, public Pawn
{
public:
    EXPORT UINodeInput(UINode *parent);
    EXPORT ~UINodeInput();

    EXPORT void setCursorPosition(float relativeX, float relativeY);
    EXPORT void setCursorSelection(float relativeX, float relativeY);
    EXPORT void startSelection();
    EXPORT void setSelectPointStart(float x, float y);
    EXPORT inline float getLineHeight() { return lineHeight; };

    EXPORT std::string getText();
    EXPORT void setText(std::string str) override;

    EXPORT void setMultilineEnabled(bool state);
    EXPORT void setSuperFocus(bool state);

    EXPORT void focus();
    EXPORT void unfocus();

    EXPORT static UINodeInput *getCurrentInput();

    Text text;

    bool bHover = false;

protected:
    struct UINodeInputPosition
    {
        int positionH = 0;
        int positionV = 0;
    };

    void inputText(std::string str);
    void inputKeyboardBackspace(InputType type, int deviceIndex, int index, bool state);
    void inputKeyboardDelete(InputType type, int deviceIndex, int index, bool state);
    void inputKeyboardReturn(InputType type, int deviceIndex, int index, bool state);
    void inputKeyboardHome(InputType type, int deviceIndex, int index, bool state);
    void inputKeyboardEnd(InputType type, int deviceIndex, int index, bool state);
    void inputKeyboardUp(InputType type, int deviceIndex, int index, bool state);
    void inputKeyboardDown(InputType type, int deviceIndex, int index, bool state);
    void inputKeyboardLeft(InputType type, int deviceIndex, int index, bool state);
    void inputKeyboardRight(InputType type, int deviceIndex, int index, bool state);
    void inputKeyboardControl(InputType type, int deviceIndex, int index, bool state);
    void inputKeyboardShift(InputType type, int deviceIndex, int index, bool state);
    void inputKeyboardC(InputType type, int deviceIndex, int index, bool state);
    void inputKeyboardV(InputType type, int deviceIndex, int index, bool state);
    void inputMouseMove(InputType type, int deviceIndex, int index, float state);
    void inputMouseButtonLeft(InputType type, int deviceIndex, int index, bool state);

    void onProcess(float delta, float zoom) override;

    void updateOutput();
    void updateSelection();
    void makeString(UINode *container, int strNumber);
    void enableRepeatKey(bool state, int index);
    void relativeToCursor(float rx, float ry, bool moveSelection = true);
    void copy();
    void paste();
    void throwEvent(UIPointerEvent ev);

    float cursorBlink = 0.0f;

    float keysRepeatTime[256];
    bool keysRepeatPressed[256];
    void (UINodeInput::*repeatCallbacks[256])(InputType, int, int, bool);

    int lineHeight = 18;

    UINode *textNode = nullptr;
    UINode *selectionNode = nullptr;
    UINode *mouseNode = nullptr;

    bool selectionInProcess = false;

    Color selectionColor = Color(0.4f, 0.4f, 0.8f, 1.0f);
    float zoom = 1.0f;

    float selectStartPointX = 0.0f;
    float selectStartPointY = 0.0f;
    float mouseX = 0.0f;
    float mouseY = 0.0f;

    bool bControlModificator = false;
    bool bShiftModificator = false;

    bool bMultilineEnabled = false;
    bool bSuperFocus = false;
    bool bIsFocused = false;
    bool bNeedsUpdate = false;

    static UINodeInput *focusedText;
};
