#include "myscene.h"

MyScene::MyScene()
{        
}

void MyScene::setAceptTouchEvents()
{
    //    views().first()->setAttribute(Qt::WA_AcceptTouchEvents);
}

void MyScene::release()
{
    if ((TileSelectButton::selected != NULL) && itemToDraw != NULL) {
        QList<QGraphicsItem *> collides = itemToDraw->collidingItems();
        for (int i = 0; i < collides.size(); i++)
        {
            if (TileMap* tmp = dynamic_cast<TileMap *> (collides[i])) {

                    tmp->changeTo(TileSelectButton::selected->text());
            }
        }

        this->removeItem(itemToDraw);
        delete itemToDraw;
        itemToDraw = NULL;
    }
}

void MyScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (TileSelectButton::selected != NULL) {
        if (itemToDraw != NULL) {
            this->removeItem(itemToDraw);
            delete itemToDraw;
            itemToDraw = NULL;
        }
        itemToDraw = new QGraphicsLineItem;
        itemToDraw->setPen(QPen(Qt::black, 3, Qt::SolidLine));
        itemToDraw->setLine(origPoint.x(), origPoint.y(),event->scenePos().x(),event->scenePos().y());
        this->addItem(itemToDraw);
    }
    QGraphicsScene::mouseMoveEvent(event);
//    event->accept();
}

void MyScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    //qDebug()<<"MyScene press"<<event->source();
    if (TileSelectButton::selected != NULL)
        this->origPoint = event->scenePos();
    QGraphicsScene::mousePressEvent(event);
//    event->accept();
}

void MyScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    //qDebug()<<"MyScene release"<<event->source();
    release();
    QGraphicsScene::mouseReleaseEvent(event);
    //    event->accept();
}




MyGraphicsView::MyGraphicsView() : QGraphicsView()
{
    setAttribute(Qt::WA_AcceptTouchEvents);
}

bool MyGraphicsView::event(QEvent *event)
{
//    qDebug()<<"eventView" << event->type();
    if ((event->type() == QEvent::TouchBegin)||(event->type() == QEvent::TouchUpdate)||(event->type() == QEvent::TouchEnd)) {
        if (!event->isAccepted()) {
            touchEvent(event);
            event->accept();
            return true;
        }
        else {
            return false;
        }
    }
//    return QGraphicsView::event(event);
}

void MyGraphicsView::touchEvent(QEvent *event)
{
    QTouchEvent* tevent = dynamic_cast<QTouchEvent*>(event);
    qDebug() << tevent->touchPoints().size();
    if (tevent->touchPoints().size() > 1) {
        this->setDragMode(QGraphicsView::ScrollHandDrag);
        this->setInteractive(false);
    }
    else {
        this->setDragMode(QGraphicsView::NoDrag);
        this->setInteractive(true);
    }
}
