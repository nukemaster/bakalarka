#ifndef KINECT_H
#define KINECT_H

#include <QDebug>

#include <libfreenect2/libfreenect2.hpp>
#include <libfreenect2/frame_listener_impl.h>

#include "opencv2/core/core.hpp"
//#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <fstream>

bool SaveMatBinary(const std::string& filename, const cv::Mat& output);
bool LoadMatBinary(const std::string& filename, cv::Mat& output);

class Kinect : public QObject
{
    Q_OBJECT
public:
    Kinect();
    ~Kinect();
    bool initKinect();
    cv::Mat getDepthFrame();

    bool enable_rgb     = false;
    bool enable_ir      = false;
    bool enable_depth   = true;

signals:
//    bool sendDepthFrame(cv::Mat * d_mat);

protected:
    libfreenect2::Freenect2 freenect2;
    libfreenect2::Freenect2Device *dev = 0;
    libfreenect2::PacketPipeline *pipeline = 0;
    std::string serial = "";

    libfreenect2::SyncMultiFrameListener* listener;
    libfreenect2::FrameMap frames;


    libfreenect2::Frame *rgb;
    libfreenect2::Frame *ir;
    libfreenect2::Frame *depth;

    bool loadKinectMatFromFile = true; //debug

};

#endif // KINECT_H
