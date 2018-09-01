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

QMAKE_CXXFLAGS += -std:c++latest
QMAKE_CXXFLAGS_RELEASE += -Gw -Gy -GL -GS-
QMAKE_LFLAGS_RELEASE += -LTCG

if (contains(QMAKE_HOST.arch, x86_64) | $$USE_AVX2) {
    QMAKE_CXXFLAGS_RELEASE += -arch:AVX2
}
