#pragma once
#include "physics/motion.h"
#include "common/utils.h"

class Constraint
{
public:
    EXPORT virtual void processMotion(Motion *motion);
    EXPORT virtual Vector3 processTranslation(Vector3 translation);
};
