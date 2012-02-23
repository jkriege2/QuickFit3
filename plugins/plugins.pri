include(../quickfit3.pri)
include(../quickfit_config.pri)

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/ \
               ../interfaces \
               ../base_classes

MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/
QFOUTPUT = ../../$${OUTPUTDIR_NAME}
DESTDIR = $$QFOUTPUT/plugins



#release {
#    QFOUTPUT = ../../output_release
#} else {
#    #MOC_DIR = ./.mocsd/
#    #UI_DIR = ./.uisd/
#    #RCC_DIR = ./.rccsd/
#    #OBJECTS_DIR = ./.objsd/
#    QFOUTPUT = ../../output
#}



SRC_DISTRIBUTED.files =
SRC_DISTRIBUTED.path = $${QFOUTPUT}/source/plugins/$${TARGET}/




HELP_FILES.files = ./help/*.*
HELP_FILES.path = $${QFOUTPUT}/assets/plugins/help/$${TARGET}/

HELPPIC_FILES.files = ./help/pic/*.*
HELPPIC_FILES.path = $${QFOUTPUT}/assets/plugins/help/$${TARGET}/pic/

ASSETS_FILES.files = ./assets/*.*
ASSETS_FILES.path = $${QFOUTPUT}/assets/plugins/$${TARGET}

GLOBALCONFIG_FILES.files = ./globalconfig/*.*
GLOBALCONFIG_FILES.path = $${QFOUTPUT}/globalconfig_templates/

GLOBALCONFIGWIN32_FILES.files = ./globalconfig/win32/*.*
GLOBALCONFIGWIN32_FILES.path = $${QFOUTPUT}/globalconfig_templates/

GLOBALCONFIGLINUX_FILES.files = ./globalconfig/linux/*.*
GLOBALCONFIGLINUX_FILES.path = $${QFOUTPUT}/globalconfig_templates/

GLOBALCONFIGMACX_FILES.files = ./globalconfig/macx/*.*
GLOBALCONFIGMACX_FILES.path = $${QFOUTPUT}/globalconfig_templates/

ASSETS_TRANSLATIONS.files = ./translations/*.qm
ASSETS_TRANSLATIONS.path = $${QFOUTPUT}/assets/translations/

INSTALLS += HELP_FILES HELPPIC_FILES ASSETS_FILES GLOBALCONFIG_FILES ASSETS_TRANSLATIONS SRC_DISTRIBUTED

win32 {
    INSTALLS += GLOBALCONFIGWIN32_FILES
} else {
    macx {
        INSTALLS += GLOBALCONFIGMACX_FILES
    } else {
        INSTALLS += GLOBALCONFIGLINUX_FILES
    }
}

release {
    message("building $$TARGET $$TEMPLATE (plugin) in RELEASE mode, output is in $$QFOUTPUT")
} else {
    message("building $$TARGET $$TEMPLATE (plugin) in DEBUG mode, output is in $$QFOUTPUT")
}

DEFINES += QTLIB_IN_DLL LIB_IN_DLL

CONFIG += link_prl

include(../quickfit_config.pri)
include(../libquickfit.pri)

HEADERS +=

SOURCES +=




