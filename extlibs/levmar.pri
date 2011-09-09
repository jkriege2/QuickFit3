contains( HAS_LAPACK, true ) {
   LIBS += $$LAPACK_LINK -L../../extlibs/levmar/lib/
   INCLUDEPATH += $$LAPACK_INCLUDE \
                  ../../extlibs/include/
} else {
   LIBS += -L../../extlibs/levmar/lib/ -llevmar
   INCLUDEPATH += ../../extlibs/levmar/include/
}
