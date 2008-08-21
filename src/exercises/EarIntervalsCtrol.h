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

#ifndef __LM_EARINTERVALSCTROL_H__        //to avoid nested includes
#define __LM_EARINTERVALSCTROL_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "EarIntervalsCtrol.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "EarIntvalConstrains.h"
#include "../score/Score.h"
#include "../auxmusic/Interval.h"
#include "ExerciseCtrol.h"



class lmEarIntervalsCtrol : public lmOneScoreCtrol    
{
   DECLARE_DYNAMIC_CLASS(lmEarIntervalsCtrol)

public:

    // constructor and destructor    
    lmEarIntervalsCtrol(wxWindow* parent, wxWindowID id,
               lmEarIntervalsConstrains* pConstrains, 
               const wxPoint& pos = wxDefaultPosition, 
               const wxSize& size = wxDefaultSize, int style = 0);

    ~lmEarIntervalsCtrol();

    enum {
        m_NUM_COLS = 5,
        m_NUM_ROWS = 5,
        m_NUM_BUTTONS = 25,     // NUM_COLS * NUM_ROWS;
        m_ID_BUTTON = 3010,
    };

protected:
    //implementation of virtual methods
    void InitializeStrings();
    void CreateAnswerButtons(int nHeight, int nSpacing, wxFont& font);
    void PrepareAuxScore(int nButton);
    wxString SetNewProblem();    
    wxDialog* GetSettingsDlg();
    void ReconfigureButtons();

private:
    void PrepareScore(wxString& sIntvCode, lmScore** pScore);


        // member variables

    lmEarIntervalsConstrains* m_pConstrains;
    wxButton*   m_pAnswerButton[m_NUM_BUTTONS];
    int         m_nValidIntervals;              // num of enabled buttons 
    int         m_nRealIntval[m_NUM_BUTTONS];   // intval. associated to each valid button

    //problem asked
    wxString            m_sIntvCode;
    bool                m_fAscending;
    bool                m_fHarmonic;
    lmEKeySignatures      m_nKey;
    lmNoteBits          m_tNote[2];

    DECLARE_EVENT_TABLE()
};



#endif  // __LM_EARINTERVALSCTROL_H__
