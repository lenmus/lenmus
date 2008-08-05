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
extern lmMainFrame *g_pMainFrame;


//IDs for controls
const int lmLINK_NewInLenmus = wxNewId();
const int lmLINK_NewScore = wxNewId();
const int lmLINK_OpenEBooks = wxNewId();
const int lmLINK_Recent1 = wxNewId();
const int lmLINK_Recent2 = wxNewId();


BEGIN_EVENT_TABLE(lmWelcomeWnd, lmMDIChildFrame)
    EVT_HYPERLINK   (lmLINK_NewInLenmus, lmWelcomeWnd::OnNewInLenmus)
    EVT_HYPERLINK   (lmLINK_NewScore, lmWelcomeWnd::OnNewScore)
    EVT_HYPERLINK   (lmLINK_OpenEBooks, lmWelcomeWnd::OnOpenEBooks)
    EVT_HYPERLINK   (lmLINK_Recent1, lmWelcomeWnd::OnOpenRecent)
    EVT_HYPERLINK   (lmLINK_Recent2, lmWelcomeWnd::OnOpenRecent)
    EVT_CLOSE       (lmWelcomeWnd::OnCloseWindow)
END_EVENT_TABLE()

IMPLEMENT_CLASS(lmWelcomeWnd, lmMDIChildFrame)


lmWelcomeWnd::lmWelcomeWnd(wxWindow* parent, wxWindowID id)
    : lmMDIChildFrame((lmMDIParentFrame*)parent, id, _("Welcome"))
{
	CreateControls();

    this->SetBackgroundColour(*wxWHITE);

    //in linux, links background must also be changed
	m_pLinkNewInLenmus->SetBackgroundColour(*wxWHITE);
	m_pLinkVisitWebsite->SetBackgroundColour(*wxWHITE);
	m_pLinkOpenEBooks->SetBackgroundColour(*wxWHITE);
	m_pLinkNewScore->SetBackgroundColour(*wxWHITE);
	m_pLinkRecent1->SetBackgroundColour(*wxWHITE);
	m_pLinkRecent2->SetBackgroundColour(*wxWHITE);

    //load icons
    m_pLearnIcon->SetBitmap( wxArtProvider::GetIcon(_T("welcome_news"), wxART_OTHER) );
    m_pScoreIcon->SetBitmap( wxArtProvider::GetIcon(_T("welcome_editor"), wxART_OTHER) );
    m_pPhonascusIcon->SetBitmap( wxArtProvider::GetIcon(_T("welcome_theory"), wxART_OTHER) );
    m_pBmpLeftBanner->SetBitmap( wxArtProvider::GetIcon(_T("welcome_left"), wxART_OTHER) );

	this->Layout();
}

void lmWelcomeWnd::CreateControls()
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

	m_pLearnIcon = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	pLearnSubsizer->Add( m_pLearnIcon, 0, wxALL, 5 );

	wxBoxSizer* pLearnLinksSizer;
	pLearnLinksSizer = new wxBoxSizer( wxVERTICAL );

	m_pLinkNewInLenmus = new wxHyperlinkCtrl( this, lmLINK_NewInLenmus, _("What's is new in LenMus"), wxT("http://www.wxformbuilder.org"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
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

	m_pPhonascusIcon = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	pPhonascusSubsizer->Add( m_pPhonascusIcon, 0, wxALL, 5 );

	wxBoxSizer* pPhonascusLinksSizer;
	pPhonascusLinksSizer = new wxBoxSizer( wxVERTICAL );

	m_pLinkOpenEBooks = new wxHyperlinkCtrl( this, lmLINK_OpenEBooks, _("Open eMusicBooks "), wxT("http://www.wxformbuilder.org"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
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

	m_pScoreIcon = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	pScoreSubsizer->Add( m_pScoreIcon, 0, wxALL, 5 );

	wxBoxSizer* pScoreLinksSizer;
	pScoreLinksSizer = new wxBoxSizer( wxVERTICAL );

	m_pLinkNewScore = new wxHyperlinkCtrl( this, lmLINK_NewScore, _("New score ..."), wxT("http://www.wxformbuilder.org"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
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

	m_pLinkRecent1 = new wxHyperlinkCtrl( this, lmLINK_Recent1, _("c://usr/docs/Bach.lms"), wxT("http://www.wxformbuilder.org"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	pRecentScoresLinksSizer1->Add( m_pLinkRecent1, 0, wxRIGHT|wxLEFT, 5 );

	m_pLinkRecent2 = new wxHyperlinkCtrl( this, lmLINK_Recent2, _("d://scores/Chopin prelude.lms"), wxT("http://www.wxformbuilder.org"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	pRecentScoresLinksSizer1->Add( m_pLinkRecent2, 0, wxRIGHT|wxLEFT, 5 );

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
    wxMessageBox(_T("Evento no tratado"));
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
    wxMessageBox(_T("Evento no tratado"));
}

void lmWelcomeWnd::OnCloseWindow(wxCloseEvent& event)
{
    //inform parent
    g_pMainFrame->OnCloseWelcomeWnd();

    event.Skip();   //allow to continue
}


