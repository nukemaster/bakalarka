#include "mainwindow.h"
#include "board.h"
//#include "tile.h"
#include "settings.h"
#include "detection.h"
//#include "calibration.h"
#include <QApplication>

#include <QDebug>

#include "myeventfilter.h"
#include "kinect.h"
#include "action.h"

MainWindow* Action::mainWindow = 0;

int main(int argc, char *argv[])
{
    Settings * settings = new Settings();
//    MyEventFilter* eventFiter = new MyEventFilter();

    QApplication a(argc, argv);
    a.setStyleSheet("QGraphicsView { Background: url(:/Img/Img/old-paper-texture.jpg); padding: 10px}"
                    "MainWindow {Background: url(:/Img/Img/table_texture.jpg)}"
                    "QPushButton {}"
                    "RadialMenu2 QPushButton  {font-weight: bold ; color: white; padding: 0px; border: 0px; background-image: url(:/icons/icons/emptyButton.png);}");
//a.installEventFilter(eventFiter);

    MainWindow w;
    Action::mainWindow = &w;
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
    QObject::connect(&w, &MainWindow::sSaveMap,
                     [&board](){board.saveMap();});
    QObject::connect(&w, SIGNAL(sendEndTurn()),
                     &board, SLOT(getEndTurn()));
    QObject::connect(&w, SIGNAL(sendMove()),
                     &board, SLOT(getMove()));
    QObject::connect(&w, SIGNAL(sendNewMap(int,int)),
                     &board, SLOT(getNewEmptyMap(int,int)));
    QObject::connect(&w, SIGNAL(roll(int,int)),
                     &board, SLOT(roll(int,int)));
    QObject::connect(&w, SIGNAL(getMapTile()),
                     &board, SLOT(getMapTile()));
    QObject::connect(&w, SIGNAL(getUnitTile()),
                     &board, SLOT(getUnitTile()));

    board.loadDefaultPixmaps();

    Detection* detection = new Detection();
    detection->setSettings(settings);
    DetectionThread* detectionThread = new DetectionThread(detection);
    detectionThread->start();
//    QObject::connect(detection, SIGNAL(newCoordinates(int,int, bool)),
//                     &w, SLOT(coordinatesFromDetection(int,int, bool)) );
    QObject::connect(detection, SIGNAL(newCoordinates(int,int, int)),
                     &w, SLOT(coordinatesFromDetection2(int,int, int)) );
    QObject::connect( &w, SIGNAL(playerIdOnXY(int,int)),
                                detection, SLOT(playerOnCoordinates(int,int)));


    int ret = a.exec();

    delete detectionThread;
    delete settings;


    return ret;
}
