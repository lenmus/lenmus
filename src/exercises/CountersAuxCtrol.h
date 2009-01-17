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

#ifndef __LM_COUNTERSCTROL_H__        //to avoid nested includes
#define __LM_COUNTERSCTROL_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "CountersAuxCtrol.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

// ----------------------------------------------------------------------------
//  lmCountersAuxCtrol: a control to embed in html exercises to display number
//  on right and wrong student answers, and the total score (percentage)
// ----------------------------------------------------------------------------
class lmCountersAuxCtrol : public wxWindow    
{

public:

    // constructor and destructor    
    lmCountersAuxCtrol(wxWindow* parent, wxWindowID id, double rScale,
                    const wxPoint& pos = wxDefaultPosition);

    ~lmCountersAuxCtrol();

    // event handlers
    void OnResetCounters(wxCommandEvent& WXUNUSED(event));

    // settings
    void IncrementWrong();
    void IncrementRight();
    void ResetCounters();
    void NextTeam();

private:
    void UpdateDisplays(int nTeam);
    void CreateCountersGroup(int nTeam, wxBoxSizer* pMainSizer, bool fTeam);

    //counters for right and wrong answers
    int         m_nRight[2];
    int         m_nWrong[2];

    //displays
    wxStaticText*   m_pRightCounter[2];
    wxStaticText*   m_pWrongCounter[2];
    wxStaticText*   m_pTotalCounter[2];

    //labels
    wxStaticText*   m_pTeamTxt;
    
    //teams
    int         m_nMaxTeam;             //num of teams (1..2)
    int         m_nCurrentTeam;         //team currently playing (0..1)
    bool        m_fStart;               //to ensure that first time we start with first team

    // other
    double      m_rScale;               //current scaling factor

    DECLARE_EVENT_TABLE()
};



#endif  // __LM_COUNTERSCTROL_H__

