#include "radialmenu2.h"

RadialMenu2::RadialMenu2(QWidget *parent) : QWidget(parent)
{

}

void RadialMenu2::addButton(QPushButton *button)
{
    button->setParent(this);
    buttons.append(button);
}

void RadialMenu2::calculateGeometry()
{
    int x, y;
    int size = this->buttons.size();
    double angle = (2* M_PI) / size;
    double angle2 = 0.0;

    while ((2 * M_PI * radius) < (size * (buttonsSize + 10) )) {   //nastaveni prumeru aby se vesly vsechny tlacitka na kruh, 10-konstanta aby se tlacitka primo nedotykala
        radius += 1;
    }

    //sirka a vyska menu
    QWidget::setGeometry(this->pos().x(), this->pos().y(),  2*radius + buttonsSize, 2*radius + buttonsSize);

    //maska menu
    QRegion* region = new QRegion(*(new QRect(0, 0,2*radius + buttonsSize,2*radius + buttonsSize )),QRegion::Ellipse);
    this->setMask(*region);
    delete region;

    QPushButton * tmp;
    region = new QRegion(*(new QRect(0, 0,buttonsSize,buttonsSize )),QRegion::Ellipse); //maska tlacitek
    for (int i = 0; i < size; i++ ) {   //jednotliva tlacitka
        tmp = buttons[i];
        x = radius * sin(angle2);
        y = radius * sin(M_PI / 2.0 - angle2);

        x += this->width() / 2  - buttonsSize / 2;
        y += this->height() / 2 - buttonsSize / 2;

        tmp->setGeometry(x, y, buttonsSize , buttonsSize);
        tmp->setMask(*region);

        angle2 += angle;
    }

    //centralBtn
    if (centralButton != NULL) {
        centralButton->setGeometry(this->width()/2 - buttonsSize/2, this->height()/2 - buttonsSize/2, buttonsSize, buttonsSize);
        centralButton->setMask(*region);
    }
    delete region;
}

void RadialMenu2::setPos(int x, int y)
{
    this->setGeometry(x-width()/2, y-height()/2, width(), height());
}

void RadialMenu2::setCentralButton(QPushButton *value)
{
    value->setParent(this);
    centralButton = value;
}

void RadialMenu2::conectToButton(QPushButton *btn, bool setCentralBtn)
{
    RadialMenu2* btnParent = dynamic_cast<RadialMenu2*>(btn->parent());
    if (btnParent == NULL) { //tlacitko neni v RadialMenu
        setCentralBtn = false;                
    }
    else {  //tlacitko je v RadialMenu
        QObject::connect(btn, SIGNAL(clicked(bool)), btnParent, SLOT(hide()));  //schovani rodice
    }
    QObject::connect(btn, SIGNAL(clicked(bool)), this, SLOT(showOnSameCoordinater())); //ukazani cile

    if (setCentralBtn) {
        centralButton = new QPushButton("zpet", this);
        btnParent->conectToButton(centralButton, false);
    }
}

void RadialMenu2::showOnSameCoordinater()
{
    RadialMenu2* sender = dynamic_cast<RadialMenu2*>(QObject::sender()->parent());
    show();
    if (sender != NULL) {   //nastaveni stejne pozice jako rodic
        setPos(sender->pos().x() + sender->width()/2,
               sender->pos().y() + sender->height()/2);
    }
}

void RadialMenu2::show()
{
    calculateGeometry();
    QWidget::show();
}



