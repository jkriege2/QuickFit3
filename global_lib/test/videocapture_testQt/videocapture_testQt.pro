
QT       += core gui

TARGET = videocapture_testQt
TEMPLATE = app


SOURCES += main.cpp\
    ../../tools.cpp \
    widget.cpp \
    videocaptureuserpropertywindow.cpp \
    ../../graphics_tools.cpp

HEADERS  += \
    ../../videocapture.h \
    ../../tools.h \
    widget.h \
    videocaptureuserpropertywindow.h \
    ../../graphics_tools.h

unix {
  LIBS +=
  SOURCES += ../../videocapture_v4l2.cpp
}

win32 {
  LIBS += -lgdi32 -luser32 -lkernel32 -lvfw32
  SOURCES += ../../videocapture_vfw.cpp
}

FORMS += \
    widget.ui
