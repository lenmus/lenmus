; File 'fr.nsh'
;========================================================================================
; French language file for lenmus installer/uninstaller
;---------------------------------------------------------------------------------------------------------

LicenseLangString license ${LANG_French} "license_fr.txt"

;strings to customize MUI pages
LangString MSG_OptionalComponents   ${LANG_French} "Please, choose the optional components you would like to install"


;section titles, what user sees to select components for installation
LangString TITLE_CreateIcon     ${LANG_French} "Shorcut on desktop"
LangString TITLE_Scores         ${LANG_French} "Examples of music scores"
LangString TITLE_RegKeys        ${LANG_French} "Add to Control Panel (to 'Add/Remove Programs')"

;descriptions for the sections. Displayed to user when mouse hovers over a section
LangString DESC_CreateIcon      ${LANG_French} "Create a shorcut icon on the desktop"
LangString DESC_Scores          ${LANG_French} "Create a folder containing some examples of music scores"
LangString DESC_RegKeys         ${LANG_French} "Create keys in Windows Registry to add LenMus to 'Add/Remove Programs' in the Control Panel."


;error messages and other texts
LangString ERROR_CopyFiles          ${LANG_French} "Error copying program files"
LangString ERROR_InstallFonts       ${LANG_French} "Error installing font"
LangString ERROR_CreateIcon         ${LANG_French} "Error creating shortcut on desktop"
LangString ERROR_CopyScores         ${LANG_French} "Error copying examples of music scores"
LangString MSG_CONTINUE             ${LANG_French} "Would you like to continue the installation?"
LangString MSG_ABORT                ${LANG_French} "Installation canceled"


;start menu & desktop descriptions
LangString SM_PRODUCT_GROUP             ${LANG_French} "${APP_NAME}"
LangString SHORTCUT_NAME_EXEC           ${LANG_French} "Run ${APP_NAME}"
LangString SHORTCUT_NAME_UNINSTALL      ${LANG_French} "Uninstall ${APP_NAME}"

;End French language file
;=========================================================================================
