; File 'el_GR.nsh'
;========================================================================================
; Greek language file for lenmus installer/uninstaller
;---------------------------------------------------------------------------------------------------------

LicenseLangString license ${LANG_Greek} "${LENMUS_NSIS_LOCALE}\license_el.txt"

;strings to customize MUI pages
LangString MSG_OptionalComponents   ${LANG_Greek} "Please choose the optional components you would like to install"
LangString MSG_CONTINUE             ${LANG_Greek} "Would you like to continue the installation?"
LangString MSG_ABORT                ${LANG_Greek} "Installation canceled"
LangString MSG_Running              ${LANG_Greek} "Another copy of this installer is already running."


;section titles, what user sees to select components for installation
LangString TITLE_CreateIcon     ${LANG_Greek} "Shorcut on desktop"
LangString TITLE_Scores         ${LANG_Greek} "Examples of music scores"

;descriptions for the sections. Displayed to user when mouse hovers over a section
LangString DESC_CreateIcon      ${LANG_Greek} "Create a shorcut icon on the desktop"
LangString DESC_Scores          ${LANG_Greek} "Create a folder containing some examples of music scores"


;error messages and other texts
LangString ERROR_CopyFiles          ${LANG_Greek} "Error copying program files"
LangString ERROR_CreateIcon         ${LANG_Greek} "Error creating shortcut on desktop"
LangString ERROR_CopyScores         ${LANG_Greek} "Error copying examples of music scores"


;start menu & desktop descriptions
LangString SM_PRODUCT_GROUP             ${LANG_Greek} "${APP_NAME}"
LangString SHORTCUT_NAME_EXEC           ${LANG_Greek} "Run ${APP_NAME}"
LangString SHORTCUT_NAME_UNINSTALL      ${LANG_Greek} "Uninstall ${APP_NAME}"

;other strings
LangString OTHER_LangName             ${LANG_Greek} "el"

;End Greek language file
;=========================================================================================
