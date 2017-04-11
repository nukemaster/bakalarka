#ifndef LABELBUTTON_H
#define LABELBUTTON_H

#include <QLabel>

#include <QDebug>

class LabelButton : public QLabel
{
    Q_OBJECT
public:
    LabelButton(QWidget *parent);
    void adjustInnerLabelGeometry();
    int innerLabelShiftX = 0;
    int innerLabelShiftY = 0;

    QLabel * innerLabel;

signals:
    void click();
    void click(QString str);

public slots:
    void setText(QString text);
    void setText(int val);

protected:
    void mousePressEvent(QMouseEvent* event);
};

#endif // LABELBUTTON_H
