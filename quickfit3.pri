


MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/
QFOUTPUT = ../output

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

ASSETS_TRANSLATIONS.files = ./translations/*.qm
ASSETS_TRANSLATIONS.path = $${QFOUTPUT}/assets/translations/

INSTALLS += HELP_FILES HELPPIC_FILES ASSETS_FILES ASSETS_TRANSLATIONS

OTHER_FILES +=

include(quickfit_config.pri)
