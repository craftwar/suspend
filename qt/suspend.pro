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

QMAKE_CXXFLAGS += -std:c++latest -Zc:__cplusplus
QMAKE_CXXFLAGS_RELEASE += -Gw -Gy -GL -GS-
QMAKE_LFLAGS_RELEASE += -LTCG
win32:DEFINES += _HAS_STD_BYTE=0 # may be required to workaround WinSDK Byte issue

if (contains(QMAKE_HOST.arch, x86_64) | $$USE_AVX2) {
    QMAKE_CXXFLAGS_RELEASE += -arch:AVX2
}
