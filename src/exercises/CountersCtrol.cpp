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
/*! @file CountersCtrol.cpp
    @brief Implementation file for class lmCountersCtrol
    @ingroup html_controls
*/
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


lmCountersCtrol::lmCountersCtrol(wxWindow* parent, wxWindowID id, const wxPoint& pos) :
    wxWindow(parent, id, pos, wxDefaultSize, wxNO_BORDER )
{

        //
        // Create the controls
        //

    wxBoxSizer* pMainSizer = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* pCountersSizer = new wxBoxSizer(wxHORIZONTAL);
    pMainSizer->Add(pCountersSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    //display for right answers
    wxBoxSizer* pRightSizer = new wxBoxSizer(wxVERTICAL);
    pCountersSizer->Add(pRightSizer, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    wxStaticText* pTxtRight = new wxStaticText( this, wxID_STATIC, _("Right"), wxDefaultPosition, wxDefaultSize, 0 );
    pTxtRight->SetFont(wxFont(8, wxSWISS, wxNORMAL, wxBOLD, false, _T("Arial")));
    pRightSizer->Add(pTxtRight, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM|wxADJUST_MINSIZE, 5);

    m_pRightCounter = new wxStaticText( this, wxID_STATIC, _T(""), wxDefaultPosition, wxSize(50, -1), wxALIGN_CENTRE|wxSIMPLE_BORDER|wxST_NO_AUTORESIZE );
    m_pRightCounter->SetBackgroundColour(wxColour(255, 255, 255));
    m_pRightCounter->SetFont(wxFont(18, wxSWISS, wxNORMAL, wxNORMAL, false, _T("")));
    pRightSizer->Add(m_pRightCounter, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 0);

    //display for wrong answers
    wxBoxSizer* pWrongSizer = new wxBoxSizer(wxVERTICAL);
    pCountersSizer->Add(pWrongSizer, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* pTxtWrong = new wxStaticText( this, wxID_STATIC, _("Wrong"), wxDefaultPosition, wxDefaultSize, 0 );
    pTxtWrong->SetFont(wxFont(8, wxSWISS, wxNORMAL, wxBOLD, false, _T("Arial")));
    pWrongSizer->Add(pTxtWrong, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM|wxADJUST_MINSIZE, 5);

    m_pWrongCounter = new wxStaticText( this, wxID_STATIC, _T(""), wxDefaultPosition, wxSize(50, -1), wxALIGN_CENTRE|wxSIMPLE_BORDER|wxST_NO_AUTORESIZE );
    m_pWrongCounter->SetBackgroundColour(wxColour(255, 255, 255));
    m_pWrongCounter->SetFont(wxFont(18, wxSWISS, wxNORMAL, wxNORMAL, false, _T("")));
    pWrongSizer->Add(m_pWrongCounter, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP|wxBOTTOM|wxADJUST_MINSIZE, 0);

    //display for total score
    wxBoxSizer* pTotalSizer = new wxBoxSizer(wxVERTICAL);
    pCountersSizer->Add(pTotalSizer, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    wxStaticText* pTxtTotal = new wxStaticText( this, wxID_STATIC, _("Mark"), wxDefaultPosition, wxDefaultSize, 0 );
    pTxtTotal->SetFont(wxFont(8, wxSWISS, wxNORMAL, wxBOLD, false, _T("Arial")));
    pTotalSizer->Add(pTxtTotal, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM|wxADJUST_MINSIZE, 5);

    m_pTotalCounter = new wxStaticText( this, wxID_STATIC, _T(""), wxDefaultPosition, wxSize(50, -1), wxALIGN_CENTRE|wxSIMPLE_BORDER|wxST_NO_AUTORESIZE );
    m_pTotalCounter->SetBackgroundColour(wxColour(255, 255, 255));
    m_pTotalCounter->SetFont(wxFont(18, wxSWISS, wxNORMAL, wxNORMAL, false, _T("Arial")));
    pTotalSizer->Add(m_pTotalCounter, 0, wxALIGN_CENTER_HORIZONTAL|wxLEFT|wxTOP|wxBOTTOM|wxADJUST_MINSIZE, 0);

    //'reset counters' link
    pMainSizer->Add(
        new lmUrlAuxCtrol(this, ID_LINK_RESET_COUNTERS, _("Reset counters") ),
        0, wxALIGN_CENTER_HORIZONTAL|wxLEFT|wxRIGHT|wxBOTTOM|wxADJUST_MINSIZE, 5);

    //set main window sizer
    SetSizer( pMainSizer );                 // use the sizer for window layout
    pMainSizer->SetSizeHints( this );       // set size hints to honour minimum size

    Layout();

        //
        // Initializations
        //

    ResetCounters();

}

lmCountersCtrol::~lmCountersCtrol()
{
}

void lmCountersCtrol::OnResetCounters(wxCommandEvent& WXUNUSED(event))
{
    ResetCounters();
}

void lmCountersCtrol::ResetCounters()
{
    m_nRight = 0;
    m_nWrong = 0;
    UpdateDisplays();
}

void lmCountersCtrol::IncrementWrong()
{
    m_nWrong++;
    UpdateDisplays();
}

void lmCountersCtrol::IncrementRight()
{
    m_nRight++;
    UpdateDisplays();
}

void lmCountersCtrol::UpdateDisplays()
{
    //update display for right answers
    m_pRightCounter->SetLabel( wxString::Format(_T("%d"), m_nRight) );

    //update display for right answers
    m_pWrongCounter->SetLabel( wxString::Format(_T("%d"), m_nWrong) );

    //update display for total score
    int nTotal = m_nRight + m_nWrong;
    if (nTotal == 0)
        m_pTotalCounter->SetLabel( _T("-") );
    else {
        float rScore = 10* (float)m_nRight / (float)nTotal;
        m_pTotalCounter->SetLabel( wxString::Format(_T("%.01f"), rScore) );
    }

}

