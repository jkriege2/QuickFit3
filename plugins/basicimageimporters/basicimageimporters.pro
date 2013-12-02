TEMPLATE = lib
CONFIG += plugin

TARGET = importers_basicimages
DEPENDPATH += ./

include(../plugins.pri)
include(../../extlibs/zlib.pri)
include(../../extlibs/tiff.pri)
DESTDIR = $$QFOUTPUT/plugins

# Input
HEADERS += qfbasicimageimporters.h \
           qfimagereaderlibtiff.h \
    ../../../../../LIB/trunk/libtiff_tools.h \
    qfimagereaderradhard.h \
    ../../../../../LIB/trunk/crc_ccitt.h \
    ../../../../../LIB/trunk/crc.h \
    ../../../../../LIB/trunk/frame.h \
    qfbasicimageimportersoptionswidget.h \
    qfimagereadertinytiff.h \
    ../../../../../LIB/trunk/tinytiffwriter.h \
    ../../../../../LIB/trunk/tinytiffreader.h \
    qfimagereaderdiffusion4binary.h

SOURCES += qfbasicimageimporters.cpp\
           qfimagereaderlibtiff.cpp \
    ../../../../../LIB/trunk/libtiff_tools.cpp \
    qfimagereaderradhard.cpp \
    qfbasicimageimportersoptionswidget.cpp \
    qfimagereadertinytiff.cpp \
    ../../../../../LIB/trunk/tinytiffwriter.cpp \
    ../../../../../LIB/trunk/tinytiffreader.cpp \
    qfimagereaderdiffusion4binary.cpp

FORMS = \
    qfbasicimageimportersoptionswidget.ui

RESOURCES += \
    basicimageimporters.qrc

TRANSLATIONS= ./translations/de.importers_basicimages.ts


INCLUDEPATH += ../../lib/


QT += core
CONFIG += exceptions rtti stl
