#ifndef RADIALMENU2_H
#define RADIALMENU2_H

#include <QWidget>

#include <QPushButton>
#include <QRegion>

//#include <QPalette>

class RadialMenu2 : public QWidget
{
    Q_OBJECT
public:
    explicit RadialMenu2(QWidget *parent = 0);
    void addButton(QPushButton* button);
    void calculateGeometry();
    void setPos(int x, int y);

    void setCentralButton(QPushButton *value);
    void conectToButton(QPushButton * btn, bool setCentralBtn = true); //nastavi tlacitko ktere aktivuje toto menu, vytvori centralni tlacitko pro navrat do predchoziho menu

signals:

public slots:
    void showOnSameCoordinater();
    void show();

protected:
    QVector<QPushButton*> buttons;
    QPushButton* centralButton = NULL;
    int radius = 50;
    int buttonsSize = 50;
};


#endif // RADIALMENU2_H
