# A few handy definitions to avoid repetition
!define COMPANY_NAME "German Cancer Research Center (DKFZ)"
!define PRODUCT_NAME "QuickFit"
!define PRODUCT_VERSION "3.0 (SVN %%SVNVER%% COMPILEDATE: %%COMPILEDATE%%, %%BITDEPTH%%-bit)"
!define BIT_DEPTH "%%BITDEPTH%%"
!define HELPURL "http://www.dkfz.de/Macromol/quickfit/"
!define UPDATEURL "http://www.dkfz.de/Macromol/quickfit/#download"
!define URLInfoAbout "http://www.dkfz.de/Macromol/quickfit/"
!define UNINSTALL_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANY_NAME} ${PRODUCT_NAME}"

# Set the installer name and compilation properties
OutFile "%%INSTALLER_BASENAME%%_setup.exe"
RequestExecutionLevel admin

# Set some basic installer properties
Name "${PRODUCT_NAME}"
BrandingText "Copyright (c) 2013 by Jan W. Krieger (German Cancer research Center - DKFZ)"
ShowInstDetails show
ShowUninstDetails show
SetCompressor /FINAL  LZMA 

# Set the default installation location
InstallDir "%%INSTALLER_INSTDIR%%\${PRODUCT_NAME}3"

# Include Multiuser setup
!define MULTIUSER_EXECUTIONLEVEL Highest
!define MULTIUSER_MUI
!define MULTIUSER_INSTALLMODE_COMMANDLINE
!include MultiUser.nsh

# Include the ModernUI 2 library to make everything look nice
!include "MUI2.nsh"
!include "MUI_EXTRAPAGES.nsh"

# Installer pages
# We use definitions described in the ModernUI 2 documentation to customize the UI
Var StartMenuFolder
!define MUI_WELCOMEFINISHPAGE_BITMAP "..\images\splash_installer.bmp"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP  "..\images\splash_installer.bmp"
!define MUI_WELCOMEPAGE_TITLE "Install ${PRODUCT_NAME}, %%BITDEPTH%%-bit (%%SVNVER%%, %%COMPILEDATE%%)!"
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_README "README.txt"
!insertmacro MUI_PAGE_LICENSE "LICENSE.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKLM" 
!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\QuickFit 3" 
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder" 
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
!insertmacro MUI_PAGE_INSTFILES
!define MUI_FINISHPAGE_RUN "quickfit3.exe"
!define MUI_FINISHPAGE_RUN_TEXT "Launch ${PRODUCT_NAME} now!"
!define MUI_FINISHPAGE_SHOWREADME ""
!define MUI_FINISHPAGE_SHOWREADME_CHECKED
!define MUI_FINISHPAGE_SHOWREADME_TEXT "Create Desktop Shortcut"
!define MUI_FINISHPAGE_SHOWREADME_FUNCTION finishpageaction
!insertmacro MUI_PAGE_FINISH

!include "FileFunc.nsh"
 
# Remember the installation directory for future updates and the uninstaller
InstallDirRegKey HKLM "Software${COMPANY_NAME}${PRODUCT_NAME}" "InstallDir"



# Uninstaller pages
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_COMPONENTS
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

# Make sure the appropriate ModernUI language strings are included
!insertmacro MUI_LANGUAGE "English"

  ;Set up install lang strings for 1st lang
  ${ReadmeLanguage} "${LANG_ENGLISH}" \
          "Read Me" \
          "Please review the following important information." \
          "About $(^name):" \
          "$\n  Click on scrollbar arrows or press Page Down to review the entire text."
 
  ;Set up uninstall lang strings for 1st lang
  ${Un.ReadmeLanguage} "${LANG_ENGLISH}" \
          "Read Me" \
          "Please review the following important Uninstall information." \
          "About $(^name) Uninstall:" \
          "$\n  Click on scrollbar arrows or press Page Down to review the entire text."


#Macro to create URL shortcut
!macro CreateInternetShortcut FILENAME URL ICONFILE ICONINDEX
WriteINIStr "${FILENAME}.url" "InternetShortcut" "URL" "${URL}"
WriteINIStr "${FILENAME}.url" "InternetShortcut" "IconFile" "${ICONFILE}"
WriteINIStr "${FILENAME}.url" "InternetShortcut" "IconIndex" "${ICONINDEX}"
!macroend

!macro VerifyUserIsAdmin
UserInfo::GetAccountType
pop $0
${If} $0 != "admin" ;Require admin rights on NT4+
        messageBox mb_iconstop "Administrator rights required!"
        setErrorLevel 740 ;ERROR_ELEVATION_REQUIRED
        quit
${EndIf}
!macroend


#function to create desktop shortcut when the  user selects it
Function finishpageaction
	CreateShortCut "$desktop\${PRODUCT_NAME}.lnk" "$instdir\quickfit3.exe"
FunctionEnd


# This installs the main application
Section "${PRODUCT_NAME} ${PRODUCT_VERSION}" sec_main
	Push $OUTDIR ; Store previous output directory
	SetOutPath "$INSTDIR" ; Set output directory

	#CreateShortCut "$SMPROGRAMS${COMPANY_NAME}${PRODUCT_NAME}.lnk" "$OUTDIRquickfit3.exe"
	
	# automatically created list of install files
	%%INSTALLER_DIRS%%
	%%INSTALLER_FILES%%

	# Create an uninstaller and add it to the Add an Add/Remove programs list
	WriteUninstaller Uninstall.exe
	WriteRegStr HKLM  "${UNINSTALL_KEY}" DisplayName "${PRODUCT_NAME}"
	WriteRegStr HKLM  "${UNINSTALL_KEY}" DisplayIcon "$OUTDIR\Uninstall.exe"
	WriteRegStr HKLM  "${UNINSTALL_KEY}" UninstallString "$OUTDIR\Uninstall.exe"
	
	
	
	WriteRegStr HKLM  "${UNINSTALL_KEY}" "DisplayName" "${PRODUCT_NAME}"
	WriteRegStr HKLM  "${UNINSTALL_KEY}" "UninstallString" "$\"$INSTDIR\Uninstall.exe$\""
	WriteRegStr HKLM  "${UNINSTALL_KEY}" "QuietUninstallString" "$\"$INSTDIR\Uninstall.exe$\" /S"
	WriteRegStr HKLM  "${UNINSTALL_KEY}" "InstallLocation" "$\"$INSTDIR$\""
	WriteRegStr HKLM  "${UNINSTALL_KEY}" "DisplayIcon" "$\"$INSTDIR\qf3icon.ico$\""
	WriteRegStr HKLM  "${UNINSTALL_KEY}" "Publisher" "$\"${COMPANY_NAME}$\""
	WriteRegStr HKLM  "${UNINSTALL_KEY}" "URLUpdateInfo" "$\"${UPDATEURL}$\""
	WriteRegStr HKLM  "${UNINSTALL_KEY}" "URLInfoAbout" "$\"${HELPURL}$\""
	WriteRegStr HKLM  "${UNINSTALL_KEY}" "DisplayVersion" "$\"${PRODUCT_VERSION}$\""
	# There is no option for modifying or repairing the install
	WriteRegDWORD HKLM  "${UNINSTALL_KEY}" "NoModify" 1
	WriteRegDWORD HKLM  "${UNINSTALL_KEY}" "NoRepair" 1
	# Set the INSTALLSIZE constant (!defined at the top of this script) so Add/Remove Programs can accurately report the size
	 ${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
	IntFmt $0 "0x%08X" $0
	WriteRegDWORD HKLM  "${UNINSTALL_KEY}" "EstimatedSize" "$0"

	!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
	# write start menu entries
	createDirectory "$SMPROGRAMS\$StartMenuFolder"
	createShortCut "$SMPROGRAMS\$StartMenuFolder\${PRODUCT_NAME}.lnk" "$INSTDIR\quickfit3.exe" "" "$INSTDIR\qf3icon.ico"
	!insertmacro CreateInternetShortcut \
		  "$SMPROGRAMS\$StartMenuFolder\${PRODUCT_NAME} homepage" \
		  "${HELPURL}" \
		  "" "0"
	createShortCut "$SMPROGRAMS\$StartMenuFolder\${PRODUCT_NAME}_releasenotes.lnk" "$INSTDIR\releasenotes.html" "" "" 0 SW_SHOWNORMAL "" "Release Notes" 
	createShortCut "$SMPROGRAMS\$StartMenuFolder\${PRODUCT_NAME}_license.lnk" "$INSTDIR\LICENSE.txt" "" "" 0 SW_SHOWNORMAL "" "License" 
	createShortCut "$SMPROGRAMS\$StartMenuFolder\${PRODUCT_NAME}_readme.lnk" "$INSTDIR\README.txt" "" "" 0 SW_SHOWNORMAL "" "Read Me" 

	createShortCut "$SMPROGRAMS\$StartMenuFolder\${PRODUCT_NAME}_uninstall.lnk" "$INSTDIR\Uninstall.exe" "" 
!insertmacro MUI_STARTMENU_WRITE_END
	
	Pop $OUTDIR ; Restore the original output directory
SectionEnd






# This installs the SPIM plugins
Section "SPIM Plugins" sec_spim
	Push $OUTDIR ; Store previous output directory
	SetOutPath "$INSTDIR" ; Set output directory
	
	# automatically created list of install files
	%%SPIMINSTALLER_DIRS%%
	%%SPIMINSTALLER_FILES%%
	
	# Set the INSTALLSIZE constant (!defined at the top of this script) so Add/Remove Programs can accurately report the size
	 ${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
	IntFmt $0 "0x%08X" $0
	WriteRegDWORD HKLM  "${UNINSTALL_KEY}" "EstimatedSize" "$0"

	Pop $OUTDIR ; Restore the original output directory
SectionEnd



function .onInit
	setShellVarContext all
	!insertmacro VerifyUserIsAdmin
	!insertmacro MULTIUSER_INIT
	IntOp $0 ${SF_SELECTED} | ${SF_RO}
	IntOp $0 $0 | ${SF_BOLD}
    SectionSetFlags ${sec_main} $0
    SectionSetFlags ${sec_spim} 0

functionEnd




#Uninstaller
function un.onInit
	SetShellVarContext all
 
	#Verify the uninstaller - last chance to back out
	MessageBox MB_OKCANCEL "Permanantly remove ${PRODUCT_NAME}?" IDOK next
		Abort
	next:
	!insertmacro VerifyUserIsAdmin
	!insertmacro MULTIUSER_UNINIT
functionEnd


# This uninstalls the main application
Section "un.${PRODUCT_NAME} ${PRODUCT_VERSION}" sec_un_main

	!insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
	
	# Remove the program and Start menu shortcut
	Delete /REBOOTOK "$SMPROGRAMS\$StartMenuFolder\${PRODUCT_NAME}_uninstall.lnk"
	Delete /REBOOTOK "$SMPROGRAMS\$StartMenuFolder\${PRODUCT_NAME} homepage.url"
	Delete /REBOOTOK "$SMPROGRAMS\$StartMenuFolder\${PRODUCT_NAME}_releasenotes.lnk"
	Delete /REBOOTOK "$SMPROGRAMS\$StartMenuFolder\${PRODUCT_NAME}_license.lnk"
	Delete /REBOOTOK "$SMPROGRAMS\$StartMenuFolder\${PRODUCT_NAME}_readme.lnk"
	Delete /REBOOTOK "$SMPROGRAMS\$StartMenuFolder\${PRODUCT_NAME}.lnk"
	RMDir /REBOOTOK "$SMPROGRAMS\$StartMenuFolder"

	# automatically created list of install files
	%%SPIMUNINSTALLER_FILES%%
	%%UNINSTALLER_FILES%%
	RMDir /REBOOTOK "$SMPROGRAMS${COMPANY_NAME}"
	
	# Remove the uninstaller and Add/Remove programs information
	Delete /REBOOTOK "$INSTDIRUninstall.exe"
	DeleteRegKey HKLM  "${UNINSTALL_KEY}"
	
	# Remove the installation directory if empty
	RMDir /r /REBOOTOK "$INSTDIR"
SectionEnd