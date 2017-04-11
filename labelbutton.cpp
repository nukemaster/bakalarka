#include "labelbutton.h"

LabelButton::LabelButton(QWidget *parent) :QLabel(parent)
{    
    innerLabel = new QLabel(this);
    QFont f( "Arial", 20, QFont::Bold);
    innerLabel->setFont(f);
}

void LabelButton::adjustInnerLabelGeometry()
{
    innerLabel->adjustSize();
    innerLabel->setGeometry(width()/2 - innerLabel->width()/2 + innerLabelShiftX,
                            height()/2 - innerLabel->height()/2 + innerLabelShiftY,
                            innerLabel->width(),
                            innerLabel->height());
}

void LabelButton::setText(QString text)
{
    innerLabel->setText(text);
    innerLabel->adjustSize();
    adjustSize();
}

void LabelButton::setText(int val)
{
    setText(QString::number(val));
}

void LabelButton::mousePressEvent(QMouseEvent *event)
{
    click();
    click(innerLabel->text());
}
