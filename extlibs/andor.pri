exists(andor.inc):include(andor.inc)

win32 {
	contains( ANDOR_WIN64, true ) {
	  LIBS+= -L../../extlibs/andor_win64
	  INCLUDEPATH += ../../extlibs/andor_win64
	} else {
	  LIBS+= -L../../extlibs/andor_win32
	  INCLUDEPATH += ../../extlibs/andor_win32
	}
}

LIBS += -landor

HEADERS +=

SOURCES +=



