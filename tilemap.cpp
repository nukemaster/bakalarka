#include "tilemap.h"

const double TileMap::speedCost                       = 5.0;                //cena pro vstup na pole
const double TileMap::speedCostDifficultTerain        = 10.0;               //cena pro vstup na pole
const double TileMap::speedCostDiag                   = 7.0710678;          //cena pro vstup na pole
const double TileMap::speedCostDiagDifficultTerain    = 14.1421356;         //cena pro vstup na pole

TileMap::TileMap(int x, int y, TileType* tileType)
{
    this->setPos(x, y);
    this->setPixmap(*tileType->pixmap);
    this->pixmap = tileType->pixmap;
    this->blocking = tileType->blocking;
    this->difficultTerrain = tileType->difficultTerrain;
}

TileMap::TileMap(int x, int y, QString tileTypeName)
{    
    TileType* tileType = this->board->tileTypes.value(tileTypeName);
    this->setPos(x, y);
    this->setPixmap(*tileType->pixmap);
    this->pixmap = tileType->pixmap;
    this->blocking = tileType->blocking;
    this->difficultTerrain = tileType->difficultTerrain;
}

void TileMap::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug()<<this->x<<this->y<<this->color;
    //event->accept();
    this->sendClick();
    this->board->handleBoardClick(this);
}

void TileMap::refreshPixmap()
{
    Tile::setPixmap(*this->pixmap);
}

void TileMap::changeTo(QString tileTypeName)
{
    TileType * typ = this->board->tileTypes[tileTypeName];
    this->setPixmap(*typ->pixmap);
    this->pixmap = typ->pixmap;
    this->blocking = typ->blocking;
    this->difficultTerrain = typ->difficultTerrain;
    this->color = typ->color;
}

void TileMap::showPixmap(QPixmap *pixmap)
{
    Tile::setPixmap(*pixmap);
}

double TileMap::getSpeedCost(bool diagonal = false)
{
    if (difficultTerrain) {
        return (diagonal) ? this->speedCostDiagDifficultTerain : this->speedCostDifficultTerain;
    }
    else {
        return (diagonal) ? this->speedCostDiag : this->speedCost;
    }
}



TileType::TileType(QString pixmapPath, bool blocking, bool difficultTerrain, QString color)
{
    this->pixmap = new QPixmap(pixmapPath);
    this->blocking = blocking;
    this->difficultTerrain = difficultTerrain;
    this->color = color;
}
