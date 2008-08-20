//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "WelcomeWnd.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/hyperlink.h"

#include "WelcomeWnd.h"
#include "TheApp.h"         //to get access to locale info.
#include "ArtProvider.h"        // to use ArtProvider for managing icons
//access to MainFrame
#include "../app/MainFrame.h"
extern lmMainFrame* g_pMainFrame;

// Paths names
#include "../globals/Paths.h"
extern lmPaths* g_pPaths;

//IDs for controls
const int lmLINK_NewInLenmus = wxNewId();
const int lmLINK_NewScore = wxNewId();
const int lmLINK_OpenEBooks = wxNewId();
const int lmLINK_Recent1 = wxNewId();
const int lmLINK_Recent2 = wxNewId();
const int lmLINK_Recent3 = wxNewId();
const int lmLINK_Recent4 = wxNewId();
const int lmLINK_Recent5 = wxNewId();
const int lmLINK_Recent6 = wxNewId();
const int lmLINK_Recent7 = wxNewId();
const int lmLINK_Recent8 = wxNewId();
const int lmLINK_Recent9 = wxNewId();


BEGIN_EVENT_TABLE(lmWelcomeWnd, lmMDIChildFrame)
    EVT_HYPERLINK   (lmLINK_NewInLenmus, lmWelcomeWnd::OnNewInLenmus)
    EVT_HYPERLINK   (lmLINK_NewScore, lmWelcomeWnd::OnNewScore)
    EVT_HYPERLINK   (lmLINK_OpenEBooks, lmWelcomeWnd::OnOpenEBooks)
    EVT_HYPERLINK   (lmLINK_Recent1, lmWelcomeWnd::OnOpenRecent)
    EVT_HYPERLINK   (lmLINK_Recent2, lmWelcomeWnd::OnOpenRecent)
    EVT_HYPERLINK   (lmLINK_Recent3, lmWelcomeWnd::OnOpenRecent)
    EVT_HYPERLINK   (lmLINK_Recent4, lmWelcomeWnd::OnOpenRecent)
    EVT_HYPERLINK   (lmLINK_Recent5, lmWelcomeWnd::OnOpenRecent)
    EVT_HYPERLINK   (lmLINK_Recent6, lmWelcomeWnd::OnOpenRecent)
    EVT_HYPERLINK   (lmLINK_Recent7, lmWelcomeWnd::OnOpenRecent)
    EVT_HYPERLINK   (lmLINK_Recent8, lmWelcomeWnd::OnOpenRecent)
    EVT_HYPERLINK   (lmLINK_Recent9, lmWelcomeWnd::OnOpenRecent)
    EVT_CLOSE       (lmWelcomeWnd::OnCloseWindow)
END_EVENT_TABLE()

IMPLEMENT_CLASS(lmWelcomeWnd, lmMDIChildFrame)


lmWelcomeWnd::lmWelcomeWnd(wxWindow* parent, wxWindowID id)
    : lmMDIChildFrame((lmMDIParentFrame*)parent, id, _("Welcome"))
{
    //get recent open files history and get number of files saved
    wxFileHistory* pHistory = g_pMainFrame->GetFileHistory();
    int nRecentFiles = pHistory->GetCount();
	CreateControls(nRecentFiles, pHistory);

    this->SetBackgroundColour(*wxWHITE);

    //in linux, links background must also be changed
	m_pLinkNewInLenmus->SetBackgroundColour(*wxWHITE);
	m_pLinkVisitWebsite->SetBackgroundColour(*wxWHITE);
	m_pLinkOpenEBooks->SetBackgroundColour(*wxWHITE);
	m_pLinkNewScore->SetBackgroundColour(*wxWHITE);
    for (int i=0; i < nRecentFiles; i++)
	    m_pLinkRecent[i]->SetBackgroundColour(*wxWHITE);

    //load icons
    m_pLearnIcon->SetBitmap( wxArtProvider::GetIcon(_T("welcome_news"), wxART_OTHER) );
    m_pScoreIcon->SetBitmap( wxArtProvider::GetIcon(_T("welcome_editor"), wxART_OTHER) );
    m_pPhonascusIcon->SetBitmap( wxArtProvider::GetIcon(_T("welcome_theory"), wxART_OTHER) );
    m_pBmpLeftBanner->SetBitmap( wxArtProvider::GetIcon(_T("welcome_left"), wxART_OTHER) );

    //load recent open files
    pHistory = g_pMainFrame->GetFileHistory();

    this->Refresh();

}

void lmWelcomeWnd::CreateControls(int nRecentFiles, wxFileHistory* pHistory)
{
    //Controls creation for WelcomeWnd

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* pMainSizer;
	pMainSizer = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* pLeftBannerSizer;
	pLeftBannerSizer = new wxBoxSizer( wxHORIZONTAL );

	m_pBmpLeftBanner = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 165,773 ), 0 );
	pLeftBannerSizer->Add( m_pBmpLeftBanner, 0, 0, 5 );

	pMainSizer->Add( pLeftBannerSizer, 0, wxEXPAND, 20 );

	wxBoxSizer* pContentSizer;
	pContentSizer = new wxBoxSizer( wxHORIZONTAL );


	pContentSizer->Add( 0, 0, 1, wxEXPAND, 5 );

	wxBoxSizer* pItemsSizer;
	pItemsSizer = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* pHeadersSizer;
	pHeadersSizer = new wxBoxSizer( wxHORIZONTAL );


	pHeadersSizer->Add( 40, 0, 1, 0, 5 );

	m_pTxtTitle = new wxStaticText( this, wxID_ANY, _("Welcome to LenMus"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pTxtTitle->Wrap( -1 );
	m_pTxtTitle->SetFont( wxFont( 16, 74, 90, 92, false, wxT("Tahoma") ) );

	pHeadersSizer->Add( m_pTxtTitle, 0, wxALL, 5 );


	pHeadersSizer->Add( 40, 0, 1, 0, 5 );

	pItemsSizer->Add( pHeadersSizer, 1, wxEXPAND, 5 );

	wxBoxSizer* pLearnSizer;
	pLearnSizer = new wxBoxSizer( wxVERTICAL );

	m_pLearnTitle = new wxStaticText( this, wxID_ANY, _("Learn about LenMus"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pLearnTitle->Wrap( -1 );
	m_pLearnTitle->SetFont( wxFont( 10, 74, 90, 92, false, wxT("Tahoma") ) );

	pLearnSizer->Add( m_pLearnTitle, 0, wxALL, 5 );

	wxBoxSizer* pLearnSubsizer;
	pLearnSubsizer = new wxBoxSizer( wxHORIZONTAL );

	m_pLearnIcon = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize(45, 45), 0 );
	pLearnSubsizer->Add( m_pLearnIcon, 0, wxALL, 5 );

	wxBoxSizer* pLearnLinksSizer;
	pLearnLinksSizer = new wxBoxSizer( wxVERTICAL );

	m_pLinkNewInLenmus = new wxHyperlinkCtrl( this, lmLINK_NewInLenmus, _("What's is new in LenMus"), wxT("http://www.lenmus.org"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	pLearnLinksSizer->Add( m_pLinkNewInLenmus, 0, wxRIGHT|wxLEFT, 5 );

	m_pLinkVisitWebsite = new wxHyperlinkCtrl( this, wxID_ANY, _("Visit LenMus website"), wxT("http://www.lenmus.org"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	pLearnLinksSizer->Add( m_pLinkVisitWebsite, 0, wxRIGHT|wxLEFT, 5 );

	pLearnSubsizer->Add( pLearnLinksSizer, 1, wxEXPAND|wxLEFT, 10 );

	pLearnSizer->Add( pLearnSubsizer, 1, wxEXPAND, 5 );

	pItemsSizer->Add( pLearnSizer, 0, wxTOP|wxRIGHT|wxLEFT, 20 );

	wxBoxSizer* pPhonascusSizer;
	pPhonascusSizer = new wxBoxSizer( wxVERTICAL );

	m_pPhonascusTitle = new wxStaticText( this, wxID_ANY, _("Learn and practise music theory"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pPhonascusTitle->Wrap( -1 );
	m_pPhonascusTitle->SetFont( wxFont( 10, 74, 90, 92, false, wxT("Tahoma") ) );

	pPhonascusSizer->Add( m_pPhonascusTitle, 0, wxALL, 5 );

	wxBoxSizer* pPhonascusSubsizer;
	pPhonascusSubsizer = new wxBoxSizer( wxHORIZONTAL );

	m_pPhonascusIcon = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize(45, 45), 0 );
	pPhonascusSubsizer->Add( m_pPhonascusIcon, 0, wxALL, 5 );

	wxBoxSizer* pPhonascusLinksSizer;
	pPhonascusLinksSizer = new wxBoxSizer( wxVERTICAL );

	m_pLinkOpenEBooks = new wxHyperlinkCtrl( this, lmLINK_OpenEBooks, _("Open eMusicBooks "), wxT("http://www.lenmus.org"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	pPhonascusLinksSizer->Add( m_pLinkOpenEBooks, 0, wxRIGHT|wxLEFT, 5 );

	pPhonascusSubsizer->Add( pPhonascusLinksSizer, 1, wxEXPAND|wxLEFT, 10 );

	pPhonascusSizer->Add( pPhonascusSubsizer, 1, wxEXPAND, 5 );

	pItemsSizer->Add( pPhonascusSizer, 0, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 20 );

	wxBoxSizer* pScoreSizer;
	pScoreSizer = new wxBoxSizer( wxVERTICAL );

	m_pScoreTitle = new wxStaticText( this, wxID_ANY, _("Create and edit music scores"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pScoreTitle->Wrap( -1 );
	m_pScoreTitle->SetFont( wxFont( 10, 74, 90, 92, false, wxT("Tahoma") ) );

	pScoreSizer->Add( m_pScoreTitle, 0, wxALL, 5 );

	wxBoxSizer* pScoreSubsizer;
	pScoreSubsizer = new wxBoxSizer( wxHORIZONTAL );

	m_pScoreIcon = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize(45, 45), 0 );
	pScoreSubsizer->Add( m_pScoreIcon, 0, wxALL, 5 );

	wxBoxSizer* pScoreLinksSizer;
	pScoreLinksSizer = new wxBoxSizer( wxVERTICAL );

	m_pLinkNewScore = new wxHyperlinkCtrl( this, lmLINK_NewScore, _("New score ..."), wxT("http://www.lenmus.org"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	pScoreLinksSizer->Add( m_pLinkNewScore, 0, wxRIGHT|wxLEFT, 5 );

	wxBoxSizer* pRecentScoresSizer;
	pRecentScoresSizer = new wxBoxSizer( wxVERTICAL );

	m_pRecentScoresTitle = new wxStaticText( this, wxID_ANY, _("Recent scores"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pRecentScoresTitle->Wrap( -1 );
	m_pRecentScoresTitle->SetFont( wxFont( 10, 74, 90, 92, false, wxT("Tahoma") ) );

	pRecentScoresSizer->Add( m_pRecentScoresTitle, 0, wxRIGHT|wxLEFT, 5 );

	wxBoxSizer* pRecentScoresLinksSizer;
	pRecentScoresLinksSizer = new wxBoxSizer( wxHORIZONTAL );


	pRecentScoresLinksSizer->Add( 40, 0, 0, 0, 5 );

	wxBoxSizer* pRecentScoresLinksSizer1;
	pRecentScoresLinksSizer1 = new wxBoxSizer( wxVERTICAL );

    for (int i=0; i < nRecentFiles; i++)
    {
	    m_pLinkRecent[i] =
            new wxHyperlinkCtrl(this, lmLINK_Recent1+i, pHistory->GetHistoryFile(i),
                                pHistory->GetHistoryFile(i), wxDefaultPosition, wxDefaultSize,
                                wxHL_DEFAULT_STYLE);
	    pRecentScoresLinksSizer1->Add( m_pLinkRecent[i], 0, wxRIGHT|wxLEFT, 5 );
    }

	pRecentScoresLinksSizer->Add( pRecentScoresLinksSizer1, 1, wxEXPAND, 5 );

	pRecentScoresSizer->Add( pRecentScoresLinksSizer, 0, wxEXPAND, 5 );

	pScoreLinksSizer->Add( pRecentScoresSizer, 1, wxEXPAND|wxTOP, 5 );

	pScoreSubsizer->Add( pScoreLinksSizer, 1, wxEXPAND|wxLEFT, 10 );

	pScoreSizer->Add( pScoreSubsizer, 1, wxEXPAND, 5 );

	pItemsSizer->Add( pScoreSizer, 0, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 20 );

	pContentSizer->Add( pItemsSizer, 6, 0, 5 );


	pContentSizer->Add( 0, 0, 1, wxEXPAND, 5 );

	pMainSizer->Add( pContentSizer, 1, wxEXPAND|wxTOP, 20 );

	this->SetSizer( pMainSizer );
	this->Layout();
}

lmWelcomeWnd::~lmWelcomeWnd()
{
}

void lmWelcomeWnd::OnNewInLenmus(wxHyperlinkEvent& event)
{
    wxString sPath = g_pPaths->GetRootPath();
    wxFileName oFile;
    oFile.SetPath(sPath);
    oFile.AppendDir(_T("docs"));
    oFile.SetFullName(_T("release_notes.htm"));
    ::wxLaunchDefaultBrowser( oFile.GetFullPath() );
}

void lmWelcomeWnd::OnNewScore(wxHyperlinkEvent& event)
{
    wxCommandEvent myEvent;     //It is not used. So I do not initialize it
    g_pMainFrame->OnScoreWizard(myEvent);
}

void lmWelcomeWnd::OnOpenEBooks(wxHyperlinkEvent& event)
{
    wxCommandEvent myEvent;     //It is not used. So I do not initialize it
    g_pMainFrame->OnOpenBook(myEvent);
}

void lmWelcomeWnd::OnOpenRecent(wxHyperlinkEvent& event)
{
    wxString sFile = m_pLinkRecent[event.GetId() - lmLINK_Recent1]->GetURL();
    g_pMainFrame->OpenRecentFile(sFile);
}

void lmWelcomeWnd::OnCloseWindow(wxCloseEvent& event)
{
    //inform parent
    g_pMainFrame->OnCloseWelcomeWnd();

    event.Skip();   //allow to continue
}


