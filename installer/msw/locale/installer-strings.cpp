//=======================================================================================
// This file contains all translatable strings for the Windows installer, so that they
// can be, automatically, added to lenmus_xx.pot file
//=======================================================================================
wxString sTxt;

//strings to customize MUI pages and message boxes
/* MSG_OptionalComponents */    sTxt = _("Please choose the optional components you would like to install");
/* MSG_CONTINUE */              sTxt = _("Would you like to continue the installation?");
/* MSG_ABORT */                 sTxt = _("Installation canceled");
/* MSG_Running */               sTxt = _("The installer is already running.");

//section titles, what user sees to select components for installation
/* TITLE_CreateIcon */  sTxt = _("Shorcut on desktop");
/* TITLE_Scores */      sTxt = _("Examples of music scores");

//descriptions for the sections. Displayed to user when mouse hovers over a section
/* DESC_CreateIcon */   sTxt = _("Create a shorcut icon on the desktop");
/* DESC_Scores */       sTxt = _("Create a folder containing some examples of music scores");

//error messages
/* ERROR_CopyFiles */       sTxt = _("Error copying program files");
/* ERROR_CreateIcon */      sTxt = _("Error creating shortcut on desktop");
/* ERROR_CopyScores */      sTxt = _("Error copying examples of music scores");

//start menu & desktop descriptions
/* SHORTCUT_NAME_EXEC */        sTxt = _("Run LenMus");
/* SHORTCUT_NAME_UNINSTALL */   sTxt = _("Uninstall LenMus");


//Obsolete
// TITLE_RegKeys        sTxt = _("Add to Control Panel (to 'Add/Remove Programs')");
// DESC_RegKeys         sTxt = _("Create keys in Windows Registry to add LenMus to 'Add/Remove Programs' in the Control Panel.");
// ERROR_InstallFonts   sTxt = _("Error installing font");
