#include "mainwindow.h"
#include "ui_mainwindow.h"

void myScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (this->drawMode) {
        if (itemToDraw != NULL) {
            this->removeItem(itemToDraw);
            delete itemToDraw;
            itemToDraw = NULL;
        }
        itemToDraw = new QGraphicsLineItem;
        itemToDraw->setPen(QPen(Qt::black, 3, Qt::SolidLine));
        itemToDraw->setLine(origPoint.x(), origPoint.y(),event->scenePos().x(),event->scenePos().y());
        this->addItem(itemToDraw);
    }
}

void myScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (this->drawMode)
        this->origPoint = event->scenePos();
    QGraphicsScene::mousePressEvent(event);
}

void myScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (this->drawMode && itemToDraw != NULL) {
        QList<QGraphicsItem *> collides = itemToDraw->collidingItems();
        for (int i = 0; i < collides.size(); i++)
        {
            if (TileMap* tmp = dynamic_cast<TileMap *> (collides[i])) {
                if (drawMode == 1) { //wall
                    tmp->changeTo("wall");
                    tmp->blocking = true;
                }
                else if (drawMode == 2) { //empty
                    tmp->changeTo("empty");
                    tmp->blocking = false;
                }
            }
        }


        this->removeItem(itemToDraw);
        delete itemToDraw;
        itemToDraw = NULL;
    }
    QGraphicsScene::mouseReleaseEvent(event);
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->scene = new myScene();
    this->ui->graphicsView->setScene(this->scene);
    this->ui->graphicsView->setGeometry(0,0, this->width(), this->height());

    this->hideAll();
    this->ui->box_startMenu->show();
}

MainWindow::~MainWindow()
{
    delete ui;

}

void MainWindow::hideAll()
{
    ui->menuDM->hide();
    ui->pushButtonDM->hide();
    //ui->pushButtonMapLoad->hide();
    ui->box_startMenu->hide();
    //ui->pushButtonStartCombat->hide();
    ui->widgetPlayer->hide();
    ui->box_newEmptyMapSetting->hide();
    ui->box_editMap->hide();
}

void MainWindow::setFullScreen()
{
    this->showFullScreen();
    this->ui->graphicsView->setGeometry(0,0, this->width(), this->height());
}

void MainWindow::coordinatesFromDetection(int x, int y)
{
    ui->label->setText(QString::number(x));
    ui->label_2->setText(QString::number(y));
}

void MainWindow::getPixmapItem(QGraphicsPixmapItem *item)
{
    this->scene->addItem(item);
}

void MainWindow::getRangeItem(QGraphicsEllipseItem *item)
{
    this->scene->addItem(item);
}

void MainWindow::getBoardState(int state)
{
    this->ui->labelBoardState->setText(QString::number(state));
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

void MainWindow::on_pushButtonAddUnit_clicked()
{
    ///todo: nacteni souboru s jednotkou, preparsrovani souboru, vytvoreni jednotky a poslani
    this->sendNewUnit();
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
        tr("Open Map"), "", tr("Image Files (*.xml)"));
    this->sendMap(fileName);
}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         

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

void MainWindow::on_btn_editMap_wall_clicked()
{
    dynamic_cast<myScene *> (this->ui->graphicsView->scene())->drawMode = 1;
}

void MainWindow::on_btn_editMap_end_clicked()
{
    this->hideAll();
    dynamic_cast<myScene *> (this->ui->graphicsView->scene())->drawMode = 0 ;
    getBoardState(this->state);
}

void MainWindow::on_btn_editMap_empty_clicked()
{
    dynamic_cast<myScene *> (this->ui->graphicsView->scene())->drawMode = 2;
}

void MainWindow::on_btn_newMap_back_clicked()
{
    getBoardState(this->state);
}
