#include "dicebutton.h"

DiceButton::DiceButton(int type, QWidget *parent) :QLabel(parent)
{
    this->type = type;

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
    innerLabel = new QLabel(this);
    QFont f( "Arial", 20, QFont::Bold);
    innerLabel->setFont( f);
}

void DiceButton::adjustInnerLabelGeometry()
{
    innerLabel->adjustSize();
    innerLabel->setGeometry(width()/2 - innerLabel->width()/2 + innerLabelShiftX,
                            height()/2 - innerLabel->height()/2 + innerLabelShiftY,
                            innerLabel->width(),
                            innerLabel->height());
}

void DiceButton::mousePressEvent(QMouseEvent *event)
{
    int val = roll(1, type);
    innerLabel->setText(QString::number(val));
    adjustInnerLabelGeometry();
}
