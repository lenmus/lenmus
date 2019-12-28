//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2015 LenMus project
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
#include "lenmus_tool_page_rhythmic_dictation.h"

#include "lenmus_tool_box.h"
#include "lenmus_tool_group.h"
#include "lenmus_button.h"
#include "lenmus_utilities.h"
#include "lenmus_tool_page_barlines.h"

//wxWidgets
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
    #include <wx/xrc/xmlres.h>
    #include <wx/bmpcbox.h>
    #include <wx/statline.h>
    #include <wx/clrpicker.h>
#endif


namespace lenmus
{


#define lmSPACING 5

//event IDs
#define k_num_note_buttons          5   //w, h, q, e, s
#define k_num_cursor_buttons        2
#define k_num_options_buttons       3   //dot, note/rest,   8

enum {
	k_id_button_note = 2600,
	k_id_button_note_last = k_id_button_note + k_num_note_buttons -1,
	k_id_button_cursor,
	k_id_button_cursor_last= k_id_button_cursor + k_num_cursor_buttons -1,
    k_id_button_options,
	k_id_button_options_last= k_id_button_options + k_num_options_buttons -1,
};

//=======================================================================================
// ToolPageRhythmicDictation implementation
//=======================================================================================

wxIMPLEMENT_DYNAMIC_CLASS(ToolPageRhythmicDictation, ToolPage);

//---------------------------------------------------------------------------------------
ToolPageRhythmicDictation::ToolPageRhythmicDictation(wxWindow* parent)
{
    Create(parent);
}

//---------------------------------------------------------------------------------------
void ToolPageRhythmicDictation::Create(wxWindow* parent)
{
    ToolPage::CreatePage(parent, k_page_rhythmic_dictation);
    create_tool_groups();

    m_sPageToolTip = "";        //Not used
    m_sPageBitmapName = "";     //Not used
    m_title = _("Rhythm");
    m_selector = _("Rhythmic dictation exercise");
}

//---------------------------------------------------------------------------------------
void ToolPageRhythmicDictation::create_tool_groups()
{
    wxBoxSizer *pMainSizer = GetMainSizer();

    add_group( LENMUS_NEW GrpNoteDuration(this, pMainSizer) );
    add_group( LENMUS_NEW GrpNoteDots(this, pMainSizer) );
    add_group( LENMUS_NEW GrpBarlines2(this, pMainSizer) );


	create_layout();
	select_group(k_grp_NoteDuration);
}

//---------------------------------------------------------------------------------------
int ToolPageRhythmicDictation::translate_key(int key, int keyFlags)
{
    KeyTranslator* pTr = get_key_translator();
    int cmd = pTr->translate(k_key_context_dictation, key, keyFlags);
    if (cmd == k_cmd_null)
        cmd = pTr->translate(k_key_context_barlines, key, keyFlags);

    return cmd;
}

////---------------------------------------------------------------------------------------
//wxString ToolPageRhythmicDictation::GetToolShortDescription()
//{
//    //returns a short description of the selected tool. This description is used to
//    //be displayed in the status bar
//
//    return _("Add barline");
//}


//=======================================================================================
// GrpRhythmicDictation implementation
//=======================================================================================

wxBEGIN_EVENT_TABLE(GrpRhythmicDictation, ToolGroup)
    EVT_COMMAND_RANGE (k_id_button_note, k_id_button_note_last,
                       wxEVT_COMMAND_BUTTON_CLICKED,
                       GrpRhythmicDictation::on_note_button)
    EVT_COMMAND_RANGE (k_id_button_cursor, k_id_button_cursor_last,
                       wxEVT_COMMAND_BUTTON_CLICKED,
                       GrpRhythmicDictation::on_cursor_button)
    EVT_COMMAND_RANGE (k_id_button_options, k_id_button_options_last,
                       wxEVT_COMMAND_BUTTON_CLICKED,
                       GrpRhythmicDictation::on_options_button)
wxEND_EVENT_TABLE()

//---------------------------------------------------------------------------------------
GrpRhythmicDictation::GrpRhythmicDictation(ToolPage* pParent, wxBoxSizer* WXUNUSED(pMainSizer))
        : ToolGroup(pParent, k_group_type_tool_selector, pParent->GetColors())
        , m_selectedToolID(k_tool_none)
{
}

//---------------------------------------------------------------------------------------
void GrpRhythmicDictation::create_controls_in_group(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group
    set_group_title(_("Rhythmic dictation tools"));
    wxBoxSizer* pCtrolsSizer = create_main_sizer_for_group(pMainSizer);

    //create the specific controls for this group

    //buttons for adding notes
        //whole note/rest
	wxBoxSizer* pRow1Sizer = new wxBoxSizer( wxHORIZONTAL );

    wxSize btSize(24, 24);
    CheckButton* pButton;
	pButton = new CheckButton(this, k_id_button_note, wxBitmap(24,24),
                              wxDefaultPosition, wxSize(24, 24));
    pButton->SetBitmapUp("tie", "", btSize);
    pButton->SetBitmapDown("tie", "button_selected_flat", btSize);
    pButton->SetBitmapOver("tie", "button_over_flat", btSize);
    pButton->SetBitmapDisabled("tie_dis", "", btSize);
    pButton->SetToolTip(_("Add/remove a tie to/from selected notes"));
	pRow1Sizer->Add( pButton, wxSizerFlags(0).Border(wxALL, 2) );

        //half note/rest
	pButton = new CheckButton(this, k_id_button_note+1, wxBitmap(24,24),
                              wxDefaultPosition, wxSize(24, 24));
    pButton->SetBitmapUp("tuplet", "", btSize);
    pButton->SetBitmapDown("tuplet", "button_selected_flat", btSize);
    pButton->SetBitmapOver("tuplet", "button_over_flat", btSize);
    pButton->SetBitmapDisabled("tuplet_dis", "", btSize);
    pButton->SetToolTip(_("Add/remove tuplet to/from selected notes"));
	pRow1Sizer->Add( pButton, wxSizerFlags(0).Border(wxALL, 2) );

        //quarter note/rest
	pButton = new CheckButton(this, k_id_button_note+2, wxBitmap(24,24),
                              wxDefaultPosition, wxSize(24, 24));
    pButton->SetBitmapUp("toggle_stem", "", btSize);
    pButton->SetBitmapDown("toggle_stem", "button_selected_flat", btSize);
    pButton->SetBitmapOver("toggle_stem", "button_over_flat", btSize);
    pButton->SetBitmapDisabled("toggle_stem_dis", "", btSize);
    pButton->SetToolTip(_("Toggle stem in selected notes"));
	pRow1Sizer->Add( pButton, wxSizerFlags(0).Border(wxALL, 2) );

    pCtrolsSizer->Add( pRow1Sizer, 0, wxEXPAND, 5 );
    //    wxButton* pButton;
    //    for (int i=0; i < 4; ++i)
    //    {
    //        pButton = LENMUS_NEW wxButton(this, k_id_button_0 + i, _("11"));
    //    //        m_clefs[i] = LENMUS_NEW wxBitmapButton(this, k_id_button+i, create_bitmap(i),
    //    //                                               wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
    //        pCtrolsSizer->Add( pButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
    //    }

	//buttons for moving cursor
	wxBoxSizer* pRow2Sizer = new wxBoxSizer( wxHORIZONTAL );

	pButton = new CheckButton(this, k_id_button_cursor, wxBitmap(24,24),
                              wxDefaultPosition, wxSize(24, 24));
    pButton->SetBitmapUp("toggle_stem", "", btSize);
    pButton->SetBitmapDown("toggle_stem", "button_selected_flat", btSize);
    pButton->SetBitmapOver("toggle_stem", "button_over_flat", btSize);
    pButton->SetBitmapDisabled("toggle_stem_dis", "", btSize);
    pButton->SetToolTip(_("Toggle stem in selected notes"));
	pRow2Sizer->Add( pButton, wxSizerFlags(0).Border(wxALL, 2) );

    pCtrolsSizer->Add( pRow2Sizer, 0, wxEXPAND, 5 );

	//other commands: backdelete, delete & insert barline

	//push button for dots

	this->Layout();
}

//---------------------------------------------------------------------------------------
void GrpRhythmicDictation::on_note_button(wxCommandEvent& event)
{
    m_selectedToolID = k_tool_note;

    switch (event.GetId())
    {
        case k_id_button_note:    m_duration = k_duration_whole;    break;
        case k_id_button_note+1:  m_duration = k_duration_half;     break;
        case k_id_button_note+2:  m_duration = k_duration_quarter;  break;
        case k_id_button_note+3:  m_duration = k_duration_eighth;   break;
        case k_id_button_note+4:  m_duration = k_duration_16th;     break;
    }

    //generate key command
}

//---------------------------------------------------------------------------------------
void GrpRhythmicDictation::on_cursor_button(wxCommandEvent& event)
{
    m_selectedToolID = k_tool_note_tie;
    PostToolBoxEvent(m_selectedToolID, event.IsChecked());
}

//---------------------------------------------------------------------------------------
void GrpRhythmicDictation::on_options_button(wxCommandEvent& event)
{
    m_selectedToolID = k_tool_note_tuplet;
    PostToolBoxEvent(m_selectedToolID, event.IsChecked());
}

//---------------------------------------------------------------------------------------
void GrpRhythmicDictation::update_tools_info(ToolsInfo* WXUNUSED(pInfo))
{
    //TODO: define ToolsInfo options for this group
}

//---------------------------------------------------------------------------------------
void GrpRhythmicDictation::synchronize_with_cursor(bool fEnable, DocCursor* WXUNUSED(pCursor))
{
    //TODO
    EnableGroup(fEnable);
}

//---------------------------------------------------------------------------------------
void GrpRhythmicDictation::synchronize_with_selection(bool fEnable,
                                                  SelectionSet* WXUNUSED(pSelection))
{
    //TODO
    EnableGroup(fEnable);
}



}   //namespace lenmus
