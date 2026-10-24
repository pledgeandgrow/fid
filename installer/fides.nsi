; installer/fides.nsi — NSIS installer for Fides Language
;
; Build:
;   makensis installer/fides.nsi
;
; Output:
;   installer/Fides-Setup-<version>.exe

; ─── Version ─────────────────────────────────────────────────────────────

!define VERSION "1.0.0"
!define PRODUCT_NAME "Fides"
!define PRODUCT_PUBLISHER "Pledge & Grow"
!define PRODUCT_URL "https://github.com/pledgeandgrow/p-lang"
!define PRODUCT_REGKEY "Software\Fides"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\Fides"
!define ENV_HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"

; ─── Includes ────────────────────────────────────────────────────────────

!include "MUI2.nsh"

; ─── General ──────────────────────────────────────────────────────────────

Name "${PRODUCT_NAME} ${VERSION}"
OutFile "Fides-Setup-${VERSION}.exe"
InstallDir "$PROGRAMFILES64\Fides"
InstallDirRegKey HKLM "${PRODUCT_REGKEY}" "InstallDir"
ShowInstDetails show
ShowUnInstDetails show
SetCompressor /SOLID lzma
RequestExecutionLevel admin
Unicode True

; ─── Version Info ─────────────────────────────────────────────────────────

VIProductVersion "1.0.0.0"
VIAddVersionKey "ProductName" "Fides Language"
VIAddVersionKey "CompanyName" "Pledge"
VIAddVersionKey "LegalCopyright" "© 2026 Pledge"
VIAddVersionKey "FileDescription" "Fides Language Installer"
VIAddVersionKey "FileVersion" "${VERSION}"
VIAddVersionKey "ProductVersion" "${VERSION}"

; ─── Modern UI Settings ──────────────────────────────────────────────────

!define MUI_ABORTWARNING
!define MUI_FINISHPAGE_RUN "$INSTDIR\fides.exe"
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

; ─── Section: Core (required) ────────────────────────────────────────────

Section "Fides Compiler (required)" SecCore
  SectionIn RO

  SetOutPath "$INSTDIR"

  ; Compiler binary (renamed from fides-c3.exe to fides.exe — kept for
  ; backwards compatibility with docs and shortcuts that say `fides`)
  File /oname=fides.exe "..\release\fides-c3.exe"

  ; The same compiler under its canonical name, plus the companion tools.
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
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "DisplayName" "Fides Language"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "DisplayIcon" '"$INSTDIR\fides.exe"'
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${VERSION}"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_URL}"
  WriteRegDWORD HKLM "${PRODUCT_UNINST_KEY}" "NoModify" 1
  WriteRegDWORD HKLM "${PRODUCT_UNINST_KEY}" "NoRepair" 1

  ; Create uninstaller
  WriteUninstaller "$INSTDIR\uninstall.exe"

  ; Add to PATH (append to existing PATH)
  ReadRegStr $0 HKLM "${ENV_HKLM}" "PATH"
  StrCmp $0 "" +3
    WriteRegExpandStr HKLM "${ENV_HKLM}" "PATH" "$0;$INSTDIR"
    Goto +2
  WriteRegExpandStr HKLM "${ENV_HKLM}" "PATH" "$INSTDIR"

  ; Broadcast environment change
  SendMessage HWND_BROADCAST ${WM_SETTINGCHANGE} 0 "STR:Environment" /TIMEOUT=5000

  DetailPrint "Fides installed to $INSTDIR"
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
  DetailPrint "Installed examples"
SectionEnd

; ─── Section: File Association (.fd) ─────────────────────────────────────

Section "Associate .fd files" SecAssoc
  ; Register .fd extension
  WriteRegStr HKCR ".fd" "" "Fides.SourceFile"
  WriteRegStr HKCR "Fides.SourceFile" "" "Fides Source File"
  WriteRegStr HKCR "Fides.SourceFile\shell\open\command" "" '"$INSTDIR\fides.exe" "%1"'
  WriteRegStr HKCR "Fides.SourceFile\DefaultIcon" "" '"$INSTDIR\fides.exe",0'
  DetailPrint "Associated .fd files with Fides"
SectionEnd

; ─── Section: Start Menu Shortcuts ───────────────────────────────────────

Section "Start Menu Shortcuts" SecShortcuts
  CreateDirectory "$SMPROGRAMS\Fides"
  CreateShortcut "$SMPROGRAMS\Fides\Fides Compiler.lnk" "$INSTDIR\fides.exe" \
    "" "$INSTDIR\fides.exe" 0
  CreateShortcut "$SMPROGRAMS\Fides\Uninstall Fides.lnk" "$INSTDIR\uninstall.exe" \
    "" "" 0
  DetailPrint "Created Start Menu shortcuts"
SectionEnd

; ─── Section Descriptions ────────────────────────────────────────────────

LangString DESC_Core ${LANG_ENGLISH} "The Fides compiler (fides.exe / fidesc.exe) and the companion tools ark, scribe, and veil. Required."
LangString DESC_Stdlib ${LANG_ENGLISH} "Standard library modules (core, vec, string, math, etc.)."
LangString DESC_Runtime ${LANG_ENGLISH} "C runtime files for SDL, audio, networking."
LangString DESC_Docs ${LANG_ENGLISH} "Language reference, guides, and roadmap documentation."
LangString DESC_Examples ${LANG_ENGLISH} "Example Fides programs to get started."
LangString DESC_Assoc ${LANG_ENGLISH} "Open .fd files with Fides when double-clicked."
LangString DESC_Shortcuts ${LANG_ENGLISH} "Create Start Menu shortcuts for Fides."

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
!insertmacro MUI_DESCRIPTION_TEXT ${SecCore} $(DESC_Core)
!insertmacro MUI_DESCRIPTION_TEXT ${SecStdlib} $(DESC_Stdlib)
!insertmacro MUI_DESCRIPTION_TEXT ${SecRuntime} $(DESC_Runtime)
!insertmacro MUI_DESCRIPTION_TEXT ${SecDocs} $(DESC_Docs)
!insertmacro MUI_DESCRIPTION_TEXT ${SecExamples} $(DESC_Examples)
!insertmacro MUI_DESCRIPTION_TEXT ${SecAssoc} $(DESC_Assoc)
!insertmacro MUI_DESCRIPTION_TEXT ${SecShortcuts} $(DESC_Shortcuts)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

; ─── Uninstall ───────────────────────────────────────────────────────────

Section "Uninstall"
  ; Remove files
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

  ; Remove shortcuts
  RMDir /r "$SMPROGRAMS\Fides"

  ; Remove from PATH (simple approach — just remove the install dir from PATH)
  ReadRegStr $0 HKLM "${ENV_HKLM}" "PATH"
  ; Write back without our path (best effort)
  WriteRegExpandStr HKLM "${ENV_HKLM}" "PATH" "$0"
  SendMessage HWND_BROADCAST ${WM_SETTINGCHANGE} 0 "STR:Environment" /TIMEOUT=5000

  ; Remove file association
  DeleteRegKey HKCR ".fd"
  DeleteRegKey HKCR "Fides.SourceFile"

  ; Remove registry keys
  DeleteRegKey HKLM "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_REGKEY}"

  ; Remove install directory (if empty)
  RMDir "$INSTDIR"

  DetailPrint "Fides uninstalled"
SectionEnd
