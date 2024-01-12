// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "ui/uiNodeInput.h"
#include "rtengine.h"
#include <SDL.h>

UINodeInput::UINodeInput(UINode *parent) : UINode(parent), Pawn(this)
{
    registerTextCallback(&UINodeInput::inputText);
    auto inputKeyboardBackspace = registerButtonCallback(&UINodeInput::inputKeyboardBackspace);
    inputKeyboardBackspace->addKeyboardBinding(KeyboardCodes::BACKSPACE, 1.0f);
    auto inputKeyboardDelete = registerButtonCallback(&UINodeInput::inputKeyboardDelete);
    inputKeyboardDelete->addKeyboardBinding(KeyboardCodes::DELETE, 1.0f);
    auto inputKeyboardReturn = registerButtonCallback(&UINodeInput::inputKeyboardReturn);
    inputKeyboardReturn->addKeyboardBinding(KeyboardCodes::RETURN, 1.0f);
    auto inputKeyboardControl = registerButtonCallback(&UINodeInput::inputKeyboardControl);
    inputKeyboardControl->addKeyboardBinding(KeyboardCodes::LCTRL, 1.0f);
    inputKeyboardControl->addKeyboardBinding(KeyboardCodes::RCTRL, 1.0f);
    auto inputKeyboardShift = registerButtonCallback(&UINodeInput::inputKeyboardShift);
    inputKeyboardShift->addKeyboardBinding(KeyboardCodes::LSHIFT, 1.0f);
    inputKeyboardShift->addKeyboardBinding(KeyboardCodes::RSHIFT, 1.0f);
    auto inputKeyboardC = registerButtonCallback(&UINodeInput::inputKeyboardC);
    inputKeyboardC->addKeyboardBinding(KeyboardCodes::C, 1.0f);
    auto inputKeyboardV = registerButtonCallback(&UINodeInput::inputKeyboardV);
    inputKeyboardV->addKeyboardBinding(KeyboardCodes::V, 1.0f);
    auto inputKeyboardUp = registerButtonCallback(&UINodeInput::inputKeyboardUp);
    inputKeyboardUp->addKeyboardBinding(KeyboardCodes::UP, 1.0f);
    auto inputKeyboardDown = registerButtonCallback(&UINodeInput::inputKeyboardDown);
    inputKeyboardDown->addKeyboardBinding(KeyboardCodes::DOWN, 1.0f);
    auto inputKeyboardLeft = registerButtonCallback(&UINodeInput::inputKeyboardLeft);
    inputKeyboardLeft->addKeyboardBinding(KeyboardCodes::LEFT, 1.0f);
    auto inputKeyboardRight = registerButtonCallback(&UINodeInput::inputKeyboardRight);
    inputKeyboardRight->addKeyboardBinding(KeyboardCodes::RIGHT, 1.0f);
    auto inputKeyboardHome = registerButtonCallback(&UINodeInput::inputKeyboardHome);
    inputKeyboardHome->addKeyboardBinding(KeyboardCodes::HOME, 1.0f);
    auto inputKeyboardEnd = registerButtonCallback(&UINodeInput::inputKeyboardEnd);
    inputKeyboardEnd->addKeyboardBinding(KeyboardCodes::END, 1.0f);
    auto inputMouseMove = registerAxisCallback(&UINodeInput::inputMouseMove);
    inputMouseMove->addMouseMoveBinding(1.0f);
    auto inputMouseButtonLeft = registerButtonCallback(&UINodeInput::inputMouseButtonLeft);
    inputMouseButtonLeft->addMouseButtonBinding(InputTypeMouse::LEFT_BUTTON, 1.0f);

    for (int i = 0; i < 256; i++)
    {
        keysRepeatTime[i] = 0.0f;
        keysRepeatPressed[i] = false;
        repeatCallbacks[i] = nullptr;
    }

    repeatCallbacks[(int)KeyboardCodes::BACKSPACE] = &UINodeInput::inputKeyboardBackspace;
    repeatCallbacks[(int)KeyboardCodes::DELETE] = &UINodeInput::inputKeyboardDelete;
    repeatCallbacks[(int)KeyboardCodes::RETURN] = &UINodeInput::inputKeyboardReturn;
    repeatCallbacks[(int)KeyboardCodes::UP] = &UINodeInput::inputKeyboardUp;
    repeatCallbacks[(int)KeyboardCodes::DOWN] = &UINodeInput::inputKeyboardDown;
    repeatCallbacks[(int)KeyboardCodes::LEFT] = &UINodeInput::inputKeyboardLeft;
    repeatCallbacks[(int)KeyboardCodes::RIGHT] = &UINodeInput::inputKeyboardRight;

    selectionNode = createChildNode();
    selectionNode->style.setPositioning(UIPosition::Absolute);
    selectionNode->style.setLayoutDirection(UILayoutDirection::Vertical);
    selectionNode->style.setHoverCursor(UICursor::IBeam);

    textNode = createChildNode();
    textNode->style.setPositioning(UIPosition::Absolute);
    textNode->style.setLayoutDirection(UILayoutDirection::Vertical);
    textNode->style.setHoverCursor(UICursor::IBeam);
    textNode->style.setPaddingBottomPoints(lineHeight * 4);
    textNode->style.setWidthPercentage(100);

    mouseNode = createChildNode();
    mouseNode->style.setWidthPoints(1);
    mouseNode->style.setPositioning(UIPosition::Absolute);
    mouseNode->style.setBackgroundColor(style.getTextColor());
    mouseNode->style.setVisibility(UIVisibility::Hidden);
    mouseNode->style.setHoverCursor(UICursor::IBeam);

    textNode->setUserCallbackData(this);
    textNode->setPointerCallback([](PointerCallbackData *data) -> bool
                                 {
                    UINodeInput *userData = reinterpret_cast<UINodeInput *>(data->userData);
                    if (data->event == UIPointerEvent::CursorClickLeftPress){
                        float relativeX = data->relativeX;
                        float relativeY = data->relativeY;
                        userData->setSelectPointStart(data->globalX - data->relativeX, data->globalY - data->relativeY);
                        userData->setCursorPosition(relativeX, relativeY);
                        userData->startSelection();
                    }
                    return true; });

    style.setOverflowVisibility(UIVisibility::Hidden);
    style.setWidthPercentage(100);
    style.setHeightPercentage(100);
    style.setScroll(UIScroll::Enabled);

    updateOutput();
}

void UINodeInput::setCursorPosition(float relativeX, float relativeY)
{
    relativeToCursor(relativeX, relativeY);
    text.clearSelection();
    updateSelection();
}

void UINodeInput::setCursorSelection(float relativeX, float relativeY)
{
    if (selectionInProcess)
    {
        relativeToCursor(relativeX, relativeY, false);
        updateSelection();
    }
}

void UINodeInput::startSelection()
{
    selectionInProcess = true;
}

void UINodeInput::setSelectPointStart(float x, float y)
{
    selectStartPointX = x;
    selectStartPointY = y;
}

void UINodeInput::inputText(std::string str)
{
    text.removeSelected();
    text.putAtCursor(str);
    updateSelection();
    updateOutput();
}

void UINodeInput::inputKeyboardBackspace(InputType type, int deviceIndex, int index, bool state)
{
    if (state)
    {
        text.removeBack();
        cursorBlink = 0.0f;
        updateOutput();
        updateSelection();
    }
    enableRepeatKey(state, (int)KeyboardCodes::BACKSPACE);
}

void UINodeInput::inputKeyboardDelete(InputType type, int deviceIndex, int index, bool state)
{
    if (state)
    {
        text.removeForward();
        cursorBlink = 0.0f;
        updateOutput();
        updateSelection();
    }
    enableRepeatKey(state, (int)KeyboardCodes::DELETE);
}

void UINodeInput::inputKeyboardReturn(InputType type, int deviceIndex, int index, bool state)
{
    if (state)
    {
        text.removeSelected();
        std::string *str = text.getCurrentString();
        std::string saveStr = str->substr(text.getCursor().positionH, str->length());
        *str = str->substr(0, text.getCursor().positionH);
        text.addString(text.getCursor().positionV + 1, saveStr);
        text.setCursorPosition(text.getCursor().positionV + 1, 0);
        cursorBlink = 0.0f;
        updateOutput();
        updateSelection();
    }
    enableRepeatKey(state, (int)KeyboardCodes::RETURN);
}

void UINodeInput::inputKeyboardHome(InputType type, int deviceIndex, int index, bool state)
{
    if (state)
    {
        setCursorPosition(text.getCursor().positionV, 0);
        cursorBlink = 0.0f;
    }
}

void UINodeInput::inputKeyboardEnd(InputType type, int deviceIndex, int index, bool state)
{
    if (state)
    {
        std::string *str = text.getCurrentString();
        setCursorPosition(text.getCursor().positionV, str->length());
        cursorBlink = 0.0f;
    }
}

void UINodeInput::inputKeyboardUp(InputType type, int deviceIndex, int index, bool state)
{
    if (state)
        text.moveCursor(0, -1);
    updateSelection();
    enableRepeatKey(state, (int)KeyboardCodes::UP);
}

void UINodeInput::inputKeyboardDown(InputType type, int deviceIndex, int index, bool state)
{
    if (state)
        text.moveCursor(0, 1);
    updateSelection();
    enableRepeatKey(state, (int)KeyboardCodes::DOWN);
}

void UINodeInput::inputKeyboardLeft(InputType type, int deviceIndex, int index, bool state)
{
    if (state)
        text.moveCursor(-1, 0);
    updateSelection();
    enableRepeatKey(state, (int)KeyboardCodes::LEFT);
}

void UINodeInput::inputKeyboardRight(InputType type, int deviceIndex, int index, bool state)
{
    if (state)
        text.moveCursor(1, 0);
    updateSelection();
    enableRepeatKey(state, (int)KeyboardCodes::RIGHT);
}

void UINodeInput::inputKeyboardControl(InputType type, int deviceIndex, int index, bool state)
{
    bControlModificator = state;
}

void UINodeInput::inputKeyboardShift(InputType type, int deviceIndex, int index, bool state)
{
    bShiftModificator = state;
}

void UINodeInput::inputKeyboardC(InputType type, int deviceIndex, int index, bool state)
{
    if (bControlModificator && state)
        copy();
}

void UINodeInput::inputKeyboardV(InputType type, int deviceIndex, int index, bool state)
{
    if (bControlModificator && state)
        paste();
}

void UINodeInput::inputMouseMove(InputType type, int deviceIndex, int index, float state)
{
    if (index == (int)InputTypeMouseMove::MOVE_HORIZONTAL)
        mouseX = state;
    if (index == (int)InputTypeMouseMove::MOVE_VERTICAL)
        mouseY = state;

    if (selectionInProcess && (index == (int)InputTypeMouseMove::MOVE_HORIZONTAL || index == (int)InputTypeMouseMove::MOVE_VERTICAL))
    {
        float relativeX = mouseX / zoom - selectStartPointX;
        float relativeY = mouseY / zoom - selectStartPointY;
        setCursorSelection(relativeX, relativeY);
    }
}

void UINodeInput::inputMouseButtonLeft(InputType type, int deviceIndex, int index, bool state)
{
    if (!state)
        selectionInProcess = false;
}

void UINodeInput::onProcess(float delta, float zoom)
{
    this->zoom = zoom;
    if (style.getFontSize() + 2 != lineHeight)
    {
        lineHeight = style.getFontSize() + 2;
        updateOutput();
        updateSelection();
    }

    cursorBlink += delta * 1.1f;
    if (cursorBlink >= 1.0f)
        cursorBlink -= 1.0f;

    mouseNode->style.setHeightPoints(style.getFontSize());

    std::string *str = text.getCurrentString();
    std::string cut = str->substr(0, text.getCursor().positionH);

    if (mouseNode)
    {
        mouseNode->style.setVisibility(cursorBlink < 0.5f ? UIVisibility::Visible : UIVisibility::Hidden);
        mouseNode->style.setBackgroundColor(style.getTextColor());
        mouseNode->style.setMarginTopPoints(lineHeight * text.getCursor().positionV);
        mouseNode->style.setMarginLeftPoints(UIChierarchyElement::getTextWidth(style.getFontSize() * zoom, str->substr(0, text.getCursor().positionH)) / zoom + 1.0f);
    }

    // keys repeat for control keys cause SDL provide repeat in usual way only for text input
    for (int i = 0; i < 256; i++)
    {
        if (keysRepeatPressed[i])
        {
            keysRepeatTime[i] += delta;
            if (keysRepeatTime[i] > 0.25f && repeatCallbacks[i])
            {
                keysRepeatTime[i] = 0.21f;
                (this->*(repeatCallbacks[i]))(InputType::KEYBOARD, 0, i, true);
            }
        }
    }
}

void UINodeInput::updateOutput()
{
    text.updateCursorBounts();
    textNode->clearChildren();

    for (int i = 0; i < text.getStringsAmount(); i++)
    {
        auto stringContainer = textNode->createChildNode();
        stringContainer->style.setHeightPoints(lineHeight);
        makeString(stringContainer, i);
    }
}

void UINodeInput::updateSelection()
{
    selectionNode->clearChildren();
    int start, end, inLineStart, inLineEnd;
    text.getSelection(&start, &end, &inLineStart, &inLineEnd);

    for (int i = start; i <= end; i++)
    {
        auto selectionSubNode = selectionNode->createChildNode();
        selectionSubNode->style.setBackgroundColor(selectionColor);
        selectionSubNode->style.setHeightPoints(lineHeight);
        selectionSubNode->style.setPositioning(UIPosition::Absolute);
        selectionSubNode->style.setMarginTopPoints(i * lineHeight);

        if (i == start || i == end)
        {
            std::string *str = text.getStringByNumber(i);
            int symStart = 0;
            int symEnd = 0;

            if (start == end)
            {
                symStart = inLineStart;
                symEnd = inLineEnd;
            }
            else
            {
                if (i == start)
                {
                    symStart = inLineStart;
                    symEnd = str->length();
                }
                if (i == end)
                {
                    symStart = 0;
                    symEnd = inLineEnd;
                }
            }

            float shift = UIChierarchyElement::getTextWidth(style.getFontSize() * zoom, str->substr(0, symStart)) / zoom;
            float width = UIChierarchyElement::getTextWidth(style.getFontSize() * zoom, str->substr(symStart, symEnd - symStart)) / zoom;

            selectionSubNode->style.setMarginLeftPoints(shift);
            selectionSubNode->style.setWidthPoints(width);
        }
        else
        {
            std::string *str = text.getStringByNumber(i);
            float width = UIChierarchyElement::getTextWidth(style.getFontSize() * zoom, *str) / zoom;
            selectionSubNode->style.setWidthPoints(width);
        }
    }
}

void UINodeInput::makeString(UINode *container, int strNumber)
{
    std::string *str = text.getStringByNumber(strNumber);
    if (str->length() > 0)
    {
        auto string = container->createChildNode();
        string->setText(*str);
        string->style.setTextColor(style.getTextColor());
        string->style.setHeightPoints(lineHeight);
        string->style.setTextVerticalAlign(UITextAlign::Middle);
        string->style.setFontSize(style.getFontSize());
    }
}

void UINodeInput::enableRepeatKey(bool state, int index)
{
    if (index > 0 && index < 256 && keysRepeatPressed[index] != state)
    {
        keysRepeatPressed[index] = state;
        keysRepeatTime[index] = 0.0f;
        for (int i = 0; i < 256; i++)
        {
            if (index != i)
                keysRepeatPressed[i] = false;
        }
    }
}

void UINodeInput::relativeToCursor(float rx, float ry, bool moveSelection)
{
    int lineNum = min(max(static_cast<int>(ry / lineHeight), 0), text.getStringsAmount() - 1);
    text.setCursorPosition(lineNum, 0, moveSelection);

    std::string *str = text.getStringByNumber(lineNum);
    float closest = 9999.0f;
    for (int i = 0; i <= str->length(); i++)
    {
        std::string substr = str->substr(0, i);
        float width = static_cast<float>(UIChierarchyElement::getTextWidth(style.getFontSize() * zoom, substr)) / zoom;
        float dist = fabsf(width - rx);
        if (dist < closest)
        {
            text.setCursorPosition(lineNum, i, moveSelection);
            closest = dist;
        }
    }
}

void UINodeInput::copy()
{
    std::string str = text.getSelectionAsString();
    SDL_SetClipboardText(str.c_str());
}

void UINodeInput::paste()
{
    char *str = SDL_GetClipboardText();

    text.removeSelected();
    text.putAtCursor(std::string(str));
    updateSelection();
    updateOutput();
}