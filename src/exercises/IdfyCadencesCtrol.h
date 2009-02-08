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

#ifndef __LM_IDFYCADENCECTROL_H__        //to avoid nested includes
#define __LM_IDFYCADENCECTROL_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "IdfyCadencesCtrol.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "CadencesConstrains.h"
#include "../score/Score.h"
#include "ExerciseCtrol.h"



class lmIdfyCadencesCtrol : public lmOneScoreCtrol    
{
public:

    // constructor and destructor    
    lmIdfyCadencesCtrol(wxWindow* parent, wxWindowID id,
               lmCadencesConstrains* pConstrains, 
               const wxPoint& pos = wxDefaultPosition, 
               const wxSize& size = wxDefaultSize, int style = 0);

    ~lmIdfyCadencesCtrol();

    //implementation of virtual methods
    void InitializeStrings();
    void CreateAnswerButtons(int nHeight, int nSpacing, wxFont& font);
    void PrepareAuxScore(int nButton);
    wxString SetNewProblem();    
    wxDialog* GetSettingsDlg();
    void OnSettingsChanged();


private:
    wxString PrepareScore(lmEClefType nClef, lmECadenceType nType, lmScore** pProblemScore,
                          lmScore** pSolutionScore = NULL );
    int DisplayButton(int iBt, lmECadenceType iStartC, lmECadenceType iEndC, wxString sButtonLabel);

        // member variables

    enum {
        m_NUM_COLS = 4,
        m_NUM_ROWS = 2,
        m_NUM_BUTTONS = 8,     // NUM_COLS * NUM_ROWS;
    };

    lmCadencesConstrains* m_pConstrains;       //constraints for the exercise

    //problem asked
    lmEKeySignatures  m_nKey;

    //answer
    wxButton*       m_pAnswerButton[m_NUM_BUTTONS];     //buttons for the answers
    //cadence that corresponds to each valid button
    lmECadenceType  m_nStartCadence[m_NUM_BUTTONS];
    lmECadenceType  m_nEndCadence[m_NUM_BUTTONS];

    DECLARE_EVENT_TABLE()
};



#endif  // __LM_IDFYCADENCECTROL_H__
