//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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
//-------------------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "AboutDlg.h"
#endif

// for (compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/dialog.h"
#include "wx/button.h"
#include "wx/xrc/xmlres.h"

#include "AboutDialog.h"

//to determine debug/release version
extern bool g_fReleaseVersion;        // to enable/disable debug features
extern bool g_fReleaseBehaviour;    // This flag is only used to force release behaviour when
                                    // in debug mode, and only for some functions (the ones using this flag)

//to get wxMidi version
#include "wxMidi.h"

#include "TheApp.h"         //to get access to locale info.

// To get char and scale info
#include "../app/MainFrame.h"
extern lmMainFrame* g_pMainFrame;

#include "../html/HtmlWindow.h"

//IDs for controls
const int lmID_BTN_ACCEPT = wxNewId();
const int lmID_BTN_ART_CREDITS = wxNewId();
const int lmID_BTN_BUILD_INFO = wxNewId();
const int lmID_BTN_LICENCE = wxNewId();
const int lmID_BTN_PURPOSE = wxNewId();
const int lmID_BTN_SOFTWARE_CREDITS = wxNewId();
const int lmID_BTN_DEVELOPERS = wxNewId();


BEGIN_EVENT_TABLE(lmAboutDialog, wxDialog)
    EVT_BUTTON(lmID_BTN_ACCEPT, lmAboutDialog::OnAccept )
    EVT_BUTTON(lmID_BTN_PURPOSE, lmAboutDialog::OnPurpose )
    EVT_BUTTON(lmID_BTN_LICENCE, lmAboutDialog::OnLicense )
    EVT_BUTTON(lmID_BTN_DEVELOPERS, lmAboutDialog::OnDevelopers )
    EVT_BUTTON(lmID_BTN_ART_CREDITS, lmAboutDialog::OnArtCredits )
    EVT_BUTTON(lmID_BTN_SOFTWARE_CREDITS, lmAboutDialog::OnSoftwareCredits )
    //EVT_BUTTON( XRCID( "btnTranslators" ), lmAboutDialog::OnTranslators )
    EVT_BUTTON(lmID_BTN_BUILD_INFO, lmAboutDialog::OnBuildInfo )

END_EVENT_TABLE()


lmAboutDialog::lmAboutDialog(wxWindow* pParent)
	: wxDialog(pParent, wxID_ANY, _T("LenMus"), wxDefaultPosition, wxSize(600, 400),
               wxDEFAULT_DIALOG_STYLE, _T("dialogBox"))
{
    // create the dialog controls
	CreateControls();

    //load logo icon
    m_pBmpLogo->SetBitmap( wxArtProvider::GetIcon(_T("logo50x67"), wxART_OTHER) );

    //initializations
    m_sHeader =
      _T("<html>")
      _T("<head><META http-equiv=\"Content-Type\" content=\"text/html; charset=") +
      wxLocale::GetSystemEncodingName() +
      _T("\"></head>")
      _T("<body bgcolor=\"#ffffff\">")
      _T("<font size=1>");

    //version
    m_sVersionNumber = wxGetApp().GetVersionNumber();

    //title and subtitle
    m_pTxtTitle->SetLabel(m_pTxtTitle->GetLabel() + m_sVersionNumber);
    m_pTxtSubtitle->SetLabel(_("A free program for music language learning"));

    CentreOnScreen();

    //display purpose information
    wxCommandEvent event;       //no need to initialize it as it is not used
    OnPurpose(event);

}

lmAboutDialog::~lmAboutDialog()
{
}

void lmAboutDialog::CreateControls()
{
	wxBoxSizer* pMainSizer;
	pMainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* pHeadersSizer;
	pHeadersSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_pBmpLogo = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	pHeadersSizer->Add( m_pBmpLogo, 0, wxALIGN_TOP|wxALL, 5 );
	
	wxBoxSizer* pTitlesSizer;
	pTitlesSizer = new wxBoxSizer( wxVERTICAL );
	
	m_pTxtTitle = new wxStaticText( this, wxID_ANY, _T("LenMus Phonascus"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pTxtTitle->SetFont( wxFont( 14, 74, 90, 92, false, _("Arial") ) );
	
	pTitlesSizer->Add( m_pTxtTitle, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5 );
	
	m_pTxtSubtitle = new wxStaticText( this, wxID_ANY, _("A free program for music language learning"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pTxtSubtitle->SetFont( wxFont( 10, 74, 90, 90, false, _("Arial") ) );
	
	pTitlesSizer->Add( m_pTxtSubtitle, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5 );
	
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
	
	m_pBtnDevelopers = new wxButton( this, lmID_BTN_DEVELOPERS, _("Project team"), wxDefaultPosition, wxDefaultSize, 0 );
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

void lmAboutDialog::OnAccept(wxCommandEvent& WXUNUSED(event))
{
   EndModal(wxID_OK);
}

void lmAboutDialog::OnPurpose(wxCommandEvent& WXUNUSED(event))
{
    wxString sContent = m_sHeader +
      _T("<center>")
      _T("<h3>") + _("Purpose") + _T("</h3>")
      _T("</center><p>") +
_("LenMus is a free program to help you in the study of music theory and  \
ear training. It is available for Windows but soon it will be also available \
for Mac OS X, Linux, and other Unix-like operating systems.") +
      _T("</p><p>") +
_("The LenMus Project is an open project, committed to the principles of \
Open Source, free education, and open access to information. It has no comercial \
purpose. It is an open workbench for working on all areas related to teaching \
music, and music representation and management with computers. It aims at \
developing publicly available knowledge, methods and algorithms related to all \
these areas and at the same time provides free quality software for music \
students, amateurs, and teachers.") +
      _T("</p><p>") +
_("If you find a bug or have a suggestion, please contact us. \
We depend on your feedback in order to continue to improve \
LenMus. So please visit our website and give us your bug reports \
and feature requests. Thank you very much.") +
      _T("</p></body></html>");

    m_pHtmlWindow->SetPage(sContent);

}

void lmAboutDialog::OnLicense(wxCommandEvent& WXUNUSED(event))
{
    wxString sContent = m_sHeader +
        _T("<center>")
        _T("<h3>") + _("License") + _T("</h3></center><p>")
        _T("LenMus ") + m_sVersionNumber + _T(" ") +
        _("Copyright &copy; 2002-2008 Cecilio Salmer&oacute;n.") +
        _T("</p><p>") +
_("This program is free software; you can redistribute it and/or modify it \
under the terms of the GNU General Public License as published by the Free \
Software Foundation, either version 3 of the License, or (at your option) \
any later version.") +
        _T("</p><p>") +
_("This program is distributed in the hope that it will be useful, but \
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY \
or FITNESS FOR A PARTICULAR PURPOSE.  For more details see the GNU General \
Public License at ") +
        _T("<a href='http://www.gnu.org/licenses/'>http://www.gnu.org/licenses/</a>")
        _T("</p></body></html>");

        m_pHtmlWindow->SetPage(sContent);

}

void lmAboutDialog::OnArtCredits(wxCommandEvent& WXUNUSED(event))
{
    wxString sContent = m_sHeader +
        _T("<center>")
        _T("<h3>") + _("Art desing credits") + _T("</h3></center><p>") +
        _("Some icons are original artwork for LenMus project; all these original icons are released \
under GNU GPL licence and under Creative Commons Attribution-ShareAlike \
license, you can choose the one you prefer for the intended usage. The other \
icons are taken from different sources:") +
        _T("<br /><br /></p><ul><li>") +
        _("Taken or derived from those available in the Ximian collection \
(http://www.novell.com/coolsolutions/feature/1637.html). \
Novell, who retains the copyright, has released these icons under the LGPL license. \
This means that you can use the icons in your \
programs free of charge. If you want to fork the icons into your own icon collection, you need \
to retain the license and the original copyrights, but from there you're free to do what \
you want.") +
        _T("<br /></li><li>") +
        _("Taken or derived from the GNOME project (http://art.gnome.org/themes/icon/1150), \
Tango theme. These icons are released under the Creative Commons Attribution-ShareAlike \
license.") +
        _T("<br /></li><li>") +
        _("These icons are taken or derived from KDE project \
(http://kde.openoffice.org/servlets/ProjectDocumentList?folderID=314&amp;expandFolder=314). \
These icons are released under the GNU GPL license.") +
        _T("<br /></li><li>") +
        _("Taken or derived from eMule project (http://sourceforge.net/projects/emule) \
icons. eMule is a GNU GPL project so I assume that its icons are also available under GNU GPL.") +
        _T("<br /></li><li>") +
_("Taken or derived from John Zaitseff's icons \
(http://www.zap.org.au/documents/icons/dirtree-icons/sample.html). According to what \
is said there, these icons are released under the terms of the GNU General Public License.") +
        _T("<br /><br /></li></ul>") +
        _("To LenMus developers knowledge, all used icons are freely usable either \
under GNU GPL, \
LGPL licence, or Creative Commons. If this were not the case, please, let us \
know to stop using them. \
See file \"icons.htm\" in folder \"res/icons\" for more details.") +
        _T("</p></body></html>");

        m_pHtmlWindow->SetPage(sContent);

}

void lmAboutDialog::OnSoftwareCredits(wxCommandEvent& WXUNUSED(event))
{
    wxString sContent = m_sHeader +
        _T("<center>")
        _T("<h3>") + _("Software credits") + _T("</h3></center><p>") +
        _("LenMus Phonascus uses <b>PortMidi</b> the Portable Real-Time MIDI Library \
(http://www.cs.cmu.edu/~music/portmusic/). PortMidi is copyright © 1999-2000 Ross \
Bencina and Phil Burk and copyright © 2001 Roger B. Dannenberg. Its licence permits \
free use.") +
        _T("</p><p>") +
        _("This program uses the <b>Anti-Grain Geometry</b> (http://www.antigrain.com) \
software developed by Maxim Shemanarev. AGG is an excellent open source, \
free graphic library, in source code. \
It produces pixel images \
in memory from vectorial data and it supports anti-aliasing, \
has very high performance, it is platform independent, very \
flexible and extensible, with a light weight design, and very good \
reliability and stability.") +
        _T("</p><p>") +
        _("This program uses the <b>FreeType 2</b> \
software. It is a free open source font engine, designed to be small, efficient, \
highly customizable and portable, while capable of producing high-quality output \
(glyph images). It can be used in graphics libraries, display servers, font \
conversion tools, text image generation tools, and many other products as well. \
It was developed by David Turner, Robert Wilhelm, and Werner Lemberg. FreeType is \
copyright © 1996-2002 The FreeType Project (http://www.freetype.org).") +
        _T("</p><p>") +
        _("LenMus Phonascus is built using the <b>wxWidgets</b> application framework \
(http://www.wxwidgets.org). It is 'Open Source', has multi-platform support, it is \
ease to learn and extend, it has a helpful community, and also has the possibility \
to use it in commercial products without licencing.") +
        _T("</p><p>") +
        _("Most of 'lmbasic.ttf' font glyphs are taken from <b>LilyPond</b> GNU GPL project \
(http://lilypond.org), Feta font.") +
        _T("</p></body></html>");

        m_pHtmlWindow->SetPage(sContent);
}

void lmAboutDialog::OnBuildInfo(wxCommandEvent& WXUNUSED(event))
{
   #if defined(__WXMSW__)
    // Get screen information
    wxScreenDC dc;
    wxSize ppiScreen = dc.GetPPI();     //logical pixels per inch of screen
    wxString sScreenPPI = wxString::Format(_T("PPI: (x=%d, y=%d), Char size (px): (w=%d, h=%d)"),
                            ppiScreen.GetWidth(), ppiScreen.GetHeight(),
                            dc.GetCharWidth(), dc.GetCharHeight() );

    //get info about font size
    wxString sFontInfo = _T("No window available");
    lmHtmlWindow* pHtmlWnd = g_pMainFrame->GetHtmlWindow();
    if (pHtmlWnd) {
        double rHtmlWinScale = pHtmlWnd->GetScale();
        int nCharWidth = pHtmlWnd->GetCharWidth();
        int nCharHeight = pHtmlWnd->GetCharHeight();
        sFontInfo = wxString::Format(_T("Char size (px): (w=%d, h=%d), rScale=%.4f"),
                        nCharWidth, nCharHeight, rHtmlWinScale );
    }
    #else
    wxString sFontInfo = _T("No info available");
    wxString sScreenPPI = _T("No info available");
    #endif

    //Prepare build info message
    wxString sContent = m_sHeader +
        _T("<center>")
        _T("<h3>") + _("Build information") + _T("</h3></center><p>") +
        _("Program build date:") +
        _T(" ") __TDATE__ _T("<br>") +
        wxVERSION_STRING + _T("<br>") +
        _("wxMidi Version ") + wxMIDI_VERSION + _T("<br><br><br>") +
        _("Your computer information:") +
        _T("<br>Charset encoding: ") + wxLocale::GetSystemEncodingName() + 
        _T("<br>System locale name: ") + wxGetApp().GetLocaleSysName() + 
        _T("<br>Canonical locale name: ") + wxGetApp().GetLanguageCanonicalName() + 
        _T("<br><br>Display: ") + sScreenPPI + 
        _T("<br>eBook window: ") + sFontInfo + 
        _T("<br></body></html>");



        m_pHtmlWindow->SetPage(sContent);

}

void lmAboutDialog::OnDevelopers(wxCommandEvent& WXUNUSED(event))
{
    wxString sContent = m_sHeader +
      _T("<center>")
      _T("<p><font size='+2'><b>") + _("Project team") + _T("</b></font><br />")
      + _("(alphabetical order)") +
      _T("</p></center><p>") +
_("This program has been designed and developed by volunteers. \
Some of them devoted a few hours, others months or years. But all them \
gave their time for free to the project. Thanks to all them.") +
    _T("</p><p>&nbsp;</p><p>") +
    _T("<table border='0' width='100%' cellpadding='0' cellspacing='0'>")
        //
    _T("<tr><td colspan='3'><b>") +
        _("Management and programming:") + _T("</b></td></tr>")
    _T("  <tr><td width='40px'>&nbsp;</td><td colspan='2'>Cecilio Salmer&oacute;n</td></tr>")
    _T("<tr><td colspan='3'>&nbsp;</td></tr>")
        //
    _T("<tr><td colspan='3'><b>") +
        _("Music texts and exercises:") + _T("</b></td></tr>")
    _T("  <tr><td>&nbsp;</td><td colspan='2'>Marcelo G&aacute;lvez</td></tr>")
    _T("  <tr><td>&nbsp;</td><td colspan='2'>Cecilio Salmer&oacute;n</td></tr>")
    _T("<tr><td colspan='3'>&nbsp;</td></tr>")
        //
   //_T("<tr><td colspan='3'><b>") +
   //     _("Package for Linux prepared by:") + _T("</b></td></tr>")
   // _T("  <tr><td>&nbsp;</td><td colspan='2'>Juan Manuel Garc&iacute;a Molina</td></tr>")
   // _T("<tr><td colspan='3'>&nbsp;</td></tr>")
        //
    _T("<tr><td colspan='3'><b>") +
        _("Translation:") + _T("</b></td></tr>")
    _T("  <tr><td>&nbsp;</td><td colspan='2'>") + _("Basque") + _T("</td></tr>")
        _T("<tr><td>&nbsp;</td><td width='40px'>&nbsp;</td>")
        _T("<td>Karlos del Olmo Serna</td></tr>")
    _T("  <tr><td>&nbsp;</td><td colspan='2'>") + _("Dutch") + _T("</td></tr>")
        _T("<tr><td>&nbsp;</td><td width='40px'>&nbsp;</td>")
        _T("<td>Jack Van Handenhove</td></tr>")
    _T("  <tr><td>&nbsp;</td><td colspan='2'>") + _("French") + _T("</td></tr>")
        _T("<tr><td>&nbsp;</td><td width='40px'>&nbsp;</td>")
        _T("<td>Carlos Alarcia</td></tr>")
        _T("<tr><td>&nbsp;</td><td width='40px'>&nbsp;</td>")
        _T("<td>Christophe Marcoux</td></tr>")
    _T("  <tr><td>&nbsp;</td><td colspan='2'>") + _("Italian") + _T("</td></tr>")
        _T("<tr><td>&nbsp;</td><td width='40px'>&nbsp;</td>")
        _T("<td>Mauro Giubileo</td></tr>")
    _T("  <tr><td>&nbsp;</td><td colspan='2'>") + _("Spanish") + _T("</td></tr>")
        _T("<tr><td>&nbsp;</td><td width='40px'>&nbsp;</td>")
        _T("<td>Cecilio Salmer&oacute;n</td></tr>")
    _T("  <tr><td>&nbsp;</td><td colspan='2'>") + _("Turkish") + _T("</td></tr>")
        _T("<tr><td>&nbsp;</td><td width='40px'>&nbsp;</td>")
        _T("<td>Elif &Ouml;zt&uuml;rk</td></tr>")
    _T("<tr><td colspan='3'>&nbsp;</td></tr>")
        //
    _T("<tr><td colspan='3'><b>") +
        _("Website maintenace:") + _T("</b></td></tr>")
    _T("  <tr><td>&nbsp;</td><td colspan='2'>Jack Van Handenhove</td></tr>")
    _T("  <tr><td>&nbsp;</td><td colspan='2'>Cecilio Salmer&oacute;n</td></tr>")
    _T("<tr><td colspan='3'>&nbsp;</td></tr>")
        //
    _T("<tr><td colspan='3'><b>") +
        _("Thanks also to:") + _T("</b></td></tr>")
    _T("  <tr><td>&nbsp;</td><td colspan='2'>Javier Alejano</td></tr>")
    _T("  <tr><td>&nbsp;</td><td colspan='2'>Ana Mar&iacute;a Madorr&aacute;n</td></tr>")
    _T("  <tr><td>&nbsp;</td><td colspan='2'>Mar&iacute;a Jes&uacute;s Mart&iacute;nez Pascua</td></tr>")
    _T("</table><p>&nbsp;</p><p>") +
_("If you would like to help developing this program or join the project to help in any \
other way, you are indeed welcome; please, visit the LenMus website at www.lenmus.org and \
leave a message in the 'contact' page. Thank you and welcome.") +
    _T("</p></body></html>");

    m_pHtmlWindow->SetPage(sContent);

}

