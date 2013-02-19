//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2013 LenMus project
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

//lenmus
#include "lenmus_command_window.h"
#include "lenmus_standard_header.h"

//#include "lenmus_paths.h"
#include "lenmus_main_frame.h"
//#include "lenmus_string.h"

//wxWidgets
#ifndef WX_PRECOMP
    #include <wx/utils.h>
    #include <wx/textctrl.h>
    #include <wx/intl.h>
    #include <wx/sizer.h>
#endif
#include <wx/msgdlg.h>


namespace lenmus
{

//IDs for controls
const int k_id_text = wxNewId();


BEGIN_EVENT_TABLE(CommandWindow, wxPanel)
    EVT_TEXT_ENTER(k_id_text, CommandWindow::on_enter)
END_EVENT_TABLE()


//=======================================================================================
// CommandWindow implementation
//=======================================================================================
CommandWindow::CommandWindow(wxWindow* parent)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize)
{
    wxBeginBusyCursor();

    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

    m_textctrl = new wxTextCtrl(this, k_id_text, wxEmptyString,
                                wxDefaultPosition, wxDefaultSize,
                                wxTE_PROCESS_ENTER);

    topsizer->Add(m_textctrl, wxSizerFlags().Expand());


    SetAutoLayout( true );
    SetSizer( topsizer );

    topsizer->SetSizeHints( this );
    topsizer->Fit( this );

    m_textctrl->SetSelection(-1, -1);
    m_textctrl->SetFocus();

    wxEndBusyCursor();
}

//---------------------------------------------------------------------------------------
void CommandWindow::on_enter(wxCommandEvent& WXUNUSED(event))
{
    wxCommandEvent myEvent(LM_EVT_EDIT_COMMAND, k_id_edit_command);
    myEvent.SetString( get_value() );
    ::wxPostEvent(this, myEvent);

    m_textctrl->SetValue(wxEmptyString);
}

//---------------------------------------------------------------------------------------
wxString CommandWindow::get_value() const
{
    return m_textctrl->GetValue();
}


}   //namespace lenmus
