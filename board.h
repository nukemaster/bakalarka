#ifndef BOARD_H
#define BOARD_H

#include <QObject>
#include <QImage>
#include <QVector>
#include <QGraphicsPixmapItem>

#include "tile.h"

#include <QDebug>

class Tile;
class TileUnit;
class TileMap;

class Board : public QObject
{
    Q_OBJECT

public:
    Board();
    bool loadMap(QString mapLocation = QString::QString(":/Demo/map_test3.bmp"));
    void handleBoardClick(TileMap *tile);
    void handleUnitClick(TileUnit * tile);
    Tile * unitOnXY(int x, int y);

    QVector<TileMap *> boardMap;
    QVector<TileUnit *> boardUnits;
    TileUnit * newUnitBuffer = NULL; //docasne ulozena jednotka mezi nactenim a vlozenim na plochu
    QVector<QPixmap *> pixmaps;

    int columns;
    int rows;
    int state = 0;


    bool insertUnitFromBuffer(int x, int y);
    Tile *tileOnXY(int x, int y);
public slots:
    void getNewUnit();

signals:
    //void sendBoard(QVector<int> board);
    void sendPixmapItem(QGraphicsPixmapItem * item);


};

#endif // BOARD_H
