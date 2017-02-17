#ifndef LABELBUTTON_H
#define LABELBUTTON_H

#include <QLabel>
#include <QMouseEvent>

#include <QDebug>

//#define D4      0;
//#define D6      1;
//#define D8      2;
//#define D10     3;
//#define D12     4;
//#define D20     5;
//#define D100    6;

#define D4_IMG      ":/Img/Img/d4.png"
#define D6_IMG      ":/Img/Img/d6.png"
#define D8_IMG      ":/Img/Img/d8.png"
#define D10_IMG     ":/Img/Img/d10.png"
#define D12_IMG     ":/Img/Img/d12.png"
#define D20_IMG     ":/Img/Img/d20.png"
#define D100_IMG    ":/Img/Img/d100.png"

class DiceButton : public QLabel
{
    Q_OBJECT
public:
    DiceButton(int type, QWidget * parent = 0 );

    int type;
    QLabel * innerLabel;
    void adjustInnerLabelGeometry();
    int innerLabelShiftX = 0;
    int innerLabelShiftY = 0;

    static const int D4 = 4;
    static const int D6 = 6;
    static const int D8 = 8;
    static const int D10 = 10;
    static const int D12 = 12;
    static const int D20 = 20;
    static const int D100 = 100;
signals:
    int roll(int, int);

protected:
    void mousePressEvent(QMouseEvent* event);
};

#endif // LABELBUTTON_H
