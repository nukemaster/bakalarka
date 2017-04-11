#include "tile.h"

Board * Tile::board = NULL;

Tile::Tile(int x, int y, QPixmap * pixmap)
{
    this->setPixmap(*pixmap); //nastaveni aktualni pixmapy
    this->setPos(x, y);
}

Tile::Tile(QPixmap *pixmap)
{
    this->setPixmap(*pixmap); //nastaveni aktualni pixmapy
}

Tile::Tile()
{

}

void Tile::setPixmap(QPixmap pixmap)
{
    this->posShiftX = (TILESIZE - pixmap.width()) / 2;
    this->posShiftY = (TILESIZE - pixmap.height()) / 2;
    QGraphicsPixmapItem::setPixmap(pixmap);
}
/*
void Tile::setPixmapI(int index)
{
    QPixmap * pix = (*(this->pixmaps))[index];
    setPixmap(*pix);

}
*/
void Tile::setPos(int x, int y)
{
    QGraphicsPixmapItem::setPos(x * TILESIZE + this->posShiftX, y * TILESIZE + this->posShiftY);
    this->x = x;
    this->y = y;
}

void Tile::setScenePos(QPointF pos)
{
    QGraphicsPixmapItem::setPos(pos);
}
