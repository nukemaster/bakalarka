#ifndef BOARD_H
#define BOARD_H

#include <QObject>
#include <QImage>
#include <QColor>
#include <QVector>
#include <QGraphicsPixmapItem>

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDomDocument>

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
    bool loadMap(QString mapLocation);
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
    int statePreDM = 0; //uklada stav ke kteremu se vraci, napr po DM modu


    bool insertUnitFromBuffer(int x, int y);
    Tile *tileOnXY(int x, int y);
    void setState(int value);

public slots:
    void getNewUnit();
    void getDMMode(bool val);
    void getMapFileName(QString filename);

signals:
    //void sendBoard(QVector<int> board);
    void sendPixmapItem(QGraphicsPixmapItem * item);
    void sendStateChange(int state);
};

class TileType
{
public:
    QPixmap * imgFile;
    QString colour;
    bool blocking;
    bool speedCost;
    bool speedCostDiagonal;
};

#endif // BOARD_H
