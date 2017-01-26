#ifndef RADIALMENU_H
#define RADIALMENU_H

#include <QWidget>

#include <QPalette>
#include <QLayout>
#include <QPushButton>

#include <math.h>

#include <QDebug>




#define M_PI 3.14159265358979323846

class RadialButton;
class ButtonLayer;
//class RadualMenu;

//class RadialMenu : public QWidget
//{

//public:
//    explicit RadialMenu(QWidget *parent = 0);
//    RadialMenu *setL1(const std::vector<QString> &labels);


//signals:

//public slots:

//private:
//    ButtonLayer * l1;
//    ButtonLayer * l2;
//};

/////
/// \brief The ButtonLayer class
///
class ButtonLayer : public QWidget
{
    Q_OBJECT
public:
    ButtonLayer(const std::vector<QString> &labels, QWidget * parent = 0);
    void setGeometry(int x, int y, int width, int height);
    void togleShow();
    void setNextLayers(std::vector<ButtonLayer *> layers);
    
    int getRadius() const;
    void setRadius(int value);

    int getButtonsSize() const;
    void setButtonsSize(int value);

    int getSize() const;

public slots:
    //void getreturnValue();

private:
    int size = 0;
    int radius = 75;
    int buttonsSize = 50;
    std::vector<RadialButton *> buttons;
    ButtonLayer *setButtonGeometry();

    QPushButton* hideButton;
};


///
/// \brief The RadialButton class
///
class RadialButton : public QPushButton
{
    Q_OBJECT
public:
    RadialButton(QWidget *parent = 0);
    ButtonLayer *nextLayer = NULL;
    void setNextLayer(ButtonLayer *value);

public slots:
    void click();
    void animateClick();
};

#endif // RADIALMENU_H
