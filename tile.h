#ifndef TILE_H
#define TILE_H

#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QObject>
#include "board.h"

#define TILESIZE 50

class Board;
class TileType;


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
    int state = 0;

    //void addPixmap(QPixmap *pixmap);

    //void mousePressEvent(QGraphicsSceneMouseEvent * event);

    ///<overwrite>
    void setPixmap(QPixmap pixmap);
    //void setPixmapI(int index);
    void setPos(int x, int y);
    ///</overwrite>

    int posShiftX = 0;
    int posShiftY = 0;

signals:
    void sendClick();

};

class TileMap : public Tile
{
public:
    TileMap(int x, int y, TileType* tileType);
    TileMap(int x, int y, QString tileTypeName);
    double getSpeedCost(bool diagonal);

    void mousePressEvent(QGraphicsSceneMouseEvent * event);

    bool difficultTerrain;

    QPixmap * pixmap;
    void refreshPixmap();
    void changeTo(QString tileTypeName);
    void showPixmap(QPixmap * pixmap);

    double maxRangeRemain = -1.0;   //pomocna promena pri ukazovani range

protected:
    static const double speedCost;                    //cena pro vstup na pole
    static const double speedCostDifficultTerain;     //cena pro vstup na pole
    static const double speedCostDiag;                //cena pro vstup na pole
    static const double speedCostDiagDifficultTerain; //cena pro vstup na pole
};

class TileUnit : public Tile
{
public:
    TileUnit();
    TileUnit(QPixmap *pixmap);

    void setPixmap(int id);
    void addPixmap(QPixmap *pixmap);

    void mousePressEvent(QGraphicsSceneMouseEvent * event);

    void rollInitiative();
    void endOfRound();

    void showRange(int range);

    QVector<QPixmap *> pixmaps; //seznam moznych pixmap, 0 - defaultni

    int initiative = 0;
    double speed = 30;
    double speedRemain = 30;
};

#endif // TILE_H
