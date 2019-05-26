; File 'nl.nsh'
;========================================================================================
; Dutch language file for lenmus installer/uninstaller
;---------------------------------------------------------------------------------------------------------

LicenseLangString license ${LANG_Dutch} "${LENMUS_NSIS_LOCALE}\license_nl.txt"

;strings to customize MUI pages
LangString MSG_OptionalComponents   ${LANG_Dutch} "Kies aub de optionele componenten die u wenst te installeren"
LangString MSG_CONTINUE             ${LANG_Dutch} "Wilt u de installatie verderzetten?"
LangString MSG_ABORT                ${LANG_Dutch} "Installatie geannulleerd"
LangString MSG_Running              ${LANG_Dutch} "Another copy of this installer is already running."


;section titles, what user sees to select components for installation
LangString TITLE_CreateIcon     ${LANG_Dutch} "Snelkoppeling op bureaublad"
LangString TITLE_Scores         ${LANG_Dutch} "Voorbeelden van partituren"

;descriptions for the sections. Displayed to user when mouse hovers over a section
LangString DESC_CreateIcon      ${LANG_Dutch} "Een snelkoppeling aanmaken op het bureaublad"
LangString DESC_Scores          ${LANG_Dutch} "Een folder aanmaken met enkele voorbeelden van partituren"


;error messages and other texts
LangString ERROR_CopyFiles          ${LANG_Dutch} "Fout bij het copiëren van de programmabestanden"
LangString ERROR_CreateIcon         ${LANG_Dutch} "Fout bij het aanmaken van een snelkoppeling op het bureaublad"
LangString ERROR_CopyScores         ${LANG_Dutch} "Fout bij het copieren van de partituurvoorbeelden"


;start menu & desktop descriptions
LangString SM_PRODUCT_GROUP             ${LANG_Dutch} "${APP_NAME}"
LangString SHORTCUT_NAME_EXEC           ${LANG_Dutch} "Run ${APP_NAME}"
LangString SHORTCUT_NAME_UNINSTALL      ${LANG_Dutch} "Installatie verwijderen ${APP_NAME}"

;other strings
LangString OTHER_LangName             ${LANG_Dutch} "nl"

;End Dutch language file
;=========================================================================================
