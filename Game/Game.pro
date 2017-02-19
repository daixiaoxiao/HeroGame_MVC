#-------------------------------------------------
#
# Project created by QtCreator 2016-10-12T22:41:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Game
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    arenamodel.cpp \
    arenaview.cpp \
    gameengine.cpp \
    protagonistmodel.cpp \
    enemymodel.cpp \
    healthpackmodel.cpp \
    arenascene.cpp \
    pathfinding2.cpp \
    stategy.cpp \
    myframe.cpp


HEADERS  += mainwindow.h \
    arenamodel.h \
    arenaview.h \
    gameengine.h \
    protagonistmodel.h \
    enemymodel.h \
    healthpackmodel.h \
    arenascene.h \
    node.h \
    pathfinding2.h \
    stategy.h \
    myframe.h

FORMS    += mainwindow.ui

RESOURCES += \
    images.qrc

DISTFILES +=

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../Downloads/libworld.so/release/ -lworld
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../Downloads/libworld.so/debug/ -lworld
else:unix: LIBS += -L$$PWD/../../../../Downloads/libworld.so/ -lworld

INCLUDEPATH += $$PWD/../../../../Downloads/libworld.so
DEPENDPATH += $$PWD/../../../../Downloads/libworld.so
