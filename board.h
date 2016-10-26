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

#include <QEventLoop>

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
    void boardToDefault();

    void handleBoardClick(TileMap *tile);
    void handleUnitClick(TileUnit * tile);

    TileUnit * unitOnXY(int x, int y);
    TileMap *mapOnXY(int x, int y);
    Tile *tileOnXY(int x, int y);

    QVector<TileMap *> boardMap;
    QVector<TileUnit *> boardUnits;
    QVector<QPixmap *> pixmaps;

    int columns;
    int rows;
    int state = 0;
    int stateTmp = 0; //uklada stav ke kteremu se vraci, napr po DM modu
    int onTurn = 0;     //index aktualni jednotku na tahu

    void endOfRound();
    bool displayRange(int x, int y, double rangeRemain, bool checkBlockingUnits);
    int tilesInRange = 0;

    bool canGoOnTile(TileMap * tile);
    bool canGoOnTile(int x, int y);

    void setState(int value);

    TileMap * getMapTile();
    TileMap * lastClickMap = NULL;
    TileUnit * lastClickUnit = NULL;

    QEventLoop* waitOnMapClick;
    QEventLoop* waitOnUnitClick;

public slots:
    void getNewUnit();
    void getDMMode(bool val);
    void getMapFileName(QString filename);
    void getStartCombat();
    void getEndTurn();
    void getMove();

signals:
    //void sendBoard(QVector<int> board);
    void sendPixmapItem(QGraphicsPixmapItem * item);
    void sendStateChange(int state);
};

class TileType
{
public:
    int  imgFile;
    int  imgFile1;
    int  imgFile2;
    QString colour;
    bool blocking;
    double speedCost;
    double speedCostDiagonal;
};

#endif // BOARD_H
