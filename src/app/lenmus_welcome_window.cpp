//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2015 LenMus project
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
#include "lenmus_welcome_window.h"
#include "lenmus_standard_header.h"

#include "lenmus_paths.h"
#include "lenmus_main_frame.h"
#include "lenmus_string.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/hyperlink.h>
#include <wx/panel.h>
#include <wx/docview.h>      //for wxFileHistory
#include <wx/artprov.h>


namespace lenmus
{

//IDs for controls
const int lmLINK_NewInLenmus = wxNewId();
const int lmLINK_NewScore = wxNewId();
const int lmLINK_QuickGuide = wxNewId();
const int lmLINK_OpenEBooks = wxNewId();
const int lmLINK_Instructions = wxNewId();
const int lmLINK_Recent1 = wxNewId();
const int lmLINK_Recent2 = wxNewId();
const int lmLINK_Recent3 = wxNewId();
const int lmLINK_Recent4 = wxNewId();
const int lmLINK_Recent5 = wxNewId();
const int lmLINK_Recent6 = wxNewId();
const int lmLINK_Recent7 = wxNewId();
const int lmLINK_Recent8 = wxNewId();
const int lmLINK_Recent9 = wxNewId();


wxBEGIN_EVENT_TABLE(WelcomeWindow, wxScrolledWindow)
    EVT_HYPERLINK   (lmLINK_NewInLenmus, WelcomeWindow::OnNewInLenmus)
    EVT_HYPERLINK   (lmLINK_NewScore, WelcomeWindow::OnNewScore)
    EVT_HYPERLINK   (lmLINK_QuickGuide, WelcomeWindow::OnQuickGuide)
    EVT_HYPERLINK   (lmLINK_OpenEBooks, WelcomeWindow::OnOpenEBooks)
    EVT_HYPERLINK   (lmLINK_Instructions, WelcomeWindow::OnInstructions)
    EVT_HYPERLINK   (lmLINK_Recent1, WelcomeWindow::OnOpenRecent)
    EVT_HYPERLINK   (lmLINK_Recent2, WelcomeWindow::OnOpenRecent)
    EVT_HYPERLINK   (lmLINK_Recent3, WelcomeWindow::OnOpenRecent)
    EVT_HYPERLINK   (lmLINK_Recent4, WelcomeWindow::OnOpenRecent)
    EVT_HYPERLINK   (lmLINK_Recent5, WelcomeWindow::OnOpenRecent)
    EVT_HYPERLINK   (lmLINK_Recent6, WelcomeWindow::OnOpenRecent)
    EVT_HYPERLINK   (lmLINK_Recent7, WelcomeWindow::OnOpenRecent)
    EVT_HYPERLINK   (lmLINK_Recent8, WelcomeWindow::OnOpenRecent)
    EVT_HYPERLINK   (lmLINK_Recent9, WelcomeWindow::OnOpenRecent)
wxEND_EVENT_TABLE()

//---------------------------------------------------------------------------------------
WelcomeWindow::WelcomeWindow(ContentWindow* parent, ApplicationScope& appScope,
                             wxFileHistory* pHistory, wxWindowID id)
    : wxScrolledWindow(parent, id, wxDefaultPosition, wxDefaultSize, wxHW_SCROLLBAR_AUTO | wxHSCROLL | wxVSCROLL)
    , CanvasInterface(parent)
    , m_appScope(appScope)
{
    SetVirtualSize(1000, 750);      //1000 x (770-20) px
    SetScrollRate(20, 20);          //20px steps

	CreateControls(pHistory);

    //load icons
    m_pLearnIcon->SetBitmap( wxArtProvider::GetIcon("welcome_news", wxART_OTHER) );
    //TODO 5.0
    //m_pScoreIcon->SetBitmap( wxArtProvider::GetIcon("welcome_editor", wxART_OTHER) );
    m_pPhonascusIcon->SetBitmap( wxArtProvider::GetIcon("welcome_theory", wxART_OTHER) );
    m_pBmpLeftBanner->SetBitmap( wxArtProvider::GetIcon("welcome_left", wxART_OTHER) );
}

//---------------------------------------------------------------------------------------
void WelcomeWindow::CreateControls(wxFileHistory* WXUNUSED(pHistory))
{
#if (0)     //removed until editor ready
    int nRecentFiles = pHistory->GetCount();
#endif

    //content generated with wxFormBuilder. Modified to:
    // - add version number to title
    // - add loop to load recent files

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );

	wxBoxSizer* pMainSizer;
	pMainSizer = LENMUS_NEW wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* pLeftBannerSizer;
	pLeftBannerSizer = LENMUS_NEW wxBoxSizer( wxHORIZONTAL );

	m_pBmpLeftBanner = LENMUS_NEW wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 165,773 ), 0 );
	pLeftBannerSizer->Add( m_pBmpLeftBanner, 0, 0, 5 );

	pMainSizer->Add( pLeftBannerSizer, 0, wxEXPAND, 20 );

	wxBoxSizer* pContentSizer;
	pContentSizer = LENMUS_NEW wxBoxSizer( wxHORIZONTAL );


	pContentSizer->Add( 0, 0, 1, wxEXPAND, 5 );

	wxBoxSizer* pItemsSizer;
	pItemsSizer = LENMUS_NEW wxBoxSizer( wxVERTICAL );

	wxBoxSizer* pHeadersSizer;
	pHeadersSizer = LENMUS_NEW wxBoxSizer( wxVERTICAL );

	m_pTxtTitle = LENMUS_NEW wxStaticText( this, wxID_ANY, _("Welcome to LenMus"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pTxtTitle->Wrap( -1 );
	m_pTxtTitle->SetFont( wxFont( 16, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Tahoma" ) );

	pHeadersSizer->Add( m_pTxtTitle, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

    //added ----------------------------------------------------
    wxString version = m_appScope.get_version_string();
    wxString title = wxString::Format(_("Version %s"), version.wx_str());
    //----------------------------------------------------------
	m_pTxtVersion = LENMUS_NEW wxStaticText( this, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, 0 );
	m_pTxtVersion->Wrap( -1 );
	pHeadersSizer->Add( m_pTxtVersion, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

	pItemsSizer->Add( pHeadersSizer, 1, wxEXPAND, 5 );

	wxBoxSizer* pLearnSizer;
	pLearnSizer = LENMUS_NEW wxBoxSizer( wxVERTICAL );

	m_pLearnTitle = LENMUS_NEW wxStaticText( this, wxID_ANY, _("About LenMus"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pLearnTitle->Wrap( -1 );
	m_pLearnTitle->SetFont( wxFont( 10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Tahoma" ) );

	pLearnSizer->Add( m_pLearnTitle, 0, wxALL, 5 );

	wxBoxSizer* pLearnSubsizer;
	pLearnSubsizer = LENMUS_NEW wxBoxSizer( wxHORIZONTAL );

	m_pLearnIcon = LENMUS_NEW wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 45,45 ), 0 );
	pLearnSubsizer->Add( m_pLearnIcon, 0, wxALL, 5 );

	wxBoxSizer* pLearnLinksSizer;
	pLearnLinksSizer = LENMUS_NEW wxBoxSizer( wxVERTICAL );

	m_pLinkNewInLenmus = LENMUS_NEW wxHyperlinkCtrl( this, lmLINK_NewInLenmus, _("What's is new in this version"), "", wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	m_pLinkNewInLenmus->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );

	pLearnLinksSizer->Add( m_pLinkNewInLenmus, 0, wxRIGHT|wxLEFT, 5 );

	m_pLinkReportError = LENMUS_NEW wxHyperlinkCtrl( this, wxID_ANY, _("Report an error or make suggestions"), "https://github.com/lenmus/lenmus/issues", wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	m_pLinkReportError->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );

	pLearnLinksSizer->Add( m_pLinkReportError, 0, wxTOP|wxRIGHT|wxLEFT, 5 );

	m_pLinkVisitWebsite = LENMUS_NEW wxHyperlinkCtrl( this, wxID_ANY, _("Visit LenMus website"), "http://www.lenmus.org", wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	m_pLinkVisitWebsite->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );

	pLearnLinksSizer->Add( m_pLinkVisitWebsite, 0, wxTOP|wxRIGHT|wxLEFT, 5 );

	pLearnSubsizer->Add( pLearnLinksSizer, 1, wxEXPAND|wxLEFT, 10 );

	pLearnSizer->Add( pLearnSubsizer, 1, wxEXPAND, 5 );

	pItemsSizer->Add( pLearnSizer, 0, wxTOP|wxRIGHT|wxLEFT, 20 );

	wxBoxSizer* pPhonascusSizer;
	pPhonascusSizer = LENMUS_NEW wxBoxSizer( wxVERTICAL );

	m_pPhonascusTitle = LENMUS_NEW wxStaticText( this, wxID_ANY, _("Learn and practise music theory"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pPhonascusTitle->Wrap( -1 );
	m_pPhonascusTitle->SetFont( wxFont( 10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Tahoma" ) );

	pPhonascusSizer->Add( m_pPhonascusTitle, 0, wxALL, 5 );

	wxBoxSizer* pPhonascusSubsizer;
	pPhonascusSubsizer = LENMUS_NEW wxBoxSizer( wxHORIZONTAL );

	m_pPhonascusIcon = LENMUS_NEW wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 45,45 ), 0 );
	pPhonascusSubsizer->Add( m_pPhonascusIcon, 0, wxALL, 5 );

	wxBoxSizer* pPhonascusLinksSizer;
	pPhonascusLinksSizer = LENMUS_NEW wxBoxSizer( wxVERTICAL );

	m_pLinkInstructions = LENMUS_NEW wxHyperlinkCtrl( this, lmLINK_Instructions, _("Study guide: How to use this program"), "", wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	m_pLinkInstructions->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );

	pPhonascusLinksSizer->Add( m_pLinkInstructions, 0, wxRIGHT|wxLEFT, 5 );

	m_pLinkOpenEBooks = LENMUS_NEW wxHyperlinkCtrl( this, lmLINK_OpenEBooks, _("Open the music books"), "", wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	m_pLinkOpenEBooks->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );

	pPhonascusLinksSizer->Add( m_pLinkOpenEBooks, 0, wxTOP|wxRIGHT|wxLEFT, 5 );

#if (0)     //removed until editor ready
	wxBoxSizer* pRecentScoresSizer;
	pRecentScoresSizer = LENMUS_NEW wxBoxSizer( wxVERTICAL );

	m_pRecentScoresTitle = LENMUS_NEW wxStaticText( this, wxID_ANY, _("Recent documents"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pRecentScoresTitle->Wrap( -1 );
	m_pRecentScoresTitle->SetFont( wxFont( 10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Tahoma" ) );

	pRecentScoresSizer->Add( m_pRecentScoresTitle, 0, wxRIGHT|wxLEFT, 5 );

	wxBoxSizer* pRecentScoresLinksSizer;
	pRecentScoresLinksSizer = LENMUS_NEW wxBoxSizer( wxHORIZONTAL );


	pRecentScoresLinksSizer->Add( 40, 0, 0, 0, 5 );

	wxBoxSizer* pRecentScoresLinksSizer1;
	pRecentScoresLinksSizer1 = LENMUS_NEW wxBoxSizer( wxVERTICAL );

    //loop to load recent files --------------------------------------------------
    for (int i=0; i < nRecentFiles; i++)
    {
	    m_pLinkRecent[i] =
            LENMUS_NEW wxHyperlinkCtrl(this, lmLINK_Recent1+i, pHistory->GetHistoryFile(i),
                                pHistory->GetHistoryFile(i), wxDefaultPosition, wxDefaultSize,
                                wxHL_DEFAULT_STYLE);
        m_pLinkRecent[i]->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );
	    pRecentScoresLinksSizer1->Add( m_pLinkRecent[i], 0, wxRIGHT|wxLEFT, 5 );
    }
    //----------------------------------------------------------------------------

	pRecentScoresLinksSizer->Add( pRecentScoresLinksSizer1, 1, wxEXPAND, 5 );

	pRecentScoresSizer->Add( pRecentScoresLinksSizer, 0, wxEXPAND, 5 );

	pPhonascusLinksSizer->Add( pRecentScoresSizer, 1, wxEXPAND|wxTOP, 5 );
#endif

	pPhonascusSubsizer->Add( pPhonascusLinksSizer, 1, wxEXPAND|wxLEFT, 10 );

	pPhonascusSizer->Add( pPhonascusSubsizer, 1, wxEXPAND, 5 );

	pItemsSizer->Add( pPhonascusSizer, 0, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 20 );

	pContentSizer->Add( pItemsSizer, 10, 0, 5 );


	pContentSizer->Add( 0, 0, 1, wxEXPAND, 5 );

	pMainSizer->Add( pContentSizer, 1, wxEXPAND|wxTOP, 20 );

	this->SetSizer( pMainSizer );
	this->Layout();
}

//---------------------------------------------------------------------------------------
WelcomeWindow::~WelcomeWindow()
{
}

//---------------------------------------------------------------------------------------
void WelcomeWindow::OnNewInLenmus(wxHyperlinkEvent& WXUNUSED(event))
{
    wxString sDoc = "release_notes.htm";
    ShowDocument(sDoc);
}

//---------------------------------------------------------------------------------------
void WelcomeWindow::OnQuickGuide(wxHyperlinkEvent& WXUNUSED(event))
{
////    wxString sDoc = "editor_quick_guide.htm";
////    ShowDocument(sDoc);
//    wxCommandEvent myEvent(wxEVT_COMMAND_MENU_SELECTED, k_menu_help_editor_quick);
//    ::wxPostEvent(this, myEvent);
}

//---------------------------------------------------------------------------------------
void WelcomeWindow::ShowDocument(wxString& sDocName)
{
    Paths* pPaths = m_appScope.get_paths();
    wxString sPath = pPaths->GetLocalePath();
    wxFileName oFile(sPath, sDocName, wxPATH_NATIVE);
	if (!oFile.FileExists())
	{
		//try to display the english version
		sPath = pPaths->GetLocaleRootPath();
		oFile.AssignDir(sPath);
		oFile.AppendDir("en");
		oFile.SetFullName(sDocName);
        if (!oFile.FileExists())
        {
            wxMessageBox(_("Sorry: File not found!"));
            wxLogMessage("[WelcomeWindow::ShowDocument] File %s' not found!",
                         oFile.GetFullPath().wx_str() );
            return;
        }
	}
    ::wxLaunchDefaultBrowser( oFile.GetFullPath() );
}

//---------------------------------------------------------------------------------------
void WelcomeWindow::OnNewScore(wxHyperlinkEvent& WXUNUSED(event))
{
    //TODO 5.0
//    wxCommandEvent myEvent;     //It is not used. So I do not initialize it
//    ::wxPostEvent()
//    g_pMainFrame->OnScoreWizard(myEvent);
}

//---------------------------------------------------------------------------------------
void WelcomeWindow::OnOpenEBooks(wxHyperlinkEvent& WXUNUSED(event))
{
    wxCommandEvent myEvent(wxEVT_COMMAND_MENU_SELECTED, k_menu_open_books);
    ::wxPostEvent(this, myEvent);
}

//---------------------------------------------------------------------------------------
void WelcomeWindow::OnOpenRecent(wxHyperlinkEvent& event)
{
    int i = event.GetId() - lmLINK_Recent1;
    wxCommandEvent myEvent(wxEVT_COMMAND_MENU_SELECTED, wxID_FILE1+i);
    ::wxPostEvent(this, myEvent);
}

//---------------------------------------------------------------------------------------
void WelcomeWindow::OnInstructions(wxHyperlinkEvent& WXUNUSED(event))
{
    wxString sDoc = "study-guide.htm";
    ShowDocument(sDoc);
//    wxCommandEvent myEvent(wxEVT_COMMAND_MENU_SELECTED, k_menu_help_study_guide);
//    ::wxPostEvent(this, myEvent);
}


}   //namespace lenmus
