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


#define SETTINGS "detection.xml"

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
    bool settings();
    ~Detection();

signals:
    void newCoordinates(int x, int y);

private:
    Ui::Detection *ui;

    ///<Input Options>
    ///// orez
    int minDepth;
    int maxDepth;
    int x1;
    int x2;
    int y1;
    int y2;
    ///// touchArea
    int touchDepth;
    int touchAreaX1;
    int touchAreaX2;
    int touchAreaY1;
    int touchAreaY2;
    ///// input
    int inputDepthWidth;
    int inputDepthHeight;
    int inputDepthSize; //this->inputDepthWidth * this->inputDepthHeight
    ///</input Options>
    /// <CVProcessing>
    bool erode1 = false;
    bool dilate1 = false;
    bool blur = false;
    bool dilate2 = false;
    /// </CVProcessing>

    /// <kinect>
    IDepthFrameReader*  d_reader;     // Kinect depth data source
    IColorFrameReader*  c_reader;     // Kinect color data source

    bool initKinect();
    void d_getKinectData();
    ///</kinect>

    UINT16* d_buffer = NULL;
    BYTE*   BGRADepth = NULL;
    Mat d_mat;

    void d_buffer2mat();
    std::vector<cv::Point> cvDetect(Mat d_mat);
    bool isPointInTouchArea(Point point);
    int pointDepth(Point point);
};

class DetectionThread : public QThread
{
public:
    DetectionThread(Detection * detection);
    Detection * detection;

    void run();
};

#endif // DETECTION_H
