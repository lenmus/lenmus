// RCS-ID: $Id: AboutDialog.cpp,v 1.3 2006/02/23 19:15:54 cecilios Exp $
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
/*! @file AboutDialog.cpp
    @brief Implementation file for class lmAboutDialog
    @ingroup app_gui
*/
#ifdef __GNUG__
// #pragma implementation
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/dialog.h>
#include <wx/html/htmlwin.h>
#include <wx/button.h>
#include <wx/dcclient.h>
#include <wx/sizer.h>
#include <wx/statbmp.h>
#include <wx/intl.h>

#include "TheApp.h"
#include "AboutDialog.h"
#include "global.h"

//to determine debug/release version
extern bool g_fReleaseVersion;        // to enable/disable debug features
extern bool g_fReleaseBehaviour;    // This flag is only used to force release behaviour when
                                    // in debug mode, and only for some functions (the ones using this flag)

//to get wxMidi version
#include "wxMidi.h"

BEGIN_EVENT_TABLE(lmAboutDialog, wxDialog)
   EVT_BUTTON(wxID_OK, lmAboutDialog::OnOK)
END_EVENT_TABLE()

IMPLEMENT_CLASS(lmAboutDialog, wxDialog)

lmAboutDialog::lmAboutDialog(wxWindow * parent)
:  wxDialog(parent, -1, _("About LenMus..."),
         wxDefaultPosition, wxSize(600, 430), wxDEFAULT_DIALOG_STYLE)
{
    wxString versionStr = LM_VERSION_STR;

    // locale info
    wxString sLocale = wxGetApp().GetLocaleName();
    wxString sSysname = wxGetApp().GetLocaleSysName();
    wxString sCanname = wxGetApp().GetLanguageCanonicalName();

    wxString sLocaleInfo;
    sLocaleInfo.Printf( _("Language: %s\nSystem locale name: %s\nCanonical locale name: %s\n"),
        sLocale.c_str(), sSysname.c_str(), sCanname.c_str() );

        //
        // Build information
        //

    wxString informationStr;

    // Build date
    informationStr += _("Program build date:");
    informationStr += wxT(" ") __TDATE__ wxT("<br>\n");

    // wxWindows version:
    informationStr += wxVERSION_STRING;
    informationStr += wxT("<br>\n");

    // wxMidi version
    informationStr += _("wxMidi Version ");
    informationStr += wxMIDI_VERSION;
    informationStr += wxT("<br>\n");

    // Locale info
    informationStr += sLocaleInfo;
    informationStr += wxT("<br>\n");


    wxString par1Str = wxGetTranslation(
        wxT("LenMus is a free program written by a team of volunteer developers \
            around the world (well this is a hope, for now it is developed only by me). ")
        _T("LenMus is available for Windows but soon it will be also available ")
        _T("for Mac OS X, Linux, and other Unix-like operating systems.")
    );

    wxString par2Str;
    if (g_fReleaseVersion)    {
        par2Str = wxGetTranslation(
            _T("This is a stable, completed release of LenMus.  However, if ")
            _T("you find a bug or have a suggestion, please contact us.  "));
    }
    else if (g_fReleaseBehaviour) {
         par2Str = wxGetTranslation(
            _T("This is a debug version of the program but some debug features (i.e. the ")
            _T("Debug menu item) are disabled and release version behaviour is enforced. ")
            _T("This is so to facilitate debugging. As this is a beta version it may contain ")
            _T("bugs and unfinished features.  "));
    }
    else {
         par2Str = wxGetTranslation(
            _T("This is a debug development version of the program.  It may contain ")
            _T("bugs and unfinished features.  "));
    }
    par2Str += wxGetTranslation(
                _T("We depend on feedback from you in order to continue to improve ")
                _T("LenMus. So please visit our website and give us your bug reports  ")
                _T("and feature requests."));

    //
    // Credits
    //

    wxString sCredits = wxGetTranslation(
        _T("<p>Some icons are original artwork for LenMus project; all these original icons are released ")
        _T("under GNU GPL licence and under Creative Commons Attribution-ShareAlike ")
        _T("license, you can choose the one you prefer for the intended usage. The other ")
        _T("icons are taken from different sources:<br /><br /></p>")
        _T("<ul>")
        _T("<li>Taken or derived from those available in the Ximian collection ")
        _T("(http://www.novell.com/coolsolutions/feature/1637.html). ")
        _T("Novell, who retains the copyright, has released these icons under the LGPL license. ")
        _T("This means that you can use the icons in your ")
        _T("programs free of charge. If you want to fork the icons into your own icon collection, you need ")
        _T("to retain the license and the original copyrights, but from there you're free to do what ")
        _T("you want.<br /></li> ")
        _T("<li>Taken or derived from the GNOME project (http://art.gnome.org/themes/icon/1150), ")
        _T("Tango theme. These icons are released under the Creative Commons Attribution-ShareAlike ")
        _T("license.<br /></li> ")
        _T("<li>These icons are taken or derived from KDE project ")
        _T("(http://kde.openoffice.org/servlets/ProjectDocumentList?folderID=314&amp;expandFolder=314). ")
        _T("These icons are released under the GNU GPL license.<br /></li> ")
        _T("<li>Taken or derived from eMule project (http://sourceforge.net/projects/emule) ")
        _T("icons. eMule is a GNU GPL project so I assume that its icons are also available under GNU GPL.<br /></li> ")
        _T("<li>Taken or derived from John Zaitseff's icons ")
        _T("(http://www.zap.org.au/documents/icons/dirtree-icons/sample.html). According to what ")
        _T("is said there, these icons are released under the ")
        _T("terms of the GNU General Public License.<br /><br /></li>")
        _T("</ul>")
        _T("To LenMus developers knowledge, all used icons are freely usable either ")
        _T("under GNU GPL, ")
        _T("LGPL licence, or Creative Commons. If this were not the case, please, let us ")
        _T("know to stop using them. ")
        _T("See file \"icons.htm\" in folder \"res/icons\" for more details.</p>"));


    wxString managersCredits, developersCredits, translatorsCredits;
    wxString artDesignersCredits, musicalDirectionCredits;
    managersCredits += _T("<p><center><b>");
    //managersCredits += _("Project management");
    managersCredits += _("Management and developent");
    managersCredits += _T("<br>Cecilio Salmer&oacute;n");
    managersCredits += _T("</b></center>");

    developersCredits += _T("<p><center><b>");
    developersCredits += _("Developers</b>");
    developersCredits += _T("<br>Cecilio Salmer&oacute;n");
    developersCredits += _T("</center>");

    artDesignersCredits += _T("<p><center><b>");
    artDesignersCredits += _("Art desings");
    artDesignersCredits += _T("</b></center>");

    musicalDirectionCredits += _T("<p><center><b>");
    musicalDirectionCredits += _("Musical direction");
    musicalDirectionCredits += _T("</b></center>");

    translatorsCredits += _T("<p><center><b>");
    translatorsCredits += _("Translators</b>");
    translatorsCredits += _T("<br>English translation by Cecilio Salmer&oacute;n.");
    translatorsCredits += _T("</center>");

    wxString localeStr = wxLocale::GetSystemEncodingName();

    // Licence

    wxString sLicence =
        _T("LenMus ") + versionStr +
        _(" Copyright &copy; 2002-2006 Cecilio Salmer&oacute;n.") +
_("<p>This program is free software; you can redistribute it and/or modify it \
under the terms of the GNU General Public License as published by the Free \
Software Foundation; either version 2 of the License, or (at your option) \
any later version.") +
        wxGetTranslation(
            _T("<p>This program is distributed in the hope that it will be useful, but ")
            _T("WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY ")
            _T("or FITNESS FOR A PARTICULAR PURPOSE.  For more details see the GNU General ")
            _T("Public License at http://www.gnu.org/licenses/licenses.html"));

    wxString sContent =
      _T("<html>")
      _T("<head><META http-equiv=\"Content-Type\" content=\"text/html; charset=")
      + localeStr +
      _T("\"></head>")
      _T("<body bgcolor=\"#ffffff\">")
      _T("<font size=1>") +
      par1Str +
      _T("<p>") +
      par2Str +
      _T("<p><center>")
      _T("http://www.lenmus.org/")
      _T("</center><p>")
      _T("<center><b>") + _("Licence") + _T("</b></center>") +
      _T("<p><br>") +
      sLicence +
      _T("<p>")
      _T("<center><b>") + _("Release information") + _T("</b></center>")
      _T("<p><br>")
      + informationStr +
      _T("<p>&nbsp;</p>") +
      _T("<center><b>") + _("Art Design Credits") + _T("</b></center>")
      + sCredits +
//      + managersCredits
//      + developersCredits +
 //     + musicalDirectionCredits
 //     + artDesignersCredits
 //     + translatorsCredits +
      _T("<p>&nbsp;</p><p>&nbsp;</p><center>") +
      _("Copyright &copy; 2002-2006 Cecilio Salmer&oacute;n") +
      _T("</center></body>")
      _T("</html>");

    wxString sTitle =
      _T("<html>")
      _T("<head><META http-equiv=\"Content-Type\" content=\"text/html; charset=")
      + localeStr +
      _T("\"></head>")
      _T("<body bgcolor=\"#ffffff\">")
      _T("<font size=1>")
      _T("<center>")
      _T("<h3>LenMus ") + versionStr + _T("</h3><b>") +
      _("A free program for music language learning") +
      _T("</b></center></body>")
      _T("</html>");

    Centre();
    this->SetBackgroundColour(wxColour(255, 255, 255));

    wxBitmap* pLogo = new wxBITMAP(logo50x67_bmp);
    wxStaticBitmap* pIcon = new wxStaticBitmap(this, -1, *pLogo, wxPoint(15, 10));
    delete pLogo;

    wxHtmlWindow *pTitle = new wxHtmlWindow(this, -1,
                                         wxPoint(110, 10),
                                         wxSize(490, 80),
                                         wxHW_SCROLLBAR_NEVER | wxNO_BORDER );

    pTitle->SetPage(sTitle);

    wxHtmlWindow *html = new wxHtmlWindow(this, -1,
                                         wxPoint(20, 90),
                                         wxSize(560, 260),
                                         wxHW_SCROLLBAR_AUTO | wxSIMPLE_BORDER );
    html->SetPage(sContent);

    wxButton *pOK = new wxButton(this, wxID_OK, _("Accept"), wxPoint(250, 370),
                                wxSize(100, 24));
    pOK->SetDefault();
    pOK->SetFocus();

}

lmAboutDialog::~lmAboutDialog()
{
}

void lmAboutDialog::OnOK(wxCommandEvent& WXUNUSED(event))
{
   EndModal(wxID_OK);
}
