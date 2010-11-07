//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2010 LenMus project
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

#ifndef __LM_IDFYTONALITYCTROL_H__        //to avoid nested includes
#define __LM_IDFYTONALITYCTROL_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "IdfyTonalityCtrol.cpp"
#endif

// For compilers that support precompilation, includes <wx/wx.h>.
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "TonalityConstrains.h"
#include "../score/Score.h"
#include "ExerciseCtrol.h"



class lmIdfyTonalityCtrol : public lmOneScoreCtrol    
{
public:

    // constructor and destructor    
    lmIdfyTonalityCtrol(wxWindow* parent, wxWindowID id,
               lmTonalityConstrains* pConstrains, 
               const wxPoint& pos = wxDefaultPosition, 
               const wxSize& size = wxDefaultSize, int style = 0);

    ~lmIdfyTonalityCtrol();

    //implementation of virtual methods
    void InitializeStrings();
    void CreateAnswerButtons(int nHeight, int nSpacing, wxFont& font);
    void PrepareAuxScore(int nButton);
    wxString SetNewProblem();    
    wxDialog* GetSettingsDlg();
    void OnSettingsChanged();

protected:
    bool CheckSuccessFailure(int nButton);

private:
    wxString PrepareScore(lmEClefType nClef, lmEKeySignatures nType,
                          lmScore** pProblemScore,
                          lmScore** pSolutionScore = NULL );

        // member variables

    enum {
        m_NUM_COLS = 5,
        m_NUM_ROWS = 6,
        m_NUM_BUTTONS = 30,     // <= NUM_COLS * NUM_ROWS;
    };

    lmTonalityConstrains* m_pConstrains;       //constraints for the exercise

    //problem asked
    lmEKeySignatures  m_nKey;

    //answer
    wxButton*       m_pAnswerButton[m_NUM_BUTTONS];     //buttons for the answers
    //key signature that corresponds to each button
    lmEKeySignatures  m_nRealKey[m_NUM_BUTTONS];

    DECLARE_EVENT_TABLE()
};



#endif  // __LM_IDFYTONALITYCTROL_H__
