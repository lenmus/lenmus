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

#ifndef __LENMUS_TOOL_RHYTHMIC_DICTATION_H__
#define __LENMUS_TOOL_RHYTHMIC_DICTATION_H__

//lenmus
#include "lenmus_tool_group.h"
#include "lenmus_tool_page.h"
#include "lenmus_tool_page_notes.h"

//lomse
#include "lomse_internal_model.h"
using namespace lomse;

class wxBitmapComboBox;
class wxRadioBox;
class wxListBox;


using namespace std;


namespace lenmus
{

class CheckButton;


//---------------------------------------------------------------------------------------
// Group for rhythmic dictation exercises
//---------------------------------------------------------------------------------------
class GrpRhythmicDictation: public ToolGroup
{
protected:
    EToolID m_selectedToolID;
    int m_duration;

public:
    GrpRhythmicDictation(ToolPage* pParent, wxBoxSizer* pMainSizer);
    ~GrpRhythmicDictation() {}

    //mandatory overrides
    void update_tools_info(ToolsInfo* pInfo);
    EToolGroupID get_group_id() { return k_grp_rhythmic_dictation_tools; }
    EToolID get_selected_tool_id() { return m_selectedToolID; }
    void synchronize_with_cursor(bool fEnable, DocCursor* pCursor);
    void synchronize_with_selection(bool fEnable, SelectionSet* pSelection);

private:
    void create_controls_in_group(wxBoxSizer* pMainSizer);
    void on_note_button(wxCommandEvent& event);
    void on_cursor_button(wxCommandEvent& event);
    void on_options_button(wxCommandEvent& event);

    wxDECLARE_EVENT_TABLE();
};


//---------------------------------------------------------------------------------------
// The page panel
//---------------------------------------------------------------------------------------
class ToolPageRhythmicDictation : public ToolPage
{
	wxDECLARE_DYNAMIC_CLASS(ToolPageRhythmicDictation);

private:

public:
    ToolPageRhythmicDictation() {}
    ToolPageRhythmicDictation(wxWindow* parent);
    ~ToolPageRhythmicDictation() {}
    void Create(wxWindow* parent);

    //mandatory overrides
    void create_tool_groups();

    //overrides
    int translate_key(int key, int keyFlags);

//    //current tool/group info
//    wxString GetToolShortDescription();


protected:
    //mandatory overrides
    long get_key_translation_context() { return k_key_context_dictation; }

};



}   // namespace lenmus

#endif    // __LENMUS_TOOL_RHYTHMIC_DICTATION_H__
