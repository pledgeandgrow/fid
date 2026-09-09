; installer/FileAssociation.nsh — File association helpers
;
; Provides:
;   ${RegisterExtension}   - Associate a file extension with an executable
;   ${UnRegisterExtension}  - Remove association
;
; Usage:
;   ${RegisterExtension} "$INSTDIR\fides.exe" ".fd" "Fides Source File"
;   ${UnRegisterExtension} ".fd" "Fides Source File"

!ifndef FILEASSOCIATION_NSH
!define FILEASSOCIATION_NSH

!macro RegisterExtension Executable ExtName Description
  Push "${Executable}"
  Push "${ExtName}"
  Push "${Description}"
  Call RegisterExtension
!macroend

!macro UnRegisterExtension ExtName Description
  Push "${ExtName}"
  Push "${Description}"
  Call un.UnRegisterExtension
!macroend

Function RegisterExtension
  Pop $2  ; Description
  Pop $1  ; ExtName (.fd)
  Pop $0  ; Executable

  ; Register the file type
  WriteRegStr HKCR $1 "" $2

  ; Register the description
  WriteRegStr HKCR "$2" "" "$2"
  WriteRegStr HKCR "$2\shell" "" "open"
  WriteRegStr HKCR "$2\shell\open\command" "" '"$0" "%1"'
  WriteRegStr HKCR "$2\DefaultIcon" "" "$0,0"

  ; Notify the shell
  SendMessage HWND_BROADCAST ${WM_SETTINGCHANGE} 0 "STR:Shell" /TIMEOUT=5000
FunctionEnd

Function un.UnRegisterExtension
  Pop $1  ; Description
  Pop $0  ; ExtName (.fd)

  ; Delete the file type
  DeleteRegKey HKCR $0
  DeleteRegKey HKCR $1

  ; Notify the shell
  SendMessage HWND_BROADCAST ${WM_SETTINGCHANGE} 0 "STR:Shell" /TIMEOUT=5000
FunctionEnd

!endif
