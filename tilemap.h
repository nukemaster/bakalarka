#ifndef TILEMAP_H
#define TILEMAP_H

//class Board;

#include "tile.h"
#include "board.h"

class TileType
{
public:
    TileType(QString pixmapPath, bool blocking, bool difficultTerrain, QString color = NULL);

    QPixmap * pixmap;
    QString color;
    bool blocking;
    bool difficultTerrain;
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

    QString color = "#000000";

protected:
    static const double speedCost;                    //cena pro vstup na pole
    static const double speedCostDifficultTerain;     //cena pro vstup na pole
    static const double speedCostDiag;                //cena pro vstup na pole
    static const double speedCostDiagDifficultTerain; //cena pro vstup na pole
};



#endif // TILEMAP_H
