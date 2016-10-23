#include "board.h"

Board::Board()
{
    //this->sendBoard(this->board);
    this->pixmaps.append(new QPixmap(":/Img/tile_grid.png"));   //0
    this->pixmaps.append(new QPixmap(":/Img/tile_wall.png"));   //1

    Tile::board = this;
}

bool Board::loadMap(QString mapLocation)
{
    QImage image;

    if (image.load(mapLocation) == false)
        return false;

    this->columns = image.width();
    this->rows = image.height();
    this->boardMap.resize(rows*columns);

    for(int y = 0; y < this->rows ; y++)
    {
        int yplus = y * (this->columns);
        for(int x = 0; x < this->columns ; x++)
        {
            if(image.pixel(x,y) == qRgb(255,255,255)) {  //bila
                this->boardMap[x+ yplus] = new TileMap(x, y, this->pixmaps[0]);
                this->boardMap[x+ yplus]->blocking = false;
            }
            else if (image.pixel(x,y) == qRgb(0,0,0)) {  //cerna
                this->boardMap[x+ yplus] = new TileMap(x, y, this->pixmaps[1]);
            }
            else {
                this->boardMap[x+ yplus] = new TileMap(x, y, this->pixmaps[0]);
                this->boardMap[x+ yplus]->blocking = false;
            }
            this->sendPixmapItem(this->boardMap[x+ yplus]);
        }
    }

    return true;
}

void Board::handleUnitClick(TileUnit *tile)
{

}

void Board::handleBoardClick(TileMap * tile)
{
    switch (this->state) {
    case 0:     //default
        qDebug()<<"click, state 0: "<<tile->x<<tile->y;
        break;
    case 1:     //vlozeni nove jednotky, ocekava
        qDebug()<<"click state 1: "<<tile->x<<tile->y;
        if (this->insertUnitFromBuffer(tile->x, tile->y) == true) {
            this->state = 0;
        }
        break;
    default:
        break;
    }
}

bool Board::insertUnitFromBuffer(int x, int y)
{
    if (this->newUnitBuffer == NULL) {
        return false;
    }
    if (this->unitOnXY(x, y) != NULL) {
        return false;
    }
    if (this->tileOnXY(x, y)->blocking == true) {
        return false;
    }
    this->newUnitBuffer->setPos(x, y);
    this->boardUnits.append(this->newUnitBuffer);
    this->sendPixmapItem(this->newUnitBuffer);
    this->newUnitBuffer = NULL;
    return true;
}

Tile * Board::unitOnXY(int x, int y)
{
    for (int i = 0; i < this->boardUnits.size(); i++) {
        if ((this->boardUnits[i]->x == x) && (this->boardUnits[i]->y) == y) {
            return this->boardUnits[i];
        }
    }
    return NULL;
}

Tile * Board::tileOnXY(int x, int y)
{
    return this->boardMap[x + y * this->rows];
}

void Board::getNewUnit()
{
    this->newUnitBuffer = new TileUnit(new QPixmap(":/Img/unit_blue.png"));
    qDebug()<<"pridani nova jednotky, ocekavano klepnuti na Tile;";
    this->state = 1;
}
