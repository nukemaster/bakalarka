#include "mainwindow.h"
#include "board.h"
#include "tile.h"
#include "settings.h"
#include "detection.h"
#include "calibration.h"
#include <QApplication>

#include <QDebug>

#include "myeventfilter.h"
#include "kinect.h"

int main(int argc, char *argv[])
{
    Settings * settings = new Settings();

    QApplication a(argc, argv);
    a.setStyleSheet("QGraphicsView { Background: url(:/Img/Img/old-paper-texture.jpg)}"
                    "QPushButton {}"
                    "RadialMenu2 QPushButton  {color: blue}");

    MainWindow w;
    w.setSettings(settings);
    w.setFullScreen();
//    w.show();

    Board board;
    w.columns = board.columns;
    w.rows = board.rows;

    QObject::connect(&board, SIGNAL(sendStateChange(int)),
                     &w, SLOT(getBoardState(int)) );
    QObject::connect(&board, SIGNAL(sendPixmapItem(QGraphicsPixmapItem *)),
                     &w, SLOT(getPixmapItem(QGraphicsPixmapItem *)) );
    QObject::connect(&board, SIGNAL(sendRangeItem(QGraphicsItem*)),
                     &w, SLOT(getRangeItem(QGraphicsItem *)) );
    QObject::connect(&board, SIGNAL(sendTileTypesNames(QVector<QString>)),
                     &w, SLOT(getTileTypes(QVector<QString>)) );

    QObject::connect(&w, SIGNAL(sendNewUnit(QString)),
                     &board, SLOT(getNewUnit(QString)) );
    QObject::connect(&w, SIGNAL(sendDMMode(bool)),
                     &board, SLOT(getDMMode(bool)));
    QObject::connect(&w, SIGNAL(sendMap(QString)),
                     &board, SLOT(getMapFileName(QString)));
    QObject::connect(&w, SIGNAL(sendStartCombat()),
                     &board, SLOT(getStartCombat()));
    QObject::connect(&w, SIGNAL(sendEndTurn()),
                     &board, SLOT(getEndTurn()));
    QObject::connect(&w, SIGNAL(sendMove()),
                     &board, SLOT(getMove()));
    QObject::connect(&w, SIGNAL(sendNewMap(int,int)),
                     &board, SLOT(getNewEmptyMap(int,int)));
    QObject::connect(&w, SIGNAL(roll(int,int)),
                     &board, SLOT(roll(int,int)));

    board.loadDefaultPixmaps();

    Detection* detection = new Detection();
    detection->setSettings(settings);
    DetectionThread* detectionThread = new DetectionThread(detection);
    detectionThread->start();
//    QObject::connect(detection, SIGNAL(newCoordinates(int,int, bool)),
//                     &w, SLOT(coordinatesFromDetection(int,int, bool)) );
    QObject::connect(detection, SIGNAL(newCoordinates(int,int, bool)),
                     &w, SLOT(coordinatesFromDetection2(int,int, bool)) );
    QObject::connect( &w, SIGNAL(playerIdOnXY(int,int)),
                                detection, SLOT(playerOnCoordinates(int,int)));

    Calibration* calibration = new Calibration();
    calibration->setSettings(settings);
//    calibration->show();
    QObject::connect(detection, SIGNAL(newRawCoordinates(int,int,bool)),
                     calibration, SLOT(coordinatesFromDetection(int,int,bool)));
    QObject::connect(detection, SIGNAL(sendMat(cv::Mat*)),
                     calibration, SLOT(getMat(cv::Mat*)));
    QObject::connect(&w, SIGNAL(showCalibration()),
                     calibration, SLOT(show()));

    int ret = a.exec();

    delete calibration;
    delete detectionThread;
    delete settings;


    return ret;
}
