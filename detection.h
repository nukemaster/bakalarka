#ifndef DETECTION_H
#define DETECTION_H

#include <QWidget>
#include <QThread>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <QDebug>

#include "settings.h"
#include "kinect.h"

#include <iostream>

#define PLAYER_MAXIMUM 4

class Settings;

using namespace cv;
using namespace std;

namespace Ui {
class Detection;
}

class Detection : public QWidget
{
    Q_OBJECT

public:
    bool d_matMutex = false;
    explicit Detection(QWidget *parent = 0);
    void run();
    cv::Point2i coordinateConversion(int x, int y);
    cv::Point2i coordinateBackConvertion(int x, int y);
    ~Detection();

    void setSettings(Settings *value);
    void initPlayersPoints();

public slots:
    int playerOnCoordinates(int x, int y);
    bool getDepthFrame(cv::Mat* d_mat);

signals:
    void newCoordinates(int x, int y, int playerID);
    void newRawCoordinates(int x, int y, bool inTouchArea);
    void sendMat(cv::Mat* mat);

private:
    void inputCorection();

    Ui::Detection *ui;
    Settings *settings;

    /// <kinect>
    Kinect* kinect;


    ///Player positions
    std::vector<std::vector<cv::Point>> playersPoins; //prvni vector je vector hracu, druhy vector bodu patrici hraci

    std::vector<std::vector<cv::Point>> playersHulls; //konvexHulls jednotlivych hracu

    Mat d_mat;

    std::vector<cv::Point> cvDetect(Mat d_mat);
    bool isPointInTouchArea(Point point);
    bool isPointInTouchDepth(Point point);
    int pointDepth(Point point);
};

class DetectionThread : public QThread
{
public:
    DetectionThread(Detection * detection);
    ~DetectionThread();
    Detection * detection;

    bool stop = false;

    void run();
};

#endif // DETECTION_H
