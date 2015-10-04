INCLUDEPATH += $$PWD/libxlsxwriter-master/include/xlsxwriter/ \
               $$PWD/libxlsxwriter-master/include/ \
               $$PWD/libxlsxwriter-master/include/third_party/ \
               $$PWD/libxlsxwriter-master/include/third_party/minizip/

HEADERS += \
    libxlsxwriter-master/include/xlsxwriter.h \
    libxlsxwriter-master/include/xlsxwriter/app.h \
    libxlsxwriter-master/include/xlsxwriter/common.h \
    libxlsxwriter-master/include/xlsxwriter/content_types.h \
    libxlsxwriter-master/include/xlsxwriter/core.h \
    libxlsxwriter-master/include/xlsxwriter/format.h \
    libxlsxwriter-master/include/xlsxwriter/hash_table.h \
    libxlsxwriter-master/include/xlsxwriter/packager.h \
    libxlsxwriter-master/include/xlsxwriter/relationships.h \
    libxlsxwriter-master/include/xlsxwriter/shared_strings.h \
    libxlsxwriter-master/include/xlsxwriter/styles.h \
    libxlsxwriter-master/include/xlsxwriter/theme.h \
    libxlsxwriter-master/include/xlsxwriter/utility.h \
    libxlsxwriter-master/include/xlsxwriter/workbook.h \
    libxlsxwriter-master/include/xlsxwriter/worksheet.h \
    libxlsxwriter-master/include/xlsxwriter/xmlwriter.h \
    libxlsxwriter-master/include/xlsxwriter/third_party/queue.h \
    libxlsxwriter-master/include/xlsxwriter/third_party/minizip/ioapi.h \
    libxlsxwriter-master/include/xlsxwriter/third_party/minizip/zip.h \
    $$PWD/libxlsxwriter-master/third_party/minizip/crypt.h \
    $$PWD/libxlsxwriter-master/third_party/minizip/ioapi.h \
    $$PWD/libxlsxwriter-master/third_party/minizip/iowin32.h \
    $$PWD/libxlsxwriter-master/third_party/minizip/mztools.h \
    $$PWD/libxlsxwriter-master/third_party/minizip/unzip.h \
    $$PWD/libxlsxwriter-master/third_party/minizip/zip.h

SOURCES += \
    libxlsxwriter-master/src/app.c \
    libxlsxwriter-master/src/content_types.c \
    libxlsxwriter-master/src/core.c \
    libxlsxwriter-master/src/format.c \
    libxlsxwriter-master/src/hash_table.c \
    libxlsxwriter-master/src/packager.c \
    libxlsxwriter-master/src/relationships.c \
    libxlsxwriter-master/src/shared_strings.c \
    libxlsxwriter-master/src/styles.c \
    libxlsxwriter-master/src/theme.c \
    libxlsxwriter-master/src/utility.c \
    libxlsxwriter-master/src/workbook.c \
    libxlsxwriter-master/src/worksheet.c \
    libxlsxwriter-master/src/xmlwriter.c \
    $$PWD/libxlsxwriter-master/third_party/minizip/ioapi.c \
    $$PWD/libxlsxwriter-master/third_party/minizip/zip.c

#win32 {
#    SOURCES += \
#        $$PWD/libxlsxwriter-master/third_party/minizip/iowin32.c

#    HEADERS += \
#        $$PWD/libxlsxwriter-master/third_party/minizip/iowin32.h
#}
