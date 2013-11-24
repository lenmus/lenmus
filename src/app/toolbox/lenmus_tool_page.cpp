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
#include "lenmus_tool_page.h"
#include "lenmus_tool_box_events.h"
#include "lenmus_tool_page_rhythmic_dictation.h"

//wxWidgets.
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/xrc/xmlres.h>
#include <wx/bmpcbox.h>
#include <wx/statline.h>
#include <wx/settings.h>



namespace lenmus
{


//=======================================================================================
// KeyTranslator implementation
//=======================================================================================
KeyTranslator::KeyTranslator(ApplicationScope& appScope)
    : m_appScope(appScope)
{
    //TODO: ask m_appScope for translation map

    //load maps with default assignments

    //context: any ----------------------------------------------------------------------
        //cursor
    m_any[ make_pair(WXK_UP, 0) ] = k_cmd_cursor_move_up;
    m_any[ make_pair(WXK_LEFT, 0)] = k_cmd_cursor_move_prev;
    m_any[ make_pair(WXK_RIGHT, 0)] = k_cmd_cursor_move_next;
    m_any[ make_pair(WXK_DOWN, 0)] = k_cmd_cursor_move_down;
    m_any[ make_pair(WXK_RETURN, 0)] = k_cmd_cursor_enter;
    m_any[ make_pair(WXK_RETURN, k_kbd_ctrl)] = k_cmd_cursor_exit;
    m_any[ make_pair(WXK_RIGHT, k_kbd_ctrl)] = k_cmd_cursor_to_next_measure;
    m_any[ make_pair(WXK_LEFT, k_kbd_ctrl)] = k_cmd_cursor_to_prev_measure;
    m_any[ make_pair(WXK_HOME, 0)] = k_cmd_cursor_to_start_of_system;
    m_any[ make_pair(WXK_END, 0)] = k_cmd_cursor_to_end_of_system;
    m_any[ make_pair(WXK_PAGEDOWN, 0)] = k_cmd_cursor_to_next_page;
    m_any[ make_pair(WXK_PAGEUP, 0)] = k_cmd_cursor_to_prev_page;
    m_any[ make_pair(WXK_UP, k_kbd_ctrl)] = k_cmd_cursor_to_first_staff;
    m_any[ make_pair(WXK_DOWN, k_kbd_ctrl)] = k_cmd_cursor_to_last_staff;
    m_any[ make_pair(WXK_HOME, k_kbd_ctrl)] = k_cmd_cursor_to_first_measure;
    m_any[ make_pair(WXK_END, k_kbd_ctrl)] = k_cmd_cursor_to_last_measure;
        //delete
    m_any[ make_pair(WXK_DELETE, 0)] = k_cmd_delete_selection_or_pointed_object;
    m_any[ make_pair(WXK_BACK, 0)] = k_cmd_move_prev_and_delete_pointed_object;
        //zoom
    m_any[ make_pair(int('+'), k_kbd_ctrl)] = k_cmd_zoom_in;
    m_any[ make_pair(int('-'), k_kbd_ctrl)] = k_cmd_zoom_out;

    //context: page Clefs ---------------------------------------------------------------
    m_clefs[ make_pair(int('Y'), 0)] = k_cmd_clef_ask;

    //context: page Notes ---------------------------------------------------------------
    m_notes[ make_pair(int('A'), 0)] = k_cmd_note_step_a;
    m_notes[ make_pair(int('B'), 0)] = k_cmd_note_step_b;
    m_notes[ make_pair(int('C'), 0)] = k_cmd_note_step_c;
    m_notes[ make_pair(int('D'), 0)] = k_cmd_note_step_d;
    m_notes[ make_pair(int('E'), 0)] = k_cmd_note_step_e;
    m_notes[ make_pair(int('F'), 0)] = k_cmd_note_step_f;
    m_notes[ make_pair(int('G'), 0)] = k_cmd_note_step_g;
    m_notes[ make_pair(int('Y'), 0)] = k_cmd_clef_ask;
        //increment/decrement/change octave
        //insert rest
    m_notes[ make_pair(int(' '), 0)] = k_cmd_rest;

    //context: page Barlines ------------------------------------------------------------
    m_barlines[ make_pair(int('B'), 0)] = k_cmd_barline;


    //Load command names
    m_names[k_cmd_null] = _("Unknown key command");
    m_names[k_cmd_cursor_move_prev] = _("Cursor move back");
    m_names[k_cmd_cursor_move_next] = _("Cursor move forward");
    m_names[k_cmd_cursor_enter] = _("Cursor enter");
    m_names[k_cmd_cursor_exit] = _("Cursr exit");
    m_names[k_cmd_cursor_move_up] = _("Cursor move up");
    m_names[k_cmd_cursor_move_down] = _("Cursor move down");
    m_names[k_cmd_cursor_to_start_of_system] = _("Cursor to first measure in system");
    m_names[k_cmd_cursor_to_end_of_system] = _("Cursor to last measure in system");
    m_names[k_cmd_cursor_to_next_page] = _("Cursor to next page");
    m_names[k_cmd_cursor_to_prev_page] = _("Cursor to prev. page");
    m_names[k_cmd_cursor_to_next_measure] = _("Cursor to next measure");
    m_names[k_cmd_cursor_to_prev_measure] = _("Cursor to prev. measure");
    m_names[k_cmd_cursor_to_first_staff] = _("Cursor to first staff, at current time");
    m_names[k_cmd_cursor_to_last_staff] = _("Cursor to last staff, at current time");
    m_names[k_cmd_cursor_to_first_measure] = _("Cursor to first measure");
    m_names[k_cmd_cursor_to_last_measure] = _("Cursor to last measure");
    m_names[k_cmd_delete_selection_or_pointed_object] = _("Delete selection");
    m_names[k_cmd_move_prev_and_delete_pointed_object] = _("Back and delete");
    m_names[k_cmd_zoom_in] = _("Zoom in");
    m_names[k_cmd_zoom_out] = _("Zoom out");
    m_names[k_cmd_clef_ask] = _("Display dialog for choosing clef and add choosen clef");
    m_names[k_cmd_note_step_a] = _("Add A note");
    m_names[k_cmd_note_step_b] = _("Add B note");
    m_names[k_cmd_note_step_c] = _("Add C note");
    m_names[k_cmd_note_step_d] = _("Add D note");
    m_names[k_cmd_note_step_e] = _("Add E note");
    m_names[k_cmd_note_step_f] = _("Add F note");
    m_names[k_cmd_note_step_g] = _("Add G note");
    m_names[k_cmd_rest] = _("Add rest");
    m_names[k_cmd_barline] = _("Add barline");
}

//---------------------------------------------------------------------------------------
KeyTranslator::~KeyTranslator()
{
    m_any.clear();
    m_notes.clear();
    m_clefs.clear();
    m_barlines.clear();
}

//---------------------------------------------------------------------------------------
int KeyTranslator::translate(int context, int key, unsigned flags)
{
    map< pair<int, unsigned>, EKeyCommands>* table;
    switch(context)
    {
        case k_key_context_none:        table = &m_any;         break;
        case k_key_context_any:         table = &m_any;         break;
        case k_key_context_clefs:       table = &m_clefs;       break;
        case k_key_context_notes:       table = &m_notes;       break;
        case k_key_context_note_rest:   table = &m_notes;       break;
        case k_key_context_barlines:    table = &m_barlines;    break;
        case k_key_context_symbols:     table = &m_notes;       break;
        default:                        table = &m_notes;
    }

//    {
//        stringstream msg;
//        msg << "Translating key. Context=" << context
//            << ", key=" << key << ", "
//            << (flags & k_kbd_alt ? "+Alt" : "")
//            << (flags & k_kbd_ctrl ? "+Ctrl" : "")
//            << (flags & k_kbd_shift ? "+Shift" : "");
//
//        LOMSE_LOG_INFO(msg.str());
//    }

    pair<int, unsigned> code = make_pair(key, flags);
    map< pair<int, unsigned>, EKeyCommands>::const_iterator it = table->find(code);
	if (it != table->end())
        return it->second;
    else if (context != k_key_context_none && context != k_key_context_any)
        return translate(k_key_context_any, key, flags);
    else
    {
        stringstream msg;
        msg << "Invalid key. Context=" << context
            << ", key=" << key << ", "
            << (flags & k_kbd_alt ? "+Alt" : "")
            << (flags & k_kbd_ctrl ? "+Ctrl" : "")
            << (flags & k_kbd_shift ? "+Shift" : "");

        LOMSE_LOG_ERROR(msg.str());
        return k_cmd_null;
    }
}

//---------------------------------------------------------------------------------------
wxString KeyTranslator::get_key_name(int keyCmd)
{
    map<int, wxString>::const_iterator it = m_names.find(keyCmd);
	if (it != m_names.end())
        return it->second;
    else
        return _T("Name not in table");
}



//=======================================================================================
// ToolPage implementation
//=======================================================================================
#define lmPANEL_WIDTH 150


BEGIN_EVENT_TABLE(ToolPage, wxPanel)
    //EVT_PAINT(ToolPage::OnPaintEvent)
END_EVENT_TABLE()

IMPLEMENT_ABSTRACT_CLASS(ToolPage, wxPanel)


//---------------------------------------------------------------------------------------
ToolPage::ToolPage()
{
}

//---------------------------------------------------------------------------------------
ToolPage::ToolPage(wxWindow* parent, EToolPageID nPageID)
{
    CreatePage(parent, nPageID);
}

//---------------------------------------------------------------------------------------
void ToolPage::CreatePage(wxWindow* parent, EToolPageID nPageID)
{
    //base class
    wxPanel::Create(parent, -1, wxPoint(0,0), wxDefaultSize,
                    wxBORDER_NONE|wxTAB_TRAVERSAL);

	//main sizer
    m_pMainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(m_pMainSizer);

	//set colors
	SetBackgroundColour(GetColors()->Normal());  //Bright());

    //initializations
    m_sPageToolTip = _T("");
    m_sPageBitmapName = _T("");
    m_nPageID = nPageID;
}

//---------------------------------------------------------------------------------------
ToolPage::~ToolPage()
{
    //delete groups
    std::list<ToolGroup*>::iterator it;
    for (it = m_groups.begin(); it != m_groups.end(); ++it)
    {
        delete *it;
    }
}

//---------------------------------------------------------------------------------------
void ToolPage::create_layout()
{
    //create groups' controls
    std::list<ToolGroup*>::iterator it;
    for (it = m_groups.begin(); it != m_groups.end(); ++it)
    {
        (*it)->create_controls_in_group(m_pMainSizer);
    }
    SetAutoLayout(true);
    m_pMainSizer->Fit(this);
    m_pMainSizer->SetSizeHints(this);
    m_pMainSizer->Layout();
}

//---------------------------------------------------------------------------------------
void ToolPage::on_tool_changed(EToolID toolID, EToolGroupID groupID)
{
    //deselect tools in any related groups to the one issuing the callback
    //and post tool box event to the active controller

    if (m_groups.empty())
        return;

    //if selected group is a tool selector one, deselect any other
    //tool-selector group in this page and save information about current
    //group and tool
    if (GetToolGroup(groupID)->is_tool_selector_group())
        DeselectRelatedGroups(groupID);

    //save info about current tool and current group
    m_nCurGroupID = groupID;
    m_nCurToolID = toolID;

    //post tool box event to the active controller
    ToolBoxToolSelectedEvent event(toolID, true /* tool is selected*/);
    ::wxPostEvent(this, event);
}

//---------------------------------------------------------------------------------------
ToolGroup* ToolPage::GetToolGroup(EToolGroupID nGroupID)
{
    //return pointer to group nGroupID

    std::list<ToolGroup*>::iterator it;
    for (it = m_groups.begin(); it != m_groups.end(); ++it)
    {
        if ((*it)->get_group_id() == nGroupID)
            return *it;
    }
    wxASSERT(false);
    return (ToolGroup*)NULL;      //compiler happy
}

//---------------------------------------------------------------------------------------
void ToolPage::DeselectRelatedGroups(EToolGroupID nGroupID)
{
    //When there are several groups in the same tool page (i.e, clefs, keys and
    //time signatures) the groups must behave as if they where a single 'logical
    //group', that is, selecting a tool in a group will deselect any tool on the
    //other related groups.
    //If selected group is a tool-selector one, this method is invoked to deselect
    //any other tool-selector group in this page.

    //select/deselect groups
    std::list<ToolGroup*>::iterator it;
    for (it = m_groups.begin(); it != m_groups.end(); ++it)
    {
        if ((*it)->is_tool_selector_group())
            (*it)->SetSelected((*it)->get_group_id() == nGroupID);
    }
}

//---------------------------------------------------------------------------------------
void ToolPage::select_group_and_notify(ToolGroup* pGroup)
{
    do_select_group(pGroup);

    //post tool box event to the active controller
    ToolBoxToolSelectedEvent event(m_nCurToolID, true /* tool is selected*/);
    ::wxPostEvent(this, event);
}

//---------------------------------------------------------------------------------------
void ToolPage::select_group(EToolGroupID groupID)
{
    std::list<ToolGroup*>::iterator it;
    for (it = m_groups.begin(); it != m_groups.end(); ++it)
    {
        if ((*it)->get_group_id() == groupID)
            do_select_group(*it);
    }
}

//---------------------------------------------------------------------------------------
void ToolPage::do_select_group(ToolGroup* pGroup)
{
    //Selects received group (it MUST be a tool-selector one) and deselects
    //any other tool-selector group in the same page. Sends a tool box event
    //to the active controller.


    //select/deselect groups
    //When there are several tool-selector groups in the same tool page (i.e, clefs,
    //keys and time signatures) the groups must behave as if they where a single
    //'logical group', that is, selecting a tool in a group will deselect any tool
    //on the other related groups.
    std::list<ToolGroup*>::iterator it;
    for (it = m_groups.begin(); it != m_groups.end(); ++it)
    {
        if ((*it)->is_tool_selector_group())
            (*it)->SetSelected(pGroup == *it);
    }

    //save information about current group and tool
    m_nCurGroupID = pGroup->get_group_id();
    m_nCurToolID = pGroup->get_selected_tool_id();
}

//---------------------------------------------------------------------------------------
void ToolPage::AddGroup(ToolGroup* pGroup)
{
    m_groups.push_back(pGroup);
}

//---------------------------------------------------------------------------------------
void ToolPage::add_group(ToolGroup* pGroup)
{
    m_groups.push_back(pGroup);
}

//---------------------------------------------------------------------------------------
void ToolPage::OnPaintEvent(wxPaintEvent & event)
{
#if 0
    wxPaintDC dc(this);
    wxRect rect;
    GetClientSize(&rect.width, &rect.height);

    //dc.SetBrush(*wxRED_BRUSH);
    //dc.DrawRectangle(rect);

    int nWidth1 = 40;
    wxRect rectBg = rect;
    rectBg.SetWidth(nWidth1);
    wxColour colorBg = GetColors()->GetColour(lmCOLOUR_GROUP_BACKGROUND_NORMAL);
    wxColour colorLigth = ToolboxTheme::LightenColour(colorBg, 0.4f);
    wxColour colorDark = ToolboxTheme::DarkenColour(colorBg, 0.15f);
    dc.GradientFillLinear(rectBg, colorDark, colorLigth, wxEAST );

    colorLigth = ToolboxTheme::LightenColour(colorBg, 0.3f);
    colorDark = ToolboxTheme::DarkenColour(colorBg, 0.1f);
    rectBg.SetWidth(rect.width - nWidth1);
    rectBg.x = nWidth1;
    dc.GradientFillLinear(rectBg, colorLigth, colorDark, wxEAST );
#endif
}

//---------------------------------------------------------------------------------------
void ToolPage::ReconfigureForMouseMode(int nMode)
{
    //Enable/disable each group in this page, depending on its usability for
    //currently selected mouse mode

    if (nMode == k_mouse_mode_undefined)
        return;

    std::list<ToolGroup*>::iterator it;
    for (it = m_groups.begin(); it != m_groups.end(); ++it)
    {
        (*it)->EnableForMouseMode(nMode);
    }
}

//---------------------------------------------------------------------------------------
int ToolPage::translate_key(int key, unsigned keyFlags)
{
    KeyTranslator* pTr = get_key_translator();
    int context = get_key_translation_context();
    return pTr->translate(context, key, keyFlags);
}

//---------------------------------------------------------------------------------------
bool ToolPage::process_key(wxKeyEvent& event)
{
    std::list<ToolGroup*>::iterator it;
    for (it = m_groups.begin(); it != m_groups.end(); ++it)
    {
        if ( (*it)->process_key(event) )
            return true;
    }
    return false;   //not processed
}

//---------------------------------------------------------------------------------------
void ToolPage::update_tools_info(ToolsInfo* pInfo)
{
    std::list<ToolGroup*>::iterator it;
    for (it = m_groups.begin(); it != m_groups.end(); ++it)
        (*it)->update_tools_info(pInfo);
}

//---------------------------------------------------------------------------------------
KeyTranslator* ToolPage::get_key_translator()
{
    ToolBox* pToolBox = GetToolBox();
    ApplicationScope& scope = pToolBox->get_app_scope();
    return scope.get_key_translator();
}

//---------------------------------------------------------------------------------------
void ToolPage::synchronize_with_cursor(bool fEnable, DocCursor* pCursor)
{
    std::list<ToolGroup*>::iterator it;
    for (it = m_groups.begin(); it != m_groups.end(); ++it)
        (*it)->synchronize_with_cursor(fEnable, pCursor);
}

//---------------------------------------------------------------------------------------
void ToolPage::synchronize_with_selection(bool fEnable, SelectionSet* pSelection)
{
    std::list<ToolGroup*>::iterator it;
    for (it = m_groups.begin(); it != m_groups.end(); ++it)
        (*it)->synchronize_with_selection(fEnable, pSelection);
}


}   //namespace lenmus
