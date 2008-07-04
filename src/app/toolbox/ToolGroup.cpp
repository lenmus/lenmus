//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
//
//    This program is free software; you can redistribute it and/or modify it under the
//    terms of the GNU General Public License as published by the Free Software Foundation;
//    either version 2 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this
//    program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street,
//    Fifth Floor, Boston, MA  02110-1301, USA.
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
#include "../ArtProvider.h"        // to use ArtProvider for managing icons
#include "../../widgets/Button.h"
#include "../TheApp.h"              //to use GetMainFrame()
#include "../MainFrame.h"           //to use lmMainFrame


#define lmTOOLGROUP_SIZE wxSize(160, 90)
#define lmTOOLGROUP_STYLE wxCAPTION | wxRESIZE_BORDER


//-----------------------------------------------------------------------------------
// lmToolGroup implementation
//-----------------------------------------------------------------------------------
lmToolGroup::lmToolGroup(lmToolPage* pParent)
	: wxPanel(pParent, wxID_ANY, wxDefaultPosition, lmTOOLGROUP_SIZE)
{
	m_pParent = pParent;

    SetFont(wxFont(8, wxSWISS, wxNORMAL, wxBOLD, false, wxT("Tahoma")));
	SetForegroundColour(m_pParent->GetColors()->PrettyDark());
	SetBackgroundColour(m_pParent->GetColors()->Bright());    //Normal());
}

lmToolGroup::~lmToolGroup()
{
}

wxBoxSizer* lmToolGroup::CreateGroup(wxBoxSizer* pMainSizer, wxString sTitle)
{    
	//create common controls for a lmToolGroup
	wxStaticBoxSizer* pAuxSizer =
	    new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, sTitle ), wxVERTICAL );
	pMainSizer->Add( this, 0, wxALL|wxEXPAND, 5 );

	wxBoxSizer* pCtrolsSizer = new wxBoxSizer( wxVERTICAL );
	pAuxSizer->Add( pCtrolsSizer, 1, wxEXPAND, 5 );
	pAuxSizer->Fit( this );
	this->SetSizer( pAuxSizer );

	return pCtrolsSizer;
}

int lmToolGroup::GetGroupWitdh()
{	
	int width, height;
	m_pParent->GetClientSize(&width, &height);
	return width;
}



//-----------------------------------------------------------------------------------
// lmToolButtonsGroup implementation
//  A group of buttons, only one of them selected. If fAllowNone=true then the group
//  can have no button selected
//-----------------------------------------------------------------------------------

lmToolButtonsGroup::lmToolButtonsGroup(lmToolPage* pParent, int nNumButtons, bool fAllowNone,
                                       wxBoxSizer* pMainSizer, int nFirstButtonID)
	: lmToolGroup(pParent)
{
	m_nSelButton = -1;	            //none selected
    m_nFirstButtonID = nFirstButtonID;
    m_fAllowNone = fAllowNone;
    m_nNumButtons = nNumButtons;
    m_pButton.resize(nNumButtons);

    ConnectButtonEvents();
}

lmToolButtonsGroup::~lmToolButtonsGroup()
{
}

void lmToolButtonsGroup::ConnectButtonEvents()
{
    //connect OnButton events
    m_pParent->Connect( m_nFirstButtonID, m_nFirstButtonID + m_nNumButtons - 1,
                        wxEVT_COMMAND_BUTTON_CLICKED,
                        (wxObjectEventFunction)& lmToolButtonsGroup::OnButton );
                      //wxCommandEventHandler(lmToolButtonsGroup::OnButton) );
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
			m_pButton[i]->Press();
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
    //AWARE:
    //  As this method is connected to the event handler of the parent object,
    //  when it gets invoked, the 'this' pointer points to the parent (the lmToolBox object).
    //  Therefore, we must get and use a pointer to the real affected lmToolButtonsGroup
    //  object.
    lmToolButtonsGroup* pThis = (lmToolButtonsGroup*)event.GetEventObject();

    int iB = event.GetId() - pThis->GetFirstButtonID();
    if (pThis->IsNoneAllowed() && pThis->GetSelectedButton() == iB)
	    pThis->SelectButton(-1);       //no button selected
    else
	    pThis->SelectButton(iB);
}
