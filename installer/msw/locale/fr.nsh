; File 'fr.nsh'
;========================================================================================
; French language file for lenmus installer/uninstaller
;---------------------------------------------------------------------------------------------------------

LicenseLangString license ${LANG_French} ".\locale\license_fr.txt"

;strings to customize MUI pages
LangString MSG_OptionalComponents   ${LANG_French} "S'il vous plaît, choisissez les composants optionels que vous voulez installer."


;section titles, what user sees to select components for installation
LangString TITLE_CreateIcon     ${LANG_French} "Raccourci sur le bureau"
LangString TITLE_Scores         ${LANG_French} "Exemples de partitions"
LangString TITLE_RegKeys        ${LANG_French} "Ajouter au panneau de configuration (dans 'Ajouter/Supprimer des Programmes')"

;descriptions for the sections. Displayed to user when mouse hovers over a section
LangString DESC_CreateIcon      ${LANG_French} "Créer un raccourci sur le bureau"
LangString DESC_Scores          ${LANG_French} "Créer un dossier contenant quelques exemples de partitions"
LangString DESC_RegKeys         ${LANG_French} "Créer des clés dans le régistre de Windows pour ajouter LenMus à la liste 'Ajouter/Supprimer des programmes' dans le panneau de configuration."


;error messages and other texts
LangString ERROR_CopyFiles          ${LANG_French} "Erreur durant la copie des fichiers du logiciel"
LangString ERROR_InstallFonts       ${LANG_French} "Erreur durant l'installation des polices"
LangString ERROR_CreateIcon         ${LANG_French} "Erreur durant la création du raccourci sur le bureau"
LangString ERROR_CopyScores         ${LANG_French} "Erreur durant la copie des exemples de partitions"
LangString MSG_CONTINUE             ${LANG_French} "Voulez-vous poursuivre l'installation ?"
LangString MSG_ABORT                ${LANG_French} "Installation annulée"


;start menu & desktop descriptions
LangString SM_PRODUCT_GROUP             ${LANG_French} "${APP_NAME}"
LangString SHORTCUT_NAME_EXEC           ${LANG_French} "Exécuter ${APP_NAME}"
LangString SHORTCUT_NAME_UNINSTALL      ${LANG_French} "Désinstaller ${APP_NAME}"

;other strings
LangString OTHER_LangName             ${LANG_French} "fr"

;End French language file
;=========================================================================================
