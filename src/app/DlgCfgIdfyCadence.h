//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
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

#ifndef __DLGCFGIDFYCADENCE_H__        //to avoid nested includes
#define __DLGCFGIDFYCADENCE_H__

#if defined(__GNUG__) && !defined(__APPLE__)
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


private:
    bool VerifyData();

    lmCadencesConstrains*   m_pConstrains;      // the constraints to set up
    bool                    m_fTheoryMode;      // exercise type: theory / aural training

    //controls
    wxCheckBox*     m_pChkCadence[lm_eCadMaxCadence];   // Allowed cadences check boxes
    wxCheckBox*     m_pChkKeySign[earmFa+1];    // Allowed key signatures check boxes
    wxRadioBox*     m_pBoxShowKey;              // box with show key radio buttons
    wxRadioBox*     m_pBoxAnswerType;           // box with answer type radio buttons

    wxStaticBitmap* m_pBmpKeySignError;
    wxStaticText*   m_pLblKeySignError;
    wxStaticBitmap* m_pBmpAllowedCadencesError;
    wxStaticText*   m_pLblAllowedCadencesError;
    wxStaticBitmap* m_pBmpButtonsError;
    wxStaticText*   m_pLblButtonsError;


    DECLARE_EVENT_TABLE()
};

#endif    // __DLGCFGIDFYCADENCE_H__
