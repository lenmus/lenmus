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

#include "CountersAuxCtrol.h"
#include "UrlAuxCtrol.h"
#include "../../sound/WaveManager.h"
#include "../Generators.h"

//access to global flag
extern bool g_fAnswerSoundsEnabled;
extern bool g_fTeamCounters;



//--------------------------------------------------------------------------------
// lmCountersAuxCtrol implementation
// An abstract control to embed in html exercises to display statistics about
// user performance in doing the exercise
//--------------------------------------------------------------------------------

lmCountersAuxCtrol::lmCountersAuxCtrol(wxWindow* parent, wxWindowID id,  double rScale,
                                       const wxPoint& pos, const wxSize& size)
    : wxPanel(parent, id, pos, size, wxBORDER_NONE)
{
    //initializations
    m_rScale = rScale;

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

//IDs for controls
enum {
    ID_LINK_RESET_COUNTERS = 3600,
};


BEGIN_EVENT_TABLE(lmQuizAuxCtrol, lmCountersAuxCtrol)
    LM_EVT_URL_CLICK(ID_LINK_RESET_COUNTERS, lmQuizAuxCtrol::OnResetCounters)
END_EVENT_TABLE()


lmQuizAuxCtrol::lmQuizAuxCtrol(wxWindow* parent, wxWindowID id,  double rScale,
                               lmQuizManager* pProblemMngr, const wxPoint& pos)
    : lmCountersAuxCtrol(parent, id, rScale, pos)
{
    //initializations
    m_pProblemMngr = pProblemMngr;

    // Create the controls
    this->SetBackgroundColour(*wxWHITE);
    wxBoxSizer* pMainSizer = new wxBoxSizer(wxVERTICAL);
    if (g_fTeamCounters)
    {
        m_pProblemMngr->SetNumTeams(2);
        m_pTeamTxt = new wxStaticText( this, wxID_STATIC, _("Two teams competition"), wxDefaultPosition,
                            wxSize(-1, -1),
                            wxALIGN_CENTRE|wxBORDER_NONE|wxST_NO_AUTORESIZE );
        m_pTeamTxt->SetFont(wxFont(14, wxSWISS, wxNORMAL, wxBOLD, false, _T("Arial")));
        pMainSizer->Add(m_pTeamTxt, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 0);
        CreateCountersGroup(0, pMainSizer, true);
        CreateCountersGroup(1, pMainSizer, true);
    }
    else
    {
        m_pProblemMngr->SetNumTeams(1);
        m_pTeamTxt = (wxStaticText*)NULL;
        CreateCountersGroup(0, pMainSizer, false);
    }

    //'reset counters' link
    pMainSizer->Add(
        new lmUrlAuxCtrol(this, ID_LINK_RESET_COUNTERS, rScale, _("Reset counters") ),
        0, wxALIGN_CENTER_HORIZONTAL|wxLEFT|wxRIGHT|wxBOTTOM|wxADJUST_MINSIZE, 5);

    //set main window sizer
    SetSizer( pMainSizer );                 // use the sizer for window layout
    pMainSizer->SetSizeHints( this );       // set size hints to honour minimum size

    Layout();
    for (int i=0; i < m_pProblemMngr->GetNumTeams(); i++)
        UpdateDisplays(i);
}

lmQuizAuxCtrol::~lmQuizAuxCtrol()
{
}

void lmQuizAuxCtrol::CreateCountersGroup(int nTeam, wxBoxSizer* pMainSizer, bool fTeam)
{
    // Create the controls for counter group nTeam (0...n)

    wxBoxSizer* pCountersSizer = new wxBoxSizer(wxHORIZONTAL);
    pMainSizer->Add(pCountersSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    // Font sizes
    int nNormalSize = (int)(m_rScale * 8.0);      // 8pt, scaled
    int nBigSize = (int)(m_rScale * 18.0);      // 18pt, scaled

    // Boxes sizes and spacing
    int nBoxSize = (int)(m_rScale * 50.0);      // 50px, scaled
    int nSpacing = (int)(m_rScale * 5.0);       // 5px, scaled

    // Team label
    if (fTeam) {
        wxBoxSizer* pTeamSizer = new wxBoxSizer(wxVERTICAL);
        pCountersSizer->Add(pTeamSizer, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, nSpacing);

        if (nTeam == 0) {
            wxStaticText* pTxtRight = new wxStaticText( this, wxID_STATIC, _("Team"),
				wxDefaultPosition, wxDefaultSize, 0 );
            pTxtRight->SetFont(wxFont(nNormalSize, wxSWISS, wxNORMAL, wxBOLD, false,
				_T("Arial")));
            pTeamSizer->Add(pTxtRight, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM|wxADJUST_MINSIZE,
				nSpacing);
        }
        wxStaticText* pTeamLabel = new wxStaticText( this, wxID_STATIC,
                wxString::Format(_T("%d"), nTeam+1), wxDefaultPosition,
                wxSize(nBoxSize, -1), wxALIGN_CENTRE|wxBORDER_NONE|wxST_NO_AUTORESIZE );
        pTeamLabel->SetBackgroundColour(wxColour(255, 255, 255));
        pTeamLabel->SetFont(wxFont(nBigSize, wxSWISS, wxNORMAL, wxNORMAL, false, _T("")));
        pTeamSizer->Add(pTeamLabel, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 0);
    }

    //display for right answers
    wxBoxSizer* pRightSizer = new wxBoxSizer(wxVERTICAL);
    pCountersSizer->Add(pRightSizer, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, nSpacing);

    if (nTeam == 0) {
        wxStaticText* pTxtRight = new wxStaticText( this, wxID_STATIC, _("Right"),
			wxDefaultPosition, wxDefaultSize, 0 );
        pTxtRight->SetFont(wxFont(nNormalSize, wxSWISS, wxNORMAL, wxBOLD, false, _T("Arial")));
        pRightSizer->Add(pTxtRight, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM|wxADJUST_MINSIZE,
			nSpacing);
    }
    m_pRightCounter[nTeam] = new wxStaticText( this, wxID_STATIC, _T(""),
		wxDefaultPosition, wxSize(nBoxSize, -1),
		wxALIGN_CENTRE|wxBORDER_SIMPLE|wxST_NO_AUTORESIZE );
    m_pRightCounter[nTeam]->SetBackgroundColour(wxColour(255, 255, 255));
    m_pRightCounter[nTeam]->SetFont(wxFont(nBigSize, wxSWISS, wxNORMAL, wxNORMAL, false, _T("")));
    pRightSizer->Add(m_pRightCounter[nTeam], 0,
		wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 0);

    //display for wrong answers
    wxBoxSizer* pWrongSizer = new wxBoxSizer(wxVERTICAL);
    pCountersSizer->Add(pWrongSizer, 0, wxALIGN_CENTER_VERTICAL|wxALL, nSpacing);

    if (nTeam == 0) {
        wxStaticText* pTxtWrong = new wxStaticText( this, wxID_STATIC, _("Wrong"),
			wxDefaultPosition, wxDefaultSize, 0 );
        pTxtWrong->SetFont(wxFont(nNormalSize, wxSWISS, wxNORMAL, wxBOLD, false, _T("Arial")));
        pWrongSizer->Add(pTxtWrong, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM|wxADJUST_MINSIZE, nSpacing);
    }
    m_pWrongCounter[nTeam] = new wxStaticText( this, wxID_STATIC, _T(""),
		wxDefaultPosition, wxSize(nBoxSize, -1),
		wxALIGN_CENTRE|wxBORDER_SIMPLE|wxST_NO_AUTORESIZE );
    m_pWrongCounter[nTeam]->SetBackgroundColour(wxColour(255, 255, 255));
    m_pWrongCounter[nTeam]->SetFont(wxFont(nBigSize, wxSWISS, wxNORMAL, wxNORMAL, false, _T("")));
    pWrongSizer->Add(m_pWrongCounter[nTeam], 0,
		wxALIGN_CENTER_HORIZONTAL|wxTOP|wxBOTTOM|wxADJUST_MINSIZE, 0);

    //display for total score
    wxBoxSizer* pTotalSizer = new wxBoxSizer(wxVERTICAL);
    pCountersSizer->Add(pTotalSizer, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, nSpacing);

    if (nTeam == 0) {
        wxStaticText* pTxtTotal = new wxStaticText( this, wxID_STATIC, _("Mark"),
			wxDefaultPosition, wxDefaultSize, 0 );
        pTxtTotal->SetFont(wxFont(nNormalSize, wxSWISS, wxNORMAL, wxBOLD, false, _T("Arial")));
        pTotalSizer->Add(pTxtTotal, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM|wxADJUST_MINSIZE, nSpacing);
    }
    m_pTotalCounter[nTeam] = new wxStaticText( this, wxID_STATIC, _T(""),
		wxDefaultPosition, wxSize(nBoxSize, -1),
		wxALIGN_CENTRE|wxBORDER_SIMPLE|wxST_NO_AUTORESIZE );
    m_pTotalCounter[nTeam]->SetBackgroundColour(wxColour(255, 255, 255));
    m_pTotalCounter[nTeam]->SetFont(wxFont(nBigSize, wxSWISS, wxNORMAL, wxNORMAL, false, _T("Arial")));
    pTotalSizer->Add(m_pTotalCounter[nTeam], 0,
		wxALIGN_CENTER_HORIZONTAL|wxLEFT|wxTOP|wxBOTTOM|wxADJUST_MINSIZE, 0);

}

void lmQuizAuxCtrol::UpdateDisplay(bool fSuccess)
{
    WXUNUSED(fSuccess);
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
    if (m_pTeamTxt)
        m_pTeamTxt->SetLabel( wxString::Format(_("Team's %d turn"),
                                               m_pProblemMngr->GetCurrentTeam() + 1) );
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

enum
{
	lmID_LINK_EXPLAIN = 3600,
};

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
    //Modifications near lin 385: replacement of m_pLinkExplain (wxStaticTxt) by
    //an lmUrlAuxCtrol

	wxBoxSizer* m_pDataSizer;
	m_pDataSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* m_pSetsSizer;
	m_pSetsSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_pTxtPoor = new wxStaticText( this, wxID_ANY, _("17,2%"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
	m_pTxtPoor->Wrap( -1 );
	m_pTxtPoor->SetFont( wxFont( 8, 74, 90, 90, false, wxT("Arial") ) );
	m_pTxtPoor->SetForegroundColour( wxColour( 255, 255, 255 ) );
	m_pTxtPoor->SetBackgroundColour( wxColour( 255, 0, 0 ) );
	m_pTxtPoor->SetMinSize( wxSize( 50,-1 ) );
	
	m_pSetsSizer->Add( m_pTxtPoor, 1, wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_pTxtFair = new wxStaticText( this, wxID_ANY, _("17,2%"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
	m_pTxtFair->Wrap( -1 );
	m_pTxtFair->SetFont( wxFont( 8, 74, 90, 90, false, wxT("Arial") ) );
	m_pTxtFair->SetBackgroundColour( wxColour( 255, 205, 53 ) );
	m_pTxtFair->SetMinSize( wxSize( 50,-1 ) );
	
	m_pSetsSizer->Add( m_pTxtFair, 1, wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_pTxtGood = new wxStaticText( this, wxID_ANY, _("57,2%"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
	m_pTxtGood->Wrap( -1 );
	m_pTxtGood->SetFont( wxFont( 8, 74, 90, 90, false, wxT("Arial") ) );
	m_pTxtGood->SetBackgroundColour( wxColour( 204, 255, 0 ) );
	m_pTxtGood->SetMinSize( wxSize( 50,-1 ) );
	
	m_pSetsSizer->Add( m_pTxtGood, 1, wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_pTxtAchieved = new wxStaticText( this, wxID_ANY, _("17,2%"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
	m_pTxtAchieved->Wrap( -1 );
	m_pTxtAchieved->SetFont( wxFont( 8, 74, 90, 90, false, wxT("Arial") ) );
	m_pTxtAchieved->SetBackgroundColour( wxColour( 0, 255, 64 ) );
	m_pTxtAchieved->SetMinSize( wxSize( 50,-1 ) );
	
	m_pSetsSizer->Add( m_pTxtAchieved, 1, wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_pDataSizer->Add( m_pSetsSizer, 0, wxTOP|wxEXPAND, 5 );
	
	wxBoxSizer* m_pProgressSizer;
	m_pProgressSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_pLblProgress = new wxStaticText( this, wxID_ANY, _("Progress:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pLblProgress->Wrap( -1 );
	m_pLblProgress->SetFont( wxFont( 12, 74, 90, 92, false, wxT("Arial") ) );
	
	m_pProgressSizer->Add( m_pLblProgress, 0, wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_pTxtProgress = new wxStaticText( this, wxID_ANY, _("45,7%"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pTxtProgress->Wrap( -1 );
	m_pTxtProgress->SetFont( wxFont( 12, 74, 90, 92, false, wxT("Arial") ) );
	
	m_pProgressSizer->Add( m_pTxtProgress, 1, wxALL, 5 );
	
    //FOLLOWING LINES REPLACED------------------------------
	//m_pLinkExplain = new wxStaticText( this, lmID_LINK_EXPLAIN, _("Explain"), wxDefaultPosition, wxDefaultSize, 0 );
	//m_pLinkExplain->Wrap( -1 );
	//m_pLinkExplain->SetFont( wxFont( 8, 74, 90, 90, true, wxT("Tahoma") ) );
	//m_pLinkExplain->SetForegroundColour( wxColour( 0, 0, 255 ) );

	//m_pProgressSizer->Add( m_pLinkExplain, 0, wxTOP|wxBOTTOM|wxLEFT, 5 );
        //'reset counters' link
    m_pProgressSizer->Add(
        new lmUrlAuxCtrol(this, lmID_LINK_EXPLAIN, m_rScale, _("Explain") ),
        0, wxTOP|wxBOTTOM|wxLEFT, 5);
    //END OF MODIFICATION----------------------------------
	
	m_pDataSizer->Add( m_pProgressSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( m_pDataSizer );
	this->Layout();
}

void lmLeitnerAuxCtrol::UpdateDisplay(bool fSuccess)
{
    float rPoor = m_pProblemMngr->GetPoor();
    float rFair = m_pProblemMngr->GetFair();
    float rGood = m_pProblemMngr->GetGood();
    float rAchieved = m_pProblemMngr->GetAchieved();
    float rProgress = m_pProblemMngr->GetProgress();

    //update display
	m_pTxtPoor->SetLabel( wxString::Format(_T("%.01f%"), rPoor) );
	m_pTxtFair->SetLabel( wxString::Format(_T("%.01f%"), rFair) );
	m_pTxtGood->SetLabel( wxString::Format(_T("%.01f%"), rGood) );
	m_pTxtAchieved->SetLabel( wxString::Format(_T("%.01f%"), rAchieved) );
	m_pTxtProgress->SetLabel( wxString::Format(_T("%.01f%"), rProgress) );

    m_pTxtPoor->SetBackgroundColour(*wxRED);    //TEST in Linux

    Layout();
}

void lmLeitnerAuxCtrol::OnExplainProgress(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(_T("Click on link 'Explain'"));
}
