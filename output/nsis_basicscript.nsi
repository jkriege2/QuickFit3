# A few handy definitions to avoid repetition
!define COMPANY_NAME "German Cancer Research Center (DKFZ)"
!define PRODUCT_NAME "QuickFit"
!define PRODUCT_VERSION "3.0 (SVN %%SVNVER%%, %%BITDEPTH%%-bit)"
!define BIT_DEPTH "%%BITDEPTH%%"
!define UNINSTALL_KEY "SoftwareMicrosoftWindowsCurrentVersionUninstall${COMPANY_NAME} ${PRODUCT_NAME}"

# Set the installer name and compilation properties
OutFile "%%INSTALLER_BASENAME%%_setup.exe"
RequestExecutionLevel admin

# Set some basic installer properties
Name "${PRODUCT_NAME}"
BrandingText "Copyright (c) 2013 by Jan W. Krieger"
ShowInstDetails show
ShowUninstDetails show
SetCompressor /FINAL  LZMA 

# Set the default installation location
InstallDir "$PROGRAMFILES\${PRODUCT_NAME}3"

# Remember the installation directory for future updates and the uninstaller
InstallDirRegKey HKLM "Software${COMPANY_NAME}${PRODUCT_NAME}" "InstallDir"

# Include the ModernUI 2 library to make everything look nice
!include "MUI2.nsh"

# Installer pages
# We use definitions described in the ModernUI 2 documentation to customize the UI
!define MUI_WELCOMEPAGE_TITLE "This will install ${PRODUCT_NAME} ${PRODUCT_VERSION}!"
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_INSTFILES
!define MUI_FINISHPAGE_RUN "quickfit3.exe"
!define MUI_FINISHPAGE_RUN_TEXT "Launch ${PRODUCT_NAME} now!"
!insertmacro MUI_PAGE_FINISH

# Uninstaller pages
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_COMPONENTS
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

# Make sure the appropriate ModernUI language strings are included
!insertmacro MUI_LANGUAGE "English"



# This installs the main application
Section "${PRODUCT_NAME} ${PRODUCT_VERSION}"
	Push $OUTDIR ; Store previous output directory
	SetOutPath "$INSTDIR" ; Set output directory

	# Write the program file and make a Start menu shortcut
	File quickfit3.exe 	
	CreateShortCut "$SMPROGRAMS${COMPANY_NAME}${PRODUCT_NAME}.lnk" "$OUTDIRquickfit3.exe"
	
	# automatically created list of install files
	%%INSTALLER_DIRS%%
	%%INSTALLER_FILES%%

	# Create an uninstaller and add it to the Add an Add/Remove programs list
	WriteUninstaller Uninstall.exe
	WriteRegStr HKLM "${UNINSTALL_KEY}" DisplayName "${COMPANY_NAME} ${PRODUCT_NAME}"
	WriteRegStr HKLM "${UNINSTALL_KEY}" DisplayIcon '"$OUTDIRUninstall.exe"'
	WriteRegStr HKLM "${UNINSTALL_KEY}" UninstallString '"$OUTDIRUninstall.exe"'
	
	Pop $OUTDIR ; Restore the original output directory
SectionEnd

# This uninstalls the main application
Section "un.${PRODUCT_NAME} ${PRODUCT_VERSION}"
	# Remove the program and Start menu shortcut
	Delete /REBOOTOK "$INSTDIR\quickfit3.exe"
	Delete /REBOOTOK "$SMPROGRAMS${COMPANY_NAME}${PRODUCT_NAME}.lnk"
	# automatically created list of install files
	%%UNINSTALLER_FILES%%
	RMDir /REBOOTOK "$SMPROGRAMS${COMPANY_NAME}"
	
	# Remove the uninstaller and Add/Remove programs information
	Delete /REBOOTOK "$INSTDIRUninstall.exe"
	DeleteRegKey HKLM "${UNINSTALL_KEY}"
	
	# Remove the installation directory if empty
	RMDir /r /REBOOTOK "$INSTDIR"
SectionEnd





# This installs the SPIM plugins
Section "SPIM Plugins"
	Push $OUTDIR ; Store previous output directory
	SetOutPath "$INSTDIR" ; Set output directory
	
	# automatically created list of install files
	%%SPIMINSTALLER_DIRS%%
	%%SPIMINSTALLER_FILES%%

	Pop $OUTDIR ; Restore the original output directory
SectionEnd


# This uninstalls the main application
Section "un.SPIM Plugins"
	# automatically created list of install files
	%%SPIMUNINSTALLER_FILES%%

SectionEnd