#include "board.h"

Board::Board()
{
    Tile::board = this;

    this->waitOnMapClick = new QEventLoop();
    this->waitOnUnitClick = new QEventLoop();

    this->setState(0);
    this->loadDefaultPixmaps();
}

bool Board::createEmptyMap(int width, int height)
{
    //promazani dlazdic
    for (int i = 0; i < this->boardMap.size(); i++) {
        delete this->boardMap[i];
    }
    this->boardMap.clear();
    for (int i = 0; i < this->boardUnits.size(); i++) {
        delete this->boardUnits[i];
    }
    this->boardUnits.clear();

    boardToDefault();

    this->columns = width;
    this->rows = height;
    this->boardMap.resize(width*height);

    for (int y = 0; y < height; y++) {
        int yplus = y * (width);
        for (int x = 0; x < width; x++) {
            this->boardMap[x+ yplus] = new TileMap(x, y, "empty");
            this->boardMap[x+ yplus]->blocking = false;
            this->boardMap[x+ yplus]->speedCost = 5;
            this->boardMap[x+ yplus]->speedCostDiag = 7.0710678;
            this->sendPixmapItem(this->boardMap[x+ yplus]);
            QObject::connect(this->boardMap[x+ yplus], SIGNAL(sendClick()), this->waitOnMapClick, SLOT(quit()) );
        }
    }

    this->setState(-1);

    return true;
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

    QString mapImg = QFileInfo(file).absoluteDir().absolutePath() + "/" + e0.attribute("file");

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

    QString path = QFileInfo(file).absoluteDir().absolutePath() + "/";
    QVector<TileType *> tileTypes;
    TileType* tmp;
    //nacteni moznych barev/dlazdic z XML
     for (e = e0.firstChildElement("tile") ; ! e.isNull(); e = e.nextSiblingElement("tile")) {
         QString name = e.firstChildElement("pixmap").toElement().attribute("name");
         QString imgPath = path + e.firstChildElement("pixmap").toElement().text();
         this->pixmaps[name] = new QPixmap(imgPath);

         tmp = new TileType();
         tmp->color = e.attribute("color");
         tmp->pixmapIndex = name;
         tmp->blocking = (e.firstChildElement("blocking").toElement().text().toInt() == 1) ? true : false;
         tmp->difficultTerrain = (e.firstChildElement("difficultTerrain").toElement().text().toInt() == 1) ? true : false;
         tileTypes.append(tmp);
     }

     file.close();

    for(int y = 0; y < this->rows ; y++)
    {
        int yplus = y * (this->columns);
        for(int x = 0; x < this->columns ; x++)
        {
            for (int i = 0; i < tileTypes.size(); i++) {
                if (QColor(image.pixel(x, y)).name() == tileTypes[i]->color ) {
                    this->boardMap[x+ yplus] = new TileMap(x, y, tileTypes[i]->pixmapIndex);
                    this->boardMap[x+ yplus]->blocking      = tileTypes[i]->blocking;
                    this->boardMap[x+ yplus]->speedCost     = 5; //tileTypes[i]->speedCost;
                    this->boardMap[x+ yplus]->speedCostDiag = 7;//tileTypes[i]->speedCostDiagonal;
                    this->sendPixmapItem(this->boardMap[x+ yplus]);
                    QObject::connect(this->boardMap[x+ yplus], SIGNAL(sendClick()), this->waitOnMapClick, SLOT(quit()) );
                    break;
                }
            }
        }
    }

    for (int i = 0; i < tileTypes.size(); i++) {
        delete tileTypes[i];
    }

    this->setState(-1); //do DM modu

    return true;
}

void Board::boardToDefault()
{
    this->tilesInRange = 0;
    for (int i = 0; i < boardMap.size(); i++) {
        boardMap[i]->setPixmap(boardMap[i]->pixmapIndex);
        boardMap[i]->maxRangeRemain = -1;
    }
}

void Board::handleUnitClick(TileUnit *tile)
{
    this->lastClickUnit = tile;
}

void Board::handleBoardClick(TileMap * tile)
{
    this->lastClickMap = tile;
}

bool Board::canGoOnTile(TileMap *tile)
{
    if (tile == NULL)
        return false;
    if (tile->blocking == true)
        return false;
    if (unitOnXY(tile->x, tile->y) != NULL)
        return false;
    return true;
}

bool Board::canGoOnTile(int x, int y)
{
    TileMap * tile = mapOnXY(x, y);
    if (tile == NULL)
        return false;
    return this->canGoOnTile(tile);
}

TileUnit * Board::unitOnXY(int x, int y)
{
    for (int i = 0; i < this->boardUnits.size(); i++) {
        if ((this->boardUnits[i]->x == x) && (this->boardUnits[i]->y) == y) {
            return this->boardUnits[i];
        }
    }
    return NULL;
}

TileMap *Board::mapOnXY(int x, int y)
{
    if ((x < 0)||(x >= this->columns))
        return NULL;
    if ((y < 0)||(y >= this->rows))
        return NULL;
    return this->boardMap[x + y * (this->columns)];
}

Tile * Board::tileOnXY(int x, int y)
{
    if ((x < 0)||(x >= columns)||(y < 0)||(y >= rows))
        return NULL;
    return this->boardMap[x + y * this->rows];
}

void Board::loadDefaultPixmaps()
{
    pixmaps["empty"] = new QPixmap(DEFAULT_EMPTY_TILE_IMG);
    pixmaps["wall"] = new QPixmap(DEFAULT_WALL_TILE_IMG);
    pixmaps["range"] = new QPixmap(DEFAULT_RANGE_INDICATOR);
}

void Board::endOfRound()
{
    for (int i = 0; i < this->boardUnits.size(); i++) {
        this->boardUnits[i]->endOfRound();
    }
}

bool Board::displayRange2(int x, int y, double range)
{
    int tmp = (((range*2)/5)+1)*TILESIZE;
    int offset = TILESIZE/2 - tmp/2;
    QGraphicsEllipseItem * circle = new QGraphicsEllipseItem(x*TILESIZE + offset, y*TILESIZE+ offset, tmp, tmp);
    sendRangeItem(circle);
    return true;
}

bool Board::displayRange(int x, int y, double rangeRemain, bool checkBlockingUnits = false)
{   
    int x1, y1;

    TileMap * tile = mapOnXY(x, y);
    if (checkBlockingUnits) {
        if (this->tilesInRange != 0) { //neprovede se u pocatecni dlazdice
            TileUnit * tmpUnit;
            if ((tmpUnit = this->unitOnXY(x, y)) != NULL) {//pokud je na dlazdici jednotka
                if (tmpUnit->blocking)
                    return false;
            }
        }
    }
    if (tile->maxRangeRemain < 0) { //poprve zapocitana v rangi
        tilesInRange++;
    }
    if (tile->maxRangeRemain < rangeRemain)
        tile->maxRangeRemain = rangeRemain;
    tile->showPixmap("range");

    x1 = x+1;
    y1 = y+0;
    tile = mapOnXY(x1, y1);
    if ((tile != NULL)&&(tile->blocking != true)&&(tile->speedCost <= rangeRemain))
        displayRange(x1, y1, rangeRemain - tile->speedCost, true);
    x1 = x-1;
    y1 = y+0;
    tile = mapOnXY(x1, y1);
    if ((tile != NULL)&&(tile->blocking != true)&&(tile->speedCost <= rangeRemain))
        displayRange(x1, y1, rangeRemain - tile->speedCost, true);
    x1 = x+0;
    y1 = y+1;
    tile = mapOnXY(x1, y1);
    if ((tile != NULL)&&(tile->blocking != true)&&(tile->speedCost <= rangeRemain))
        displayRange(x1, y1, rangeRemain - tile->speedCost, true);
    x1 = x+0;
    y1 = y-1;
    tile = mapOnXY(x1, y1);
    if ((tile != NULL)&&(tile->blocking != true)&&(tile->speedCost <= rangeRemain))
        displayRange(x1, y1, rangeRemain - tile->speedCost, true);


    x1 = x+1;
    y1 = y+1;
    tile = mapOnXY(x1, y1);
    if ((tile != NULL)&&(tile->blocking != true)&&(tile->speedCostDiag <= rangeRemain))
        displayRange(x1, y1, rangeRemain - tile->speedCostDiag, true);
    x1 = x-1;
    y1 = y-1;
    tile = mapOnXY(x1, y1);
    if ((tile != NULL)&&(tile->blocking != true)&&(tile->speedCostDiag <= rangeRemain))
        displayRange(x1, y1, rangeRemain - tile->speedCostDiag, true);
    x1 = x-1;
    y1 = y+1;
    tile = mapOnXY(x1, y1);
    if ((tile != NULL)&&(tile->blocking != true)&&(tile->speedCostDiag <= rangeRemain))
        displayRange(x1, y1, rangeRemain - tile->speedCostDiag, true);
    x1 = x+1;
    y1 = y-1;
    tile = mapOnXY(x1, y1);
    if ((tile != NULL)&&(tile->blocking != true)&&(tile->speedCostDiag <= rangeRemain))
        displayRange(x1, y1, rangeRemain - tile->speedCostDiag, true);

    return true;
}

void Board::setState(int value)
{
    this->sendStateChange(value);
    state = value;
}

TileMap *Board::getMapTile()
{
    this->stateTmp = state;
    this->setState(-2);
    this->waitOnMapClick->exec(); //cekani nez nekdo klikne na tile
    if (lastClickMap == NULL)
        qDebug()<<"chybaaaaa";
    TileMap * tmp = lastClickMap;
    lastClickMap = NULL;
    this->setState(this->stateTmp);
    return tmp;
}

void Board::getNewUnit()
{
    this->setState(-2);
    TileMap* mapTile = NULL;
    while (!canGoOnTile(mapTile)) {
        mapTile = this->getMapTile();  //zadani na ktere pole se bude vkladat
    }

    TileUnit *newUnitBuffer = new TileUnit(new QPixmap(":/Img/unit_blue.png"));
    newUnitBuffer->addPixmap(new QPixmap(":/Img/unit_blue_selected.png"));
    newUnitBuffer->setPos(mapTile->x, mapTile->y);
    QObject::connect(newUnitBuffer, SIGNAL(sendClick()), this, SLOT(this->waitOnUnitClick->quit()) );
    this->boardUnits.append(newUnitBuffer);
    this->sendPixmapItem(newUnitBuffer);

    this->setState(-1); //nastaveni stavu hraci plochy pro vlozeni nove jednotky
}

void Board::getDMMode(bool val)
{
    if (val == true) {
        this->stateTmp = this->state;
        this->setState(-1);
    }
    else {
        this->setState(this->stateTmp);
    }
}

void Board::getMapFileName(QString filename)
{
    loadMap(filename);
}

void Board::getStartCombat()
{
    if (boardUnits.empty())
        return;

    for (int i = 0; i < this->boardUnits.size(); i++) {
        this->boardUnits[i]->rollInitiative();
    }

    //serazeni v Board::boardUnits sestupne podle initiative
    std::sort(this->boardUnits.begin(), this->boardUnits.end() ,
                [](const TileUnit* a, const TileUnit* b)
                {
                    return a->initiative > b->initiative;
                }
        );

    this->boardUnits.first()->setPixmap(1);

    setState(1);
}

void Board::getEndTurn()
{
    this->boardToDefault();
    this->boardUnits[this->onTurn]->setPixmap(0);
    this->onTurn++;
    if (this->onTurn >= this->boardUnits.size()) {
        this->endOfRound();
        this->onTurn = 0;
    }
    this->boardUnits[this->onTurn]->setPixmap(1);
}

void Board::getMove()
{
    this->boardToDefault(); //pro jistotu
    TileUnit * unit = this->boardUnits[this->onTurn];
    this->displayRange(unit->x, unit->y, unit->speedRemain, true);
    if (this->tilesInRange == 1) {  //pokud neni v rangi vice nez 1 jednotka se nemuze nikam pohnout -> zbytecne ocekavat vztup
        this->boardToDefault();
        return;
    }
    TileMap * map;
    while ((map = this->getMapTile())->maxRangeRemain < 0.0) {    }
    unit->setPos(map->x, map->y);
    unit->speedRemain = map->maxRangeRemain;
    this->boardToDefault(); //vycisteni hraci plochy
}

void Board::getNewEmptyMap(int x, int y)
{
    createEmptyMap(x, y);
}


