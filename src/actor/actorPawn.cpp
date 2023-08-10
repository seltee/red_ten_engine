// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "actor/actorPawn.h"

ActorPawn::ActorPawn(void *selfPointer) : Pawn(selfPointer)
{
    registerClassName("ActorPawn");
}
