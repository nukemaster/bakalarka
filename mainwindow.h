#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QGraphicsPixmapItem>

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
    int columns;
    int rows;

public slots:
    void coordinatesFromDetection(int x, int y);
    void getPixmapItem(QGraphicsPixmapItem * item);

private slots:
    void on_pushButtonAddUnit_clicked();

signals:
    void sendNewUnit();

private:
    Ui::MainWindow *ui;
    QGraphicsScene * scene;

};

#endif // MAINWINDOW_H
