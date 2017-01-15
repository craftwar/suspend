QT += core
QT -= gui

CONFIG += c++11

TARGET = suspend
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

win32:LIBS += -lKernel32

DISTFILES += \
    doc.txt
