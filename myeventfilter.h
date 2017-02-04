#ifndef MYEVENTFILTER_H
#define MYEVENTFILTER_H

#include <QObject>
#include <QEvent>
#include <QMouseEvent>

#include <QDebug>

class MyEventFilter : public QObject
{
    Q_OBJECT
public:
    MyEventFilter();
    //bool eventFilter(QObject * object, QEvent * event);
    bool eventFilter(QObject * object, QEvent *event);
};

#endif // MYEVENTFILTER_H
