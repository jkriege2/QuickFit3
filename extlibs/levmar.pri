contains( HAS_LAPACK, true ) {
   LIBS += $$LAPACK_LINK -L../../extlibs/levmar/lib/ -llevmar -L$$QFOUTPUT -lquickfit3lib -lquickfit3widgets
   INCLUDEPATH += $$LAPACK_INCLUDE \
                  ../../extlibs/include/
} else {
   LIBS += -L../../extlibs/levmar/lib/ -llevmar -L$$QFOUTPUT -lquickfit3lib -lquickfit3widgets
   INCLUDEPATH += ../../extlibs/include/
}
