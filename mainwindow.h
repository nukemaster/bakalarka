#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QGraphicsPixmapItem>
#include <QGraphicsEllipseItem>
//#include <QFileDialog>

#include <QMouseEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>

#include <QDebug>

#include <QPushButton>
#include <QString>
#include <QWidget>

#include <QThread>

#include <QTimer>

#include <QGestureEvent>

//#include "tile.h"
#include "tileunit.h"

#include "radialmenu2.h"
#include "myscene.h"
#include "settings.h"
#include "dicebutton.h"
#include "filedialog.h"

#include "QPaintEngine"
#include "QTransform"

#include <QDesktopWidget>

#include <QGraphicsProxyWidget>

#include <QDir>
#include <QScroller>

#include "action.h"

#include <QEventLoop>

#define DEFAULT_SPEED_COST 5

#define GRAPHICVIEW_BORDER_SIZE 100 //velikost okraje kolem mapy

class TileUnit;
class TileSelectButton;

class QApplication;

namespace Ui {
class MainWindow;
}

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
    void addTextToPlayerInfoBox(QString str, int playerID);

    QVector <TileSelectButton *> editMapBtns;
    QVector <QVector <RadialMenu2*>> playersRadialMenus;
    void displayRadialMenu(bool show);
    QVector <DiceMenu *> diceMenus;
    QVector <QLabel *> playersInfoPanels;
    QVector <QPushButton*> playerOKButton;
    QVector <QVector<QPushButton*>> smallBtn;
    void showSmallBtn(int playerID);
    void hideSmallBtn();

    QPixmap cursorPixmap;
    QPixmap cursorPixmapClick;

    bool mousePressed = false;
    long normalizeMouseCoordinate(int coor, int maxCoor);


    void setSettings(Settings *value);
//    QPoint corectTouchCoordinates(int x, int y);

    int dungeonMaster = 2;

    void styleToFantasyBtn(QPushButton* btn, int x = -1, int y = -1);

    QGraphicsScene * scene;

    QEventLoop* waitOnUnitClick;
    TileUnit *tmpUnit = NULL;
    TileUnit *selectUnit(int playerID);


public slots:
    void coordinatesFromDetection2(const int x, const int y, const int playerID);
    void getPixmapItem(QGraphicsPixmapItem * item);
    void getRangeItem(QGraphicsItem * item);
    void getBoardState(int state);

private slots:    
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
    int getRoll(int n, int t);

    void on_btn_saveMap_pressed();

    void on_btn_hideShowEditMap_pressed();

signals:
    void sendNewUnit(QString unitFileName);
    void sendMap(QString mapFileName);
    void sendDMMode(bool val);
    void sendStartCombat();
    void sendEndTurn();
    void sendMove();
    void sendNewMap(int x, int y);
    void sSaveMap();

    int playerIdOnXY(int x, int y);
    int roll(int dNumbet, int dType);
    int showCalibration();

    void playerOKPressed(int playerID);
    void smallButtonPressed(int playerID, int btnID);
    void playerOnBoard(int playerID);

    TileMap * getMapTile();
    TileUnit * getUnitTile();

private:
    Ui::MainWindow *ui;    
    QCursor * cursorBlue;
    QCursor * cursorRed;

protected:
    Settings * settings;
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
//    void gestureEvent(QGestureEvent *event);
    bool event(QEvent * event);

    void touchBeginEvent(QTouchEvent *event);
    void touchUpdateEvent(QTouchEvent *event);
    void touchEndEvent(QTouchEvent *event);
    bool touchEvent(QTouchEvent* event);

    void createRollMenus();
    void createRollMenu(int playerID, QString panelText);

};

#endif // MAINWINDOW_H
