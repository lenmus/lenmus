; File 'fr.nsh'
;========================================================================================
; French language file for lenmus installer/uninstaller
;---------------------------------------------------------------------------------------------------------

LicenseLangString license ${LANG_French} ".\locale\license_fr.txt"

;strings to customize MUI pages
LangString MSG_OptionalComponents   ${LANG_French} "Please choose the optional components you would like to install"


;section titles, what user sees to select components for installation
LangString TITLE_CreateIcon     ${LANG_French} "Raccourci sur l'écran"
LangString TITLE_Scores         ${LANG_French} "Exemples de partitions"
LangString TITLE_RegKeys        ${LANG_French} "Ajouter au panneau de control (pour 'ajouter/enlever Logiciels')"

;descriptions for the sections. Displayed to user when mouse hovers over a section
LangString DESC_CreateIcon      ${LANG_French} "Créer un raccourci icone sur l'écran"
LangString DESC_Scores          ${LANG_French} "Créer un dosier contenant quelques exemples de partitions"
LangString DESC_RegKeys         ${LANG_French} "Créer clés dans le régistre de Windows pour ajouter LenMus au 'Ajouter/enlever logiciels' sur le panneau de control."


;error messages and other texts
LangString ERROR_CopyFiles          ${LANG_French} "Erreur faisant copies des fichiers du logiciel"
LangString ERROR_InstallFonts       ${LANG_French} "Erreur pendant l'installation de fonts"
LangString ERROR_CreateIcon         ${LANG_French} "Erreur créant raccourci icone sur l'écran"
LangString ERROR_CopyScores         ${LANG_French} "Erreur copiant exemples de partitions"
LangString MSG_CONTINUE             ${LANG_French} "Voudrais-vous continuer avec l'installation?"
LangString MSG_ABORT                ${LANG_French} "Installation arretée"


;start menu & desktop descriptions
LangString SM_PRODUCT_GROUP             ${LANG_French} "${APP_NAME}"
LangString SHORTCUT_NAME_EXEC           ${LANG_French} "Tourner ${APP_NAME}"
LangString SHORTCUT_NAME_UNINSTALL      ${LANG_French} "Enlever l'installation ${APP_NAME}"

;other strings
LangString OTHER_LangName             ${LANG_French} "fr"

;End French language file
;=========================================================================================
