#include "actionaoe.h"

ActionAoE::ActionAoE(const QDomNode &node) : ActionVsSave(node)
{
    aoeEffectSize = node.firstChildElement("aoeEffectSize").text().toInt();
    aoeType = enums::Maps::translateAoeEffectType(node.firstChildElement("aoeEffectType").text());
    frendlyFire = (node.firstChildElement("frendlyFire").text().toInt() == 0) ? false : true;

    waitOnAoE = new QEventLoop();
}

void ActionAoE::displayRange()
{
    switch (aoeType) {
    case sphere:
        displaySphere();
        break;
    case cube:
        displayCube();
    case cone:
        displayCone();
    default:
        break;
    }
}

void ActionAoE::deleteRange()
{
    if (aoeEffect != NULL) {
        delete aoeEffect;
    }
    deleteRangeCircle();
}

QList<ActionVsSave::unitLos *> ActionAoE::getTargets()
{
    QList<unitLos *> ret;
    unitLos* retNode;

    displayRange();
    int stateTmp = mainWindow->state;
    mainWindow->getBoardState(-2);

    QTimer * cancelTimer = new QTimer();
    QMetaObject::Connection timer_connection = QObject::connect(mainWindow, &MainWindow::playerOnBoard, [this, cancelTimer](int playerID){
        if (playerID == this->unit->ownerID)
            cancelTimer->start(1000);
    });
    QMetaObject::Connection cancel_connection = QObject::connect(cancelTimer, &QTimer::timeout, [this](){
        deleteRange();
        this->aoeEffect = NULL;
        this->waitOnAoE->exit();
    });
    mainWindow->playerOKButton[this->unit->ownerID]->show();
    QMetaObject::Connection ok_connection = QObject::connect(mainWindow->playerOKButton[this->unit->ownerID], &QPushButton::clicked,
            waitOnAoE, &QEventLoop::exit);  //potvrzeni


    cancelTimer->start(1000);
    waitOnAoE->exec();

    mainWindow->playerOKButton[this->unit->ownerID]->hide();

    QObject::disconnect(timer_connection);
    QObject::disconnect(cancel_connection);
    QObject::disconnect(ok_connection);
    delete cancelTimer;
    mainWindow->getBoardState(stateTmp);

    if (aoeEffect == NULL) {    //cancel akce
        deleteRange();
        return ret;
    }

    QList<QGraphicsItem *>  itemList = aoeEffect->collidingItems();
    for (int i = 0; i < itemList.size(); i++) {
        TileUnit* target = dynamic_cast<TileUnit*>(itemList.at(i));
        if (target != NULL) {
            if (target == this->unit) { //preskoci hrace ktery kouzlo pouzil, todo: pridat prepinani v xml souboru akce
                continue;
            }
            if (!frendlyFire) { //pokud je vypnut frendly fire preskoci jednotky se stejnym tymem
                if (target->team == unit->team) {
                    continue;
                }
            }
            retNode = new unitLos;
            retNode->unit = target;
            retNode->los = clearLoS; //todo: dodelat LoS
            ret.append(retNode);
        }
    }

    deleteRange();

    return ret;
}

void ActionAoE::displaySphere()
{
    displayRangeOnUnit(range);

    int pxRadius = aoeEffectSize * 10;
    aoeEffect = new AoESphere(0, 0, pxRadius * 2, pxRadius * 2, rangeGraphicItemCircle);

    aoeEffect->setPos(range *10, range *10);
}

void ActionAoE::displayCube()
{
    displayRangeOnUnit(range);

    int pxEffectSize = aoeEffectSize * 10;
    aoeEffect = new AoECube(QRectF(0, 0, pxEffectSize, pxEffectSize), rangeGraphicItemCircle);

    aoeEffect->setPos(range *10, range *10);
}

void ActionAoE::displayCone()
{
    displayRangeOnUnit(range);

    int pxEffectSize = aoeEffectSize * 10;
    aoeEffect = new AoECone(pxEffectSize, rangeGraphicItemCircle);

    aoeEffect->setPos(range *10, range *10);
}


AoECube::AoECube(const QRectF &rect, QGraphicsItem *parent) : QGraphicsRectItem(rect, parent)
{
    setBrush(QBrush(QColor(0, 0, 255, 150)));

    this->setTransformOriginPoint(rect.x() + rect.width()/2,rect.y());

    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
}

void AoECube::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->buttons() == Qt::RightButton) {

        QPointF p1 = this->mapToScene( transformOriginPoint());
        QPointF p2 = mInitialPos;
        QPointF p3 = event->scenePos();

        mInitialPos = event->scenePos();

        p2 -= p1;
        p3 -= p1;

        double angle = (atan2(p2.y(), p2.x()) - atan2(p3.y(), p3.x())) * 180/3.14159265359;

        prepareGeometryChange();
//        this->setTransformOriginPoint(/*p1*/0,0);
        this->setRotation((this->rotation() - angle));

        event->accept();
    }
    QGraphicsItem::mouseMoveEvent(event);
}

void AoECube::mousePressEvent(QGraphicsSceneMouseEvent *event)
{    
    if (event->buttons() == Qt::RightButton) {
        mInitialPos = event->scenePos();

        event->accept();
    }
    QGraphicsItem::mousePressEvent(event);
}

QVariant AoECube::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene()) {
        // value is the new position.
        QPointF newPos = value.toPointF();
        newPos += transformOriginPoint();

        if (!parentItem()->contains(newPos)) {
            // Keep the item inside the scene rect.
            QPoint P0 = parentItem()->boundingRect().center().toPoint();
            int radius = parentItem()->boundingRect().height()/2;

            double theta = atan2(newPos.y() - P0.y(), newPos.x() - P0.x());
            newPos.setX( P0.x() + radius * cos(theta));
            newPos.setY( P0.y() + radius * sin(theta));

            newPos -= transformOriginPoint();

            return newPos;
        }
    }

    return QGraphicsItem::itemChange(change, value);
}


AoESphere::AoESphere(qreal x, qreal y, qreal width, qreal height, QGraphicsItem *parent) :
    QGraphicsEllipseItem(x, y, width, height, parent)
{
    setBrush(QBrush(QColor(0, 0, 255, 150)));

    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
}

QVariant AoESphere::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene()) {
        // value is the new position.
        QPointF newPos = value.toPointF();
        newPos.setX(newPos.x() + this->boundingRect().width()/2);
        newPos.setY(newPos.y() + this->boundingRect().height()/2);

        if (!parentItem()->contains(newPos)) {
            // Keep the item inside the scene rect.
            QPoint P0 = parentItem()->boundingRect().center().toPoint();
            int radius = parentItem()->boundingRect().height()/2;

            double theta = atan2(newPos.y() - P0.y(), newPos.x() - P0.x());
            newPos.setX( P0.x() + radius * cos(theta));
            newPos.setY( P0.y() + radius * sin(theta));

            newPos.setX(newPos.x() - this->boundingRect().width()/2 );
            newPos.setY(newPos.y() - this->boundingRect().height()/2);

            return newPos;
        }
    }

    return QGraphicsItem::itemChange(change, value);
}

AoECone::AoECone(qreal size, QGraphicsItem *parent) : QGraphicsPolygonItem(parent)
{
    setPolygon(QPolygonF( QVector<QPointF>() << QPointF( 0, 0 ) << QPointF( 0, size ) << QPointF( size, size/2 ) ));
    setTransformOriginPoint(QPointF( size, size/2 ));

    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setAcceptTouchEvents(true);

    setBrush(QBrush(QColor(0, 0, 255, 150)));        
}

bool AoECone::sceneEvent(QEvent *event)
{
    if (event->type() == QEvent::TouchBegin) {
        QTouchEvent* tevent = dynamic_cast<QTouchEvent*>(event);
            mInitialPos = tevent->touchPoints().first().scenePos();
            return true;
    }
    if (event->type() == QEvent::TouchUpdate) {
        QTouchEvent* tevent = dynamic_cast<QTouchEvent*>(event);
        if (tevent->touchPoints().size() > 1) { //rotace
            QPointF p1 = this->mapToScene( transformOriginPoint());
            QPointF p2 = mInitialPos;
            QPointF p3 = tevent->touchPoints().first().scenePos();

            mInitialPos = tevent->touchPoints().first().scenePos();

            p2 -= p1;
            p3 -= p1;

            double angle = (atan2(p2.y(), p2.x()) - atan2(p3.y(), p3.x())) * 180/3.14159265359;

            setRotation((rotation() - angle));

            return true;
        }
        if (tevent->touchPoints().size() == 1) { //pohyb
            QPointF p1 = tevent->touchPoints().first().scenePos();
            setPos(pos().x() + p1.x() - mInitialPos.x(),
                   pos().y() + p1.y() - mInitialPos.y());
            mInitialPos = p1;
            return true;
        }
    }
    return QGraphicsItem::sceneEvent(event);
}

QVariant AoECone::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene()) {
        // value is the new position.
        QPointF newPos = value.toPointF();
        newPos += transformOriginPoint();

        if (!parentItem()->contains(newPos)) {
            // Keep the item inside the scene rect.
            QPoint P0 = parentItem()->boundingRect().center().toPoint();
            int radius = parentItem()->boundingRect().height()/2;

            double theta = atan2(newPos.y() - P0.y(), newPos.x() - P0.x());
            newPos.setX( P0.x() + radius * cos(theta));
            newPos.setY( P0.y() + radius * sin(theta));

            newPos -= transformOriginPoint();

            return newPos;
        }
    }

    return QGraphicsItem::itemChange(change, value);
}
