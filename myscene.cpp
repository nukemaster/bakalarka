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
    event->accept();
}

void MyScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    //qDebug()<<"MyScene press"<<event->source();
    if (TileSelectButton::selected != NULL)
        this->origPoint = event->scenePos();
    QGraphicsScene::mousePressEvent(event);
    event->accept();
}

void MyScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    //qDebug()<<"MyScene release"<<event->source();
    release();
    QGraphicsScene::mouseReleaseEvent(event);
    event->accept();
}
