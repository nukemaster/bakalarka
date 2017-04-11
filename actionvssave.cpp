#include "actionvssave.h"

ActionVsSave::ActionVsSave(const QDomNode &node) : Action(node)
{
    QString savingTypeStr = node.firstChildElement("savingType").text();
    savingType = enums::Maps::translateStrToAbilityScoreType(savingTypeStr);
    difficulityClass = node.firstChildElement("dc").text().toInt();
    range = node.firstChildElement("range").text().toInt();
    spellSlotLevel = (spellLevel) node.firstChildElement("spellLevel").text().toInt();
    //dmg
    QString damageStr = node.firstChildElement("damage").text();
    diceType damageDiceType;
    parseDamage(damageStr, dmgDiceCount, damageDiceType, dmgMod);
    dmgDice = new Dice(damageDiceType);
    saveSuccessDamage = enums::Maps::translateStrToDamageSaveSuccess(node.firstChildElement("damageSaveSuccess").text());
}

void ActionVsSave::executeAction()
{
    bool disadvantage = false;
    bool advantage = false;

    if (!havePlayerResources()) {
        unit->writeToOwnerInfoBox("You cant do this.");
        return;
    }

    QList<unitLos*> targets = getTargets();

    if (targets.empty()) {
        return;
    }

    int damage = dmgDice->roll(dmgDiceCount) + dmgMod; //DMG roll

    int targetsInLoS = 0;
    for (int i = 0; i < targets.size(); i++) {
        lineOfSightType los = targets[i]->los;
        TileUnit* target = targets[i]->unit;

        if (los == noLoS) {
//            unit->writeToOwnerInfoBox("No line of sight.");
            continue;
        } else {
            targetsInLoS++;
        }

        int savingThrow = target->savingThrowRoll(savingType, advantage, disadvantage, los);

        if (savingThrow >= difficulityClass) {  //line of sight
            //success
            target->writeToOwnerInfoBox(QString::number(savingThrow) + " -> successful " + enums::Maps::translateAbilityScoreTypeToStr(savingType) + " save");
            switch (saveSuccessDamage) {
            case none:
                damage = 0;
                break;
            case half:
                damage = damage/2;
            default:
                break;
            }
        }
        else {
            target->writeToOwnerInfoBox(QString::number(savingThrow) + " -> failed " + enums::Maps::translateAbilityScoreTypeToStr(savingType) + " save");
        }
        target->addHP((-1) * damage);
    }

//    if (targetsInLoS != 0) {
            payCost();
//    }



}

QList<ActionVsSave::unitLos*> ActionVsSave::getTargets()
{
    QList<unitLos *> ret;
    unitLos *retNode = new unitLos;
    displayRangeOnUnit(range);
    retNode->unit = selectTargetUnitInRange();   //zjiseni cile
    if (retNode->unit != NULL) {
        retNode->los = lineOfSightBetweenTwoUnits(retNode->unit, unit); //zjisteni lineOfSight
        ret.push_back(retNode);
    }
    deleteRangeCircle();
    return ret;
}

bool ActionVsSave::nooneInLoS()
{
    return true;
}
