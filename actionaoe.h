#ifndef ACTIONAOE_H
#define ACTIONAOE_H

#include <actionvssave.h>

#include <QEventLoop>
#include <QTimer>

#include <QTouchEvent>

class ActionAoE: public ActionVsSave
{
public:
    ActionAoE(const QDomNode &node);
    void displayRange();
    void deleteRange();
    QList<unitLos*> getTargets();


private:
    aoeEffectType aoeType;
    int aoeEffectSize;
    bool frendlyFire;

    QGraphicsItem * aoeEffect;
    QEventLoop* waitOnAoE;

    void displaySphere();
    void displayCube();
    void displayCone();

};

class AoECube : public QGraphicsRectItem
{
public:
    AoECube(const QRectF &rect, QGraphicsItem *parent = Q_NULLPTR);

protected:
    //pohyb
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    //rotace
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void mousePressEvent(QGraphicsSceneMouseEvent * event);

    QPointF mInitialPos;
};

class AoESphere : public QGraphicsEllipseItem
{
public:
    AoESphere(qreal x, qreal y, qreal width, qreal height, QGraphicsItem *parent = Q_NULLPTR);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
};

class AoECone : public QGraphicsPolygonItem
{
public:
    AoECone(qreal size, QGraphicsItem *parent = Q_NULLPTR);

protected:
    //pohyb
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    //rotace
//    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
//    void mousePressEvent(QGraphicsSceneMouseEvent * event);

    QPointF mInitialPos;
//    double touchAngle;

    bool sceneEvent(QEvent *event);
};

#endif // ACTIONAOE_H
