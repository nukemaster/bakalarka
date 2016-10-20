#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
