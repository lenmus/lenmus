; File 'en.nsh'
;========================================================================================
; English language file for lenmus installer/uninstaller
;---------------------------------------------------------------------------------------------------------

LicenseLangString license ${LANG_English} "${LENMUS_NSIS_LOCALE}\license_en.txt"

;strings to customize MUI pages
LangString MSG_OptionalComponents   ${LANG_English} "Please choose the optional components you would like to install"
LangString MSG_CONTINUE             ${LANG_English} "Would you like to continue the installation?"
LangString MSG_ABORT                ${LANG_English} "Installation canceled"
LangString MSG_Running              ${LANG_English} "Another copy of this installer is already running."


;section titles, what user sees to select components for installation
LangString TITLE_CreateIcon     ${LANG_English} "Shorcut on desktop"
LangString TITLE_Scores         ${LANG_English} "Examples of music scores"

;descriptions for the sections. Displayed to user when mouse hovers over a section
LangString DESC_CreateIcon      ${LANG_English} "Create a shorcut icon on the desktop"
LangString DESC_Scores          ${LANG_English} "Create a folder containing some examples of music scores"


;error messages and other texts
LangString ERROR_CopyFiles          ${LANG_English} "Error copying program files"
LangString ERROR_CreateIcon         ${LANG_English} "Error creating shortcut on desktop"
LangString ERROR_CopyScores         ${LANG_English} "Error copying examples of music scores"


;start menu & desktop descriptions
LangString SM_PRODUCT_GROUP             ${LANG_English} "${APP_NAME}"
LangString SHORTCUT_NAME_EXEC           ${LANG_English} "Run ${APP_NAME}"
LangString SHORTCUT_NAME_UNINSTALL      ${LANG_English} "Uninstall ${APP_NAME}"

;other strings
LangString OTHER_LangName             ${LANG_English} "en"

;End English language file
;=========================================================================================
