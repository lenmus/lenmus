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

#ifndef __LM_DLGCFGIDFYCHORD_H__        //to avoid nested includes
#define __LM_DLGCFGIDFYCHORD_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "DlgCfgIdfyChord.cpp"
#endif

// headers
#include <wx/dialog.h>
#include <wx/spinctrl.h>        //to use wxSpinCtrl

#include "../ChordConstrains.h"

// class definition
class lmDlgCfgIdfyChord : public wxDialog {

public:
    lmDlgCfgIdfyChord(wxWindow* parent, lmChordConstrains* pConstrains,
                      bool fTheoryMode);
    virtual ~lmDlgCfgIdfyChord();

    // event handlers
    void OnAcceptClicked(wxCommandEvent& WXUNUSED(event));
    void OnCancelClicked(wxCommandEvent& WXUNUSED(event)) { EndDialog(wxID_CANCEL); }
    void OnDataChanged(wxCommandEvent& WXUNUSED(event));


private:
    bool VerifyData();

    lmChordConstrains*  m_pConstrains;          // the constraints to set up
    bool                m_fTheoryMode;

    //controls
    wxCheckBox*     m_pChkAllowInversions;
    wxCheckBox*     m_pChkDisplayKey;
    wxCheckBox*     m_pChkPlayMode[3];          // allowed play modes
    wxCheckBox*     m_pChkChord[ect_MaxInExercises];       // Allowed chords check boxes
    wxCheckBox*     m_pChkKeySign[earmFa+1];    // Allowed key signatures check boxes
    wxStaticBox*    m_pBoxPlayModes;            // box with play mode check boxes

    wxStaticBitmap* m_pBmpPlayModeError;        // error icons and messages      
    wxStaticText*   m_pLblPlayModeError;
    wxStaticBitmap* m_pBmpKeySignError;
    wxStaticText*   m_pLblKeySignError;
    wxStaticBitmap* m_pBmpAllowedChordsError;
    wxStaticText*   m_pLblAllowedChordsError;


    DECLARE_EVENT_TABLE()
};

#endif    // __LM_DLGCFGIDFYCHORD_H__
