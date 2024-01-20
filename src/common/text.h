// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include <vector>
#include <string>

class Text
{
public:
    struct CursorPosition
    {
        int positionH = 0;
        int positionV = 0;
    };

    EXPORT Text();

    EXPORT void setCursorPosition(int strNum, int charPos, bool moveSelection = true);
    EXPORT void putAtCursor(std::string strToPut);
    EXPORT void clearSelection();
    EXPORT bool isSelected();
    EXPORT void removeSelected();
    EXPORT void moveCursor(int x, int y);
    EXPORT void removeBack();
    EXPORT void removeForward();
    EXPORT void addString(int afterStrNum, std::string str);
    EXPORT std::string *getStringByNumber(int number);
    EXPORT std::string *getCurrentString();
    EXPORT inline int getStringsAmount() { return strings.size(); }
    EXPORT void updateCursorBounds();
    EXPORT void getSelection(int *strStart, int *strEnd, int *symStart, int *symEnd);
    EXPORT std::string getSelectionAsString();
    EXPORT std::string getAllAsString();
    EXPORT void setText(std::string text);

    EXPORT inline CursorPosition getCursor() { return cursor; }
    EXPORT inline CursorPosition getSelection() { return selection; }

protected:
    std::vector<std::string> strings;
    CursorPosition cursor;
    CursorPosition selection;
};