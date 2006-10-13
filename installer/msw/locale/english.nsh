; File "english.nsh"
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
