#ifndef TILE_H
#define TILE_H

#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include "board.h"

#define TILESIZE 50

class Board;


class Tile  : public QGraphicsPixmapItem
{
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
    void setPos(int x, int y);
    ///</overwrite>

    int posShiftX = 0;
    int posShiftY = 0;
};

class TileMap : public Tile
{
public:
    TileMap();
    TileMap(int x, int y, QPixmap *pixmap);

    void mousePressEvent(QGraphicsSceneMouseEvent * event);

    void setPixmap(QPixmap pixmap);


    int indexPixmap0;
    int indexPixmap1;
    int indexPixmap2;

    double speedCost = 5.0;                //cena pro vstup na pole
    double speedCostDiag = 7.0710678;    //cena pro vstup na pole
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

    void showRange(int range);

    QVector<QPixmap *> pixmaps; //seznam moznych pixmap, 0 - defaultni

    int initiative = 0;
    double speed = 30;
};

#endif // TILE_H
