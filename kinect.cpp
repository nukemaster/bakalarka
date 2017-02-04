#include "kinect.h"

Kinect::Kinect()
{
    initKinect();
}

Kinect::~Kinect()
{
    dev->stop();
    dev->close();
}

bool Kinect::initKinect()
{
    if(freenect2.enumerateDevices() == 0)
    {
      qDebug() << "no device connected!";
      return false;
    }
    if (serial == "")
    {
      serial = freenect2.getDefaultDeviceSerialNumber();
    }

    if(pipeline)
    {
        //! [open]
        dev = freenect2.openDevice(serial, pipeline);
        //! [open]
    } else {
        dev = freenect2.openDevice(serial);
    }

    this->listener = new libfreenect2::SyncMultiFrameListener(libfreenect2::Frame::Depth);

    dev->setIrAndDepthFrameListener(listener);

    if (!dev->startStreams(false, true)) {
        return false;
    }
    return true;
}

cv::Mat Kinect::getDepthFrame()
{
    cv::Mat d_mat;

    if (loadKinectMatFromFile) {
        LoadMatBinary("saveMatBinary", d_mat);
        return d_mat;
    }

    if (serial == "" ) {
        initKinect();
    }

    listener->waitForNewFrame(frames);

    depth  = frames[libfreenect2::Frame::Depth];

    d_mat = cv::Mat(cv::Size(depth->width, depth->height), CV_32FC1, depth->data);

//    std::ofstream myfile;
//    myfile.open("matTest");
//    myfile << d_mat;
//    myfile.close();

//    loadKinectMatFromFile = true;
//    SaveMatBinary("saveMatBinary", d_mat);

//    *(d_mat) /=  4096.0f;
    //cv::imshow("Kinect", *d_mat);
    //sendDepthFrame(d_mat);

    listener->release(frames);

    return d_mat;
}


#include <fstream>

//! Write cv::Mat as binary
/*!
\param[out] ofs output file stream
\param[in] out_mat mat to save
*/
bool writeMatBinary(std::ofstream& ofs, const cv::Mat& out_mat)
{
    if(!ofs.is_open()){
        return false;
    }
    if(out_mat.empty()){
        int s = 0;
        ofs.write((const char*)(&s), sizeof(int));
        return true;
    }
    int type = out_mat.type();
    ofs.write((const char*)(&out_mat.rows), sizeof(int));
    ofs.write((const char*)(&out_mat.cols), sizeof(int));
    ofs.write((const char*)(&type), sizeof(int));
    ofs.write((const char*)(out_mat.data), out_mat.elemSize() * out_mat.total());

    return true;
}


//! Save cv::Mat as binary
/*!
\param[in] filename filaname to save
\param[in] output cvmat to save
*/
bool SaveMatBinary(const std::string& filename, const cv::Mat& output){
    std::ofstream ofs(filename, std::ios::binary);
    return writeMatBinary(ofs, output);
}


//! Read cv::Mat from binary
/*!
\param[in] ifs input file stream
\param[out] in_mat mat to load
*/
bool readMatBinary(std::ifstream& ifs, cv::Mat& in_mat)
{
    if(!ifs.is_open()){
        return false;
    }

    int rows, cols, type;
    ifs.read((char*)(&rows), sizeof(int));
    if(rows==0){
        return true;
    }
    ifs.read((char*)(&cols), sizeof(int));
    ifs.read((char*)(&type), sizeof(int));

    in_mat.release();
    in_mat.create(rows, cols, type);
    ifs.read((char*)(in_mat.data), in_mat.elemSize() * in_mat.total());

    return true;
}


//! Load cv::Mat as binary
/*!
\param[in] filename filaname to load
\param[out] output loaded cv::Mat
*/
bool LoadMatBinary(const std::string& filename, cv::Mat& output){
    std::ifstream ifs(filename, std::ios::binary);
    return readMatBinary(ifs, output);
}
