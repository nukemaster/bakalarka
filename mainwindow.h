#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

public slots:
    void coordinatesFromDetection(int x, int y);

signals:
    void test(int x, int y);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
