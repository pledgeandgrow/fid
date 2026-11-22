; installer/fides.nsi — NSIS installer for the Fid language (its compiler is
; Fides: fid.exe). Up to 0.8.0 the product was named Fides; this installer
; removes such an install first (.onInit) so only one is on PATH.
;
; Build:
;   makensis installer/fides.nsi
;
; Output:
;   installer/Fid-Setup-<version>.exe (releases up to 0.8.0: Fides-Setup)

; ─── Version ─────────────────────────────────────────────────────────────

; build-installer.sh passes -DVERSION=<version>; this is the default
!ifndef VERSION
  !define VERSION "0.8.0"
!endif
!define PRODUCT_NAME "Fid"
!define PRODUCT_PUBLISHER "Pledge & Grow"
!define PRODUCT_URL "https://github.com/pledgeandgrow/fid"
!define PRODUCT_REGKEY "Software\Fid"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\Fid"
; The keys of the product as it was named up to 0.8.0
!define OLD_REGKEY "Software\Fides"
!define OLD_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\Fides"
!define ENV_HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"

; ─── Includes ────────────────────────────────────────────────────────────

!include "MUI2.nsh"
!include "WordFunc.nsh"

; ─── General ──────────────────────────────────────────────────────────────

Name "${PRODUCT_NAME} ${VERSION}"
OutFile "Fid-Setup-${VERSION}.exe"
InstallDir "$PROGRAMFILES64\Fid"
InstallDirRegKey HKLM "${PRODUCT_REGKEY}" "InstallDir"
ShowInstDetails show
ShowUnInstDetails show
SetCompressor /SOLID lzma
RequestExecutionLevel admin
Unicode True

; ─── Version Info ─────────────────────────────────────────────────────────

VIProductVersion "1.0.0.0"
VIAddVersionKey "ProductName" "Fid Language"
VIAddVersionKey "CompanyName" "Pledge"
VIAddVersionKey "LegalCopyright" "© 2026 Pledge"
VIAddVersionKey "FileDescription" "Fid Language Installer"
VIAddVersionKey "FileVersion" "${VERSION}"
VIAddVersionKey "ProductVersion" "${VERSION}"

; ─── Modern UI Settings ──────────────────────────────────────────────────

!define MUI_ABORTWARNING
!define MUI_FINISHPAGE_RUN "$INSTDIR\fid.exe"
!define MUI_FINISHPAGE_RUN_PARAMETERS "--version"

; ─── Pages ────────────────────────────────────────────────────────────────

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "license.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

; ─── Languages ──────────────────────────────────────────────────────────

!insertmacro MUI_LANGUAGE "English"

; ─── PATH ──────────────────────────────────────────────────────────────────
; Remove directory $R9 from the system PATH (every place it appears: alone,
; first, in the middle, last). The uninstaller of 0.8.0 and earlier wrote
; PATH back unchanged, so its directory stayed on PATH after uninstall.

!macro REMOVE_FROM_PATH UN
Function ${UN}RemoveFromPath
  Push $0
  ReadRegStr $0 HKLM "${ENV_HKLM}" "PATH"
  StrCmp $0 "$R9" 0 +3
    StrCpy $0 ""
    Goto write
  ${${UN}WordReplace} "$0" ";$R9;" ";" "+" $0
  ${${UN}WordReplace} "$0" ";$R9" "" "+}" $0
  ${${UN}WordReplace} "$0" "$R9;" "" "+{" $0
  write:
  WriteRegExpandStr HKLM "${ENV_HKLM}" "PATH" "$0"
  Pop $0
FunctionEnd
!macroend
!insertmacro REMOVE_FROM_PATH ""
!insertmacro REMOVE_FROM_PATH "un."

; ─── An install under the old name ──────────────────────────────────────────
; Fides 0.8.0 and earlier: run its uninstaller silently in its own directory,
; then take that directory off PATH (its uninstaller did not).

Function .onInit
  ReadRegStr $R0 HKLM "${OLD_UNINST_KEY}" "UninstallString"
  StrCmp $R0 "" done
  ReadRegStr $R1 HKLM "${OLD_REGKEY}" "InstallDir"
  StrCmp $R1 "" done
  IfSilent run
  MessageBox MB_OKCANCEL|MB_ICONINFORMATION "Fides (the name of Fid up to 0.8.0) is installed in$\r$\n$R1$\r$\n$\r$\nIt will be uninstalled first. Your own files are not touched." IDOK run
  Abort
  run:
  ExecWait '$R0 /S _?=$R1'
  Delete "$R1\uninstall.exe"
  RMDir "$R1"
  StrCpy $R9 $R1
  Call RemoveFromPath
  DeleteRegKey HKLM "${OLD_UNINST_KEY}"
  DeleteRegKey HKLM "${OLD_REGKEY}"
  done:
FunctionEnd

; ─── Section: Core (required) ────────────────────────────────────────────

Section "Fides Compiler (required)" SecCore
  SectionIn RO

  SetOutPath "$INSTDIR"

  ; The self-hosted compiler (written in Fides, compiled by itself to the
  ; fixpoint by scripts/bootstrap.sh) as fid.exe. It finds the standard
  ; library in $INSTDIR\stdlib.
  File "..\release\fid.exe"

  ; The Rust bootstrap compiler (fidesc.exe), plus the companion tools.
  ; Stage them in release\ first (build-installer.sh / build-installer.yml do).
  File "..\release\fidesc.exe"
  File "..\release\ark.exe"
  File "..\release\scribe.exe"
  File "..\release\veil.exe"

  ; License file
  File "license.txt"

  ; Write registry keys
  WriteRegStr HKLM "${PRODUCT_REGKEY}" "InstallDir" "$INSTDIR"
  WriteRegStr HKLM "${PRODUCT_REGKEY}" "Version" "${VERSION}"

  ; Write uninstall info
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "DisplayName" "Fid Language"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "DisplayIcon" '"$INSTDIR\fid.exe"'
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${VERSION}"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_URL}"
  WriteRegDWORD HKLM "${PRODUCT_UNINST_KEY}" "NoModify" 1
  WriteRegDWORD HKLM "${PRODUCT_UNINST_KEY}" "NoRepair" 1

  ; Create uninstaller
  WriteUninstaller "$INSTDIR\uninstall.exe"

  ; Add to PATH once: a reinstall takes the old entry out first
  StrCpy $R9 "$INSTDIR"
  Call RemoveFromPath
  ReadRegStr $0 HKLM "${ENV_HKLM}" "PATH"
  StrCmp $0 "" +3
    WriteRegExpandStr HKLM "${ENV_HKLM}" "PATH" "$0;$INSTDIR"
    Goto +2
  WriteRegExpandStr HKLM "${ENV_HKLM}" "PATH" "$INSTDIR"

  ; Broadcast environment change
  SendMessage HWND_BROADCAST ${WM_SETTINGCHANGE} 0 "STR:Environment" /TIMEOUT=5000

  DetailPrint "Fid installed to $INSTDIR"
  DetailPrint "Added $INSTDIR to PATH"
SectionEnd

; ─── Section: Standard Library ───────────────────────────────────────────

Section "Standard Library" SecStdlib
  SetOutPath "$INSTDIR\stdlib"
  File /r "..\stdlib\*.fd"
  DetailPrint "Installed standard library"
SectionEnd

; ─── Section: C Runtime ──────────────────────────────────────────────────

Section "C Runtime" SecRuntime
  SetOutPath "$INSTDIR\runtime"
  File "..\runtime\*.c"
  File "..\runtime\*.h"
  DetailPrint "Installed C runtime"
SectionEnd

; ─── Section: Documentation ──────────────────────────────────────────────

Section "Documentation" SecDocs
  SetOutPath "$INSTDIR\docs"
  File /r "..\docs\*.*"
  DetailPrint "Installed documentation"
SectionEnd

; ─── Section: Examples ───────────────────────────────────────────────────

Section "Example Programs" SecExamples
  SetOutPath "$INSTDIR\examples"
  File "..\tests\bootstrap\hello.fd"
  File "..\examples\*.fd"
  File "..\examples\README.md"
  DetailPrint "Installed examples"
SectionEnd

; ─── Section: Seeded store of unit objects (inc-roadmap B6) ──────────────
; Objects of the core and stdlib units, built by build-installer.sh with
; `fid ccompile --inc --seed-store`; an installed fid looks in
; $INSTDIR\cache before compiling a unit (docs/reference/incremental.md).

Section "Prebuilt standard library objects" SecCache
  SetOutPath "$INSTDIR\cache"
  File /nonfatal "..\build\installer\cache\*.exe"
  DetailPrint "Installed the seeded store of unit objects"
SectionEnd

; ─── Section: File Association (.fd) ─────────────────────────────────────

Section "Associate .fd files" SecAssoc
  ; Register .fd extension
  WriteRegStr HKCR ".fd" "" "Fid.SourceFile"
  WriteRegStr HKCR "Fid.SourceFile" "" "Fid Source File"
  WriteRegStr HKCR "Fid.SourceFile\shell\open\command" "" '"$INSTDIR\fid.exe" "%1"'
  WriteRegStr HKCR "Fid.SourceFile\DefaultIcon" "" '"$INSTDIR\fid.exe",0'
  DeleteRegKey HKCR "Fides.SourceFile"
  DetailPrint "Associated .fd files with Fid"
SectionEnd

; ─── Section: Start Menu Shortcuts ───────────────────────────────────────

Section "Start Menu Shortcuts" SecShortcuts
  CreateDirectory "$SMPROGRAMS\Fid"
  CreateShortcut "$SMPROGRAMS\Fid\Fides Compiler.lnk" "$INSTDIR\fid.exe" \
    "" "$INSTDIR\fid.exe" 0
  CreateShortcut "$SMPROGRAMS\Fid\Uninstall Fid.lnk" "$INSTDIR\uninstall.exe" \
    "" "" 0
  DetailPrint "Created Start Menu shortcuts"
SectionEnd

; ─── Section Descriptions ────────────────────────────────────────────────

LangString DESC_Core ${LANG_ENGLISH} "The Fides compiler (fid.exe / fidesc.exe) and the companion tools ark, scribe, and veil. Required."
LangString DESC_Stdlib ${LANG_ENGLISH} "Standard library modules (core, vec, string, math, etc.)."
LangString DESC_Runtime ${LANG_ENGLISH} "C runtime files for SDL, audio, networking."
LangString DESC_Docs ${LANG_ENGLISH} "Language reference, guides, and roadmap documentation."
LangString DESC_Examples ${LANG_ENGLISH} "Example Fid programs to get started."
LangString DESC_Cache ${LANG_ENGLISH} "Prebuilt objects of the core and standard library units, so a first per-module build (fid ccompile --inc) does not compile them."
LangString DESC_Assoc ${LANG_ENGLISH} "Open .fd files with Fid when double-clicked."
LangString DESC_Shortcuts ${LANG_ENGLISH} "Create Start Menu shortcuts for Fid."

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
!insertmacro MUI_DESCRIPTION_TEXT ${SecCore} $(DESC_Core)
!insertmacro MUI_DESCRIPTION_TEXT ${SecStdlib} $(DESC_Stdlib)
!insertmacro MUI_DESCRIPTION_TEXT ${SecRuntime} $(DESC_Runtime)
!insertmacro MUI_DESCRIPTION_TEXT ${SecDocs} $(DESC_Docs)
!insertmacro MUI_DESCRIPTION_TEXT ${SecExamples} $(DESC_Examples)
!insertmacro MUI_DESCRIPTION_TEXT ${SecCache} $(DESC_Cache)
!insertmacro MUI_DESCRIPTION_TEXT ${SecAssoc} $(DESC_Assoc)
!insertmacro MUI_DESCRIPTION_TEXT ${SecShortcuts} $(DESC_Shortcuts)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

; ─── Uninstall ───────────────────────────────────────────────────────────

Section "Uninstall"
  ; Remove files
  Delete "$INSTDIR\fid.exe"
  ; fides.exe: the name in v1.0.0
  Delete "$INSTDIR\fides.exe"
  Delete "$INSTDIR\fidesc.exe"
  Delete "$INSTDIR\ark.exe"
  Delete "$INSTDIR\scribe.exe"
  Delete "$INSTDIR\veil.exe"
  Delete "$INSTDIR\license.txt"
  Delete "$INSTDIR\uninstall.exe"
  RMDir /r "$INSTDIR\stdlib"
  RMDir /r "$INSTDIR\runtime"
  RMDir /r "$INSTDIR\docs"
  RMDir /r "$INSTDIR\examples"
  RMDir /r "$INSTDIR\cache"

  ; Remove shortcuts
  RMDir /r "$SMPROGRAMS\Fid"

  ; Remove the install directory from PATH
  StrCpy $R9 "$INSTDIR"
  Call un.RemoveFromPath
  SendMessage HWND_BROADCAST ${WM_SETTINGCHANGE} 0 "STR:Environment" /TIMEOUT=5000

  ; Remove file association
  DeleteRegKey HKCR ".fd"
  DeleteRegKey HKCR "Fid.SourceFile"

  ; Remove registry keys
  DeleteRegKey HKLM "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_REGKEY}"

  ; Remove install directory (if empty)
  RMDir "$INSTDIR"

  DetailPrint "Fid uninstalled"
SectionEnd
