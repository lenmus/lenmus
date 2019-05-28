; File 'tr.nsh'
;========================================================================================
; Turkish language file for lenmus installer/uninstaller
;---------------------------------------------------------------------------------------------------------

LicenseLangString license ${LANG_Turkish} "${LENMUS_NSIS_LOCALE}\license_tr.txt"

;strings to customize MUI pages
LangString MSG_OptionalComponents   ${LANG_Turkish} "Lütfen, kurmak istediğiniz isteğe bağlı parçaları seç"
LangString MSG_Running              ${LANG_Turkish} "Another copy of this installer is already running"


;section titles, what user sees to select components for installation
LangString TITLE_CreateIcon     ${LANG_Turkish} "Masaüstünde kısa kesinti"
LangString TITLE_Scores         ${LANG_Turkish} "Müzik sayılarınının örnekleri"
LangString TITLE_RegKeys        ${LANG_Turkish} "Kontrol paneli ekle(programlara ekle/sil"

;descriptions for the sections. Displayed to user when mouse hovers over a section
LangString DESC_CreateIcon      ${LANG_Turkish} "Masaüstünde kısayol ikonları yarat"
LangString DESC_Scores          ${LANG_Turkish} "Müzik sayılarının bazı örneklerini  kapsayan dosyayı  yarat"
LangString DESC_RegKeys         ${LANG_Turkish} "LenMus'u eklemek için windows kayıtında ana resimlerleri yarat, kontrol panelinde  'programa ekle/sil' "


;error messages and other texts
LangString ERROR_CopyFiles          ${LANG_Turkish} "Program dosyaları kopyalama hatası"
LangString ERROR_InstallFonts       ${LANG_Turkish} "Yazıtipi kurma hatası"
LangString ERROR_CreateIcon         ${LANG_Turkish} "Masaüstünde kısa kesintili hatalar yaratır"
LangString ERROR_CopyScores         ${LANG_Turkish} "Müzik sayılarının kopyalanmasındaki hataları örnekler"
LangString MSG_CONTINUE             ${LANG_Turkish} "Kurmaya devam etmek istiyormusun?"
LangString MSG_ABORT                ${LANG_Turkish} "Kanalı kur"


;start menu & desktop descriptions
LangString SM_PRODUCT_GROUP             ${LANG_Turkish} "${APP_NAME}"
LangString SHORTCUT_NAME_EXEC           ${LANG_Turkish} "Çalıştır ${APP_NAME}"
LangString SHORTCUT_NAME_UNINSTALL      ${LANG_Turkish} "Kaldır ${APP_NAME}"

;other strings
LangString OTHER_LangName             ${LANG_Turkish} "tr"

;End Turkish language file
;=========================================================================================
