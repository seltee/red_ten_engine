// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "common/text.h"

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

Text::Text()
{
    strings.push_back("");
    cursor.positionH = 0;
    cursor.positionV = 0;
    selection.positionH = 0;
    selection.positionV = 0;
}

void Text::setCursorPosition(int strNum, int charPos, bool moveSelection)
{
    cursor.positionV = min(max(strNum, 0), strings.size() - 1);
    std::string *str = getStringByNumber(cursor.positionV);
    cursor.positionH = min(max(charPos, 0), str->length());
    if (moveSelection)
    {
        selection.positionH = cursor.positionH;
        selection.positionV = cursor.positionV;
    }
}

void Text::putAtCursor(std::string strToPut)
{
    std::string *str = getStringByNumber(cursor.positionV);
    if (str)
    {
        std::string start, end;
        if (str->length() > 0)
        {
            start = str->substr(0, cursor.positionH);
            end = str->substr(cursor.positionH, str->length());
        }

        *str = start;
        std::string rest = strToPut;
        while (true)
        {
            auto p = rest.find("\n");
            if (p < rest.size())
            {
                *str += rest.substr(0, p);
                printf("i %s\n", rest.substr(0, p).c_str());
                cursor.positionV++;
                addString(cursor.positionV, "");
                str = getStringByNumber(cursor.positionV);
                rest = rest.substr(p + 1, rest.length());
            }
            else
            {
                *str += rest;
                break;
            }
        }
        *str += end;

        cursor.positionH += strToPut.length();
        setCursorPosition(cursor.positionV, cursor.positionH);
    }
}

void Text::clearSelection()
{
    selection.positionH = cursor.positionH;
    selection.positionV = cursor.positionV;
}

bool Text::isSelected()
{
    return !(cursor.positionH == selection.positionH && cursor.positionV == selection.positionV);
}

void Text::removeSelected()
{
    if (!isSelected())
        return;

    int start, end, inLineStart, inLineEnd;
    getSelection(&start, &end, &inLineStart, &inLineEnd);

    bool multiline = start != end;

    for (int i = start; i <= end; i++)
    {
        if (i == start || i == end)
        {
            std::string *str = getStringByNumber(i);
            int symStart = 0;
            int symEnd = 0;

            if (start == end)
            {
                symStart = cursor.positionH;
                symEnd = selection.positionH;
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
            if (symStart > symEnd)
            {
                int b = symStart;
                symStart = symEnd;
                symEnd = b;
            }
            *str = str->substr(0, symStart) + str->substr(symEnd, str->length());
        }
        else
        {
            strings.erase(strings.begin() + i);
            i--;
            end--;
        }
    }
    setCursorPosition(start, inLineStart);
    if (multiline)
        removeForward();
}

void Text::moveCursor(int x, int y)
{
    cursor.positionH += x;
    if (cursor.positionH < 0 && x < 0)
    {
        if (cursor.positionV > 0)
        {
            cursor.positionV = max(cursor.positionV - 1, 0);
            std::string *str = getStringByNumber(cursor.positionV);
            cursor.positionH = str->length();
        }
        else
        {
            cursor.positionH = 0;
        }
    }

    std::string *str = getStringByNumber(cursor.positionV);
    if (cursor.positionH > str->length() && x > 0)
    {
        if (cursor.positionV < strings.size() - 1)
        {
            cursor.positionH = 0;
            cursor.positionV++;
        }
        else
        {
            cursor.positionH = str->length();
        }
    }

    cursor.positionV += y;
    if (cursor.positionV < 0 && y < 0)
    {
        cursor.positionV = 0;
        cursor.positionH = 0;
    }

    if (cursor.positionV >= strings.size() && y > 0)
    {
        cursor.positionV = strings.size() - 1;
        std::string *str = getStringByNumber(cursor.positionV);
        cursor.positionH = str->length();
    }

    setCursorPosition(cursor.positionV, cursor.positionH);
}

void Text::removeBack()
{
    if (isSelected())
        removeSelected();
    else
    {
        std::string *str = getStringByNumber(cursor.positionV);
        if (cursor.positionH > 0)
        {
            *str = str->substr(0, cursor.positionH - 1) + str->substr(cursor.positionH, str->length());
            cursor.positionH--;
        }
        else
        {
            if (cursor.positionV > 0)
            {
                std::string saveStr = *str;
                strings.erase(strings.begin() + cursor.positionV);
                cursor.positionV--;
                str = getStringByNumber(cursor.positionV);
                setCursorPosition(cursor.positionV, str->length());
                *str += saveStr;
            }
        }
        setCursorPosition(cursor.positionV, cursor.positionH);
    }
}

void Text::removeForward()
{
    if (isSelected())
        removeSelected();
    else
    {
        std::string *str = getStringByNumber(cursor.positionV);
        if (cursor.positionH < str->length())
        {
            *str = str->substr(0, cursor.positionH) + str->substr(cursor.positionH + 1, str->length());
        }
        else
        {
            if (cursor.positionV < strings.size() - 1)
            {
                std::string saveStr = *getStringByNumber(cursor.positionV + 1);
                strings.erase(strings.begin() + cursor.positionV + 1);
                *str += saveStr;
            }
        }
        setCursorPosition(cursor.positionV, cursor.positionH);
    }
}

void Text::addString(int afterStrNum, std::string str)
{
    strings.insert(strings.begin() + afterStrNum, str);
}

std::string *Text::getStringByNumber(int number)
{
    if (number < 0)
        number = 0;
    if (number >= strings.size())
        number = strings.size() - 1;
    return &strings.at(number);
}

std::string *Text::getCurrentString()
{
    return getStringByNumber(cursor.positionV);
}

void Text::updateCursorBounts()
{
    bool bIsSelected = isSelected();

    if (cursor.positionV < 0)
        cursor.positionV = 0;
    if (cursor.positionV >= strings.size())
        cursor.positionV = strings.size() - 1;

    std::string *str = getCurrentString();
    if (cursor.positionH > str->length())
        cursor.positionH = str->length();
    if (cursor.positionH < 0)
        cursor.positionH = 0;

    if (!bIsSelected)
    {
        selection.positionH = cursor.positionH;
        selection.positionV = cursor.positionV;
    }
}

void Text::getSelection(int *strStart, int *strEnd, int *symStart, int *symEnd)
{
    int start = cursor.positionV > selection.positionV ? selection.positionV : cursor.positionV;
    int end = cursor.positionV > selection.positionV ? cursor.positionV : selection.positionV;
    int inLineStart = cursor.positionV > selection.positionV ? selection.positionH : cursor.positionH;
    int inLineEnd = cursor.positionV > selection.positionV ? cursor.positionH : selection.positionH;
    if (start == end && inLineStart > inLineEnd)
    {
        int b = inLineStart;
        inLineStart = inLineEnd;
        inLineEnd = b;
    }
    *strStart = start;
    *strEnd = end;
    *symStart = inLineStart;
    *symEnd = inLineEnd;
}

std::string Text::getSelectionAsString()
{
    int strStart, strEnd, symStart, symEnd;
    getSelection(&strStart, &strEnd, &symStart, &symEnd);
    std::string out;
    if (!isSelected())
        return out;

    for (int i = strStart; i <= strEnd; i++)
    {
        if (i == strStart || i == strEnd)
        {
            std::string *str = getStringByNumber(i);
            int takeStart = 0;
            int takeEnd = 0;

            if (strStart == strEnd)
            {
                takeStart = symStart;
                takeEnd = symEnd;
            }
            else
            {
                if (i == strStart)
                {
                    takeStart = symStart;
                    takeEnd = str->length();
                }
                if (i == strEnd)
                {
                    takeStart = 0;
                    takeEnd = symEnd;
                }
            }

            out += str->substr(takeStart, takeEnd - takeStart);
        }
        else
        {
            out += strings.at(i);
        }

        if (i != strEnd)
            out += "\n";
    }

    return out;
}