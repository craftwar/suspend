QT += core
QT -= gui

TARGET = suspend
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

win32:LIBS += -lKernel32

DISTFILES += \
    doc.txt

QMAKE_CXXFLAGS = -std:c++latest
QMAKE_CXXFLAGS_RELEASE = -Gw -Gy -GL -GS-

contains(QMAKE_HOST.arch, x86_64):{
    QMAKE_CXXFLAGS_RELEASE += -arch:AVX2
}
