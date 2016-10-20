#include "mainwindow.h"
#include "detection.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();


    Detection detection;
    DetectionThread detectionThread(&detection);
    detectionThread.start();
    //detection.show(); // todo: dodelat posuvniky pro nastaveni detekce
    QObject::connect(&detection, SIGNAL(newCoordinates(int,int)), &w, SLOT(coordinatesFromDetection(int,int)) );


    return a.exec();
}
