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

#ifndef __LENMUS_TOOL_BARLINES_H__
#define __LENMUS_TOOL_BARLINES_H__

//lenmus
#include "lenmus_tool_group.h"
#include "lenmus_tool_page.h"
//#include "../../score/defs.h"

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
// Group for selecting barline type, using image list
//---------------------------------------------------------------------------------------
class GrpBarlines : public ToolGroup
{
public:
    GrpBarlines(ToolPage* pParent, wxBoxSizer* pMainSizer, int nValidMouseModes = 0xFFFF);
    ~GrpBarlines() {}

    //mandatory overrides
    void update_tools_info(ToolsInfo* pInfo);
    inline EToolGroupID get_group_id() { return k_grp_BarlineType; }
    EToolID get_selected_tool_id();
    void synchronize_with_cursor(bool fEnable, DocCursor* pCursor);
    void synchronize_with_selection(bool fEnable, SelectionSet* pSelection);

	//access to selected barline
	EBarline GetSelectedBarline();

	//event handlers
    void OnBarlinesList(wxCommandEvent& event);


private:
    void create_controls_in_group(wxBoxSizer* pMainSizer);

	wxBitmapComboBox*   m_pBarlinesList;

    wxDECLARE_EVENT_TABLE();
};

//---------------------------------------------------------------------------------------
// Group for selecting barline type, using buttons
//---------------------------------------------------------------------------------------
class GrpBarlines2 : public ToolButtonsGroup
{
public:
    GrpBarlines2(ToolPage* pParent, wxBoxSizer* pMainSizer, int nValidMouseModes = 0xFFFF);
    ~GrpBarlines2() {}

    //mandatory overrides
    void update_tools_info(ToolsInfo* pInfo);
    EToolGroupID get_group_id() { return k_grp_BarlineType; }
    void synchronize_with_cursor(bool fEnable, DocCursor* pCursor);
    void synchronize_with_selection(bool fEnable, SelectionSet* pSelection);
    void create_controls_in_group(wxBoxSizer* pMainSizer);

	//access to selected barline
	EBarline get_selected_barline();

	//event handlers
    void OnBarlinesList(wxCommandEvent& event);

protected:
    //overrides
    long get_key_translation_context() { return k_key_context_dictation; }
    bool process_key(wxKeyEvent& event);

    void set_buttons_bitmaps();
};



//---------------------------------------------------------------------------------------
// The page panel
//---------------------------------------------------------------------------------------
class ToolPageBarlines : public ToolPage
{
	wxDECLARE_DYNAMIC_CLASS(ToolPageBarlines);

public:
    ToolPageBarlines() {}
    ToolPageBarlines(wxWindow* parent);
    ~ToolPageBarlines() {}
    void Create(wxWindow* parent);

    //mandatory overrides
    void create_tool_groups();

//    //current tool/group info
//    wxString GetToolShortDescription();

private:
    //mandatory overrides
    long get_key_translation_context() { return k_key_context_barlines; }

};



}   // namespace lenmus

#endif    // __LENMUS_TOOL_BARLINES_H__
