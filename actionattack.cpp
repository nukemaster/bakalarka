#include "actionattack.h"

const std::map<QString, ActionAttack::attackType> ActionAttack::strToAttackTypeMap = {
    {"melee", ActionAttack::melee},
    {"ranged", ActionAttack::ranged},
 };

ActionAttack::ActionAttack(const QDomNode &node) : Action(node)
{
    type = translateStrToAttackType(node.firstChildElement("type").text());
    range = node.firstChildElement("range").text().toInt();
    maxRange = (node.firstChildElement("maxRange").isNull()) ? -1 : node.firstChildElement("maxRange").text().toInt();
//    qDebug()<<name<<maxRange<<node.firstChildElement("maxRange").text().toInt();
    attackMod = node.firstChildElement("attackModifier").text().toInt();
    QString damageStr = node.firstChildElement("damage").text();
    diceType damageDiceType;
    parseDamage(damageStr, dmgDiceCount, damageDiceType, dmgMod);
    dmgDice = new Dice(damageDiceType);
}

void ActionAttack::executeAction()
{
    bool disAdvantage = false;

    if (!havePlayerResources()) {
        unit->writeToOwnerInfoBox("You cant do this.");
        return;
    }
    displayRangeOnUnit(range);
    if (maxRange != -1)
        displayRangeOnUnit2(maxRange);

    TileUnit* target = NULL;
    do {
        target = selectTargetUnitInRange();
        if (target == NULL) {
            deleteRangeCircle();
            return;
        }
        if (checkTargetIsInRange(target)) {
            break;
        }
        if ((maxRange != -1) && checkTargetIsInRange2(target)) {
            disAdvantage = true;
            break;
        }
        qDebug()<<(maxRange != -1)<<(!checkTargetIsInRange2(target));
    } while (1/*(!checkTargetIsInRange(target))*//* && ((maxRange != -1) && (!checkTargetIsInRange2(target)))*/);

    deleteRangeCircle();
    if (rangeGraphicItemCircle2 != NULL) {
        delete rangeGraphicItemCircle2;
    }

    attackTarget(target, disAdvantage);

}

void ActionAttack::attackTarget(TileUnit *target, bool disadvantage)
{
//    bool disadvantage = false;
    bool advantage = false;

    if (!havePlayerResources()) {
        unit->writeToOwnerInfoBox("You cant do this.");
        return;
    }

    lineOfSightType los = lineOfSightBetweenTwoUnits(target, unit);
    if (los == noLoS) {
        unit->writeToOwnerInfoBox("No line of sight.");
        return;
    }
    payCost();
    if (type == ranged) {   //jednotka ma disadvantage na ranged utoky pokud 5ft od ni stoji hostile jednotka
        displayRangeOnUnit(5);
        QList<QGraphicsItem *> unitsInMeleeRange = rangeGraphicItemCircle->collidingItems();
        for (int i = 0; i < unitsInMeleeRange.size(); i++) {
            TileUnit* tmpUnit = dynamic_cast<TileUnit*>(unitsInMeleeRange[i]);
            if (tmpUnit != NULL) {
                if (tmpUnit == target) {
                    disadvantage = true;
                    break;
                }
                if ((tmpUnit->team != unit->team)&&(tmpUnit->team != 0)) {
                    disadvantage = true;
                    break;
                }
            }
        }
        deleteRangeCircle();
    }
    int attackRoll = unit->makeD20Roll(attackMod, advantage, disadvantage); // d20 roll
    if ((attackRoll - attackMod) == 1) { //kriticke minuti
        attackRoll = -1;
    }
    if (target->armorClassCheck(attackRoll)) {
        int dmgDiceCount = this->dmgDiceCount;
        if ((attackRoll - attackMod) == 20) { //kriticke minuti
            dmgDiceCount *= 2;
        }
        int dmgRoll = dmgDice->roll(dmgDiceCount) + dmgMod;
        target->addHP((-1) *dmgRoll);
        unit->writeToOwnerInfoBox("hit: " + QString::number(dmgRoll) + " dmg");
    }
    else {
        unit->writeToOwnerInfoBox("miss");
    }
}

bool ActionAttack::isTargetInRange(TileUnit *target)
{
    displayRangeOnUnit(range);
    bool ret = rangeGraphicItemCircle->collidesWithItem(target);
    deleteRangeCircle();
    return ret;
}

ActionAttack::attackType ActionAttack::translateStrToAttackType(QString index)
{
    return (strToAttackTypeMap.count(index))? strToAttackTypeMap.at(index) : melee;
}

void ActionAttack::displayRangeOnUnit2(int ftRange)
{
    int pxRange = ftRange * 10; //1tile = 5ft; 1 tile je 50px; ==> 1ft = 10px
    pxRange += 25;  // pripocitani pole na kterem jednotka stoji

    rangeGraphicItemCircle2 = mainWindow->scene->addEllipse(0,0,pxRange*2,pxRange*2, QPen()/*, brush*/);
    rangeGraphicItemCircle2->setPos( unit->pos().x() + unit->boundingRect().size().width()/2 - pxRange,
                                    unit->pos().y() + unit->boundingRect().size().height()/2- pxRange);

    QRadialGradient grad(rangeGraphicItemCircle2->boundingRect().center(),pxRange);
    grad.setColorAt(0, QColor(255, 0, 0, 20));
    grad.setColorAt(0.8, QColor(255, 0, 0, 80));
    grad.setColorAt(1, QColor(255, 0, 0, 150));
    QBrush brush(grad);
    dynamic_cast<QGraphicsEllipseItem*>(rangeGraphicItemCircle2)->setBrush(brush);
}

bool ActionAttack::checkTargetIsInRange2(TileUnit *target)
{
    if (target == NULL) return true;
    return target->collidesWithItem(rangeGraphicItemCircle2);
}
