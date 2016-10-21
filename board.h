#ifndef BOARD_H
#define BOARD_H

#include <QObject>
#include <QImage>
#include <QVector>
#include <QGraphicsPixmapItem>

#include "tile.h"

#include <QDebug>

class Board : public QObject
{
    Q_OBJECT

public:
    Board();
    bool loadMap(QString mapLocation = QString::QString(":/Demo/map_test3.bmp"));

    QVector<Tile *> board;
    QVector<QPixmap *> pixmaps;

    int columns;
    int rows;

signals:
    //void sendBoard(QVector<int> board);
    void sendPixmapItem(QGraphicsPixmapItem * item);

};

#endif // BOARD_H
