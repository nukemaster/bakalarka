#include "mainwindow.h"
#include "detection.h"
#include "board.h"
#include <QApplication>


#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    Board board;
    w.columns = board.columns;
    w.rows = board.rows;

    QObject::connect(&board, SIGNAL(sendPixmapItem(QGraphicsPixmapItem *)), &w, SLOT(getPixmapItem(QGraphicsPixmapItem *)) );

    board.loadMap();
qDebug()<<"konec";
    //QObject::connect(&board, SIGNAL(sendBoard(QVector<int>)), &w, SLOT(getMap(QVector<int>)) );

    //board.sendBoard(board.board);
    /*
    Detection detection;
    DetectionThread detectionThread(&detection);
    detectionThread.start();
    //detection.show(); // todo: dodelat posuvniky pro nastaveni detekce
    QObject::connect(&detection, SIGNAL(newCoordinates(int,int)), &w, SLOT(coordinatesFromDetection(int,int)) );
    */

    return a.exec();
}
