#include "detection.h"
#include "ui_detection.h"

Detection::Detection(QWidget *parent) :
    QWidget(parent)
{
    kinect = new Kinect();
}


void Detection::run()
{
//    std::vector<cv::Point> points;
//    cv::Point2i point;

    if (!d_mat.empty()) {
        d_matMutex = false;
        d_mat.release();
    }
    this->d_mat = kinect->getDepthFrame();
    d_matMutex = true;
    inputCorection();
    this->cvDetect(d_mat);
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

std::vector<cv::Point> Detection::cvDetect(Mat d_mat)
{
    vector<Vec4i> hierarchy;
    vector<vector<Point> > contours;
    std::vector<cv::Point> validPoints;
    std::vector<bool> p = {false, false, false, false};
    Point foundPoint;

//    Mat drawing; //debug

    //thresholding
    cv::inRange(d_mat, Scalar(settings->minDepth), Scalar(settings->maxDepth), d_mat);

//    d_mat.convertTo(drawing, CV_8UC3); //debug

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
        //serazeni kontur podle velikosti
        vector <vector<int>> contourSizes;
        for (int i = 0; i < (signed int) contours.size(); i++)
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

                //zjisteni kteremu hraci patri zpracovavana kontura
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

            //nalezeni bodu kam hrac ukazuje
            if (hull[0].size() > 2)
            {
                //nalezeni konvexnich defektu
                std::vector<int> hullIndexes;
                cv::convexHull(cv::Mat(contours[largestContour]), hullIndexes, true);
                std::vector<cv::Vec4i> convexityDefects;
                cv::convexityDefects(cv::Mat(contours[largestContour]), hullIndexes, convexityDefects);

                //hledani nejvetsiho konvexniho defektu
                int maxArea = 0;    //ulozeni plochy nejvetsiho zatim nalezeneho konvexniho defektu
                cv::Point po1, po2, po3;    //ulozeni vrcholu nejvetsiho konvexniho defektu
                for (size_t i = 0; i < convexityDefects.size(); i++)
                {
                    cv::Point p1 = contours[largestContour][convexityDefects[i][0]];
                    cv::Point p2 = contours[largestContour][convexityDefects[i][1]];
                    cv::Point p3 = contours[largestContour][convexityDefects[i][2]];                    

                    //pocitani obsahu konvexniho defektu
                    int a = cv::norm(p1-p2);
                    int b = cv::norm(p2-p3);
                    int c = cv::norm(p3-p1);
                    int s = (a+b+c)/2;
                    s = sqrt(s*(s-a)*(s-b)*(s-c));

                    if (s > maxArea) {
                        maxArea = s;
                        po1 = p1;
                        po2 = p2;
                        po3 = p3;
                    }
                }

                //vybrani nejvzdalenejsiho bodu konvexniho defektu od hrace
                if (cv::pointPolygonTest(playersPoins[player],  po1, true) < cv::pointPolygonTest(playersPoins[player],  po2, true)) {
                    foundPoint = po1;
                }
                else {
                    foundPoint = po2;
                }

                validPoints.push_back(foundPoint);
//                cv::circle(drawing, foundPoint, 12, cv::Scalar(255), 1);
//                cv::imshow("debug", drawing);

                //kontrola zda neni bod v oblasti deteku
                if (isPointInTouchArea(foundPoint)) {
                    foundPoint  = coordinateConversion(foundPoint.x, foundPoint.y);
//                    cursor().setPos(foundPoint.x, foundPoint.y);
//                    qDebug()<<foundPoint.x << foundPoint.y;
//                    qDebug()<<player;
                    newCoordinates(foundPoint.x, foundPoint.y, player);
                }
            }
        }
    }

    for (int i = 0; i < p.size(); i++) {    //procisteni hracu kteri nebyly detekovani
        if (p[i] == false) {
            playersHulls[i].clear();
        }
    }

//    cv::imshow("test", d_mat);

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
    while (d_matMutex == false) {
    //qDebug()<<d_mat.at<float>(point.x, point.y);
        return d_mat.at<float>(point.x, point.y);
    }
}

Detection::~Detection()
{
    //todo
//    delete d_reader;
//    delete c_reader;
//    delete d_buffer;
//    delete BGRADepth;
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
    //qDebug()<<
    return player;
}

bool Detection::getDepthFrame(cv::Mat* d_mat)
{
    //cv::imshow("D_Mat", (*d_mat) );

    this->cvDetect(*(d_mat));
    delete d_mat;
    return true;
}

void Detection::setSettings(Settings *value)
{
    settings = value;

    initPlayersPoints();
    //inicializace bufferu
//    this->d_buffer  = new UINT16 [settings->inputDepthSize];
//    this->BGRADepth = new BYTE [settings->inputDepthSize * 4];
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
    for (int i = 0; i < d_mat.rows; i++)
    {
        for (int j = 0; j < d_mat.cols; j++)
        {
            int index = j * settings->inputDepthWidth + i;
            d_mat.at<float>(i, j) = d_mat.at<float>(i, j) - (settings->inputCorectionX * j + settings->inputCorectionY * i);
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

        //usleep(10000); //
//        qDebug()<<"loop";
    }

    delete this->detection;
}



