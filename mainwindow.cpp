#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->ui->statusBar->hide();
    this->ui->mainToolBar->hide();

    this->scene = /*new QGraphicsScene();//*/new MyScene();
    this->ui->graphicsView->setScene(this->scene);

    this->hideAll();
    this->ui->box_startMenu->show();

    this->cursorBlue = new QCursor(QPixmap(":/Img/Img/cursor_blue.png"));
    this->cursorRed =  new QCursor(QPixmap(":/Img/Img/cursor_red.png"));

//    this->setCursor(*cursorBlue);

    this->setAttribute(Qt::WA_AcceptTouchEvents);

    playersRadialMenus = QVector<QVector<RadialMenu2*>>(4);
    playerOKButton = QVector<QPushButton*>(4);
    smallBtn = QVector<QVector<QPushButton*>>(4);
//    playersInfoPanels = QVector<QLabel *>(4);

    QScroller::grabGesture(ui->scrollAreaWidgetContents);
    QScroller::grabGesture(ui->graphicsView);    

    waitOnUnitClick = new QEventLoop();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief MainWindow::hideAll skryje UI prvky - pouziva se pri prechodu do jineho stavu desky aby se skryly prvky ktere uz nemaji byt zobrazeny
 */
void MainWindow::hideAll()
{
    ui->menuDM->hide();
    ui->box_startMenu->hide();
    ui->widgetPlayer->hide();
    ui->box_newEmptyMapSetting->hide();
    ui->box_editMap->hide();
    ui->btn_hideShowEditMap->hide();
    ui->editBtsScrollArea->hide();
    for(int i = 0; i < playerOKButton.size();i++) {
        playerOKButton[i]->hide();
    }
}

/**
 * @brief MainWindow::setFullScreen nastavi mainWindow na fullscreen, nastaví velikosti a pozice některých prvků
 */
void MainWindow::setFullScreen()
{
    this->showFullScreen();
    this->ui->graphicsView->setGeometry(GRAPHICVIEW_BORDER_SIZE, GRAPHICVIEW_BORDER_SIZE,
                                        QApplication::desktop()->screenGeometry().width() - 2* GRAPHICVIEW_BORDER_SIZE,
                                        QApplication::desktop()->screenGeometry().height() - 2* GRAPHICVIEW_BORDER_SIZE); //velikost mapy

    ui->box_startMenu->setGeometry(1920 / 2 - 250/2, 100, 250, ui->box_startMenu->height());
    ui->box_editMap->setGeometry(0, 100, 250, 1080-400);
    ui->editBtsScrollArea->setGeometry(100, 100, 260, 1080-200);
//    ui->box_editMap->setParent(ui->editBtsScrollArea);
    ui->editBtsScrollArea->setWidget(ui->box_editMap);
//    ui->box_newEmptyMapSetting->setGeometry(1920 / 2 - 250/2, 100, 250, ui->box_newEmptyMapSetting->height());

    ui->menuDM->setGeometry(ui->graphicsView->x() , ui->graphicsView->height() + ui->graphicsView->y() + 10, 770, 75);

    styleToFantasyBtn(ui->pushButtonAddUnit, 0, 0);
    styleToFantasyBtn(ui->btn_editMap_menuToggle, 260, 0);
    styleToFantasyBtn(ui->pushButtonStartCombat, 520, 0);
    styleToFantasyBtn(ui->btn_editMap_end);
    styleToFantasyBtn(ui->btn_saveMap);
    styleToFantasyBtn(ui->btn_hideShowEditMap, 100, 1080 - 90);
    styleToFantasyBtn(ui->pushButtonMapLoad);
    styleToFantasyBtn(ui->btn_newMap);
    styleToFantasyBtn(ui->btn_newMap_create);
    styleToFantasyBtn(ui->btn_newMap_back);

    createRollMenus();
}

/**
 * @brief MainWindow::addTextToPlayerInfoBox - prida text do infoBoxu odpovidajiciho hrace, text bude pridan na novou radku
 * @param str   - text ktery bude pridan
 * @param playerID
 */
void MainWindow::addTextToPlayerInfoBox(QString str, int playerID)
{
    playersInfoPanels[playerID]->setText(str + "\n" + playersInfoPanels[playerID]->text());
}

/**
 * @brief MainWindow::displayRadialMenu - zasle signal RadialMenu2::setCanBeShown2(value) do kazdeho radialMenu; pouziva se pro skryvani / zobrazovani radial menu pri ocekavani vztupu (vyber hrace,...)
 * @param show false - skryje radialMenu, true - zobrazi radialMenu
 */
void MainWindow::displayRadialMenu(bool show)
{
    for (int i = 0; i < playersRadialMenus.size(); i++) {
        for (int ii = 0; ii < playersRadialMenus[i].size(); ii++) {
            playersRadialMenus[i][ii]->setCanBeShown2(show);
        }
    }
}

/**
 * @brief MainWindow::showSmallBtn zasle signal show() smalBtn odpovidajiciho hrace
 * @param playerID
 */
void MainWindow::showSmallBtn(int playerID)
{
    for (int i = 0; i < smallBtn.at(playerID).size(); i++) {
        smallBtn.at(playerID).at(i)->show();
    }
}

/**
 * @brief MainWindow::hideSmallBtn skryje vsechny smallBtn
 */
void MainWindow::hideSmallBtn()
{
    for (int i = 0; i < smallBtn.size(); i++) {
        for (int ii = 0; ii < smallBtn.at(i).size(); ii++) {
            smallBtn.at(i).at(ii)->hide();
        }
    }
}

long MainWindow::normalizeMouseCoordinate(int coor, int maxCoor)
{
    return ((65535.0 / maxCoor) * coor);
}

/**
 * @brief MainWindow::coordinatesFromDetection2 - slot kterym aplikace prijma souradnice hracu
 * @param x - souradnice kde nad obrazovkou se nachazi hracova ruka
 * @param y - souradnice kde nad obrazovkou se nachazi hracova ruka
 * @param playerID - identifikator hrace (posmeru hodinovych rucicek zacinajici od 0)
 */
void MainWindow::coordinatesFromDetection2(const int x, const int y, const int playerID)
{
//    cursor().setPos(x, y); //DEBUG - vizualizace
    playerOnBoard(playerID);
    for (int i = 0; i < playersRadialMenus[playerID].size(); i++) {
        if (playersRadialMenus[playerID][i]->isOnCoordinates(QPoint(x, y))) {            
            playersRadialMenus[playerID][i]->show1s();
        }
    }
}

/**
 * @brief MainWindow::getPixmapItem metoda pro pridavani Itemu do sceny, pokud se jedna o tridu TileUnit vytvori se pro tridu i RadialMenu ktere jsou take pridany do sceny
 * @param item
 */
void MainWindow::getPixmapItem(QGraphicsPixmapItem *item)
{
    int player = 0;
    this->scene->addItem(item);

    TileUnit* unit;
    if ((unit = dynamic_cast<TileUnit*>(item))){
        QPoint realP = ui->graphicsView->viewport()->mapToGlobal(ui->graphicsView->mapFromScene(unit->pos())); //globalni pozice jednotky
        player = this->playerIdOnXY(realP.x(), realP.y());  //identifikace hrace
//        player = 2; //DEBUG
        unit->ownerID = player;
        unit->createRadialMenus();

        QObject::connect(unit, &TileUnit::writeToOwnerInfoBox, [this, unit](QString str){this->addTextToPlayerInfoBox(str, unit->ownerID);}); //propojeni jednotky s infoBoxem
        QObject::connect(unit, &TileUnit::getDiceMenuSum, [this, player]()->int{return this->diceMenus[player]->suma->innerLabel->text().toInt();});
        QObject::connect(unit, &TileUnit::sendClick, [this, unit](){this->tmpUnit = unit; this->waitOnUnitClick->exit();});

        for (int i = 0; i < unit->radialMenus.size(); i++) {
            RadialMenu2* tmpRadialMenu = unit->radialMenus.at(i);
            tmpRadialMenu->proxyWidget = scene->addWidget(tmpRadialMenu);
            tmpRadialMenu->proxyWidget->setZValue(20);
            tmpRadialMenu->setRotation(player);
            tmpRadialMenu->hide();
            tmpRadialMenu->calculateGeometry();
            playersRadialMenus[player].append(tmpRadialMenu);
        }
    }
}

void MainWindow::getRangeItem(QGraphicsItem *item)
{
    this->scene->addItem(item);
}

/**
 * @brief MainWindow::getBoardState zobrazi / skryje UI v zavislosti na stavu
 * @param state - id stavu
 */
void MainWindow::getBoardState(int state)
{
    this->hideAll();

    switch (state) {
    case 1: // combat
//        this->ui->pushButtonDM->show();
        //delete this->ui->pushButtonStartCombat;
        //this->ui->widgetPlayer->show();
        this->displayRadialMenu(true);
        this->ui->menuDM->show();
        break;
    case 0: //ihned po zapnutí, main menu
        this->ui->box_startMenu->show();        
        break;
    case -1:    //hra bezi pred combatem
//        this->ui->pushButtonDM->show();
        this->ui->menuDM->show();
        break;
    case -2:    //schovane dm menu - vyber dlazdice pro pridani jednotky
        this->displayRadialMenu(false);
        break;
    }
    this->state = state;
}

/**
 * @brief MainWindow::getTileTypes - vytvori tlacitka pro vyber typu dlazdice v menu pro upravovani mapy
 * @param types -   seznam nazvu typu
 */
void MainWindow::getTileTypes(QVector<QString> types)
{
    for (int i = 0; i < editMapBtns.size(); i++) {
        delete editMapBtns[i];
    }
    editMapBtns.resize(0);
    for (int i = 0; i < types.size(); i++) {
        editMapBtns.append(new TileSelectButton(types[i], this->ui->box_editMap));
        styleToFantasyBtn(editMapBtns.last());
        editMapBtns.last()->setGeometry(0, 10 + (i+2)*75,  editMapBtns.last()->width(),  editMapBtns.last()->height());
    }
//    ui->box_editMap->adjustSize();
//    ui->editBtsScrollArea->setGeometry(100, 100, 250, 1080-200);
//    ui->editBtsScrollArea->adjustSize();
}

/**
 * @brief MainWindow::on_pushButtonAddUnit_clicked stisknuti tlacitka pro pridani jednotky
 */
void MainWindow::on_pushButtonAddUnit_clicked()
{
    int stateTmp = state;
    FileDialog* fileDialog = new FileDialog("units/", this);
    QObject::connect(fileDialog, SIGNAL(click(QString)), this, SIGNAL(sendNewUnit(QString)));
    QObject::connect(fileDialog, &FileDialog::click, [stateTmp, this](){this->getBoardState(stateTmp);});
    getBoardState(-2);
    fileDialog->show();
}

void MainWindow::on_pushButtonDM_clicked()
{
    if (this->ui->menuDM->isHidden()) {
        this->ui->menuDM->show();
        this->sendDMMode(true);
    }
    else {
        this->ui->menuDM->hide();
        this->sendDMMode(false);
    }
}

void MainWindow::on_pushButtonMapLoad_clicked()
{
    FileDialog* fileDialog = new FileDialog("maps/", this);
    QObject::connect(fileDialog, SIGNAL(click(QString)), this, SIGNAL(sendMap(QString)));
    fileDialog->show();
}

bool MainWindow::event(QEvent *event)
{
//    if (event->type() == QEvent::Gesture) {
//        gestureEvent(dynamic_cast<QGestureEvent*>(event));
//    }
    if (event->type() == QEvent::TouchBegin) {
        touchEvent(dynamic_cast<QTouchEvent*>(event));
        touchBeginEvent(dynamic_cast<QTouchEvent*>(event));

        return false;
    }
    if (event->type() == QEvent::TouchUpdate) {//       
        touchEvent(dynamic_cast<QTouchEvent*>(event));
        touchUpdateEvent(dynamic_cast<QTouchEvent*>(event));
        return false;
    }
    if (event->type() == QEvent::TouchEnd) {        
//        QWidget::event(event);
        touchEvent(dynamic_cast<QTouchEvent*>(event));
        touchEndEvent(dynamic_cast<QTouchEvent*>(event));
        return false;
    }
    return QWidget::event(event);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
}

void MainWindow::touchBeginEvent(QTouchEvent * event)
{
}

void MainWindow::touchUpdateEvent(QTouchEvent *event)
{
}

void MainWindow::touchEndEvent(QTouchEvent *event)
{
    dynamic_cast<MyScene*>(scene)->release();
}

bool MainWindow::touchEvent(QTouchEvent *event)
{        
}

/**
 * @brief MainWindow::createRollMenus vytvoreni menu pro kazdeho hrace (kostky, infoBox,...)
 */
void MainWindow::createRollMenus()
{
    createRollMenu(0, "player 0");
    createRollMenu(1, "player 1");
    createRollMenu(2, "player 2 - DM");
    createRollMenu(3, "player 3");
}

/**
 * @brief MainWindow::createRollMenu vytvoreni jednoho menu hrace (kostky, infoBox,...)
 * @param playerID  -    id hrace pro urceni rotace a pozice menu
 * @param panelText -   defaultni napis v infoBoxu
 */
void MainWindow::createRollMenu(int playerID, QString panelText)
{
    int margin =10;
    double manuScale = 0.6;
    QString infoPanelStyle = "background: rgba(125,125,125,50%); font-size: 25px; padding: 5px";

    //horni menu
    QGraphicsScene *scene1 = new QGraphicsScene();
    QGraphicsView *view1 = new QGraphicsView(this);
    //info panel
    QLabel *panel = new QLabel();
    panel->setStyleSheet(infoPanelStyle);
    panel->setGeometry(0,0, 490, 120);
    scene1->addWidget(panel);
    panel->setText(panelText+panel->text());
    panel->setAlignment(Qt::AlignTop);
    playersInfoPanels.push_back(panel);
    //diceMenu
    DiceMenu* diceMenu = new DiceMenu();
    diceMenu->setGeometry(500, 0, diceMenu->width(), diceMenu->height());
    scene1->addWidget(diceMenu);
    view1->setScene(scene1);
    view1->setStyleSheet("background:transparent; padding: 0px");
    view1->setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
    view1->setVerticalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
    view1->scale(manuScale, manuScale);

    switch (playerID) { //umisteni menu a rotace
    case 0:
        view1->rotate(180);
        view1->adjustSize();
        view1->setGeometry(ui->graphicsView->x(),
                           ui->graphicsView->y() - view1->height() - margin,
                           view1->width(), view1->height());
        break;
    case 1:
        view1->rotate(90);
        view1->adjustSize();
        view1->setGeometry(ui->graphicsView->x() - view1->width() - margin,
                           ui->graphicsView->height() + ui->graphicsView->y() - view1->height() - 70,
                           view1->width(), view1->height() + 70);
        break;
    case 2:
        view1->adjustSize();
        view1->setGeometry(ui->graphicsView->width() + ui->graphicsView->x() - view1->width(),
                           ui->graphicsView->height() + ui->graphicsView->y() + margin,
                           view1->width(), view1->height());
        break;
    case 3:
        view1->rotate(270);
        view1->adjustSize();
        view1->setGeometry(ui->graphicsView->width() + ui->graphicsView->x() + margin,
                           ui->graphicsView->y(),
                           view1->width(), view1->height() + 70);
        break;
    default:
        break;
    }

    view1->show();
    diceMenus.push_back(diceMenu);
    //okButton
    playerOKButton[playerID] = new QPushButton("OK");
    styleToFantasyBtn(playerOKButton[playerID]);
    scene1->addWidget(playerOKButton[playerID])->setScale(1.7);
    QObject::connect(playerOKButton[playerID], &QPushButton::clicked, [this, playerID](){this->playerOKPressed(playerID);});
    playerOKButton[playerID]->hide();
    //smallButtons
    QPushButton* smallBtn = new QPushButton("OK");
    scene1->addWidget(smallBtn)->setScale(1.7);
    QObject::connect(smallBtn, &QPushButton::clicked, [this, playerID](){this->smallButtonPressed(playerID, 0);});
    smallBtn->setStyleSheet("QPushButton {Background: url(:/Img/Img/buttonSmall.png); Border: 0px; font: bold 20px;}"
                       "QPushButton:hover {Background: url(:/Img/Img/fantasy_button_active.png)}"
                       "QPushButton:open {Background: url(:/Img/Img/fantasy_button_clicked.png)}");
    smallBtn->setGeometry(490, 0, 156, 71);
    this->smallBtn[playerID].append(smallBtn);
    smallBtn->hide();
    smallBtn = new QPushButton("Cancel");
    scene1->addWidget(smallBtn)->setScale(1.7);
    QObject::connect(smallBtn, &QPushButton::clicked, [this, playerID](){this->smallButtonPressed(playerID, 1);});
    smallBtn->setStyleSheet("QPushButton {Background: url(:/Img/Img/buttonSmall.png); Border: 0px; font: bold 20px;}"
                       "QPushButton:hover {Background: url(:/Img/Img/fantasy_button_active.png)}"
                       "QPushButton:open {Background: url(:/Img/Img/fantasy_button_clicked.png)}");
    smallBtn->setGeometry(756, 0, 156, 71);
    this->smallBtn[playerID].append(smallBtn);
    smallBtn->hide();
}

//void MainWindow::gestureEvent(QGestureEvent *event)
//{
//    qDebug()<<"gestureEvent";
//}

//QPoint MainWindow::corectTouchCoordinates(int x, int y)
//{
//    x *= settings->touchKX;
//    y *= settings->touchKY;
//    x -= settings->touchModX * settings->touchKX;
//    y -= settings->touchModY * settings->touchKX;
//    return QPoint(x, y);
//}

/**
 * @brief MainWindow::styleToFantasyBtn - nastavi styl tlacitka a jeho pozici
 * @param btn   -   tlacitko ktere se bude upravovat
 * @param x -   nova pozice, pokud je nezadano nebo zadano -1 pouzije se stavajici pozice
 * @param y -   nova pozice, pokud je nezadano nebo zadano -1 pouzije se stavajici pozice
 */
void MainWindow::styleToFantasyBtn(QPushButton *btn, int x, int y)
{
    x = (x == -1) ? btn->x() : x;
    y = (y == -1) ? btn->y() : y;
    btn->setStyleSheet("QPushButton {Background: url(:/Img/Img/fantasy_button_normal.png); Border: 0px; font: bold 20px;}"
                       "QPushButton:hover {Background: url(:/Img/Img/fantasy_button_active.png)}"
                       "QPushButton:open {Background: url(:/Img/Img/fantasy_button_clicked.png)}");
    btn->setGeometry(x, y, 250, 71);
}

/**
 * @brief MainWindow::selectUnit    - vyber jednotky z hraci plochy
 * @param playerID - id hrace ktery jednotku vybira
 * @return vraci vybranou jednotku
 */
TileUnit *MainWindow::selectUnit(int playerID)
{
    int stateTmp = state;
    getBoardState(-2);

    waitOnUnitClick->exec(); //cekani na klik na jednotku

    TileUnit * tmp = tmpUnit;
    tmpUnit = NULL;
    getBoardState(stateTmp);
    return tmp;

}

void MainWindow::on_pushButtonStartCombat_clicked()
{
    ui->pushButtonStartCombat->deleteLater();
    this->sendStartCombat();
}

void MainWindow::on_pushButtonEndTurn_clicked()
{
    this->sendEndTurn();
}

void MainWindow::on_pushButtonMove_clicked()
{
    this->sendMove();
}

void MainWindow::on_slider_newMap_X_valueChanged(int value)
{
    this->ui->label_newMap_X->setText(QString::number(value));
    this->ui->label_newMap_X_ft->setText(QString::number(value * DEFAULT_SPEED_COST));
    this->ui->label_newMap_X_m->setText(QString::number(value * DEFAULT_SPEED_COST * 0.3));
}

void MainWindow::on_slider_newMap_Y_valueChanged(int value)
{
    this->ui->label_newMap_Y->setText(QString::number(value));
    this->ui->label_newMap_Y_ft->setText(QString::number(value * DEFAULT_SPEED_COST));
    this->ui->label_newMap_Y_m->setText(QString::number(value * DEFAULT_SPEED_COST * 0.3));
}

void MainWindow::on_btn_newMap_create_clicked()
{
    sendNewMap(this->ui->label_newMap_X->text().toInt(), this->ui->label_newMap_Y->text().toInt());
}

void MainWindow::on_btn_newMap_clicked()
{
    this->hideAll();
    this->ui->box_newEmptyMapSetting->show();
}

void MainWindow::on_btn_editMap_menuToggle_clicked()
{
    this->hideAll();
    this->ui->box_editMap->show();
    ui->editBtsScrollArea->show();
    ui->btn_hideShowEditMap->show();
}

void MainWindow::on_btn_editMap_end_clicked()
{
    this->hideAll();

    getBoardState(this->state);    
    TileSelectButton::selected = NULL;
}

void MainWindow::on_btn_newMap_back_clicked()
{
    getBoardState(this->state);
}

int MainWindow::getRoll(int n, int t)
{
    return roll(n, t);
}

void MainWindow::setSettings(Settings *value)
{
    settings = value;
}

TileSelectButton*  TileSelectButton::selected = NULL;

TileSelectButton::TileSelectButton(const QString &text, QWidget *parent)
    : QPushButton(text, parent)
{
    connect(this, SIGNAL(clicked(bool)), this, SLOT(click()));
}

void TileSelectButton::select()
{
    this->selected = this;
}

void TileSelectButton::click()
{
    this->select();
}

void MainWindow::on_btn_saveMap_pressed()
{
    sSaveMap();
}

void MainWindow::on_btn_hideShowEditMap_pressed()
{
    if (ui->editBtsScrollArea->isHidden())
        ui->editBtsScrollArea->show();
    else
        ui->editBtsScrollArea->hide();
}
