//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
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

#ifndef __THEOINTERVALSCTROL_H__        //to avoid nested includes
#define __THEOINTERVALSCTROL_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "TheoIntervalsCtrol.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "TheoIntervalsConstrains.h"
#include "../score/Score.h"


class lmTheoIntervalsCtrol : public lmOneScoreCtrol        
{
public:

    // constructor and destructor    
    lmTheoIntervalsCtrol(wxWindow* parent, wxWindowID id,
               lmTheoIntervalsConstrains* pConstrains, 
               const wxPoint& pos = wxDefaultPosition, 
               const wxSize& size = wxDefaultSize, int style = 0);

    ~lmTheoIntervalsCtrol();

    //implementation of virtual methods
    void InitializeStrings();
    void CreateAnswerButtons();
    void PrepareAuxScore(int nButton);
    wxString SetNewProblem();    
    wxDialog* GetSettingsDlg();
    void ReconfigureButtons();

    enum {
        m_NUM_COLS = 7,
        m_NUM_ROWS = 6,
        m_NUM_BUTTONS = 44,     
    };


private:
    void EnableButtons(bool fEnable);
    void SetButtonsForNotes();
    void SetButtonsForIntervals();

        // member variables

    lmTheoIntervalsConstrains* m_pConstrains;
    lmDPitch        m_DPitch[2];           //the pitch of the two notes
    bool            m_fIntervalKnown;
    int             m_nCurrentKeyboard;     //not avoid unnecessary redrawing 

    //buttons for the answers: 6 rows, 7 cols + 2 extra buttons (Unisons) = 44 buttons
    wxButton*       m_pAnswerButton[m_NUM_BUTTONS];
    wxStaticText*   m_pRowLabel[m_NUM_ROWS];
    wxStaticText*   m_pColumnLabel[m_NUM_COLS];

    //to give the answer
    EClefType       m_nClef;

    DECLARE_EVENT_TABLE()
};



#endif  // __THEOINTERVALSCTROL_H__
