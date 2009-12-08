//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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
#pragma implementation "CountersAuxCtrol.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include <wx/artprov.h>

#include "CountersAuxCtrol.h"
#include "UrlAuxCtrol.h"
#include "../../sound/WaveManager.h"
#include "../Generators.h"
#include "../../app/DlgDebug.h"

//access to global flag
extern bool g_fAnswerSoundsEnabled;


//IDs for controls
enum {
    lmID_LINK_RESET_COUNTERS = 3600,
	lmID_LINK_EXPLAIN,
};

//--------------------------------------------------------------------------------
// lmCountersAuxCtrol implementation
// An abstract control to embed in html exercises to display statistics about
// user performance in doing the exercise
//--------------------------------------------------------------------------------

lmCountersAuxCtrol::lmCountersAuxCtrol(wxWindow* parent, wxWindowID id,  double rScale,
                                       const wxPoint& pos, const wxSize& size)
    : wxPanel(parent, id, pos, size, wxBORDER_NONE)
      , m_rScale(rScale)
{
    //set background white (needed in Linux)
    this->SetBackgroundColour(*wxWHITE);
}

lmCountersAuxCtrol::~lmCountersAuxCtrol()
{
}

void lmCountersAuxCtrol::RightWrongSound(bool fSuccess)
{
    if (g_fAnswerSoundsEnabled)
    {
        lmWaveManager* pWave = lmWaveManager::GetInstance();
        if (fSuccess)
            pWave->RightAnswerSound();
        else
            pWave->WrongAnswerSound();
    }
}



//--------------------------------------------------------------------------------
// lmQuizAuxCtrol implementation
// A control to embed in html exercises to display the number of right and
// wrong student answers, in current session, and the total score (percentage)
//--------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(lmQuizAuxCtrol, lmCountersAuxCtrol)
    LM_EVT_URL_CLICK(lmID_LINK_RESET_COUNTERS, lmQuizAuxCtrol::OnResetCounters)
END_EVENT_TABLE()


lmQuizAuxCtrol::lmQuizAuxCtrol(wxWindow* parent, wxWindowID id, int nNumTeams, double rScale,
                               lmQuizManager* pProblemMngr, const wxPoint& pos)
    : lmCountersAuxCtrol(parent, id, rScale, pos)
      , m_pProblemMngr(pProblemMngr)
      , m_fTwoTeamsMode(nNumTeams == 2)
{
    wxASSERT(nNumTeams == 1 || nNumTeams == 2);

    // Create the controls
    this->SetBackgroundColour(*wxWHITE);
    wxBoxSizer* pMainSizer = new wxBoxSizer(wxVERTICAL);
    m_pProblemMngr->SetNumTeams(nNumTeams);
    for(int i=0; i < nNumTeams; i++)
        CreateCountersGroup(i, pMainSizer);

    //load icons
    wxBitmap bmp = wxArtProvider::GetBitmap(_T("button_accept"), wxART_TOOLBAR, wxSize(24,24));
    m_pBmpRight->SetBitmap(bmp);
    bmp = wxArtProvider::GetBitmap(_T("button_cancel"), wxART_TOOLBAR, wxSize(24,24));
    m_pBmpWrong->SetBitmap(bmp);
    bmp = wxArtProvider::GetBitmap(_T("diploma_cap"), wxART_TOOLBAR, wxSize(35,24));
    m_pBmpTotal->SetBitmap(bmp);
    if (m_fTwoTeamsMode)
    {
        m_bmpRed = wxArtProvider::GetBitmap(_T("team_red"), wxART_TOOLBAR, wxSize(24,24));
        m_pBmpTeam[0]->SetBitmap(m_bmpRed);
        m_bmpBlue = wxArtProvider::GetBitmap(_T("team_blue"), wxART_TOOLBAR, wxSize(24,24));
        m_pBmpTeam[1]->SetBitmap(m_bmpBlue);
        m_bmpGrey = wxArtProvider::GetBitmap(_T("team_grey"), wxART_TOOLBAR, wxSize(24,24));
    }

    //'reset counters' link
    pMainSizer->Add(
        new lmUrlAuxCtrol(this, lmID_LINK_RESET_COUNTERS, rScale, _("Reset counters"),
                          _T("link_reset") ),
        0, wxALIGN_CENTER_HORIZONTAL|wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 5);

    //set main window sizer
    SetSizer( pMainSizer );                 // use the sizer for window layout
    pMainSizer->SetSizeHints( this );       // set size hints to honour minimum size

    Layout();
    for (int i=0; i < nNumTeams; i++)
        UpdateDisplays(i);
}

lmQuizAuxCtrol::~lmQuizAuxCtrol()
{
}

void lmQuizAuxCtrol::CreateCountersGroup(int nTeam, wxBoxSizer* pMainSizer)
{
    // Create the controls for counter group nTeam (0...n)

    wxBoxSizer* pCountersSizer = new wxBoxSizer(wxHORIZONTAL);
    pMainSizer->Add(pCountersSizer, 0, wxALIGN_CENTER_HORIZONTAL, 5);

    // Font sizes
    int nNormalSize = (int)(m_rScale * 8.0);      // 8pt, scaled
    int nBigSize = (int)(m_rScale * 12.0);      // 12pt, scaled

    // Boxes sizes and spacing
    int nBoxSize = (int)(m_rScale * 50.0);      // 50px, scaled
    int nSpacing = (int)(m_rScale * 5.0);       // 5px, scaled

    // Team label
    if (m_fTwoTeamsMode)
    {
        wxBoxSizer* pTeamSizer = new wxBoxSizer(wxVERTICAL);
        pCountersSizer->Add(pTeamSizer, 0, wxALIGN_CENTER_VERTICAL, nSpacing);

        //if first team add headers (spacer)
        if (nTeam == 0)
	        pTeamSizer->Add( 0, 24, 0, wxEXPAND, nSpacing );       //spacer

        //add team icon
	    m_pBmpTeam[nTeam] = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	    pTeamSizer->Add( m_pBmpTeam[nTeam], 0, wxRIGHT, nSpacing );
        m_pBmpTeam[nTeam]->SetToolTip(
            (nTeam == 0 ? _("Counters for Team A") : _("Counters for Team B")) );
    }

    //display for right answers
    wxBoxSizer* pRightSizer = new wxBoxSizer(wxVERTICAL);
    pCountersSizer->Add(pRightSizer, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM, nSpacing);

    if (nTeam == 0)
    {
	    m_pBmpRight = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	    pRightSizer->Add( m_pBmpRight, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM|wxRIGHT|wxLEFT, nSpacing );
        m_pBmpRight->SetToolTip(_("Right answers counter"));
    }
    m_pRightCounter[nTeam] = new wxStaticText( this, wxID_STATIC, _T(""),
		wxDefaultPosition, wxSize(nBoxSize, -1),
		wxALIGN_CENTRE|wxST_NO_AUTORESIZE|wxSIMPLE_BORDER );
	m_pRightCounter[nTeam]->Wrap( -1 );
    m_pRightCounter[nTeam]->SetBackgroundColour(wxColour(255, 255, 255));
    m_pRightCounter[nTeam]->SetFont(wxFont(nBigSize, wxSWISS, wxNORMAL, wxBOLD, false, _T("")));
    m_pRightCounter[nTeam]->SetToolTip(_("Right answers counter"));
    pRightSizer->Add(m_pRightCounter[nTeam], 0,
		wxALIGN_CENTER_HORIZONTAL|wxADJUST_MINSIZE, 0);

    //display for wrong answers
    wxBoxSizer* pWrongSizer = new wxBoxSizer(wxVERTICAL);
    pCountersSizer->Add(pWrongSizer, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM, nSpacing);

    if (nTeam == 0)
    {
	    m_pBmpWrong = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	    pWrongSizer->Add( m_pBmpWrong, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM|wxRIGHT|wxLEFT, nSpacing );
        m_pBmpWrong->SetToolTip(_("Wrong answers counter"));
    }
    m_pWrongCounter[nTeam] = new wxStaticText( this, wxID_STATIC, _T(""),
		wxDefaultPosition, wxSize(nBoxSize, -1),
		wxALIGN_CENTRE|wxBORDER_SIMPLE|wxST_NO_AUTORESIZE );
    m_pWrongCounter[nTeam]->SetBackgroundColour(wxColour(255, 255, 255));
    m_pWrongCounter[nTeam]->SetFont(wxFont(nBigSize, wxSWISS, wxNORMAL, wxBOLD, false, _T("")));
    m_pWrongCounter[nTeam]->SetToolTip(_("Wrong answers counter"));
    pWrongSizer->Add(m_pWrongCounter[nTeam], 0,
		wxALIGN_CENTER_HORIZONTAL|wxADJUST_MINSIZE, 0);

    //display for total score
    wxBoxSizer* pTotalSizer = new wxBoxSizer(wxVERTICAL);
    pCountersSizer->Add(pTotalSizer, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM, nSpacing);

    if (nTeam == 0)
    {
	    m_pBmpTotal = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	    pTotalSizer->Add( m_pBmpTotal, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM|wxRIGHT|wxLEFT, nSpacing );
        m_pBmpTotal->SetToolTip(_("Total: your marks"));
    }
    m_pTotalCounter[nTeam] = new wxStaticText( this, wxID_STATIC, _T(""),
		wxDefaultPosition, wxSize(nBoxSize, -1),
		wxALIGN_CENTRE|wxBORDER_SIMPLE|wxST_NO_AUTORESIZE );
    m_pTotalCounter[nTeam]->SetBackgroundColour(wxColour(255, 255, 255));
    m_pTotalCounter[nTeam]->SetFont(wxFont(nBigSize, wxSWISS, wxNORMAL, wxBOLD, false, _T("Arial")));
    m_pTotalCounter[nTeam]->SetToolTip(_("Total: your marks"));
    pTotalSizer->Add(m_pTotalCounter[nTeam], 0,
		wxALIGN_CENTER_HORIZONTAL|wxADJUST_MINSIZE, 0);

}

void lmQuizAuxCtrol::UpdateDisplay()
{
    UpdateDisplays( m_pProblemMngr->GetCurrentTeam() );
}

void lmQuizAuxCtrol::OnResetCounters(wxCommandEvent& WXUNUSED(event))
{
    m_pProblemMngr->ResetCounters();
    for (int i=0; i < m_pProblemMngr->GetNumTeams(); i++)
        UpdateDisplays(i);
}

void lmQuizAuxCtrol::UpdateDisplays(int nTeam)
{
    int nRight = m_pProblemMngr->GetRight(nTeam);
    int nWrong = m_pProblemMngr->GetWrong(nTeam);

    //update display for right answers
    m_pRightCounter[nTeam]->SetLabel( wxString::Format(_T("%d"), nRight) );

    //update display for wrong answers
    m_pWrongCounter[nTeam]->SetLabel( wxString::Format(_T("%d"), nWrong) );

    //update display for total score
    int nTotal = nRight + nWrong;
    if (nTotal == 0)
        m_pTotalCounter[nTeam]->SetLabel( _T("-") );
    else
    {
        float rScore = 10.0f * (float)nRight / (float)nTotal;
        m_pTotalCounter[nTeam]->SetLabel( wxString::Format(_T("%.01f"), rScore) );
    }
    Refresh();
}

void lmQuizAuxCtrol::NextTeam()
{
    m_pProblemMngr->NextTeam();

    //update label
    if (m_fTwoTeamsMode)
    {
        if (m_pProblemMngr->GetCurrentTeam() == 0)
        {
            m_pBmpTeam[0]->SetBitmap(m_bmpRed);
            m_pBmpTeam[1]->SetBitmap(m_bmpGrey);
        }
        else
        {
            m_pBmpTeam[0]->SetBitmap(m_bmpGrey);
            m_pBmpTeam[1]->SetBitmap(m_bmpBlue);
        }
    }
}

void lmQuizAuxCtrol::OnNewQuestion()
{
    NextTeam();
}




//--------------------------------------------------------------------------------
// lmLeitnerAuxCtrol implementation
// A control to embed in html exercises to display statistics
// on user performance in learning the subject. It uses the Leitner system of
// spaced repetitions
// -------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(lmLeitnerAuxCtrol, lmCountersAuxCtrol)
    LM_EVT_URL_CLICK(lmID_LINK_EXPLAIN, lmLeitnerAuxCtrol::OnExplainProgress)
END_EVENT_TABLE()


lmLeitnerAuxCtrol::lmLeitnerAuxCtrol(wxWindow* parent, wxWindowID id, double rScale,
                                     lmLeitnerManager* pProblemMngr,
                                     const wxPoint& pos)
    : lmCountersAuxCtrol(parent, id, rScale, pos, wxSize(220, 130))
{
    //initializations
    m_pProblemMngr = pProblemMngr;

    // Create the controls
    CreateControls();
}

lmLeitnerAuxCtrol::~lmLeitnerAuxCtrol()
{
}

void lmLeitnerAuxCtrol::CreateControls()
{
    //AWARE: Code created with wxFormBuilder and copied here.
    //Modifications near lin 357: replacement of m_pLinkExplain (wxStaticTxt) by
    //an lmUrlAuxCtrol

	wxBoxSizer* m_pDataSizer;
	m_pDataSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* m_pFirstLineSizer;
	m_pFirstLineSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_pTxtNumQuestions = new wxStaticText( this, wxID_ANY, _T("36 / 0"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
	m_pTxtNumQuestions->Wrap( -1 );
	m_pTxtNumQuestions->SetFont( wxFont( 8, 74, 90, 90, false, wxT("Arial") ) );
	
	m_pFirstLineSizer->Add( m_pTxtNumQuestions, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
	
	m_pFirstLineSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_pLblEST = new wxStaticText( this, wxID_ANY, _T("EST:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pLblEST->Wrap( -1 );
	m_pLblEST->SetFont( wxFont( 8, 74, 90, 92, false, wxT("Tahoma") ) );
	
	m_pFirstLineSizer->Add( m_pLblEST, 0, wxRIGHT|wxLEFT, 5 );
	
	m_pTxtTime = new wxStaticText( this, wxID_ANY, _T("1h:27m"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
	m_pTxtTime->Wrap( -1 );
	m_pTxtTime->SetFont( wxFont( 8, 74, 90, 90, false, wxT("Arial") ) );
	
	m_pFirstLineSizer->Add( m_pTxtTime, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	
	m_pFirstLineSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
    //FOLLOWING LINES REPLACED------------------------------
	//m_pLinkExplain = new wxStaticText( this, lmID_LINK_EXPLAIN, _("?"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	//m_pLinkExplain->Wrap( -1 );
	//m_pLinkExplain->SetFont( wxFont( 8, 74, 90, 90, true, wxT("Tahoma") ) );
	//m_pLinkExplain->SetForegroundColour( wxColour( 0, 0, 255 ) );
	//
	//m_pFirstLineSizer->Add( m_pLinkExplain, 0, wxRIGHT|wxLEFT, 5 );
    m_pFirstLineSizer->Add(
        new lmUrlAuxCtrol(this, lmID_LINK_EXPLAIN, m_rScale, _T("?"), lmNO_BITMAP),
        0, wxRIGHT|wxLEFT, 5);
    //END OF MODIFICATION----------------------------------
	
	m_pDataSizer->Add( m_pFirstLineSizer, 0, wxEXPAND, 5 );
	
	wxFlexGridSizer* m_pGridSizer;
	m_pGridSizer = new wxFlexGridSizer( 2, 3, 0, 0 );
	m_pGridSizer->SetFlexibleDirection( wxHORIZONTAL );
	m_pGridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_pLblSession = new wxStaticText( this, wxID_ANY, _("Session:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pLblSession->Wrap( -1 );
	m_pLblSession->SetFont( wxFont( 8, 74, 90, 90, false, wxT("Arial") ) );
	
	m_pGridSizer->Add( m_pLblSession, 0, wxLEFT, 5 );
	
	m_pTxtSession = new wxStaticText( this, wxID_ANY, _T("0.0%"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pTxtSession->Wrap( -1 );
	m_pTxtSession->SetFont( wxFont( 8, 74, 90, 90, false, wxT("Arial") ) );
	
	m_pGridSizer->Add( m_pTxtSession, 0, wxRIGHT|wxLEFT|wxALIGN_RIGHT, 5 );
	
	m_pGaugeSession = new wxGauge( this, wxID_ANY, 100, wxDefaultPosition, wxSize( 85,15 ), wxGA_HORIZONTAL|wxGA_SMOOTH );
	m_pGaugeSession->SetValue( 100 ); 
	m_pGridSizer->Add( m_pGaugeSession, 0, 0, 5 );
	
	m_pLblGlobal = new wxStaticText( this, wxID_ANY, _("Global:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pLblGlobal->Wrap( -1 );
	m_pLblGlobal->SetFont( wxFont( 8, 74, 90, 90, false, wxT("Arial") ) );
	
	m_pGridSizer->Add( m_pLblGlobal, 0, wxLEFT, 5 );
	
	m_pTxtGlobal = new wxStaticText( this, wxID_ANY, _T("0.0%"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pTxtGlobal->Wrap( -1 );
	m_pTxtGlobal->SetFont( wxFont( 8, 74, 90, 90, false, wxT("Arial") ) );
	
	m_pGridSizer->Add( m_pTxtGlobal, 0, wxRIGHT|wxLEFT|wxALIGN_RIGHT, 5 );
	
	m_pGaugeGlobal = new wxGauge( this, wxID_ANY, 100, wxDefaultPosition, wxSize( 85,15 ), wxGA_HORIZONTAL|wxGA_SMOOTH );
	m_pGaugeGlobal->SetValue( 70 ); 
	m_pGridSizer->Add( m_pGaugeGlobal, 0, 0, 5 );
	
	m_pDataSizer->Add( m_pGridSizer, 1, wxEXPAND|wxTOP, 5 );
	
	this->SetSizer( m_pDataSizer );
	this->Layout();
}

void lmLeitnerAuxCtrol::UpdateDisplay()
{
    int nNew = m_pProblemMngr->GetNew();
    int nExpired = m_pProblemMngr->GetExpired();
    int nTotal = m_pProblemMngr->GetTotal();
    float rSessionProgress = m_pProblemMngr->GetSessionProgress();
    float rGlobalProgress = m_pProblemMngr->GetGlobalProgress();
    wxTimeSpan tsEST = m_pProblemMngr->GetEstimatedSessionTime();

    //update display
    m_pTxtNumQuestions->SetLabel( wxString::Format(_T("%d / %d"), nNew, nExpired) );
    m_pTxtSession->SetLabel( wxString::Format(_T("%.01f%"), rSessionProgress) );
	m_pGaugeSession->SetValue( int(rSessionProgress + 0.5f) ); 
    m_pTxtGlobal->SetLabel( wxString::Format(_T("%.01f%"), rGlobalProgress) );
	m_pGaugeGlobal->SetValue( int(rGlobalProgress + 0.5f) ); 
    m_pTxtTime->SetLabel( tsEST.Format(_T("%Hh:%Mm:%Ss")) ); 

    Layout();
}

void lmLeitnerAuxCtrol::OnExplainProgress(wxCommandEvent& WXUNUSED(event))
{
    lmHtmlDlg dlg(this, _("Progress report"));
    dlg.SetContent( m_pProblemMngr->GetProgressReport() );
    dlg.ShowModal();
}



//--------------------------------------------------------------------------------
// lmPractiseAuxCtrol implementation
// A control to embed in html exercises to display statistics
// on user performance in learning the subject. It uses the Leitner system of
// spaced repetitions
// -------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(lmPractiseAuxCtrol, lmCountersAuxCtrol)
    LM_EVT_URL_CLICK(lmID_LINK_RESET_COUNTERS, lmPractiseAuxCtrol::OnResetCounters)
END_EVENT_TABLE()


lmPractiseAuxCtrol::lmPractiseAuxCtrol(wxWindow* parent, wxWindowID id, double rScale,
                                     lmLeitnerManager* pProblemMngr,
                                     const wxPoint& pos)
    : lmCountersAuxCtrol(parent, id, rScale, pos, wxSize(220, 130))
{
    //initializations
    m_pProblemMngr = pProblemMngr;

    // Create the controls
    CreateControls();

    //load icons
    wxBitmap bmp = wxArtProvider::GetBitmap(_T("button_accept"), wxART_TOOLBAR, wxSize(24,24));
    m_pBmpRight->SetBitmap(bmp);
    bmp = wxArtProvider::GetBitmap(_T("button_cancel"), wxART_TOOLBAR, wxSize(24,24));
    m_pBmpWrong->SetBitmap(bmp);
    bmp = wxArtProvider::GetBitmap(_T("diploma_cap"), wxART_TOOLBAR, wxSize(35,24));
    m_pBmpTotal->SetBitmap(bmp);
}

lmPractiseAuxCtrol::~lmPractiseAuxCtrol()
{
}

void lmPractiseAuxCtrol::CreateControls()
{
    //AWARE: Code created with wxFormBuilder and copied here.
    //Modifications:
    // - near line 483: do not load bitmap at creation time
    // - near line 499: do not load bitmap at creation time
    // - near line 515: do not load bitmap at creation time
    // - near line 530: replacement of m_pLinkResetCounters (wxStaticTxt) by an lmUrlAuxCtrol

	wxBoxSizer* pCtrolSizer;
	pCtrolSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* pCountersSizer;
	pCountersSizer = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* pWrongSizer;
	pWrongSizer = new wxBoxSizer( wxVERTICAL );
	
    //m_pBmpWrong = new wxStaticBitmap( this, wxID_ANY, wxBitmap( wxT("../res/icons/button_cancel_24.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
    m_pBmpWrong = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
    pWrongSizer->Add( m_pBmpWrong, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_pTxtWrong = new wxStaticText( this, wxID_ANY, _T("0"), wxDefaultPosition, wxSize( 50,-1 ), wxALIGN_CENTRE|wxST_NO_AUTORESIZE|wxSIMPLE_BORDER );
	m_pTxtWrong->Wrap( -1 );
	m_pTxtWrong->SetFont( wxFont( 12, 74, 90, 92, false, wxT("Arial") ) );
	m_pTxtWrong->SetBackgroundColour( wxColour( 255, 255, 255 ) );
	
	pWrongSizer->Add( m_pTxtWrong, 0, wxALIGN_CENTER_HORIZONTAL|wxADJUST_MINSIZE|wxBOTTOM, 0 );
	
	pCountersSizer->Add( pWrongSizer, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxRIGHT, 5 );
	
	wxBoxSizer* pRightSizer;
	pRightSizer = new wxBoxSizer( wxVERTICAL );
	
	//m_pBmpRight = new wxStaticBitmap( this, wxID_ANY, wxBitmap( wxT("../res/icons/button_accept_24.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
	m_pBmpRight = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	pRightSizer->Add( m_pBmpRight, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_pTxtRight = new wxStaticText( this, wxID_ANY, _T("0"), wxDefaultPosition, wxSize( 50,-1 ), wxALIGN_CENTRE|wxST_NO_AUTORESIZE|wxSIMPLE_BORDER );
	m_pTxtRight->Wrap( -1 );
	m_pTxtRight->SetFont( wxFont( 12, 74, 90, 92, false, wxT("Arial") ) );
	m_pTxtRight->SetBackgroundColour( wxColour( 255, 255, 255 ) );
	
	pRightSizer->Add( m_pTxtRight, 0, wxTOP|wxBOTTOM|wxADJUST_MINSIZE|wxALIGN_CENTER_HORIZONTAL, 0 );
	
	pCountersSizer->Add( pRightSizer, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxRIGHT, 5 );
	
	wxBoxSizer* pTotalSizer;
	pTotalSizer = new wxBoxSizer( wxVERTICAL );
	
	//m_pBmpTotal = new wxStaticBitmap( this, wxID_ANY, wxBitmap( wxT("../res/icons/diploma_cap_24.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
	m_pBmpTotal = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	pTotalSizer->Add( m_pBmpTotal, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	m_pTxtTotal = new wxStaticText( this, wxID_ANY, _T("-"), wxDefaultPosition, wxSize( 50,-1 ), wxALIGN_CENTRE|wxST_NO_AUTORESIZE|wxSIMPLE_BORDER );
	m_pTxtTotal->Wrap( -1 );
	m_pTxtTotal->SetFont( wxFont( 12, 74, 90, 92, false, wxT("Arial") ) );
	m_pTxtTotal->SetBackgroundColour( wxColour( 255, 255, 255 ) );
	
	pTotalSizer->Add( m_pTxtTotal, 0, wxALIGN_CENTER_HORIZONTAL|wxADJUST_MINSIZE|wxTOP|wxBOTTOM, 0 );
	
	pCountersSizer->Add( pTotalSizer, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM, 5 );
	
	pCtrolSizer->Add( pCountersSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
    //FOLLOWING LINES REPLACED------------------------------
	//m_pLinkResetCounters = new wxStaticText( this, lmID_LINK_RESET_COUNTERS, _("Reset counters"), wxDefaultPosition, wxDefaultSize, 0 );
	//m_pLinkResetCounters->Wrap( -1 );
	//m_pLinkResetCounters->SetFont( wxFont( 8, 74, 90, 90, true, wxT("Tahoma") ) );
	//m_pLinkResetCounters->SetForegroundColour( wxColour( 0, 0, 255 ) );
	//
	//pCtrolSizer->Add( m_pLinkResetCounters, 0, wxALIGN_CENTER_HORIZONTAL|wxLEFT|wxRIGHT|wxBOTTOM|wxADJUST_MINSIZE, 5 );
    pCtrolSizer->Add(
        new lmUrlAuxCtrol(this, lmID_LINK_RESET_COUNTERS, m_rScale, _("Reset counters"),
                          _T("link_reset")),
        0, wxALIGN_CENTER_HORIZONTAL|wxLEFT|wxRIGHT|wxBOTTOM|wxADJUST_MINSIZE, 5 );
    //END OF MODIFICATION----------------------------------
	
	this->SetSizer( pCtrolSizer );
	this->Layout();
}

void lmPractiseAuxCtrol::UpdateDisplay()
{
    int nRight = m_pProblemMngr->GetRight();
    int nWrong = m_pProblemMngr->GetWrong();

    //update display for right answers
    m_pTxtRight->SetLabel( wxString::Format(_T("%d"), nRight) );

    //update display for wrong answers
    m_pTxtWrong->SetLabel( wxString::Format(_T("%d"), nWrong) );

    //update display for total score
    int nTotal = nRight + nWrong;
    if (nTotal == 0)
        m_pTxtTotal->SetLabel( _T("-") );
    else
    {
        float rScore = (float)(10 * nRight) / (float)nTotal;
        m_pTxtTotal->SetLabel( wxString::Format(_T("%.01f"), rScore) );
    }
    Refresh();
}

void lmPractiseAuxCtrol::OnResetCounters(wxCommandEvent& WXUNUSED(event))
{
    m_pProblemMngr->ResetPractiseCounters();
    UpdateDisplay();
}

