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
#include "ToolBoxEvents.h"
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
#include "ToolBarlines.h"
//TO_ADD: add here the new tool panel include file


//-------------------------------------------------------------------------------------------------
// lmToolBoxConfiguration: Helper class to define a ToolBox configuration
//-------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------
// lmToolBox implementation
//-------------------------------------------------------------------------------------------------

//layout parameters
const int SPACING = 4;          //spacing (pixels) around each sizer
const int BUTTON_SPACING = 4;	//spacing (pixels) between buttons
const int BUTTON_SIZE = 32;		//tools button size (pixels)
const int NUM_COLUMNS = 4;      //number of buttons per row
// ToolBox width = NUM_COLUMNS * BUTTON_SIZE + 2*(NUM_COLUMNS-1)*BUTTON_SPACING + 2*SPACING
//				 = 4*32 + 2*3*4 + 2*4 = 128+24+8 = 160

//const int ID_BUTTON = 2200;
#define lm_NUM_ENTRY_MODE_BUTTONS 2

enum
{
    ID_BUTTON = 2200,
    lmID_BT_EntryMode_Keyboard = ID_BUTTON + 16,    //+ NUM_BUTTONS,
    lmID_BT_EntryMode_Mouse,
};


BEGIN_EVENT_TABLE(lmToolBox, wxPanel)
	EVT_CHAR (lmToolBox::OnKeyPress)
    EVT_COMMAND_RANGE (ID_BUTTON, ID_BUTTON+NUM_BUTTONS-1, wxEVT_COMMAND_BUTTON_CLICKED, lmToolBox::OnButtonClicked)
    EVT_SIZE (lmToolBox::OnResize)
END_EVENT_TABLE()

IMPLEMENT_CLASS(lmToolBox, wxPanel)


lmToolBox::lmToolBox(wxWindow* parent, wxWindowID id)
    : wxPanel(parent, id, wxPoint(0,0), wxSize(170, -1), wxBORDER_NONE)
	, m_nSelTool(lmPAGE_NONE)
{
	//set colors
	m_colors.SetBaseColor( wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE) );

	//initialize pages's array with default standard pages
    for (int i=0; i < (int)lmPAGE_MAX; i++)
	{
        lmToolPage* pPage = CreatePage((lmEToolPage)i);
        AddPage(pPage, (lmEToolPage)i);
        m_cActivePages[i] = (int)m_cPages.size() - 1;
    }

	CreateControls();

	SelectToolPage(lmPAGE_NOTES);

}

void lmToolBox::CreateControls()
{
    //Controls creation for ToolsBox Dlg

    //the main sizer, to contain the three areas
    wxBoxSizer* pMainSizer = new wxBoxSizer(wxVERTICAL);

    //panel for the entry mode group
    m_pEntryModeGroup = new lmGrpEntryMode(this, pMainSizer, &m_colors);

    //panel for the fixed group
	m_pSpecialGroup = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
	pMainSizer->Add( m_pSpecialGroup, 0, wxEXPAND, 5 );
    m_pSpecialGroup->Show(false);

    //the tool page buttons selection area
	wxPanel* pSelectPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    pSelectPanel->SetBackgroundColour(m_colors.Normal());

	wxBoxSizer* pSelectSizer = new wxBoxSizer( wxVERTICAL );

    wxGridSizer* pButtonsSizer = new wxGridSizer(NUM_COLUMNS);

    wxSize btSize(BUTTON_SIZE, BUTTON_SIZE);
    int iB = 0;
    int iMax = m_cPages.size();
    std::vector<lmToolPage*>::iterator it;
    for(it=m_cPages.begin(); it != m_cPages.end(); ++it, ++iB)
    {
        m_pButton[iB] = new lmCheckButton(pSelectPanel, ID_BUTTON + iB, wxBitmap(btSize.x, btSize.y));
        m_pButton[iB]->SetBitmapUp((*it)->GetPageBitmapName(), _T(""), btSize);
        m_pButton[iB]->SetBitmapDown((*it)->GetPageBitmapName(), _T("button_selected_flat"), btSize);
        m_pButton[iB]->SetBitmapOver((*it)->GetPageBitmapName(), _T("button_over_flat"), btSize);
        m_pButton[iB]->SetToolTip((*it)->GetPageToolTip());
		int sides = 0;
		if (iB > 0) sides |= wxLEFT;
		if (iB < iMax-1) sides |= wxRIGHT;
		pButtonsSizer->Add(m_pButton[iB],
						   wxSizerFlags(0).Border(sides, BUTTON_SPACING) );
    }

    pSelectSizer->Add( pButtonsSizer, 1, wxEXPAND|wxALL, SPACING );

	pSelectPanel->SetSizer( pSelectSizer );
	pSelectPanel->Layout();
	pSelectSizer->Fit( pSelectPanel );
	pMainSizer->Add( pSelectPanel, 0, 0, SPACING );

    //the pages
	m_pPageSizer = new wxBoxSizer( wxVERTICAL );

	int nWidth = NUM_COLUMNS * BUTTON_SIZE + 2*(NUM_COLUMNS-1)*BUTTON_SPACING + 2*SPACING;
    m_pEmptyPage = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(-1, 800),
							   wxBORDER_SUNKEN|wxTAB_TRAVERSAL );
    m_pEmptyPage->SetBackgroundColour(m_colors.Bright());
	m_pCurPage = m_pEmptyPage;
	m_pPageSizer->Add( m_pCurPage, 1, wxEXPAND, SPACING );

	pMainSizer->Add( m_pPageSizer, 1, wxEXPAND, SPACING );

	SetSizer( pMainSizer );
    pMainSizer->SetSizeHints(this);
	Layout();
}

lmToolBox::~lmToolBox()
{
}

void lmToolBox::GetConfiguration(lmToolBoxConfiguration* pConfig)
{
    //Updates received config object with current configuration data

    //active pages
    for (int i=0; i < (int)lmPAGE_MAX; i++)
        pConfig->m_Pages[i] = m_cActivePages[i];

    //other info
    pConfig->m_pSpecialGroup = m_pSpecialGroup;
    pConfig->m_fSpecialGroupVisible = m_pSpecialGroup->IsShown();
    pConfig->m_nSelTool = m_nSelTool;             //selected tool

    //mark as valid
    pConfig->m_fIsValid = true;
}

void lmToolBox::SetConfiguration(lmToolBoxConfiguration* pConfig)
{
    //Reconfigures ToolBox as specified by received pConfig parameter

    if (!pConfig)
    {
        SetDefaultConfiguration();
        return;
    }

    wxASSERT(pConfig->IsOk());

    //active pages
    for (int i=0; i < (int)lmPAGE_MAX; i++)
        m_cActivePages[i] = pConfig->m_Pages[i];

    //other info
    m_pSpecialGroup = pConfig->m_pSpecialGroup;   //panel for the special group
    m_nSelTool = pConfig->m_nSelTool;             //selected tool

    //apply changes
    if (m_pSpecialGroup)
        m_pSpecialGroup->Show(pConfig->m_fSpecialGroupVisible);

    GetSizer()->Layout();

    SelectToolPage(m_nSelTool);
}

lmToolPage* lmToolBox::CreatePage(lmEToolPage nPanel)
{
    switch(nPanel) {
		case lmPAGE_SELECTION:
            return (lmToolPage*)NULL;
        case lmPAGE_CLEFS:
            return new lmToolPageClefs(this);
		case lmPAGE_KEY_SIGN:
            return (lmToolPage*)NULL;
		case lmPAGE_TIME_SIGN:
            return (lmToolPage*)NULL;
        case lmPAGE_NOTES:
        {
            lmToolPageNotes* pPage = new lmToolPageNotesStd(this);
            pPage->CreateGroups();
            return pPage;
        }

        case lmPAGE_BARLINES:
            return new lmToolPageBarlines(this);
        //TO_ADD: Add a new case block for creating the new tool panel
        default:
            wxASSERT(false);
    }
    return (lmToolPage*)NULL;

}

void lmToolBox::AddPage(lmToolPage* pPage, int nToolId)
{
    //Adds a page to the toolbox.

	//add the page to the panel's array
    pPage->Show(false);
    m_cPages.push_back(pPage);
}

void lmToolBox::SetAsActive(lmToolPage* pPage, int nToolId)
{
    //locate page pPage;
    int nPage=0;
    std::vector<lmToolPage*>::iterator it;
    for (it = m_cPages.begin(); it != m_cPages.end(); ++it, ++nPage)
    {
        if (*it == pPage)
            break;
    }
    wxASSERT(*it == pPage);

    int nOldPage = m_cActivePages[nToolId];
    m_cActivePages[nToolId] = nPage;

    ////hide old page and show the new active one
    //m_cPages[nOldPage]->Hide();
    //pPage->Show();
    //m_pPageSizer->Replace(m_cPages[nOldPage], pPage);
    //pPage->SetFocus();
    //GetSizer()->Layout();
}

void lmToolBox::OnButtonClicked(wxCommandEvent& event)
{
    //identify button pressed
	SelectToolPage((lmEToolPage)(event.GetId() - ID_BUTTON));
}

void lmToolBox::SelectToolPage(lmEToolPage nTool)
{
	if (!(nTool > lmPAGE_NONE && nTool < lmPAGE_MAX))
        return;

    SelectButton((int)nTool);
	m_nSelTool = nTool;

    //hide current page and save it
    wxPanel* pOldPage = m_pCurPage;
    pOldPage->Hide();

    //show new one
    int nActivePage = m_cActivePages[nTool];
    m_pCurPage = (m_cPages[nActivePage] ? m_cPages[nActivePage] : m_pEmptyPage);
    m_pCurPage->Show();
    m_pPageSizer->Replace(pOldPage, m_pCurPage);
    m_pCurPage->SetFocus();
    GetSizer()->Layout();

    //return focus to active view
    GetMainFrame()->SetFocusOnActiveView();

    //post tool box page change event to the active controller
    wxWindow* pWnd = GetMainFrame()->GetActiveController();
    if (pWnd)
    {
        lmToolBoxPageChangedEvent event(nTool);
        ::wxPostEvent(pWnd, event);
    }

}

lmToolPageNotes* lmToolBox::GetNoteProperties() const 
{ 
    return (lmToolPageNotes*)m_cPages[ m_cActivePages[lmPAGE_NOTES] ];
}

void lmToolBox::SelectButton(int nTool)
{
	// Set selected button as 'pressed' and all others as 'released'
	for(int i=0; i < (int)lmPAGE_MAX; i++)
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
    //wxSize newSize = event.GetSize();
    //wxLogMessage(_T("[lmToolBox::OnResize] New size: %d, %d"), newSize.x, newSize.y);
}

void lmToolBox::AddSpecialTools(wxPanel* pNewPanel, wxEvtHandler* pHandler)
{
    //Adds the special tools panel at top of ToolBox

    wxPanel* pOldPanel = m_pSpecialGroup;
    m_pSpecialGroup->Show(false);

    wxSizer* pMainSizer = GetSizer();
    pMainSizer->Replace(pOldPanel, pNewPanel);

    m_cSpecialGroups.push_back(pNewPanel);

    //delete pOldPanel;
    m_pSpecialGroup = pNewPanel;
    m_pSpecialGroup->Show(true);
    pMainSizer->Layout();
}

void lmToolBox::SetDefaultConfiguration()
{
    for (int i=0; i < (int)lmPAGE_MAX; i++)
        SetAsActive(m_cPages[i], i);

    //hide fixed group if displayed
    if (m_pSpecialGroup)
        m_pSpecialGroup->Show(false);

    SelectToolPage(m_nSelTool);
}

wxMenu* lmToolBox::GetContextualMenuForSelectedPage()
{
    lmToolPage* pSelPage = m_cPages[ m_cActivePages[m_nSelTool] ];
    return pSelPage->GetContextualMenuForToolPage();
}

void lmToolBox::OnPopUpMenuEvent(wxCommandEvent& event)
{
    lmToolPage* pSelPage = m_cPages[ m_cActivePages[m_nSelTool] ];
    pSelPage->OnPopUpMenuEvent(event);
}



//--------------------------------------------------------------------------------
// lmGrpEntryMode implementation
//--------------------------------------------------------------------------------

lmGrpEntryMode::lmGrpEntryMode(wxPanel* pParent, wxBoxSizer* pMainSizer, lmColorScheme* pColours)
        : lmToolButtonsGroup(pParent, lm_NUM_ENTRY_MODE_BUTTONS, lmTBG_ONE_SELECTED, pMainSizer,
                             lmID_BT_EntryMode_Keyboard, pColours)
{
    CreateControls(pMainSizer);
}

void lmGrpEntryMode::CreateControls(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group
    wxBoxSizer* pCtrolsSizer = CreateGroup(pMainSizer, _("Data entry mode"));

    wxBoxSizer* pButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
	pCtrolsSizer->Add(pButtonsSizer);
    wxSize btSize(24, 24);

    //keyboard entry mode
	m_pButton[0] = new lmCheckButton(this, lmID_BT_EntryMode_Keyboard, wxBitmap(24, 24));
    wxString sBtName = _T("data_entry_keyboard");
    m_pButton[0]->SetBitmapUp(sBtName, _T(""), btSize);
    m_pButton[0]->SetBitmapDown(sBtName, _T("button_selected_flat"), btSize);
    m_pButton[0]->SetBitmapOver(sBtName, _T("button_over_flat"), btSize);
	m_pButton[0]->SetToolTip(_T("Use keyboard to enter notes/rests"));
	pButtonsSizer->Add(m_pButton[0], wxSizerFlags(0).Border(wxALL, 0) );

    //mouse entry mode
	m_pButton[1] = new lmCheckButton(this, lmID_BT_EntryMode_Mouse, wxBitmap(24, 24));
    sBtName = _T("data_entry_mouse");
    m_pButton[1]->SetBitmapUp(sBtName, _T(""), btSize);
    m_pButton[1]->SetBitmapDown(sBtName, _T("button_selected_flat"), btSize);
    m_pButton[1]->SetBitmapOver(sBtName, _T("button_over_flat"), btSize);
	m_pButton[1]->SetToolTip(_T("Use mouse to enter notes/rests"));
	pButtonsSizer->Add(m_pButton[1], wxSizerFlags(0).Border(wxALL, 0) );

    this->Layout();

	SelectButton(0);	//select keyboard data entry mode
}


