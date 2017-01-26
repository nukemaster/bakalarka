#include "calibration.h"
#include "ui_calibration.h"

Calibration::Calibration(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Calibration)
{
    ui->setupUi(this);

    waitOnPoint = new QEventLoop();
    waitOnTouchPoint = new QEventLoop();
    QObject::connect(this, SIGNAL(pointRecorded()), waitOnPoint, SLOT(quit()));

    outPoint1 = QPointF(200, 200);
    outPoint2 = QPointF(700, 900);
    outPoint3 = QPointF(1400, 380);

    outTouchPoint1 = QPointF(0, 0);
    outTouchPoint2 = QPointF(1920, 1080);


    this->setAttribute(Qt::WA_AcceptTouchEvents);
}

Calibration::~Calibration()
{
    this->pointRecorded();
    delete ui;
    delete waitOnPoint;

}

void Calibration::setValues()
{
    ui->slider_inputCut_X1->setValue(settings->x1);
    ui->slider_inputCut_Y1->setValue(settings->y1);
    ui->slider_inputCut_X2->setValue(settings->x2);
    ui->slider_inputCut_Y2->setValue(settings->y2);
    ui->slider_inpicCut_maxDepth->setValue(settings->maxDepth);
    ui->slider_touchArea_X1->setValue(settings->touchAreaX1);
    ui->slider_touchArea_Y1->setValue(settings->touchAreaY1);
    ui->slider_touchArea_X2->setValue(settings->touchAreaX2);
    ui->slider_touchArea_Y2->setValue(settings->touchAreaY2);
    ui->slider_touchArea_depth->setValue( settings->touchDepth);
    ui->doubleSpinBox->setValue(settings->inputCorectionX);
    ui->doubleSpinBox_2->setValue(settings->inputCorectionY);
}

void Calibration::coordinatesFromDetection(int x, int y, bool clicked)
{

    //Indikace kliknuti
    if (clicked) {
        ui->clicked_indicator->setPixmap(QPixmap(":/Img/unit_blue_selected.png"));
    }
    else {
        ui->clicked_indicator->setPixmap(QPixmap(":/Img/unit_blue.png"));
    }

    if (recordClick && clicked) {
        if (isnan(recordedX) || isnan(recordedY)) {
            recordedX = x;
            recordedY = y;
            recordedN = 1;
        }
        else {
            recordedX += x;
            recordedY += y;
            recordedN++;
        }
    }

    if (recordClick && !clicked && !isnan(recordedX) && !isnan(recordedX)) {
        recordClick = false;

        tmpX = recordedX / recordedN;
        tmpY = recordedY / recordedN;

        recordedX = _Nan._Double;
        recordedY = _Nan._Double;

        pointRecorded();

    }
}

void Calibration::setSettings(Settings *value)
{
    settings = value;
    setUiElements();
    setValues();
}

void Calibration::setUiElements()
{
    ui->slider_inputCut_X1->setMaximum(settings->inputDepthWidth);
    ui->slider_inputCut_X2->setMaximum(settings->inputDepthWidth);
    ui->slider_touchArea_X1->setMaximum(settings->inputDepthWidth);
    ui->slider_touchArea_X2->setMaximum(settings->inputDepthWidth);

    ui->slider_inputCut_Y1->setMaximum(settings->inputDepthHeight);
    ui->slider_inputCut_Y2->setMaximum(settings->inputDepthHeight);
    ui->slider_touchArea_Y1->setMaximum(settings->inputDepthHeight);
    ui->slider_touchArea_Y2->setMaximum(settings->inputDepthHeight);

    ui->calibrationImage->hide();
    ui->label_P1_val->hide();
    ui->label_P2_val->hide();
    ui->label_P3_val->hide();
    ui->label_tP1_val->hide();
    ui->label_tP2_val->hide();

}

void Calibration::touchEvent(QTouchEvent * event)
{
    qDebug()<<"touchEvent";
    tmpPoint = event->touchPoints()[0].pos();
    waitOnTouchPoint->exit();
}

bool Calibration::event(QEvent *event)
{
    if (event->type() == QEvent::TouchBegin) {
        this->touchEvent(dynamic_cast<QTouchEvent*>( event));
        return true;
    }
    if (event->type() == QEvent::TouchUpdate) {
        //this->touchUpdateEvent(dynamic_cast<QTouchEvent*>( event));
        //return true;
    }
    return QWidget::event(event);
}

void Calibration::on_slider_inputCut_Y1_valueChanged(int value)
{
    settings->y1 = value;
}

void Calibration::on_slider_inputCut_X1_valueChanged(int value)
{
    settings->x1 = value;
}

void Calibration::on_slider_inputCut_Y2_valueChanged(int value)
{
    settings->y2 = value;
}

void Calibration::on_slider_inputCut_X2_valueChanged(int value)
{
    settings->x2 = value;
}

void Calibration::on_slider_touchArea_X1_valueChanged(int value)
{
    settings->touchAreaX1 = value;
}

void Calibration::on_slider_touchArea_Y2_valueChanged(int value)
{
    settings->touchAreaY2 = value;
}

void Calibration::on_slider_touchArea_X2_valueChanged(int value)
{
    settings->touchAreaX2 = value;
}

void Calibration::on_slider_touchArea_Y1_valueChanged(int value)
{
    settings->touchAreaY1 = value;
}

void Calibration::on_slider_inpicCut_maxDepth_valueChanged(int value)
{
    settings->maxDepth = value;
}

void Calibration::on_slider_touchArea_depth_valueChanged(int value)
{
    settings->touchDepth = value;
}

void Calibration::on_btn_calibration_P1_clicked()
{
    recordClick = true;

    this->ui->calibrationImage->show();
    this->ui->calibrationImage->setPixmap(QPixmap(":/Img/Img/BP_kalibrace.png"));
    this->showFullScreen();
    this->ui->calibrationImage->setGeometry(0,0,1920,1080);

    waitOnPoint->exec();    //cekani na vztup

    inPoint1 = QPointF(tmpX, tmpY);
    ui->label_P1_val->setText(QString::number(tmpX) + ", " + QString::number(tmpY));
    ui->label_P1_val->show();
    qDebug()<<"P1"<<tmpX<<tmpY;

    this->showNormal();
    this->ui->calibrationImage->hide();
}

void Calibration::on_btn_calibration_P2_clicked()
{
    recordClick = true;

    this->ui->calibrationImage->show();
    this->ui->calibrationImage->setPixmap(QPixmap(":/Img/Img/BP_kalibrace.png"));
    this->showFullScreen();
    this->ui->calibrationImage->setGeometry(0,0,1920,1080);

    waitOnPoint->exec();    //cekani na vztup

    inPoint2 = QPointF(tmpX, tmpY);
    ui->label_P2_val->setText(QString::number(tmpX) + ", " + QString::number(tmpY));
    ui->label_P2_val->show();
    qDebug()<<"P2"<<tmpX<<tmpY;

    this->showNormal();
    this->ui->calibrationImage->hide();
}

void Calibration::on_btn_calibration_P3_clicked()
{
    recordClick = true;

    this->ui->calibrationImage->show();
    this->ui->calibrationImage->setPixmap(QPixmap(":/Img/Img/BP_kalibrace.png"));
    this->showFullScreen();
    this->ui->calibrationImage->setGeometry(0,0,1920,1080);

    waitOnPoint->exec();    //cekani na vztup

    inPoint3 = QPointF(tmpX, tmpY);
    ui->label_P3_val->setText(QString::number(tmpX) + ", " + QString::number(tmpY));
    ui->label_P3_val->show();
    qDebug()<<"P3"<<tmpX<<tmpY;

    this->showNormal();
    this->ui->calibrationImage->hide();
}

void Calibration::on_doubleSpinBox_valueChanged(double arg1)
{
    settings->inputCorectionX = arg1;
}

void Calibration::on_doubleSpinBox_2_valueChanged(double arg1)
{
    settings->inputCorectionY = arg1;
}

void Calibration::on_pushButton_clicked()
{

    if (!((inPoint1.isNull()) && (inPoint2.isNull()) && (inPoint3.isNull()))) {
        double kX = ((inPoint1.x() - inPoint2.x())/(outPoint1.x() - outPoint2.x()));
        kX += ((inPoint2.x() - inPoint3.x())/(outPoint2.x() - outPoint3.x()));
        kX += ((inPoint3.x() - inPoint1.x())/(outPoint3.x() - outPoint1.x()));
        kX /= 3;

        double kY = ((inPoint1.y() - inPoint2.y())/(outPoint1.y() - outPoint2.y()));
        kY += ((inPoint2.y() - inPoint3.y())/(outPoint2.y() - outPoint3.y()));
        kY += ((inPoint3.y() - inPoint1.y())/(outPoint3.y() - outPoint1.y()));
        kY /= 3;

        int dX = outPoint1.x() * kX - inPoint1.x();
        dX += outPoint1.x() * kX - inPoint1.x();
        dX += outPoint1.x() * kX - inPoint1.x();
        dX /= 3;

        int dY = outPoint1.y() * kY - inPoint1.y();
        dY += outPoint1.y() * kY - inPoint1.y();
        dY += outPoint1.y() * kY - inPoint1.y();
        dY /= 3;

        this->kX = kX;
        this->kY = kY;
        this->deltaX = dX;
        this->deltaY = dY;

        ui->label_calibration_values->setText("k> x: " + QString::number(this->kX) +
                                              ", y: " + QString::number(this->kY) +
                                              "\nΔ> x: " + QString::number(this->deltaX) +
                                              ", y: " + QString::number(this->deltaY));

    }

    if ((inTouchPoint1.isNull())||(inTouchPoint2.isNull())) {
        return;
    }

    touchXmod = inTouchPoint1.x();
    touchYmod = inTouchPoint1.y();
    touchWidth = (inTouchPoint2.x() - touchXmod);
    touchHeight = (inTouchPoint2.y() - touchYmod);
    touchKX = 1920 / touchWidth;
    touchKY = 1080 / touchHeight;

    this->ui->label_calibration_values->setText(/*ui->label_calibration_values->text() +*/
                                          "\ntouchMod> x: " + QString::number(this->touchXmod) +
                                          "\n y: " + QString::number(this->touchYmod) +
                                          "\ntouchSize> width: " + QString::number(this->touchWidth) +
                                          "\n height: " + QString::number(this->touchHeight) +
                                          "\ntouchK> x: " + QString::number(this->touchKX) +
                                          "\n y: " + QString::number(this->touchKY)) ;

    qDebug()<<touchXmod<<touchYmod<<touchKX<<touchKY;

}



void Calibration::on_btn_save_clicked()
{
    QFile file("C:\\Users\\dymac_000\\Documents\\BP\\hra\\build-hra-Desktop_Qt_5_5_1_MSVC2013_32bit-Vydání\\test.xml");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;

    QXmlStreamWriter stream(&file);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();

    stream.writeStartElement("setting");
        stream.writeStartElement("orez");
            stream.writeTextElement("minDepth", "0");
            stream.writeTextElement("maxDepth", QString::number(ui->slider_inpicCut_maxDepth->value()));
            stream.writeTextElement("x1", QString::number(ui->slider_inputCut_X1->value()));
            stream.writeTextElement("y1", QString::number(ui->slider_inputCut_Y1->value()));
            stream.writeTextElement("x2", QString::number(ui->slider_inputCut_X2->value()));
            stream.writeTextElement("y2", QString::number(ui->slider_inputCut_Y2->value()));
        stream.writeEndElement();
        stream.writeStartElement("inputCorection");
            stream.writeTextElement("kx", QString::number(ui->doubleSpinBox->value()));
            stream.writeTextElement("ky", QString::number(ui->doubleSpinBox_2->value()));
        stream.writeEndElement();
        stream.writeStartElement("touchArea");
            stream.writeTextElement("depth", QString::number(ui->slider_touchArea_depth->value()));
            stream.writeTextElement("x1", QString::number(ui->slider_touchArea_X1->value()));
            stream.writeTextElement("y1", QString::number(ui->slider_touchArea_Y1->value()));
            stream.writeTextElement("x2", QString::number(ui->slider_touchArea_X2->value()));
            stream.writeTextElement("y2", QString::number(ui->slider_touchArea_Y2->value()));
        stream.writeEndElement();
        stream.writeStartElement("CVProcessing");
            stream.writeTextElement("erode", "true");
            stream.writeTextElement("dilate1", "true");
            stream.writeTextElement("blur", "true");
            stream.writeTextElement("dilate2", "false");
        stream.writeEndElement();
        stream.writeStartElement("input");
            stream.writeTextElement("width", "512");
            stream.writeTextElement("height", "424");
        stream.writeEndElement();
        stream.writeStartElement("conversion");
            stream.writeStartElement("k");
                stream.writeAttribute("x", QString::number(kX));
                stream.writeAttribute("y", QString::number(kY));
            stream.writeEndElement();
            stream.writeStartElement("delta");
                stream.writeAttribute("x", QString::number(deltaX));
                stream.writeAttribute("y", QString::number(deltaY));
            stream.writeEndElement();
        stream.writeEndElement();
        stream.writeStartElement("touchCalibration");
            stream.writeStartElement("touchMod");
                stream.writeAttribute("x", QString::number(touchXmod));
                stream.writeAttribute("y", QString::number(touchYmod));
            stream.writeEndElement();
            stream.writeStartElement("touchSize");
                stream.writeAttribute("width", QString::number(touchWidth));
                stream.writeAttribute("height", QString::number(touchHeight));
            stream.writeEndElement();
            stream.writeStartElement("touchK");
                stream.writeAttribute("x", QString::number(touchKX));
                stream.writeAttribute("y", QString::number(touchKY));
            stream.writeEndElement();
        stream.writeEndElement();
    stream.writeEndElement();


    stream.writeEndDocument();

    file.close();
}

void Calibration::on_btn_calibration_tP1_clicked()
{

    this->ui->calibrationImage->show();
    QPixmap tmpPixmap = QPixmap(1920, 1080);
    tmpPixmap.fill();
    this->ui->calibrationImage->setPixmap(tmpPixmap);
    this->showFullScreen();
    this->ui->calibrationImage->setGeometry(0,0,1920,1080);

    ui->clicked_indicator->setGeometry(outTouchPoint1.x()-20, outTouchPoint1.y()-20, 50, 50);

    waitOnTouchPoint->exec();    //cekani na vztup

    inTouchPoint1 = tmpPoint;
    ui->label_tP1_val->setText(QString::number(tmpPoint.x()) + ", " + QString::number(tmpPoint.y()));
    ui->label_tP1_val->show();
    qDebug()<<"tP1"<<inTouchPoint1;

    this->ui->calibrationImage->hide();
    this->showNormal();

}

void Calibration::on_btn_calibration_tP2_clicked()
{
    this->ui->calibrationImage->show();
    QPixmap tmpPixmap = QPixmap(1920, 1080);
    tmpPixmap.fill();
    this->ui->calibrationImage->setPixmap(tmpPixmap);
    this->showFullScreen();
    this->ui->calibrationImage->setGeometry(0,0,1920,1080);

    ui->clicked_indicator->setGeometry(outTouchPoint2.x()-20, outTouchPoint2.y()-20, 40, 40);


    waitOnTouchPoint->exec();    //cekani na vztup

    inTouchPoint2 = tmpPoint;
    ui->label_tP2_val->setText(QString::number(tmpPoint.x()) + ", " + QString::number(tmpPoint.y()));
    ui->label_tP2_val->show();
    qDebug()<<"tP2"<<inTouchPoint2;

    this->ui->calibrationImage->hide();
    this->showNormal();

}
