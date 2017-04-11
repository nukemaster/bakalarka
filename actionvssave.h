#ifndef ACTIONVSSAVE_H
#define ACTIONVSSAVE_H

#include "action.h"

class ActionVsSave: public Action
{
public:
    ActionVsSave(const QDomNode &node);
    void executeAction();

protected:
    struct unitLos{
        TileUnit* unit;
        lineOfSightType los;
    };
    virtual QList<unitLos *> getTargets();
    virtual bool nooneInLoS(); //true pokud se ma platit cena i kdyz neni nikdo v LoS

    abilityScoreType savingType;
    int difficulityClass;
    int range;
    int dmgDiceCount;
    int dmgMod;
    Dice* dmgDice;
    damageSaveSuccess saveSuccessDamage;
};

#include "actionaoe.h"

#endif // ACTIONVSSAVE_H
