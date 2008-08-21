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

#ifndef __LM_DLGCFGTHEOINTERVALS_H__        //to avoid nested includes
#define __LM_DLGCFGTHEOINTERVALS_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "DlgCfgTheoIntervals.cpp"
#endif

// headers
#include "wx/dialog.h"
#include "wx/spinctrl.h"        //to use wxSpinCtrl

#include "../exercises/Constrains.h"
#include "../exercises/TheoIntervalsConstrains.h"

// class definition
class lmDlgCfgTheoIntervals : public wxDialog {

public:
    lmDlgCfgTheoIntervals(wxWindow * parent, lmTheoIntervalsConstrains* pConstrains);
    virtual ~lmDlgCfgTheoIntervals();

    // event handlers
    void OnAcceptClicked(wxCommandEvent& WXUNUSED(event));
    void OnCancelClicked(wxCommandEvent& WXUNUSED(event)) { EndDialog(wxID_CANCEL); }
    void OnControlClicked(wxCommandEvent& WXUNUSED(event));

private:
    bool VerifyData();

    lmTheoIntervalsConstrains*   m_pConstrains; // the constraints to set up

    //controls
    wxCheckBox*     m_pChkDoubleAccidentals;
    wxCheckBox*     m_pChkAccidentals;
    wxCheckBox*     m_pChkClef[7];              // Allowed clefs check boxes

    wxRadioBox*     m_radProblemType;

    wxStaticBitmap* m_pBmpClefError;            // error icons and messages
    wxStaticText*   m_pLblClefError;



    DECLARE_EVENT_TABLE()
};

#endif    // __LM_DLGCFGTHEOINTERVALS_H__
