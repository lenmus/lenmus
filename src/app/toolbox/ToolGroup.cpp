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
lmToolGroup::lmToolGroup(wxPanel* pParent, lmColorScheme* pColours)
	: wxPanel(pParent, wxID_ANY, wxDefaultPosition, lmTOOLGROUP_SIZE)
    , m_pParent(pParent)
    , m_pColours(pColours)
{
    SetFont(wxFont(8, wxSWISS, wxNORMAL, wxBOLD, false, wxT("Tahoma")));

    //AWARE: ToolGroup is used as a control in properties dialogs. In them, 
    // the owner is not a lmToolPage and pointer pColors is NULL

    if(pColours)
    {
        SetForegroundColour(pColours->PrettyDark());
	    SetBackgroundColour(pColours->Bright());    //Normal());
    }
}

lmToolGroup::~lmToolGroup()
{
}

wxBoxSizer* lmToolGroup::CreateGroup(wxBoxSizer* pMainSizer, wxString sTitle)
{    
	//create common controls for a lmToolGroup
    m_pBoxTitle = new wxStaticBox( this, wxID_ANY, sTitle );
	wxStaticBoxSizer* pAuxSizer = new wxStaticBoxSizer(m_pBoxTitle, wxVERTICAL);
	pMainSizer->Add( this, 0, wxLEFT|wxRIGHT|wxEXPAND, 5 );

	wxBoxSizer* pCtrolsSizer = new wxBoxSizer( wxVERTICAL );
	pAuxSizer->Add( pCtrolsSizer, 1, wxEXPAND, 5 );
	pAuxSizer->Fit( this );
	this->SetSizer( pAuxSizer );

	return pCtrolsSizer;
}

void lmToolGroup::SetSelected(bool fSelected)
{
    //AWARE: ToolGroup is used as a control in properties dialogs. In them, 
    // the owner is not a lmToolPage and pointer pColors is NULL
    if(!m_pColours)
        return;

    if (fSelected)
    {
        SetForegroundColour(m_pColours->Dark());
	    SetBackgroundColour(m_pColours->Bright());
    }
    else
    {
        SetForegroundColour(m_pColours->PrettyDark());
	    SetBackgroundColour(m_pColours->Normal());
    }
}

int lmToolGroup::GetGroupWitdh()
{	
	int width, height;
	m_pParent->GetClientSize(&width, &height);
	return width;
}

void lmToolGroup::EnableGroup(bool fEnable)
{
    m_pBoxTitle->Enable(fEnable);
    //wxWindowListNode* pNode = this->GetChildren().GetFirst();
    //while(pNode)
    //{
    //  wxWindow* pCtrol = pNode->GetData();
    //  pCtrol->Enable(fEnable);
    // 
    //  pNode = pNode->GetNext();
    //}
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




//-----------------------------------------------------------------------------------
// lmToolButtonsGroup implementation
//  A group of buttons, only one of them selected. If fAllowNone=true then the group
//  can have no button selected
//-----------------------------------------------------------------------------------

lmToolButtonsGroup::lmToolButtonsGroup(wxPanel* pParent, int nNumButtons, bool fAllowNone,
                                       wxBoxSizer* pMainSizer, int nFirstButtonEventID,
                                       lmEToolID nFirstButtonToolID,
                                       lmColorScheme* pColours)
	: lmToolGroup(pParent, pColours)
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
