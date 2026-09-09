; installer/fides.nsi — NSIS installer for Fides Language
;
; Build:
;   makensis installer/fides.nsi
;
; Output:
;   installer/Fides-Setup-<version>.exe
;
; Requirements:
;   - NSIS 3.x (https://nsis.sourceforge.io/)
;   - On Linux: sudo apt install nsis
;   - On Windows: download from nsis.sourceforge.io
;
; The installer will:
;   - Install Fides to C:\Program Files\Fides\
;   - Copy compiler (fides.exe), stdlib, runtime, docs
;   - Add Fides to PATH
;   - Associate .fd files with Fides
;   - Create Start Menu shortcuts
;   - Include uninstaller

; ─── Version ─────────────────────────────────────────────────────────────

!define VERSION "1.0.0"
!define PRODUCT_NAME "Fides"
!define PRODUCT_PUBLISHER "Pledge"
!define PRODUCT_URL "https://github.com/medzer/pledge/p-lang"
!define PRODUCT_REGKEY "Software\Fides"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\Fides"

; ─── Includes ────────────────────────────────────────────────────────────

!include "MUI2.nsh"
!include "EnvVarUpdate.nsh"
!include "FileAssociation.nsh"

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

; ─── Version Info (shows in file properties) ─────────────────────────────

VIProductVersion "1.0.0.0"
VIAddVersionKey "ProductName" "Fides Language"
VIAddVersionKey "CompanyName" "Pledge"
VIAddVersionKey "LegalCopyright" "© 2026 Pledge"
VIAddVersionKey "FileDescription" "Fides Language Installer"
VIAddVersionKey "FileVersion" "${VERSION}"
VIAddVersionKey "ProductVersion" "${VERSION}"

; ─── Modern UI Settings ──────────────────────────────────────────────────

!define MUI_ABORTWARNING
; !define MUI_ICON "fides.ico"
; !define MUI_UNICON "fides.ico"
!define MUI_FINISHPAGE_RUN "$INSTDIR\fides.exe"
!define MUI_FINISHPAGE_RUN_PARAMETERS "--version"
!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\docs\guides\quickstart.md"

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

  ; Compiler binary (renamed from fides-c3.exe to fides.exe)
  File /oname=fides.exe "..\release\fides-c3.exe"

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

  ; Add to PATH
  ${EnvVarUpdate} $0 "PATH" "A" "HKLM" "$INSTDIR"

  DetailPrint "Fides installed to $INSTDIR"
  DetailPrint "Added $INSTDIR to PATH"
SectionEnd

; ─── Section: Standard Library ───────────────────────────────────────────

Section "Standard Library" SecStdlib
  SetOutPath "$INSTDIR\stdlib"
  File /r "..\stdlib\*.fd"
  DetailPrint "Installed standard library (99 modules)"
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
  ${RegisterExtension} "$INSTDIR\fides.exe" ".fd" "Fides Source File"
  DetailPrint "Associated .fd files with Fides"
SectionEnd

; ─── Section: Start Menu Shortcuts ───────────────────────────────────────

Section "Start Menu Shortcuts" SecShortcuts
  CreateDirectory "$SMPROGRAMS\Fides"
  CreateShortcut "$SMPROGRAMS\Fides\Fides Compiler.lnk" "$INSTDIR\fides.exe" \
    "" "$INSTDIR\fides.exe" 0
  CreateShortcut "$SMPROGRAMS\Fides\Documentation.lnk" "$INSTDIR\docs\guides\quickstart.md" \
    "" "" 0
  CreateShortcut "$SMPROGRAMS\Fides\Uninstall Fides.lnk" "$INSTDIR\uninstall.exe" \
    "" "" 0
  DetailPrint "Created Start Menu shortcuts"
SectionEnd

; ─── Section Descriptions ────────────────────────────────────────────────

LangString DESC_Core ${LANG_ENGLISH} "The Fides compiler (fides.exe). Required."
LangString DESC_Stdlib ${LANG_ENGLISH} "Standard library modules (core, vec, string, math, etc.)."
LangString DESC_Runtime ${LANG_ENGLISH} "C runtime files for SDL, audio, networking, and other host functionality."
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
  Delete "$INSTDIR\license.txt"
  Delete "$INSTDIR\uninstall.exe"
  RMDir /r "$INSTDIR\stdlib"
  RMDir /r "$INSTDIR\runtime"
  RMDir /r "$INSTDIR\docs"
  RMDir /r "$INSTDIR\examples"

  ; Remove shortcuts
  RMDir /r "$SMPROGRAMS\Fides"

  ; Remove from PATH
  ${un.EnvVarUpdate} $0 "PATH" "R" "HKLM" "$INSTDIR"

  ; Remove file association
  ${UnRegisterExtension} ".fd" "Fides Source File"

  ; Remove registry keys
  DeleteRegKey HKLM "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_REGKEY}"

  ; Remove install directory (if empty)
  RMDir "$INSTDIR"

  DetailPrint "Fides uninstalled"
SectionEnd
