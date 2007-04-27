; File 'nl.nsh'
;========================================================================================
; Dutch language file for lenmus installer/uninstaller
;---------------------------------------------------------------------------------------------------------

LicenseLangString license ${LANG_Dutch} ".\locale\license_nl.txt"

;strings to customize MUI pages
LangString MSG_OptionalComponents   ${LANG_Dutch} "Toon de optionele componenten die u wenst te installeren, aub"


;section titles, what user sees to select components for installation
LangString TITLE_CreateIcon     ${LANG_Dutch} "Snelkoppeling op bureaublad"
LangString TITLE_Scores         ${LANG_Dutch} "Voorbeelden van partituren"
LangString TITLE_RegKeys        ${LANG_Dutch} "Toevoegen aan Controle Paneel (Programma's toevoegen/verwijderen)"

;descriptions for the sections. Displayed to user when mouse hovers over a section
LangString DESC_CreateIcon      ${LANG_Dutch} "Een snelkoppeling aanmaken op het bureaublad"
LangString DESC_Scores          ${LANG_Dutch} "Een folder aanmaken met enkele voorbeelden van partituren"
LangString DESC_RegKeys         ${LANG_Dutch} "Genereer sleutels in de Windows Registry om LenMus toe te voegen aan 'Programma's toevoegen/verwijderen' in het Controle Paneel."


;error messages and other texts
LangString ERROR_CopyFiles          ${LANG_Dutch} "Error copying program files"
LangString ERROR_InstallFonts       ${LANG_Dutch} "Fout bij het installeren van de font"
LangString ERROR_CreateIcon         ${LANG_Dutch} "Fout bij het aanmaken van een snelkoppeling op de desktop"
LangString ERROR_CopyScores         ${LANG_Dutch} "Fout bij het copieren van de partituurvoorbeelden"
LangString MSG_CONTINUE             ${LANG_Dutch} "Wilt u de installatie verder zetten?"
LangString MSG_ABORT                ${LANG_Dutch} "Installation canceled"


;start menu & desktop descriptions
LangString SM_PRODUCT_GROUP             ${LANG_Dutch} "${APP_NAME}"
LangString SHORTCUT_NAME_EXEC           ${LANG_Dutch} "Run ${APP_NAME}"
LangString SHORTCUT_NAME_UNINSTALL      ${LANG_Dutch} "Installatie verwijderen ${APP_NAME}"

;other strings
LangString OTHER_LangName             ${LANG_Dutch} "nl"

;End Dutch language file
;=========================================================================================
