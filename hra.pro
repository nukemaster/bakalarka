#-------------------------------------------------
#
# Project created by QtCreator 2016-10-03T17:25:29
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = hra
TEMPLATE = app


SOURCES += main.cpp\
    detection.cpp\
    calibration.cpp\
    mainwindow.cpp \
    board.cpp \
    tile.cpp \
    settings.cpp \
    myscene.cpp \
    myeventfilter.cpp \
    kinect.cpp \
    radialmenu2.cpp \
    dicebutton.cpp

HEADERS  += mainwindow.h \
    detection.h\
    calibration.h\
    board.h \
    tile.h \
    settings.h \
    myscene.h \
    myeventfilter.h \
    kinect.h \
    radialmenu2.h \
    dicebutton.h

FORMS    += mainwindow.ui \
    detection.ui \
    calibration.ui

#openCV
unix:!macx: LIBS += -L$$OPENCV_PATH/lib/ -lopencv_core
unix:!macx: LIBS += -L$$OPENCV_PATH/lib/ -lopencv_highgui
unix:!macx: LIBS += -L$$OPENCV_PATH/lib/ -lopencv_imgproc

INCLUDEPATH += $$OPENCV_PATH/include
DEPENDPATH += $$OPENCV_PATH/include

#freenect2
#unix:!macx: LIBS += -L$$FREENECT_PATH/lib/ -lfreenect2

#INCLUDEPATH += $$FREENECT_PATH/include
#DEPENDPATH += $$FREENECT_PATH/include


RESOURCES += \
    res.qrc

unix:!macx: LIBS += -L$$PWD/../../../../freenect2/lib/ -lfreenect2

INCLUDEPATH += $$PWD/../../../../freenect2/include
DEPENDPATH += $$PWD/../../../../freenect2/include
