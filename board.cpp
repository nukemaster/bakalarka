#include "board.h"

Board::Board()
{
    //this->sendBoard(this->board);
    this->pixmaps.append(new QPixmap(":/Img/tile_grid.png"));
    this->pixmaps.append(new QPixmap(":/Img/tile_wall.png"));

}

bool Board::loadMap(QString mapLocation)
{
    QImage image;

    if (image.load(mapLocation) == false)
        return false;

    this->columns = image.width();
    this->rows = image.height();
    this->board.resize(rows*columns);

    for(int y = 0; y < this->rows ; y++)
    {
        int yplus = y * (this->columns);
        for(int x = 0; x < this->columns ; x++)
        {
            if(image.pixel(x,y) == qRgb(255,255,255)) {  //bila
                this->board[x+ yplus] = (new Tile(x, y, this->pixmaps[0]));
                this->board[x+ yplus]->blocking = false;
                //this->board[x + yplus] = 0; //grid
            }
            else if (image.pixel(x,y) == qRgb(0,0,0)) {  //cerna
                this->board[x+ yplus] = (new Tile(x, y, this->pixmaps[1]));
                //this->board[x + yplus] = 1; //wall
            }
            else {
                this->board[x+ yplus] = (new Tile(x, y, this->pixmaps[0]));
                //this->board[x + yplus] = 0; //chyba - stejne jako grid
            }
            this->sendPixmapItem(this->board[x+ yplus]);
        }
    }

    return true;
}

