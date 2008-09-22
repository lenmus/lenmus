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

#ifndef __LM_DLGCFGIDFYSCALE_H__        //to avoid nested includes
#define __LM_DLGCFGIDFYSCALE_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "DlgCfgIdfyScale.cpp"
#endif

// headers
#include "wx/dialog.h"

#include "../exercises/ScalesConstrains.h"

// class definition
class lmDlgCfgIdfyScale : public wxDialog {

public:
    lmDlgCfgIdfyScale(wxWindow* parent, lmScalesConstrains* pConstrains,
                      bool fTheoryMode);
    virtual ~lmDlgCfgIdfyScale();

    // event handlers
    void OnAcceptClicked(wxCommandEvent& WXUNUSED(event));
    void OnCancelClicked(wxCommandEvent& WXUNUSED(event)) { EndDialog(wxID_CANCEL); }
    void OnDataChanged(wxCommandEvent& WXUNUSED(event));


private:
    bool VerifyData();

    lmScalesConstrains*  m_pConstrains;          // the constraints to set up
    bool                m_fTheoryMode;

    //controls
    wxCheckBox*     m_pChkScale[est_Max];       // Allowed chords check boxes
    wxCheckBox*     m_pChkKeySign[earmFa+1];    // Allowed key signatures check boxes
    wxRadioBox*     m_pBoxPlayModes;            // box with play mode radio buttons
    wxCheckBox*     m_pChkDisplayKey;           // Display key signature check box

    wxStaticBitmap* m_pBmpKeySignError;
    wxStaticText*   m_pLblKeySignError;
    wxStaticBitmap* m_pBmpAllowedScalesError;
    wxStaticText*   m_pLblAllowedScalesError;


    DECLARE_EVENT_TABLE()
};

#endif    // __LM_DLGCFGIDFYSCALE_H__
