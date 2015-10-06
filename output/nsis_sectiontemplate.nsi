
# Template for an installer section
Section "%%SEC_NAME%%" sec_%%SEC_ID%%
	Push $OUTDIR ; Store previous output directory
	SetOutPath "$INSTDIR\" ; Set output directory
	
	SectionIn 3
	%%INSTALLER_INSECS%%
	
	# automatically created list of install files
	%%INSTALLER_DIRS%%
	%%INSTALLER_FILES%%
	
	# Set the INSTALLSIZE constant (!defined at the top of this script) so Add/Remove Programs can accurately report the size
	 ${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
	IntFmt $0 "0x%08X" $0
	WriteRegDWORD HKLM  "${UNINSTALL_KEY}" "EstimatedSize" "$0"

	Pop $OUTDIR ; Restore the original output directory
SectionEnd
