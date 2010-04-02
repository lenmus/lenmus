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

#ifndef __LM_DLGCFGEARINTERVALS_H__        //to avoid nested includes
#define __LM_DLGCFGEARINTERVALS_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "DlgCfgEarIntervals.cpp"
#endif

// headers
#include "wx/dialog.h"
#include "wx/spinctrl.h"        //to use wxSpinCtrl
#include "wx/notebook.h"

#include "../EarIntvalConstrains.h"

// class definition
class lmDlgCfgEarIntervals : public wxDialog {

public:
    lmDlgCfgEarIntervals(wxWindow * parent, lmEarIntervalsConstrains* pConstrains,
        bool fEnableFirstEqual = false);
    virtual ~lmDlgCfgEarIntervals();

    // event handlers
    void OnAcceptClicked(wxCommandEvent& WXUNUSED(event));
    void OnCancelClicked(wxCommandEvent& WXUNUSED(event)) { EndDialog(wxID_CANCEL); }
    void OnCboFromNote(wxCommandEvent& WXUNUSED(event));
    void OnCboToNote(wxCommandEvent& WXUNUSED(event));
    void OnPageChanging(wxNotebookEvent& event);
    void OnChkKeyClicked(wxCommandEvent& WXUNUSED(event));
    void OnChkIntvalTypeClicked(wxCommandEvent& WXUNUSED(event));
    void OnRadAccidentalsClicked(wxCommandEvent& WXUNUSED(event));
    void OnChkIntvalClicked(wxCommandEvent& WXUNUSED(event));

    //void AppendText(wxString sText);
    void EnableKeySignCheckBoxes(bool fEnable);


private:
    bool VerifyData();

    lmEarIntervalsConstrains*   m_pConstrains;      // the constraints to set up
    bool            m_fEnableFirstEqual;

    //controls
    wxComboBox*     m_pCboFromNote;
    wxComboBox*     m_pCboToNote;
    wxCheckBox*     m_pChkStartSameNote;

    wxStaticBitmap* m_pBmpRangeError;               // error icons and messages
    wxStaticText*   m_pLblRangeError;
    wxStaticBitmap* m_pBmpIntvalTypeError;
    wxStaticText*   m_pLblIntvalTypeError;
    wxStaticBitmap* m_pBmpKeySignError;
    wxStaticText*   m_pLblKeySignError;
    wxStaticBitmap* m_pBmpAllowedIntvalError;
    wxStaticText*   m_pLblAllowedIntvalError;
    wxStaticBitmap* m_pBmpGeneralError;
    wxStaticText*   m_pLblGeneralError;

    wxCheckBox*     m_pChkIntvalType[3];            // allowed types of intervals
    wxCheckBox*     m_pChkIntval[lmNUM_INTVALS];    // Allowed intervals check boxes
    wxCheckBox*     m_pChkKeySign[earmFa+1];        // Allowed key signatures check boxes


    DECLARE_EVENT_TABLE()
};

#endif    // __LM_DLGCFGEARINTERVALS_H__
