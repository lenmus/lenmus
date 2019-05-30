;==============================================================================================
; Windows installer for LenMus. Unicode version
;
; NSIS v3.04 script for generating the exe installer and uninstaller
;
; To add a new language:
;   Search for ADD_LANG and modify inthese points
;   Chack that language is supported by NSIS in NSIS/Contrib/Language files/
;
;
;--------------------------------------------------------------------------------------
;    LenMus Phonascus: The teacher of music
;    Copyright (c) 2002-2019 Cecilio Salmeron
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
  SetCompressor /SOLID lzma

;use the new XP controls style when running on Windows XP
  XPStyle on

;set Execution Level to Admin
  RequestExecutionLevel admin

;some helper defines and variables
  !include "LenMusVersion.nsh"
  !insertmacro LenMusVersion

  Name "lenmus v${APP_VERSION}"     ;product name displayed by the installer


;Specify path and name of resulting installer


;define path and filename of installer and uninstaller
  !define INSTALLER_NAME "lenmus_${APP_VERSION}_setup.exe"
  !define UNINSTALLER_NAME "uninstall_lenmus_${APP_VERSION}.exe"
  OutFile "${INSTALLER_NAME}"

;define the default installation directory and registry key
  !define REGISTRY_UNINSTALL "LenMus_${APP_VERSION}"
  !define REGISTRY_DATA "LenMus\Phonascus\${APP_VERSION}"

  InstallDir "$PROGRAMFILES\LenMus${APP_VERSION}"
  InstallDirRegKey HKCU "Software\${REGISTRY_DATA}" ""


;support for Modern UI
  !include "MUI2.nsh"

;support for GetParent
  !include "FileFunc.nsh"
  !insertmacro un.GetParent
  
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
    !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\${REGISTRY_DATA}"
    !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "StartMenuFolder"
    !define MUI_STARTMENUPAGE_DEFAULTFOLDER $(SM_PRODUCT_GROUP)
    !insertmacro MUI_PAGE_STARTMENU Application $STARTMENU_FOLDER

  ;instalation page
    !insertmacro MUI_PAGE_INSTFILES

  ;finish page: run installed program?
    !define MUI_FINISHPAGE_RUN "$INSTDIR\bin\${LENMUS_EXE}"
    !insertmacro MUI_PAGE_FINISH


;MUI: uninstall Pages
  ;ask for confirmation to uninstall
  !insertmacro MUI_UNPAGE_CONFIRM
  
  ;proceed to uninstall
  !insertmacro MUI_UNPAGE_INSTFILES



;language selection dialog settings
  ;Remember the installer language
  !define MUI_LANGDLL_REGISTRY_ROOT "HKCU"
  !define MUI_LANGDLL_REGISTRY_KEY "Software\${REGISTRY_DATA}"
  !define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"


  ;available languages
  !define MUI_LANGDLL_ALLLANGUAGES
  
  ; ADD_LANG
  !insertmacro MUI_LANGUAGE "English" ;first language is the default language
  !insertmacro MUI_LANGUAGE "Basque"
  !insertmacro MUI_LANGUAGE "SimpChinese"
  !insertmacro MUI_LANGUAGE "Dutch"
  !insertmacro MUI_LANGUAGE "French"         
  !insertmacro MUI_LANGUAGE "Galician"
  !insertmacro MUI_LANGUAGE "German"
;**  !insertmacro MUI_LANGUAGE "Greek"
  !insertmacro MUI_LANGUAGE "Italian"
  !insertmacro MUI_LANGUAGE "Spanish"
  !insertmacro MUI_LANGUAGE "Turkish"

  ;reserve files for languages
    ;These files should be inserted before other files in the data block
    ;Keep these lines before any File command
    !insertmacro MUI_RESERVEFILE_LANGDLL


;variable to retry installation when error found
  var "STEP"


;instruct installer and uninstaller to show install/uninstall log to the user
  ShowInstDetails show                   ;show install log 
  ShowUnInstDetails show                 ;show uninstall log




;---------------------------------------------------------------------------------------------------
; ADD_LANG
;languaje files to support different languages during installation
;---------------------------------------------------------------------------------------------------
  !addincludedir ".\locale"
  !include "eu.nsh"
  !include "zh_CN.nsh"
  !include "nl.nsh"
  !include "en.nsh"
  !include "fr.nsh"
  !include "gl_ES.nsh"
  !include "de.nsh"
;**  !include "el_GR.nsh"
  !include "it.nsh"
  !include "es.nsh"
  !include "tr.nsh"





; *********************************************************************
; Install Sections
; *********************************************************************

;Specify the requested execution level for Windows Vista and Windows 7
  RequestExecutionLevel admin
 

;Start installation
Function .onInit

    ;Check if the user is running multiple instances of the installer
    System::Call 'kernel32::CreateMutexA(i 0, i 0, t "LenMusMutex") i .r1 ?e'
    Pop $R0
    StrCmp $R0 0 +3
      MessageBox MB_OK|MB_ICONEXCLAMATION $(MSG_Running)
      Abort "$(MSG_ABORT)"

    ;Show installer language selection page
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
     ;SetOutPath "$INSTDIR\docs"
     ;File "..\..\docs\html\install.htm"

    ; ADD_LANG
     File ".\locale\license_en.txt"
     File ".\locale\license_es.txt"
     File ".\locale\license_fr.txt"
     File ".\locale\license_tr.txt"
     File ".\locale\license_nl.txt"
     File ".\locale\license_eu.txt"
     File ".\locale\license_it.txt"
     File ".\locale\license_gl_ES.txt"
     File ".\locale\license_de.txt"
;**     File ".\locale\license_el_GR.txt"
     
     File ".\locale\LICENSE_GNU_GPL_1.3.txt"
     File ".\locale\LICENSE_GNU_FDL_1.3.txt"
     
     
     SetOutPath "$INSTDIR\bin"
     File "..\..\zz_build-area\bin\*.*"
     File "libpng16.dll"
     File "zlib.dll"
     ;File "${LENMUS_EXE}"       ;** REMEMBER to place exec in \installer\msw\ folder !
     ;File "..\..\..\..\lomse\trunk\packages\freetype\bin\freetype6.dll"
     ;File "..\..\..\..\lomse\trunk\packages\freetype\bin\zlib1.dll"
     ;File "..\..\packages\wxMidi\lib\pm\pm_dll.dll"
     ;File "..\..\packages\wxSQLite3\sqlite3\lib\sqlite3.dll"
     ;File "msvcr71.dll"
     ;File "msvcp71.dll"

    ;locale. Common folder
     SetOutPath "$INSTDIR\locale\common"
     File "..\..\locale\common\*.*"

    ; ADD_LANG
     SetOutPath "$INSTDIR\locale\en"
     File "..\..\locale\en\*.htm"
     SetOutPath "$INSTDIR\locale\es"
     File "..\..\locale\es\*.mo"
     File "..\..\locale\es\*.htm"
     SetOutPath "$INSTDIR\locale\fr"
     File "..\..\locale\fr\*.mo"
;**     File "..\..\locale\fr\*.htm"
     SetOutPath "$INSTDIR\locale\tr"
     File "..\..\locale\tr\*.mo"
;**     File "..\..\locale\tr\*.htm"
     SetOutPath "$INSTDIR\locale\nl"
     File "..\..\locale\nl\*.mo"
;**     File "..\..\locale\nl\*.htm"
     SetOutPath "$INSTDIR\locale\eu"
     File "..\..\locale\eu\*.mo"
;**     File "..\..\locale\eu\*.htm"
     SetOutPath "$INSTDIR\locale\it"
     File "..\..\locale\it\*.mo"
;**     File "..\..\locale\it\*.htm"
     SetOutPath "$INSTDIR\locale\gl_ES"
     File "..\..\locale\gl_ES\*.mo"
     File "..\..\locale\gl_ES\*.htm"
     SetOutPath "$INSTDIR\locale\zh_CN"
     File "..\..\locale\zh_CN\*.mo"
     File "..\..\locale\zh_CN\*.htm"
     SetOutPath "$INSTDIR\locale\de"
     File "..\..\locale\de\*.mo"
     File "..\..\locale\de\*.htm"
;**     SetOutPath "$INSTDIR\locale\el_GR"
;**     File "..\..\locale\el_GR\*.mo"
;**     File "..\..\locale\el_GR\*.htm"

    ; ADD_LANG
     SetOutPath "$INSTDIR\locale\en\books"
     File "..\..\locale\en\books\*.lmb"
     SetOutPath "$INSTDIR\locale\es\books"
     File "..\..\locale\es\books\*.lmb"
     SetOutPath "$INSTDIR\locale\fr\books"
     File "..\..\locale\fr\books\*.lmb"
     SetOutPath "$INSTDIR\locale\tr\books"
     File "..\..\locale\tr\books\*.lmb"
     SetOutPath "$INSTDIR\locale\nl\books"
     File "..\..\locale\nl\books\*.lmb"
     SetOutPath "$INSTDIR\locale\eu\books"
     File "..\..\locale\eu\books\*.lmb"
     SetOutPath "$INSTDIR\locale\it\books"
     File "..\..\locale\it\books\*.lmb"
     SetOutPath "$INSTDIR\locale\gl_ES\books"
     File "..\..\locale\gl_ES\books\*.lmb"
     SetOutPath "$INSTDIR\locale\zh_CN\books"
     File "..\..\locale\zh_CN\books\*.lmb"
     SetOutPath "$INSTDIR\locale\de\books"
     File "..\..\locale\de\books\*.lmb"
;**     SetOutPath "$INSTDIR\locale\el_GR\books"
;**     File "..\..\locale\el_GR\books\*.lmb"

    
    ;resources
     SetOutPath "$INSTDIR\res\bitmaps"
     File "..\..\res\bitmaps\*.*"
     SetOutPath "$INSTDIR\res\icons"
     File "..\..\res\icons\*.*"
     SetOutPath "$INSTDIR\res\sounds"
     File "..\..\res\sounds\*.*"
     ;SetOutPath "$INSTDIR\res\keys"
     ;File "..\..\res\keys\*.png"
     SetOutPath "$INSTDIR\res\cursors"
     File "..\..\res\cursors\*.png"
     
     SetOutPath "$INSTDIR\res\fonts"
     File "..\..\res\fonts\*.ttf"
     File "..\..\res\fonts\*.ttc"

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



  ;create entries in Start Menu folder
  ;-----------------------------------------------------------------------------------
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\$(SHORTCUT_NAME_EXEC).lnk" "$INSTDIR\bin\${LENMUS_EXE}"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\$(SHORTCUT_NAME_UNINSTALL).lnk" "$INSTDIR\bin\${UNINSTALLER_NAME}"
  !insertmacro MUI_STARTMENU_WRITE_END

    ;write uninstaller
    WriteUninstaller "$INSTDIR\bin\${UNINSTALLER_NAME}"
    
    
; Add uninstall information to Add/Remove Programs so that lenmus program 
; will be included on the "Add/Remove Programs" folder of the Control Pannel 
; http://msdn.microsoft.com/en-us/library/aa372105%28VS.85%29.aspx
;-----------------------------------------------------------------------------------------------

    ; mandatory keys for un-installing
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${REGISTRY_UNINSTALL}" "DisplayName" "$(^Name)"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${REGISTRY_UNINSTALL}" "UninstallString" "$INSTDIR\bin\${UNINSTALLER_NAME}"

    ; optional information
    
    ;NoModify (DWORD) - 1 if uninstaller has no option to modify the installed application
    ;NoRepair (DWORD) - 1 if the uninstaller has no option to repair the installation
    WriteRegDWord HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${REGISTRY_UNINSTALL}" "NoModifiy" 1
    WriteRegDWord HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${REGISTRY_UNINSTALL}" "NoRepair" 1

    ; additional info for updater
    WriteRegStr HKCU "Software\${REGISTRY_DATA}" "InstallRoot" "$INSTDIR"

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
     CreateShortCut "$DESKTOP\lenmus ${APP_VERSION}.lnk" "$INSTDIR\bin\${LENMUS_EXE}"
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



;assign descriptions to sections -------------------------------------------------------

  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${CreateIcon} $(DESC_CreateIcon)
    !insertmacro MUI_DESCRIPTION_TEXT ${Scores} $(DESC_Scores)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END




############################################################################################
;                                 Uninstaller                                              ;
############################################################################################

Function un.onInit
  ;first of all, show installer language selection page if language not set
    !insertmacro MUI_LANGDLL_DISPLAY

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
  
  ; move to root directory and delete all folders and files
  ${un.GetParent} "$INSTDIR" $LENMUS_DIR
  !insertmacro RemoveFilesAndSubDirs "$LENMUS_DIR"
  RMDir "$LENMUS_DIR"

  ;delete link on desktop
  Delete "$DESKTOP\lenmus ${APP_VERSION}.lnk"
  
  ;delete Start Menu folder entries
  !insertmacro MUI_STARTMENU_GETFOLDER Application $MUI_TEMP
  Delete "$SMPROGRAMS\$MUI_TEMP\*.*"
  StrCpy $MUI_TEMP "$SMPROGRAMS\$MUI_TEMP"
  RMDir $MUI_TEMP

  ;delete registry keys created by the installer
  DeleteRegKey  HKCU "Software\${REGISTRY_DATA}"
  DeleteRegKey  HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${REGISTRY_UNINSTALL}"
  
  SetAutoClose false
  
SectionEnd

;End of lenmus install/uninstall script
