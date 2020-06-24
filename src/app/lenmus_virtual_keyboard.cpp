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

//lenmus
#include "lenmus_virtual_keyboard.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/bmpbuttn.h>
#include <wx/button.h>


namespace lenmus
{


static const long k_id_button = wxNewId();

//---------------------------------------------------------------------------------------
wxBEGIN_EVENT_TABLE(VirtualKeyboard, wxPanel)
    EVT_COMMAND_RANGE (k_id_button, k_id_button + VirtualKeyboard::k_num_buttons,
                       wxEVT_COMMAND_BUTTON_CLICKED, VirtualKeyboard::on_button)
	EVT_CHAR_HOOK(VirtualKeyboard::on_key_down)
wxEND_EVENT_TABLE()


//---------------------------------------------------------------------------------------
VirtualKeyboard::VirtualKeyboard(wxWindow* parent, wxWindowID id, const wxPoint& pos,
                                 const wxSize& size, long style)
   : wxPanel(parent, id, pos, size, style)
{
    create_buttons();
    Show(true);
}

//---------------------------------------------------------------------------------------
VirtualKeyboard::~VirtualKeyboard()
{
}

//---------------------------------------------------------------------------------------
void VirtualKeyboard::create_buttons()
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* mainSizer = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* hSizer = nullptr;
	int i;
    for(i=0; i < k_num_buttons; i++)
    {
        if (i % 5 == 0)
        {
            hSizer = new wxBoxSizer( wxHORIZONTAL );
            mainSizer->Add( hSizer, 1, wxEXPAND, 5 );
        }
//        m_keys[i] = LENMUS_NEW wxBitmapButton(this, k_id_button+i, create_bitmap(i),
//                                               wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
        m_keys[i] = LENMUS_NEW wxButton(this, k_id_button+i, "XX",
                                               wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
        hSizer->Add( m_keys[i], 0, wxTOP|wxRIGHT|wxLEFT|wxALIGN_CENTER_HORIZONTAL, 5 );
    }

	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
}

//---------------------------------------------------------------------------------------
wxBitmap VirtualKeyboard::create_bitmap(int WXUNUSED(i))
{
//    return generate_bitmap_for_clef_ctrol(m_appScope, m_tClefs[i].sClefName,
//                                          m_tClefs[i].nClefType);
    return wxBitmap();  //nullptr;
}

//---------------------------------------------------------------------------------------
void VirtualKeyboard::on_button(wxCommandEvent& WXUNUSED(event))
{
//    int i = event.GetId() - k_id_button;
//    m_index = m_tClefs[i].nClefType;
//    EndModal(wxID_OK);
}

//---------------------------------------------------------------------------------------
void VirtualKeyboard::on_key_down(wxKeyEvent& WXUNUSED(event))
{
//    int ch = event.GetKeyCode();
//    if (ch == WXK_ESCAPE)
//    {
//        EndModal(wxID_CANCEL);
//        return;
//    }
//
//    int i = sLetters.Find(ch);
//    if (i == wxNOT_FOUND)
//    {
//        wxString letters = sLetters.Upper();
//        i = letters.Find(ch);
//    }
//
//    //wxMessageBox(wxString::Format("OnKeyDown in VirtualKeyboard. key=%d, i=%d", ch, i));
//    if (i == wxNOT_FOUND)
//        return;     //ignore key
//
//    m_index = m_tClefs[i].nClefType;
//    EndModal(wxID_OK);
}


}   //namespace lenmus
