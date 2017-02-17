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

void TileUnit::setRadialMenuPos()
{
    QGraphicsView *v = scene()->views().first();
    QPointF sceneP = mapToScene( boundingRect().topLeft());
    QPoint viewP =  v->mapFromScene(sceneP);
    radialMenu->setPos(v->viewport()->mapToGlobal(viewP).x() + boundingRect().width()/2,
                             v->viewport()->mapToGlobal(viewP).y() + boundingRect().height()/2);
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
    //event->accept();
    //QGraphicsPixmapItem::mousePressEvent(event);
    this->board->handleUnitClick(this);
}

void TileUnit::rollInitiative()
{
    /* initialize random seed: */


    this->initiative = rand() % 19 + this->initiativeMod;
}

void TileUnit::startTurn()
{
    setPixmap(1);
    setRadialMenuPos();
    radialMenu->show();
}

void TileUnit::endTurn()
{
    setPixmap(0);
    radialMenu->hide();
}

void TileUnit::endOfRound()
{
    this->speedRemain = this->speed;
    action = true;
    bAction = true;
    reaction = true;
}


///////////////////////////////////////
/////////////// TileMap ///////////////
///////////////////////////////////////
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
