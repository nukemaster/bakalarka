#include "dice.h"

Dice::Dice(diceType typ)
{
    type = typ;
}

int Dice::roll()
{    
    return (rand() % type) +1;
}

int Dice::roll(int numberOfDices)
{
    int ret = 0;
    for (int i = 0; i < numberOfDices; i++) {
        ret += roll();
    }
    return ret;
}
