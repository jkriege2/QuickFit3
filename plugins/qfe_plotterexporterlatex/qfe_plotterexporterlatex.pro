TEMPLATE = lib
CONFIG += plugin

TARGET = qfe_plotterexporterlatex
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../plugins.pri)

include(../../libquickfitwidgets.pri)

DESTDIR = $${QFOUTPUT}/plugins/

# Input
HEADERS += qfeplotterexporterlatex.h \
    jkqtplatexengineadapter.h \
    QTeXEngine/src/QTeXEngine.h

SOURCES += qfeplotterexporterlatex.cpp \
    jkqtplatexengineadapter.cpp \
    QTeXEngine/src/QTeXPaintEngine.cpp \
    QTeXEngine/src/QTeXPaintDevice.cpp

win32 {
  DEFINES += HAS_EMF_ENGINE
  SOURCES += EmfEngine/src/EmfEngine.cpp \
             EmfEngine/src/EmfPaintDevice.cpp \
             jkqtpemfengineadapter.cpp

  HEADERS += EmfEngine/src/EmfEngine.h \
             jkqtpemfengineadapter.h
}

FORMS =

RESOURCES += qfeplotterexporterlatex.qrc

TRANSLATIONS= ./translations/de.qfe_plotterexporterlatex.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg
CONFIG += exceptions rtti stl




SRC_DISTRIBUTED.files = $$HEADERS \
                        $$SOURCES \
                        $$FORMS \
                        $$RESOURCES \
                        $$TRANSLATIONS \
                        $$GPL3LICENSE \
                        QTeXEngine-0.3-opensource.zip \
                        EmfEngine-opensource-0.8.zip
