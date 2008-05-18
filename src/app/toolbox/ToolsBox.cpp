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
#pragma implementation "ToolsBox.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/cursor.h"
#include "wx/statline.h"

#include "../MainFrame.h"
#include "ToolsBox.h"
#include "../ArtProvider.h"        // to use ArtProvider for managing icons
#include "../TheApp.h"
#include "../ScoreCanvas.h"
#include "../../widgets/Button.h"


//-----------------------------------------------------------------------------------
//AWARE
//
//    Things to do to add a new tools panel to the Tools Box dialog:
//     1. Create a new panel class derived from lmToolPage
//     2. Look for "//TO_ADD:" tags in ToolsBox.h and follow instructions there
//     3. Look for "//TO_ADD:" tags in this file and follow instructions there
//
//-----------------------------------------------------------------------------------

// Panels
#include "ToolNotes.h"
#include "ToolClef.h"
//TO_ADD: add here the new tool panel include file



//layout parameters
const int SPACING = 5;          //spacing (pixels) around each sizer
const int BUTTON_SPACING = 2;	//spacing (pixels) between buttons
const int BUTTON_SIZE = 32;		//tools button size (pixels)
const int NUM_COLUMNS = 5;      //number of buttons per row
const int ID_BUTTON = 2200;


BEGIN_EVENT_TABLE(lmToolBox, wxPanel)
	EVT_CHAR (lmToolBox::OnKeyPress)
    EVT_COMMAND_RANGE (ID_BUTTON, ID_BUTTON+NUM_BUTTONS-1, wxEVT_COMMAND_BUTTON_CLICKED, lmToolBox::OnButtonClicked)
    EVT_SIZE (lmToolBox::OnResize) 
END_EVENT_TABLE()

IMPLEMENT_CLASS(lmToolBox, wxPanel)

// an entry for the tools buttons table
typedef struct lmToolsDataStruct {
    lmEEditTool nToolId;		// button ID
    wxString    sBitmap;		// bitmap name
	wxString	sToolTip;		// tool tip
} lmToolsData;


// Tools table
static const lmToolsData m_aToolsData[] = {
    //tool ID			bitmap name					tool tip
    //-----------		-------------				-------------
    {lmTOOL_SELECTION,	_T("tool_selection"),		_("Select objects") },
    {lmTOOL_CLEFS,		_T("tool_clefs"),			_("Add or edit clefs") },
	{lmTOOL_KEY_SIGN,	_T("tool_key_signatures"),	_("Add or edit key signatures") },
	{lmTOOL_TIME_SIGN,	_T("tool_time_signatures"),	_("Add or edit time signatures") },
    {lmTOOL_NOTES,		_T("tool_notes"),			_("Add or edit notes") },
	{lmTOOL_BARLINES,	_T("tool_barlines"),		_("Add or edit barlines and rehearsal marks") },
	//TO_ADD: Add here information about the new tool
	//NEXT ONE MUST BE THE LAST ONE
	{lmTOOL_NONE,		_T(""), _T("") },
};

lmToolBox::lmToolBox(wxWindow* parent, wxWindowID id)
    : wxPanel(parent, id, wxPoint(0,0), wxSize(170, -1), wxBORDER_NONE)
{
	//Create the dialog
	m_nSelTool = lmTOOL_NONE;

	//set colors
	m_colors.SetBaseColor( wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE) );

	CreateControls();

	//initialize panel's array
    for (int i=0; i < (int)lmTOOL_MAX; i++)
	{
        wxPanel* pPanel = CreatePanel((lmEEditTool)i);
        if (pPanel) pPanel->Show(false);
        m_cPanels.push_back(pPanel);
    }

	SelectTool(lmTOOL_NOTES);

}

void lmToolBox::CreateControls()
{
    //Controls creation for ToolsBox Dlg

    //the main sizer, to contain the three areas
    wxBoxSizer* pMainSizer = new wxBoxSizer(wxVERTICAL);

    //the tool page buttons selection area
	wxPanel* pSelectPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    pSelectPanel->SetBackgroundColour(m_colors.Normal());

	wxBoxSizer* pSelectSizer = new wxBoxSizer( wxVERTICAL );
	
    wxGridSizer* pButtonsSizer = new wxGridSizer(NUM_COLUMNS);
    int iMax = sizeof(m_aToolsData)/sizeof(lmToolsData);
    wxSize btSize(BUTTON_SIZE, BUTTON_SIZE);
	for (int iB=0; iB < iMax; iB++)
	{
		if (m_aToolsData[iB].nToolId == lmTOOL_NONE) break;

        m_pButton[iB] = new lmCheckButton(pSelectPanel, ID_BUTTON + iB, wxBitmap(btSize.x, btSize.y));
        m_pButton[iB]->SetBitmapUp(m_aToolsData[iB].sBitmap, _T(""), btSize);
        m_pButton[iB]->SetBitmapDown(m_aToolsData[iB].sBitmap, _T("button_selected_flat"), btSize);
        m_pButton[iB]->SetBitmapOver(m_aToolsData[iB].sBitmap, _T("button_over_flat"), btSize);
        pButtonsSizer->Add(m_pButton[iB], 0, 0, BUTTON_SPACING);
	}

    pSelectSizer->Add( pButtonsSizer, 1, wxEXPAND|wxALL, 5 );
	
	pSelectPanel->SetSizer( pSelectSizer );
	pSelectPanel->Layout();
	pSelectSizer->Fit( pSelectPanel );
	pMainSizer->Add( pSelectPanel, 0, 0, 5 );
	
    //the pages
	m_pPageSizer = new wxBoxSizer( wxVERTICAL );
	
    m_pEmptyPage = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxSize(170, 400), wxBORDER_SUNKEN|wxTAB_TRAVERSAL );
    m_pEmptyPage->SetBackgroundColour(m_colors.Bright());
	m_pCurPage = m_pEmptyPage;
	m_pPageSizer->Add( m_pCurPage, 1, wxEXPAND, 5 );
	
	pMainSizer->Add( m_pPageSizer, 1, wxEXPAND, 5 );
	
	SetSizer( pMainSizer );
    pMainSizer->SetSizeHints(this);
	Layout();
}

lmToolBox::~lmToolBox()
{
}

wxPanel* lmToolBox::CreatePanel(lmEEditTool nPanel)
{
    switch(nPanel) {
		case lmTOOL_SELECTION:
            return (wxPanel*)NULL;
        case lmTOOL_CLEFS:
            return new lmToolClef(this);
		case lmTOOL_KEY_SIGN:
            return (wxPanel*)NULL;
		case lmTOOL_TIME_SIGN:
            return (wxPanel*)NULL;
        case lmTOOL_NOTES:
            return new lmToolNotes(this);
        case lmTOOL_BARLINES:
            return (wxPanel*)NULL;	//new lmToolBarlinesOpt(m_pCurPage);
        //TO_ADD: Add a new case block for creating the new tool panel
        default:
            wxASSERT(false);
    }
    return (wxPanel*)NULL;

}

void lmToolBox::OnButtonClicked(wxCommandEvent& event)
{
    //identify button pressed
	SelectTool((lmEEditTool)(event.GetId() - ID_BUTTON));

	//lmController* pController = g_pTheApp->GetActiveController();
	//pController->SetCursor(*wxCROSS_CURSOR);
    //wxLogMessage(_T("[lmToolBox::OnButtonClicked] Tool %d selected"), m_nSelTool);
}

void lmToolBox::SelectTool(lmEEditTool nTool)
{
	if (!(nTool > lmTOOL_NONE && nTool < lmTOOL_MAX)) 
        return;

    SelectButton((int)nTool);
	m_nSelTool = nTool;

    //hide current page and save it
    wxPanel* pOldPage = m_pCurPage;
    pOldPage->Hide();

    //show new one
    m_pCurPage = (m_cPanels[nTool] ? m_cPanels[nTool] : m_pEmptyPage);
    m_pCurPage->Show();
    m_pPageSizer->Replace(pOldPage, m_pCurPage); 
    m_pCurPage->SetFocus();
    GetSizer()->Layout();

    //return focus to active view
    GetMainFrame()->SetFocusOnActiveView();
}

void lmToolBox::SelectButton(int nTool)
{
	// Set selected button as 'pressed' and all others as 'released'
	for(int i=0; i < (int)lmTOOL_MAX; i++)
	{
		if (i != nTool)
			m_pButton[i]->Release();
		else
			m_pButton[i]->Press();
	}
}

void lmToolBox::OnKeyPress(wxKeyEvent& event)
{
	//redirect all key press events to the active child window
	GetMainFrame()->RedirectKeyPressEvent(event);
}

void lmToolBox::OnResize(wxSizeEvent& event)
{
    wxSize newSize = event.GetSize();
    wxLogMessage(_T("[lmToolBox::OnResize] New size: %d, %d"), newSize.x, newSize.y);
}

