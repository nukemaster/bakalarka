#include "board.h"

Board::Board()
{
    Tile::board = this;

    this->waitOnMapClick = new QEventLoop();
    this->waitOnUnitClick = new QEventLoop();

    this->setState(0);
    this->loadDefaultPixmaps();

    srand (time(NULL)); //initialize random seed
}

Board::~Board()
{
    waitOnMapClick->quit();
    waitOnUnitClick->quit();

    delete waitOnMapClick;
    delete waitOnUnitClick;
    for (int i = 0; i < this->boardMap.size(); i++) {
        delete this->boardMap[i];
    }
    this->boardMap.clear();
    for (int i = 0; i < this->boardUnits.size(); i++) {
        delete this->boardUnits[i];
    }
}

/**
 * @brief Board::createEmptyMap vytvoří novou mapu z "empty" TileType
 * @param width
 * @param height
 * @return
 */
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
            this->boardMap[x+ yplus] = new TileMap(x, y, tileTypes["empty"]);
            this->sendPixmapItem(this->boardMap[x+ yplus]);
            QObject::connect(this->boardMap[x+ yplus], SIGNAL(sendClick()), this->waitOnMapClick, SLOT(quit()) );
        }
    }

    this->setState(-1);

    return true;
}

/**
 * @brief Board::loadMap načtení mapy a TileTypes z XML
 * @param mapLocation cesta k XML souboru
 * @return
 */
bool Board::loadMap(QString mapLocation)
{
    QImage image;

    QFile file(mapLocation);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return false;
    //parsrovani XML souboru a tvorba TileTypes
    QDomDocument xml("setting");
    xml.setContent(&file);
    QDomElement e, e0;

    e0 = xml.firstChildElement("map");

    QString mapImg = QFileInfo(file).absoluteDir().absolutePath() + "/" + e0.attribute("file");

    QString path = QFileInfo(file).absoluteDir().absolutePath() + "/";
    //nacteni moznych barev/dlazdic z XML
    for (e = e0.firstChildElement("tile") ; ! e.isNull(); e = e.nextSiblingElement("tile")) {
         QString name           = e.firstChildElement("pixmap").toElement().attribute("name");
         QString imgPath        = path + e.firstChildElement("pixmap").toElement().text();
         bool blocking          = (e.firstChildElement("blocking").toElement().text().toInt() == 1) ? true : false;
         bool difficultTerrain  = (e.firstChildElement("difficultTerrain").toElement().text().toInt() == 1) ? true : false;
         QString color          = e.attribute("color");

         this->tileTypes[name] = new TileType(imgPath, blocking, difficultTerrain, color);
    }

    file.close();

    //vygenerovani tlacitek v menu editace mapy
    QMap<QString, TileType *>::const_iterator i = tileTypes.begin();
    QVector<QString> typeNames;
    while (i != tileTypes.constEnd()) {
        if (i.key() != "range") //vynechani nevhodnych pixmap
            typeNames.append( i.key());
        ++i;
    }
    sendTileTypesNames(typeNames);

    //načtení mapy z obrázku
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

    for(int y = 0; y < this->rows ; y++)
    {
        int yplus = y * (this->columns);
        for(int x = 0; x < this->columns ; x++)
        {
            QMap<QString, TileType *>::const_iterator i = tileTypes.constBegin();
            while (i != tileTypes.constEnd()) {
                if (QColor(image.pixel(x, y)).name() == i.value()->color ) {
                    this->boardMap[x+ yplus] = new TileMap(x, y, i.value());
                    this->boardMap[x+ yplus]->color = i.value()->color;
                    this->sendPixmapItem(this->boardMap[x+ yplus]);
                    QObject::connect(this->boardMap[x+ yplus], SIGNAL(sendClick()), this->waitOnMapClick, SLOT(quit()) );
                    break;
                }
                ++i;
            }
        }
    }
    this->setState(-1); //do DM modu
saveMap();
    return true;
}

void Board::saveMap(QString mapName)
{
    QFile file("maps/" + mapName + ".xml");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;

    QImage map(columns, rows, QImage::Format_ARGB32);
    for (int y = 0; y < this->rows; y++) {
        for (int x = 0; x < this->columns; x++) {
            map.setPixelColor(x, y, QColor(dynamic_cast<TileMap*>(this->tileOnXY(x, y))->color));
            qDebug()<<x<<y<<this->tileOnXY(x, y)->x<<this->tileOnXY(x, y)->y<<QColor(dynamic_cast<TileMap*>(this->tileOnXY(x, y))->color);
        }
    }

    if (!QDir("maps/" + mapName).exists()) {
        QDir().mkdir("maps/" + mapName);
    }
    QFile f("maps/" + mapName + "/" + mapName + ".png");
    f.open(QIODevice::WriteOnly);
    map.save(&f);

    QXmlStreamWriter stream(&file);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();

    stream.writeStartElement("map");
        stream.writeAttribute("file", mapName + "/" + mapName + ".png");
        stream.writeTextElement("preview", mapName + "/" + mapName + ".png");
        for(QMap<QString, TileType *>::iterator i=tileTypes.begin(); i!=tileTypes.end(); ++i) {
            if (i.key() == "range")
                continue;
            stream.writeStartElement("tile");
                stream.writeAttribute("color", i.value()->color);
                stream.writeStartElement("pixmap");
                    stream.writeAttribute("name", i.key());
                    QFile f("maps/" + mapName + "/" + i.key() + ".png");
                    f.open(QIODevice::WriteOnly);
//                    QPixmap pixmap(i.value()->pixmap);
                    i.value()->pixmap->save(&f);
                    stream.writeCharacters(mapName + "/" + i.key() + ".png");
                stream.writeEndElement();
                stream.writeTextElement("blocking", (i.value()->blocking)?"1":"0");
                stream.writeTextElement("difficultTerrain", (i.value()->difficultTerrain)?"1":"0");
            stream.writeEndElement();
        }
    stream.writeEndElement();

    stream.writeEndDocument();
    file.close();

}

void Board::boardToDefault()
{
    this->tilesInRange = 0;
    for (int i = 0; i < boardMap.size(); i++) {
        boardMap[i]->refreshPixmap();
        boardMap[i]->maxRangeRemain = -1;
    }
    for (int i = 0; i < this->tmp.size(); i++) {
        delete tmp[i];
    }
    tmp.resize(0);
}

int Board::roll(int dNumber, int dType)
{
    int ret = 0;
    for (int i = 0; i < dNumber; i++) {
        ret += (rand() % dType) + 1;
    }
    return  ret;
}

void Board::handleUnitClick(TileUnit *tile)
{
    this->lastClickUnit = tile;
    waitOnUnitClick->exit();
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
    return this->boardMap[x + y * this->columns];
}

void Board::loadDefaultPixmaps()
{
    tileTypes["empty"]  = new TileType(DEFAULT_EMPTY_TILE_IMG, false, false);
    tileTypes["wall"]   = new TileType(DEFAULT_WALL_TILE_IMG, true, false);
    tileTypes["range"]  = new TileType(DEFAULT_RANGE_INDICATOR, false, false);

    //vygenerovani tlacitek v menu editace mapy
    QMap<QString, TileType *>::const_iterator i = tileTypes.begin();
    QVector<QString> typeNames;
    while (i != tileTypes.constEnd()) {
        if (i.key() != "range") //vynechani nevhodnych pixmap
            typeNames.append( i.key());
        ++i;
    }
    sendTileTypesNames(typeNames);
}

void Board::endOfRound()
{
    for (int i = 0; i < this->boardUnits.size(); i++) {
        this->boardUnits[i]->endOfRound();
    }
}

/**
 * @brief Board::displayRange2 pokus o detekci range pomocí kolize QGraphicsItem, nefunkcni
 * @param x
 * @param y
 * @param range
 * @return
 */
bool Board::displayRange2(int x, int y, double range)
{
    int tmp = (((range*2)/5)+1)*TILESIZE;
    int offset = TILESIZE/2 - tmp/2;
    QGraphicsEllipseItem * circle = new QGraphicsEllipseItem(x*TILESIZE + offset, y*TILESIZE+ offset, tmp, tmp);
    QBrush b = QBrush(Qt::red, Qt::SolidPattern);
    //b.setStyle(Qt::SolidPattern);
    //b.setColor(QColor(0,255,0,150));
    //circle->setBrush(b);
    sendRangeItem(circle);

    QList<QGraphicsItem * > items = circle->collidingItems();
    TileMap * tile;
    for (int i = 0; i < items.size(); ++i) {
        if (tile = dynamic_cast<TileMap *>(items[i])) {
            //tile->showPixmap(tileTypes["wall"]->pixmap);

        }
    }
    //circle->setPos(0, -300);
    return true;
}

bool Board::displayRange(int x, int y, double rangeRemain, bool checkBlockingUnits = false)
{   
    int x1, y1;

    TileMap * tile = mapOnXY(x, y);


    if (tile->maxRangeRemain < rangeRemain) {
        if (tile->maxRangeRemain < 0) {
            if (checkBlockingUnits) {
                if (this->tilesInRange != 0) { //neprovede se u pocatecni dlazdice
                    TileUnit * tmpUnit;
                    if ((tmpUnit = this->unitOnXY(x, y)) != NULL) {//pokud je na dlazdici jednotka
                        if (tmpUnit->blocking)
                            return false;
                    }
                }
            }
            tilesInRange++;
            TileMap * tileRange = new TileMap(tile->x, tile->y, "range");
            QObject::connect(tileRange, SIGNAL(sendClick()), this->waitOnMapClick, SLOT(quit()) );
            sendPixmapItem(tileRange);
            tileRange->maxRangeRemain = rangeRemain;
            tmp.append(tileRange);
        }
        tile->maxRangeRemain = rangeRemain;
    }
    else
        return false;



    x1 = x+1;
    y1 = y+0;
    tile = mapOnXY(x1, y1);
    if ((tile != NULL)&&(tile->blocking != true)&&(tile->getSpeedCost(false) <= rangeRemain))
        displayRange(x1, y1, rangeRemain - tile->getSpeedCost(false), true);
    x1 = x-1;
    y1 = y+0;
    tile = mapOnXY(x1, y1);
    if ((tile != NULL)&&(tile->blocking != true)&&(tile->getSpeedCost(false) <= rangeRemain))
        displayRange(x1, y1, rangeRemain - tile->getSpeedCost(false), true);
    x1 = x+0;
    y1 = y+1;
    tile = mapOnXY(x1, y1);
    if ((tile != NULL)&&(tile->blocking != true)&&(tile->getSpeedCost(false) <= rangeRemain))
        displayRange(x1, y1, rangeRemain - tile->getSpeedCost(false), true);
    x1 = x+0;
    y1 = y-1;
    tile = mapOnXY(x1, y1);
    if ((tile != NULL)&&(tile->blocking != true)&&(tile->getSpeedCost(false) <= rangeRemain))
        displayRange(x1, y1, rangeRemain - tile->getSpeedCost(false), true);


    x1 = x+1;
    y1 = y+1;
    tile = mapOnXY(x1, y1);
    if ((tile != NULL)&&(tile->blocking != true)&&(tile->getSpeedCost(true) <= rangeRemain))
        displayRange(x1, y1, rangeRemain - tile->getSpeedCost(true), true);
    x1 = x-1;
    y1 = y-1;
    tile = mapOnXY(x1, y1);
    if ((tile != NULL)&&(tile->blocking != true)&&(tile->getSpeedCost(true) <= rangeRemain))
        displayRange(x1, y1, rangeRemain - tile->getSpeedCost(true), true);
    x1 = x-1;
    y1 = y+1;
    tile = mapOnXY(x1, y1);
    if ((tile != NULL)&&(tile->blocking != true)&&(tile->getSpeedCost(true) <= rangeRemain))
        displayRange(x1, y1, rangeRemain - tile->getSpeedCost(true), true);
    x1 = x+1;
    y1 = y-1;
    tile = mapOnXY(x1, y1);
    if ((tile != NULL)&&(tile->blocking != true)&&(tile->getSpeedCost(true) <= rangeRemain))
        displayRange(x1, y1, rangeRemain - tile->getSpeedCost(true), true);

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
        qDebug()<<"err> lastClickMap == NULL";
    TileMap * tmp = lastClickMap;
    lastClickMap = NULL;
    this->setState(this->stateTmp);
    return tmp;
}

TileUnit *Board::getUnitTile()
{
    this->stateTmp = state;
    this->setState(-2);
    this->waitOnUnitClick->exec(); //cekani nez nekdo klikne na tile
    if (lastClickUnit == NULL)
        qDebug()<<"err> lastClickUnit == NULL";
    TileUnit * tmp = lastClickUnit;
    lastClickUnit = NULL;
    this->setState(this->stateTmp);
    return tmp;
}

void Board::getNewUnit(QString unitLocation)
{
    int tmpState = state;
    this->setState(-2);
    TileMap* mapTile = NULL;
    while (!canGoOnTile(mapTile)) {
        mapTile = this->getMapTile();  //zadani na ktere pole se bude vkladat
    }

    TileUnit *newUnitBuffer = new TileUnit(unitLocation);
    boardUnits.append(newUnitBuffer);
    sendPixmapItem(newUnitBuffer);
    newUnitBuffer->setPos(mapTile->x, mapTile->y);

    this->setState(tmpState); //nastaveni stavu hraci plochy pro vlozeni nove jednotky
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

    this->boardUnits.first()->startTurn();

    setState(1);
}

void Board::getEndTurn()
{
    qDebug()<<"Board::getEndTurn";
    this->boardToDefault();
    this->boardUnits[this->onTurn]->endTurn();
    this->onTurn++;
    if (this->onTurn >= this->boardUnits.size()) {
        this->endOfRound();
        this->onTurn = 0;
    }
    this->boardUnits[this->onTurn]->startTurn();
}

void Board::getMove()
{
    this->boardToDefault(); //pro jistotu
    TileUnit * unit = this->boardUnits[this->onTurn];

    //this->displayRange2(unit->x, unit->y, unit->speedRemain);
    this->displayRange(unit->x, unit->y, unit->speedRemain, true);
    //this->displayRange2(unit->x, unit->y, unit->speedRemain);

    if (this->tilesInRange == 1) {  //pokud neni v rangi vice nez 1 jednotka se nemuze nikam pohnout -> zbytecne ocekavat vztup
        this->boardToDefault();
        return;
    }
    TileMap * map;
    while ((map = this->getMapTile())->maxRangeRemain < 0.0) {    }
    unit->setPos(map->x, map->y);
    unit->speedRemain = mapOnXY(map->x, map->y)->maxRangeRemain;
    unit->setRadialMenuPos();
    this->boardToDefault(); //vycisteni hraci plochy
}

void Board::getNewEmptyMap(int x, int y)
{
    createEmptyMap(x, y);
}



//TileType::TileType(QString pixmapPath, bool blocking, bool difficultTerrain, QString color)
//{
//    this->pixmap = new QPixmap(pixmapPath);
//    this->blocking = blocking;
//    this->difficultTerrain = difficultTerrain;
//    this->color = color;
//}
