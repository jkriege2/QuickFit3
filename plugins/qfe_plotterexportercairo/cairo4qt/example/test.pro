SOURCES += main.cpp widget.cpp ../src/qpaintengine_cairo.cpp
HEADERS += widget.h ../src/qpaintengine_cairo_p.h
RESOURCES += test.qrc
INCLUDEPATH += /usr/include/cairo ../src/
LIBS += -lcairo
