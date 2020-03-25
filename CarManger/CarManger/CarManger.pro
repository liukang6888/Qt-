#-------------------------------------------------
#
# Project created by QtCreator 2020-03-19T11:12:56
#
#-------------------------------------------------

QT       += core gui sql xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CarManger
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    domxml.cpp \
    histogram.cpp \
    pie.cpp \
    login.cpp \
    resetpwd.cpp




HEADERS  += mainwindow.h \
    domxml.h \
    histogram.h \
    pie.h \
    login.h \
    resetpwd.h





FORMS    += mainwindow.ui \
    login.ui \
    resetpwd.ui

RESOURCES += \
    image.qrc

CONFIG  += C++11
