macx {
    !contains( LIBS, quickfit3lib ) {
        QMAKE_LFLAGS += -F$$QFOUTPUT
        LIBS += -framework quickfit3lib
    }
} else {
    !contains( LIBS, -lquickfit3lib ) {
        LIBS += -L$$QFOUTPUT  -lquickfit3lib
    }
}

INCLUDEPATH += $$QFOUTPUT/../lib/

HEADERS +=

