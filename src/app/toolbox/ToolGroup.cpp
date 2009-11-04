//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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
//-------------------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "ToolGroup.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "ToolGroup.h"
#include "ToolPage.h"
#include "ColorScheme.h"
#include "ToolBoxEvents.h"
#include "../ArtProvider.h"        // to use ArtProvider for managing icons
#include "../../widgets/Button.h"
#include "../TheApp.h"              //to use GetMainFrame()
#include "../MainFrame.h"           //to use lmMainFrame


#define lmTOOLGROUP_SIZE wxSize(160, 90)
#define lmTOOLGROUP_STYLE wxCAPTION | wxRESIZE_BORDER



//-----------------------------------------------------------------------------------
// lmToolGroup implementation
//-----------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(lmToolGroup, wxPanel)
    //EVT_MOTION(lmToolGroup::OnMouseMoved)
    EVT_LEFT_DOWN(lmToolGroup::OnMouseDown)
    EVT_LEFT_UP(lmToolGroup::OnMouseReleased)
    //EVT_RIGHT_DOWN(lmToolGroup::OnRightClick)
    EVT_LEAVE_WINDOW(lmToolGroup::OnMouseLeftWindow)
    //EVT_KEY_DOWN(lmToolGroup::OnKeyPressed)
    //EVT_KEY_UP(lmToolGroup::OnKeyReleased)
    //EVT_MOUSEWHEEL(lmToolGroup::OnMouseWheelMoved)
    EVT_PAINT(lmToolGroup::OnPaintEvent)
END_EVENT_TABLE()

lmToolGroup::lmToolGroup(wxPanel* pParent, lmColorScheme* pColours,
                         int nValidMouseModes)
	: wxPanel(pParent, wxID_ANY, wxDefaultPosition, lmTOOLGROUP_SIZE)
    , m_pParent(pParent)
    , m_pColours(pColours)
    , m_fSelected(true)
    , m_nValidMouseModes(nValidMouseModes)
{
    SetFont(wxFont(8, wxSWISS, wxNORMAL, wxBOLD, false, wxT("Tahoma")));

    //AWARE: ToolGroup is used as a control in properties dialogs. In them, 
    // the owner is not a lmToolPage and pointer pColors is NULL

    if(pColours)
    {
        SetForegroundColour(pColours->PrettyDark());
	    SetBackgroundColour(pColours->Normal());        //Bright()); 
    }

//    m_sTitle = label;
    m_fMousePressedDown = false;
}

lmToolGroup::~lmToolGroup()
{
}

wxBoxSizer* lmToolGroup::CreateGroupSizer(wxBoxSizer* pMainSizer)
{    
	//create common controls for a lmToolGroup and returns the sizer in which
    //derived class must add its controls

	m_pCtrolsSizer = new wxBoxSizer( wxVERTICAL );
	
    //spacer for group title
    if (m_sTitle != _T(""))
	    m_pCtrolsSizer->Add( 0, 0, 0, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 10 );
	
	m_pGroupSizer = new wxBoxSizer( wxVERTICAL );
	m_pCtrolsSizer->Add( m_pGroupSizer, 1, wxEXPAND|wxALL, 5 );
	
	this->SetSizer( m_pCtrolsSizer );
	this->Layout();
	m_pCtrolsSizer->Fit( this );
	pMainSizer->Add( this, 0, wxLEFT|wxRIGHT|wxTOP|wxEXPAND, 2 );

    return m_pGroupSizer;
}

void lmToolGroup::SetSelected(bool fSelected)
{
    m_fSelected = fSelected;
}

int lmToolGroup::GetGroupWitdh()
{	
	int width, height;
	m_pParent->GetClientSize(&width, &height);
	return width;
}

void lmToolGroup::EnableGroup(bool fEnable)
{
    Enable(fEnable);
    DoPaintNow();
}

void lmToolGroup::PostToolBoxEvent(lmEToolID nToolID, bool fSelected)
{
    //post tool box event to the active controller
    wxWindow* pWnd = GetMainFrame()->GetActiveController();
    if (pWnd)
    {
	    lmToolBox* pToolBox = GetMainFrame()->GetActiveToolBox();
	    wxASSERT(pToolBox);
        lmToolBoxToolSelectedEvent event(this->GetToolGroupID(), pToolBox->GetCurrentPageID(), nToolID,
                             fSelected);
        ::wxPostEvent( pWnd, event );
    }
}
 
void lmToolGroup::OnPaintEvent(wxPaintEvent & evt)
{
    //called by the system when the panel needs to be redrawn. You can also trigger
    //this call by calling Refresh()/Update().

    wxPaintDC dc(this);
    DoRender(dc);
}
 
void lmToolGroup::DoRender(wxDC& dc)
{
    //Here we do the actual rendering. It is a separate method so that it can work
    //no matter what type of DC is used (i.e. wxPaintDC or wxClientDC)

    //set colors
    //AWARE: ToolGroup is used as a control in properties dialogs. In them, 
    // the owner is not a lmToolPage and pointer pColors is NULL

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
    if (m_sTitle != _T(""))
    {
        rectBg.SetHeight(13);
        //if (m_fSelected)
        //{
        //    wxColour cTop = wxColour(215,224,224);  //lmColorScheme::LightenColour(colorBg, 0.9f);
        //    wxColour cBottom = wxColour(182,202,202);  //lmColorScheme::DarkenColour(cTop, 0.2f);
        //    dc.GradientFillLinear(rectBg, cTop, cBottom, wxSOUTH );
        //}
        //else
        {
            wxColour cTop = lmColorScheme::LightenColour(colorBg, 0.6f);
            wxColour cBottom = colorBg; //lmColorScheme::DarkenColour(colorBg, 0.15f);
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
    dc.SetPen( lmColorScheme::LightenColour(colorBg, 0.9f) );
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
    if (m_sTitle != _T(""))
    {
        dc.SetFont( wxFont( 8, 74, 90, 90, false, wxT("Tahoma") ) );
        if (!this->IsEnabled())
        {
            dc.SetTextForeground(*wxWHITE);
            dc.DrawText( m_sTitle, 5, 2 );
        }
        dc.SetTextForeground(colorTitle);
        dc.DrawText( m_sTitle, 4, 1 );
    }
}
 
void lmToolGroup::DoPaintNow()
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

void lmToolGroup::OnMouseDown(wxMouseEvent& event)
{
    //m_fMousePressedDown = true;
    //DoPaintNow();
}

void lmToolGroup::OnMouseReleased(wxMouseEvent& event)
{
    //m_fMousePressedDown = false;
    //DoPaintNow();
    //
    ////wxMessageBox( _T("Click on group") );
}

void lmToolGroup::OnMouseLeftWindow(wxMouseEvent& event)
{
    //if (m_fMousePressedDown)
    //{
    //    m_fMousePressedDown = false;
    //    DoPaintNow();
    //}
}

void lmToolGroup::EnableForMouseMode(int nMode)
{
    //Enable/disable this group, depending on its usability for specified mouse mode

    EnableGroup((m_nValidMouseModes & nMode) != 0);
}



//-----------------------------------------------------------------------------------
// lmToolButtonsGroup implementation
//  A group of buttons, only one of them selected. If fAllowNone=true then the group
//  can have no button selected
//-----------------------------------------------------------------------------------

lmToolButtonsGroup::lmToolButtonsGroup(wxPanel* pParent, int nNumButtons, bool fAllowNone,
                                       wxBoxSizer* pMainSizer, int nFirstButtonEventID,
                                       lmEToolID nFirstButtonToolID,
                                       lmColorScheme* pColours,
                                       int nValidMouseModes)
	: lmToolGroup(pParent, pColours, nValidMouseModes)
	, m_nSelButton(-1)	            //none selected
    , m_nFirstButtonEventID(nFirstButtonEventID)
    , m_nFirstButtonToolID((int)nFirstButtonToolID)
    , m_fAllowNone(fAllowNone)
    , m_nNumButtons(nNumButtons)
{
    m_pButton.resize(nNumButtons);
    ConnectButtonEvents();
}

lmToolButtonsGroup::~lmToolButtonsGroup()
{
}

void lmToolButtonsGroup::ConnectButtonEvents()
{
    //connect OnButton events
    Connect( m_nFirstButtonEventID, m_nFirstButtonEventID + m_nNumButtons - 1,
             wxEVT_COMMAND_BUTTON_CLICKED,
             (wxObjectEventFunction)& lmToolButtonsGroup::OnButton );
}

void lmToolButtonsGroup::SelectButton(int iB)
{
    // Set selected button as 'pressed' and all others as 'released'

	m_nSelButton = iB;
	for(int i=0; i < m_nNumButtons; i++)
	{
		if (i != iB)
			m_pButton[i]->Release();
		else
        {
			m_pButton[i]->Press();
        }
	}

    //return focus to active view
    GetMainFrame()->SetFocusOnActiveView();
}

void lmToolButtonsGroup::SelectNextButton()
{
    if (++m_nSelButton == m_nNumButtons)
        m_nSelButton = (m_fAllowNone ? -1 : 0);

    SelectButton(m_nSelButton);
}

void lmToolButtonsGroup::SelectPrevButton()
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

void lmToolButtonsGroup::OnButton(wxCommandEvent& event)
{
    int iB = event.GetId() - GetFirstButtonEventID();
    if (IsNoneAllowed() && GetSelectedButton() == iB)
	    SelectButton(-1);       //no button selected
    else
	    SelectButton(iB);
    
    //inform derived class
    OnButtonSelected( m_nSelButton );
}

void lmToolButtonsGroup::OnButtonSelected(int nSelButton)
{
    //Notify owner page about the tool change, unless it is the MouseGroup, In this
    //case post a tool change event directly to the active controller

    //AWARE: ToolGroup can be used as a control in properties dialogs. In them, 
    // the owner is not a lmToolPage but a wxPanel. Events will not be posted
    // to wxPanels

    if ( m_pParent->IsKindOf(CLASSINFO(lmToolPage)) )
    {
        //notify owner page about the tool change
        ((lmToolPage*)m_pParent)->OnToolChanged(this->GetToolGroupID(),
                                                (lmEToolID)nSelButton);
    }
    else if (this->GetToolGroupID() == lmGRP_MouseMode)
    {
        //post tool box event to the active controller
        wxWindow* pWnd = GetMainFrame()->GetActiveController();
        if (pWnd)
        {
	        lmToolBox* pToolBox = GetMainFrame()->GetActiveToolBox();
	        wxASSERT(pToolBox);
            lmToolBoxToolSelectedEvent event(this->GetToolGroupID(), pToolBox->GetCurrentPageID(),
                                nSelButton, true);
            ::wxPostEvent( pWnd, event );
        }
    }
    else
    {
        //this ToolGroup is used as a control in properties dialogs. Default
        //behaviour is to do nothing
    }
}
