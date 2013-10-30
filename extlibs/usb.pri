INCLUDEPATH += ../../extlibs/libusb/include/
LIBS += -L../../extlibs/libusb/lib/ #-lusb
DEFINES += QF_HAS_LIBUSB
win32:LIBS += -lgdi32

include(usb1.0.pri)
