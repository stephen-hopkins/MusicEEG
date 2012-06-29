#-------------------------------------------------
#
# Project created by QtCreator 2012-06-11T15:37:39
#
#-------------------------------------------------

QT       += core gui
QT       += sql
QT       += phonon

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MusicEEG
TEMPLATE = app


SOURCES += main.cpp\
        mainw.cpp \
    headset.cpp \
    database.cpp \
    meexception.cpp

HEADERS  += mainw.h \
    headset.h \
    EmoStateDLL.h \
    edkErrorCode.h \
    edk.h \
    database.h \
    meexception.h

FORMS    += mainw.ui

win32: LIBS += -L$$PWD/../../Steve/EEG/ -ledk
win32: LIBS += -L$$PWD/../../Steve/EEG/ -ledk_utils

INCLUDEPATH += $$PWD/../../Steve/EEG
DEPENDPATH += $$PWD/../../Steve/EEG





