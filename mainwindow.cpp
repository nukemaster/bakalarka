#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->ui->statusBar->hide();
    this->ui->mainToolBar->hide();

    this->scene = new MyScene();
    this->ui->graphicsView->setScene(this->scene);
//    dynamic_cast<MyScene*>(scene)->setAceptTouchEvents();

    this->hideAll();
    this->ui->box_startMenu->show();

    this->cursorBlue = new QCursor(QPixmap(":/Img/Img/cursor_blue.png"));
    this->cursorRed =  new QCursor(QPixmap(":/Img/Img/cursor_red.png"));

    this->setCursor(*cursorBlue);

    this->setAttribute(Qt::WA_AcceptTouchEvents);

    createRollMenu();
}

MainWindow::~MainWindow()
{
    delete ui;

}

void MainWindow::hideAll()
{
    ui->menuDM->hide();
    ui->pushButtonDM->hide();
    ui->box_startMenu->hide();
    ui->widgetPlayer->hide();
    ui->box_newEmptyMapSetting->hide();
    ui->box_editMap->hide();
}

void MainWindow::setFullScreen()
{
    this->showFullScreen();
    this->ui->graphicsView->setGeometry(0,0, QApplication::desktop()->screenGeometry().width(), QApplication::desktop()->screenGeometry().height());

    ui->box_dicesRolls->setGeometry(QApplication::desktop()->screenGeometry().width()-130, 0, 130, QApplication::desktop()->screenGeometry().height());

    ui->box_startMenu->setGeometry(1920 / 2 - 250/2, 100, 250, ui->box_startMenu->height());
    ui->box_editMap->setGeometry(ui->box_editMap->x(), ui->box_editMap->y(), 250, ui->box_editMap->height());
    ui->box_newEmptyMapSetting->setGeometry(1920 / 2 - 250/2, 100, 250, ui->box_newEmptyMapSetting->height());
    ui->menuDM->setGeometry(ui->menuDM->x(), ui->menuDM->y(), 250, ui->menuDM->height());

    styleToFantasyBtn(ui->pushButtonAddUnit);
    styleToFantasyBtn(ui->btn_editMap_menuToggle);
    styleToFantasyBtn(ui->pushButtonStartCombat);
    styleToFantasyBtn(ui->btn_editMap_end);
    styleToFantasyBtn(ui->pushButtonMapLoad);
    styleToFantasyBtn(ui->btn_newMap);
    styleToFantasyBtn(ui->btn_calibration);
    styleToFantasyBtn(ui->btn_newMap_create);
    styleToFantasyBtn(ui->btn_newMap_back);
}

long MainWindow::normalizeMouseCoordinate(int coor, int maxCoor)
{
    return ((65535.0 / maxCoor) * coor);
}

void MainWindow::coordinatesFromDetection(int x, int y, bool inClickArea)
{
    //ovladani pomoci kinectu
}

void MainWindow::coordinatesFromDetection2(int x, int y, bool inClickArea)
{
    if (x == -1) {
        ui->menuDM->hide();
        return;
    }
    if (playerIdOnXY(x, y) == dungeonMaster) {
        ui->menuDM->show();
        return;
    }

    ui->menuDM->hide();

}

void MainWindow::getPixmapItem(QGraphicsPixmapItem *item)
{
    this->scene->addItem(item);

    TileUnit* unit;
    if ((unit = dynamic_cast<TileUnit*>(item))){
        unit->radialMenu = new RadialMenu2(this);
        QPushButton* btn = new QPushButton("pohyb");
        QObject::connect(btn, SIGNAL(pressed()), this, SLOT(on_pushButtonMove_clicked()));
        unit->radialMenu->addButton(btn);

        btn = new QPushButton("konec tahu");
        QObject::connect(btn, SIGNAL(pressed()), this, SLOT(on_pushButtonEndTurn_clicked()));
        unit->radialMenu->addButton(btn);

        unit->radialMenu->calculateGeometry();

        //unit->radialMenu->show();
    }
}

void MainWindow::getRangeItem(QGraphicsItem *item)
{
    this->scene->addItem(item);
}

void MainWindow::getBoardState(int state)
{
    this->hideAll();

    switch (state) {
    case 1:
        this->ui->pushButtonDM->show();
        //delete this->ui->pushButtonStartCombat;
        //this->ui->widgetPlayer->show();
        break;
    case 0:
        this->ui->box_startMenu->show();
        break;
    case -1:
        this->ui->pushButtonDM->show();
        this->ui->menuDM->show();
        break;
    case -2:
        break;
    }
    this->state = state;
}

void MainWindow::getTileTypes(QVector<QString> types)
{
    for (int i = 0; i < editMapBtns.size(); i++) {
        delete editMapBtns[i];
    }
    editMapBtns.resize(0);

    for (int i = 0; i < types.size(); i++) {
        editMapBtns.append(new TileSelectButton(types[i], this->ui->box_editMap));
        editMapBtns.last()->setGeometry(0, i*75, 100, 50);
    }
}

void MainWindow::on_pushButtonAddUnit_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Load Unit"), "", tr("unit XML (*.xml)"));
    if (fileName != NULL)
        this->sendNewUnit(fileName);
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
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Map"), "", tr("XML (*.xml)"));
    if (fileName != NULL) {
        this->sendMap(fileName);
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    QTouchEvent *touchEvent = NULL;
    if ((touchEvent = dynamic_cast<QTouchEvent*>(event)) != NULL ) {
        qDebug()<<"touchEvent"<<touchEvent->type();
        if ((event->type() == QEvent::TouchBegin)){
            this->touchBeginEvent(touchEvent);
        }
        if ((event->type() == QEvent::TouchUpdate)) {
            this->touchUpdateEvent(touchEvent);
        }
        if ((event->type() == QEvent::TouchEnd)||(event->type() == QEvent::TouchCancel)) {
            this->touchEndEvent(touchEvent);
        }
        event->accept();
        return true;
    }
    QMouseEvent * mouseEvent;
    if ((event->type() == QEvent::MouseButtonPress)||(event->type() == QEvent::MouseButtonRelease)||(event->type() == QEvent::MouseMove)) {
        if ((mouseEvent = dynamic_cast<QMouseEvent *>(event)) != NULL) {

        }
    }
    return false;
}

bool MainWindow::event(QEvent *event)
{

    if (event->type() == QEvent::Gesture) {
        gestureEvent(dynamic_cast<QGestureEvent*>(event));
    }
    if (event->type() == QEvent::TouchBegin) {

    }
    if (event->type() == QEvent::TouchUpdate) {

    }
    if (event->type() == QEvent::TouchEnd) {
        touchEndEvent(dynamic_cast<QTouchEvent*>(event));
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

void MainWindow::createRollMenu()
{
    int y = 110;
    DiceButton* lBtn = new DiceButton(DiceButton::D4, ui->box_dicesRolls);
    QObject::connect(lBtn, SIGNAL(roll(int,int)), this, SLOT(getRoll(int,int)));
    lBtn->setGeometry(lBtn->pos().x(), y, lBtn->width(), lBtn->height());
    y += 140;
    lBtn = new DiceButton(DiceButton::D6, ui->box_dicesRolls);
    QObject::connect(lBtn, SIGNAL(roll(int,int)), this, SLOT(getRoll(int,int)));
    lBtn->setGeometry(lBtn->pos().x(), y, lBtn->width(), lBtn->height());
    y += 140;
    lBtn = new DiceButton(DiceButton::D8, ui->box_dicesRolls);
    QObject::connect(lBtn, SIGNAL(roll(int,int)), this, SLOT(getRoll(int,int)));
    lBtn->setGeometry(lBtn->pos().x(), y, lBtn->width(), lBtn->height());
    y += 140;
    lBtn = new DiceButton(DiceButton::D10, ui->box_dicesRolls);
    QObject::connect(lBtn, SIGNAL(roll(int,int)), this, SLOT(getRoll(int,int)));
    lBtn->setGeometry(lBtn->pos().x(), y, lBtn->width(), lBtn->height());
    y += 140;
    lBtn = new DiceButton(DiceButton::D12, ui->box_dicesRolls);
    QObject::connect(lBtn, SIGNAL(roll(int,int)), this, SLOT(getRoll(int,int)));
    lBtn->setGeometry(lBtn->pos().x(), y, lBtn->width(), lBtn->height());
    y += 140;
    lBtn = new DiceButton(DiceButton::D20, ui->box_dicesRolls);
    QObject::connect(lBtn, SIGNAL(roll(int,int)), this, SLOT(getRoll(int,int)));
    lBtn->setGeometry(lBtn->pos().x(), y, lBtn->width(), lBtn->height());
    y += 140;
}

void MainWindow::gestureEvent(QGestureEvent *event)
{
    qDebug()<<"gestureEvent";
}

QPoint MainWindow::corectTouchCoordinates(int x, int y)
{
    x *= settings->touchKX;
    y *= settings->touchKY;
    x -= settings->touchModX * settings->touchKX;
    y -= settings->touchModY * settings->touchKX;
    return QPoint(x, y);
}

void MainWindow::styleToFantasyBtn(QPushButton *btn)
{
    btn->setStyleSheet("QPushButton {Background: url(:/Img/Img/fantasy_button_normal.png); Border: 0px; font: bold 20px;}"
                       "QPushButton:hover {Background: url(:/Img/Img/fantasy_button_active.png)}"
                       "QPushButton:open {Background: url(:/Img/Img/fantasy_button_clicked.png)}");
    btn->setGeometry(0, btn->y(), 250, 71);
}

void MainWindow::on_pushButtonStartCombat_clicked()
{
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

void MainWindow::on_btn_calibration_clicked()
{
    showCalibration();
}
