#ifndef RADIALMENU2_H
#define RADIALMENU2_H

#include <QWidget>
#include <QGraphicsProxyWidget>

#include <QTimer>

#include <QPushButton>
#include <QRegion>

#include <QDebug>
#include <QGraphicsView>

class RadialMenu2 : public QWidget
{
    Q_OBJECT
public:
    explicit RadialMenu2(QWidget *parent = 0);
//    ~RadialMenu2();
    void addButton(QPushButton* button);
    void calculateGeometry();
    void setPos(int x, int y);
    void setRotation(int player);
    void setRotation();

    void setCentralButton(QPushButton *value);
    void conectToButton(QPushButton * btn, bool setCentralBtn = true); //nastavi tlacitko ktere aktivuje toto menu, vytvori centralni tlacitko pro navrat do predchoziho menu

    QGraphicsProxyWidget* proxyWidget;


    bool getCanBeShown() const;
    void setCanBeShown(bool value);
    bool isOnCoordinates(QPoint point);

    void setCanBeShown2(bool value);

signals:

public slots:
    void showOnSameCoordinater();
    void show();
    void show1s();

protected:
    QVector<QPushButton*> buttons;
    QPushButton* centralButton = NULL;
    int radius = 100;
    int buttonsSize = 75;
    QTimer *hideTimer;
    bool canBeShown = false;
    bool canBeShown2 = true;
    int playerID;
};


#endif // RADIALMENU2_H
