#include "pawn.h"
#include "controller/inputController.h"

std::vector<Input<Pawn> *> Pawn::inputs;

Pawn::Pawn(void *selfPointer)
{
    setSelfPointer(selfPointer);
}

Pawn::~Pawn()
{
    removeAllInputs();
}

void Pawn::addInput(Input<Pawn> *input)
{
    inputs.push_back(input);
}

void Pawn::removeAllInputs()
{
    auto it = inputs.begin();
    while (it != inputs.end())
        if ((*it)->getOwner() == selfPointer)
        {
            delete (*it);
            it = inputs.erase(it);
        }
        else
            ++it;
}
