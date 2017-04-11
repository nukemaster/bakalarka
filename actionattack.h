#ifndef ACTIONATTACK_H
#define ACTIONATTACK_H

#include "action.h"

class ActionAttack: public Action
{

public:
    ActionAttack(const QDomNode &node);
    void executeAction();
    void attackTarget(TileUnit* target, bool disadvantage = false); //utok bez vyberu cile, pouzito pro attack of oportunity
    bool isTargetInRange(TileUnit * target); //pouzito pro attack of oportunity


private:
    int range;
    int maxRange;
    int attackMod;
    Dice* dmgDice;
    int dmgDiceCount;
    int dmgMod;
    enum attackType{ranged, melee};
    const static std::map<QString, attackType> strToAttackTypeMap;
    attackType translateStrToAttackType(QString index);
    attackType type;

    void displayRangeOnUnit2(int ftRange);
    QGraphicsItem* rangeGraphicItemCircle2 = NULL;
    bool checkTargetIsInRange2(TileUnit *target);
};

#endif // ACTIONATTACK_H
