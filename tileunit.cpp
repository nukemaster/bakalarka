#include "tileunit.h"

TileUnit::TileUnit(QString unitFileLocation) : spellSlots(10)
{
    QFile file(unitFileLocation);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
    //parsrovani XML souboru
    QDomDocument xml("setting");
    xml.setContent(&file);
    QDomElement e, e1;

    e = xml.firstChildElement("unit");

    QString path = QFileInfo(file).absoluteDir().absolutePath() + "/";

         team               = e.firstChildElement("team").toElement().text().toInt();
         speed              = e.firstChildElement("speed").toElement().text().toInt();
         speedRemain        = speed;
         initiativeMod      = e.firstChildElement("initiative").toElement().text().toInt();
         QString imgNormal  = path + e.firstChildElement("normalPixmap").toElement().text();
         this->addPixmap(new QPixmap(imgNormal));
         setPixmap(0);
         QString imgSelected= path + e.firstChildElement("selectedPixmap").toElement().text();
         this->addPixmap(new QPixmap(imgSelected));
         this->text        = e.firstChildElement("string").text();
             QGraphicsTextItem* text = new QGraphicsTextItem(this->text, this);
             text->setScale(2);
             text->setPos(5, 0);
         hitPointsMax       = e.firstChildElement("hitPointsMax").text().toInt();
         hitPoints          = e.firstChildElement("hitPoints").text().toInt();
         tmpHitPoints       = e.firstChildElement("tmpHitPoints").text().toInt();
         armorClass         = e.firstChildElement("armorClass").text().toInt();
         e1                 = e.firstChildElement("savingThrows");
            strSave = e1.firstChildElement("str").text().toInt();
            conSave = e1.firstChildElement("con").text().toInt();
            dexSave = e1.firstChildElement("dex").text().toInt();
            wisSave = e1.firstChildElement("wis").text().toInt();
            intSave = e1.firstChildElement("int").text().toInt();
            chaSave = e1.firstChildElement("cha").text().toInt();
        e1  = e.firstChildElement("spellSlots");
            spellSlots[spell1] = e1.firstChildElement("l1").text().toInt();
            spellSlots[spell2] = e1.firstChildElement("l2").text().toInt();
            spellSlots[spell3] = e1.firstChildElement("l3").text().toInt();
            spellSlots[spell4] = e1.firstChildElement("l4").text().toInt();
            spellSlots[spell5] = e1.firstChildElement("l5").text().toInt();
            spellSlots[spell6] = e1.firstChildElement("l6").text().toInt();
            spellSlots[spell7] = e1.firstChildElement("l7").text().toInt();
            spellSlots[spell8] = e1.firstChildElement("l8").text().toInt();
            spellSlots[spell9] = e1.firstChildElement("l9").text().toInt();

        QDomNodeList actionList = e.elementsByTagName("attack");
        for (int i = 0; i < actionList.size(); i++) {
            Action * akce = new ActionAttack(actionList.at(i));
            akce->unit = this;
            actions.append(akce);
        }

        actionList = e.elementsByTagName("vsSaveAttack");
        for (int i = 0; i < actionList.size(); i++) {
            Action * akce = new ActionVsSave(actionList.at(i));
            akce->unit = this;
            actions.append(akce);
        }

        actionList = e.elementsByTagName("aoeAction");
        for (int i = 0; i < actionList.size(); i++) {
            Action * akce = new ActionAoE(actionList.at(i));
            akce->unit = this;
            actions.append(akce);
        }

        actionList = e.elementsByTagName("heal");
        for (int i = 0; i < actionList.size(); i++) {
            Action * akce = new ActionHeal(actionList.at(i));
            akce->unit = this;
            actions.append(akce);
        }

        actionList = e.elementsByTagName("extra");
        for (int i = 0; i < actionList.size(); i++) {
            Action * akce = new ActionExtra(actionList.at(i));
            akce->unit = this;
            actions.append(akce);
        }

        attackOfOportunity = new ActionAttack(e.firstChildElement("oportunityAttack"));
        attackOfOportunity->unit = this;

        moovmentAction = new ActionMovement();
        moovmentAction->unit = this;

    file.close();

//    btn_cancel = new QPushButton("Cancel", Action::mainWindow);
//    btn_ok = new QPushButton("OK", Action::mainWindow);
//    btn_cancel->show();
}

TileUnit::~TileUnit()
{
    radialMenus[1]->deleteLater();
    for (int i = 0; i < radialMenus.size(); i++) {
        radialMenus[i]->hide();
    }
}

void TileUnit::setRadialMenuPos()
{
    radialMenu->setPos(this->scenePos().x() + this->sceneBoundingRect().width() / 2,
                       this->scenePos().y() + this->sceneBoundingRect().height() / 2);
}

void TileUnit::setPixmap(int id)
{
    Tile::setPixmap(*(this->pixmaps[id]));
}

void TileUnit::addPixmap(QPixmap *pixmap)
{
    this->pixmaps.append(pixmap);
}

void TileUnit::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    this->board->handleUnitClick(this);
    sendClick();
}

void TileUnit::addHP(int val)
{
    if (val > 0) { //pridavani zivotu
        hitPoints += val;
        hitPoints = (hitPoints > hitPointsMax) ? hitPointsMax : hitPoints;
    }
    else {  //odebirani zivotu
        if (tmpHitPoints > 0) {
            tmpHitPoints += val;
            if (tmpHitPoints < 0) {
                val = tmpHitPoints * -1; //presah z tmp
            }
        }
        hitPoints += val;
        if (hitPoints <= 0) {
            if (rollDeathSaves == false) {
                rollDeathSaves = true;
                reaction = false;
            }
            else {
                deathSaveRoll();
            }

//            zeroHitPoits();
        }
    }
    writeToOwnerInfoBox(text + "> HP change:" + QString::number(val) + "; remain:" + QString::number(hitPoints));
}

int TileUnit::makeD20Roll(int mod, bool advantage, bool disadvantage, lineOfSightType loSType)
{
    QString writeString = "d20 Roll ";
    int roll = (rand() % 19) +1;
    if (advantage && disadvantage) {
        advantage = false;
        disadvantage = false;
    }
    if (advantage) {
        int roll2 = roll = (rand() % 19) +1;
        roll = (roll > roll2) ? roll : roll2;
        writeString += "(ADV)";
    }
    if (disadvantage) {
        int roll2 = roll = (rand() % 19) +1;
        roll = (roll < roll2) ? roll : roll2;
        writeString += "(DISADV)";
    }
    writeString += ": " + QString::number(roll) + " + " + QString::number(mod) + " = " + QString::number(roll+mod);
    writeToOwnerInfoBox(writeString);
    return roll+mod;
}

void TileUnit::zeroHitPoits()
{
    //todo: oznameni, deathSaving throws
qDebug()<<"zeroHP";
    if (isOnTurn == true) {
        if (rollDeathSaves == true) {
            deathSaveRoll();
        }
        callEndTurn();
    }
}

void TileUnit::deathSaveRoll()
{
    qDebug()<<deathSaveFail<<deathSaveSuccess;
    int roll = (rand() % 19) + 1;
    QString str = "deathSavingRoll: " + QString::number(roll) ;
    if (roll >= 10) { //success
        writeToOwnerInfoBox(str + " - success;");
        deathSaveSuccess++;
        if (deathSaveSuccess >= 3) {
            writeToOwnerInfoBox(this->text + " - stabilised");
            rollDeathSaves = false;
            deathSaveFail = 0;
            deathSaveSuccess = 0;
        }
    }
    else { //fail
        deathSaveFail++;
        writeToOwnerInfoBox(str + " - fail;");
        if (deathSaveFail >= 3) {
            writeToOwnerInfoBox(this->text + " - unit dead");
            deathSaveFail = 0;
            deathSaveSuccess = 0;
            this->dead = true;
            this->blocking = false;
            this->hide();
        }
    }
}

int TileUnit::getSpellSlots(spellLevel spellSlo) const
{
    return spellSlots[spellSlo];
}

void TileUnit::setSpellSlots(const spellLevel spellSlo, const int &value)
{
    spellSlots[spellSlo] = value;
    resourcesChange();
}

int TileUnit::getAttackRemains() const
{
    return attackRemains;
}

void TileUnit::setAttackRemains(int value)
{
    attackRemains = value;
    resourcesChange();
}

bool TileUnit::getReaction() const
{
    return reaction;
}

void TileUnit::setReaction(bool value)
{
    reaction = value;
    resourcesChange();
}

bool TileUnit::getBAction() const
{
    return bAction;
}

void TileUnit::setBAction(bool value)
{
    bAction = value;
    resourcesChange();
}

bool TileUnit::getAction() const
{
    return action;
}

void TileUnit::setAction(bool value)
{
    action = value;
    resourcesChange();
}

void TileUnit::createRadialMenus()
{
    QString buttonsStyle = "QPushButton:disabled { background: rgb(100, 0, 0);}";
    radialMenu = new RadialMenu2();

    QPushButton* btn = new QPushButton(""); //konec tahu
    btn->setStyleSheet(btn->styleSheet() + "padding: 0px; border: 0px; background-image: url(:/icons/icons/EndOfTurnScaled.png);");
    QObject::connect(btn, SIGNAL(pressed()), Action::mainWindow, SIGNAL(sendEndTurn())); //todo predelat mainWindow
    radialMenu->addButton(btn);

//    btn = new QPushButton(""); //pohyb
//    btn->setStyleSheet(btn->styleSheet() + "padding: 0px; border: 0px; background-image: url(:/icons/icons/moveIcon.png);");
//    QObject::connect(btn, SIGNAL(pressed()), Action::mainWindow, SLOT(on_pushButtonMove_clicked())); //todo predelat mainWindow
//    radialMenu->addButton(btn);
    btn = new QPushButton(""); //pohyb
    btn->setStyleSheet(btn->styleSheet() + "padding: 0px; border: 0px; background-image: url(:/icons/icons/moveIcon.png);");
    QObject::connect(btn, &QPushButton::pressed, [this](){this->moovmentAction->executeAction();});
    radialMenu->addButton(btn);

    RadialMenu2* akceRM = new RadialMenu2();
    RadialMenu2* bAkceRM = new RadialMenu2();
    RadialMenu2* extraRM = new RadialMenu2();


    for (int i = 0; i < actions.size(); i++) {
        btn = new QPushButton(actions[i]->name);
        btn->setStyleSheet(buttonsStyle);
        QObject::connect(this, &TileUnit::resourcesChange, [btn, i, this]()   {
            if (actions[i]->havePlayerResources()) {
                btn->setDisabled(false);
//                btn->show();
            }
            else {
                btn->setDisabled(true);
//                btn->hide();
            }
//            qDebug()<<actions[i]->havePlayerResources();
        });
        QObject::connect(btn, &QPushButton::pressed, [this, i]()   { this->actions[i]->executeAction(); });
        if (actions[i]->cost.indexOf(enums::action) >= 0) {
            akceRM->addButton(btn);
        }
        else if (actions[i]->cost.indexOf(enums::bonusAction) >= 0) {
            bAkceRM->addButton(btn);
        }
        else {
            extraRM->addButton(btn);
        }
    }

    /*HitPointsMenu*/
    RadialMenu2 * hpMenu = new RadialMenu2();


    btn = new QPushButton("+suma");
    btn->setStyleSheet(btn->styleSheet() + "padding: 0px; border: 0px; background-image: url(:/icons/icons/emptyButton.png);");
    QObject::connect(btn, &QPushButton::pressed, [this]() { this->addHP(getDiceMenuSum()); });
    hpMenu->addButton(btn);

    btn = new QPushButton("+5");
    btn->setStyleSheet(btn->styleSheet() + "padding: 0px; border: 0px; background-image: url(:/icons/icons/emptyButton.png);");
    QObject::connect(btn, &QPushButton::pressed, [this]() { this->addHP(5); });
    hpMenu->addButton(btn);

    btn = new QPushButton("+1");
    btn->setStyleSheet(btn->styleSheet() + "padding: 0px; border: 0px; background-image: url(:/icons/icons/emptyButton.png);");
    QObject::connect(btn, &QPushButton::pressed, [this]() { this->addHP(1); });
    hpMenu->addButton(btn);

    btn = new QPushButton("-1");
    btn->setStyleSheet(btn->styleSheet() + "padding: 0px; border: 0px; background-image: url(:/icons/icons/emptyButton.png);");
    QObject::connect(btn, &QPushButton::pressed, [this]() { this->addHP(-1); });
    hpMenu->addButton(btn);

    btn = new QPushButton("-5");
    btn->setStyleSheet(btn->styleSheet() + "padding: 0px; border: 0px; background-image: url(:/icons/icons/emptyButton.png);");
    QObject::connect(btn, &QPushButton::pressed, [this]() { this->addHP(-5); });
    hpMenu->addButton(btn);

    btn = new QPushButton("-suma");
    btn->setStyleSheet(btn->styleSheet() + "padding: 0px; border: 0px; background-image: url(:/icons/icons/emptyButton.png);");
    QObject::connect(btn, &QPushButton::pressed, [this]() { this->addHP(getDiceMenuSum()); });
    hpMenu->addButton(btn);

    /*pripojeni*/
    btn = new QPushButton("HP");
    btn->setStyleSheet(btn->styleSheet() + "padding: 0px; border: 0px; background-image: url(:/icons/icons/emptyButton.png);");
    radialMenu->addButton(btn);
    hpMenu->conectToButton(btn);

    btn = new QPushButton("Akce");
    btn->setStyleSheet(btn->styleSheet() + "padding: 0px; border: 0px; background-image: url(:/icons/icons/emptyButton.png);");
    radialMenu->addButton(btn);
    akceRM->conectToButton(btn);

    btn = new QPushButton("bAkce");
    btn->setStyleSheet(btn->styleSheet() + "padding: 0px; border: 0px; background-image: url(:/icons/icons/emptyButton.png);");
    radialMenu->addButton(btn);
    bAkceRM->conectToButton(btn);

    btn = new QPushButton("extra");
    btn->setStyleSheet(btn->styleSheet() + "padding: 0px; border: 0px; background-image: url(:/icons/icons/emptyButton.png);");
    radialMenu->addButton(btn);
    extraRM->conectToButton(btn);
    /*!HitPointsMenu*/

    radialMenus.append(akceRM);
    radialMenus.append(bAkceRM);
    radialMenus.append(extraRM);
    radialMenus.append(radialMenu);
    radialMenus.append(hpMenu);

}

void TileUnit::rollInitiative()
{
    int roll = (rand() % 19) + 1;
    this->initiative = roll + this->initiativeMod;
    writeToOwnerInfoBox("initiative roll: " + QString::number(roll) + " + " + QString::number(initiativeMod) + " = " + QString::number(initiative));
}

/**
 * @brief TileUnit::startTurn metoda se vola pokazde kdyz jednotka prijde na tah, zarizuje volani pro deathSaves, zobrazuje menu, meni obrazek jednotky na aktivni, resetuje prostredky (akce,...)...
 */
void TileUnit::startTurn()
{
    qDebug()<<"startTurn";
    isOnTurn = true;

    if (dead == true) {     //jednotka je mrtva nemuze hrat
        callEndTurn();
        return;
    }
    if (hitPoints <= 0 ) {  //jednotka ma 0hp, bud je stabilizovana nebo haze deathSave
        zeroHitPoits();
//        callEndTurn();
        return;
    }
    //pokud mela jednotka 0hp a byla vylecena resetuje se deathSave a dalsi uz nehaze
    deathSaveFail = 0;
    deathSaveSuccess = 0;
    rollDeathSaves = false;

    //reset prostredku
    this->speedRemain = this->speed;
    setAttackRemains( attackPerAction);
    setAction( true);
    setBAction( true);
    setReaction( true);

    scene()->views()[0]->centerOn(this);    //vycentrovani sceny na jednotku
    setPixmap(1);   //nastaveni pixmapu na aktivni
    setRadialMenuPos();    
//    radialMenu->setCanBeShown(true);
    for (int i = 0; i < radialMenus.size(); i++) {
        radialMenus[i]->setCanBeShown(true);
    }
    radialMenu->setCanBeShown2(true);
    setRadialMenuPos();
//    radialMenu->setPos(this->scenePos().x(), this->scenePos().y());
    radialMenu->show1s();
}

/**
 * @brief TileUnit::endTurn metoda se vola pokazde kdyz jednotka ukonci svuj tah; skryva menu, nastavuje obrazek na defautni
 */
void TileUnit::endTurn()
{    
    isOnTurn = false;
    setPixmap(0);
//    radialMenu->setCanBeShown(false);
    for (int i = 0; i < radialMenus.size(); i++) {
        radialMenus[i]->setCanBeShown(false);
    }
}

/**
 * @brief TileUnit::endOfRound vola se po kazdem ukoncenem kole
 */
void TileUnit::endOfRound()
{
//    this->speedRemain = this->speed;
//    setAttackRemains( attackPerAction);
//    setAction( true);
//    setBAction( true);
//    setReaction( true);
}

/**
 * @brief TileUnit::callEndTurn volani ukonceni tahu
 */
void TileUnit::callEndTurn()
{
    board->getEndTurn();
}

/**
 * @brief TileUnit::armorClassCheck - porovna hodnotu proti AC jednotky po pripocneti bonusu z krytu
 * @param val - attack roll utocnika
 * @param losType - line of sight, cil zishava bonusy k AC pokud je v krytu
 * @return true pokud utocnik zasahl, false pokud minul
 */
bool TileUnit::armorClassCheck(int val, lineOfSightType losType)
{
    int ac = armorClass;
    if (losType == enums::threeQuartersCover) {
        ac += 5;
    }
    if (losType == halfCover) {
        ac += 2;
    }
    return (val < armorClass)?false:true; //true - zasah, false - minuti
}

/**
 * @brief TileUnit::savingThrowRoll - vraci vysledek hodu d20 po pricteni vsech modifieru
 * @param type - typ saving rollu (dex, con,...) pricte odpovidajici modifier k hodu
 * @param advantage - true pokud je na roll advantage
 * @param diadvantage - true pokud je na roll disadvantage
 * @param losType - line of sight, cil zishava bonusy k dex savu pokud je v krytu
 * @param extraMod - dalsi modifier
 * @return vraci vysledek hodu d20 po pricteni vsech modifieru
 */
int TileUnit::savingThrowRoll(abilityScoreType type, bool advantage, bool diadvantage, lineOfSightType losType, int extraMod)
{    
    switch (type) {
    case strength:
        extraMod += strSave;
        break;
    case dexterity:
        extraMod += dexSave;
        if (losType == enums::threeQuartersCover) {
            extraMod += 5;
        }
        if (losType == enums::halfCover) {
            extraMod += 2;
        }
        break;
    case constitution:
        extraMod += conSave;
        break;
    case intelect:
        extraMod += intSave;
        break;
    case wisdom:
        extraMod += wisSave;
        break;
    case charisma:
        extraMod += chaSave;
        break;
    default:
        break;
    }

    int roll = makeD20Roll(extraMod, advantage, diadvantage);
    return roll;
}
