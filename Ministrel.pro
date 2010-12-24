#-------------------------------------------------
#
# Project created by QtCreator 2010-12-17T07:05:56
#
#-------------------------------------------------

QT       += core gui

TARGET = Ministrel
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    speakthread.cpp

HEADERS  += mainwindow.h \
    speakthread.h \
    ../../../../../../usr/include/espeak/speak_lib.h

FORMS    += mainwindow.ui
LIBS += -lespeak
target.path=/usr/bin/
INSTALLS+= target

OTHER_FILES += \
    TODO.txt
