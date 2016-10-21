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
    //qDebug()<<"test";
    //this->ui->graphicsView->invalidateScene();
    this->scene->addItem(item);
}
/*
void MainWindow::getMap(QVector<int> board)
{
    for (int x = 0; x < this->columns; x++) {
        for (int y = 0; y < this->rows; y++) {
            switch (board[y * this->rows + x]) {
            case 1: //zed

                break;
            default:    //neco nedefinovaneho = prazdne pole
                break;
            }
        }
    }
}
*/
