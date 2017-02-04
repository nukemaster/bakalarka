#include "myeventfilter.h"

MyEventFilter::MyEventFilter() : QObject()
{

}

bool MyEventFilter::eventFilter(QObject * object, QEvent * event)
{
    if ((event->type() == QEvent::MouseButtonPress)||(event->type() == QEvent::MouseButtonRelease)||(event->type() == QEvent::MouseMove)) {
        qDebug()<<"eventFilter"<<event->type()<<((QMouseEvent*)event)->pos()<<((QMouseEvent*)event)->source();
        if (dynamic_cast<QMouseEvent *>(event)->source() == Qt::MouseEventSynthesizedBySystem) {
            //qDebug()<<"eventFilter filter event synthetized by system";
            event->accept();
            return true;
        }
    }
    return false;
}

