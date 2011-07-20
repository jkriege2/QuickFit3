

release {
    MOC_DIR = ./.mocs/
    UI_DIR = ./.uis/
    RCC_DIR = ./.rccs/
    OBJECTS_DIR = ./.objs/
    QFOUTPUT = ../../output
} else {
    MOC_DIR = ./.mocsd/
    UI_DIR = ./.uis/
    RCC_DIR = ./.rccsd/
    OBJECTS_DIR = ./.objsd/
    QFOUTPUT = ../../output_release
}



HELP_FILES.files = ./help/*.*
HELP_FILES.path = $${QFOUTPUT}/assets/plugins/help/$${TARGET}/

HELPPIC_FILES.files = ./help/pic/*.*
HELPPIC_FILES.path = $${QFOUTPUT}/assets/plugins/help/$${TARGET}/pic/

ASSETS_FILES.files = ./assets/*.*
ASSETS_FILES.path = $${QFOUTPUT}/assets/plugins/$${TARGET}

ASSETS_TRANSLATIONS.files = ./translations/*.qm
ASSETS_TRANSLATIONS.path = $${QFOUTPUT}/assets/translations/

INSTALLS += HELP_FILES HELPPIC_FILES ASSETS_FILES ASSETS_TRANSLATIONS
