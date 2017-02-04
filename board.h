#ifndef BOARD_H
#define BOARD_H

#include <QObject>
#include <QImage>
#include <QColor>
#include <QVector>
#include <QMap>
#include <QGraphicsPixmapItem>

#include<QBrush>

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDomDocument>

#include <QEventLoop>

#include <time.h>       /* time */

#include "tile.h"

#include <QDebug>

#define DEFAULT_EMPTY_TILE_IMG ":/Img/Img/tile_grid.png"
#define DEFAULT_RANGE_INDICATOR ":/Img/Img/tile_grid_green.png"
#define DEFAULT_WALL_TILE_IMG ":/Img/Img/tile_wall.png"

class Tile;
class TileUnit;
class TileMap;
class TileType;

class Board : public QObject
{
    Q_OBJECT

public:
    Board();
    ~Board();
    bool loadMap(QString mapLocation);
    bool createEmptyMap(int width, int height);
    void boardToDefault();

    void handleBoardClick(TileMap *tile);
    void handleUnitClick(TileUnit * tile);

    TileUnit * unitOnXY(int x, int y);
    TileMap *mapOnXY(int x, int y);
    Tile *tileOnXY(int x, int y);

    QVector<TileMap *> boardMap;
    QVector<TileMap *> tmp; //pomocne napriklad na zobrazovani range
    QVector<TileUnit *> boardUnits;
    QMap<QString, TileType *> tileTypes;

    void loadDefaultPixmaps();

    int columns;
    int rows;
    int state = 0;
    int stateTmp = 0; //uklada stav ke kteremu se vraci, napr po DM modu
    int onTurn = 0;     //index aktualni jednotku na tahu

    void endOfRound();
    bool displayRange(int x, int y, double rangeRemain, bool checkBlockingUnits);
    bool displayRange2(int x, int y, double range);
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
    void getNewUnit(QString unitLocation);
    void getDMMode(bool val);
    void getMapFileName(QString filename);
    void getStartCombat();
    void getEndTurn();
    void getMove();
    void getNewEmptyMap(int x, int y);

signals:
    //void sendBoard(QVector<int> board);
    void sendPixmapItem(QGraphicsPixmapItem * item);
    void sendRangeItem(QGraphicsItem * item);
    void sendStateChange(int state);
    void sendTileTypesNames(QVector<QString> types);
};

class TileType
{
public:
    TileType(QString pixmapPath, bool blocking, bool difficultTerrain, QString color = NULL);

    QPixmap * pixmap;
    QString color;
    bool blocking;
    bool difficultTerrain;
};

#endif // BOARD_H
