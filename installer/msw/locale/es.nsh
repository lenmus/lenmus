; File 'es.nsh'
;========================================================================================
; Spanish language file for lenmus installer/uninstaller
;---------------------------------------------------------------------------------------------------------

LicenseLangString license ${LANG_Spanish} ".\locale\license_es.txt"

;strings to customize MUI pages
LangString MSG_OptionalComponents   ${LANG_Spanish} "Please choose the optional components you would like to install"


;section titles, what user sees to select components for installation
LangString TITLE_CreateIcon     ${LANG_Spanish} "Acceso directo en el escritorio"
LangString TITLE_Scores         ${LANG_Spanish} "Ejemplos de partituras"
LangString TITLE_RegKeys        ${LANG_Spanish} "Incluir en panel de control ('Agregar/quitar software')"

;descriptions for the sections. Displayed to user when mouse hovers over a section
LangString DESC_CreateIcon      ${LANG_Spanish} "Crear un acceso directo en el escritorio"
LangString DESC_Scores          ${LANG_Spanish} "Crear una carpeta conteniendo ejemplos de partituras"
LangString DESC_RegKeys         ${LANG_Spanish} "Crea claves en el Registro de Windows para que LenMus aparezca en 'Agregar/quitar programas'"


;error messages and other texts
LangString ERROR_CopyFiles          ${LANG_Spanish} "Error durante la copia de los archivos"
LangString ERROR_InstallFonts       ${LANG_Spanish} "Error instalando el font"
LangString ERROR_CreateIcon         ${LANG_Spanish} "Error creando acceso directo en el escritorio"
LangString ERROR_CopyScores         ${LANG_Spanish} "Error al copiar las partituras de ejemplo "
LangString MSG_CONTINUE             ${LANG_Spanish} "¿Quiere continuar instalando?"
LangString MSG_ABORT                ${LANG_Spanish} "Instalación cancelada"


;start menu & desktop descriptions
LangString SM_PRODUCT_GROUP             ${LANG_Spanish} "${APP_NAME}"
LangString SHORTCUT_NAME_EXEC           ${LANG_Spanish} "Ejecutar ${APP_NAME}"
LangString SHORTCUT_NAME_UNINSTALL      ${LANG_Spanish} "Desinstalar ${APP_NAME}"

;other strings
LangString OTHER_LangName             ${LANG_Spanish} "es"

;End Spanish language file
;=========================================================================================
