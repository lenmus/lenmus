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
#include "lenmus_dlg_clefs.h"
#include "lenmus_utilities.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/defs.h>
#include <wx/button.h>
#include <wx/choice.h>
#include <wx/dialog.h>
#include <wx/intl.h>
#include <wx/sizer.h>
#include <wx/stattext.h>

#include <wx/xrc/xmlres.h>
#include <wx/hashmap.h>


namespace lenmus
{

//aux. class to contain clefs data
class ClefData
{
public:
    ClefData() {}
    ClefData(EClef type, wxString name)
        : nClefType(type), sClefName(name) {}

    EClef		nClefType;
    wxString    sClefName;
};

enum {
    lm_eNUM_CLEFS = 8,  //AWARE: change also DlgClefs::k_num_clefs in .h file
};

static ClefData m_tClefs[lm_eNUM_CLEFS];
static wxString m_language = "??";
static const wxString m_sLetters("abcdefghijklmnopqrstuvwxyz1234567890");

static const long k_id_button = wxNewId();

//---------------------------------------------------------------------------------------
wxBEGIN_EVENT_TABLE(DlgClefs, wxDialog)
    EVT_COMMAND_RANGE (k_id_button, k_id_button+k_num_clefs,
                       wxEVT_COMMAND_BUTTON_CLICKED, DlgClefs::on_button)
	EVT_CHAR_HOOK(DlgClefs::on_key_down)
wxEND_EVENT_TABLE()


//---------------------------------------------------------------------------------------
DlgClefs::DlgClefs(ApplicationScope& appScope, wxWindow* parent)
    : wxDialog(parent, wxID_ANY, _("Clefs"))
    , m_appScope(appScope)
    , m_index(-1)
{
    initialize_strings();
    create_dialog();
    SetFocus();
}

//---------------------------------------------------------------------------------------
void DlgClefs::initialize_strings()
{
    //load language dependent strings. Can not be statically initiallized because
    //then they do not get translated
    if (m_language != ApplicationScope::get_language())
    {
        //AWARE: When addign more clefs, update lm_eNUM_CLEFS;
        m_tClefs[0] = ClefData(k_clef_G2, _("G clef on 2nd line"));
        m_tClefs[1] = ClefData(k_clef_F4, _("F clef on 4th line"));
        m_tClefs[2] = ClefData(k_clef_F3, _("F clef on 3rd line"));
        m_tClefs[3] = ClefData(k_clef_C1, _("C clef on 1st line"));
        m_tClefs[4] = ClefData(k_clef_C2, _("C clef on 2nd line"));
        m_tClefs[5] = ClefData(k_clef_C3, _("C clef on 3rd line"));
        m_tClefs[6] = ClefData(k_clef_C4, _("C clef on 4th line"));
        m_tClefs[7] = ClefData(k_clef_percussion, _("Percussion clef"));
        //// other clefs not yet available
//        k_clef_C5,
//        k_clef_F5,
//        k_clef_G1,
//        k_clef_8_G2,        //8 above
//        k_clef_G2_8,        //8 below
//        k_clef_8_F4,        //8 above
//        k_clef_F4_8,        //8 below
//        k_clef_15_G2,       //15 above
//        k_clef_G2_15,       //15 below
//        k_clef_15_F4,       //15 above
//        k_clef_F4_15,       //15 below
        m_language = ApplicationScope::get_language();
    }
}

//---------------------------------------------------------------------------------------
void DlgClefs::create_dialog()
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* mainSizer = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* hSizer = nullptr;
	int i;
    for(i=0; i < k_num_clefs; i++)
    {
        if (i % 5 == 0)
        {
            hSizer = new wxBoxSizer( wxHORIZONTAL );
            mainSizer->Add( hSizer, 1, wxEXPAND, 5 );
        }
        wxBoxSizer* btSizer = new wxBoxSizer( wxVERTICAL );
        m_clefs[i] = LENMUS_NEW wxBitmapButton(this, k_id_button+i, create_bitmap(i),
                                               wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
        btSizer->Add( m_clefs[i], 0, wxTOP|wxRIGHT|wxLEFT|wxALIGN_CENTER_HORIZONTAL, 5 );

        m_letter[i] = new wxStaticText(this, wxID_ANY, m_sLetters[i], wxDefaultPosition,
                                       wxDefaultSize, 0 );
        btSizer->Add( m_letter[i], 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM|wxRIGHT|wxLEFT, 5 );

        hSizer->Add( btSizer, 1, wxEXPAND, 5 );
    }

    //add spacer and Cancel button
    if (i % 5 < 4)
    	hSizer->Add( 0, 0, 1, wxEXPAND, 5 );

    if (i % 5 == 4)
    {
        hSizer = new wxBoxSizer( wxHORIZONTAL );
        mainSizer->Add( hSizer, 1, wxEXPAND, 5 );
    }
    wxBoxSizer* btSizer = new wxBoxSizer( wxVERTICAL );
    wxButton* cancel = LENMUS_NEW wxButton(this, wxID_CANCEL, _("Cancel"));
    btSizer->Add( cancel, 0, wxTOP|wxRIGHT|wxLEFT|wxALIGN_CENTER_HORIZONTAL, 5 );
    m_letter[i] = new wxStaticText(this, wxID_ANY, "Esc", wxDefaultPosition,
                                   wxDefaultSize, 0 );
    btSizer->Add( m_letter[i], 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
    hSizer->Add( btSizer, 1, wxALIGN_CENTER_VERTICAL, 5  );

	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
}

//---------------------------------------------------------------------------------------
wxBitmap DlgClefs::create_bitmap(int i)
{
    return generate_bitmap_for_clef_ctrol(m_appScope, m_tClefs[i].sClefName,
                                          m_tClefs[i].nClefType);
}

//---------------------------------------------------------------------------------------
void DlgClefs::on_button(wxCommandEvent& event)
{
    int i = event.GetId() - k_id_button;
    m_index = m_tClefs[i].nClefType;
    EndModal(wxID_OK);
}

//---------------------------------------------------------------------------------------
void DlgClefs::on_key_down(wxKeyEvent& event)
{
    int ch = event.GetKeyCode();
    if (ch == WXK_ESCAPE)
    {
        EndModal(wxID_CANCEL);
        return;
    }

    int i = m_sLetters.Find( wxUniChar(ch) );
    if (i == wxNOT_FOUND)
    {
        wxString letters = m_sLetters.Upper();
        i = letters.Find( wxUniChar(ch) );
    }

    //wxMessageBox(wxString::Format("OnKeyDown in DlgClefs. key=%d, i=%d", ch, i));
    if (i == wxNOT_FOUND)
        return;     //ignore key

    m_index = m_tClefs[i].nClefType;
    EndModal(wxID_OK);
}


}  //namespace lenmus
