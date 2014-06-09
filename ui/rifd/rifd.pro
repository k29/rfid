#-------------------------------------------------
#
# Project created by QtCreator 2014-06-09T19:13:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = rifd
TEMPLATE = app


SOURCES += main.cpp\
        rfid.cpp \
    serial.cpp \
    tag.cpp

HEADERS  += rfid.h \
    serial.h \
    tag.h

FORMS    += rfid.ui
