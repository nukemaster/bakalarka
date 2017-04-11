#ifndef DICEBUTTON_H
#define DICEBUTTON_H

#include <QMouseEvent>

#include <QPainter>

#include <QDebug>

#include "labelbutton.h"

#define D4_IMG      ":/Img/Img/d4.png"
#define D6_IMG      ":/Img/Img/d6.png"
#define D8_IMG      ":/Img/Img/d8.png"
#define D10_IMG     ":/Img/Img/d10.png"
#define D12_IMG     ":/Img/Img/d12.png"
#define D20_IMG     ":/Img/Img/d20.png"
#define D100_IMG    ":/Img/Img/d100.png"

class DiceButton : public LabelButton
{
    Q_OBJECT
public:
    DiceButton(int type, QWidget * parent = 0 );

    int type;

    static const int D4 = 4;
    static const int D6 = 6;
    static const int D8 = 8;
    static const int D10 = 10;
    static const int D12 = 12;
    static const int D20 = 20;
    static const int D100 = 100;

public slots:
    int makeRoll();

signals:
    int roll(int, int);
    void valueRolled(int val);

protected:
    void mousePressEvent(QMouseEvent* event);
};

class DiceButtonSuma : public LabelButton
{
    Q_OBJECT
public:
    DiceButtonSuma(QWidget* parent);
    int value = 0;

public slots:
    void resetValue();
    void addValue(int val);

};

#define BUTTON_DISTACE_X 140
#define BUTTON_DISTACE_Y 0

class DiceMenu : public QWidget
{
public:
    DiceMenu(QWidget* parent = 0);

    int buttonDistanceX = BUTTON_DISTACE_X;
    int buttonDistanceY = BUTTON_DISTACE_Y;

    DiceButtonSuma* suma;

protected:
    void createButtons();
};

#endif // DICEBUTTON_H
