#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->scene = new QGraphicsScene();
    this->ui->graphicsView->setScene(this->scene);
    this->ui->graphicsView->setGeometry(0,0, this->width(), this->height());

    this->hideAll();
    this->ui->pushButtonMapLoad->show();
}

MainWindow::~MainWindow()
{
    delete ui;

}

void MainWindow::hideAll()
{
    ui->menuDM->hide();
    ui->pushButtonDM->hide();
    ui->pushButtonMapLoad->hide();
    //ui->pushButtonStartCombat->hide();
    ui->widgetPlayer->hide();
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

void MainWindow::getBoardState(int state)
{
    this->ui->labelBoardState->setText(QString::number(state));

    switch (state) {
    case 1:
        this->hideAll();
        this->ui->pushButtonDM->show();
        //delete this->ui->pushButtonStartCombat;
        this->ui->widgetPlayer->show();
        break;
    case 0:
        this->hideAll();
        this->ui->pushButtonMapLoad->show();
        break;
    case -1:
        this->hideAll();
        this->ui->pushButtonDM->show();
        this->ui->menuDM->show();
        break;
    case -2:
        this->hideAll();
        break;
    }

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
