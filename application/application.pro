TEMPLATE = app
TARGET = quickfit3
DEPENDPATH += . \
               ../lib/ \
               ../libqf3widgets/
INCLUDEPATH += . \
               ../lib/ \
               ../libqf3widgets/ \
               ../../../../LIB/trunk/ \
               ../../../../LIB/trunk/qt

include(../quickfit3.pri)
include(../libquickfitwidgets.pri)

release {
    message("building $$TARGET $$TEMPLATE in RELEASE mode, output is in $$QFOUTPUT ")
} else {
    message("building $$TARGET $$TEMPLATE in DEBUG mode, output is in $$QFOUTPUT ")
}
macx {
    message("build system is macx")
}
win32 {
    message("build system is win32")
}
unix {
    message("build system is unix")
}


DESTDIR = $$QFOUTPUT

QMAKE_RPATHDIR += $$DESTDIR
# Input
HEADERS += mainwindow.h \
           optionsdialog.h \
    ../version.h \
    qfrdrreplacedialog.h \
    qfhtmlhelpwindow.h \
    dlgcontactauthors.h \
    dlgnewversion.h \
    dlgrdrsetproperty.h \
    dlgselectprojectsubset.h \
    dlgfixfilepaths.h

SOURCES += main.cpp \
           mainwindow.cpp \
           optionsdialog.cpp \
    qfrdrreplacedialog.cpp \
    qfhtmlhelpwindow.cpp \
    dlgcontactauthors.cpp \
    dlgnewversion.cpp \
    dlgrdrsetproperty.cpp \
    dlgselectprojectsubset.cpp \
    dlgfixfilepaths.cpp

FORMS =    optionsdialog.ui \
           about.ui \
           aboutplugins.ui \
    qfrdrreplacedialog.ui \
    dlgcontactauthors.ui \
    dlgnewversion.ui \
    dlgrdrsetproperty.ui \
    dlgselectprojectsubset.ui \
    dlgfixfilepaths.ui

RESOURCES += quickfit3.qrc


TRANSLATIONS= ./translations/de.ts

INCLUDEPATH += ../../../../LIB/trunk/

win32 {
    RC_FILE = quickfit3.rc
}

QT += gui xml
CONFIG +=  exceptions rtti stl link_prl

!release {
    CONFIG += console
}


macx{
  exists(qf3icon.icns) {
    ICON = qf3icon.icns
  }
}

ICON_FILES.files = qf3icon.icns\
                   qf3icon.ico\
                   qf3icon_old.ico
ICON_FILES.path = $${QFOUTPUT}


ASSETSSTYLESHEET_FILES.files = ./assets/stylesheets/*.*
ASSETSSTYLESHEET_FILES.path = $${QFOUTPUT}/assets/stylesheets/


HELPREF_FILES.files = ./help/parserreference/*.*
HELPREF_FILES.path = $${QFOUTPUT}/assets/help/parserreference/

HELPREFPIC_FILES.files = ./help/parserreference/pic/*.*
HELPREFPIC_FILES.path = $${QFOUTPUT}/assets/help/parserreference/pic/

INSTALLS += ASSETSSTYLESHEET_FILES HELPREF_FILES HELPREFPIC_FILES ICON_FILES



