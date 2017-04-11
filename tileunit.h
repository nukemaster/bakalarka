#ifndef TILEUNIT_H
#define TILEUNIT_H

#include "tile.h"
#include "board.h"
#include "radialmenu2.h"
#include <QDomDocument>
#include <QFile>
#include <action.h>

#include <enums.h>

class Action;
//class AoEAction;

class TileUnit : public Tile
{
    Q_OBJECT
public:
    TileUnit(QString unitFileLocation);
    ~TileUnit();

    void setRadialMenuPos();

    void setPixmap(int id);
    void addPixmap(QPixmap *pixmap);

    void mousePressEvent(QGraphicsSceneMouseEvent * event);

    void rollInitiative();
    void startTurn(); //
    void endTurn();
    void endOfRound();
    bool isOnTurn = false;

    void showRange(int range);

    QVector<QPixmap *> pixmaps; //seznam moznych pixmap, 0 - defaultni
    RadialMenu2* radialMenu;
    QVector <RadialMenu2*> radialMenus;

    int ownerID; //playerID - id hrace ktery ovlada jednotku
    int team = 1;    //urceni kdo je vuci komu; 0 neutral

    QString text;
    int initiative = 0; // vysledek hodu s bonusy
    int initiativeMod = 0;
    int hitPointsMax;
    int hitPoints;
    int tmpHitPoints = 0;
    double speed = 30;
    double speedRemain = 30;

    enums::unitSize size = enums::medium;

    int armorClass = 10;

    int proficiencyB = +1;
    //skills modifiers
    int strSave = 0;
    int conSave = 0;
    int dexSave = 0;
    int intSave = 0;
    int wisSave = 0;
    int chaSave = 0;

    int attackPerAction = 1;

    int deathSavingThrowSuccesses = 0;
    int deathSavingThrowFailures  = 0;

    QVector <Action*> actions;
    Action * attackOfOportunity = NULL;
    Action * moovmentAction = NULL;

    void callEndTurn();
    void createRadialMenus();

    QPushButton* btn_cancel;
    QPushButton* btn_ok;

    bool getAction() const;
    void setAction(bool value);

    bool getBAction() const;
    void setBAction(bool value);

    bool getReaction() const;
    void setReaction(bool value);

    int getAttackRemains() const;
    void setAttackRemains(int value);

    int getSpellSlots(spellLevel spellSlo) const;
    void setSpellSlots(const spellLevel spellSlo,const int &value);

public slots:
    bool armorClassCheck(int val, enums::lineOfSightType losType = enums::noLoS);
    int savingThrowRoll(enums::abilityScoreType type, bool advantage = false, bool diadvantage = false, enums::lineOfSightType losType = enums::noLoS, int extraMod = 0);
    void addHP(int val);
    int makeD20Roll(int mod = 0, bool advantage = false, bool disadvantage = false, enums::lineOfSightType loSType = enums::noLoS);
//    void makeAttack(TileUnit* target);

signals:
    void writeToOwnerInfoBox(QString str);
    int getDiceMenuSum();
    void resourcesChange(); //dava vedet ze se smenity prostredky jednotky (akce, bakce,...)

private:
    void zeroHitPoits();
    bool dead = false;
    void deathSaveRoll();
    bool rollDeathSaves = false;
    int deathSaveSuccess = 0;
    int deathSaveFail = 0;

    bool action = true;
    bool bAction = true;
    bool reaction = true;

    int attackRemains = 1;
    QVector<int> spellSlots;    

};


#endif // TILEUNIT_H
