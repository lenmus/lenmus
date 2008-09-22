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

#ifndef __LM_DLGCFGIDFYCADENCE_H__        //to avoid nested includes
#define __LM_DLGCFGIDFYCADENCE_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "DlgCfgIdfyCadence.cpp"
#endif

// headers
#include "wx/dialog.h"

#include "../exercises/CadencesConstrains.h"

// class definition
class lmDlgCfgIdfyCadence : public wxDialog {

public:
    lmDlgCfgIdfyCadence(wxWindow* parent, lmCadencesConstrains* pConstrains,
                      bool fTheoryMode);
    virtual ~lmDlgCfgIdfyCadence();

    // event handlers
    void OnAcceptClicked(wxCommandEvent& WXUNUSED(event));
    void OnCancelClicked(wxCommandEvent& WXUNUSED(event)) { EndDialog(wxID_CANCEL); }
    void OnDataChanged(wxCommandEvent& WXUNUSED(event));
    void OnRadAnswerType(wxCommandEvent& WXUNUSED(event));
    void OnCheckAllPerfect(wxCommandEvent& WXUNUSED(event));
    void OnCheckAllPlagal(wxCommandEvent& WXUNUSED(event));
    void OnCheckAllHalf(wxCommandEvent& WXUNUSED(event));
    void OnCheckAllDeceptive(wxCommandEvent& WXUNUSED(event));
    void OnCheckAllMajor(wxCommandEvent& WXUNUSED(event));
    void OnCheckAllMinor(wxCommandEvent& WXUNUSED(event));
    void OnAnswerButton(wxCommandEvent& WXUNUSED(event));



private:
    bool VerifyData();
    void SetCadenceCheckBoxes(int iCad, bool fEnable, bool fChangeCheck=false,
							  bool fNewCheckValue=false);
    void SetAnswerButton(int iButton, bool fEnable);


    lmCadencesConstrains*   m_pConstrains;      // the constraints to set up
    bool                    m_fTheoryMode;      // exercise type: theory / aural training

    //controls
    wxCheckBox*     m_pChkCadence[lm_eCadMaxCadence];   // Allowed cadences check boxes
    wxCheckBox*     m_pChkKeySign[lmMAX_KEY+1];         // Allowed key signatures check boxes
    wxRadioBox*     m_pBoxShowKey;                  // box with show key radio buttons
    wxRadioBox*     m_pBoxAnswerType;               // box with answer type radio buttons
    wxCheckBox*     m_pChkAnswerButton[5];          // Perfect, Plagal, Half, Deceptive, Imperfect
	wxButton*       m_pBtnCheckAll[5];			    // idem.

    wxStaticBitmap* m_pBmpKeySignError;
    wxStaticText*   m_pLblKeySignError;
    wxStaticBitmap* m_pBmpAllowedCadencesError;
    wxStaticText*   m_pLblAllowedCadencesError;
    wxStaticBitmap* m_pBmpButtonsError;
    wxStaticText*   m_pLblButtonsError;


    DECLARE_EVENT_TABLE()
};

#endif    // __LM_DLGCFGIDFYCADENCE_H__
