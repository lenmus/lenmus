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

#ifndef __LM_IDFYCHORDCTROL_H__        //to avoid nested includes
#define __LM_IDFYCHORDCTROL_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "IdfyChordCtrol.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "ChordConstrains.h"
#include "../score/Score.h"
#include "ExerciseCtrol.h"



class lmIdfyChordCtrol : public lmOneScoreCtrol     
{
public:

    // constructor and destructor    
    lmIdfyChordCtrol(wxWindow* parent, wxWindowID id,
               lmChordConstrains* pConstrains, 
               const wxPoint& pos = wxDefaultPosition, 
               const wxSize& size = wxDefaultSize, int style = 0);

    ~lmIdfyChordCtrol();

protected:
    //implementation of virtual methods
    void InitializeStrings();
    void CreateAnswerButtons(int nHeight, int nSpacing, wxFont& font);
    void PrepareAuxScore(int nButton);
    wxString SetNewProblem();    
    wxDialog* GetSettingsDlg();
    void ReconfigureButtons();

private:
    wxString PrepareScore(lmEClefType nClef, lmEChordType nType, lmScore** pScore);

        // member variables

    enum {
        m_NUM_COLS = 4,
        m_NUM_ROWS = 5,
        m_NUM_BUTTONS = 20,     // NUM_COLS * NUM_ROWS;
    };

    lmChordConstrains* m_pConstrains;       //constraints for the exercise

    //problem asked
    lmEKeySignatures  m_nKey;
    wxString        m_sRootNote;
    int             m_nInversion;
    int             m_nMode;

    //answer
    wxButton*       m_pAnswerButton[m_NUM_BUTTONS]; //buttons for the answers
    int             m_nRealChord[m_NUM_BUTTONS];    //chord associated to each valid button
    wxStaticText*   m_pRowLabel[m_NUM_ROWS];

    DECLARE_EVENT_TABLE()
};



#endif  // __LM_IDFYCHORDCTROL_H__
