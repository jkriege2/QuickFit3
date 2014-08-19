TEMPLATE = lib
CONFIG += plugin

TARGET = qffcsfitfuncs
DEPENDPATH += ./

include(../plugins.pri)

DESTDIR = $$QFOUTPUT/plugins
#DLLDESTDIR = $$DESTDIR



# Input
HEADERS += fcs_fitfuctions.h \
           qffitfunctionfcsmultidiffusion.h \
           qffitfunctionfcsadiff.h \
           qffitfunctionfcsdiff.h \
           qffitfunctionfcssimplediff.h \
    qffitfunctionfcsflow.h \
    qffitfunctionfcsrot.h \
    qffitfunctionfcsoscillation.h \
    qffitfunctionfcsdiffD.h \
    qffitfunctionfcsflowv.h \
    qffitfunctioncsfcs.h \
    qffitfunctionfcsexp.h \
    qffitfunctionfcssimpleexp.h \
    qffitfunctionfcspowerlaw.h \
    qffitfunctionfcsdiffDAfterpulse.h \
    qffitfunctionfcsdiff_relbright.h \
    qffitfunctionfcsadiff_relbright.h \
    qffitfunctionfcsdiff2d_relbright.h

SOURCES += fcs_fitfuctions.cpp\
           qffitfunctionfcsmultidiffusion.cpp \
           qffitfunctionfcsadiff.cpp \
           qffitfunctionfcsdiff.cpp \
           qffitfunctionfcssimplediff.cpp \
    qffitfunctionfcsflow.cpp \
    qffitfunctionfcsrot.cpp \
    qffitfunctionfcsoscillation.cpp \
    qffitfunctionfcsdiffD.cpp \
    qffitfunctionfcsflowv.cpp \
    qffitfunctioncsfcs.cpp \
    qffitfunctionfcsexp.cpp \
    qffitfunctionfcssimpleexp.cpp \
    qffitfunctionfcspowerlaw.cpp \
    qffitfunctionfcsdiffDAfterpulse.cpp \
    qffitfunctionfcsdiff_relbright.cpp \
    qffitfunctionfcsadiff_relbright.cpp \
    qffitfunctionfcsdiff2d_relbright.cpp

FORMS +=

RESOURCES +=

TRANSLATIONS += ./translations/de.qffcsfitfuncs.ts


INCLUDEPATH += ../../lib/


QT += core
CONFIG += exceptions rtti stl


SRC_DISTRIBUTED.files = $$HEADERS \
                        $$SOURCES \
                        $$FORMS \
                        $$RESOURCES \
                        $$TRANSLATIONS \
                        $$GPL3LICENSE





