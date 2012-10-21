; File 'el_GR.nsh'
;========================================================================================
; Greek language file for lenmus installer/uninstaller
;---------------------------------------------------------------------------------------------------------

LicenseLangString license ${LANG_Greek} ".\locale\license_el_GR.txt"

;strings to customize MUI pages
LangString MSG_OptionalComponents   ${LANG_Greek} "Please choose the optional components you would like to install"


;section titles, what user sees to select components for installation
LangString TITLE_CreateIcon     ${LANG_Greek} "Shorcut on desktop"
LangString TITLE_Scores         ${LANG_Greek} "Examples of music scores"
LangString TITLE_RegKeys        ${LANG_Greek} "Add to Control Panel (to 'Add/Remove Programs')"

;descriptions for the sections. Displayed to user when mouse hovers over a section
LangString DESC_CreateIcon      ${LANG_Greek} "Create a shorcut icon on the desktop"
LangString DESC_Scores          ${LANG_Greek} "Create a folder containing some examples of music scores"
LangString DESC_RegKeys         ${LANG_Greek} "Create keys in Windows Registry to add LenMus to 'Add/Remove Programs' in the Control Panel."


;error messages and other texts
LangString ERROR_CopyFiles          ${LANG_Greek} "Error copying program files"
LangString ERROR_InstallFonts       ${LANG_Greek} "Error installing font"
LangString ERROR_CreateIcon         ${LANG_Greek} "Error creating shortcut on desktop"
LangString ERROR_CopyScores         ${LANG_Greek} "Error copying examples of music scores"
LangString MSG_CONTINUE             ${LANG_Greek} "Would you like to continue the installation?"
LangString MSG_ABORT                ${LANG_Greek} "Installation canceled"


;start menu & desktop descriptions
LangString SM_PRODUCT_GROUP             ${LANG_Greek} "${APP_NAME}"
LangString SHORTCUT_NAME_EXEC           ${LANG_Greek} "Run ${APP_NAME}"
LangString SHORTCUT_NAME_UNINSTALL      ${LANG_Greek} "Uninstall ${APP_NAME}"

;other strings
LangString OTHER_LangName             ${LANG_Greek} "el_GR"

;End Greek language file
;=========================================================================================
