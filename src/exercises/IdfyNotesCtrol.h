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

#ifndef __LM_IDFYNOTESCTROL_H__        //to avoid nested includes
#define __LM_IDFYNOTESCTROL_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "IdfyNotesCtrol.cpp"
#endif

// For compilers that support precompilation, includes <wx/wx.h>.
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "../score/Score.h"
#include "ExerciseCtrol.h"
class lmUrlAuxCtrol;
class lmNotesConstrains;


class lmIdfyNotesCtrol : public lmOneScoreCtrol    
{
public:

    // constructor and destructor    
    lmIdfyNotesCtrol(wxWindow* parent, wxWindowID id,
               lmNotesConstrains* pConstrains, 
               const wxPoint& pos = wxDefaultPosition, 
               const wxSize& size = wxDefaultSize, int style = 0);

    ~lmIdfyNotesCtrol();

    //implementation of virtual methods
    void InitializeStrings();
    void CreateAnswerButtons(int nHeight, int nSpacing, wxFont& font);
    void PrepareAuxScore(int nButton);
    wxString SetNewProblem();    
    wxDialog* GetSettingsDlg();
    void OnSettingsChanged();
    void EnableButtons(bool fEnable);

    //specific event handlers
    void OnPlayA4(wxCommandEvent& event);
    void OnPlayAllNotes(wxCommandEvent& event);
    void OnContinue(wxCommandEvent& event);

    //overrides
    void OnNewProblem(wxCommandEvent& event);
    void OnRespButton(wxCommandEvent& event);
    void DisplaySolution();

private:
    wxString PrepareScore(lmEClefType nClef, wxString& sNotePitch, lmScore** pProblemScore,
                          lmScore** pSolutionScore = NULL );
    int GetFirstOctaveForClef(lmEClefType nClef);
    void PrepareAllNotesScore();
    void MoveToInitialState();
    void DisplayAllNotes();


        // member variables

    enum {
        m_NUM_BUTTONS = 12,
    };

    lmNotesConstrains* m_pConstrains;       //constraints for the exercise

    //problem asked
    lmEKeySignatures  m_nKey;

    //answer
    wxButton*       m_pAnswerButton[m_NUM_BUTTONS];     //buttons for the answers
    wxString        m_pButtonLabel[m_NUM_BUTTONS];

    //specific controls and data
    lmUrlAuxCtrol*      m_pPlayA4;          //"Play A4 reference note" link
    lmUrlAuxCtrol*      m_pPlayAllNotes;    //"Play all notes to identify" link
    lmUrlAuxCtrol*      m_pContinue;        //"Continue" link

    DECLARE_EVENT_TABLE()
};



#endif  // __LM_IDFYNOTESCTROL_H__
