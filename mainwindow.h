#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QGraphicsPixmapItem>
#include <QGraphicsEllipseItem>
#include <QFileDialog>

#include <QMouseEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>

#include <QDebug>

#include "tile.h"

#define DEFAULT_SPEED_COST 5

namespace Ui {
class MainWindow;
}

class myScene : public QGraphicsScene
{
public:

    ///DRAWING
    int drawMode = 0;
    QPointF origPoint;
    QGraphicsLineItem* itemToDraw = NULL;

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
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



public slots:
    void coordinatesFromDetection(int x, int y);
    void getPixmapItem(QGraphicsPixmapItem * item);
    void getRangeItem(QGraphicsEllipseItem * item);

private slots:

    void getBoardState(int state);

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

    void on_btn_editMap_wall_clicked();

    void on_btn_editMap_end_clicked();

    void on_btn_editMap_empty_clicked();

signals:
    void sendNewUnit();
    void sendMap(QString mapFileName);
    void sendDMMode(bool val);
    void sendStartCombat();
    void sendEndTurn();
    void sendMove();
    void sendNewMap(int x, int y);

private:
    Ui::MainWindow *ui;
    QGraphicsScene * scene;

};

#endif // MAINWINDOW_H
