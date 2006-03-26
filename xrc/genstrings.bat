echo //OptionsDlg.xrc > xrc_strings.cpp
wxrc OptionsDlg.xrc -g >> xrc_strings.cpp

echo //DlgCfgEarIntervals.xrc >> xrc_strings.cpp
wxrc DlgCfgEarIntervals.xrc -g >> xrc_strings.cpp

echo //DlgCfgScoreReading.xrc >> xrc_strings.cpp
wxrc DlgCfgScoreReading.xrc -g >> xrc_strings.cpp

echo //DlgCfgTheoIntervals.xrc >> xrc_strings.cpp
wxrc DlgCfgTheoIntervals.xrc -g >> xrc_strings.cpp

echo //DlgPatternEditor.xrc >> xrc_strings.cpp
wxrc DlgPatternEditor.xrc -g >> xrc_strings.cpp

echo //LangOptionsPanel.xrc >> xrc_strings.cpp
wxrc LangOptionsPanel.xrc -g >> xrc_strings.cpp

echo //ToolbarsOptPanel.xrc >> xrc_strings.cpp
wxrc ToolbarsOptPanel.xrc -g >> xrc_strings.cpp
