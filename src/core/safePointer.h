// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include <list>

template <typename T>
class SafePointer
{
public:
    SafePointer(T *target) { this->target = target; }

    inline void destroy()
    {
        if (target)
            target->removeSafePointer(this);
        delete this;
    }

    inline T *getTarget() { return target; }
    inline void clearTarget() { target = nullptr; }

protected:
    T *target;
};

template <typename T>
class WithSafePointing
{
public:
    ~WithSafePointing()
    {
        for (auto &safePointer : safePointersList)
        {
            safePointer->clearTarget();
        }
    }

    inline SafePointer<T> *createSafePointer(T *target)
    {
        auto safePointer = new SafePointer<T>(target);
        safePointersList.push_back(safePointer);
        return safePointer;
    }

    inline void removeSafePointer(void *pointer)
    {
        for (auto it = safePointersList.begin(); it != safePointersList.end(); it++)
        {
            if (*it == pointer)
            {
                safePointersList.erase(it);
                return;
            }
        }
    }

    std::list<SafePointer<T> *> safePointersList;
};
