#include "action.h"

Action::Action(const QDomNode &node)
{
    name = node.firstChildElement("name").text();
    spellSlotLevel = (spellLevel) node.firstChildElement("spellSlotLevel").text().toInt();
    QDomNodeList costList = node.toElement().elementsByTagName("cost");
    for (int i = 0; i < costList.size(); i++) {
        QString costString = costList.at(i).toElement().text();
        ActionCost cost;
        if (enums::Maps::strToCostMap.count(costString) != 0) {
            cost = enums::Maps::strToCostMap.at(costString);
        }
        this->cost.append(cost);
    }
}

Action::Action()
{

}

void Action::executeAction()
{

}

bool Action::isTargetInRange(TileUnit *target)
{
    return false;
}

TileUnit *Action::selectTargetUnit(/*bool &cancel*/)
{
    int stateTmp = mainWindow->state;
    mainWindow->getBoardState(-2);

    QTimer * cancelTimer = new QTimer();
    QMetaObject::Connection timer_connection = QObject::connect(mainWindow, &MainWindow::playerOnBoard, [this, cancelTimer](int playerID){
        if (playerID == this->unit->ownerID)
            cancelTimer->start(1000);
    });
    QMetaObject::Connection cancel_connection = QObject::connect(cancelTimer, &QTimer::timeout, [this](){
        mainWindow->waitOnUnitClick->exit();
    });

    mainWindow->waitOnUnitClick->exec(); //cekani na klik na jednotku

    QObject::disconnect(timer_connection);
    QObject::disconnect(cancel_connection);
    delete cancelTimer;

    TileUnit * tmp = mainWindow->tmpUnit;
    mainWindow->tmpUnit = NULL;
    mainWindow->getBoardState(stateTmp);
    return tmp;
//    return mainWindow->selectUnit(unit->ownerID);
}

TileUnit *Action::selectTargetUnitInRange()
{
    TileUnit* target = NULL;
    target = selectTargetUnit();
    return target;
}

void Action::displayRangeOnUnit(int ftRange)
{
    int pxRange = ftRange * 10; //1tile = 5ft; 1 tile je 50px; ==> 1ft = 10px
    pxRange += 25;  // pripocitani pole na kterem jednotka stoji

    rangeGraphicItemCircle = mainWindow->scene->addEllipse(0,0,pxRange*2,pxRange*2, QPen()/*, brush*/);
    rangeGraphicItemCircle->setPos( unit->pos().x() + unit->boundingRect().size().width()/2 - pxRange,
                                    unit->pos().y() + unit->boundingRect().size().height()/2- pxRange);

    QRadialGradient grad(rangeGraphicItemCircle->boundingRect().center(),pxRange);
    grad.setColorAt(0, QColor(255, 0, 0, 20));
    grad.setColorAt(0.8, QColor(255, 0, 0, 80));
    grad.setColorAt(1, QColor(255, 0, 0, 150));
    QBrush brush(grad);
    dynamic_cast<QGraphicsEllipseItem*>(rangeGraphicItemCircle)->setBrush(brush);

}

void Action::deleteRangeCircle()
{
    if (rangeGraphicItemCircle != NULL) {
        delete rangeGraphicItemCircle;
    }
    rangeGraphicItemCircle = NULL;
}

bool Action::checkTargetIsInRange(TileUnit *target)
{
    if (target == NULL) return true;
    return target->collidesWithItem(rangeGraphicItemCircle);
}

void Action::parseDamage(const QString &damage, int &dCount, diceType &dType, int &mod)
{
    QRegExp regExp("(\\d+)(d)(\\d+)([+,-]*)(\\d*)");
    regExp.indexIn(damage);
    dCount = QString(regExp.cap(1)).toInt();
    dType = (diceType) QString(regExp.cap(3)).toInt();
    mod = QString(regExp.cap(4) + regExp.cap(5)).toInt();
}

bool Action::havePlayerResources()
{
    bool actionB = true;
    bool weaponAttackMax = true;
    for (int i = 0; i < cost.size(); i++) {
        switch (cost[i]) {
        case action:
            if (unit->getAction() == false) {
                actionB = false;
            }
            break;
        case bonusAction:
            if (unit->getBAction() == false) {
                return false;
            }
            break;
        case reaction:
            if (unit->getReaction() == false) {
                return false;
            }
            break;
        case spellSlot:
            if (unit->getSpellSlots(spellSlotLevel) <= 0) {
                return false;
            }
            break;
        case weaponAttack:
            if (unit->getAttackRemains() <= 0) {
                return false;
            }
            else {
                if (unit->getAttackRemains() == unit->attackPerAction) {
                    weaponAttackMax = true;
                }
            }
            break;
        default:
            break;
        }
    }
    //umozneni jednotkam s vecim poctem utoku za kolo ignorovat actionCost pri weapon utoku
    if (weaponAttackMax) {
        return actionB;
    }
    else {
        return !actionB;
    }
}

bool Action::payCost()
{
    if (!havePlayerResources()) {
        return false;
    }
    for (int i = 0; i < cost.size(); i++) {
        switch (cost[i]) {
        case action:
            unit->setAction(false);
            break;
        case bonusAction:
            unit->setBAction(false);
            break;
        case reaction:
            unit->setReaction(false);
            break;
        case spellSlot:
            unit->setSpellSlots(spellSlotLevel, unit->getSpellSlots(spellSlotLevel) -1 );
            break;
        case weaponAttack:
            unit->setAttackRemains(unit->getAction() -1);
            break;
        default:
            break;
        }
    }
    return true;
}

lineOfSightType Action::lineOfSightBetweenTwoUnits(TileUnit *target, TileUnit *origin)
{
    QVector<int> result = QVector<int>(4);

    if (origin == NULL) {
        origin = unit;
    }
    QPointF originP(origin->pos().x() + origin->pixmap().width()/2, origin->pos().y() + origin->pixmap().height()/2);
    QGraphicsLineItem* line = mainWindow->scene->addLine(originP.x(), originP.y(), target->pos().x(), target->pos().y());
    result[analyzeLine(line, target, origin)]++;
    delete line;

    line = mainWindow->scene->addLine(originP.x(), originP.y(), target->pos().x() + target->pixmap().width(), target->pos().y());
    result[analyzeLine(line, target, origin)]++;
    delete line;

    line = mainWindow->scene->addLine(originP.x(), originP.y(), target->pos().x(), target->pos().y() + target->pixmap().height());
    result[analyzeLine(line, target, origin)]++;
    delete line;

    line = mainWindow->scene->addLine(originP.x(), originP.y(), target->pos().x() + target->pixmap().width(), target->pos().y() + target->pixmap().height());
    result[analyzeLine(line, target, origin)]++;
    delete line;


    if (result[0] == 4) {
        return clearLoS;
    }
    if (result[0] >= 2) {
        return halfCover;
    }
    if (result[0] >= 1) {
        return threeQuartersCover;
    }
    if (result[0] == 0) {
        return noLoS;
    }

}

/**
 * @brief Action::analyzeLine
 * @todo ne kazda blocking tileMap musi mit vlastnost noLoS (dira v zemi kam nejde vstoupit ale jde pres ni strilet)
 * @param line
 * @param target
 * @param origin
 * @return enums::lineOfSightType
 */
lineOfSightType Action::analyzeLine(QGraphicsLineItem *line, TileUnit *target, TileUnit *origin)
{
    QList<QGraphicsItem*> collideItemList = line->collidingItems();
    bool noLoSB = false;
    bool halfCoverB = false;
    bool threeQuartersCoverB = false;
    for (int i = 0; i < collideItemList.size(); i++) {
        TileMap* tileTmp = dynamic_cast<TileMap*>(collideItemList[i]);
        if (tileTmp != NULL) {
            if (tileTmp->blocking) {
                noLoSB = true;
                break;  //neni treba dale prohledavat
            }
        }
        TileUnit *unitTmp = dynamic_cast<TileUnit*>(collideItemList[i]);
        if (unitTmp != NULL) {
            if ((unitTmp == target)||(unitTmp == origin))
                continue;
            if ((unitTmp->size == medium)||(unitTmp->size == small)) {
                    halfCoverB = true;
            }
        }
    }
    if (noLoSB) return noLoS;
    if (threeQuartersCoverB) return threeQuartersCover;
    if (halfCoverB) return halfCover;
    return clearLoS;
}


ActionMovement::ActionMovement()
{

}

void ActionMovement::executeAction()
{
    QList<TileUnit*> unitsInReactionRangeBefore = findUnitInReactinRange(15);
    QPointF originPos = unit->pos();
    mainWindow->sendMove();
    QPointF afterPos = unit->pos();
    QList<TileUnit*> unitsInReactionRangeAfter = findUnitInReactinRange(15);

     QList<TileUnit*> unitsWithAoO;
    //najit jednotky ktere maji pravo na AoO, jsou v unitsInReactionRangeBefore a nejsou v unitsInReactionRangeAfter
    for (int i = 0; i < unitsInReactionRangeBefore.size(); i++) {
        if (!unitsInReactionRangeBefore.at(i)->attackOfOportunity->havePlayerResources()) { //pokud nema resource na AoO nemuze ho provest
            continue;
        }
        if (unitsInReactionRangeAfter.indexOf(unitsInReactionRangeBefore.at(i)) < 0) {
            unitsWithAoO.append(unitsInReactionRangeBefore.at(i));
        }
    }

    unit->setScenePos(originPos);   //utok na jednotku se provadi predtim nez se pohla

    for (int i = 0; i < unitsWithAoO.size(); i++) {
        unitsWithAoO.at(i)->writeToOwnerInfoBox("Provest oportunity attack ");
        bool ans = false;
        mainWindow->showSmallBtn(unitsWithAoO.at(i)->ownerID);
        QEventLoop* waitLoop = new QEventLoop();
        QMetaObject::Connection ok_connection = QObject::connect(mainWindow->smallBtn.at(unitsWithAoO.at(i)->ownerID).at(0), &QPushButton::clicked, [waitLoop, &ans](){
            ans = true;
            waitLoop->exit();
        });
        QMetaObject::Connection cancel_connection = QObject::connect(mainWindow->smallBtn.at(unitsWithAoO.at(i)->ownerID).at(1), &QPushButton::clicked, [waitLoop, &ans](){
            ans = false;
            waitLoop->exit();
        });
        mainWindow->showSmallBtn(unitsWithAoO.at(i)->ownerID);

        waitLoop->exec();

        mainWindow->hideSmallBtn();
        delete waitLoop;
        QObject::disconnect(ok_connection);
        QObject::disconnect(cancel_connection);

        if (ans) {
            dynamic_cast<ActionAttack*>(unitsWithAoO.at(i)->attackOfOportunity)->attackTarget(unit);
        }
    }

    unit->setScenePos(afterPos);    //vraceni jednotky na spravnou pozici
}

bool ActionMovement::havePlayerResources()
{
    return (unit->speedRemain > 5) ? true : false;
}

QList<TileUnit *> ActionMovement::findUnitInReactinRange(int ftSearchRange)
{
    QList<TileUnit *> ret;

    displayRangeOnUnit(ftSearchRange);
    QList<QGraphicsItem *> items = rangeGraphicItemCircle->collidingItems();
    for (int i = 0; i < items.size(); i++) {
        TileUnit * unitInRange = dynamic_cast <TileUnit *>(items.at(i));
        if (unitInRange == NULL) {
            continue;
        }
        if ((unitInRange->team == unit->team) || (unit->team == 0)) {
            continue;
        }
        if ((unitInRange->attackOfOportunity == NULL) || (!unitInRange->attackOfOportunity->isTargetInRange(unit))) {
            continue;
        }
        ret.append(unitInRange);
    }
    deleteRangeCircle();
    return ret;
}

ActionHeal::ActionHeal(const QDomNode &node) : Action(node)
{
    range = node.firstChildElement("range").text().toInt();
    QString damageStr = node.firstChildElement("damage").text();
    diceType damageDiceType;
    parseDamage(damageStr, dmgDiceCount, damageDiceType, dmgMod);
    dmgDice = new Dice(damageDiceType);
}

void ActionHeal::executeAction()
{
    if (!havePlayerResources()) {
        unit->writeToOwnerInfoBox("You cant do this.");
        return;
    }

    QList<TileUnit *> targets = getTargets();

    if (getTargets().empty()) {
        return;
    }

    int healRoll = dmgDice->roll(dmgDiceCount) + dmgMod;

    for (int i = 0; i < targets.size(); i++) {
        targets[i]->addHP(healRoll);
    }

    payCost();
}

QList<TileUnit *> ActionHeal::getTargets()
{
    QList<TileUnit *> ret;
    if (range == 0) {
        ret.append(this->unit);
        return ret;
    }

    displayRangeOnUnit(range);
    TileUnit * target = selectTargetUnitInRange();   //zjiseni cile
    if (target != NULL) {
        if (lineOfSightBetweenTwoUnits(target, unit) == noLoS) { //pokud neni LoS nelze provest
            unit->writeToOwnerInfoBox("No Line of sight");
            return ret;
        }
        else {
            ret.append(target);
        }
    }
    deleteRangeCircle();
    return ret;
}

ActionExtra::ActionExtra(const QDomNode &node) : Action(node)
{
    perRest = node.firstChildElement("count").text().toInt();
    if (name == "Action Surge") {
        typ = actionSurge;
    }
}

void ActionExtra::executeAction()
{
    if (!havePlayerResources()) {
        unit->writeToOwnerInfoBox("You cant do this.");
        return;
    }

    switch (typ) {
    case actionSurge:
        if (unit->getAction() != true) {
//            qDebug()<<"extraExec"<<typ;
            unit->setAttackRemains(unit->attackPerAction);
            unit->setAction(true);
        }
        break;
    default:
        break;
    }

    payCost();
    unit->resourcesChange();
}

bool ActionExtra::havePlayerResources()
{
    switch (typ) {
    case actionSurge:
        if (perRest > 0) return true;
        break;
    default:
        break;
    }
    return false;
}

bool ActionExtra::payCost()
{
    switch (typ) {
    case actionSurge:
        perRest--;
        break;
    default:
        break;
    }
    return true;
}
