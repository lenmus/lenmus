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
const int SPACING = 1;          //spacing (pixels) around each sizer
const int BUTTON_SPACING = 2;	//spacing (pixels) between buttons
const int BUTTON_SIZE = 32;		//tools button size (pixels)
const int NUM_COLUMNS = 4;      //number of buttons per row
const int ID_BUTTON = 2200;


BEGIN_EVENT_TABLE(lmToolBox, wxPanel)
	EVT_CHAR(lmToolBox::OnKeyPress)
    EVT_COMMAND_RANGE (ID_BUTTON, ID_BUTTON+NUM_BUTTONS-1, wxEVT_COMMAND_BUTTON_CLICKED, lmToolBox::OnButtonClicked)
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
    : wxPanel(parent, id, wxPoint(0,0), wxSize(170, 400), wxNO_BORDER)
{
	//Create the dialog
	m_nSelTool = lmTOOL_NONE;
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

    //The Tool box panel has two areas:
    //1. Tool selection buttons are, in the middle
    //2. Selected tool options, in the bottom

    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));

    //the main sizer, to contain the three areas
    wxBoxSizer* pMainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(pMainSizer);

	//the buttons area
    wxGridSizer* buttonsSizer = new wxGridSizer(NUM_COLUMNS);
    pMainSizer->Add(buttonsSizer, wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, SPACING));

	//separation line
	wxStaticLine* pLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition,
										   wxDefaultSize, wxLI_HORIZONTAL );
    pMainSizer->Add(pLine, wxSizerFlags(0).Left().Border(wxGROW|wxTOP|wxBOTTOM, 5));

    //the options area
    wxBoxSizer* optsSizer = new wxBoxSizer(wxVERTICAL);
    pMainSizer->Add(optsSizer, 0, wxGROW|wxTOP, SPACING);
    m_pOptionsPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(150, 300),
								  wxNO_BORDER );
    optsSizer->Add(m_pOptionsPanel, 0, wxGROW|wxTOP, SPACING);

    //create the tool buttons
    int iMax = sizeof(m_aToolsData)/sizeof(lmToolsData);
	for (int iB=0; iB < iMax; iB++)
	{
		if (m_aToolsData[iB].nToolId == lmTOOL_NONE) break;
        m_pButton[iB] = new lmCheckButton(this, ID_BUTTON + iB,
            wxArtProvider::GetBitmap(m_aToolsData[iB].sBitmap, wxART_TOOLBAR,
									 wxSize(BUTTON_SIZE, BUTTON_SIZE) ));
		//m_pButton[iB] = new lmCheckButton(this, ID_BUTTON + iB, tool_clefs_24_xpm,
		//								  wxDefaultPosition, wxSize(24,24) );
        buttonsSizer->Add(m_pButton[iB], wxSizerFlags(0).Border(wxALL, BUTTON_SPACING) );
		m_pButton[iB]->SetToolTip(m_aToolsData[iB].sToolTip);
		m_pButton[iB]->SetBorderOver(lm_eBorderOver);
		m_pButton[iB]->SetBorderDown(lm_eBorderFlat);
	}
  //  buttonsSizer->Add(
		//new wxButton(this, wxID_ANY, _T(""), wxDefaultPosition, wxSize(26,26) ),
		//wxSizerFlags(0).Border(wxALL, BUTTON_SPACING) );
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
            return new lmToolClef(m_pOptionsPanel);
		case lmTOOL_KEY_SIGN:
            return (wxPanel*)NULL;
		case lmTOOL_TIME_SIGN:
            return (wxPanel*)NULL;
        case lmTOOL_NOTES:
            return new lmToolNotes(m_pOptionsPanel);
        case lmTOOL_BARLINES:
            return (wxPanel*)NULL;	//new lmToolBarlinesOpt(m_pOptionsPanel);
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

	//lmController* pController = g_pTheApp->GetViewController();
	//pController->SetCursor(*wxCROSS_CURSOR);
    //wxLogMessage(_T("[lmToolBox::OnButtonClicked] Tool %d selected"), m_nSelTool);
}

void lmToolBox::SelectTool(lmEEditTool nTool)
{
    if (nTool == (int)m_nSelTool) return;        //tool already selected

	if (nTool > lmTOOL_NONE && nTool < lmTOOL_MAX)
	{
		SelectButton((int)nTool);
		m_nSelTool = nTool;

		//show panel with options for selected tool
		for(int i=0; i < (int)lmTOOL_MAX; i++)
			if (m_cPanels[i]) m_cPanels[i]->Show(i == nTool);
	}

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
