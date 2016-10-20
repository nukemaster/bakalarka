#include "detection.h"
#include "ui_detection.h"

Detection::Detection(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Detection)
{
    ui->setupUi(this);


    this->settings();
    //inicializace bufferu
    this->inputDepthSize = this->inputDepthWidth * this->inputDepthHeight;
    this->d_buffer  = new UINT16 [this->inputDepthSize];
    this->BGRADepth = new BYTE [this->inputDepthSize * 4];
    this->d_mat = Mat::zeros(Size(this->inputDepthWidth, this->inputDepthHeight), CV_8UC4);

    this->initKinect();
}

void Detection::run()
{
    std::vector<cv::Point> points;

    this->d_getKinectData();
    points = this->cvDetect(d_mat);

    if (this->isPointInTouchArea(points[0])) {
        newCoordinates(points[0].x, points[0].y);
    }
    else {
        newCoordinates(-1, -1);
    }

    //imshow("depthDetection", this->d_mat) ;
    //waitKey(1);

}

bool Detection::settings()
{
    QFile file(SETTINGS);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return false;

    QDomDocument xml("setting");
    xml.setContent(&file);
    QDomElement e, e0;

    e0 = xml.firstChildElement("setting");

    e = e0.firstChildElement("orez");
    this->minDepth = e.firstChildElement("minDepth").toElement().text().toInt();
    this->maxDepth = e.firstChildElement("maxDepth").toElement().text().toInt();
    this->x1 = e.firstChildElement("x1").toElement().text().toInt();
    this->y1 = e.firstChildElement("y1").toElement().text().toInt();
    this->x2 = e.firstChildElement("x2").toElement().text().toInt();
    this->y2 = e.firstChildElement("y2").toElement().text().toInt();

    e = e0.firstChildElement("touchArea");
    this->touchDepth  = e.firstChildElement("depth").toElement().text().toInt();
    this->touchAreaX1 = e.firstChildElement("x1").toElement().text().toInt();
    this->touchAreaY1 = e.firstChildElement("y1").toElement().text().toInt();
    this->touchAreaX2 = e.firstChildElement("x2").toElement().text().toInt();
    this->touchAreaY2 = e.firstChildElement("y2").toElement().text().toInt();

    e = e0.firstChildElement("CVProcessing");
    this->erode1   = (e.firstChildElement("erode").toElement().text() == "true") ? true : false;
    this->dilate1 = (e.firstChildElement("dilate1").toElement().text() == "true") ? true : false;
    this->blur    = (e.firstChildElement("blur").toElement().text() == "true") ? true : false;
    this->dilate2 = (e.firstChildElement("dilate2").toElement().text() == "true") ? true : false;

    e = e0.firstChildElement("input");
    this->inputDepthWidth  = e.firstChildElement("width").toElement().text().toInt();
    this->inputDepthHeight = e.firstChildElement("height").toElement().text().toInt();

    file.close();

    return true;

}

bool Detection::initKinect()
{
    IKinectSensor*      sensor;
    if (FAILED(GetDefaultKinectSensor(&sensor)))
        return false;
    if (sensor) {
        sensor->Open();
        //get depth frame source
        IDepthFrameSource* d_framesource = NULL;
        sensor->get_DepthFrameSource(&d_framesource);
        d_framesource->OpenReader(&(this->d_reader));
        // get color frame source
        IColorFrameSource* c_framesource = NULL;
        sensor->get_ColorFrameSource(&c_framesource);
        c_framesource->OpenReader(&(this->c_reader));

        if (d_framesource) {
            d_framesource->Release();
            d_framesource = NULL;
        }
        if (c_framesource) {
            c_framesource->Release();
            c_framesource = NULL;
        }
        return true;
    } else
        return false;
}

void Detection::d_getKinectData()
{
    IDepthFrame* frame = NULL;

    if (SUCCEEDED(this->d_reader->AcquireLatestFrame(&frame))) {
        frame->CopyFrameDataToArray(this->inputDepthSize, this->d_buffer);
        frame->Release();
    }

    this->d_buffer2mat();
}

void Detection::d_buffer2mat()
{
    for (int x = 0; x < this->inputDepthWidth; x++)
    {
        for (int y = 0; y < this->inputDepthHeight; y++)
        {
            int index = y * this->inputDepthWidth + x;
            if ((y<y1)||(y>y2)||(x<x1)||(x>x2))
            {
                index *= 4;
                this->BGRADepth[index]    = 0xff;
                this->BGRADepth[index+1]  = 0xff;
                this->BGRADepth[index+2]  = 0xff;
                this->BGRADepth[index+3]  = 0xff;
                continue;
            }
            INT16 pom = this->d_buffer[index];
            if ((pom<maxDepth)&&(pom>minDepth)){
                pom = pom-minDepth;
                index *= 4;
                this->BGRADepth[index] = (BYTE)(pom % 256);
                if (pom > 255)
                    this->BGRADepth[index+1] = (BYTE)(pom - (pom % 256));
                else
                    this->BGRADepth[index+1] = 0x00;
                this->BGRADepth[index+2] = 0x00;
                this->BGRADepth[index+3] = 0xff;
            }
            else
                for (int i = 0; i < 4; ++i)
                    this->BGRADepth[index*4+i] = 0xff;
        }
    }
    this->d_mat = Mat(Size(this->inputDepthWidth, this->inputDepthHeight), CV_8UC4, this->BGRADepth, Mat::AUTO_STEP);
}

std::vector<cv::Point> Detection::cvDetect(Mat d_mat)
{
    vector<Vec4i> hierarchy;
    vector<vector<Point> > contours;
    std::vector<cv::Point> validPoints;
    Size d_size = Size(this->inputDepthWidth, this->inputDepthHeight);
    Mat drawing = Mat::zeros( d_size, CV_8UC3 ); //vytvoreni plochy pro vykresleni vysledky hledani

    cv::rectangle(drawing, cv::Point(touchAreaX1, touchAreaY1), cv::Point(touchAreaX2, touchAreaY2), cv::Scalar(255, 255, 0));      //vykreslen oblasti dotyku

    cv::inRange(d_mat, Scalar(0, 0, 0, 0), Scalar(255, 0xa3, 1, 255), d_mat);
    //imshow("depthDetection", d_mat);
    //waitKey(1);



    //odstranení šumu
    int erosion_size = 1;
    int blurSize = 5;
    int elementSize = 5;
    Mat erode_element = getStructuringElement( MORPH_ELLIPSE, Size( 2*erosion_size + 1, 2*erosion_size+1 ), Point( erosion_size, erosion_size ) );
    cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(2 * elementSize + 1, 2 * elementSize + 1), cv::Point(elementSize, elementSize));

    if (this->erode1) erode( d_mat, d_mat, erode_element );
    if (this->blur) cv::medianBlur(d_mat, d_mat, blurSize);
    if (this->dilate2) cv::dilate(d_mat, d_mat, element);
    if (this->dilate1) dilate( d_mat, d_mat, erode_element );

    //cv::imshow("šum", d_mat);

    //find contours
    cv::findContours(d_mat, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

    size_t largestContour = 0;
    for (size_t i = 1; i < contours.size(); i++)
    {
        if (cv::contourArea(contours[i]) > cv::contourArea(contours[largestContour]))
        largestContour = i;
    }
    cv::drawContours(drawing, contours, largestContour, cv::Scalar(0, 0, 255), 1);



    if (!contours.empty())
    {
        std::vector<std::vector<cv::Point> > hull(1);
        cv::convexHull(cv::Mat(contours[largestContour]), hull[0], false);
        cv::drawContours(drawing, hull, 0, cv::Scalar(0, 255, 0), 1);

        if (hull[0].size() > 2)
        {
            std::vector<int> hullIndexes;
            cv::convexHull(cv::Mat(contours[largestContour]), hullIndexes, true);
            std::vector<cv::Vec4i> convexityDefects;
            cv::convexityDefects(cv::Mat(contours[largestContour]), hullIndexes, convexityDefects);
            cv::Rect boundingBox = cv::boundingRect(hull[0]);
            cv::rectangle(drawing, boundingBox, cv::Scalar(75, 0, 0));
            cv::Point center = cv::Point(boundingBox.x + boundingBox.width / 2, boundingBox.y + boundingBox.height / 2);

            int maxArea = 0;    //ulozeni plochy nejvetsiho zatim nalezeneho konvexniho defektu
            cv::Point po1, po2, po3;    //ulozeni vrcholu nejvetsiho konvexniho defektu
            for (size_t i = 0; i < convexityDefects.size(); i++)
            {
                cv::Point p1 = contours[largestContour][convexityDefects[i][0]];
                cv::Point p2 = contours[largestContour][convexityDefects[i][1]];
                cv::Point p3 = contours[largestContour][convexityDefects[i][2]];
                cv::line(drawing, p1, p3, cv::Scalar(255, 0, 0), 1);
                cv::line(drawing, p3, p2, cv::Scalar(255, 0, 0), 1);

                //pocitani obsahu konvexniho defektu
                int a = cv::norm(p1-p2);
                int b = cv::norm(p2-p3);
                int c = cv::norm(p3-p1);
                int s = (a+b+c)/2;
                s = sqrt(s*(s-a)*(s-b)*(s-c));
                //hledani nejvetsiho konvexniho defektu
                if (s > maxArea) {
                    maxArea = s;
                    po1 = p1;
                    po2 = p2;
                    po3 = p3;
                }
                /*
                double angle = std::atan2(center.y - p1.y, center.x - p1.x) * 180 / CV_PI;
                double inAngle = Detection::innerAngle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
                double length = std::sqrt(std::pow(p1.x - p3.x, 2) + std::pow(p1.y - p3.y, 2));
                if (angle > -30 && angle < 160 && std::abs(inAngle) > 20 && std::abs(inAngle) < 120 && length > 0.1 * boundingBox.height)
                {
                    validPoints.push_back(p1);
                }
                */
            }

            //vykresleni vrcholu nejvetsiho konvexniho defektu
            cv::circle(drawing, po1, 12, cv::Scalar(0, 255, 255), 1);
            cv::circle(drawing, po2, 12, cv::Scalar(0, 255, 255), 1);
            cv::circle(drawing, po3, 12, cv::Scalar(0, 255, 255), 1);
            validPoints.push_back(po1);
            validPoints.push_back(po2);
            validPoints.push_back(po3);


            //kontrola zda neni bod v oblasti deteku
            for (unsigned int i = 0; i < validPoints.size(); i++) {
                if (!isPointInTouchArea(validPoints[i])) {
                    //validPoints.erase(validPoints.begin()+i);
                    continue;
                }
                if (pointDepth(validPoints[i]) > touchDepth) {  //kontrola zda bod nelezi v pod hloubkou pro dotyk
                    cv::circle(drawing, validPoints[i], 7, cv::Scalar(0, 0, 255), 2);
                }
                cv::circle(drawing, validPoints[i], 9, cv::Scalar(0, 255, 0), 2);
            }
            for (size_t i = 0; i < validPoints.size(); i++)
            {
                //cv::circle(drawing, validPoints[i], 9, cv::Scalar(0, 255, 0), 2);
            }
        }
    }


    cv::imshow("windowName2", drawing);
    waitKey(1);

    return validPoints;
}

bool Detection::isPointInTouchArea(Point point)
{
    if ((point.x > touchAreaX1)&&(point.x < touchAreaX2)) {
        if ((point.y > touchAreaY1)&&(point.y < touchAreaY2)) {
            if (this->pointDepth(point) > this->touchDepth)
                return true;
        }
    }
    return false;
}

int Detection::pointDepth(Point point)
{
    return d_buffer[point.x + point.y * this->inputDepthWidth];
}

Detection::~Detection()
{
    delete ui;
}

DetectionThread::DetectionThread(Detection * detection)
{
    this->detection = detection;
}

void DetectionThread::run()
{
    while (1) {
        this->detection->run();
        usleep(30000);
    }
}

