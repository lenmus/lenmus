//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2010-2015 LenMus project
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
//---------------------------------------------------------------------------------------

#ifndef __LENMUS_DLG_CFG_IDFY_NOTES_H__        //to avoid nested includes
#define __LENMUS_DLG_CFG_IDFY_NOTES_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_notes_constrains.h"

//wxWidgets
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/radiobut.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/stattext.h>
#include <wx/statbox.h>
#include <wx/panel.h>
#include <wx/radiobox.h>
#include <wx/notebook.h>
#include <wx/button.h>
#include <wx/dialog.h>


namespace lenmus
{

//---------------------------------------------------------------------------------------
class DlgCfgIdfyNotes : public wxDialog
{
public:
    DlgCfgIdfyNotes(wxWindow* parent, NotesConstrains* pConstrains);
    virtual ~DlgCfgIdfyNotes();

    // event handlers
    void OnAcceptClicked(wxCommandEvent& WXUNUSED(event));
    void OnCancelClicked(wxCommandEvent& WXUNUSED(event)) { EndDialog(wxID_CANCEL); }
    void OnDataChanged(wxCommandEvent& WXUNUSED(event));
    void OnRadioFromKeySignature(wxCommandEvent& WXUNUSED(event));
    void OnRadioSelectedNotes(wxCommandEvent& WXUNUSED(event));

protected:
    bool VerifyData();
    void create_controls();
    void EnableDisableNotesSelection(bool fEnable);


    NotesConstrains*   m_pConstrains;      // the constraints to set up

    //controls
    wxCheckBox*     m_pChkNote[12];     // Allowed notes check boxes

		wxNotebook* m_pNoteBook;
		wxPanel* m_pPanelNotes;
		wxRadioButton* m_pRadFromKeySignature;

		wxChoice* m_pCboKeySignature;
		wxRadioButton* m_pRadSelectedNotes;

		wxCheckBox* m_pChkC;
		wxCheckBox* m_pChkCSharp;
		wxCheckBox* m_pChkD;
		wxCheckBox* m_pChkDSharp;
		wxCheckBox* m_pChkE;
		wxCheckBox* m_pChkF;
		wxCheckBox* m_pChkFSharp;
		wxCheckBox* m_pChkG;
		wxCheckBox* m_pChkGSharp;
		wxCheckBox* m_pChkA;
		wxCheckBox* m_pChkASharp;
		wxCheckBox* m_pChkB;
		wxStaticBitmap* m_pBmpNotesError;
		wxStaticText* m_pLblNotesError;
		wxStaticText* m_pSpaceNotes;
		wxPanel* m_pPanelClef;
		wxRadioBox* m_pRadClefs;
		wxPanel* m_pPanelOther;
		wxRadioBox* m_pRadOctaves;
		wxButton* m_pBtnAccept;
		wxButton* m_pBtnCancel;


    wxDECLARE_EVENT_TABLE();
};


}   //namespace lenmus

#endif    // __LENMUS_DLG_CFG_IDFY_NOTES_H__
