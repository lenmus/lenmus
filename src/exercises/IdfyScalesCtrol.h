//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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

#ifndef __LM_IDFYSCALESCTROL_H__        //to avoid nested includes
#define __LM_IDFYSCALESCTROL_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "IdfyScalesCtrol.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "ScalesConstrains.h"
#include "../score/Score.h"
#include "ExerciseCtrol.h"



class lmIdfyScalesCtrol : public lmOneScoreCtrol    
{
public:

    // constructor and destructor    
    lmIdfyScalesCtrol(wxWindow* parent, wxWindowID id,
               lmScalesConstrains* pConstrains, 
               const wxPoint& pos = wxDefaultPosition, 
               const wxSize& size = wxDefaultSize, int style = 0);

    ~lmIdfyScalesCtrol();

    //implementation of virtual methods
    void InitializeStrings();
    void CreateAnswerButtons(int nHeight, int nSpacing, wxFont& font);
    void PrepareAuxScore(int nButton);
    wxString SetNewProblem();    
    wxDialog* GetSettingsDlg();
    void ReconfigureButtons();

private:
    wxString PrepareScore(lmEClefType nClef, EScaleType nType, lmScore** pScore);
    int ReconfigureGroup(int iBt, int iStartC, int iEndC, wxString sRowLabel);
    void DisableGregorianMajorMinor(EScaleType nType);

        // member variables

    enum {
        m_NUM_COLS = 4,
        m_NUM_ROWS = 6,
        m_NUM_BUTTONS = 24,     // NUM_COLS * NUM_ROWS;
    };

    lmScalesConstrains* m_pConstrains;       //constraints for the exercise

    //problem asked
    lmEKeySignatures  m_nKey;
    wxString        m_sRootNote;
    bool            m_fAscending;

    //answer
    wxStaticText*   m_pRowLabel[m_NUM_ROWS];
    wxButton*       m_pAnswerButton[m_NUM_BUTTONS];     //buttons for the answers
    int             m_nRealScale[m_NUM_BUTTONS];        //scale that corresponds
                                                        //   to each valid button

    DECLARE_EVENT_TABLE()
};



#endif  // __LM_IDFYSCALESCTROL_H__
