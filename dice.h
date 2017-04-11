#ifndef DICE_H
#define DICE_H

#include <random>

enum diceType {d4 = 4, d6 = 6, d8 = 8, d10 = 10, d12 = 12, d20 = 20};

class Dice
{
public:
    Dice(diceType typ);
    diceType type;
    int roll();
    int roll(int numberOfDices);
};

#endif // DICE_H
