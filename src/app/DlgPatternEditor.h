//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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

#ifndef __LM_DLGPATTERNEDITOR_H__        //to avoid nested includes
#define __LM_DLGPATTERNEDITOR_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "DlgPatternEditor.cpp"
#endif

// headers
#include "wx/dialog.h"


// class definition
class lmDlgPatternEditor : public wxDialog {

public:
    lmDlgPatternEditor(wxWindow * parent);
    virtual ~lmDlgPatternEditor();

    // event handlers
    void OnAcceptClicked(wxCommandEvent& WXUNUSED(event));
    void OnCancelClicked(wxCommandEvent& WXUNUSED(event)) { EndDialog(wxID_CANCEL); }
    void OnNoteClicked(wxCommandEvent& event);
    void OnRestClicked(wxCommandEvent& event);
    void OnClearAllClicked(wxCommandEvent& event);

    void OnDottedClicked(wxCommandEvent& event);
    void OnTiedNextClicked(wxCommandEvent& event);
    void OnNotBeamedClicked(wxCommandEvent& event);
    void OnStartBeamClicked(wxCommandEvent& event);
    void OnContinueBeamClicked(wxCommandEvent& event);
    void OnEndBeamClicked(wxCommandEvent& event);

private:
    void AddElement(wxString sStart);
    void EnableButtons();

    //controls
    wxBitmapButton*     m_pBtnNote[7];
    wxBitmapButton*     m_pBtnRest[7];
    wxTextCtrl*         m_pTxtPattern;

    wxBitmapButton*     m_pBtnDotted;
    wxBitmapButton*     m_pBtnTiedNext;
    wxBitmapButton*     m_pBtnNotBeamed;
    wxBitmapButton*     m_pBtnStartBeam;
    wxBitmapButton*     m_pBtnContinueBeam;
    wxBitmapButton*     m_pBtnEndBeam;

    //variables
    bool        m_fDotted;
    bool        m_fTiedNext;
    bool        m_fStartBeam;
    bool        m_fEndBeam;
    bool        m_fInBeam;


    DECLARE_EVENT_TABLE()
};

#endif    // __LM_DLGPATTERNEDITOR_H__
