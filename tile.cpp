#include "tile.h"



Tile::Tile(int x, int y, QPixmap * pixmap)
{
    this->x = x;
    this->y = y;
    //this->pixmap = pixmap;
    this->setPixmap(*pixmap); //nastaveni aktualni pixmapy
    this->pixmaps.append(pixmap);
    this->setPos(x * TILESIZE, y * TILESIZE);
}

void Tile::addPixmap(QPixmap * pixmap)
{
    this->pixmaps.append(pixmap);
}
