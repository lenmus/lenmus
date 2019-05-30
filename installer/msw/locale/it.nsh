; File 'it.nsh'
;========================================================================================
; Italian language file for lenmus installer/uninstaller
;---------------------------------------------------------------------------------------------------------

LicenseLangString license ${LANG_Italian} "${LENMUS_NSIS_LOCALE}\license_it.txt"

;strings to customize MUI pages
LangString MSG_OptionalComponents   ${LANG_Italian} "Si prega di scegliere i componenti opzionali da installare"
LangString MSG_CONTINUE             ${LANG_Italian} "Desideri continuare con l'installazione?"
LangString MSG_ABORT                ${LANG_Italian} "Installazione annullata"
LangString MSG_Running              ${LANG_Italian} "Another copy of this installer is already running."


;section titles, what user sees to select components for installation
LangString TITLE_CreateIcon     ${LANG_Italian} "Collegamento sul desktop"
LangString TITLE_Scores         ${LANG_Italian} "Esempi di partiture musicali"

;descriptions for the sections. Displayed to user when mouse hovers over a section
LangString DESC_CreateIcon      ${LANG_Italian} "Crea un'icona di collegamento sul desktop"
LangString DESC_Scores          ${LANG_Italian} "Crea una cartella contenente degli esempi di partiture musicali"


;error messages and other texts
LangString ERROR_CopyFiles          ${LANG_Italian} "Errore nella copia dei file del programma"
LangString ERROR_CreateIcon         ${LANG_Italian} "Errore nella creazione del collegamento sul desktop"
LangString ERROR_CopyScores         ${LANG_Italian} "Errore nel copiare esempi di partiture musicali"


;start menu & desktop descriptions
LangString SM_PRODUCT_GROUP             ${LANG_Italian} "${APP_NAME}"
LangString SHORTCUT_NAME_EXEC           ${LANG_Italian} "Avvia ${APP_NAME}"
LangString SHORTCUT_NAME_UNINSTALL      ${LANG_Italian} "Disinstalla ${APP_NAME}"

;other strings
LangString OTHER_LangName             ${LANG_Italian} "it"

;End Italian language file
;=========================================================================================
