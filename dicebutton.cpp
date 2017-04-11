#include "dicebutton.h"

DiceButton::DiceButton(int type, QWidget *parent) :LabelButton(parent)
{    
    this->type = type;

    //nastaveni pozive cisla na kostce
    if (type == D4) {
        this->setPixmap(QPixmap(D4_IMG));
        innerLabelShiftX = -10;
        innerLabelShiftY = 10;
    }
    if (type == D6) {
        this->setPixmap(QPixmap(D6_IMG));
        innerLabelShiftX = -30;
        innerLabelShiftY = 0;
    }
    if (type == D8) {
        this->setPixmap(QPixmap(D8_IMG));
        innerLabelShiftX = -25;
        innerLabelShiftY = -3;
    }
    if (type == D10) {
        this->setPixmap(QPixmap(D10_IMG));
        innerLabelShiftX = -23;
        innerLabelShiftY = -18;
    }
    if (type == D12) {
        this->setPixmap(QPixmap(D12_IMG));
        innerLabelShiftX = -15;
        innerLabelShiftY = -15;
    }
    if (type == D20) {
        this->setPixmap(QPixmap(D20_IMG));
        innerLabelShiftX = -5;
        innerLabelShiftY = -10;
    }

    this->adjustSize();

    QObject::connect(this, SIGNAL(valueRolled(int)), this, SLOT(setText(int)));
}

int DiceButton::makeRoll()
{
    return (rand() % type) + 1;
}

void DiceButton::mousePressEvent(QMouseEvent *event)
{
    int val = makeRoll();
    valueRolled(val);
    adjustInnerLabelGeometry();
}

DiceMenu::DiceMenu(QWidget *parent) : QWidget(parent)
{
    this->setStyleSheet("background:transparent;");

    createButtons();
    this->adjustSize();
}


void DiceMenu::createButtons()
{
    int x = 0;
    int y = 0;

    suma = new DiceButtonSuma(this);
    suma->setPixmap(QPixmap(":/Img/Img/sigma.png"));
    suma->innerLabelShiftX = 25;
    suma->innerLabelShiftY = 40;
    suma->adjustInnerLabelGeometry();
    suma->adjustSize();
    suma->setGeometry(x, y, suma->width(), suma->height());
    x += buttonDistanceX;
    y += buttonDistanceY;

    DiceButton* lBtn = new DiceButton(DiceButton::D4, this);
    lBtn->setGeometry(x, y, lBtn->width(), lBtn->height());
    QObject::connect(lBtn, SIGNAL(valueRolled(int)), suma, SLOT(addValue(int)));
    x += buttonDistanceX;
    y += buttonDistanceY;
    lBtn = new DiceButton(DiceButton::D6, this);
    lBtn->setGeometry(x, y, lBtn->width(), lBtn->height());
    QObject::connect(lBtn, SIGNAL(valueRolled(int)), suma, SLOT(addValue(int)));
    x += buttonDistanceX;
    y += buttonDistanceY;
    lBtn = new DiceButton(DiceButton::D8, this);
    lBtn->setGeometry(x, y, lBtn->width(), lBtn->height());
    QObject::connect(lBtn, SIGNAL(valueRolled(int)), suma, SLOT(addValue(int)));
    x += buttonDistanceX;
    y += buttonDistanceY;
    lBtn = new DiceButton(DiceButton::D10, this);
    lBtn->setGeometry(x, y, lBtn->width(), lBtn->height());
    QObject::connect(lBtn, SIGNAL(valueRolled(int)), suma, SLOT(addValue(int)));
    x += buttonDistanceX;
    y += buttonDistanceY;
    lBtn = new DiceButton(DiceButton::D12, this);
    lBtn->setGeometry(x, y, lBtn->width(), lBtn->height());
    QObject::connect(lBtn, SIGNAL(valueRolled(int)), suma, SLOT(addValue(int)));
    x += buttonDistanceX;
    y += buttonDistanceY;
    lBtn = new DiceButton(DiceButton::D20, this);
    lBtn->setGeometry(x, y, lBtn->width(), lBtn->height());
    QObject::connect(lBtn, SIGNAL(valueRolled(int)), suma, SLOT(addValue(int)));
    x += buttonDistanceX;
    y += buttonDistanceY;

}

DiceButtonSuma::DiceButtonSuma(QWidget *parent): LabelButton(parent)
{
    QObject::connect(this, SIGNAL(click()), this, SLOT(resetValue()));
}

void DiceButtonSuma::resetValue()
{
    value = 0;
    setText(QString::number(value));
}

void DiceButtonSuma::addValue(int val)
{
    value += val;
    setText(QString::number(value));
}
