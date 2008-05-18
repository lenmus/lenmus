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
#pragma implementation "CountersCtrol.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "CountersCtrol.h"
#include "UrlAuxCtrol.h"
#include "../sound/WaveManager.h"

//access to global flag
#include "../app/Preferences.h"
extern bool g_fAnswerSoundsEnabled;
extern bool g_fTeamCounters;



/*! @class lmCountersCtrol
    @ingroup html_controls
    @brief  A control to embed in html exercises to display number
            on right and wrong student answers, and the total score (percentage)
*/

//IDs for controls
enum {
    ID_LINK_RESET_COUNTERS = 3600,
};


BEGIN_EVENT_TABLE(lmCountersCtrol, wxWindow)
    LM_EVT_URL_CLICK(ID_LINK_RESET_COUNTERS, lmCountersCtrol::OnResetCounters)
END_EVENT_TABLE()


lmCountersCtrol::lmCountersCtrol(wxWindow* parent, wxWindowID id,  double rScale, 
                                 const wxPoint& pos)
    : wxWindow(parent, id, pos, wxDefaultSize, wxBORDER_NONE )
{
    //initializations
    m_nMaxTeam = 0;             
    m_nCurrentTeam = 0;
    m_rScale = rScale;

    // Create the controls
    this->SetBackgroundColour(*wxWHITE);
    wxBoxSizer* pMainSizer = new wxBoxSizer(wxVERTICAL);
    if (g_fTeamCounters)
    {
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
    ResetCounters();

}

lmCountersCtrol::~lmCountersCtrol()
{
}

void lmCountersCtrol::CreateCountersGroup(int nTeam, wxBoxSizer* pMainSizer, bool fTeam)
{
    //
    // Create the controls for counter group nTeam (0...n)
    //

    m_nMaxTeam++;

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


void lmCountersCtrol::OnResetCounters(wxCommandEvent& WXUNUSED(event))
{
    ResetCounters();
}

void lmCountersCtrol::ResetCounters()
{
    for (int i=0; i < m_nMaxTeam; i++) {
        m_nRight[i] = 0;
        m_nWrong[i] = 0;
        UpdateDisplays(i);
    }
    m_fStart = true;
}

void lmCountersCtrol::IncrementWrong()
{
    m_nWrong[m_nCurrentTeam]++;
    UpdateDisplays(m_nCurrentTeam);
    if (g_fAnswerSoundsEnabled) {
        lmWaveManager* pWave = lmWaveManager::GetInstance();
        pWave->WrongAnswerSound();
    }
}

void lmCountersCtrol::IncrementRight()
{
    m_nRight[m_nCurrentTeam]++;
    UpdateDisplays(m_nCurrentTeam);
    if (g_fAnswerSoundsEnabled) {
        lmWaveManager* pWave = lmWaveManager::GetInstance();
        pWave->RightAnswerSound();
    }
}

void lmCountersCtrol::UpdateDisplays(int nTeam)
{
    //update display for right answers
    m_pRightCounter[nTeam]->SetLabel( wxString::Format(_T("%d"), m_nRight[nTeam]) );

    //update display for right answers
    m_pWrongCounter[nTeam]->SetLabel( wxString::Format(_T("%d"), m_nWrong[nTeam]) );

    //update display for total score
    int nTotal = m_nRight[nTeam] + m_nWrong[nTeam];
    if (nTotal == 0)
        m_pTotalCounter[nTeam]->SetLabel( _T("-") );
    else {
        float rScore = 10* (float)m_nRight[nTeam] / (float)nTotal;
        m_pTotalCounter[nTeam]->SetLabel( wxString::Format(_T("%.01f"), rScore) );
    }

}

void lmCountersCtrol::NextTeam()
{
    // move to next team
    if (m_nMaxTeam > 1) {
        m_nCurrentTeam++;
        m_nCurrentTeam = m_nCurrentTeam % m_nMaxTeam;
    }

    //ensure that first time after a reset we start with first team
    if (m_fStart) {
        m_nCurrentTeam = 0;
        m_fStart = false;
    }

    //update label
    if (m_pTeamTxt) {
        m_pTeamTxt->SetLabel( wxString::Format(_("Team's %d turn"), m_nCurrentTeam+1) );
    }

}


