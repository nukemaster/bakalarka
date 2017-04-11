#ifndef MYSCENE_H
#define MYSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsPixmapItem>
#include "tilemap.h"
#include <QTouchEvent>

#include <QPushButton>

#include <QGraphicsView>
#include <QScroller>

class TileSelectButton;

class MyGraphicsView : public QGraphicsView
{
public:
    MyGraphicsView();
    bool event(QEvent *event);
    void touchEvent(QEvent* event);
};

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
