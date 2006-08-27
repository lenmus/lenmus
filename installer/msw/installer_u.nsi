;==============================================================================================
; Windows installer for LenMus. Unicode version
;
; NSIS v2.15 script for generating the exe installer and uninstaller
;
;--------------------------------------------------------------------------------------
;    LenMus Phonascus: The teacher of music
;    Copyright (c) 2002-2006 Cecilio Salmeron
;
;    This program is free software; you can redistribute it and/or modify it under the 
;    terms of the GNU General Public License as published by the Free Software Foundation;
;    either version 2 of the License, or (at your option) any later version.
;
;    This program is distributed in the hope that it will be useful, but WITHOUT ANY 
;    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
;    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
;
;    You should have received a copy of the GNU General Public License along with this 
;    program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, 
;    Fifth Floor, Boston, MA  02110-1301, USA.
;
;    For any comment, suggestion or feature request, please contact the manager of 
;    the project at cecilios@users.sourceforge.net
;
;==============================================================================================

;perform a CRC on the installer before allowing an install to make sure the installer
;is not corrupted
  CRCCheck on

;set the compression algorithm to use (zlib | bzip2 | lzma)
  SetCompressor lzma

;use the new XP controls style when running on Windows XP
  XPStyle on

;some helper defines and variables
  !define APP_VERSION "3.2"
  !define APP_NAME "LenMus Phonascus ${APP_VERSION}"
  !define APP_HOME_PAGE "http://www.lenmus.org/"

  Name "lenmus v3.2"     ;product name displayed by the installer


;support for Modern UI
  !include "MUI.nsh"

;support for GetParent
  !include "FileFunc.nsh"
  !insertmacro un.GetParent
  
;support to install fonts
  !include "FontRegAdv.nsh"
  !include "FontName.nsh"
  
;support to remove files and subdirs in one step
  !include "RemoveFilesAndSubDirs.nsh"
  


;configuration settings for the MUI interface: options, icons, bitmaps, colors, etc.
  ;show a warning when the user cancels the install
    !define MUI_ABORTWARNING
  ;icons to associate to the resulting installer and uninstaller
    !define MUI_ICON "win-install.ico"
    !define MUI_UNICON "win-uninstall.ico"

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "BannerTop.bmp"
!define MUI_WELCOMEFINISHPAGE_BITMAP "BannerLeft.bmp"
!define MUI_COMPONENTSPAGE_SMALLDESC        ;descriptions at bottom

;vars to create links on "Start Menu" folder
  Var MUI_TEMP
  Var STARTMENU_FOLDER
  
;var to store root install directory in uninstall section
  Var LENMUS_DIR


;MUI: install pages to show
  ;welcome page
    !insertmacro MUI_PAGE_WELCOME

  ;show and accept licence
    !insertmacro MUI_PAGE_LICENSE $(license)

  ;choose installation directory
    !insertmacro MUI_PAGE_DIRECTORY

  ;select optional components
    !define MUI_COMPONENTSPAGE_TEXT_COMPLIST $(MSG_OptionalComponents)
    !insertmacro MUI_PAGE_COMPONENTS

  ;ask about creating links on Start Menu
    !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU"
    !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\LenMus"
    !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "StartMenuFolder"
    !define MUI_STARTMENUPAGE_DEFAULTFOLDER $(SM_PRODUCT_GROUP)
    !insertmacro MUI_PAGE_STARTMENU Application $STARTMENU_FOLDER

  ;instalation page
    !insertmacro MUI_PAGE_INSTFILES

  ;finish page: run installed program?
    !define MUI_FINISHPAGE_RUN "$INSTDIR\bin\lenmus_u.exe"
    !insertmacro MUI_PAGE_FINISH


;MUI: uninstall Pages
  ;ask for confirmation to uninstall
  !insertmacro MUI_UNPAGE_CONFIRM
  
  ;proceed to uninstall
  !insertmacro MUI_UNPAGE_INSTFILES



;language selection dialog settings
  ;Remember the installer language
  !define MUI_LANGDLL_REGISTRY_ROOT "HKCU"
  !define MUI_LANGDLL_REGISTRY_KEY "Software\LenMus"
  !define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"

  ;available languages
  !insertmacro MUI_LANGUAGE "Spanish"
  !insertmacro MUI_LANGUAGE "English"

  ;reserve files for languages
    ;These files should be inserted before other files in the data block
    ;Keep these lines before any File command
    !insertmacro MUI_RESERVEFILE_LANGDLL



;define filename of installer and uninstaller
  !define INSTALLER_NAME "lenmus_${APP_VERSION}u_setup.exe"
  !define UNINSTALLER_NAME "uninstall_lenmus_${APP_VERSION}.exe"

;variable to retry installation when error found
  var "STEP"


;Specify path and name of resulting installer
  OutFile "${INSTALLER_NAME}"

;define the default installation directory
  InstallDir "$PROGRAMFILES\LenMus${APP_VERSION}"

;instruct installer and uninstaller to show install/uninstall log to the user
  ShowInstDetails show                   ;show install log 
  ShowUnInstDetails show                 ;show uninstall log




;---------------------------------------------------------------------------------------------------
;---------------------------------------------------------------------------------------------------
; Language files. 
; If this grows too much, move to language files, and uncomment the three next senteces
;---------------------------------------------------------------------------------------------------
;---------------------------------------------------------------------------------------------------
;languaje files to support different languages <---- Commented for now
  ;!addincludedir "."
  ;!include "lenmus_spanish.nsh"
  ;!include "lenmus_english.nsh"



; File "lenmus_spanish.nsh"
;========================================================================================

;Spanish language file for lenmus installer/uninstaller
;---------------------------------------------------------------------------------------------------------

LicenseLangString license ${LANG_Spanish} "license_spanish.txt"

;strings to customize MUI pages
LangString MSG_OptionalComponents   ${LANG_Spanish} "Por favor, seleccione los componentes opcionales que desee instalar"


;section titles, what user sees to select components for installation
LangString TITLE_CreateIcon     ${LANG_Spanish} "Acceso directo en escritorio"
LangString TITLE_Scores         ${LANG_Spanish} "Partituras de ejemplo"
LangString TITLE_RegKeys        ${LANG_Spanish} "Incluir en panel de control ('Agregar/quitar software')"

;descriptions for the sections. Displayed to user when mouse hovers over a section
LangString DESC_CreateIcon      ${LANG_Spanish} "Crear un icono de acceso directo en el escritorio"
LangString DESC_Scores          ${LANG_Spanish} "Crear una carpeta con algunas partituras de ejemplo"
LangString DESC_RegKeys         ${LANG_Spanish} "Crea claves en el Registro de Windows para que LenMus aparezca en 'Agregar/quitar programas'"


;error messages and other texts
LangString ERROR_CopyFiles          ${LANG_Spanish} "Error copiando archivos de programa"
LangString ERROR_InstallFonts       ${LANG_Spanish} "Error instalando el font"
LangString ERROR_CreateIcon         ${LANG_Spanish} "Error creando acceso directo en el escritorio"
LangString ERROR_CopyScores         ${LANG_Spanish} "Error copiando partituras de ejemplo"
LangString MSG_CONTINUE             ${LANG_Spanish} "¿Quiere continuar instalando?"
LangString MSG_ABORT                ${LANG_Spanish} "Instalación cancelada"


;start menu & desktop descriptions
LangString SM_PRODUCT_GROUP             ${LANG_Spanish} "${APP_NAME}"
LangString SHORTCUT_NAME_EXEC           ${LANG_Spanish} "Ejecutar ${APP_NAME}"
LangString SHORTCUT_NAME_UNINSTALL      ${LANG_Spanish} "Desinstalar ${APP_NAME}"

;End Spanish language file
;=========================================================================================



; File "lenmus_english.nsh"
;========================================================================================
;English language file for lenmus installer/uninstaller
;---------------------------------------------------------------------------------------------------------

LicenseLangString license ${LANG_English} "license_english.txt"

;strings to customize MUI pages
LangString MSG_OptionalComponents   ${LANG_English} "Please, choose the optional components you would like to install"


;section titles, what user sees to select components for installation
LangString TITLE_CreateIcon     ${LANG_English} "Shorcut on desktop"
LangString TITLE_Scores         ${LANG_English} "Examples of music scores"
LangString TITLE_RegKeys        ${LANG_English} "Add to Control Panel (to 'Add/Remove Programs')"

;descriptions for the sections. Displayed to user when mouse hovers over a section
LangString DESC_CreateIcon      ${LANG_English} "Create a shorcut icon on the desktop"
LangString DESC_Scores          ${LANG_English} "Create a folder containing some examples of music scores"
LangString DESC_RegKeys         ${LANG_English} "Create keys in Windows Registry to add LenMus to 'Add/Remove Programs' in the Control Panel."


;error messages and other texts
LangString ERROR_CopyFiles          ${LANG_English} "Error copying program files"
LangString ERROR_InstallFonts       ${LANG_English} "Error installing font"
LangString ERROR_CreateIcon         ${LANG_English} "Error creating shortcut on desktop"
LangString ERROR_CopyScores         ${LANG_English} "Error copying examples of music scores"
LangString MSG_CONTINUE             ${LANG_English} "Would you like to continue the installation?"
LangString MSG_ABORT                ${LANG_English} "Installation canceled"


;start menu & desktop descriptions
LangString SM_PRODUCT_GROUP             ${LANG_English} "${APP_NAME}"
LangString SHORTCUT_NAME_EXEC           ${LANG_English} "Run ${APP_NAME}"
LangString SHORTCUT_NAME_UNINSTALL      ${LANG_English} "Uninstall ${APP_NAME}"

;End English language file
;=========================================================================================

 


; *********************************************************************
; Install Sections
; *********************************************************************

;first of all, show installer language selection page
Function .onInit
  !insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd


;Install all the mandatory components
Section  "-" "MainSection"

  ; vars initialization
  StrCpy "$STEP" "Nil" 

  ;If a previous version exits delete old installed files, but no scores
  ;-----------------------------------------------------------------------------------
  !insertmacro RemoveFilesAndSubDirs "$INSTDIR\bin"
  !insertmacro RemoveFilesAndSubDirs "$INSTDIR\locale"
  !insertmacro RemoveFilesAndSubDirs "$INSTDIR\res"
  !insertmacro RemoveFilesAndSubDirs "$INSTDIR\xrc"
  !insertmacro RemoveFilesAndSubDirs "$INSTDIR\temp"


  ;install application files
  ;-----------------------------------------------------------------------------------
  CopyFiles:
     ClearErrors
     SetOverWrite try
     SetOutPath "$INSTDIR"
     File "..\..\docs\html\licence.htm"
     File "..\..\docs\html\installation.htm"
     File "..\..\docs\html\singledoc.css"
     File "..\..\docs\release\licence.txt"
     File "license_english.txt"
     File "license_spanish.txt"

     SetOutPath "$INSTDIR\bin"
     File "..\..\z_bin\lenmus_u.exe"
     File "..\..\fonts\lmbasic.ttf"
     File "msvcr71.dll"
     ;File "..\..\packages\wxMidi\lib\pm\pm_dll.dll"

     SetOutPath "$INSTDIR\locale\en"
     File "..\..\locale\en\*.*"
     SetOutPath "$INSTDIR\locale\es"
     File "..\..\locale\es\*.*"

     SetOutPath "$INSTDIR\res\bitmaps"
     File "..\..\res\bitmaps\*.*"
     SetOutPath "$INSTDIR\res\icons"
     File "..\..\res\icons\*.*"
     SetOutPath "$INSTDIR\res\sounds"
     File "..\..\res\sounds\*.*"
     SetOutPath "$INSTDIR\res\vbooks"
     File "..\..\res\vbooks\*.*"

     SetOutPath "$INSTDIR\xrc"
     File "..\..\xrc\*.xrc"

     SetOutPath "$INSTDIR\temp"
     File "..\..\temp\DataError.log"

      IfErrors +1 EndCopyFiles
        StrCmp $STEP "ErrorCopyingFiles" "Error_CopyFiles"
        StrCpy "$STEP" "ErrorCopyingFiles" 
      Goto CopyFiles
        Error_CopyFiles:
          MessageBox MB_RETRYCANCEL|MB_ICONEXCLAMATION $(ERROR_CopyFiles) IDRETRY CopyFiles
          MessageBox MB_YESNO|MB_ICONQUESTION $(MSG_CONTINUE) IDYES +2
      Abort "$(MSG_ABORT)"
     EndCopyFiles:


  ;install font
  ;-----------------------------------------------------------------------------------
  InstallFonts:
     ClearErrors
     StrCpy $FONT_DIR $FONTS
     !insertmacro InstallTTF '..\..\fonts\lmbasic.ttf'
     SendMessage ${HWND_BROADCAST} ${WM_FONTCHANGE} 0 0 /TIMEOUT=5000
     IfErrors +1 EndInstallFonts
        StrCmp $STEP "ErrorInstallingFonts" "Error_InstallFonts"
        StrCpy "$STEP" "ErrorInstallingFonts" 
      Goto InstallFonts
        Error_InstallFonts:
          MessageBox MB_RETRYCANCEL|MB_ICONEXCLAMATION $(ERROR_InstallFonts) IDRETRY InstallFonts
          MessageBox MB_YESNO|MB_ICONQUESTION $(MSG_CONTINUE) IDYES +2
      Abort "$(MSG_ABORT)"
     EndInstallFonts:

  ;create entries in Start Menu folder
  ;-----------------------------------------------------------------------------------
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\$(SHORTCUT_NAME_EXEC).lnk" "$INSTDIR\bin\lenmus_u.exe"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\$(SHORTCUT_NAME_UNINSTALL).lnk" "$INSTDIR\bin\${UNINSTALLER_NAME}"
  !insertmacro MUI_STARTMENU_WRITE_END

    ;write uninstaller
    WriteUninstaller "$INSTDIR\bin\${UNINSTALLER_NAME}"

SectionEnd

;===============================================================================================================
; Installation of optional components
;===============================================================================================================

;-----------------------------------------------------------------------------------------------
; shortcut icon on desktop
;-----------------------------------------------------------------------------------------------
Section $(TITLE_CreateIcon) CreateIcon

  CreateIcon:
     ClearErrors
     CreateShortCut "$DESKTOP\lenmus ${APP_VERSION}.lnk" "$INSTDIR\bin\lenmus_u.exe"
     IfErrors +1 EndCreateIcon
        StrCmp $STEP "ErrorCreatingIcon" "Error_CreateIcon"
        StrCpy "$STEP" "ErrorCreatingIcon" 
      Goto CreateIcon
        Error_CreateIcon:
          MessageBox MB_RETRYCANCEL|MB_ICONEXCLAMATION $(ERROR_CreateIcon) IDRETRY CreateIcon
          MessageBox MB_YESNO|MB_ICONQUESTION $(MSG_CONTINUE) IDYES +2
      Abort "$(MSG_ABORT)"
     EndCreateIcon:

SectionEnd

;-----------------------------------------------------------------------------------------------
; sample scores 
;-----------------------------------------------------------------------------------------------
Section $(TITLE_Scores) Scores

  CopyScores:
     ClearErrors
     SetOverwrite on
     SetOutPath "$INSTDIR\scores\MusicXML"
     File "..\..\scores\MusicXML\*.*"
     SetOutPath "$INSTDIR\scores\samples"
     File "..\..\scores\samples\*.*"
     IfErrors +1 EndCopyScores
        StrCmp $STEP "ErrorCopyingScores" "Error_CopyScores"
        StrCpy "$STEP" "ErrorCopyingScores" 
      Goto CopyScores
        Error_CopyScores:
          MessageBox MB_RETRYCANCEL|MB_ICONEXCLAMATION $(ERROR_CopyScores) IDRETRY CopyScores
          MessageBox MB_YESNO|MB_ICONQUESTION $(MSG_CONTINUE) IDYES +2
          Abort "$(MSG_ABORT)"
     EndCopyScores:

SectionEnd

;-----------------------------------------------------------------------------------------------
; Add uninstall information to Add/Remove Programs so that lenmus program 
; will be included on the "Add/Remove Programs" folder of the Control Pannel 
;-----------------------------------------------------------------------------------------------
Section $(TITLE_RegKeys) RegKeys

    ; mandatory keys for un-installing
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\LenMus" "DisplayName" "$(^Name)"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\LenMus" "UninstallString" "$INSTDIR\bin\${UNINSTALLER_NAME}"

    ; optional information
    
    ;NoModify (DWORD) - 1 if uninstaller has no option to modify the installed application
    ;NoRepair (DWORD) - 1 if the uninstaller has no option to repair the installation
    WriteRegDWord HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\LenMus" "NoModifiy" 1
    WriteRegDWord HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\LenMus" "NoRepair" 1

SectionEnd


;assign descriptions to sections -------------------------------------------------------

  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${CreateIcon} $(DESC_CreateIcon)
    !insertmacro MUI_DESCRIPTION_TEXT ${Scores} $(DESC_Scores)
    !insertmacro MUI_DESCRIPTION_TEXT ${RegKeys} $(DESC_RegKeys)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END




############################################################################################
;                                 Uninstaller                                              ;
############################################################################################

;get the language preference
Function un.onInit
  !insertmacro MUI_UNGETLANGUAGE
FunctionEnd

/*
Function un.onInit
  FindWindow $R0 "THMNISEdit2_MainWindowClass"
  IsWindow $R0 0 +3
  MessageBox MB_ICONEXCLAMATION|MB_OK "lenmus está ejecutándose. Antes de desinstalar lenmus debe cerrarlo."
  Abort

  UserInfo::GetAccountType
  Pop $R0
  StrCmp $R0 "Admin" 0 +2
  SetShellVarContext all
FunctionEnd
*/

Section un.Install
  ;the font will not be uninstalled as it might have been used by other applications
  
  ; move to root directory and delete all folders and files
  ${un.GetParent} "$INSTDIR" $LENMUS_DIR
  !insertmacro RemoveFilesAndSubDirs "$LENMUS_DIR"

  ;delete files
  ;Delete "$INSTDIR\*.*"
  ;Delete "$INSTDIR\bin\*.*"
  ;Delete "$INSTDIR\locale\en\*.*"
  ;Delete "$INSTDIR\locale\es\*.*"
  ;Delete "$INSTDIR\res\bitmaps\*.*"
  ;Delete "$INSTDIR\res\icons\*.*"
  ;Delete "$INSTDIR\res\sounds\*.*"
  ;Delete "$INSTDIR\res\vbooks\*.*"
  ;Delete "$INSTDIR\xrc\*.*"
  ;Delete "$INSTDIR\temp\*.*"
  ;Delete "$INSTDIR\scores\MusicXML\*.*"
  ;Delete "$INSTDIR\scores\samples\*.*"

  ;delete folders
  ;RMDir "$INSTDIR\bin"
  ;RMDir "$INSTDIR\locale\en"
  ;RMDir "$INSTDIR\locale\es"
  ;RMDir "$INSTDIR\locale"
  ;RMDir "$INSTDIR\res\bitmaps"
  ;RMDir "$INSTDIR\res\icons"
  ;RMDir "$INSTDIR\res\sounds"
  ;RMDir "$INSTDIR\res\vbooks"
  ;RMDir "$INSTDIR\res"
  ;RMDir "$INSTDIR\xrc"
  ;RMDir "$INSTDIR\temp"
  ;RMDir "$INSTDIR\scores\MusicXML"
  ;RMDir "$INSTDIR\scores\samples"
  ;RMDir "$INSTDIR\scores"
  RMDir "$LENMUS_DIR"

  ;delete ico on desktop
  Delete "$DESKTOP\lenmus ${APP_VERSION}.lnk"
  
  ;delete Start Menu folder entries
  !insertmacro MUI_STARTMENU_GETFOLDER Application $MUI_TEMP
  Delete "$SMPROGRAMS\$MUI_TEMP\*.*"
  StrCpy $MUI_TEMP "$SMPROGRAMS\$MUI_TEMP"
  RMDir $MUI_TEMP
  DeleteRegKey /ifempty HKCU "Software\LenMus"

  ;erase registry keys created by the installer
  DeleteRegKey  HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\LenMus\DisplayName"
  DeleteRegKey  HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\LenMus\UninstallString"
  DeleteRegKey  HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\LenMus\NoModifiy"
  DeleteRegKey  HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\LenMus\NoRepair"
  
  SetAutoClose false
  
SectionEnd

;End of lenmus install/uninstall script
