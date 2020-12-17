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
#if (0)   //0=old windows , 1=new window
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


#else
//lenmus
#include "lenmus_welcome_window.h"
#include "lenmus_standard_header.h"

#include "lenmus_paths.h"
#include "lenmus_main_frame.h"
#include "lenmus_string.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
//#include <wx/hyperlink.h>
#include <wx/panel.h>
//#include <wx/docview.h>      //for wxFileHistory
#include <wx/artprov.h>


namespace lenmus
{

//IDs for controls
const int k_id_exercises = wxNewId();
const int k_id_book_1 = wxNewId();
const int k_id_book_2 = wxNewId();
const int k_id_book_3 = wxNewId();
const int k_id_study_guide = wxNewId();



wxBEGIN_EVENT_TABLE(WelcomeWindow, wxScrolledWindow)
    EVT_BUTTON(k_id_exercises, WelcomeWindow::on_button_exercises)
    EVT_BUTTON(k_id_book_1, WelcomeWindow::on_button_book_1)
    EVT_BUTTON(k_id_book_2, WelcomeWindow::on_button_book_2)
    EVT_BUTTON(k_id_book_3, WelcomeWindow::on_button_book_3)
    EVT_BUTTON(k_id_study_guide, WelcomeWindow::on_button_study_guide)
wxEND_EVENT_TABLE()

//---------------------------------------------------------------------------------------
WelcomeWindow::WelcomeWindow(ContentWindow* parent, ApplicationScope& appScope,
                             wxWindowID id)
    : wxScrolledWindow(parent, id, wxDefaultPosition, wxSize(1000, 700), wxHW_SCROLLBAR_AUTO | wxHSCROLL | wxVSCROLL)
    , CanvasInterface(parent)
    , m_appScope(appScope)
{
    SetVirtualSize(1000, 750);      //1000 x (770-20) px
    SetScrollRate(20, 20);          //20px steps

	create_controls();

    //load icons
    m_logoUCA->SetBitmap( wxArtProvider::GetIcon("logoUCA", wxART_OTHER) );

    m_logo->SetBitmap( wxArtProvider::GetIcon("logo50x67", wxART_OTHER) );
    m_bmpGuide->SetBitmap( wxArtProvider::GetIcon("welcome_find", wxART_OTHER) );
    m_bmpTrain->SetBitmap( wxArtProvider::GetIcon("welcome_practice", wxART_OTHER) );
//    m_bmpCollaborate->SetBitmap( wxArtProvider::GetIcon("welcome_colaborate", wxART_OTHER) );
    m_bmpBooks->SetBitmap( wxArtProvider::GetIcon("welcome_book", wxART_OTHER) );
}

//---------------------------------------------------------------------------------------
void WelcomeWindow::create_controls()
{
    //content generated with wxFormBuilder (x_unversioned(StartPage.fbp). Modified to:
    // - add version number to title
    // - add loop to load recent files

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );

	wxBoxSizer* sizerMain;
	sizerMain = new wxBoxSizer( wxVERTICAL );

	m_logo = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( -1,-1 ), 0 );
	sizerMain->Add( m_logo, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 30 );


	m_txtWelcome = new wxStaticText( this, wxID_ANY, _("Welcome to LenMus"), wxDefaultPosition, wxDefaultSize, 0 );
	m_txtWelcome->Wrap( -1 );
	m_txtWelcome->SetFont( wxFont( 24, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	sizerMain->Add( m_txtWelcome, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP|wxRIGHT|wxLEFT, 5 );

    wxString version = m_appScope.get_version_string();
    wxString title = wxString::Format(_("Version %s"), version.wx_str());
	m_txtVersion = new wxStaticText( this, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, 0 );
	m_txtVersion->Wrap( -1 );
	sizerMain->Add( m_txtVersion, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM|wxRIGHT|wxLEFT, 5 );

	m_txtWhatToDo = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_txtWhatToDo->Wrap( -1 );
	m_txtWhatToDo->SetFont( wxFont( 14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );
	m_txtWhatToDo->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_GRAYTEXT ) );

	sizerMain->Add( m_txtWhatToDo, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );


//	sizerMain->Add( 0, 0, 1, wxEXPAND, 5 );

    //content area
	wxBoxSizer* sizerContent;
	sizerContent = new wxBoxSizer( wxHORIZONTAL );

    //margin at left
	sizerContent->Add( 0, 0, 1, wxEXPAND, 5 );
//	sizerContent->Add( 60, 0, 0, 0, 5 );

	//area for left content
	wxBoxSizer* sizerLeft;
	sizerLeft = new wxBoxSizer( wxVERTICAL );


	//Guide block
	wxBoxSizer* sizerGuideBox;
	sizerGuideBox = new wxBoxSizer( wxHORIZONTAL );

	m_bmpGuide = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	sizerGuideBox->Add( m_bmpGuide, 0, wxALL, 5 );

	wxBoxSizer* sizerGuide;
	sizerGuide = new wxBoxSizer( wxVERTICAL );

	m_txtGuide = new wxStaticText( this, wxID_ANY, _("Get started"), wxDefaultPosition, wxDefaultSize, 0 );
	m_txtGuide->Wrap( -1 );
	m_txtGuide->SetFont( wxFont( 14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	sizerGuide->Add( m_txtGuide, 0, wxALL|wxEXPAND, 5 );

	m_txtGuideText = new wxStaticText( this, wxID_ANY, _("Read about how to get the most out of LenMus."), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_txtGuideText->Wrap( 350 );
	m_txtGuideText->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INACTIVECAPTIONTEXT ) );

	sizerGuide->Add( m_txtGuideText, 0, wxALL, 5 );

	m_btnGuide = new wxButton( this, k_id_study_guide, _("Start"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerGuide->Add( m_btnGuide, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

	sizerGuideBox->Add( sizerGuide, 0, wxALL, 5 );
    sizerLeft->Add( sizerGuideBox, 0, 0, 5 );

	//End of Guide block


    sizerLeft->Add( 0, 60, 0, 0, 5 );       //space between this block and next block


	//Block: Practice
	wxBoxSizer* sizerTrainBox;
	sizerTrainBox = new wxBoxSizer( wxHORIZONTAL );

	m_bmpTrain = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	sizerTrainBox->Add( m_bmpTrain, 0, wxALL, 5 );

	wxBoxSizer* sizerTrain;
	sizerTrain = new wxBoxSizer( wxVERTICAL );

	m_txtTrain = new wxStaticText( this, wxID_ANY, _("Train with LenMus"), wxDefaultPosition, wxDefaultSize, 0 );
	m_txtTrain->Wrap( -1 );
	m_txtTrain->SetFont( wxFont( 14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	sizerTrain->Add( m_txtTrain, 0, wxALL, 5 );

	m_txtTrainText = new wxStaticText( this, wxID_ANY, _("Develop your musical skills with progressive exercises that will help you assimilate the different contents. Set your own learning pace."), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_txtTrainText->Wrap( 350 );
	m_txtTrainText->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INACTIVECAPTIONTEXT ) );

	sizerTrain->Add( m_txtTrainText, 0, wxALL, 5 );

	m_btnTrain = new wxButton( this, k_id_exercises, _("Start"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerTrain->Add( m_btnTrain, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

	sizerTrainBox->Add( sizerTrain, 0, wxALL, 5 );
	sizerLeft->Add( sizerTrainBox, 0, 0, 5 );


	//End of Practice block


	sizerContent->Add( sizerLeft, 0, 0, 5 );

	sizerContent->Add( 60, 0, 0, 0, 5 );

	//****** End of left block *****************


    //area for right content
	wxBoxSizer* sizerRight;
	sizerRight = new wxBoxSizer( wxVERTICAL );

	m_bmpBooks = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 128,-1 ), 0 );
	sizerRight->Add( m_bmpBooks, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

	m_txtBooks = new wxStaticText( this, wxID_ANY, _("Study and practice"), wxDefaultPosition, wxDefaultSize, 0 );
	m_txtBooks->Wrap( -1 );
	m_txtBooks->SetFont( wxFont( 14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	sizerRight->Add( m_txtBooks, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

	m_txtBooksText = new wxStaticText( this, wxID_ANY, _("Study with the interactive LenMus music theory books. Start with the first of these two:"), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_txtBooksText->Wrap( 340 );
	m_txtBooksText->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INACTIVECAPTIONTEXT ) );

	sizerRight->Add( m_txtBooksText, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

	wxFlexGridSizer* sizerGridBooks;
	sizerGridBooks = new wxFlexGridSizer( 3, 2, 0, 0 );
	sizerGridBooks->SetFlexibleDirection( wxHORIZONTAL );
	sizerGridBooks->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_txtLevel1 = new wxStaticText( this, wxID_ANY, _("Elementos del Lenguaje Musical I"), wxDefaultPosition, wxDefaultSize, 0 );
	m_txtLevel1->Wrap( -1 );
	m_txtLevel1->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );

	sizerGridBooks->Add( m_txtLevel1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_btnLevel1 = new wxButton( this, k_id_book_1, _("Start"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerGridBooks->Add( m_btnLevel1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_txtLevel2 = new wxStaticText( this, wxID_ANY, _("Elementos del Lenguaje Musical II"), wxDefaultPosition, wxDefaultSize, 0 );
	m_txtLevel2->Wrap( -1 );
	m_txtLevel2->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );

	sizerGridBooks->Add( m_txtLevel2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_btnLevel2 = new wxButton( this, k_id_book_2, _("Start"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerGridBooks->Add( m_btnLevel2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_txtLevel3 = new wxStaticText( this, wxID_ANY, wxT("Ejercicios de Lectura Rítmica"), wxDefaultPosition, wxDefaultSize, 0 );
	m_txtLevel3->Wrap( -1 );
	m_txtLevel3->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );

	sizerGridBooks->Add( m_txtLevel3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_btnLevel3 = new wxButton( this, k_id_book_3, _("Start"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerGridBooks->Add( m_btnLevel3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );


	sizerRight->Add( sizerGridBooks, 0, wxALIGN_CENTER_HORIZONTAL, 5 );


	//Block: UCA Logo
    sizerRight->Add( 0, 140, 0, 0, 5 );       //space between this block and next block

	wxBoxSizer* sizerUCABox;
	sizerUCABox = new wxBoxSizer( wxHORIZONTAL );

	sizerUCABox->Add( 0, 0, 1, 0, 5 );  //spacer, to align right

	m_txtUCA = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	//m_txtUCA->SetLabel( "\n" + _("Colabora:") );
	m_txtUCA->SetLabel( "\n" + _("Acknowledgments:") );
	m_txtUCA->Wrap( -1 );
	m_txtUCA->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );
	sizerUCABox->Add( m_txtUCA, 0, wxALL, 5 );

	m_logoUCA = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	sizerUCABox->Add( m_logoUCA, 0, wxALL, 5 );

	sizerRight->Add( sizerUCABox, 1, wxEXPAND, 5 );




	    // close all

	sizerContent->Add( sizerRight, 0, 0, 5 );

    //margin at right
	sizerContent->Add( 0, 0, 1, wxEXPAND, 5 );

	sizerMain->Add( sizerContent, 0, wxEXPAND, 5 );



	this->SetSizer( sizerMain );
	this->Layout();
}

//---------------------------------------------------------------------------------------
WelcomeWindow::~WelcomeWindow()
{
}

//---------------------------------------------------------------------------------------
void WelcomeWindow::on_button_exercises(wxCommandEvent& UNUSED(event))
{
    open_book("GeneralExercises.lmb");
}

//---------------------------------------------------------------------------------------
void WelcomeWindow::on_button_book_1(wxCommandEvent& UNUSED(event))
{
    open_book("L1_MusicReading.lmb");
}

//---------------------------------------------------------------------------------------
void WelcomeWindow::on_button_book_2(wxCommandEvent& UNUSED(event))
{
    open_book("L2_MusicReading.lmb");
}

//---------------------------------------------------------------------------------------
void WelcomeWindow::on_button_book_3(wxCommandEvent& UNUSED(event))
{
    open_book("TheoryHarmony.lmb");
}

//---------------------------------------------------------------------------------------
void WelcomeWindow::open_book(const wxString& filename)
{
    Paths* pPaths = m_appScope.get_paths();
    wxString sPath = pPaths->GetBooksPath();
    wxFileName oFile(sPath, filename, wxPATH_NATIVE);
    if (!oFile.FileExists())
    {
        //try to use the english version
        sPath = pPaths->GetLocaleRootPath();
        oFile.AssignDir(sPath);
        oFile.AppendDir("en");
        oFile.AppendDir("books");
        oFile.SetFullName(filename);
        if (!oFile.FileExists())
        {
            wxMessageBox(_("Sorry: File not found!"));
            LOMSE_LOG_WARN("File '%s' not found!",
                           oFile.GetFullPath().ToStdString().c_str() );
            return;
        }
    }

    wxCommandEvent myEvent(LM_EVT_OPEN_BOOK, k_id_open_book);
    myEvent.SetString( oFile.GetFullPath() );
    ::wxPostEvent(this, myEvent);
}

//---------------------------------------------------------------------------------------
void WelcomeWindow::on_button_study_guide(wxCommandEvent& UNUSED(event))
{
    wxString sDoc = "study-guide.htm";
    show_document(sDoc);
}

//---------------------------------------------------------------------------------------
void WelcomeWindow::show_document(const wxString& sDocName)
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
            wxLogMessage("[WelcomeWindow::show_document] File %s' not found!",
                         oFile.GetFullPath().wx_str() );
            return;
        }
	}
    ::wxLaunchDefaultBrowser( oFile.GetFullPath() );
}


#endif  //old or new version

}   //namespace lenmus
