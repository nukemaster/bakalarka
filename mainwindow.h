#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QGraphicsPixmapItem>
#include <QFileDialog>

#include <QDebug>

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

public slots:
    void coordinatesFromDetection(int x, int y);
    void getPixmapItem(QGraphicsPixmapItem * item);

private slots:
    void getBoardState(int state);
    void on_pushButtonAddUnit_clicked();
    void on_pushButtonDM_clicked();

    void on_pushButtonMapLoad_clicked();

    void on_pushButtonStartCombat_clicked();

    void on_pushButtonEndTurn_clicked();

    void on_pushButtonMove_clicked();

signals:
    void sendNewUnit();
    void sendMap(QString mapFileName);
    void sendDMMode(bool val);
    void sendStartCombat();
    void sendEndTurn();

private:
    Ui::MainWindow *ui;
    QGraphicsScene * scene;

};

#endif // MAINWINDOW_H
