#include "tile.h"

Board * Tile::board = NULL;

Tile::Tile(int x, int y, QPixmap * pixmap)
{
    this->setPixmap(*pixmap); //nastaveni aktualni pixmapy
    this->pixmaps.append(pixmap);
    this->setPos(x, y);
}

Tile::Tile(QPixmap *pixmap)
{
    this->setPixmap(*pixmap); //nastaveni aktualni pixmapy
    this->pixmaps.append(pixmap);
}

Tile::Tile()
{

}

void Tile::addPixmap(QPixmap * pixmap)
{
    this->pixmaps.append(pixmap);
}
/*
void Tile::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    //qDebug()<<this->test;
    event->accept();
    //QGraphicsPixmapItem::mousePressEvent(event);
    this->board->handleClick(this);
}*/

void Tile::setPixmap(QPixmap pixmap)
{
    this->pixmaps.append(&pixmap);
    this->posShiftX = (TILESIZE - pixmap.width()) / 2;
    this->posShiftY = (TILESIZE - pixmap.height()) / 2;
    QGraphicsPixmapItem::setPixmap(pixmap);
}

void Tile::setPos(int x, int y)
{
    QGraphicsPixmapItem::setPos(x * TILESIZE + this->posShiftX, y * TILESIZE + this->posShiftY);
    this->x = x;
    this->y = y;
}


///////////////////////////////////////
////////////// TileUnit ///////////////
///////////////////////////////////////

TileUnit::TileUnit()
    : Tile()
{

}

TileUnit::TileUnit(QPixmap *pixmap)
     : Tile(pixmap)
{

}

void TileUnit::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    event->accept();
    //QGraphicsPixmapItem::mousePressEvent(event);
    this->board->handleUnitClick(this);
}

///////////////////////////////////////
/////////////// TileMap ///////////////
///////////////////////////////////////

TileMap::TileMap()
    : Tile()
{

}

TileMap::TileMap(int x, int y, QPixmap *pixmap)
    : Tile(x, y, pixmap)
{

}

void TileMap::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    event->accept();
    //QGraphicsPixmapItem::mousePressEvent(event);
    this->board->handleBoardClick(this);
}
