#ifndef TILE_H
#define TILE_H

#include <QGraphicsPixmapItem>

#define TILESIZE 50

class Tile  : public QGraphicsPixmapItem
{
public:
    Tile(int x, int y, QPixmap *pixmap);
    int x;
    int y;
    bool blocking = true;

    QVector<QPixmap *> pixmaps; //seznam moznych pixmap, 0 - defaultni

    void addPixmap(QPixmap *pixmap);
};

#endif // TILE_H
