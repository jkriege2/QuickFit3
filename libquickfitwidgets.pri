include(libquickfit.pri)

macx {
    !contains( LIBS, quickfit3widgets ) {
        QMAKE_LFLAGS += -F$$QFOUTPUT
        LIBS += -framework quickfit3widgets
    }
} else {
    !contains( LIBS, -lquickfit3widgets ) {
        LIBS += -L$$QFOUTPUT  -lquickfit3widgets
    }
}

INCLUDEPATH += $$QFOUTPUT/../libqf3widgets/


