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
        filter.cpp


HEADERS  += mainwindow.h \
        oscillator.h \
        oscillatorsource.h \
        envelope.h \
        modulation.h \
        addition.h \
        filter.h

FORMS    += mainwindow.ui

DISTFILES +=

include(../audioEngine/audioplayer.pri)
include(../drumstick/drumstick.pro)
