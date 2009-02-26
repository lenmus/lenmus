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

#ifndef __LM_THEOINTERVALSCTROL_H__        //to avoid nested includes
#define __LM_THEOINTERVALSCTROL_H__

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
#include "ExerciseCtrol.h"
#include "Generators.h"


//abstract class for intervals theory exercises

class lmTheoIntervalCtrol : public lmOneScoreCtrol        
{
public:

    // constructor and destructor    
    lmTheoIntervalCtrol(wxWindow* parent, wxWindowID id,
               lmTheoIntervalsConstrains* pConstrains, 
               const wxPoint& pos = wxDefaultPosition, 
               const wxSize& size = wxDefaultSize, int style = 0);

    virtual ~lmTheoIntervalCtrol();

    //implementation of virtual methods
    void PrepareAuxScore(int nButton);

    wxDialog* GetSettingsDlg();
    void OnSettingsChanged();
    wxString SetNewProblem(); 



protected:
    virtual void ReconfigureKeyboard() {};
    void SetProblemSpace();
    virtual wxString PrepareScores()=0; 

        // member variables

    lmTheoIntervalsConstrains* m_pConstrains;

    //to give the answer
    lmEClefType         m_nClef;
    lmEKeySignatures    m_nKey;
    lmFIntval           m_fpIntv;
    lmFPitch            m_fpStart;
    lmFPitch            m_fpEnd;

};

class lmBuildIntervalCtrol : public lmTheoIntervalCtrol        
{
public:

    // constructor and destructor    
    lmBuildIntervalCtrol(wxWindow* parent, wxWindowID id,
               lmTheoIntervalsConstrains* pConstrains, 
               const wxPoint& pos = wxDefaultPosition, 
               const wxSize& size = wxDefaultSize, int style = 0);

    ~lmBuildIntervalCtrol();

    //implementation of virtual methods
    void InitializeStrings();
    void CreateAnswerButtons(int nHeight, int nSpacing, wxFont& font);

    enum {
        lm_NUM_COLS = 7,
        lm_NUM_ROWS = 5,
        lm_NUM_BUTTONS = 35,     
    };

protected:
    wxString PrepareScores(); 

private:
    //buttons for the answers: 5 rows, 7 cols = 35 buttons
    wxButton*       m_pAnswerButton[lm_NUM_BUTTONS];
    wxStaticText*   m_pRowLabel[lm_NUM_ROWS];
    wxStaticText*   m_pColumnLabel[lm_NUM_COLS];

    DECLARE_EVENT_TABLE()
};


class lmIdfyIntervalCtrol : public lmTheoIntervalCtrol        
{
public:

    // constructor and destructor    
    lmIdfyIntervalCtrol(wxWindow* parent, wxWindowID id,
               lmTheoIntervalsConstrains* pConstrains, 
               const wxPoint& pos = wxDefaultPosition, 
               const wxSize& size = wxDefaultSize, int style = 0);

    ~lmIdfyIntervalCtrol();

    //implementation of virtual methods
    void InitializeStrings();
    void CreateAnswerButtons(int nHeight, int nSpacing, wxFont& font);

    enum {
        lm_NUM_COLS = 8,
        lm_NUM_ROWS = 6,
        lm_NUM_BUTTONS = 51,     
    };


    void EnableButtons(bool fEnable);
    void ReconfigureKeyboard();

protected:
    wxString PrepareScores(); 

        // member variables

    //buttons for the answers: 6 rows, 8 cols + 3 extra buttons = 51 buttons
    wxButton*       m_pAnswerButton[lm_NUM_BUTTONS];
    wxStaticText*   m_pRowLabel[lm_NUM_ROWS];
    wxStaticText*   m_pColumnLabel[lm_NUM_COLS];

    wxBoxSizer*     m_pUnisonSizer;

    //to give the answer
    int                 m_nFirstRow;

    DECLARE_EVENT_TABLE()
};



#endif  // __LM_THEOINTERVALSCTROL_H__
