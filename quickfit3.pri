
DEFINES += QTLIB_IN_DLL

MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/

include("autoversioning.pri")


include(quickfit_config.pri)
QFOUTPUT = ../$${OUTPUTDIR_NAME}

#release {
#    QFOUTPUT = ../output_release
#} else {
#    #MOC_DIR = ./.mocsd/
#    #UI_DIR = ./.uisd/
#    #RCC_DIR = ./.rccsd/
#    #OBJECTS_DIR = ./.objsd/
#    QFOUTPUT = ../output
#}

HELP_FILES.files = ./help/*.*
HELP_FILES.path = $${QFOUTPUT}/assets/help/

HELPPIC_FILES.files = ./help/pic/*.*
HELPPIC_FILES.path = $${QFOUTPUT}/assets/help/pic/

ASSETS_FILES.files = ./assets/*.*
ASSETS_FILES.path = $${QFOUTPUT}/assets/

EXAMPLES_FILES.files = ./examples/*.*
EXAMPLES_FILES.path = $${QFOUTPUT}/examples/

ASSETS_TRANSLATIONS.files = ./translations/*.qm
ASSETS_TRANSLATIONS.path = $${QFOUTPUT}/assets/translations/

INSTALLS += HELP_FILES HELPPIC_FILES ASSETS_FILES ASSETS_TRANSLATIONS EXAMPLES_FILES

OTHER_FILES +=

QT += network
