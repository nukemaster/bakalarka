#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <windows.h>

#include <QMainWindow>
#include <QVector>
#include <QGraphicsPixmapItem>
#include <QGraphicsEllipseItem>
#include <QFileDialog>

#include <QMouseEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>

#include <QDebug>

#include <QPushButton>
#include <QString>
#include <QWidget>

#include <QThread>

#include <QGestureEvent>

#include "tile.h"

#include "radialmenu.h"
#include "myscene.h"
#include "settings.h"

#define DEFAULT_SPEED_COST 5

class QApplication;

namespace Ui {
class MainWindow;
}

class TouchFilter : public QWidget
{
public:
    TouchFilter(QWidget *parent = 0);
protected:
    bool event(QEvent *event);
    void touchEvent(QTouchEvent *event);
    void touchUpdateEvent(QTouchEvent * event);
    void touchEndEvent(QTouchEvent * event);
    bool eventFilter(QObject * object, QEvent * event);
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void hideAll();
    int columns;
    int rows;
    int state = 0;
    void setFullScreen();

    QVector <TileSelectButton *> editMapBtns;

    QPixmap cursorPixmap;
    QPixmap cursorPixmapClick;

    bool mousePressed = false;
    long normalizeMouseCoordinate(int coor, int maxCoor);


    void setSettings(Settings *value);
    QPoint corectTouchCoordinates(int x, int y);

    int dungeonMaster = 2;

public slots:
    void coordinatesFromDetection(int x, int y, bool inClickArea);
    void coordinatesFromDetection2(int x, int y, bool inClickArea);
    void getPixmapItem(QGraphicsPixmapItem * item);
    void getRangeItem(QGraphicsItem * item);

private slots:

    void getBoardState(int state);
    void getTileTypes(QVector<QString> types);

    void on_pushButtonAddUnit_clicked();
    void on_pushButtonDM_clicked();
    void on_pushButtonMapLoad_clicked();
    void on_pushButtonStartCombat_clicked();
    void on_pushButtonEndTurn_clicked();
    void on_pushButtonMove_clicked();
    void on_slider_newMap_X_valueChanged(int value);
    void on_slider_newMap_Y_valueChanged(int value);
    void on_btn_newMap_create_clicked();
    void on_btn_newMap_clicked();
    void on_btn_editMap_menuToggle_clicked();
    void on_btn_editMap_end_clicked();
    void on_btn_newMap_back_clicked();

signals:
    void sendNewUnit(QString unitFileName);
    void sendMap(QString mapFileName);
    void sendDMMode(bool val);
    void sendStartCombat();
    void sendEndTurn();
    void sendMove();
    void sendNewMap(int x, int y);

    int playerIdOnXY(int x, int y);

private:
    Ui::MainWindow *ui;
    QGraphicsScene * scene;
    QCursor * cursorBlue;
    QCursor * cursorRed;

protected:
    Settings * settings;
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void gestureEvent(QGestureEvent *event);
    bool event(QEvent * event);
    bool eventFilter(QObject * watched, QEvent * event);

    void touchBeginEvent(QTouchEvent *event);
    void touchUpdateEvent(QTouchEvent *event);
    void touchEndEvent(QTouchEvent *event);

};

#endif // MAINWINDOW_H
