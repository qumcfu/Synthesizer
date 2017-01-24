#-------------------------------------------------
#
# Project created by QtCreator 2016-12-05T13:19:54
#
#-------------------------------------------------

QT       += core gui widgets multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Synthesizer
TEMPLATE = app

INCLUDEPATH += ../audioEngine \
               ../drumstick
SOURCES += main.cpp\
        mainwindow.cpp \
        oscillator.cpp \
        oscillatorsource.cpp \
        envelope.cpp \
        modulation.cpp \
        addition.cpp \
        filter.cpp \
        detection.cpp


HEADERS  += mainwindow.h \
        oscillator.h \
        oscillatorsource.h \
        envelope.h \
        modulation.h \
        addition.h \
        filter.h \
        detection.h

FORMS    += mainwindow.ui

LIBS += -L/usr/local/lib -L/usr/local/Cellar/opencv3/3.2.0/lib


QT_CONFIG -= no-pkg-config
CONFIG += link_pkgconfig
PKGCONFIG += opencv

DISTFILES += \
    ../haarcascade_eye_tree_eyeglasses.xml \
    ../haarcascade_frontalface_alt.xml \
    ../haarcascade_lefteye_2splits.xml \
    ../haarcascade_righteye_2splits.xml

include(../audioEngine/audioplayer.pri)
include(../drumstick/drumstick.pro)

unix: PKGCONFIG += /usr/local/Cellar/opencv3/3.2.0/lib/pkgconfig/opencv.pc
