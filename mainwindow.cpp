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

    grabGesture(Qt::SwipeGesture);
    //grabGesture(Qt::TapGesture);

    //grabGesture(Qt::PanGesture);
    //setCursor(Qt::BlankCursor);

    this->setAttribute(Qt::WA_AcceptTouchEvents);
    this->installEventFilter(this);
    //ui->label->setPixmap(QPixmap(":/Img/Img/cursor_red.png"));
    //ui->label->setAttribute( Qt::WA_TransparentForMouseEvents );


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
    this->ui->graphicsView->setGeometry(0,0, QApplication::desktop()->screenGeometry().width(), QApplication::desktop()->screenGeometry().height());
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
