#ifndef SETTINGS_H
#define SETTINGS_H

//default setting values
#define DEFAULT_INPUTCUT_X1 0
#define DEFAULT_INPUTCUT_Y1 0
#define DEFAULT_INPUTCUT_X2 512
#define DEFAULT_INPUTCUT_Y2 424
#define DEFAULT_INPUTCUT_MINDEPTH 0
#define DEFAULT_INPUTCUT_MAXDEPTH 1326

#define DEFAULT_TOUCHAREA_X1 0
#define DEFAULT_TOUCHAREA_Y1 0
#define DEFAULT_TOUCHAREA_X2 512
#define DEFAULT_TOUCHAREA_Y2 424
#define DEFAULT_TOUCHAREA_DEPTH 1323

#define DEFAULT_CV_ERODE true
#define DEFAULT_CV_DILATE true
#define DEFAULT_CV_BLUR true
#define DEFAULT_CV_DILATE2 false

#define DEFAULT_INPUT_WIDTH 512
#define DEFAULT_INPUT_HEIGHT 424

#define DEFAULT_CONVERSION_K_X 1
#define DEFAULT_CONVERSION_K_Y 1

#define DEFAULT_CONVERSION_DELTA_X 0
#define DEFAULT_CONVERSION_DELTA_Y 0

#define DEFAULT_INPUT_CORECTION_X 0
#define DEFAULT_INPUT_CORECTION_Y 0

#define SETTINGS_FILE "detection.xml"

#include <QFile>
#include <QDomDocument>

class Settings
{
public:
    Settings();

    double conversionKX = DEFAULT_CONVERSION_K_X;
    double conversionKY = DEFAULT_CONVERSION_K_Y;
    double conversionDeltaX = DEFAULT_CONVERSION_DELTA_X;
    double conversionDeltaY = DEFAULT_CONVERSION_DELTA_X;

    double inputCorectionX = DEFAULT_INPUT_CORECTION_X;
    double inputCorectionY = DEFAULT_INPUT_CORECTION_Y;

    ///// orez
    int minDepth = DEFAULT_INPUTCUT_MINDEPTH;
    int maxDepth = DEFAULT_INPUTCUT_MAXDEPTH;
    int x1 = DEFAULT_INPUTCUT_X1;
    int x2 = DEFAULT_INPUTCUT_Y1;
    int y1 = DEFAULT_INPUTCUT_X2;
    int y2 = DEFAULT_INPUTCUT_Y2;
    ///// touchArea
    int touchDepth  = DEFAULT_TOUCHAREA_DEPTH;
    int touchAreaX1 = DEFAULT_TOUCHAREA_X1;
    int touchAreaX2 = DEFAULT_TOUCHAREA_Y1;
    int touchAreaY1 = DEFAULT_TOUCHAREA_X2;
    int touchAreaY2 = DEFAULT_TOUCHAREA_Y2;
    /// <CVProcessing>
    bool erode1     = DEFAULT_CV_ERODE;
    bool dilate1    = DEFAULT_CV_DILATE;
    bool blur       = DEFAULT_CV_BLUR;
    bool dilate2    = DEFAULT_CV_DILATE2;
    /// </CVProcessing>
    int inputDepthWidth;
    int inputDepthHeight;
    int inputDepthSize; //this->inputDepthWidth * this->inputDepthHeight
    ///touchCalibration
    int touchModX;
    int touchModY;
    int touchSizeX;
    int touchSizeY;
    double touchKX;
    double touchKY;

};

#endif // SETTINGS_H
