TEMPLATE = lib
CONFIG += plugin

TARGET = qfe_helpeditor
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../plugins.pri)

include(../../libquickfitwidgets.pri)

DESTDIR = $${QFOUTPUT}/plugins/

# Input
HEADERS += qfehelpeditor.h \
           qfehelpeditorwidget.h \
    replacedialog.h \
    finddialog.h \
    pasteimagedlg.h \
    selectresourceimage.h \
    pluginlinkdialog.h \
    subpluginlinkdialog.h \
    faqentrydialog.h \
    functionreferencedialog.h

SOURCES += qfehelpeditor.cpp \
           qfehelpeditorwidget.cpp \
    replacedialog.cpp \
    finddialog.cpp \
    pasteimagedlg.cpp \
    selectresourceimage.cpp \
    pluginlinkdialog.cpp \
    subpluginlinkdialog.cpp \
    faqentrydialog.cpp \
    functionreferencedialog.cpp

FORMS =    qfehelpeditorwidget.ui \
    replacedialog.ui \
    finddialog.ui \
    pasteimagedlg.ui \
    selectresourceimage.ui \
    pluginlinkdialog.ui \
    subpluginlinkdialog.ui \
    faqentrydialog.ui \
    functionreferencedialog.ui

RESOURCES += qfehelpeditor.qrc

TRANSLATIONS= ./translations/de.qfe_helpeditor.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

ASSETSe_FILES.files = ./assets/examples/*.*
ASSETSe_FILES.path = $${QFOUTPUT}/assets/plugins/$${TARGET}/examples

ASSETSt_FILES.files = ./assets/templates/*.*
ASSETSt_FILES.path = $${QFOUTPUT}/assets/plugins/$${TARGET}/templates

INSTALLS += ASSETSe_FILES ASSETSt_FILES

QT += gui xml svg
CONFIG += exceptions rtti stl



