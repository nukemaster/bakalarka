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
}

MainWindow::~MainWindow()
{
    delete ui;

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

void MainWindow::on_pushButtonAddUnit_clicked()
{
    ///todo: nacteni souboru s jednotkou, preparsrovani souboru, vytvoreni jednotky a poslani
    this->sendNewUnit();
}
