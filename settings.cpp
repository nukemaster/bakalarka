#include "settings.h"

Settings::Settings()
{
    QFile file(SETTINGS_FILE);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return;
    }

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

    e = e0.firstChildElement("inputCorection");
    this->inputCorectionX = e.firstChildElement("kx").toElement().text().toDouble();
    this->inputCorectionY = e.firstChildElement("ky").toElement().text().toDouble();

    e = e0.firstChildElement("CVProcessing");
    this->erode1   = (e.firstChildElement("erode").toElement().text() == "true") ? true : false;
    this->dilate1 = (e.firstChildElement("dilate1").toElement().text() == "true") ? true : false;
    this->blur    = (e.firstChildElement("blur").toElement().text() == "true") ? true : false;
    this->dilate2 = (e.firstChildElement("dilate2").toElement().text() == "true") ? true : false;

    e = e0.firstChildElement("input");
    this->inputDepthWidth  = e.firstChildElement("width").toElement().text().toInt();
    this->inputDepthHeight = e.firstChildElement("height").toElement().text().toInt();

    e = e0.firstChildElement("conversion");
    this->conversionKX = e.firstChildElement("k").toElement().attribute("x").toDouble();
    this->conversionKY = e.firstChildElement("k").toElement().attribute("y").toDouble();
    this->conversionDeltaX =  e.firstChildElement("delta").toElement().attribute("x").toDouble();
    this->conversionDeltaY =  e.firstChildElement("delta").toElement().attribute("y").toDouble();

    e = e0.firstChildElement("touchCalibration");
    this->touchModX = e.firstChildElement("touchMod").toElement().attribute("x").toInt();
    this->touchModY = e.firstChildElement("touchMod").toElement().attribute("y").toInt();
    this->touchKX = e.firstChildElement("touchK").toElement().attribute("x").toDouble();
    this->touchKY = e.firstChildElement("touchK").toElement().attribute("y").toDouble();

    file.close();

    inputDepthSize = inputDepthHeight * inputDepthWidth;

}

