#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <QWidget>
#include <QPixmap>
#include <QEventLoop>
#include <QPointF>
#include <QXmlStreamWriter>

#include <QTouchEvent>

#include <detection.h>
#include <settings.h>

#include <QDebug>

#define SETTINGS "detection.xml"
#define INPUT_WIDTH 512
#define INPUT_HEIGHT 424

namespace Ui {
class Calibration;
}

class Calibration : public QWidget
{
    Q_OBJECT

public:
    explicit Calibration(QWidget *parent = 0);
    ~Calibration();
    void sendInputCutValues();
    void sendTouchAreaValues();
    void setValues();

    Detection* detection;
    DetectionThread* detectionThread;

    bool recordClick = false;
    double recordedX = _Nan._Double;
    int recordedN = 0;
    double recordedY = _Nan._Double;
    double tmpX;
    double tmpY;

    double kX = 1;
    double kY = 1;
    int deltaX = 0;
    int deltaY = 0;

    int touchXmod = 0;
    int touchYmod = 0;
    double touchKX = 1;
    double touchKY = 1;
    int touchWidth = 1920;
    int touchHeight = 1080;

    QPointF  inPoint1;
    QPointF  inPoint2;
    QPointF  inPoint3;

    QPointF  outPoint1;
    QPointF  outPoint2;
    QPointF  outPoint3;

    QPointF outTouchPoint1;
    QPointF outTouchPoint2;

    QPointF inTouchPoint1;
    QPointF inTouchPoint2;


    void setSettings(Settings *value);

public slots:
    void coordinatesFromDetection(int x ,int y , bool clicked);


private:
    Settings *settings;
    Ui::Calibration *ui;

    void setUiElements();
    QEventLoop* waitOnPoint;
    QEventLoop* waitOnTouchPoint;
    QPointF tmpPoint;

signals:
    void pointRecorded();

protected:
    bool event(QEvent * event);
    void touchEvent(QTouchEvent *event);
private slots:
    void on_slider_inputCut_Y1_valueChanged(int value);
    void on_slider_inputCut_X1_valueChanged(int value);
    void on_slider_inputCut_Y2_valueChanged(int value);
    void on_slider_inputCut_X2_valueChanged(int value);
    void on_slider_touchArea_X1_valueChanged(int value);
    void on_slider_touchArea_Y2_valueChanged(int value);
    void on_slider_touchArea_X2_valueChanged(int value);
    void on_slider_touchArea_Y1_valueChanged(int value);
    void on_slider_inpicCut_maxDepth_valueChanged(int value);
    void on_slider_touchArea_depth_valueChanged(int value);
    void on_btn_calibration_P1_clicked();
    void on_doubleSpinBox_valueChanged(double arg1);
    void on_doubleSpinBox_2_valueChanged(double arg1);
    void on_pushButton_clicked();
    void on_btn_calibration_P2_clicked();
    void on_btn_calibration_P3_clicked();
    void on_btn_save_clicked();
    void on_btn_calibration_tP1_clicked();
    void on_btn_calibration_tP2_clicked();
};

#endif // CALIBRATION_H
