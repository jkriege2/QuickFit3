
unix:!macx {
   LIBS += -llapack -lblas -llevmar -L$$QFOUTPUT -lquickfit3lib -lquickfit3widgets
} else {
   LIBS += -L../../extlibs/levmar/ -llevmar -L$$QFOUTPUT -lquickfit3lib -lquickfit3widgets
   INCLUDEPATH += ../../extlibs/
}

