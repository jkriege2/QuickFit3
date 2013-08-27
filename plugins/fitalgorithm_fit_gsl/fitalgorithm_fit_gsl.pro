TEMPLATE = lib
CONFIG += plugin

TARGET = fit_gsl
DEFINES += TARGETNAME=$$TARGET

DEPENDPATH += ./
include(../plugins.pri)
include(../../extlibs/gsl.pri)

DESTDIR = $${QFOUTPUT}/plugins/


# Input
HEADERS += qffitalgorithmgslplugin.h \
		   qffitalgorithmgslsimplex.h \ 
    qffitalgorithmgsltools.h \
    qffitalgorithmgslsimplexconfigdialog.h \
    qffitalgorithmgslsimplexrand.h \
    qffitalgorithmgslderivconfigdialog.h \
    qffitalgorithmgslderivbase.h \
    qffitalgorithmgslbfgs2.h \
    qffitalgorithmgslsteepest_descent.h \
    qffitalgorithmgslconjugate_pr.h \
    qffitalgorithmgslconjugate_fr.h

SOURCES += qffitalgorithmgslplugin.cpp \
		   qffitalgorithmgslsimplex.cpp \
    qffitalgorithmgsltools.cpp \
    qffitalgorithmgslsimplexconfigdialog.cpp \
    qffitalgorithmgslsimplexrand.cpp \
    qffitalgorithmgslderivconfigdialog.cpp \
    qffitalgorithmgslderivbase.cpp \
    qffitalgorithmgslbfgs2.cpp


FORMS += \
    qffitalgorithmgslsimplexconfigdialog.ui \
    qffitalgorithmgslderivconfigdialog.ui

RESOURCES +=

TRANSLATIONS= ../../output/assets/translations/de.fit_gsl.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

SRC_DISTRIBUTED.files = $$HEADERS \
                        $$SOURCES \
                        $$FORMS \
                        $$RESOURCES \
                        $$TRANSLATIONS \
                        $$GPL3LICENSE
QT += gui xml svg
CONFIG += exceptions rtti stl
