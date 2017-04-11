#include "radialmenu2.h"

RadialMenu2::RadialMenu2(QWidget *parent) : QWidget(parent)
{
    hideTimer = new QTimer(this);
    connect(hideTimer, SIGNAL(timeout()), this, SLOT(hide())); //skryti pori timeoutu
    hideTimer->setSingleShot(true);

    this->setStyleSheet("RadialMenu2 {background:transparent;}");
}

//RadialMenu2::~RadialMenu2()
//{
//    if (centralButton != NULL) {
//        centralButton->deleteLater();
//    }
//    for (int i = 0; i < buttons.size(); i++) {
//        buttons[i]->deleteLater();
//    }
//}

void RadialMenu2::addButton(QPushButton *button)
{
    button->setParent(this);
    buttons.append(button);
}

void RadialMenu2::calculateGeometry()
{
    this->proxyWidget->setTransformOriginPoint(this->width()/2, this->height()/2);
    int x, y;
    int size = this->buttons.size();    //pocet tlacitek
    double angle = (/*2* */M_PI) / (size-1);    //uhel mezi dvema tlacitkama a stredem
    double angle2 = 1.57;

    while ((/*2 * */M_PI * radius) < (size * (buttonsSize + 10) )) {   //nastaveni prumeru aby se vesly vsechny tlacitka na kruh, 10-konstanta aby se tlacitka primo nedotykala
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
//        tmp->setStyleSheet("padding: 0px; border: 0px;");
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

void RadialMenu2::setRotation(int player)
{
    this->playerID = player;
    if (proxyWidget == NULL) {
        return;
    }

    proxyWidget->setTransformOriginPoint(height()/2, width()/2);

    switch (playerID) {
    case 0:
        proxyWidget->setRotation(180);
        break;
    case 1:
        proxyWidget->setRotation(90);
        break;
    case 2:
        proxyWidget->setRotation(0);
        break;
    case 3:
        proxyWidget->setRotation(270);
        break;
    default:
        break;
    }
}

void RadialMenu2::setRotation()
{
    setRotation(playerID);
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
        QObject::connect(btn, &QPushButton::clicked,  [btnParent](){ btnParent->setCanBeShown(false);});
        QObject::connect(btn, SIGNAL(clicked(bool)), btnParent, SLOT(hide()));  //schovani rodice
    }
    QObject::connect(btn, &QPushButton::clicked, [this](){this->setCanBeShown(true);});
    QObject::connect(btn, SIGNAL(clicked(bool)), this, SLOT(showOnSameCoordinater())); //ukazani cile


    if (setCentralBtn) {
        centralButton = new QPushButton("zpet", this);        
        btnParent->conectToButton(centralButton, false);        
    }
}

void RadialMenu2::showOnSameCoordinater()
{    
    RadialMenu2* sender = dynamic_cast<RadialMenu2*>(QObject::sender()->parent());    
    sender->proxyWidget->setRotation(0); //reset rotace
    proxyWidget->setRotation(0);    //reset rotace
    show();
    if (sender != NULL) {   //nastaveni stejne pozice jako rodic
        setPos(sender->pos().x() + sender->width()/2,
               sender->pos().y() + sender->height()/2);
    }
    this->setRotation();    //znovu otoceni do spravne polohy
    sender->setRotation();  // znovuotoceni do spravne polohy
}

void RadialMenu2::show()
{
    calculateGeometry();
    QWidget::show();
}

void RadialMenu2::show1s()
{
    if (getCanBeShown()) {
        //DEBUG
        this->show();
        return;
        //!DEBUG*
        show();
        hideTimer->start(1000);
    }
}

void RadialMenu2::setCanBeShown2(bool value)
{
    canBeShown2 = value;
    if (value == false) {
        this->hide();
    }
    else { //DEBUG
        show1s(); //DEBUG
    }   //DEBUG
}

bool RadialMenu2::getCanBeShown() const
{    
    return canBeShown && canBeShown2;
}

void RadialMenu2::setCanBeShown(bool value)
{
    if (!value) {
        hide();
    }
    canBeShown = value;
}

bool RadialMenu2::isOnCoordinates(QPoint point)
{
    QGraphicsView *v = proxyWidget->scene()->views().last();
    QPointF sceneP = /*this->proxyWidget->mapToScene*/(this->proxyWidget->pos() );
    QPoint viewP = v->mapFromScene(sceneP);
    QPoint realP = v->viewport()->mapToGlobal(viewP);
//    qDebug()<<point<<"..."<<realP << width() << height();

    if ((point.x() > realP.x()) && (point.x() < realP.x() + width()))
        if ((point.y() > realP.y()) && (point.y() < realP.y() + height()))
            return true;
    return false;
}



