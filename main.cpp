#include "mainwindow.h"
#include "detection.h"
#include "board.h"
#include "tile.h"
#include <QApplication>


#include <QDebug>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setFullScreen();

    Board board;
    w.columns = board.columns;
    w.rows = board.rows;

    QObject::connect(&board, SIGNAL(sendStateChange(int)), &w, SLOT(getBoardState(int)) );
    QObject::connect(&board, SIGNAL(sendPixmapItem(QGraphicsPixmapItem *)), &w, SLOT(getPixmapItem(QGraphicsPixmapItem *)) );

    QObject::connect(&w, SIGNAL(sendNewUnit()), &board, SLOT(getNewUnit()) );
    QObject::connect(&w, SIGNAL(sendDMMode(bool)), &board, SLOT(getDMMode(bool)));
    QObject::connect(&w, SIGNAL(sendMap(QString)), &board, SLOT(getMapFileName(QString)));
    QObject::connect(&w, SIGNAL(sendStartCombat()), &board, SLOT(getStartCombat()));
    QObject::connect(&w, SIGNAL(sendEndTurn()), &board, SLOT(getEndTurn()));
    QObject::connect(&w, SIGNAL(sendMove()), &board, SLOT(getMove()));


    //board.loadMap();

/*
    Detection detection;
    DetectionThread detectionThread(&detection);
    detectionThread.start();
    //detection.show(); // todo: dodelat posuvniky pro nastaveni detekce
    QObject::connect(&detection, SIGNAL(newCoordinates(int,int)), &w, SLOT(coordinatesFromDetection(int,int)) );
*/

    return a.exec();
}
