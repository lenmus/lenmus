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
#include "lenmus_tool_group.h"
#include "lenmus_tool_page.h"
#include "lenmus_tool_box_theme.h"
#include "lenmus_tool_box_events.h"
#include "lenmus_button.h"
#include "lenmus_main_frame.h"

//wxWidgets
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif


namespace lenmus
{


#define lmTOOLGROUP_SIZE wxSize(160, 90)
#define lmTOOLGROUP_STYLE wxCAPTION | wxRESIZE_BORDER



//-----------------------------------------------------------------------------------
// ToolGroup implementation
//-----------------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(ToolGroup, wxPanel)
    //EVT_MOTION(ToolGroup::OnMouseMoved)
    EVT_LEFT_DOWN(ToolGroup::OnMouseDown)
    EVT_LEFT_UP(ToolGroup::OnMouseReleased)
    //EVT_RIGHT_DOWN(ToolGroup::OnRightClick)
    EVT_LEAVE_WINDOW(ToolGroup::OnMouseLeftWindow)
    //EVT_KEY_DOWN(ToolGroup::OnKeyPressed)
    //EVT_KEY_UP(ToolGroup::OnKeyReleased)
    //EVT_MOUSEWHEEL(ToolGroup::OnMouseWheelMoved)
    EVT_PAINT(ToolGroup::OnPaintEvent)
wxEND_EVENT_TABLE()

//---------------------------------------------------------------------------------------
ToolGroup::ToolGroup(wxPanel* pParent, EGroupType nGroupType,
                         ToolboxTheme* pColours, int nValidMouseModes)
	: wxPanel(pParent, wxID_ANY, wxDefaultPosition, lmTOOLGROUP_SIZE)
    , m_pParent(pParent)
    , m_pColours(pColours)
    , m_fMousePressedDown(false)
    , m_nValidMouseModes(nValidMouseModes)
    , m_nGroupType(nGroupType)
    , m_fSelected(true)
{
    //set font to draw group labels
    SetFont(wxFont(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Tahoma"));

    //Any ToolGroup can be used as a control in properties dialogs. In them,
    //the owner is not a ToolPage and pointer pColors is nullptr
    m_fGuiControl = (m_pParent->IsKindOf(CLASSINFO(ToolPage)) ? false : true);

    if(pColours)
    {
        SetForegroundColour(pColours->PrettyDark());
	    SetBackgroundColour(pColours->Normal());        //Bright());
    }
    else
    {
        //the group must be used as GUI control
        wxASSERT(m_fGuiControl);
    }
}

//---------------------------------------------------------------------------------------
ToolGroup::~ToolGroup()
{
}

//---------------------------------------------------------------------------------------
wxBoxSizer* ToolGroup::create_main_sizer_for_group(wxBoxSizer* pMainSizer)
{
	//create common controls for a ToolGroup and returns the sizer in which
    //derived class must add its controls

	m_pCtrolsSizer = new wxBoxSizer( wxVERTICAL );

    //spacer for group title
    if (m_sTitle != "")
	    m_pCtrolsSizer->Add( 0, 0, 0, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 10 );

	m_pGroupSizer = new wxBoxSizer( wxVERTICAL );
	m_pCtrolsSizer->Add( m_pGroupSizer, 1, wxEXPAND|wxALL, 5 );

	this->SetSizer( m_pCtrolsSizer );
	this->Layout();
	m_pCtrolsSizer->Fit( this );
	pMainSizer->Add( this, 0, wxLEFT|wxRIGHT|wxTOP|wxEXPAND, 2 );

    return m_pGroupSizer;
}

//---------------------------------------------------------------------------------------
void ToolGroup::SetSelected(bool fSelected)
{
    //set selected/deselected state for tool-selector groups
    //If group was disabled it is first enabled.

    wxASSERT(is_tool_selector_group());

    //check if anything to do
    if (m_fSelected == fSelected)
        return;

    //change selected/deselected state
    m_fSelected = fSelected;

    //if group is disabled, enable it
    if (!this->IsEnabled())
        EnableGroup(true);
    else
        DoPaintNow();
}

//---------------------------------------------------------------------------------------
int ToolGroup::GetGroupWitdh()
{
	int width, height;
	m_pParent->GetClientSize(&width, &height);
	return width;
}

//---------------------------------------------------------------------------------------
void ToolGroup::EnableGroup(bool fEnable)
{
    //enable/disable the group and repaints group to display new state
    //if enable, for tool-selector groups the previous state (selected/deselected)
    //is restored

    //check if anything to do
    if (this->IsEnabled() == fEnable)
        return;

    //for tool-selector groups save/restore state before disabling/enabling the group
    if (is_tool_selector_group())
    {
        if (fEnable)
            m_fSelected = m_fSaveSelected;
        else
            m_fSaveSelected = m_fSelected;
    }

    Enable(fEnable);
    DoPaintNow();
}

//---------------------------------------------------------------------------------------
void ToolGroup::PostToolBoxEvent(EToolID nToolID, bool fSelected)
{
    //post tool box event. Will be handled by MainFrame
    ToolBoxToolSelectedEvent event(nToolID, fSelected);
    ::wxPostEvent(this, event);
}

//---------------------------------------------------------------------------------------
void ToolGroup::OnPaintEvent(wxPaintEvent & WXUNUSED(evt))
{
    //called by the system when the panel needs to be redrawn. You can also trigger
    //this call by calling Refresh()/Update().

    wxPaintDC dc(this);
    DoRender(dc);
}

//---------------------------------------------------------------------------------------
void ToolGroup::DoRender(wxDC& dc)
{
    //Here we do the actual rendering. It is a separate method so that it can work
    //no matter what type of DC is used (i.e. wxPaintDC or wxClientDC)

    //set colors
    //AWARE: ToolGroup is used as a control in properties dialogs. In them,
    // the owner is not a ToolPage and pointer pColors is nullptr

    wxColour colorMaxHight = wxSystemSettings::GetColour( wxSYS_COLOUR_3DHIGHLIGHT);   //button highlight
    wxColour colorHiLight = wxSystemSettings::GetColour( wxSYS_COLOUR_3DHILIGHT);
    wxColour colorMoreLight = wxSystemSettings::GetColour( wxSYS_COLOUR_3DLIGHT );
    wxColour colorLight = wxSystemSettings::GetColour( wxSYS_COLOUR_3DFACE);    //button face
    wxColour colorLessLight = wxSystemSettings::GetColour( wxSYS_COLOUR_3DSHADOW);  //button shadow
    wxColour colorDark = wxSystemSettings::GetColour( wxSYS_COLOUR_3DDKSHADOW );

    //background color
    wxColour colorBg = colorLight;
    if (m_pColours)
    {
        //if (m_fSelected)
        //    colorBg = m_pColours->GetColour(lmCOLOUR_GROUP_BACKGROUND_SELECTED);
        //else
            colorBg = m_pColours->GetColour(lmCOLOUR_GROUP_BACKGROUND_NORMAL);
    }

    //border color
    wxColour colorBorder = colorLessLight;
    if (m_pColours)
        colorBorder = m_pColours->GetColour(lmCOLOUR_GROUP_BORDER_ACTIVE);

    //title color
    wxColour colorTitle = colorLessLight;
    if (m_pColours)
    {
        if (IsEnabled())
            colorTitle = m_pColours->GetColour(lmCOLOUR_GROUP_TITLE_ACTIVE);
        else
            colorTitle = m_pColours->GetColour(lmCOLOUR_GROUP_TITLE_INACTIVE);
    }


        //Do the painting

    //background
    wxRect rect;
    GetClientSize(&rect.width, &rect.height);

    wxRect rectBg = rect;
    rectBg.Deflate(1, 1);
#if 1
    dc.SetPen( wxPen(colorBg));
    dc.SetBrush( wxBrush(colorBg));
    dc.DrawRectangle(rectBg);
    //title background
    if (m_sTitle != "")
    {
        rectBg.SetHeight(13);
        //if (m_fSelected)
        //{
        //    wxColour cTop = wxColour(215,224,224);  //ToolboxTheme::LightenColour(colorBg, 0.9f);
        //    wxColour cBottom = wxColour(182,202,202);  //ToolboxTheme::DarkenColour(cTop, 0.2f);
        //    dc.GradientFillLinear(rectBg, cTop, cBottom, wxSOUTH );
        //}
        //else
        {
            wxColour cTop = ToolboxTheme::LightenColour(colorBg, 0.6f);
            wxColour cBottom = colorBg; //ToolboxTheme::DarkenColour(colorBg, 0.15f);
            dc.GradientFillLinear(rectBg, cTop, cBottom, wxSOUTH );
        }
    }
#else
    dc.GradientFillLinear(rectBg, colorMoreLight, colorLight, wxSOUTH );
#endif

    //group border
    dc.SetPen(colorBorder);
        //left line
    dc.DrawLine(rect.GetLeft(), rect.GetTop()+2,
                rect.GetLeft(), rect.GetBottom()-2);
        //top line
    dc.DrawLine(rect.GetLeft()+2, rect.GetTop(),
                rect.GetRight()-2, rect.GetTop());
        //right line
    dc.DrawLine(rect.GetRight()-1, rect.GetTop()+2,
                rect.GetRight()-1, rect.GetBottom()-2);

        //bottom line
    dc.DrawLine(rect.GetLeft()+2, rect.GetBottom()-1,
                rect.GetRight()-2, rect.GetBottom()-1);

        //top left corner
    dc.DrawPoint(rect.GetLeft()+1, rect.GetTop()+1);
    dc.DrawPoint(rect.GetLeft()+1, rect.GetTop()+2);
    dc.DrawPoint(rect.GetLeft()+2, rect.GetTop()+1);

        //top right corner
    dc.DrawPoint(rect.GetRight()-2, rect.GetTop()+1);
    dc.DrawPoint(rect.GetRight()-2, rect.GetTop()+2);
    dc.DrawPoint(rect.GetRight()-3, rect.GetTop()+1);

        //bottom right corner
    dc.DrawPoint(rect.GetRight()-2, rect.GetBottom()-2);
    dc.DrawPoint(rect.GetRight()-2, rect.GetBottom()-3);
    dc.DrawPoint(rect.GetRight()-3, rect.GetBottom()-2);

        //bottom left corner
    dc.DrawPoint(rect.GetLeft()+1, rect.GetBottom()-2);
    dc.DrawPoint(rect.GetLeft()+1, rect.GetBottom()-3);
    dc.DrawPoint(rect.GetLeft()+2, rect.GetBottom()-2);

        //white carving
    dc.SetPen( ToolboxTheme::LightenColour(colorBg, 0.9f) );
        //left line
    dc.DrawLine(rect.GetLeft()+1, rect.GetTop()+3,
                rect.GetLeft()+1, rect.GetBottom()-3);
        //top line
    dc.DrawLine(rect.GetLeft()+3, rect.GetTop()+1,
                rect.GetRight()-3, rect.GetTop()+1);
        //right line
    dc.DrawLine(rect.GetRight(), rect.GetTop()+2,
                rect.GetRight(), rect.GetBottom()-2);

        //bottom line
    dc.DrawLine(rect.GetLeft()+3, rect.GetBottom(),
                rect.GetRight()-2, rect.GetBottom());



    //group title
    if (m_sTitle != "")
    {
        dc.SetFont( wxFont( 8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Tahoma" ) );
        if (!this->IsEnabled())
        {
            dc.SetTextForeground(*wxWHITE);
            dc.DrawText( m_sTitle, 5, 2 );
        }
        dc.SetTextForeground(colorTitle);
        dc.DrawText( m_sTitle, 4, 1 );
    }
}

//---------------------------------------------------------------------------------------
void ToolGroup::DoPaintNow()
{
    //you can use a clientDC to paint on the panel
    //at any time. Using this generally does not free you from
    //catching paint events, since it is possible that e.g. the window
    //manager throws away your drawing when the window comes to the
    //background, and expects you will redraw it when the window comes
    //back (by sending a paint event).

    // depending on your system you may need to look at double-buffered dcs
    wxClientDC dc(this);
    DoRender(dc);
}

//---------------------------------------------------------------------------------------
void ToolGroup::OnMouseDown(wxMouseEvent& WXUNUSED(event))
{
    //m_fMousePressedDown = true;
    //DoPaintNow();
}

//---------------------------------------------------------------------------------------
void ToolGroup::OnMouseReleased(wxMouseEvent& WXUNUSED(event))
{
    //select tool group by clicking on it
    if (!m_fGuiControl && is_tool_selector_group() && this->IsEnabled())
        ((ToolPage*)m_pParent)->select_group_and_notify(this);
}

//---------------------------------------------------------------------------------------
void ToolGroup::OnMouseLeftWindow(wxMouseEvent& WXUNUSED(event))
{
    //if (m_fMousePressedDown)
    //{
    //    m_fMousePressedDown = false;
    //    DoPaintNow();
    //}
}

//---------------------------------------------------------------------------------------
void ToolGroup::EnableForMouseMode(int nMode)
{
    //Enable/disable this group, depending on its usability for specified mouse mode

    EnableGroup((m_nValidMouseModes & nMode) != 0);
}

//---------------------------------------------------------------------------------------
bool ToolGroup::process_key(wxKeyEvent& WXUNUSED(event))
{
    //virtual. Default implementation
    return false;   //not processed
}

//---------------------------------------------------------------------------------------
bool ToolGroup::process_command(int WXUNUSED(cmd))
{
    //virtual. Default implementation
    return false;   //not processed
}



//=======================================================================================
// ToolButtonsGroup implementation
//  A group of buttons, only one of them selected. If fAllowNone=true then the group
//  can have no button selected
//=======================================================================================
ToolButtonsGroup::ToolButtonsGroup(wxPanel* pParent, EGroupType nGroupType,
                                       int nNumButtons, bool fAllowNone,
                                       wxBoxSizer* WXUNUSED(pMainSizer),
                                       int nFirstButtonEventID,
                                       EToolID nFirstButtonToolID,
                                       ToolboxTheme* pColours,
                                       int nValidMouseModes)
	: ToolGroup(pParent, nGroupType, pColours, nValidMouseModes)
    , m_fAllowNone(fAllowNone)
    , m_nNumButtons(nNumButtons)
	, m_nSelButton(-1)	            //none selected
    , m_nSelButtonSave(-1)
    , m_nFirstButtonEventID(nFirstButtonEventID)
    , m_nFirstButtonToolID((int)nFirstButtonToolID)
{
    m_pButton.resize(nNumButtons);
    ConnectButtonEvents();
}

//---------------------------------------------------------------------------------------
ToolButtonsGroup::~ToolButtonsGroup()
{
}

//---------------------------------------------------------------------------------------
void ToolButtonsGroup::ConnectButtonEvents()
{
    //connect OnButton events
    Connect( m_nFirstButtonEventID, m_nFirstButtonEventID + m_nNumButtons - 1,
             wxEVT_COMMAND_BUTTON_CLICKED,
             (wxObjectEventFunction)& ToolButtonsGroup::OnButton );
}

//---------------------------------------------------------------------------------------
void ToolButtonsGroup::SelectButton(int iB)
{
    // Set selected button as 'pressed' and all others as 'released'

	m_nSelButton = iB;
    if (m_nSelButtonSave == -1)
        m_nSelButtonSave = iB;

    for(int i=0; i < m_nNumButtons; i++)
	{
        if (i != iB)
            m_pButton[i]->Release();
        else
        {
			m_pButton[i]->Press();
        }
	}
//TODO TB
//    //return focus to active view
//    GetMainFrame()->SetFocusOnActiveView();
}

//---------------------------------------------------------------------------------------
void ToolButtonsGroup::SetSelected(bool fSelected)
{
    //check if something to do
    if (fSelected == m_fSelected)
        return;

    //change status
    m_fSelected = fSelected;

    //if not allowed 'no button selected' hide/restore selected button
    if (fSelected && !m_fAllowNone && m_nSelButtonSave != -1)
        SelectButton(m_nSelButtonSave);
    else if (!fSelected && !m_fAllowNone && m_nSelButton != -1)
    {
        m_pButton[m_nSelButton]->Release();
        m_nSelButtonSave = m_nSelButton;
    }
}

//---------------------------------------------------------------------------------------
void ToolButtonsGroup::SelectNextButton()
{
    if (++m_nSelButton == m_nNumButtons)
        m_nSelButton = (m_fAllowNone ? -1 : 0);

    SelectButton(m_nSelButton);
}

//---------------------------------------------------------------------------------------
void ToolButtonsGroup::SelectPrevButton()
{
    if (m_fAllowNone)
    {
        if (m_nSelButton == -1)
            m_nSelButton = m_nNumButtons;
    }
    else
    {
        if (m_nSelButton == 0)
            m_nSelButton = m_nNumButtons;
    }
    --m_nSelButton;

    SelectButton(m_nSelButton);
}

//---------------------------------------------------------------------------------------
void ToolButtonsGroup::OnButton(wxCommandEvent& event)
{
    int iB = event.GetId() - GetFirstButtonEventID();
    if (IsNoneAllowed() && GetSelectedButton() == iB)
	    SelectButton(-1);       //no button selected
    else
    {
	    SelectButton(iB);
        m_nSelButtonSave = iB;
    }

    //inform owner page
    OnButtonSelected( m_nSelButton );
}

//---------------------------------------------------------------------------------------
void ToolButtonsGroup::OnButtonSelected(int WXUNUSED(nSelButton))
{
    //Notify owner page about the tool change, unless it is the MouseGroup, In this
    //case post a tool change event directly to the active controller

    //AWARE: ToolGroup can be used as a control in properties dialogs. In them,
    // the owner is not a ToolPage but a wxPanel. Events will not be posted
    // to wxPanels

    EToolGroupID groupID = get_group_id();
    if ( m_pParent->IsKindOf(CLASSINFO(ToolPage)) )
    {
        //notify owner page about the tool change
        ToolPage* pPage = static_cast<ToolPage*>(m_pParent);
        pPage->on_tool_changed((EToolID)m_nFirstButtonToolID, groupID);
    }
    else if (groupID == k_grp_MouseMode)
    {
        //post tool box event to the active controller
        ToolBoxToolSelectedEvent event(m_nFirstButtonToolID, true /* tool is selected*/);
        ::wxPostEvent(this, event);
    }
    else
    {
        //this ToolGroup is used as a control in properties dialogs. Default
        //behaviour is to do nothing
    }
}


}   //namespace lenmus
