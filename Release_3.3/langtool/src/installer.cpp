//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
//
//    This program is free software; you can redistribute it and/or modify it under the
//    terms of the GNU General Public License as published by the Free Software Foundation;
//    either version 2 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this
//    program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street,
//    Fifth Floor, Boston, MA  02110-1301, USA.
//
//    For any comment, suggestion or feature request, please contact the manager of
//    the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------
#ifdef __GNUG__
#pragma implementation installer.h
#endif

// for (compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "installer.h"

lmInstaller::lmInstaller()
{
}

lmInstaller::~lmInstaller()
{
}

wxString lmInstaller::GetInstallerStrings(wxString sLangCode, wxString sLangName)
{
    wxString sNil = _T("");
    wxString sLang = sNil + _T("${LANG_") + sLangName + _T("} \"");

    wxString sStrings = sNil +
    _T("; File '") + sLangCode + _T(".nsh'\n")
    _T(";========================================================================================\n")
    _T("; ") + sLangName + _T(" language file for lenmus installer/uninstaller\n")
    _T(";---------------------------------------------------------------------------------------------------------\n")
    _T("\n")
    _T("LicenseLangString license ") + sLang + _T(".\\locale\\license_") + sLangCode + _T(".txt\"\n")
    _T("\n")
    _T(";strings to customize MUI pages\n")
    _T("LangString MSG_OptionalComponents   ") + sLang + _("Please, choose the optional components you would like to install") + _T("\"\n")
    _T("\n")
    _T("\n")
    _T(";section titles, what user sees to select components for installation\n")
    _T("LangString TITLE_CreateIcon     ") + sLang + _("Shorcut on desktop") + _T("\"\n")
    _T("LangString TITLE_Scores         ") + sLang + _("Examples of music scores") + _T("\"\n")
    _T("LangString TITLE_RegKeys        ") + sLang + _("Add to Control Panel (to 'Add/Remove Programs')") + _T("\"\n")
    _T("\n")
    _T(";descriptions for the sections. Displayed to user when mouse hovers over a section\n")
    _T("LangString DESC_CreateIcon      ") + sLang + _("Create a shorcut icon on the desktop") + _T("\"\n")
    _T("LangString DESC_Scores          ") + sLang + _("Create a folder containing some examples of music scores") + _T("\"\n")
    _T("LangString DESC_RegKeys         ") + sLang + _("Create keys in Windows Registry to add LenMus to 'Add/Remove Programs' in the Control Panel.") + _T("\"\n")
    _T("\n")
    _T("\n")
    _T(";error messages and other texts\n")
    _T("LangString ERROR_CopyFiles          ") + sLang + _("Error copying program files") + _T("\"\n")
    _T("LangString ERROR_InstallFonts       ") + sLang + _("Error installing font") + _T("\"\n")
    _T("LangString ERROR_CreateIcon         ") + sLang + _("Error creating shortcut on desktop") + _T("\"\n")
    _T("LangString ERROR_CopyScores         ") + sLang + _("Error copying examples of music scores") + _T("\"\n")
    _T("LangString MSG_CONTINUE             ") + sLang + _("Would you like to continue the installation?") + _T("\"\n")
    _T("LangString MSG_ABORT                ") + sLang + _("Installation canceled") + _T("\"\n")
    _T("\n")
    _T("\n")
    _T(";start menu & desktop descriptions\n")
    _T("LangString SM_PRODUCT_GROUP             ") + sLang + _T("${APP_NAME}\"\n")
    _T("LangString SHORTCUT_NAME_EXEC           ") + sLang + _("Run") + _T(" ${APP_NAME}") + _T("\"\n")
    _T("LangString SHORTCUT_NAME_UNINSTALL      ") + sLang + _("Uninstall") + _T(" ${APP_NAME}") + _T("\"\n")
    _T("\n")
    _T(";other strings\n")
    _T("LangString OTHER_LangName             ") + sLang + sLangCode + _T("\"\n")
    _T("\n")
    _T(";End ") + sLangName + _T(" language file\n")
    _T(";=========================================================================================\n");

    return sStrings;

}

