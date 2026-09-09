; installer/EnvVarUpdate.nsh — PATH environment variable manipulation
;
; Provides:
;   ${EnvVarUpdate}    - Add to PATH (Append/Prepend/Remove)
;   ${un.EnvVarUpdate} - Same for uninstaller
;
; Usage:
;   ${EnvVarUpdate} $0 "PATH" "A" "HKLM" "C:\Program Files\Fides"
;   ${un.EnvVarUpdate} $0 "PATH" "R" "HKLM" "C:\Program Files\Fides"
;
; Action:
;   A = Append to PATH
;   P = Prepend to PATH
;   R = Remove from PATH

!ifndef ENVVARUPDATE_NSH
!define ENVVARUPDATE_NSH

!macro EnvVarUpdate ResultVar EnvVarName Action RegLoc PathToAdd

  Push "${EnvVarName}"
  Push "${Action}"
  Push "${RegLoc}"
  Push "${PathToAdd}"
  Call EnvVarUpdate
  Pop ${ResultVar}

!macroend

!macro un.EnvVarUpdate ResultVar EnvVarName Action RegLoc PathToAdd

  Push "${EnvVarName}"
  Push "${Action}"
  Push "${RegLoc}"
  Push "${PathToAdd}"
  Call un.EnvVarUpdate
  Pop ${ResultVar}

!macroend

; Registry key for environment variables
!define ENV_HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"
!define ENV_HKCU "Environment"

Function EnvVarUpdate
  Exch $0       ; PathToAdd
  Exch
  Exch $1       ; RegLoc (HKLM or HKCU)
  Exch
  Exch 2
  Exch $2       ; Action (A, P, R)
  Exch 2
  Exch 3
  Exch $3       ; EnvVarName (e.g. "PATH")
  Exch 3
  Push $4       ; Current value
  Push $5       ; Temp
  Push $6       ; Temp
  Push $7       ; Temp

  ; Read current value from registry
  StrCpy $4 ""
  StrCmp $1 "HKLM" 0 +3
    ReadRegStr $4 HKLM "${ENV_HKLM}" $3
    Goto +2
  ReadRegStr $4 HKCU "${ENV_HKCU}" $3

  StrCmp $2 "R" RemoveFromPath

  ; Append or Prepend
  StrCmp $4 "" EmptyPath
    StrCpy $5 "$4;$0"
    Goto WritePath
  EmptyPath:
    StrCpy $5 "$0"
  WritePath:
    StrCmp $1 "HKLM" 0 +3
      WriteRegExpandStr HKLM "${ENV_HKLM}" $3 $5
      Goto +2
    WriteRegExpandStr HKCU "${ENV_HKCU}" $3 $5

  ; Broadcast WM_SETTINGCHANGE so other apps pick up the change
  SendMessage HWND_BROADCAST ${WM_SETTINGCHANGE} 0 "STR:Environment" /TIMEOUT=5000

  StrCpy $0 "1"   ; Success
  Goto Done

  RemoveFromPath:
    ; Remove $0 from $4
    StrCpy $5 "$4;$0;"
    ; Replace ;$0; with ;
    Push "$5"
    Push ";$0;"
    Push ";"
    Call StrRep
    Pop $5

    ; Remove trailing ;
    StrCpy $6 $5 1 -1
    StrCmp $6 ";" 0 +2
      StrCpy $5 $5 -1

    ; Write back
    StrCmp $1 "HKLM" 0 +3
      WriteRegExpandStr HKLM "${ENV_HKLM}" $3 $5
      Goto +2
    WriteRegExpandStr HKCU "${ENV_HKCU}" $3 $5

    SendMessage HWND_BROADCAST ${WM_SETTINGCHANGE} 0 "STR:Environment" /TIMEOUT=5000
    StrCpy $0 "1"

  Done:
  Pop $7
  Pop $6
  Pop $5
  Pop $4
  Pop $3
  Pop $2
  Pop $1
  Exch $0   ; Result
FunctionEnd

Function un.EnvVarUpdate
  Exch $0
  Exch
  Exch $1
  Exch
  Exch 2
  Exch $2
  Exch 2
  Exch 3
  Exch $3
  Exch 3
  Push $4
  Push $5
  Push $6

  StrCpy $4 ""
  StrCmp $1 "HKLM" 0 +3
    ReadRegStr $4 HKLM "${ENV_HKLM}" $3
    Goto +2
  ReadRegStr $4 HKCU "${ENV_HKCU}" $3

  ; Remove $0 from $4
  StrCpy $5 "$4;$0;"
  Push "$5"
  Push ";$0;"
  Push ";"
  Call un.StrRep
  Pop $5

  StrCpy $6 $5 1 -1
  StrCmp $6 ";" 0 +2
    StrCpy $5 $5 -1

  StrCmp $1 "HKLM" 0 +3
    WriteRegExpandStr HKLM "${ENV_HKLM}" $3 $5
    Goto +2
  WriteRegExpandStr HKCU "${ENV_HKCU}" $3 $5

  SendMessage HWND_BROADCAST ${WM_SETTINGCHANGE} 0 "STR:Environment" /TIMEOUT=5000
  StrCpy $0 "1"

  Pop $6
  Pop $5
  Pop $4
  Pop $3
  Pop $2
  Pop $1
  Exch $0
FunctionEnd

; String replace helper
Function StrRep
  Exch $0  ; needle
  Exch
  Exch $1  ; replacement
  Exch
  Exch 2
  Exch $2  ; haystack
  Exch 2
  Push $3
  Push $4
  Push $5
  Push $6
  Push $7

  StrCpy $7 ""
  StrCpy $5 $2

  Loop:
    StrCpy $3 $5
    Push $3
    Push $0
    Call StrStr
    Pop $4
    StrCmp $4 "" Done

    StrLen $6 $0
    StrCpy $3 $3 $4
    StrCpy $7 "$7$3$1"
    StrCpy $5 $5 $6 $4
    Goto Loop

  Done:
    StrCpy $7 "$7$5"
    StrCpy $0 $7

  Pop $7
  Pop $6
  Pop $5
  Pop $4
  Pop $3
  Pop $2
  Pop $1
  Exch $0
FunctionEnd

Function un.StrRep
  Exch $0
  Exch
  Exch $1
  Exch
  Exch 2
  Exch $2
  Exch 2
  Push $3
  Push $4
  Push $5
  Push $6
  Push $7

  StrCpy $7 ""
  StrCpy $5 $2

  Loop:
    StrCpy $3 $5
    Push $3
    Push $0
    Call un.StrStr
    Pop $4
    StrCmp $4 "" Done

    StrLen $6 $0
    StrCpy $3 $3 $4
    StrCpy $7 "$7$3$1"
    StrCpy $5 $5 $6 $4
    Goto Loop

  Done:
    StrCpy $7 "$7$5"
    StrCpy $0 $7

  Pop $7
  Pop $6
  Pop $5
  Pop $4
  Pop $3
  Pop $2
  Pop $1
  Exch $0
FunctionEnd

; StrStr helper
Function StrStr
  Exch $0  ; needle
  Exch
  Exch $1  ; haystack
  Exch
  Push $2
  Push $3
  Push $4
  Push $5

  StrLen $3 $0
  StrCpy $4 0

  Loop:
    StrCpy $5 $1 $3 $4
    StrCmp $5 $0 Found
    StrCmp $5 "" NotFound
    IntOp $4 $4 + 1
    Goto Loop

  Found:
    StrCpy $0 $4
    Goto Done

  NotFound:
    StrCpy $0 ""

  Done:
  Pop $5
  Pop $4
  Pop $3
  Pop $2
  Pop $1
  Exch $0
FunctionEnd

Function un.StrStr
  Exch $0
  Exch
  Exch $1
  Exch
  Push $2
  Push $3
  Push $4
  Push $5

  StrLen $3 $0
  StrCpy $4 0

  Loop:
    StrCpy $5 $1 $3 $4
    StrCmp $5 $0 Found
    StrCmp $5 "" NotFound
    IntOp $4 $4 + 1
    Goto Loop

  Found:
    StrCpy $0 $4
    Goto Done

  NotFound:
    StrCpy $0 ""

  Done:
  Pop $5
  Pop $4
  Pop $3
  Pop $2
  Pop $1
  Exch $0
FunctionEnd

!endif
