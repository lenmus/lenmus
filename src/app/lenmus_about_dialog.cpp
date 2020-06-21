//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2010-2015 LenMus project
//
//    This program is free software; you can redistribute it and/or modify it under the
//    terms of the GNU General Public License as published by the Free Software Foundation,
//    either version 3 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this
//    program. If not, see <http://www.gnu.org/licenses/>.
//
//    For any comment, suggestion or feature request, please contact the manager of
//    the project at cecilios@users.sourceforge.net
//
//---------------------------------------------------------------------------------------

//lenmus
#include "lenmus_about_dialog.h"

#include "lenmus_string.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/window.h>
#include <wx/dialog.h>
#include <wx/button.h>
#include <wx/statbmp.h>
#include <wx/stattext.h>
#include <wx/artprov.h>
#include <wx/gdicmn.h>
#include <wx/wxsqlite3.h>               //to access wxSQLite3 DB

//lomse
#include <lomse_doorway.h>
#include <lomse_injectors.h>
using namespace lomse;

//other
#include <wxMidi.h>         //to get wxMidi version
using namespace std;


namespace lenmus
{

//IDs for controls
const int lmID_BTN_ACCEPT = wxNewId();
const int lmID_BTN_ART_CREDITS = wxNewId();
const int lmID_BTN_BUILD_INFO = wxNewId();
const int lmID_BTN_LICENCE = wxNewId();
const int lmID_BTN_PURPOSE = wxNewId();
const int lmID_BTN_SOFTWARE_CREDITS = wxNewId();
const int lmID_BTN_DEVELOPERS = wxNewId();


wxBEGIN_EVENT_TABLE(AboutDialog, wxDialog)
    EVT_BUTTON(lmID_BTN_ACCEPT, AboutDialog::OnAccept )
    EVT_BUTTON(lmID_BTN_PURPOSE, AboutDialog::OnPurpose )
    EVT_BUTTON(lmID_BTN_LICENCE, AboutDialog::OnLicense )
    EVT_BUTTON(lmID_BTN_DEVELOPERS, AboutDialog::OnDevelopers )
    EVT_BUTTON(lmID_BTN_ART_CREDITS, AboutDialog::OnArtCredits )
    EVT_BUTTON(lmID_BTN_SOFTWARE_CREDITS, AboutDialog::OnSoftwareCredits )
    EVT_BUTTON(lmID_BTN_BUILD_INFO, AboutDialog::OnBuildInfo )

wxEND_EVENT_TABLE()


//---------------------------------------------------------------------------------------
AboutDialog::AboutDialog(wxWindow* pParent, ApplicationScope& appScope)
    : wxDialog(pParent, wxID_ANY, "LenMus", wxDefaultPosition, wxSize(600, 400),
               wxDEFAULT_DIALOG_STYLE, "dialogBox")
    , m_appScope(appScope)
{
    // create the dialog controls
    CreateControls();

    //load logo icon
    m_pBmpLogo->SetBitmap( wxArtProvider::GetIcon("logo50x67", wxART_OTHER) );

    //initializations
    m_sHeader =
      "<html>"
      _T("<head><META http-equiv=\"Content-Type\" content=\"text/html; charset=") +
      wxLocale::GetSystemEncodingName() +
      _T("\"></head>")
      _T("<body bgcolor=\"#ffffff\">")
      "<font size=1>";

    //version
    m_sVersionNumber = m_appScope.get_version_string();

    //title and subtitle
    m_pTxtTitle->SetLabel( m_appScope.get_app_full_name() );
    m_pTxtSubtitle->SetLabel(_("A free program for music language learning"));

    CentreOnScreen();

    //display purpose information
    wxCommandEvent event;       //no need to initialize it as it is not used
    OnPurpose(event);

}

//---------------------------------------------------------------------------------------
AboutDialog::~AboutDialog()
{
}

//---------------------------------------------------------------------------------------
void AboutDialog::CreateControls()
{
    wxBoxSizer* pMainSizer;
    pMainSizer = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer* pHeadersSizer;
    pHeadersSizer = new wxBoxSizer( wxHORIZONTAL );

    m_pBmpLogo = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
    pHeadersSizer->Add( m_pBmpLogo, 0, wxALIGN_TOP|wxALL, 5 );

    wxBoxSizer* pTitlesSizer;
    pTitlesSizer = new wxBoxSizer( wxVERTICAL );

    m_pTxtTitle = new wxStaticText( this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0 );
    m_pTxtTitle->SetFont( wxFont( 14, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Arial" ) );

	pTitlesSizer->Add(m_pTxtTitle, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

    m_pTxtSubtitle = new wxStaticText( this, wxID_ANY, _("A free program for music language learning"), wxDefaultPosition, wxDefaultSize, 0 );
    m_pTxtSubtitle->SetFont( wxFont( 10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Arial" ) );

	pTitlesSizer->Add(m_pTxtSubtitle, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

    pHeadersSizer->Add( pTitlesSizer, 1, wxEXPAND|wxALL, 5 );

    pMainSizer->Add( pHeadersSizer, 0, wxEXPAND|wxALL, 5 );

    wxBoxSizer* pInfoSizer;
    pInfoSizer = new wxBoxSizer( wxHORIZONTAL );

    m_pHtmlWindow = new wxHtmlWindow( this, wxID_ANY, wxDefaultPosition, wxSize( 200,150 ), wxHW_SCROLLBAR_AUTO|wxBORDER_SUNKEN|wxHSCROLL|wxVSCROLL);
    pInfoSizer->Add( m_pHtmlWindow, 1, wxEXPAND|wxBOTTOM, 5 );

    wxBoxSizer* pButtonsSizer;
    pButtonsSizer = new wxBoxSizer( wxVERTICAL );

    m_pBtnPurpose = new wxButton( this, lmID_BTN_PURPOSE, _("Purpose"), wxDefaultPosition, wxDefaultSize, 0 );
    pButtonsSizer->Add( m_pBtnPurpose, 0, wxEXPAND|wxALL, 5 );

    m_pBtnLicense = new wxButton( this, lmID_BTN_LICENCE, _("Licence"), wxDefaultPosition, wxDefaultSize, 0 );
    pButtonsSizer->Add( m_pBtnLicense, 0, wxEXPAND|wxLEFT|wxRIGHT|wxBOTTOM, 5 );

    m_pBtnDevelopers = new wxButton( this, lmID_BTN_DEVELOPERS, _("Contributors"), wxDefaultPosition, wxDefaultSize, 0 );
    pButtonsSizer->Add( m_pBtnDevelopers, 0, wxEXPAND|wxLEFT|wxRIGHT|wxBOTTOM, 5 );

    m_pBtnArtCredits = new wxButton( this, lmID_BTN_ART_CREDITS, _("Art design"), wxDefaultPosition, wxDefaultSize, 0 );
    pButtonsSizer->Add( m_pBtnArtCredits, 0, wxEXPAND|wxLEFT|wxRIGHT|wxBOTTOM, 5 );

    m_pBtnSoftwareCredits = new wxButton( this, lmID_BTN_SOFTWARE_CREDITS, _("Software used"), wxDefaultPosition, wxDefaultSize, 0 );
    pButtonsSizer->Add( m_pBtnSoftwareCredits, 0, wxEXPAND|wxLEFT|wxRIGHT|wxBOTTOM, 5 );

    m_pBtnBuildInfo = new wxButton( this, lmID_BTN_BUILD_INFO, _("Build info."), wxDefaultPosition, wxDefaultSize, 0 );
    pButtonsSizer->Add( m_pBtnBuildInfo, 0, wxEXPAND|wxLEFT|wxRIGHT|wxBOTTOM, 5 );

    pButtonsSizer->Add( 5, 5, 1, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    m_pBtnAccept = new wxButton( this, lmID_BTN_ACCEPT, _("Accept"), wxDefaultPosition, wxDefaultSize, 0 );
    pButtonsSizer->Add( m_pBtnAccept, 0, wxALIGN_CENTER_HORIZONTAL|wxLEFT|wxRIGHT|wxTOP, 5 );

    pInfoSizer->Add( pButtonsSizer, 0, wxEXPAND|wxLEFT|wxBOTTOM, 5 );

    pMainSizer->Add( pInfoSizer, 1, wxEXPAND|wxLEFT|wxRIGHT|wxBOTTOM, 5 );

    this->SetSizer( pMainSizer );
    this->Layout();
}

//---------------------------------------------------------------------------------------
void AboutDialog::OnAccept(wxCommandEvent& WXUNUSED(event))
{
   EndModal(wxID_OK);
}

//---------------------------------------------------------------------------------------
void AboutDialog::OnPurpose(wxCommandEvent& WXUNUSED(event))
{
    wxString sContent = m_sHeader +
      "<center>"
      "<h3>" + _("Purpose") + "</h3>"
      "</center><p>" +
_("LenMus is a free program to help you in the study of music theory and  \
ear training.") +
      "</p><p>" +
_("The LenMus Project is an open project, committed to the principles of \
Open Source, free education, and open access to information. It has no comercial \
purpose. It is an open workbench for working on all areas related to teaching \
music, and music representation and management with computers. It aims at \
developing publicly available knowledge, methods and algorithms related to all \
these areas and at the same time provides free quality software for music \
students, amateurs, and teachers.") +
      "</p><p>" +
_("If you find a bug or have a suggestion, please contact me. \
I depend on your feedback in order to continue to improve \
LenMus. So please visit LenMus website and give me your bug reports \
and feature requests. Thank you very much.") +
      "</p></body></html>";

    m_pHtmlWindow->SetPage(sContent);

}

//---------------------------------------------------------------------------------------
void AboutDialog::OnLicense(wxCommandEvent& WXUNUSED(event))
{
    wxString sContent = m_sHeader +
        "<center>"
        "<h3>" + _("License") + "</h3></center><p>"
        "LenMus " + m_sVersionNumber + " " +
        "Copyright &copy; 2010-2020 Cecilio Salmer&oacute;n." +
        "</p><p>" +
_("This program is free software; you can redistribute it and/or modify it \
under the terms of the GNU General Public License as published by the Free \
Software Foundation, either version 3 of the License, or (at your option) \
any later version.") +
        "</p><p>" +
_("This program is distributed in the hope that it will be useful, but \
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY \
or FITNESS FOR A PARTICULAR PURPOSE.  For more details see the GNU General \
Public License at ") +
        "<a href='http://www.gnu.org/licenses/'>http://www.gnu.org/licenses/</a>"
        "</p></body></html>";

        m_pHtmlWindow->SetPage(sContent);

}

//---------------------------------------------------------------------------------------
void AboutDialog::OnArtCredits(wxCommandEvent& WXUNUSED(event))
{
    wxString sContent = m_sHeader +
        "<center>"
        "<h3>" + _("Art desing credits") + "</h3></center><p>" +
        _("Some icons are original artwork for LenMus project; all these original icons are released \
under GNU GPL licence and under Creative Commons Attribution-ShareAlike \
license, you can choose the one you prefer for the intended usage. The other \
icons are taken from different sources:") +
        "<br /><br /></p><ul><li>" +
        _("Taken or derived from those available in the Ximian collection \
(http://www.novell.com/coolsolutions/feature/1637.html). \
Novell, who retains the copyright, has released these icons under the LGPL license. \
This means that you can use the icons in your \
programs free of charge. If you want to fork the icons into your own icon collection, you need \
to retain the license and the original copyrights, but from there you're free to do what \
you want.") +
        "<br /></li><li>" +
        _("Taken or derived from the GNOME project (http://art.gnome.org/themes/icon/1150), \
Tango theme. These icons are released under the Creative Commons Attribution-ShareAlike \
license.") +
        "<br /></li><li>" +
        _("These icons are taken or derived from KDE project \
(http://kde.openoffice.org/servlets/ProjectDocumentList?folderID=314&amp;expandFolder=314). \
These icons are released under the GNU GPL license.") +
        "<br /></li><li>" +
        _("Taken or derived from eMule project (http://sourceforge.net/projects/emule) \
icons. eMule is a GNU GPL project so I assume that its icons are also available under GNU GPL.") +
        "<br /></li><li>" +
_("Taken or derived from John Zaitseff's icons \
(http://www.zap.org.au/documents/icons/dirtree-icons/sample.html). According to what \
is said there, these icons are released under the terms of the GNU General Public License.") +
        "<br /><br /></li></ul>" +
        _("To LenMus developers knowledge, all used icons are freely usable either \
under GNU GPL, \
LGPL licence, or Creative Commons. If this were not the case, please, let us \
know to stop using them. \
See file \"icons.htm\" in folder \"res/icons\" for more details.") +
        "</p></body></html>";

        m_pHtmlWindow->SetPage(sContent);

}

//---------------------------------------------------------------------------------------
void AboutDialog::OnSoftwareCredits(wxCommandEvent& WXUNUSED(event))
{
    wxString sContent = m_sHeader +
        "<center>"
        "<h3>" + _("Software credits") + "</h3></center><p>" +
        _("LenMus Phonascus uses <b>PortMidi</b> the Portable Real-Time MIDI Library \
(http://www.cs.cmu.edu/~music/portmusic/). PortMidi is copyright (c) 1999-2000 Ross \
Bencina and Phil Burk and copyright (c) 2001 Roger B. Dannenberg. Its licence permits \
free use.") +
        "</p><p>" +
        _("LenMus uses <b>sqlite3</b> (http://www.sqlite.org/), \
with <b>wxSQLite3</b> wrapper (http://wxcode.sourceforge.net/components/wxsqlite3) for wxWidgets. \
SQLite is a widely used library that implements a transactional SQL database engine. \
Unlike client-server database management systems, the SQLite engine is not a standalone process \
with which the program communicates. Instead, the SQLite library is linked with your program \
and becomes an integral part of it. SQLite was created by D. Richard Hipp and the source code \
is in the public domain and is thus free for use for any purpose, commercial or private. \
Wrapper wxSQLite3 was written by Ulrich Telle.") +
        "</p><p>" +
        _("LenMus Phonascus is built using the <b>wxWidgets</b> application framework \
(http://www.wxwidgets.org). It is 'Open Source', has multi-platform support, it is \
ease to learn and extend, it has a helpful community, and also has the possibility \
to use it in commercial products without licencing.") +
        "</p><p>" +
        _("And, of course, this program uses the LenMus <b>Lomse</b> free library \
to render the eBooks and the scores. See https://github.com/lenmus/lomse") +
        "</p></body></html>";

        m_pHtmlWindow->SetPage(sContent);
}

//---------------------------------------------------------------------------------------
void AboutDialog::OnBuildInfo(wxCommandEvent& WXUNUSED(event))
{
    //get info
    wxString sLomseVersion = to_wx_string( LibraryScope::get_version_long_string() );
    wxSQLite3Database* pDB = m_appScope.get_database();
    wxString sSQLiteVersion = pDB->GetVersion();
    wxString sLenmusNumbers = wxString::Format("[%d.%d.%s]",
                                           m_appScope.get_version_major(),
                                           m_appScope.get_version_minor(),
                                           m_appScope.get_version_patch()
                                          );
    wxString sLomseNumbers = wxString::Format("[%d.%d.%d]",
                                           LibraryScope::get_version_major(),
                                           LibraryScope::get_version_minor(),
                                           LibraryScope::get_version_patch()
                                          );

    //Prepare build info message
    wxString sContent = m_sHeader +
        "<center>"
        "<h3>" + _("Build information") + "</h3></center><p>" +
        _("Program version:") + " " + m_appScope.get_long_version_string() +
        " " + sLenmusNumbers + "<br>" +
        _("Program build date:") + " " __DATE__ + ", " + __TIME__ + "<br>" +
        wxVERSION_STRING + "<br>" +
        "lomse " + sLomseVersion + " " + sLomseNumbers + "<br>" +
        "wxMidi " + wxMIDI_VERSION + "<br>" +
        "sqlite3 " + sSQLiteVersion + "<br><br><br>" +
        _("Your computer information:") +
        "<br>Charset encoding: " + wxLocale::GetSystemEncodingName() +
        //"<br>System locale name: " + wxGetApp().GetLocaleSysName() +
        //"<br>Canonical locale name: " + wxGetApp().GetLanguageCanonicalName() +
        "</p></body></html>";

    m_pHtmlWindow->SetPage(sContent);

}

//---------------------------------------------------------------------------------------
void AboutDialog::OnDevelopers(wxCommandEvent& WXUNUSED(event))
{
    wxString sContent = m_sHeader +
    "<center>"
    "<p><font size='+2'><b>" + _("Contributors") + "</b></font><br />" +
    "</p></center><p>" +
    "<table border='0' width='100%' cellpadding='0' cellspacing='0'>"
        //
    "<tr><td colspan='3'><b>" +
        _("Project management:") + "</b></td></tr>"
    "  <tr><td width='40'>&nbsp;</td><td colspan='2'>Cecilio Salmer&oacute;n</td></tr>"
    "<tr><td colspan='3'>&nbsp;</td></tr>"
        //
    "<tr><td colspan='3'><b>" +
        _("Programming:") + "</b></td></tr>"
    "  <tr><td>&nbsp;</td><td colspan='2'>Cecilio Salmer&oacute;n</td></tr>"
    "  <tr><td>&nbsp;</td><td colspan='2'>Carlos De La Fuente</td></tr>"
    "<tr><td colspan='3'>&nbsp;</td></tr>"
        //
    "<tr><td colspan='3'><b>" +
        _("Linux package:") + "</b></td></tr>"
    "  <tr><td>&nbsp;</td><td colspan='2'>Antonio Nicol&aacute;s Pina</td></tr>"
    "  <tr><td>&nbsp;</td><td colspan='2'>Miguel Fernandez Fidalgo</td></tr>"
    "<tr><td colspan='3'>&nbsp;</td></tr>"
        //
    "<tr><td colspan='3'><b>" +
        _("macOS package:") + "</b></td></tr>"
    "  <tr><td>&nbsp;</td><td colspan='2'>Bob Byerly</td></tr>"
    "<tr><td colspan='3'>&nbsp;</td></tr>"
        //
    "<tr><td colspan='3'><b>" +
        _("Music advice:") + "</b></td></tr>"
    "<tr><td colspan='3'>" +
    "  <tr><td>&nbsp;</td><td colspan='2'>Javier Alejano</td></tr>"
    "  <tr><td>&nbsp;</td><td colspan='2'>Marcelo G&aacute;lvez</td></tr>"
    "  <tr><td>&nbsp;</td><td colspan='2'>Ana Mar&iacute;a Madorr&aacute;n</td></tr>"
    "  <tr><td>&nbsp;</td><td colspan='2'>Emilio Mes&iacute;as</td></tr>"
    "</td></tr>"
    "<tr><td colspan='3'>&nbsp;</td></tr>"
        //
    "<tr><td colspan='3'><b>" +
        _("Music texts and exercises:") + "</b></td></tr>"
    "  <tr><td>&nbsp;</td><td colspan='2'>" +
    _("See credits page on each eBook") +
    "</td></tr>"
    "<tr><td colspan='3'>&nbsp;</td></tr>"
        //
    "<tr><td colspan='3'><b>" +
        _("Translation:") + "</b></td></tr>"
    "  <tr><td>&nbsp;</td><td colspan='2'><b><i>" + _("Basque") + "</i></b></td></tr>"
        "<tr><td>&nbsp;</td><td width='40'>&nbsp;</td>"
        "<td>Karlos del Olmo Serna</td></tr>"
    "  <tr><td>&nbsp;</td><td colspan='2'><b><i>" + _("Dutch") + "</i></b></td></tr>"
        "<tr><td>&nbsp;</td><td width='40'>&nbsp;</td>"
        "<td>Jack Van Handenhove</td></tr>"
    "  <tr><td>&nbsp;</td><td colspan='2'><b><i>" + _("Simplified Chinese") + "</i></b></td></tr>"
        "<tr><td>&nbsp;</td><td width='40'>&nbsp;</td>"
        L"<td>Wenxi Lu (卢文汐)</td></tr>"
    "  <tr><td>&nbsp;</td><td colspan='2'><b><i>" + _("French") + "</i></b></td></tr>"
        "<tr><td>&nbsp;</td><td width='40'>&nbsp;</td>"
        "<td>Carlos Alarcia</td></tr>"
        "<tr><td>&nbsp;</td><td width='40'>&nbsp;</td>"
        L"<td>Frédéric Baudrand</td></tr>"
        "<tr><td>&nbsp;</td><td width='40'>&nbsp;</td>"
        "<td>Christophe Marcoux</td></tr>"
    "  <tr><td>&nbsp;</td><td colspan='2'><b><i>" + _("Galician") + "</i></b></td></tr>"
        "<tr><td>&nbsp;</td><td width='40'>&nbsp;</td>"
        "<td>Emilio Mes&iacute;as</td></tr>"
        "<tr><td>&nbsp;</td><td width='40'>&nbsp;</td>"
        "<td>Xema Sanxurxo Rodr&iacute;guez</td></tr>"
    "  <tr><td>&nbsp;</td><td colspan='2'><b><i>" + _("German") + "</i></b></td></tr>"
        "<tr><td>&nbsp;</td><td width='40'>&nbsp;</td>"
        "<td>Undine Peters</td></tr>"
    "  <tr><td>&nbsp;</td><td colspan='2'><b><i>" + _("Greek") + "</i></b></td></tr>"
        "<tr><td>&nbsp;</td><td width='40'>&nbsp;</td>"
        L"<td>Avgeris Tsironis (Αυγέρης Τσιρώνης)</td></tr>"
    "  <tr><td>&nbsp;</td><td colspan='2'><b><i>" + _("Italian") + "</i></b></td></tr>"
        "<tr><td>&nbsp;</td><td width='40'>&nbsp;</td>"
        "<td>Mauro Giubileo</td></tr>"
#if (0)     //Russian language not ready
    "  <tr><td>&nbsp;</td><td colspan='2'><b><i>" + _("Russian") + "</i></b></td></tr>"
        "<tr><td>&nbsp;</td><td width='40'>&nbsp;</td>"
        "<td>Arthur Aslanyan</td></tr>"
#endif
    "  <tr><td>&nbsp;</td><td colspan='2'><b><i>" + _("Spanish") + "</i></b></td></tr>"
        "<tr><td>&nbsp;</td><td width='40'>&nbsp;</td>"
        "<td>Cecilio Salmer&oacute;n</td></tr>"
    "  <tr><td>&nbsp;</td><td colspan='2'><b><i>" + _("Turkish") + "</i></b></td></tr>"
        "<tr><td>&nbsp;</td><td width='40'>&nbsp;</td>"
        "<td>Elif &Ouml;zt&uuml;rk</td></tr>"
    "<tr><td colspan='3'>&nbsp;</td></tr>"
        //
    "<tr><td colspan='3'><b>" +
        _("Thanks also to:") + "</b></td></tr>"
    "  <tr><td>&nbsp;</td><td colspan='2'>Karim Doumaz</td></tr>"
    "  <tr><td>&nbsp;</td><td colspan='2'>Jack Van Handenhove</td></tr>"
    "  <tr><td>&nbsp;</td><td colspan='2'>Mar&iacute;a Jes&uacute;s Mart&iacute;nez Pascua</td></tr>"
    "</table><p>&nbsp;</p><p>" +
_("If you would like to help developing this program or join the project to help in any \
other way, you are indeed welcome; please, visit LenMus at GitHub (https://github.com/lenmus/lenmus) \
and leave a message by opening a new issue. Thank you and welcome.") +
    "</p></body></html>";

    m_pHtmlWindow->SetPage(sContent);

}


}   //namespace lenmus
