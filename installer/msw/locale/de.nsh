; File 'de.nsh'
;========================================================================================
; German language file for lenmus installer/uninstaller
;---------------------------------------------------------------------------------------------------------

LicenseLangString license ${LANG_German} ".\locale\license_de.txt"

;strings to customize MUI pages
LangString MSG_OptionalComponents   ${LANG_German} "Please choose the optional components you would like to install"


;section titles, what user sees to select components for installation
LangString TITLE_CreateIcon     ${LANG_German} "Shorcut on desktop"
LangString TITLE_Scores         ${LANG_German} "Examples of music scores"
LangString TITLE_RegKeys        ${LANG_German} "Add to Control Panel (to 'Add/Remove Programs')"

;descriptions for the sections. Displayed to user when mouse hovers over a section
LangString DESC_CreateIcon      ${LANG_German} "Create a shorcut icon on the desktop"
LangString DESC_Scores          ${LANG_German} "Create a folder containing some examples of music scores"
LangString DESC_RegKeys         ${LANG_German} "Create keys in Windows Registry to add LenMus to 'Add/Remove Programs' in the Control Panel."


;error messages and other texts
LangString ERROR_CopyFiles          ${LANG_German} "Error copying program files"
LangString ERROR_InstallFonts       ${LANG_German} "Error installing font"
LangString ERROR_CreateIcon         ${LANG_German} "Error creating shortcut on desktop"
LangString ERROR_CopyScores         ${LANG_German} "Error copying examples of music scores"
LangString MSG_CONTINUE             ${LANG_German} "Would you like to continue the installation?"
LangString MSG_ABORT                ${LANG_German} "Installation canceled"


;start menu & desktop descriptions
LangString SM_PRODUCT_GROUP             ${LANG_German} "${APP_NAME}"
LangString SHORTCUT_NAME_EXEC           ${LANG_German} "Run ${APP_NAME}"
LangString SHORTCUT_NAME_UNINSTALL      ${LANG_German} "Uninstall ${APP_NAME}"

;other strings
LangString OTHER_LangName             ${LANG_German} "de"

;End German language file
;=========================================================================================
