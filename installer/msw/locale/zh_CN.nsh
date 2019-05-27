; File 'zh_CN.nsh'
;========================================================================================
; Simplified Chinese  language file for lenmus installer/uninstaller
;---------------------------------------------------------------------------------------------------------

LicenseLangString license ${LANG_SimpChinese} "${LENMUS_NSIS_LOCALE}\license_en.txt"

;strings to customize MUI pages
LangString MSG_OptionalComponents   ${LANG_SimpChinese} "請選擇您要安裝的可選組件"
LangString MSG_CONTINUE             ${LANG_SimpChinese} "你希望继续安装吗？"
LangString MSG_ABORT                ${LANG_SimpChinese} "取消安装"
LangString MSG_Running              ${LANG_SimpChinese} "此安裝程序的另一個副本已在運行。"


;section titles, what user sees to select components for installation
LangString TITLE_CreateIcon     ${LANG_SimpChinese} "桌面上的快捷方式"
LangString TITLE_Scores         ${LANG_SimpChinese} "乐谱的例子"

;descriptions for the sections. Displayed to user when mouse hovers over a section
LangString DESC_CreateIcon      ${LANG_SimpChinese} "在桌面上創建快捷方式圖標"
LangString DESC_Scores          ${LANG_SimpChinese} "創建一個包含一些樂譜分數示例的文件夾"


;error messages and other texts
LangString ERROR_CopyFiles          ${LANG_SimpChinese} "程序文件复制失败"
LangString ERROR_CreateIcon         ${LANG_SimpChinese} "创建桌面快捷方式失败"
LangString ERROR_CopyScores         ${LANG_SimpChinese} "複製音樂樂譜的示例時出錯"


;start menu & desktop descriptions
LangString SM_PRODUCT_GROUP             ${LANG_SimpChinese} "${APP_NAME}"
LangString SHORTCUT_NAME_EXEC           ${LANG_SimpChinese} "運行 ${APP_NAME}"
LangString SHORTCUT_NAME_UNINSTALL      ${LANG_SimpChinese} "卸載 ${APP_NAME}"

;other strings
LangString OTHER_LangName             ${LANG_SimpChinese} "zn_CN"

;End Simplified Chinese language file
;=========================================================================================
