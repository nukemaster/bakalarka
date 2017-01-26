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

    this->hideAll();
    this->ui->box_startMenu->show();

    this->cursorBlue = new QCursor(QPixmap(":/Img/Img/cursor_blue.png"));
    this->cursorRed =  new QCursor(QPixmap(":/Img/Img/cursor_red.png"));

    this->setCursor(*cursorBlue);

    /*ui->graphicsView->*/

    grabGesture(Qt::SwipeGesture);
    //grabGesture(Qt::TapGesture);

    //grabGesture(Qt::PanGesture);
    //setCursor(Qt::BlankCursor);

    this->setAttribute(Qt::WA_AcceptTouchEvents);
    this->installEventFilter(this);
    //ui->label->setPixmap(QPixmap(":/Img/Img/cursor_red.png"));
    //ui->label->setAttribute( Qt::WA_TransparentForMouseEvents );

    //TouchFilter * touchfilter = new TouchFilter();
    //touchfilter->show();

    ButtonLayer * m = (new ButtonLayer({"a", "b", "c", "d", "e", "f"}, this));
    m->setGeometry(250, 880, /*m->width(), m->height()*/ 500,500);
    m->show();
    //m->setStyleSheet("background-color:black;");

    ButtonLayer * m2 = (new ButtonLayer({"a", "a", "a", "a", "a", "a"}, m));
    m->setNextLayers({m2, m2, m2, m2, m2, m2});
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
    this->ui->graphicsView->setGeometry(0,0, this->width(), this->height());

    //ui->pushButton->setGeometry(1700, 800, ui->pushButton->width(), ui->pushButton->height() );

}

long MainWindow::normalizeMouseCoordinate(int coor, int maxCoor)
{
    return ((65535.0 / maxCoor) * coor);
}

void MainWindow::coordinatesFromDetection(int x, int y, bool inClickArea)
{
    INPUT mEvent;
    mEvent.type = INPUT_MOUSE;
    mEvent.mi.dx = normalizeMouseCoordinate(x, this->width());
    mEvent.mi.dy = normalizeMouseCoordinate(y, this->height());
    mEvent.mi.dwExtraInfo = 0;
    mEvent.mi.mouseData =   0;
    mEvent.mi.time =        0;

    if ((x < 0)||(y < 0)) {
        if (mousePressed) {
            mEvent.mi.dwFlags = MOUSEEVENTF_LEFTUP;
            mEvent.mi.dx = 0;
            mEvent.mi.dy = 0;
            SendInput(1, &mEvent, sizeof(INPUT));
            mousePressed = false;
            this->setCursor(*cursorBlue);
        }
        return;
    }

    if (inClickArea) {

        if (mousePressed) {
            mEvent.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
            SendInput(1, &mEvent, sizeof(INPUT));
            qDebug()<<playerIdOnXY(x, y);
        }
        else {
            INPUT mEvent2[2];

            mEvent2[0].type = INPUT_MOUSE;
            mEvent2[0].mi.dx =  normalizeMouseCoordinate(x, this->width());
            mEvent2[0].mi.dy =  normalizeMouseCoordinate(y, this->height());
            mEvent2[0].mi.dwExtraInfo = 0;
            mEvent2[0].mi.mouseData =   0;
            mEvent2[0].mi.time =        0;
            mEvent2[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_LEFTUP;

            mEvent2[1].type = INPUT_MOUSE;
            mEvent2[1].mi.dx = 0;// normalizeMouseCoordinate(x, this->width());
            mEvent2[1].mi.dy = 0;// normalizeMouseCoordinate(y, this->height());
            mEvent2[1].mi.dwExtraInfo = 0;
            mEvent2[1].mi.mouseData =   0;
            mEvent2[1].mi.time =        0;
            mEvent2[1].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

            SendInput(2, mEvent2, sizeof(INPUT));

            this->setCursor(*cursorRed);

            mousePressed = true;
        }
    }
    else {
        if (mousePressed) {
            mEvent.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP;
            SendInput(1, &mEvent, sizeof(INPUT));
            mousePressed = false;
            this->setCursor(*cursorBlue);
        }
        else {
            cursor().setPos(QPoint(x, y));
        }
    }
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
        this->ui->widgetPlayer->show();
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
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Map"), "", tr("map XML (*.xml)"));
    this->sendMap(fileName);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    qDebug()<<"MainWindow mousePressEvent" << event->pos() << event->source();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug()<<"MainWindow mouseReleaseEvent"<< event->pos() << event->source();
}

void MainWindow::gestureEvent(QGestureEvent *event)
{
    qDebug()<<"event";
}

void MainWindow::touchBeginEvent(QTouchEvent * event)
{
    event->setAccepted(true);
    ui->label->show();

    QPoint point = corectTouchCoordinates(event->touchPoints()[0].pos().x(), event->touchPoints()[0].pos().y());
    int x = point.x();
    int y = point.y();

    ui->label->setGeometry(x-20, y-20, 40,40);

    INPUT mEvent2[2];

    mEvent2[0].type = INPUT_MOUSE;
    mEvent2[0].mi.dx =  normalizeMouseCoordinate(x, this->width());
    mEvent2[0].mi.dy =  normalizeMouseCoordinate(y, this->height());
    mEvent2[0].mi.dwExtraInfo = 0;
    mEvent2[0].mi.mouseData =   0;
    mEvent2[0].mi.time =        0;
    mEvent2[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

    mEvent2[1].type = INPUT_MOUSE;
    mEvent2[1].mi.dx =  0/*normalizeMouseCoordinate(x, this->width())*/;
    mEvent2[1].mi.dy =  0/*normalizeMouseCoordinate(y, this->height())*/;
    mEvent2[1].mi.dwExtraInfo = 0;
    mEvent2[1].mi.mouseData =   0;
    mEvent2[1].mi.time =        0 ;
    mEvent2[1].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

    SendInput(2, mEvent2, sizeof(INPUT));
}

bool MainWindow::event(QEvent *event)
{

    if (event->type() == QEvent::Gesture) {
        qDebug()<<"gesture";
    }
    if (event->type() == QEvent::TouchBegin) {
        //event->accept();
        //this->touchEvent(dynamic_cast<QTouchEvent*>( event));
        //qDebug()<<event->device();
//        ui->label->show();
//        qDebug()<<"test";
//        ui->label->setGeometry(dynamic_cast<QTouchEvent*>(event)->touchPoints()[0].pos().x()-20,
//                dynamic_cast<QTouchEvent*>(event)->touchPoints()[0].pos().y()-20, 40,40);

//        return true;
        qDebug()<<(dynamic_cast<QTouchEvent * >(event))->touchPoints()[0].pos();
    }
    if (event->type() == QEvent::TouchUpdate) {

//        ui->label->setGeometry(dynamic_cast<QTouchEvent*>(event)->touchPoints()[0].pos().x()-20,
//                dynamic_cast<QTouchEvent*>(event)->touchPoints()[0].pos().y()-20, 40,40);

        //event->accept();
        //qDebug()<<"event()";
        //this->touchEvent(dynamic_cast<QTouchEvent*>( event));
//        return true;
    }
//    if (event->type() == QEvent::TouchEnd) {

//        ui->label->hide();
//    }
    return QWidget::event(event);
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{

    QTouchEvent *touchEvent = NULL;
    if ((touchEvent = dynamic_cast<QTouchEvent*>(event)) != NULL ) {
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
        qDebug()<<"touchEvent"<<touchEvent->type();
        return true;
    }
//    QMouseEvent * mouseEvent;
//    if ((event->type() == QEvent::MouseButtonPress)||(event->type() == QEvent::MouseButtonRelease)||(event->type() == QEvent::MouseMove))
//        if ((mouseEvent = dynamic_cast<QMouseEvent *>(event)) != NULL) {
//            qDebug()<<"QMouseEvent"<<event->type();
//            if (mouseEvent->source() == Qt::MouseEventNotSynthesized) {
//                //qDebug()<<"not synthetiset";
//                //return true;

//            }
//            if (mouseEvent->source() == Qt::MouseEventSynthesizedByQt) {
//                //qDebug()<<"synthetiset by Qt";
//            }
//            if (mouseEvent->source() == Qt::MouseEventSynthesizedBySystem) {
//                qDebug()<<"synthetiset by system";
//                event->accept();
//                return true;
//            }

//    }
    return false;
}

void MainWindow::touchUpdateEvent(QTouchEvent *event)
{
    event->setAccepted(true);

    QPoint point = corectTouchCoordinates(event->touchPoints()[0].pos().x(), event->touchPoints()[0].pos().y());
    int x = point.x();
    int y = point.y();

    ui->label->setGeometry(x-20, y-20, 40,40);

    INPUT mEvent2[1];

    mEvent2[0].type = INPUT_MOUSE;
    mEvent2[0].mi.dx =  normalizeMouseCoordinate(x, this->width());
    mEvent2[0].mi.dy =  normalizeMouseCoordinate(y, this->height());
    mEvent2[0].mi.dwExtraInfo = 0;
    mEvent2[0].mi.mouseData =   0;
    mEvent2[0].mi.time =        0;
    mEvent2[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;


    SendInput(1, mEvent2, sizeof(INPUT));
}

void MainWindow::touchEndEvent(QTouchEvent *event)
{
    event->setAccepted(true);
    ui->label->hide();

    QPoint point = corectTouchCoordinates(event->touchPoints()[0].pos().x(), event->touchPoints()[0].pos().y());
    int x = point.x();
    int y = point.y();

    ui->label->setGeometry(x-20, y-20, 40,40);
    qDebug()<<x<<y;
    INPUT mEvent2[3];

    mEvent2[0].type = INPUT_MOUSE;
    mEvent2[0].mi.dx =  normalizeMouseCoordinate(x, this->width());
    mEvent2[0].mi.dy =  normalizeMouseCoordinate(y, this->height());
    mEvent2[0].mi.dwExtraInfo = 0;
    mEvent2[0].mi.mouseData =   0;
    mEvent2[0].mi.time =        0;
    mEvent2[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

    mEvent2[1].type = INPUT_MOUSE;
    mEvent2[1].mi.dx =  normalizeMouseCoordinate(x, this->width());
    mEvent2[1].mi.dy =  normalizeMouseCoordinate(y, this->height());
    mEvent2[1].mi.dwExtraInfo = 0;
    mEvent2[1].mi.mouseData =   0;
    mEvent2[1].mi.time =        0;
    mEvent2[1].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN;

    mEvent2[2].type = INPUT_MOUSE;
    mEvent2[2].mi.dx =  normalizeMouseCoordinate(x, this->width());
    mEvent2[2].mi.dy =  normalizeMouseCoordinate(y, this->height());
    mEvent2[2].mi.dwExtraInfo = 0;
    mEvent2[2].mi.mouseData =   0;
    mEvent2[2].mi.time =        0;
    mEvent2[2].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP;

    qDebug()<<SendInput(3, mEvent2, sizeof(INPUT));
    //Sleep(1000);
}

QPoint MainWindow::corectTouchCoordinates(int x, int y)
{
    x *= settings->touchKX;
    y *= settings->touchKY;
    x -= settings->touchModX * settings->touchKX;
    y -= settings->touchModY * settings->touchKX;
    return QPoint(x, y);
}

TileSelectButton*  TileSelectButton::selected = NULL;

TileSelectButton::TileSelectButton(const QString &text, QWidget *parent)
    : QPushButton(text, parent)
{
    //QPushButton::QPushButton( "&text", parent);
    connect(this, SIGNAL(clicked(bool)), this, SLOT(click()));
}

void TileSelectButton::select()
{
    //selectedValue = this->text();
    this->selected = this;
}
/*
QString TileSelectButton::getSelectedValue()
{
    return selectedValue;
}
*/
void TileSelectButton::click()
{
    //QPushButton::click();
    this->select();
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

void MainWindow::setSettings(Settings *value)
{
    settings = value;
}

TouchFilter::TouchFilter(QWidget *parent) : QWidget(parent)
{
    this->setGeometry(0,0,1920,1080);
    //this->setAttribute( Qt::WA_TransparentForMouseEvents );
    this->setAttribute(Qt::WA_AcceptTouchEvents);
    setStyleSheet("background:transparent;");
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint);
    this->installEventFilter(this);

}

bool TouchFilter::event(QEvent *event)
{
    if (event->type() == QEvent::TouchBegin) {
        qDebug()<<"TouchFilter> begin";
        this->touchEvent(dynamic_cast<QTouchEvent*>( event));
        return true;
    }
    if (event->type() == QEvent::TouchUpdate) {
        qDebug()<<"TouchFilter> update";
        touchUpdateEvent(dynamic_cast<QTouchEvent*>( event));
        //this->touchUpdateEvent(dynamic_cast<QTouchEvent*>( event));
        return true;
    }
    if (event->type() == QEvent::TouchEnd) {
        qDebug()<<"TouchFilter> end";
        this->touchEndEvent(dynamic_cast<QTouchEvent*>( event));
        return true;
    }
    return QWidget::event(event);
}

void TouchFilter::touchEvent(QTouchEvent *event)
{
    //qDebug()<<"touchFilter touchEvent" << event->target();

//    QList<QTouchEvent::TouchPoint> newList;
//    newList.append(QTouchEvent::TouchPoint());
//    newList[0].setPos(dynamic_cast<MainWindow *> (parent())->corectTouchCoordinates(event->touchPoints()[0].pos().x(), event->touchPoints()[0].pos().y()));
//    QTouchEvent * newEvent = new QTouchEvent(QEvent::TouchBegin, Q_NULLPTR, Qt::NoModifier, Qt::TouchPointStates(), newList);
//    newEvent->setTarget(this->parent());
//    QCoreApplication::postEvent(this->parent(), newEvent);
    event->setAccepted(true);
    MainWindow * w = dynamic_cast<MainWindow*>(this->parent());
    //w->ui->label->show();

    QPoint point = w->corectTouchCoordinates(event->touchPoints()[0].pos().x(), event->touchPoints()[0].pos().y());
    int x = point.x();
    int y = point.y();

    //w->ui->label->setGeometry(x-20, y-20, 40,40);

    INPUT mEvent2[1];

    mEvent2[0].type = INPUT_MOUSE;
    mEvent2[0].mi.dx =  w->normalizeMouseCoordinate(x, this->width());
    mEvent2[0].mi.dy =  w->normalizeMouseCoordinate(y, this->height());
    mEvent2[0].mi.dwExtraInfo = 0;
    mEvent2[0].mi.mouseData =   0;
    mEvent2[0].mi.time =        0;
    mEvent2[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_LEFTDOWN;

    SendInput(1, mEvent2, sizeof(INPUT));

    //event->accept();
}

void TouchFilter::touchUpdateEvent(QTouchEvent *event)
{
//    QList<QTouchEvent::TouchPoint> newList;
//    newList.append(QTouchEvent::TouchPoint());
//    newList[0].setPos(dynamic_cast<MainWindow *> (parent())->corectTouchCoordinates(event->touchPoints()[0].pos().x(), event->touchPoints()[0].pos().y()));
//    QTouchEvent * newEvent = new QTouchEvent(QEvent::TouchUpdate, Q_NULLPTR, Qt::NoModifier, Qt::TouchPointStates(), newList);
//    newEvent->setTarget(this->parent());
//    QCoreApplication::postEvent(this->parent(), newEvent);
    event->setAccepted(true);
    MainWindow * w = dynamic_cast<MainWindow*>(this->parent());

    QPoint point = w->corectTouchCoordinates(event->touchPoints()[0].pos().x(), event->touchPoints()[0].pos().y());
    int x = point.x();
    int y = point.y();

    //ui->label->setGeometry(x-20, y-20, 40,40);

    INPUT mEvent2[1];

    mEvent2[0].type = INPUT_MOUSE;
    mEvent2[0].mi.dx =  w->normalizeMouseCoordinate(x, this->width());
    mEvent2[0].mi.dy =  w->normalizeMouseCoordinate(y, this->height());
    mEvent2[0].mi.dwExtraInfo = 0;
    mEvent2[0].mi.mouseData =   0;
    mEvent2[0].mi.time =        0;
    mEvent2[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

    SendInput(1, mEvent2, sizeof(INPUT));
    //event->accept();
}

void TouchFilter::touchEndEvent(QTouchEvent *event)
{
//    QList<QTouchEvent::TouchPoint> newList;
//    newList.append(QTouchEvent::TouchPoint());
//    newList[0].setPos(dynamic_cast<MainWindow *> (parent())->corectTouchCoordinates(event->touchPoints()[0].pos().x(), event->touchPoints()[0].pos().y()));
//    QTouchEvent * newEvent = new QTouchEvent(QEvent::TouchEnd, Q_NULLPTR, Qt::NoModifier, Qt::TouchPointStates(), newList);
//    newEvent->setTarget(this->parent());
//    QCoreApplication::postEvent(this->parent(), newEvent);

    //event->accept();
    MainWindow * w = dynamic_cast<MainWindow*>(this->parent());
    event->setAccepted(true);
    //ui->label->hide();

    QPoint point = w->corectTouchCoordinates(event->touchPoints()[0].pos().x(), event->touchPoints()[0].pos().y());
    int x = point.x();
    int y = point.y();

    //ui->label->setGeometry(x-20, y-20, 40,40);

    INPUT mEvent2[2];

    mEvent2[0].type = INPUT_MOUSE;
    mEvent2[0].mi.dx =  w->normalizeMouseCoordinate(x, this->width());
    mEvent2[0].mi.dy =  w->normalizeMouseCoordinate(y, this->height());
    mEvent2[0].mi.dwExtraInfo = 0;
    mEvent2[0].mi.mouseData =   0;
    mEvent2[0].mi.time =        0;
    mEvent2[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

    mEvent2[1].type = INPUT_MOUSE;
    mEvent2[1].mi.dx =  w->normalizeMouseCoordinate(x, this->width());
    mEvent2[1].mi.dy =  w->normalizeMouseCoordinate(y, this->height());
    mEvent2[1].mi.dwExtraInfo = 0;
    mEvent2[1].mi.mouseData =   0;
    mEvent2[1].mi.time =        0;
    mEvent2[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;


    SendInput(2, mEvent2, sizeof(INPUT));
}

bool TouchFilter::eventFilter(QObject *object, QEvent *event)
{
    if ((event->type() == QEvent::MouseButtonPress)||(event->type() == QEvent::MouseButtonRelease)||(event->type() == QEvent::MouseMove)) {
        qDebug()<<"touchFilter eventFilter";
        return true;

    }
    return false;
}
