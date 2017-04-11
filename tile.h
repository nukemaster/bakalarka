#ifndef TILE_H
#define TILE_H

class Board;

//#include "board.h"

#include <QObject>
#include <QGraphicsPixmapItem>

#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QGraphicsView>

#include "enums.h"

#define TILESIZE 50


class Tile  :  public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    static Board *board;

    Tile();
    Tile(int x, int y, QPixmap *pixmap);
    Tile(QPixmap *pixmap);
    int x;
    int y;
    bool blocking = true;
    enums::lineOfSightType loSType = enums::noLoS;
    int state = 0;

    //void addPixmap(QPixmap *pixmap);

    //void mousePressEvent(QGraphicsSceneMouseEvent * event);

    ///<overwrite>
    void setPixmap(QPixmap pixmap);
    //void setPixmapI(int index);
    void setPos(int x, int y);
    void setScenePos(QPointF pos);
    ///</overwrite>

    int posShiftX = 0;
    int posShiftY = 0;

signals:
    void sendClick();

};

//class TileMap : public Tile
//{
//public:
//    TileMap(int x, int y, TileType* tileType);
//    TileMap(int x, int y, QString tileTypeName);
//    double getSpeedCost(bool diagonal);

//    void mousePressEvent(QGraphicsSceneMouseEvent * event);

//    bool difficultTerrain;

//    QPixmap * pixmap;
//    void refreshPixmap();
//    void changeTo(QString tileTypeName);
//    void showPixmap(QPixmap * pixmap);

//    double maxRangeRemain = -1.0;   //pomocna promena pri ukazovani range

//protected:
//    static const double speedCost;                    //cena pro vstup na pole
//    static const double speedCostDifficultTerain;     //cena pro vstup na pole
//    static const double speedCostDiag;                //cena pro vstup na pole
//    static const double speedCostDiagDifficultTerain; //cena pro vstup na pole
//};

//enum abilityScoreType {strength, dexterity, constitution, intelect, wisdom, charisma};

//class TileUnit : public Tile
//{
//    Q_OBJECT
//public:
//    TileUnit();
//    TileUnit(QPixmap *pixmap);

//    void setRadialMenuPos();

//    void setPixmap(int id);
//    void addPixmap(QPixmap *pixmap);

//    void mousePressEvent(QGraphicsSceneMouseEvent * event);

//    void rollInitiative();
//    void startTurn(); //
//    void endTurn();
//    void endOfRound();

//    void showRange(int range);

//    QVector<QPixmap *> pixmaps; //seznam moznych pixmap, 0 - defaultni
//    RadialMenu2* radialMenu;

//    QString text;
//    int initiative = 0; // vysledek hodu s bonusy
//    int initiativeMod = 0;
//    int hitPointsMax;
//    int hitPoints;
//    int tmpHitPoints = 0;
//    double speed = 30;
//    double speedRemain = 30;

//    int armorClass = 10;

//    int proficiencyB = +1;
//    //skills modifiers
//    int strSave = 0;
//    int conSave = 0;
//    int dexSave = 0;
//    int intSave = 0;
//    int wisSave = 0;
//    int chaSave = 0;

//    int deathSavingThrowSuccesses = 0;
//    int deathSavingThrowFailures  = 0;

//    bool action = true;
//    bool bAction = true;
//    bool reaction = true;

//public slots:
//    bool savingThrow(abilityScoreType type, int DC);
//    void addHP(int val);
//};

#endif // TILE_H
