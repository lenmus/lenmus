;==============================================================================================
; LenMus project.
; Windows intaller for a language pack
;
; NSIS v2.46 script for generating the installer for a language pack
;
;--------------------------------------------------------------------------------------
;    LenMus Phonascus: The teacher of music
;    Copyright (c) 2002-2012 Cecilio Salmeron
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
  
SilentInstall normal
WindowIcon on

;some helper defines and variables
  !define LANG_CODE "fr"
  !define LANG_NAME "French"
  !define APP_NAME "LenMus Phonascus ${APP_VERSION}"
  !define APP_VERSION "5.0b1"
  !define APP_HOME_PAGE "http://www.lenmus.org/"

;product name displayed by the installer
  Name "'Lenmus ${APP_VERSION} ${LANG_NAME} files'"
  
;define filename of installer
  !define INSTALLER_NAME "lenmus_${APP_VERSION}_${LANG_NAME}_updater.exe"

;Specify path and name of resulting installer
  OutFile "${INSTALLER_NAME}"
  !define REGISTRY_DATA "LenMus\Phonascus\${APP_VERSION}"

;variable to contain the installation directory
  !define INSTDIR ""

;support for Modern UI
  !include "MUI.nsh"

;support for GetParent
  !include "FileFunc.nsh"
  !insertmacro un.GetParent 


;configuration settings for the MUI interface: options, icons, bitmaps, colors, etc.
  ;show a warning when the user cancels the install
    !define MUI_ABORTWARNING
  ;icons to associate to the resulting installer and uninstaller
    !define MUI_ICON "win-install.ico"

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "BannerTop.bmp"
!define MUI_WELCOMEFINISHPAGE_BITMAP "BannerLeft.bmp"
!define MUI_COMPONENTSPAGE_SMALLDESC        ;descriptions at bottom


;MUI: install pages to show
  ;welcome page
    !insertmacro MUI_PAGE_WELCOME

  ;instalation page
    !insertmacro MUI_PAGE_INSTFILES

  ;finish page
    !insertmacro MUI_PAGE_FINISH


;language selection dialog settings

  ;language to install
  !insertmacro MUI_LANGUAGE ${LANG_NAME}

  ;reserve files for languages
    ;These files should be inserted before other files in the data block
    ;Keep these lines before any File command
    !insertmacro MUI_RESERVEFILE_LANGDLL

;variable to retry installation when error found
  var "STEP"

;instruct installer to show install log to the user
  ShowInstDetails show



;---------------------------------------------------------------------------------------------------
; ADD_LANG
;languaje files to support different languages during installation and for messages
;---------------------------------------------------------------------------------------------------
  !addincludedir ".\locale"
  !include "eu.nsh"
  !include "nl.nsh"
  !include "en.nsh"
  !include "fr.nsh"
  !include "gl_ES.nsh"
  ;!include "el_GR.nsh"
  !include "it.nsh"
  !include "es.nsh"
  !include "tr.nsh"



; *********************************************************************
; Install Sections
; *********************************************************************


Function .onInit
  ;first of all, show installer language selection page if language not set
  ;-----------------------------------------------------------------------------------
    !insertmacro MUI_LANGDLL_DISPLAY

  ;get path to lenmus root folder
  ;-----------------------------------------------------------------------------------
    ReadRegStr $0 HKCU "Software\${REGISTRY_DATA}" "InstallRoot"
    StrCmp $0 "" 0 NoAbort
        MessageBox MB_OK $(ERROR_FindingInstall)
        Abort ; causes installer to quit.
    NoAbort:
        StrCpy "$INSTDIR" $0
    
 FunctionEnd

;---------------------------------------------------------------------------------------------------
;Install all the mandatory components
Section  "-" "MainSection"

  ; vars initialization
  StrCpy "$STEP" "Nil" 


  ;install language files
  ;-----------------------------------------------------------------------------------
  CopyFiles:
     ClearErrors
     SetOverWrite try
     SetOutPath "$INSTDIR\docs"
     File ".\locale\license_${LANG_CODE}.txt"
     
     SetOutPath "$INSTDIR\locale\${LANG_CODE}\books"
     File "..\..\locale\${LANG_CODE}\books\*.lmb"

     SetOutPath "$INSTDIR\locale\${LANG_CODE}\images"
     File "..\..\locale\${LANG_CODE}\images\*.*"

     SetOutPath "$INSTDIR\locale\${LANG_CODE}"
     File "..\..\locale\${LANG_CODE}\*.mo"
     File "..\..\locale\${LANG_CODE}\*.htm"
     File "..\..\locale\${LANG_CODE}\help.htb"
     File "..\..\locale\common\singledoc.css"

      IfErrors +1 EndCopyFiles
        StrCmp $STEP "ErrorCopyingFiles" "Error_CopyFiles"
        StrCpy "$STEP" "ErrorCopyingFiles" 
      Goto CopyFiles
        Error_CopyFiles:
          MessageBox MB_RETRYCANCEL|MB_ICONEXCLAMATION $(ERROR_CopyFiles) IDRETRY CopyFiles
          MessageBox MB_YESNO|MB_ICONQUESTION $(MSG_CONTINUE) IDYES +2
      Abort "$(MSG_ABORT)"
     EndCopyFiles:


SectionEnd


;End of lenmus language files install script
