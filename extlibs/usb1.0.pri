INCLUDEPATH += ../../extlibs/libusb/include/
LIBS += -L../../extlibs/libusb/lib/ -lusb-1.0
DEFINES += QF_HAS_LIBUSB
win32:LIBS += -lgdi32