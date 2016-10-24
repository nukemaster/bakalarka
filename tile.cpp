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
    this->pixmaps.append(pixmap);
}

void TileUnit::setPixmap(int id)
{
    Tile::setPixmap(*(this->pixmaps[id]));
}

void TileUnit::addPixmap(QPixmap *pixmap)
{
    this->pixmaps.append(pixmap);
}

void TileUnit::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    event->accept();
    //QGraphicsPixmapItem::mousePressEvent(event);
    this->board->handleUnitClick(this);
}

void TileUnit::rollInitiative()
{
    this->initiative = rand();
}

void TileUnit::showRange(int range)
{

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

void TileMap::setPixmap(QPixmap pixmap)
{
    Tile::setPixmap(pixmap);
}
