#ifndef MYSCENE_H
#define MYSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsPixmapItem>
#include "tile.h"


#include <QPushButton>

class TileSelectButton;

class MyScene : public QGraphicsScene
{
public:
    MyScene();
    void setAceptTouchEvents();
    ///DRAWING
    int drawMode = 0;
    QPointF origPoint;
    QGraphicsLineItem* itemToDraw = NULL;
    void release();

private:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
};

class TileSelectButton : public QPushButton
{
    Q_OBJECT
public:

    TileSelectButton(const QString & text, QWidget * parent = 0);
    //static QString selectedValue;
    static TileSelectButton* selected;

    void select();

    QCursor *cur;

public slots:
    void click();
};
#endif // MYSCENE_H
