#include "detection.h"
#include "ui_detection.h"

Detection::Detection(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Detection)
{
    ui->setupUi(this);

    this->initKinect();
}


void Detection::run()
{
    std::vector<cv::Point> points;
    cv::Point2i point;

    this->d_getKinectData();
    points = this->cvDetect(d_mat);

    if (points.empty()) {
        newCoordinates(-1, -1, false);
        return;
    }
    point = coordinateConversion(points[0].x, points[0].y);
    if (this->isPointInTouchArea(points[0])) {
        if (this->isPointInTouchDepth(points[0])) {
            newCoordinates(point.x, point.y, true);
            newRawCoordinates(points[0].x, points[0].y, true);
        }
        else {
            newCoordinates(point.x, point.y, false);
            newRawCoordinates(points[0].x, points[0].y, false);
        }

    }
    else {
        newCoordinates(-1, -1, false);
    }


    cv::rectangle(this->d_mat,  cv::Point(settings->touchAreaX1, settings->touchAreaY1),
                                cv::Point(settings->touchAreaX2, settings->touchAreaY2),
                  cv::Scalar(255, 255, 0));      //vykreslen oblasti dotyku
    imshow("depthDetection", this->d_mat) ;
    waitKey(1);

}

/**
 * @brief Detection::coordinateConversion prevod z kinektu na body obrazu
 * @param x
 * @param y
 * @return
 */
Point2i Detection::coordinateConversion(int x, int y)
{
    cv::Point2i point;
    point.x = x / settings->conversionKX + settings->conversionDeltaX / settings->conversionKX;
    point.y = y / settings->conversionKY + settings->conversionDeltaY / settings->conversionKY;
    return point;
}

/**
 * @brief Detection::coordinateBackConvertion prevod z obrazu na souradnice kinektu
 * @param x
 * @param y
 * @return
 */
Point2i Detection::coordinateBackConvertion(int x, int y)
{
    cv::Point2i point;
    point.x = x * settings->conversionKX - settings->conversionDeltaX * settings->conversionKX;
    point.y = y * settings->conversionKY - settings->conversionDeltaY * settings->conversionKY;
    return point;
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
        frame->CopyFrameDataToArray(settings->inputDepthSize, this->d_buffer);
        frame->Release();
        inputCorection(); //inputCorection
    }

    this->d_buffer2mat();
}

void Detection::d_buffer2mat()
{


    for (int x = 0; x < settings->inputDepthWidth; x++)
    {
        for (int y = 0; y < settings->inputDepthHeight; y++)
        {
            int index = y * settings->inputDepthWidth + x;
            if ((y < settings->y1)||(y > settings->y2)||(x < settings->x1)||(x > settings->x2))     //inputCut
            {
                index *= 4;
                this->BGRADepth[index]    = 0xff;
                this->BGRADepth[index+1]  = 0xff;
                this->BGRADepth[index+2]  = 0xff;
                this->BGRADepth[index+3]  = 0xff;
                continue;
            }
            INT16 pom = this->d_buffer[index];

            if ((pom < settings->maxDepth)&&(pom > settings->minDepth)){
                pom = pom - settings->minDepth;
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
    this->d_mat = Mat(Size(settings->inputDepthWidth, settings->inputDepthHeight), CV_8UC4, this->BGRADepth, Mat::AUTO_STEP);
}

std::vector<cv::Point> Detection::cvDetect(Mat d_mat)
{
    vector<Vec4i> hierarchy;
    vector<vector<Point> > contours;
    std::vector<cv::Point> validPoints;
    Size d_size = Size(settings->inputDepthWidth, settings->inputDepthHeight);
    Mat drawing = Mat::zeros( d_size, CV_8UC3 ); //vytvoreni plochy pro vykresleni vysledky hledani
    std::vector<bool> p = {false, false, false, false};

    cv::rectangle(drawing, cv::Point(settings->touchAreaX1, settings->touchAreaY1), cv::Point(settings->touchAreaX2, settings->touchAreaY2), cv::Scalar(255, 255, 0));      //vykreslen oblasti dotyku

    cv::inRange(d_mat, Scalar(0, 0, 0, 0), Scalar(255, 0xa3, 1, 255), d_mat);
//    imshow("depthDetection", d_mat);
//    waitKey(1);

    //odstranení šumu
    int erosion_size = 1;
    int blurSize = 5;
    int elementSize = 5;
    Mat erode_element = getStructuringElement( MORPH_ELLIPSE, Size( 2*erosion_size + 1, 2*erosion_size+1 ), Point( erosion_size, erosion_size ) );
    cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(2 * elementSize + 1, 2 * elementSize + 1), cv::Point(elementSize, elementSize));

    if (settings->erode1) erode( d_mat, d_mat, erode_element );
    if (settings->blur) cv::medianBlur(d_mat, d_mat, blurSize);
    if (settings->dilate2) cv::dilate(d_mat, d_mat, element);
    if (settings->dilate1) dilate( d_mat, d_mat, erode_element );

    //cv::imshow("šum", d_mat);

    //find contours
    cv::findContours(d_mat, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

    if (!contours.empty())
    {
        vector <vector<int>> contourSizes;
        for (int i = 0; i < (signed int) contours.size(); i++)    //find largest contour
        {
            contourSizes.push_back({i, (int) cv::contourArea(contours[i])}); //ulozi index contury s jejim obsahem
        }
        std::sort(contourSizes.begin(), contourSizes.end(), [](vector<int> a, vector<int> b){return a[1] > b[1];});

        int contourOffset = 0; //pokud se kontura neidentifikuje jako hrac navisise o jedna
        for (int n = 0; n < 4; n++) { //n pocet hracu (detekovanych kontur)
            int player = -1;
            int largestContour;
            std::vector<std::vector<cv::Point>> hull(1);

            do {    // vyfiltrovani kontur ktere nepatri hracum a prirazeni kontur hracum
                if ((n + contourOffset) >= contourSizes.size()) {
                    break;
                }
                largestContour = contourSizes[n + contourOffset][0]; //index nejvetsi nezpracovane kontury

                cv::convexHull(cv::Mat(contours[largestContour]), hull[0], false);

                if (hull[0].size() > 2) {
                    for (int ii = 0; ii < playersPoins.size(); ii++) {  //pro kazdeho hrace
                        for (int i = 0; i < playersPoins[ii].size(); i++) { //prohledej vsechny jeh body
                            if (cv::pointPolygonTest(hull[0],  playersPoins[ii][i], false) >= 0) {  //vyzkousej jestly nelezi v prave prohledevane konture
                                player = ii;
                                playersHulls[ii] = hull[0];
                                p[ii] = true;
                                break;
                            }
                        }
                        if (player != -1) { //hras se identifikoval a neni t5eba hledat dal
                            break;
                        }
                    }
                }
                if (player == -1) {
                    contourOffset++;
                }
            } while ((player == -1) && ((n + contourOffset) < contourSizes.size()));
            if (player == -1) { //nenalezen hrac a nezbyvaji zadne nezpracovane kontury
                break;
            }

            cv::drawContours(drawing, contours, largestContour, cv::Scalar(0, 0, 255), 1); //vykreslen9 kontury
            cv::drawContours(drawing, hull, 0, cv::Scalar(0, 255, 0), 1);

            if (hull[0].size() > 2)
            {

                std::vector<int> hullIndexes;
                cv::convexHull(cv::Mat(contours[largestContour]), hullIndexes, true);
                std::vector<cv::Vec4i> convexityDefects;
                cv::convexityDefects(cv::Mat(contours[largestContour]), hullIndexes, convexityDefects);
                cv::Rect boundingBox = cv::boundingRect(hull[0]);

                cv::putText(drawing, std::to_string(player), cv::Point(boundingBox.x, boundingBox.y), 1, 1, cv::Scalar(0, 255, 0));

                cv::rectangle(drawing, boundingBox, cv::Scalar(0, 255, 0));
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
                        validPoints.erase(validPoints.begin()+i);
                        continue;
                    }

                    if (isPointInTouchDepth(validPoints[i])) {  //kontrola zda bod nelezi v pod hloubkou pro dotyk
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
    }

    for (int ii = 0; ii < playersPoins.size(); ii++) {
        for (int i = 0; i < playersPoins[ii].size(); i++) {
            cv::circle(drawing, playersPoins[ii][i], 12, cv::Scalar(255, 255, 255), 1);
        }
    }
    for (int i = 0; i < p.size(); i++) {    //procisteni hracu kteri nebyly detekovani
        if (p[i] == false) {
            playersHulls[i].clear();
        }
    }

    cv::imshow("windowName2", drawing);
    waitKey(1);
    //if (!validPoints.empty())
    //    qDebug()<<this->pointDepth(validPoints[0]);
    return validPoints;
}

bool Detection::isPointInTouchArea(Point point)
{
    if ((point.x > settings->touchAreaX1)&&(point.x < settings->touchAreaX2)) {
        if ((point.y > settings->touchAreaY1)&&(point.y < settings->touchAreaY2)) {
            return true;
        }
    }
    return false;
}

bool Detection::isPointInTouchDepth(Point point)
{
    if (this->pointDepth(point) > settings->touchDepth)
        return true;
    return false;
}

int Detection::pointDepth(Point point)
{
    return d_buffer[point.x + point.y * settings->inputDepthWidth];
}

Detection::~Detection()
{
    delete ui;
    delete d_reader;
    delete c_reader;
    delete d_buffer;
    delete BGRADepth;
}

///
/// \brief Detection::playerOnCoordinates
/// \param x souradnice obrazovky
/// \param y souradnice obrazovky
/// \return id hrace
///
int Detection::playerOnCoordinates(int x, int y)
{
    vector<int> a;
    cv::Point2i point = coordinateBackConvertion(x, y);

    for (int i = 0; i < playersHulls.size(); i++) {
        if (playersHulls[i].empty()) {
            a.push_back(-10000); //nejaka hodne velka nesmyslna hodnota
            continue;
        }
        a.push_back(cv::pointPolygonTest(playersHulls[i], point, true));
    }
    int player = 0;
    for (int i = 0; i < a.size(); i++) {
        if (a[i] > a[player]) {
            player = i;
        }
    }
    //qDebug()<<"detection>"<<player;
    return player;
}

void Detection::setSettings(Settings *value)
{
    settings = value;

    initPlayersPoints();
    //inicializace bufferu
    this->d_buffer  = new UINT16 [settings->inputDepthSize];
    this->BGRADepth = new BYTE [settings->inputDepthSize * 4];
    this->d_mat = Mat::zeros(Size(settings->inputDepthWidth, settings->inputDepthHeight), CV_8UC4);
}

void Detection::initPlayersPoints()
{
    std::vector<cv::Point> tmp;
    tmp.push_back(cv::Point((settings->touchAreaX2 - settings->touchAreaX1)/2 + settings->touchAreaX1 + (settings->touchAreaX2 - settings->touchAreaX1)/5,
                            settings->touchAreaY1/2));
    tmp.push_back(cv::Point((settings->touchAreaX2 - settings->touchAreaX1)/2 + settings->touchAreaX1 - (settings->touchAreaX2 - settings->touchAreaX1)/5,
                            settings->touchAreaY1/2));
    playersPoins.push_back(tmp);
    tmp.clear();

    tmp.push_back(cv::Point((settings->x2 - settings->touchAreaX2)/2 + settings->touchAreaX2,
                            (settings->touchAreaY2 - settings->touchAreaY1)/2 + settings->touchAreaY1));
    playersPoins.push_back(tmp);
    tmp.clear();

    tmp.push_back(cv::Point((settings->touchAreaX2 - settings->touchAreaX1)/2 + settings->touchAreaX1 + (settings->touchAreaX2 - settings->touchAreaX1)/5, (settings->y2 - settings->touchAreaY2)/2 + settings->touchAreaY2));
    tmp.push_back(cv::Point((settings->touchAreaX2 - settings->touchAreaX1)/2 + settings->touchAreaX1 - (settings->touchAreaX2 - settings->touchAreaX1)/5, (settings->y2 - settings->touchAreaY2)/2 + settings->touchAreaY2));
    tmp.push_back(cv::Point((settings->touchAreaX2 - settings->touchAreaX1)/2 + settings->touchAreaX1,
                                (settings->y2 - settings->touchAreaY2)/2 + settings->touchAreaY2));
    playersPoins.push_back(tmp);
    tmp.clear();

    tmp.push_back(cv::Point(settings->touchAreaX1/2, (settings->touchAreaY2 - settings->touchAreaY1)/2 + settings->touchAreaY1));
    playersPoins.push_back(tmp);
    tmp.clear();

    playersHulls.resize(PLAYER_MAXIMUM);
}

void Detection::inputCorection()
{
    for (int x = 0; x < settings->inputDepthWidth; x++)
    {
        for (int y = 0; y < settings->inputDepthHeight; y++)
        {
            int index = y * settings->inputDepthWidth + x;
            //INT16 pom = this->d_buffer[index];

            this->d_buffer[index] -= settings->inputCorectionX * x + settings->inputCorectionY * y;
        }
    }
}

DetectionThread::DetectionThread(Detection * detection)
{
    this->detection = detection;
}

DetectionThread::~DetectionThread()
{
    this->stop  = true;
    this->exit();
}

void DetectionThread::run()
{
    while (this->stop == false) {
        this->detection->run();
        usleep(10000); //30000
    }

    delete this->detection;
}



