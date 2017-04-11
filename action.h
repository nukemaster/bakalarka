#ifndef ACTION_H
#define ACTION_H

#include <QDomNode>

#include <dice.h>
//#include <tileunit.h>
#include <mainwindow.h>
#include <QRegExp>
#include "enums.h"

using namespace enums;

class TileUnit;

class MainWindow;

class Action
{
public:
    Action(const QDomNode &node);
    Action();
    virtual void executeAction();
    virtual bool isTargetInRange(TileUnit * target);
    static MainWindow* mainWindow;
    TileUnit *selectTargetUnit();
    TileUnit *selectTargetUnitInRange();
    void displayRangeOnUnit(int ftRange);
    void deleteRangeCircle();
    bool checkTargetIsInRange(TileUnit* target); //true - je v rangeCircle; false - neni v range
    void parseDamage(const QString &damage, int &dCount, diceType &dType, int &mod);

    QString name;
    QVector<ActionCost> cost;    
    spellLevel spellSlotLevel;
    virtual bool havePlayerResources(); //zkontroluje zda jednotka vlastnici akce ma pozadovane prostredky pro provedeni, spellSloty, akce,...
    virtual bool payCost(); //odecte potrebne prostredky pro provedeni akce

    TileUnit* unit; //jednotka ktera vlastni akci

protected:
    lineOfSightType lineOfSightBetweenTwoUnits(TileUnit *target, TileUnit *origin = NULL);
    QGraphicsItem* rangeGraphicItemCircle = NULL;

private:    
    lineOfSightType analyzeLine(QGraphicsLineItem* line, TileUnit *target, TileUnit *origin);

};

class ActionMovement : public Action
{
public:
    ActionMovement();
    void executeAction();
    bool havePlayerResources();

protected:
    QList<TileUnit *> findUnitInReactinRange(int ftSearchRange);
};

class ActionHeal : public Action
{
public:
    ActionHeal(const QDomNode &node);
    void executeAction();
    virtual QList<TileUnit *> getTargets();

private:
    int range;
    Dice* dmgDice;
    int dmgDiceCount;
    int dmgMod;
};

enum extraType{actionSurge};

class ActionExtra : public Action
{
public:
    ActionExtra(const QDomNode &node);
    void executeAction();
    extraType typ;
    int perRest = 0;

    bool havePlayerResources();
    bool payCost();
};

#include "actionattack.h"

#include "actionvssave.h"



#endif // ACTION_H
