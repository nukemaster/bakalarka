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
        mainwindow.cpp \
    detection.cpp \
    board.cpp \
    tile.cpp \
    calibration.cpp \
    radialmenu.cpp \
    settings.cpp \
    myscene.cpp \
    myeventfilter.cpp

HEADERS  += mainwindow.h \
    detection.h \
    board.h \
    tile.h \
    calibration.h \
    radialmenu.h \
    settings.h \
    myscene.h \
    myeventfilter.h

FORMS    += mainwindow.ui \
    detection.ui \
    calibration.ui

INCLUDEPATH += $$(KINECTSDK20_DIR)/inc
DEPENDPATH += $$(KINECTSDK20_DIR)/inc

win32: LIBS += -L$$(KINECTSDK20_DIR)/Lib/x86/ -lKinect20

PRE_TARGETDEPS += $$(KINECTSDK20_DIR)/Lib/x86/Kinect20.lib
#else:win32-g++: PRE_TARGETDEPS += $$(KINECTSDK20_DIR)/Lib/x86/libKinect20.a



INCLUDEPATH += $$(OPENCV_DIR)/include
DEPENDPATH += $$(OPENCV_DIR)/include

win32:CONFIG(release, debug|release): LIBS += -L$$(OPENCV_DIR)/x86/vc12/lib/ -lopencv_world300
else:win32:CONFIG(debug, debug|release): LIBS += -L$$(OPENCV_DIR)/x86/vc12/lib/ -lopencv_world300d

win32:LIBS += -luser32

RESOURCES += \
    res.qrc
