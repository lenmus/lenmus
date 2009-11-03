;==============================================================================================
; Windows installer for LenMus. Unicode version
;
; NSIS v2.15 script for generating the exe installer and uninstaller
;
; To add a new language:
;   Search for ADD_LANG and modify inthese points
;
;
;--------------------------------------------------------------------------------------
;    LenMus Phonascus: The teacher of music
;    Copyright (c) 2002-2009 LenMus project
;
;    This program is free software; you can redistribute it and/or modify it under the 
;    terms of the GNU General Public License as published by the Free Software Foundation;
;    either version 3 of the License, or (at your option) any later version.
;
;    This program is distributed in the hope that it will be useful, but WITHOUT ANY 
;    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
;    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
;
;    You should have received a copy of the GNU General Public License
;    along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
  !define APP_VERSION "4.1a1"               ;<--------- version 
  !define APP_NAME "LenMus Phonascus ${APP_VERSION}"
  !define APP_HOME_PAGE "http://www.lenmus.org/"

  Name "lenmus v4.1 a1"     ;product name displayed by the installer    ;<--------- version 


;support for Modern UI
  !include "MUI.nsh"

;support for GetParent
  !include "FileFunc.nsh"
  !insertmacro un.GetParent
  
;;support to install fonts
;  !include "FontRegAdv.nsh"
;  !include "FontName.nsh"
  
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
    !define MUI_FINISHPAGE_RUN "$INSTDIR\bin\lenmus_d.exe"    ;<--------- lenmus.exe 
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
  ; ADD_LANG
  !insertmacro MUI_LANGUAGE "Basque"
  !insertmacro MUI_LANGUAGE "Dutch"
  !insertmacro MUI_LANGUAGE "English"
  !insertmacro MUI_LANGUAGE "French"         
  !insertmacro MUI_LANGUAGE "Galician"
  !insertmacro MUI_LANGUAGE "Italian"
  !insertmacro MUI_LANGUAGE "Spanish"
  !insertmacro MUI_LANGUAGE "Turkish"

  ;reserve files for languages
    ;These files should be inserted before other files in the data block
    ;Keep these lines before any File command
    !insertmacro MUI_RESERVEFILE_LANGDLL



;define filename of installer and uninstaller
  !define INSTALLER_NAME "lenmus_${APP_VERSION}_setup.exe"
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
; ADD_LANG
;languaje files to support different languages during installation
;---------------------------------------------------------------------------------------------------
  !addincludedir ".\locale"
  !include "eu.nsh"
  !include "nl.nsh"
  !include "en.nsh"
  !include "fr.nsh"
  !include "gl_ES.nsh"
  !include "it.nsh"
  !include "es.nsh"
  !include "tr.nsh"





; *********************************************************************
; Install Sections
; *********************************************************************

;first of all, show installer language selection page
Function .onInit
  !insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd

;------------
;function to write an string into a file
; Usage:
;   Push "hello$\r$\n" ;text to write to file 
;   Push "$INSTDIR\log.txt" ;file to write to 
;   Call WriteToFile
;------------
Function WriteToFile
 Exch $0 ;file to write to
 Exch
 Exch $1 ;text to write
 
  FileOpen $0 $0 a #open file
   FileSeek $0 0 END #go to end
   FileWrite $0 $1 #write to file
  FileClose $0
 
 Pop $1
 Pop $0
FunctionEnd


;Install all the mandatory components
Section  "-" "MainSection"

  ; vars initialization
  StrCpy "$STEP" "Nil" 

  ;If a previous version exits delete old installed files, but no scores nor templates
  ;-----------------------------------------------------------------------------------
  !insertmacro RemoveFilesAndSubDirs "$INSTDIR\bin"
  !insertmacro RemoveFilesAndSubDirs "$INSTDIR\books"
  !insertmacro RemoveFilesAndSubDirs "$INSTDIR\docs"
  !insertmacro RemoveFilesAndSubDirs "$INSTDIR\locale"
  !insertmacro RemoveFilesAndSubDirs "$INSTDIR\logs"
  !insertmacro RemoveFilesAndSubDirs "$INSTDIR\res"
  !insertmacro RemoveFilesAndSubDirs "$INSTDIR\temp"
  !insertmacro RemoveFilesAndSubDirs "$INSTDIR\xrc"


  ;install application files
  ;-----------------------------------------------------------------------------------
  CopyFiles:
     ClearErrors
     SetOverWrite try
     SetOutPath "$INSTDIR\docs"

    ; ADD_LANG
     File ".\locale\license_en.txt"
     File ".\locale\license_es.txt"
     File ".\locale\license_fr.txt"
     File ".\locale\license_tr.txt"
     File ".\locale\license_nl.txt"
     File ".\locale\license_eu.txt"
     File ".\locale\license_it.txt"
     File ".\locale\license_gl_ES.txt"
     File ".\locale\LICENSE*"
     
     File "..\..\docs\html\LICENSE_GNU_GPL_1.3.txt"
     File "..\..\docs\html\LICENSE_GNU_FDL_1.3.txt"
     
     SetOutPath "$INSTDIR\docs\images"
     File "..\..\docs\html\images\*.*"

     SetOutPath "$INSTDIR\bin"
     File "..\..\z_bin\lenmus_d.exe"            ;<--------- lenmus.exe
     File "..\..\packages\freetype\bin\freetype6.dll"
     File "..\..\packages\freetype\bin\zlib1.dll"
;     File "..\..\packages\wxMidi\lib\pm\pm_dll.dll"
     File "..\..\packages\wxSQLite3\sqlite3\lib\sqlite3.dll"
     File "..\..\fonts\lmbasic2.ttf"
     File "msvcr71.dll"
     File "msvcp71.dll"

    ; ADD_LANG
     SetOutPath "$INSTDIR\books\en"
     File "..\..\books\en\*.lmb"
     SetOutPath "$INSTDIR\books\es"
     File "..\..\books\es\*.lmb"
     SetOutPath "$INSTDIR\books\fr"
     File "..\..\books\fr\*.lmb"
     SetOutPath "$INSTDIR\books\tr"
     File "..\..\books\tr\*.lmb"
     SetOutPath "$INSTDIR\books\nl"
     File "..\..\books\nl\*.lmb"
     SetOutPath "$INSTDIR\books\eu"
     File "..\..\books\eu\*.lmb"
     SetOutPath "$INSTDIR\books\it"
     File "..\..\books\it\*.lmb"
     SetOutPath "$INSTDIR\books\gl_ES"
     File "..\..\books\gl_ES\*.lmb"

    ; ADD_LANG
     SetOutPath "$INSTDIR\locale\en"
     File "..\..\locale\en\*.mo"
     File "..\..\locale\en\*.htm"
     File "..\..\locale\en\help.htb"
     File "..\..\locale\common\singledoc.css"
     SetOutPath "$INSTDIR\locale\es"
     File "..\..\locale\es\*.mo"
     File "..\..\locale\es\*.htm"
     File "..\..\locale\es\help.htb"
     File "..\..\locale\common\singledoc.css"
     SetOutPath "$INSTDIR\locale\fr"
     File "..\..\locale\fr\*.mo"
     File "..\..\locale\fr\*.htm"
     File "..\..\locale\fr\help.htb"
     File "..\..\locale\common\singledoc.css"
     SetOutPath "$INSTDIR\locale\tr"
     File "..\..\locale\tr\*.mo"
     File "..\..\locale\tr\*.htm"
     File "..\..\locale\tr\help.htb"
     File "..\..\locale\common\singledoc.css"
     SetOutPath "$INSTDIR\locale\nl"
     File "..\..\locale\nl\*.mo"
     File "..\..\locale\nl\*.htm"
     File "..\..\locale\nl\help.htb"
     File "..\..\locale\common\singledoc.css"
     SetOutPath "$INSTDIR\locale\eu"
     File "..\..\locale\eu\*.mo"
     File "..\..\locale\eu\*.htm"
     File "..\..\locale\eu\help.htb"
     File "..\..\locale\common\singledoc.css"
     SetOutPath "$INSTDIR\locale\it"
     File "..\..\locale\it\*.mo"
     File "..\..\locale\it\*.htm"
     File "..\..\locale\it\help.htb"
     File "..\..\locale\common\singledoc.css"
     SetOutPath "$INSTDIR\locale\gl_ES"
     File "..\..\locale\gl_ES\*.mo"
     File "..\..\locale\gl_ES\*.htm"
     File "..\..\locale\gl_ES\help.htb"
     File "..\..\locale\common\singledoc.css"


     SetOutPath "$INSTDIR\res\bitmaps"
     File "..\..\res\bitmaps\*.*"
     SetOutPath "$INSTDIR\res\icons"
     File "..\..\res\icons\*.*"
     SetOutPath "$INSTDIR\res\sounds"
     File "..\..\res\sounds\*.*"
     SetOutPath "$INSTDIR\res\keys"
     File "..\..\res\keys\*.png"
     SetOutPath "$INSTDIR\res\cursors"
     File "..\..\res\cursors\*.png"
;     SetOutPath "$INSTDIR\res\figures"
;     File "..\..\res\figures\*.png"

     SetOutPath "$INSTDIR\xrc"
     File "..\..\xrc\*.xrc"
     
     SetOutPath "$INSTDIR\templates"
     File "..\..\templates\*.lms"
     
     SetOutPath "$INSTDIR\temp"

     SetOutPath "$INSTDIR\logs"

      IfErrors +1 EndCopyFiles
        StrCmp $STEP "ErrorCopyingFiles" "Error_CopyFiles"
        StrCpy "$STEP" "ErrorCopyingFiles" 
      Goto CopyFiles
        Error_CopyFiles:
          MessageBox MB_RETRYCANCEL|MB_ICONEXCLAMATION $(ERROR_CopyFiles) IDRETRY CopyFiles
          MessageBox MB_YESNO|MB_ICONQUESTION $(MSG_CONTINUE) IDYES +2
      Abort "$(MSG_ABORT)"
     EndCopyFiles:

  ; Save install options
  ;-----------------------------------------------------------------------------------
     SetOutPath "$INSTDIR\bin"
     Push "$(OTHER_LangName)"                   ;text to write to file 
     Push "$INSTDIR\\bin\config_ini.txt"        ;file to write to 
     Call WriteToFile



;  ;install font
;  ;-----------------------------------------------------------------------------------
;  InstallFonts:
;     ClearErrors
;     StrCpy $FONT_DIR $FONTS
;     !insertmacro InstallTTF '..\..\fonts\lmbasic.ttf'
;     SendMessage ${HWND_BROADCAST} ${WM_FONTCHANGE} 0 0 /TIMEOUT=5000
;     IfErrors +1 EndInstallFonts
;        StrCmp $STEP "ErrorInstallingFonts" "Error_InstallFonts"
;        StrCpy "$STEP" "ErrorInstallingFonts" 
;      Goto InstallFonts
;        Error_InstallFonts:
;          MessageBox MB_RETRYCANCEL|MB_ICONEXCLAMATION $(ERROR_InstallFonts) IDRETRY InstallFonts
;          MessageBox MB_YESNO|MB_ICONQUESTION $(MSG_CONTINUE) IDYES +2
;      Abort "$(MSG_ABORT)"
;     EndInstallFonts:

  ;create entries in Start Menu folder
  ;-----------------------------------------------------------------------------------
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\$(SHORTCUT_NAME_EXEC).lnk" "$INSTDIR\bin\lenmus.exe"
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
     CreateShortCut "$DESKTOP\lenmus ${APP_VERSION}.lnk" "$INSTDIR\bin\lenmus_d.exe" ;<--------- lenmus.exe"
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
     File "..\..\scores\MusicXML\*.xml"
     SetOutPath "$INSTDIR\scores\samples"
     File "..\..\scores\samples\*.lms"
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
;;;;;;;  ;the font will not be uninstalled as it might have been used by other applications
  
  ; move to root directory and delete all folders and files
  ${un.GetParent} "$INSTDIR" $LENMUS_DIR
  !insertmacro RemoveFilesAndSubDirs "$LENMUS_DIR"
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
