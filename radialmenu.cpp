#include "radialmenu.h"

RadialButton::RadialButton(QWidget *parent) : QPushButton(parent)
{
    QObject::connect(this, SIGNAL(pressed()), this, SLOT(click()));
}

void RadialButton::setNextLayer(ButtonLayer *value)
{
    nextLayer = value;
}

void RadialButton::click()
{
    if (nextLayer == NULL) {
    }
    else {
        nextLayer->togleShow();
    }
}

void RadialButton::animateClick()
{

}



ButtonLayer::ButtonLayer(const std::vector<QString> &labels, QWidget *parent) : QWidget(parent)
{
    QPalette pal = palette();

    // set black background
    pal.setColor(QPalette::Background, Qt::black);
    this->setAutoFillBackground(true);
    this->setPalette(pal);

    ButtonLayer* tmp;
    if ((tmp = dynamic_cast<ButtonLayer*>(parent)) != NULL) {
        radius = tmp->radius + tmp->buttonsSize;
    }
    for (int i = 0; i < labels.size(); i++) {
        RadialButton * tmp = new RadialButton(this);
        tmp->setText(labels[i]);
        buttons.push_back(tmp);
    }
    this->size = labels.size();
    this->hide();

    QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    this ->setSizePolicy(sizePolicy);

    this->hideButton = new QPushButton(this);
    this->hideButton->setText("Back");
    QObject::connect(hideButton, SIGNAL(pressed()), this, SLOT(hide()));

    setButtonGeometry();
}

void ButtonLayer::setGeometry(int x, int y, int width, int height)
{
    QWidget::setGeometry(x, y, width, height);
    //setButtonGeometry();
}

void ButtonLayer::togleShow()
{
    if (isHidden() == true) {
        this->show();
    }
    else {
        this->hide();
    }
}

void ButtonLayer::setNextLayers(std::vector<ButtonLayer *> layers)
{
    if (layers.size() != this->size) {
        return;
    }

    for (int i = 0; i < this->size; i++) {
        RadialButton *tmp = buttons[i];
        tmp->setNextLayer(layers[i]);
        buttons[i] = tmp;
    }
}

ButtonLayer *ButtonLayer::setButtonGeometry()
{
    int x, y;
    int size = this->buttons.size();
    double angle = (2* M_PI) / size;
    double angle2 = 0.0;

    while ((2 * M_PI * radius) < (size * buttonsSize * 1.14142 )) {   //nastaveni prumeru aby se vesly vsechny tlacitka na kruh
        radius += 25;
    }

    QWidget::setGeometry(this->pos().x(), this->pos().y(),  2*radius + buttonsSize, 2*radius + buttonsSize);
    this->setMinimumHeight(2*radius + buttonsSize);
    this->setMinimumWidth(2*radius + buttonsSize);



    QRegion* region = new QRegion(*(new QRect(0, 0,2*radius + buttonsSize,2*radius + buttonsSize )),QRegion::Ellipse);
    //this->setMask(*region);
    delete region;

    region = new QRegion(*(new QRect(0, 0,buttonsSize,buttonsSize )),QRegion::Ellipse);

    this->hideButton->setMask(*region);
    this->hideButton->setGeometry(this->width()/2 - buttonsSize/2, this->height()/2 - buttonsSize/2, buttonsSize, buttonsSize);

    RadialButton * tmp;
    for (int i = 0; i < size; i++ ) {

        tmp = buttons[i];
        x = radius * sin(angle2);
        y = radius * sin(M_PI / 2.0 - angle2);

        x += this->width() / 2  - buttonsSize / 2;
        y += this->height() / 2 - buttonsSize / 2;

        tmp->setGeometry(x, y, buttonsSize , buttonsSize);
        tmp->setMask(*region);

        angle2 += angle;
    }
    delete region;
    return this;
}

int ButtonLayer::getRadius() const
{
    return radius;
}

void ButtonLayer::setRadius(int value)
{
    radius = value;
}

int ButtonLayer::getButtonsSize() const
{
    return buttonsSize;
}

void ButtonLayer::setButtonsSize(int value)
{
    buttonsSize = value;
}

int ButtonLayer::getSize() const
{
    return size;
}

