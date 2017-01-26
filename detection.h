#ifndef DETECTION_H
#define DETECTION_H

#include <QWidget>
#include <QThread>
#include <QFile>
#include <QDomDocument>

#include <Windows.h>
#include <Kinect.h>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <QDebug>

#include <QImage>
#include <QPixmap>
#include <settings.h>

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
    explicit Detection(QWidget *parent = 0);
    void run();
    cv::Point2i coordinateConversion(int x, int y);
    cv::Point2i coordinateBackConvertion(int x, int y);
    ~Detection();

    void setSettings(Settings *value);
    void initPlayersPoints();

public slots:
    int playerOnCoordinates(int x, int y);

signals:
    void newCoordinates(int x, int y, bool inTouchArea);
    void newRawCoordinates(int x, int y, bool inTouchArea);

private:
    void inputCorection();

    Ui::Detection *ui;
    Settings *settings;

    /// <kinect>
    IDepthFrameReader*  d_reader;     // Kinect depth data source
    IColorFrameReader*  c_reader;     // Kinect color data source

    bool initKinect();
    void d_getKinectData();
    ///</kinect>

    ///Player positions
    std::vector<std::vector<cv::Point>> playersPoins; //prvni vector je vector hracu, druhy vector bodu patrici hraci

    std::vector<std::vector<cv::Point>> playersHulls; //konvexHulls jednotlivych hracu

    UINT16* d_buffer  = NULL;
    BYTE*   BGRADepth = NULL;
    Mat d_mat;

    void d_buffer2mat();
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
