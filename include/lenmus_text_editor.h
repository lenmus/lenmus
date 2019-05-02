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

#ifndef __LENMUS_TEXT_EDITOR_H__        //to avoid nested includes
#define __LENMUS_TEXT_EDITOR_H__

//lenmus
#include "lenmus_standard_header.h"

//lomse
#include <lomse_internal_model.h>
using namespace lomse;

//wxWidgets
//#define system ::system         //bypass for bug in wxcrtbase.h: "reference to 'system' is ambiguous"
#include <wx/dialog.h>
#undef system                   //bypass for bug in wxcrtbase.h: "reference to 'system' is ambiguous"

//forward declarations
class wxTextCtrl;
class wxWindow;
class wxToolBar;


namespace lenmus
{

//---------------------------------------------------------------------------------------
class TextEditor : public wxDialog
{
   wxDECLARE_DYNAMIC_CLASS(TextEditor);

protected:
    wxTextCtrl*     m_pTxtData;

public:
    TextEditor(wxWindow* parent, wxString sTitle, wxString sData=wxEmptyString);
    virtual ~TextEditor();

    //evnt handlers
    void on_ok_button(wxCommandEvent& WXUNUSED(event));
    void on_cancel_button(wxCommandEvent& WXUNUSED(event));
    //void on_enter_key(wxCommandEvent& WXUNUSED(event));

    void load_text(const wxString& sText);
    void load_paragraph(Document* pDoc, ImoParagraph* pImo);

    wxString get_text();

protected:
    wxToolBar* create_tool_bar();

    wxDECLARE_EVENT_TABLE();
};


}   // namespace lenmus

#endif    // __LENMUS_TEXT_EDITOR_H__
