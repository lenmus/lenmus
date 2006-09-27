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
/*! @file EarScalesCtrol.h
    @brief Header file for class lmEarScalesCtrol
    @ingroup html_controls
*/

#ifndef __EARSCALESCTROL_H__        //to avoid nested includes
#define __EARSCALESCTROL_H__

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

//#include "EarExerciseCtrol.h"

#include "ScalesConstrains.h"
#include "../score/score.h"
#include "ScoreAuxCtrol.h"
#include "UrlAuxCtrol.h"
#include "CountersCtrol.h"



class lmEarScalesCtrol :    public lmEarExerciseCtrol    
{
public:

    // constructor and destructor    
    lmEarScalesCtrol(wxWindow* parent, wxWindowID id,
               lmScalesConstrains* pConstrains, 
               const wxPoint& pos = wxDefaultPosition, 
               const wxSize& size = wxDefaultSize, int style = 0);

    ~lmEarScalesCtrol();

    //implementation of virtual methods
    void InitializeStrings();
    void CreateAnswerButtons();
    void SetButtonColor(int i, wxColour& color)
            { m_pAnswerButton[i]->SetBackgroundColour(color); }

    // event handlers
    void OnRespButton(wxCommandEvent& event);
    void OnResetCounters(wxCommandEvent& event);
    void OnSettingsButton(wxCommandEvent& event);

private:
    void SetUpButtons();
    void EnableButtons(bool fEnable);
    void NewProblem();
    void ResetExercise();
    void ResetCounters();
    wxString PrepareScore(EClefType nClef, EScaleType nType, lmScore** pScore);

        // member variables

    lmScalesConstrains* m_pConstrains;       //constrains for the exercise

    //problem asked
    EKeySignatures  m_nKey;
    wxString        m_sRootNote;
    int             m_nInversion;
    int             m_nMode;

    //answer
    wxStaticText*   m_pRowLabel[5];
    wxButton*       m_pAnswerButton[est_Max];   //buttons for the answers
    int             m_nRealChord[est_Max];      // chord that corresponds
                                                // to each valid button

    DECLARE_EVENT_TABLE()
};



#endif  // __EARSCALESCTROL_H__
