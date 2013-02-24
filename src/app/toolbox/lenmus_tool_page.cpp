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
//#include "../TheApp.h"              //to use GetMainFrame()
//#include "../MainFrame.h"           //to use lmMainFrame

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
    wxPanel::Create(parent, -1, wxPoint(0,0), wxDefaultSize,    //wxDefaultPosition, wxSize(-1, -1),  //lmPANEL_WIDTH, -1),
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
    m_fGroupsCreated = false;
}

//---------------------------------------------------------------------------------------
ToolPage::~ToolPage()
{
    //delete groups
    std::list<ToolGroup*>::iterator it;
    for (it = m_Groups.begin(); it != m_Groups.end(); ++it)
    {
        delete *it;
    }
}

//---------------------------------------------------------------------------------------
void ToolPage::CreateLayout()
{
    //create groups' controls
    std::list<ToolGroup*>::iterator it;
    for (it = m_Groups.begin(); it != m_Groups.end(); ++it)
    {
        (*it)->CreateGroupControls(m_pMainSizer);
    }
    SetAutoLayout(true);
    m_pMainSizer->Fit(this);
    m_pMainSizer->SetSizeHints(this);
    m_pMainSizer->Layout();
}

//---------------------------------------------------------------------------------------
void ToolPage::OnToolChanged(EToolGroupID nGroupID, EToolID nToolID)
{
    //deselect tools in any related groups to the one issuing the callback
    //and post tool box event to the active controller

    if (!m_fGroupsCreated)
        return;

    //if selected group is a tool selector one, deselect any other
    //tool-selector group in this page and save information about current
    //group and tool
    if (GetToolGroup(nGroupID)->IsToolSelectorGroup())
    {
        DeselectRelatedGroups(nGroupID);
        m_nCurGroupID = nGroupID;
        m_nCurToolID = nToolID;
    }

//TODO TB
//    //post tool box event to the active controller
//    wxWindow* pWnd = GetMainFrame()->GetActiveController();
//    if (pWnd)
//    {
//        ToolBoxToolSelectedEvent event(nGroupID, m_nPageID, nToolID, true);
//        ::wxPostEvent( pWnd, event );
//    }
}

//---------------------------------------------------------------------------------------
ToolGroup* ToolPage::GetToolGroup(EToolGroupID nGroupID)
{
    //return pointer to group nGroupID

    std::list<ToolGroup*>::iterator it;
    for (it = m_Groups.begin(); it != m_Groups.end(); ++it)
    {
        if ((*it)->GetToolGroupID() == nGroupID)
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
    for (it = m_Groups.begin(); it != m_Groups.end(); ++it)
    {
        if ((*it)->IsToolSelectorGroup())
            (*it)->SetSelected((*it)->GetToolGroupID() == nGroupID);
    }
}

//---------------------------------------------------------------------------------------
void ToolPage::SelectGroup(ToolGroup* pGroup)
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
    for (it = m_Groups.begin(); it != m_Groups.end(); ++it)
    {
        if ((*it)->IsToolSelectorGroup())
            (*it)->SetSelected(pGroup == *it);
    }

    //save information about current group and tool
    m_nCurGroupID = pGroup->GetToolGroupID();
    m_nCurToolID = pGroup->GetCurrentToolID();

//TODO TB
//    //post tool box event to the active controller
//    wxWindow* pWnd = GetMainFrame()->GetActiveController();
//    if (pWnd)
//    {
//        ToolBoxToolSelectedEvent event(m_nCurGroupID, m_nPageID, m_nCurToolID, true);
//        ::wxPostEvent( pWnd, event );
//    }
}

//---------------------------------------------------------------------------------------
void ToolPage::AddGroup(ToolGroup* pGroup)
{
    m_Groups.push_back(pGroup);
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

    if (nMode == lmMM_UNDEFINED)
        return;

    std::list<ToolGroup*>::iterator it;
    for (it = m_Groups.begin(); it != m_Groups.end(); ++it)
    {
        (*it)->EnableForMouseMode(nMode);
    }
}


}   //namespace lenmus
