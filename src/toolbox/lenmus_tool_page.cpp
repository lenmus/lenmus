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
// ToolPage implementation
//=======================================================================================
#define lmPANEL_WIDTH 150


wxBEGIN_EVENT_TABLE(ToolPage, wxPanel)
    //EVT_PAINT(ToolPage::OnPaintEvent)
wxEND_EVENT_TABLE()

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
    m_sPageToolTip = "";
    m_sPageBitmapName = "";
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
    return (ToolGroup*)nullptr;      //compiler happy
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
void ToolPage::OnPaintEvent(wxPaintEvent & WXUNUSED(event))
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
int ToolPage::translate_key(int key, int keyFlags)
{
    KeyTranslator* pTr = get_key_translator();
    long context = get_key_translation_context();
    return pTr->translate(context, key, keyFlags);
}

//---------------------------------------------------------------------------------------
bool ToolPage::process_command(int cmd)
{
    if (cmd == k_cmd_null)
        return false;   //not processed

    std::list<ToolGroup*>::iterator it;
    for (it = m_groups.begin(); it != m_groups.end(); ++it)
    {
        if ( (*it)->process_command(cmd) )
            return true;
    }
    return false;   //not processed
}

//---------------------------------------------------------------------------------------
bool ToolPage::process_key(wxKeyEvent& event)
{
//    std::list<ToolGroup*>::iterator it;
//    for (it = m_groups.begin(); it != m_groups.end(); ++it)
//    {
//        if ( (*it)->process_key(event) )
//            return true;
//    }
//    return false;   //not processed

    int key = event.GetKeyCode();
    int keyFlags = event.GetModifiers();
    int cmd = translate_key(key, keyFlags);

    return (cmd == k_cmd_null ? false : process_command(cmd));
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
