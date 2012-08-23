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
    musicplayer.cpp \
    qcustomplot.cpp \
    displayemotion.cpp \
    recommender.cpp

HEADERS  += mainw.h \
    headset.h \
    EmoStateDLL.h \
    edkErrorCode.h \
    edk.h \
    database.h \
    musicplayer.h \
    qcustomplot.h \
    displayemotion.h \
    recommender.h

FORMS    += mainw.ui \
    displayemotion.ui

win32: LIBS += -L$$PWD -ledk
win32: LIBS += -L$$PWD -ledk_utils

INCLUDEPATH += $$PWD/../Emotiv
DEPENDPATH += $$PWD/../Emotiv

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../taglib/taglib/Release/x86/ -ltag
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../taglib/taglib/Debug/x86/ -ltagd

INCLUDEPATH += $$PWD/../taglib/include
DEPENDPATH += $$PWD/../taglib/include

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../taglib/taglib/Release/x86/tag.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../taglib/taglib/Debug/x86/tagd.lib

INCLUDEPATH += $$PWD/../taglib/
DEPENDPATH += $$PWD/../taglib/

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../taglib/taglib/Debug/x86/ -ltag
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../taglib/taglib/Debug/x86/ -ltagd


