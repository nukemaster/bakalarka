#include "board.h"

Board::Board()
{
    Tile::board = this;

    this->setState(0);
}

bool Board::loadMap(QString mapLocation)
{
   QImage image;

   QFile file(mapLocation);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return false;

    QDomDocument xml("setting");
    xml.setContent(&file);
    QDomElement e, e0;

    e0 = xml.firstChildElement("map");

    QString mapImg = QFileInfo(file).absoluteDir().absolutePath() + "/" + e0.firstChildElement("imgFile").toElement().text();

    if (image.load(mapImg) == false)
        return false;

    //promazani dlazdic
    for (int i = 0; i < this->boardMap.size(); i++) {
        delete this->boardMap[i];
    }
    this->boardMap.clear();
    for (int i = 0; i < this->boardUnits.size(); i++) {
        delete this->boardUnits[i];
    }
    this->boardUnits.clear();

    this->columns = image.width();
    this->rows = image.height();
    this->boardMap.resize(rows*columns);

    QVector<TileType *> tileTypes;
    TileType* tmp;
    //nacteni moznych barev/dlazdic z XML
     for (e = e0.firstChildElement("tile") ; ! e.isNull(); e = e.nextSiblingElement("tile")) {
         tmp = new TileType();
         tmp->imgFile = new QPixmap(QFileInfo(file).absoluteDir().absolutePath() + "/" + e.firstChildElement("imgFile").toElement().text());
         this->pixmaps.append(tmp->imgFile);
         tmp->colour = e.firstChildElement("colour").toElement().text();
         tmp->blocking = (e.firstChildElement("blocking").toElement().text().toInt() == 1) ? true : false;
         tmp->speedCost = e.firstChildElement("speedCost").toElement().text().toDouble();
         tmp->speedCostDiagonal = e.firstChildElement("speedCostDiagonal").toElement().text().toDouble();
         tileTypes.append(tmp);
     }

     file.close();

    for(int y = 0; y < this->rows ; y++)
    {
        int yplus = y * (this->columns);
        for(int x = 0; x < this->columns ; x++)
        {
            for (int i = 0; i < tileTypes.size(); i++) {
                if (QColor(image.pixel(x, y)).name() == tileTypes[i]->colour ) {
                    this->boardMap[x+ yplus] = new TileMap(x, y, tileTypes[i]->imgFile);
                    this->boardMap[x+ yplus]->blocking = tileTypes[i]->blocking;
                    this->boardMap[x+ yplus]->speedCost = tileTypes[i]->speedCost;
                    this->boardMap[x+ yplus]->speedCostDiag = tileTypes[i]->speedCostDiagonal;
                    this->sendPixmapItem(this->boardMap[x+ yplus]);
                    break;
                }
            }
        }
    }


    this->setState(-1); //do DM modu

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
    case -2:     //vlozeni nove jednotky, ocekava
        qDebug()<<"click state 1: "<<tile->x<<tile->y;
        if (this->insertUnitFromBuffer(tile->x, tile->y) == true) {
            this->setState(-1);   //navraeni do DM modu
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

void Board::setState(int value)
{
    this->sendStateChange(value);
    state = value;
}

Tile * Board::tileOnXY(int x, int y)
{
    return this->boardMap[x + y * this->rows];
}

void Board::getNewUnit()
{
    this->newUnitBuffer = new TileUnit(new QPixmap(":/Img/unit_blue.png"));
    qDebug()<<"pridani nova jednotky, ocekavano klepnuti na Tile;";
    this->setState(-2);
}

void Board::getDMMode(bool val)
{
    if (val == true) {
        this->statePreDM = this->state;
        this->setState(-1);
    }
    else {
        this->setState(this->statePreDM);
    }
}

void Board::getMapFileName(QString filename)
{
    loadMap(filename);
}

