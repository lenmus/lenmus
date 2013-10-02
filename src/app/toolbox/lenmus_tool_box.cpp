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
#include "lenmus_tool_box.h"
#include "lenmus_tool_box_events.h"
#include "lenmus_tool_page.h"
#include "lenmus_button.h"
#include "lenmus_edit_interface.h"

//lomse
#include <lomse_events.h>
#include <lomse_interactor.h>
using namespace lomse;

//---------------------------------------------------------------------------------------
//AWARE
//
//    Things to do to add a new tools panel to the Tools Box dialog:
//     1. Create a new panel class derived from ToolPage
//     2. Look for "//TO_ADD:" tags in lenmus_tool_box.h and follow instructions there
//     3. Look for "//TO_ADD:" tags in this file and follow instructions there
//
//---------------------------------------------------------------------------------------

// Panels
#include "lenmus_tool_page_notes.h"
#include "lenmus_tool_page_barlines.h"
#include "lenmus_tool_page_symbols.h"
#include "lenmus_tool_page_clefs.h"
//TO_ADD: add here the new tool panel include file


//wxWidgets
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
    #include <wx/cursor.h>
    #include <wx/statline.h>
    #include <wx/sizer.h>
#endif


namespace lenmus
{

//=======================================================================================
// ToolBox implementation
//=======================================================================================

//layout parameters
const int SPACING = 4;          //spacing (pixels) around each sizer
const int BUTTON_SPACING = 4;	//spacing (pixels) between buttons
const int BUTTON_SIZE = 32;		//tools button size (pixels)
const int NUM_COLUMNS = 4;      //number of buttons per row
// ToolBox width = NUM_COLUMNS * BUTTON_SIZE + 2*(NUM_COLUMNS-1)*BUTTON_SPACING + 2*SPACING
//				 = 4*32 + 2*3*4 + 2*4 = 128+24+8 = 160

//const int ID_BUTTON = 2200;
#define lm_NUM_MOUSE_MODE_BUTTONS 2

enum
{
    ID_BUTTON = 2200,
    lmID_BT_MouseMode_Pointer = ID_BUTTON + 16,    //+ NUM_BUTTONS,
    lmID_BT_MouseMode_DataEntry,
};


BEGIN_EVENT_TABLE(ToolBox, wxPanel)
//	EVT_CHAR (ToolBox::OnKeyPress)
    EVT_COMMAND_RANGE (ID_BUTTON, ID_BUTTON+NUM_BUTTONS-1, wxEVT_COMMAND_BUTTON_CLICKED, ToolBox::OnButtonClicked)
    EVT_SIZE (ToolBox::OnResize)
    //EVT_ERASE_BACKGROUND(ToolBox::OnEraseBackground)
    LM_EVT_UPDATE_UI(ToolBox::on_update_UI)
END_EVENT_TABLE()

IMPLEMENT_CLASS(ToolBox, wxPanel)


//---------------------------------------------------------------------------------------
ToolBox::ToolBox(wxWindow* parent, wxWindowID id, ApplicationScope& appScope)
    : wxPanel(parent, id, wxPoint(0,0), wxSize(170, -1), wxBORDER_NONE)
    , m_appScope(appScope)
	, m_nCurPageID(k_page_none)
{
	//set colors
	m_colors.SetBaseColor( wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE) );

	//initialize pages's array with default standard pages
    for (int i=0; i < (int)k_page_max; i++)
    {
        ToolPage* pPage = CreatePage((EToolPageID)i);
        AddPage(pPage, (EToolPageID)i);
        m_cActivePages[i] = (int)m_cPages.size() - 1;
    }

	CreateControls();

	SelectToolPage(k_page_notes);
}

//---------------------------------------------------------------------------------------
void ToolBox::CreateControls()
{
    //Controls creation for ToolsBox Dlg

    //the main sizer, to contain the three areas
    wxBoxSizer* pMainSizer = new wxBoxSizer(wxVERTICAL);

    //panel for the mouse mode group
    m_pMouseModeGroup = new GrpMouseMode(this, pMainSizer, &m_colors);
    m_pMouseModeGroup->CreateGroupControls(pMainSizer);

    //panel for the fixed group
	m_pSpecialGroup = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
	pMainSizer->Add( m_pSpecialGroup, 0, wxEXPAND, 5 );
    m_pSpecialGroup->Show(false);

    //line
	wxStaticLine* pLine1 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	pMainSizer->Add( pLine1, 0, wxEXPAND|wxTOP, 2);

    //the tool page buttons selection area
	wxPanel* pSelectPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                        wxBORDER_NONE|wxTAB_TRAVERSAL );
    pSelectPanel->SetBackgroundColour(m_colors.Bright());

	wxBoxSizer* pSelectSizer = new wxBoxSizer( wxVERTICAL );

    wxGridSizer* pButtonsSizer = new wxGridSizer(NUM_COLUMNS);

    wxSize btSize(BUTTON_SIZE, BUTTON_SIZE);
    int iB = 0;
    int iMax = m_cPages.size();
    std::vector<ToolPage*>::iterator it;
    for(it=m_cPages.begin(); it != m_cPages.end(); ++it, ++iB)
    {
        m_pButton[iB] = new CheckButton(pSelectPanel, ID_BUTTON + iB, wxBitmap(btSize.x, btSize.y));
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

    //line
	wxStaticLine* pLine2 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	pMainSizer->Add( pLine2, 0, wxEXPAND, 0);

    //the pages
	m_pPageSizer = new wxBoxSizer( wxVERTICAL );

	//int nWidth = NUM_COLUMNS * BUTTON_SIZE + 2*(NUM_COLUMNS-1)*BUTTON_SPACING + 2*SPACING;
    m_pEmptyPage = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(-1, 800),
							   wxBORDER_NONE|wxTAB_TRAVERSAL );
    m_pEmptyPage->SetBackgroundColour(m_colors.Bright());
	m_pCurPage = m_pEmptyPage;
	m_pPageSizer->Add( m_pCurPage, 1, wxEXPAND, SPACING );

	pMainSizer->Add( m_pPageSizer, 1, wxEXPAND, SPACING );

	SetSizer( pMainSizer );
    pMainSizer->SetSizeHints(this);

	Layout();
}

//---------------------------------------------------------------------------------------
ToolBox::~ToolBox()
{
}

//---------------------------------------------------------------------------------------
void ToolBox::GetConfiguration(ToolBoxConfiguration* pConfig)
{
    //Updates received config object with current configuration data

    //active pages
    for (int i=0; i < k_page_max; i++)
        pConfig->m_Pages[i] = m_cActivePages[i];

    //other info
    pConfig->m_pSpecialGroup = m_pSpecialGroup;
    pConfig->m_fSpecialGroupVisible = m_pSpecialGroup->IsShown();
    pConfig->m_nCurPageID = m_nCurPageID;             //selected page

    //mark as valid
    pConfig->m_fIsValid = true;
}

//---------------------------------------------------------------------------------------
void ToolBox::SetConfiguration(ToolBoxConfiguration* pConfig)
{
    //Reconfigures ToolBox as specified by received pConfig parameter

    if (!pConfig)
    {
        SetDefaultConfiguration();
        return;
    }

    wxASSERT(pConfig->IsOk());

    //active pages
    for (int i=0; i < (int)k_page_max; i++)
        m_cActivePages[i] = pConfig->m_Pages[i];

    //other info
    m_pSpecialGroup = pConfig->m_pSpecialGroup;   //panel for the special group
    m_nCurPageID = pConfig->m_nCurPageID;             //selected page

    //apply changes
    if (m_pSpecialGroup)
        m_pSpecialGroup->Show(pConfig->m_fSpecialGroupVisible);

    GetSizer()->Layout();

    SelectToolPage(m_nCurPageID);
}

//---------------------------------------------------------------------------------------
ToolPage* ToolBox::CreatePage(EToolPageID nPanel)
{
    switch(nPanel)
    {
        case k_page_clefs:
            return new ToolPageClefs(this);

        case k_page_notes:
        {
            ToolPageNotes* pPage = new ToolPageNotesStd(this);
            pPage->CreateGroups();
            return pPage;
        }

        case k_page_barlines:
            return new ToolPageBarlines(this);

        case k_page_symbols:
            return new ToolPageSymbols(this);

        //TO_ADD: Add a new case block for creating the new tool panel
        default:
            wxASSERT(false);
    }
    return NULL;
}

//---------------------------------------------------------------------------------------
void ToolBox::AddPage(ToolPage* pPage, int nToolId)
{
    //Adds a page to the toolbox.

	//add the page to the panel's array
    pPage->Show(false);
    m_cPages.push_back(pPage);
}

//---------------------------------------------------------------------------------------
void ToolBox::SetAsActive(ToolPage* pPage, int nToolId)
{
    //locate page pPage;
    int nPage=0;
    std::vector<ToolPage*>::iterator it;
    for (it = m_cPages.begin(); it != m_cPages.end(); ++it, ++nPage)
    {
        if (*it == pPage)
            break;
    }
    wxASSERT(*it == pPage);

    int nOldPage = m_cActivePages[nToolId];
    m_cActivePages[nToolId] = nPage;

    //hide old page and show the new active one
    m_cPages[nOldPage]->Hide();
    pPage->Show();
    m_pPageSizer->Replace(m_cPages[nOldPage], pPage);
    pPage->SetFocus();
    GetSizer()->Layout();
}

//---------------------------------------------------------------------------------------
void ToolBox::OnButtonClicked(wxCommandEvent& event)
{
    //identify button pressed
	SelectToolPage((EToolPageID)(event.GetId() - ID_BUTTON));
}

//---------------------------------------------------------------------------------------
void ToolBox::SelectToolPage(EToolPageID nTool)
{
	if (!(nTool > k_page_none && nTool < k_page_max))
        return;

    SelectButton((int)nTool);
	m_nCurPageID = nTool;

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
    EditInterface* pEditGui = m_appScope.get_edit_gui();
    pEditGui->set_focus_on_document_window();

    //post tool box page change event to the active controller
    ToolBoxPageChangedEvent event(nTool);
    ::wxPostEvent(this, event);
}

//---------------------------------------------------------------------------------------
ToolPageNotes* ToolBox::GetNoteProperties() const
{
    return (ToolPageNotes*)m_cPages[ m_cActivePages[k_page_notes] ];
}

//---------------------------------------------------------------------------------------
void ToolBox::SelectButton(int nTool)
{
	// Set selected button as 'pressed' and all others as 'released'
	for(int i=0; i < (int)k_page_max; i++)
	{
		if (i != nTool)
			m_pButton[i]->Release();
		else
			m_pButton[i]->Press();
	}
}

////---------------------------------------------------------------------------------------
//void ToolBox::OnKeyPress(wxKeyEvent& event)
//{
////TODO TB
////	//redirect all key press events to the active child window
////	GetMainFrame()->RedirectKeyPressEvent(event);
//    wxMessageBox(_T("[ToolBox::OnKeyPress] Key pressed!"));
//}

//---------------------------------------------------------------------------------------
void ToolBox::OnResize(wxSizeEvent& event)
{
//TODO TB
    //wxSize newSize = event.GetSize();
    //wxLogMessage(_T("[ToolBox::OnResize] New size: %d, %d"), newSize.x, newSize.y);
}

//---------------------------------------------------------------------------------------
void ToolBox::AddSpecialTools(wxPanel* pNewPanel, wxEvtHandler* pHandler)
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

//---------------------------------------------------------------------------------------
void ToolBox::SetDefaultConfiguration()
{
    for (int i=0; i < (int)k_page_max; i++)
        SetAsActive(m_cPages[i], i);

    //hide fixed group if displayed
    if (m_pSpecialGroup)
        m_pSpecialGroup->Show(false);

    SelectToolPage(m_nCurPageID);
}

//---------------------------------------------------------------------------------------
wxMenu* ToolBox::GetContextualMenuForSelectedPage()
{
//TODO TB
//    ToolPage* pSelPage = m_cPages[ m_cActivePages[m_nCurPageID] ];
//    return pSelPage->GetContextualMenuForToolPage();
    return NULL;
}

//---------------------------------------------------------------------------------------
void ToolBox::OnPopUpMenuEvent(wxCommandEvent& event)
{
//TODO TB
//    ToolPage* pSelPage = m_cPages[ m_cActivePages[m_nCurPageID] ];
//    pSelPage->OnPopUpMenuEvent(event);
}

//---------------------------------------------------------------------------------------
int ToolBox::GetMouseMode()
{
    //Determines selected mouse mode (pointer, data entry, eraser, etc.)

    return m_pMouseModeGroup->GetMouseMode();
}

//---------------------------------------------------------------------------------------
wxString ToolBox::GetToolShortDescription()
{
    //returns a short description of the selected tool. This description is used to
    //be displayed in the status bar

    return GetSelectedPage()->GetToolShortDescription();
}

//---------------------------------------------------------------------------------------
EToolGroupID ToolBox::GetCurrentGroupID()
{
    return GetSelectedPage()->GetCurrentGroupID();
}

//---------------------------------------------------------------------------------------
EToolID ToolBox::GetCurrentToolID()
{
    return GetSelectedPage()->GetCurrentToolID();
}

//---------------------------------------------------------------------------------------
void ToolBox::OnEraseBackground(wxEraseEvent& event)
{
    //wxDC* pDC = event.GetDC();
    //pDC->SetBrush(*wxRED_BRUSH);
    //pDC->DrawRectangle(this->GetRect());
}

//---------------------------------------------------------------------------------------
bool ToolBox::process_key(wxKeyEvent& event)
{
    //returns true if event is accepted and processed

    int nKeyCode = event.GetKeyCode();
	bool fProcessed = true;

	//Verify common keys working with all tools
	switch (nKeyCode)
	{
		case WXK_F2:
			SelectToolPage(k_page_clefs);
			break;

		case WXK_F3:
			SelectToolPage(k_page_notes);
			break;

		case WXK_F4:
			SelectToolPage(k_page_barlines);
			break;

		case WXK_F5:
			SelectToolPage(k_page_symbols);
			break;

        //TO_ADD: code to select the new page to add

		default:
			fProcessed = false;
	}

	//if not processed, check if specific for current selected tool panel
	if (!fProcessed)
	{
        ToolPage* pCurPage = GetSelectedPage();
        fProcessed = pCurPage->process_key(event);
	}

	return fProcessed;
}

//---------------------------------------------------------------------------------------
void ToolBox::on_update_UI(lmUpdateUIEvent& event)
{
    LOMSE_LOG_DEBUG(lomse::Logger::k_events, "");

    SpEventUpdateUI pEv = event.get_lomse_event();
    WpInteractor wpInteractor = pEv->get_interactor();
    if (SpInteractor sp = wpInteractor.lock())
    {
        SelectionSet* pSelection = pEv->get_selection();
        DocCursor* pCursor = pEv->get_cursor();
        synchronize_tools(pSelection, pCursor);
    }
    else
        LOMSE_LOG_TRACE(lomse::Logger::k_events, "Event is obsolete");
}

//---------------------------------------------------------------------------------------
void ToolBox::enable_tools(bool fEnable)
{
    enable_mouse_mode_buttons(fEnable);
    enable_current_page(fEnable);
    enable_page_selectors(fEnable);
}

//---------------------------------------------------------------------------------------
void ToolBox::enable_mouse_mode_buttons(bool fEnable)
{
    m_pMouseModeGroup->Enable(fEnable);
    m_pSpecialGroup->Enable(fEnable);
}

//---------------------------------------------------------------------------------------
void ToolBox::enable_current_page(bool fEnable)
{
    m_pCurPage->Enable(fEnable);
}

//---------------------------------------------------------------------------------------
void ToolBox::enable_page_selectors(bool fEnable)
{
    int iMax = m_cPages.size();
    for(int i=0; i < iMax; ++i)
        m_pButton[i]->Enable(fEnable);
}

//---------------------------------------------------------------------------------------
void ToolBox::synchronize_tools(SelectionSet* pSelection, DocCursor* pCursor)
{
    //synchronize toolbox selected options with current selection and cursor object

    if (!pSelection->empty())
    {
        //there is a selection. Disable options related to cursor
        synchronize_with_cursor(false);
        synchronize_with_selection(true, pSelection);
    }
    else
    {
        //No selection. Disable options related to selections
        synchronize_with_cursor(true, pCursor);
        synchronize_with_selection(false);
    }

//	//options independent from caret/selection
//
//        case k_page_notes:
//            //voice and octave
//            {
//                ToolPageNotes* pPage = (ToolPageNotes*)pToolBox->GetToolPanel(k_page_notes);
//                lmGrpOctave* pGrp = (lmGrpOctave*)pPage->GetToolGroup(k_grp_Octave);
//                pGrp->SetOctave(m_nOctave);
//            }
//            break;
}

//---------------------------------------------------------------------------------------
void ToolBox::synchronize_with_cursor(bool fEnable, DocCursor* pCursor)
{
    //enable toolbox options depending on current pointed object

    ToolPage* pCurPage = GetSelectedPage();
    if (pCurPage)
        pCurPage->synchronize_with_cursor(fEnable, pCursor);
}

//---------------------------------------------------------------------------------------
void ToolBox::synchronize_with_selection(bool fEnable, SelectionSet* pSelection)
{
    //enable toolbox options depending on current selected objects

    ToolPage* pCurPage = GetSelectedPage();
    if (pCurPage)
        pCurPage->synchronize_with_selection(fEnable, pSelection);
}

////---------------------------------------------------------------------------------------
//void ToolBox::RestoreToolBoxSelections()
//{
//    //restore toolbox selected options to those previously selected by user
//
//    if (!m_fToolBoxSavedOptions) return;        //nothing to do
//
//    m_fToolBoxSavedOptions = false;
//
//    switch( pToolBox->GetCurrentPageID() )
//    {
//        case k_page_none:
//            return;         //nothing selected!
//
//        case k_page_notes:
//            //restore duration, dots, accidentals
//            {
//                ToolPageNotes* pTool = (ToolPageNotes*)pToolBox->GetToolPanel(k_page_notes);
//                pTool->SetNoteDotsButton(m_nTbDots);
//                pTool->SetNoteAccButton(m_nTbAcc);
//                pTool->SetNoteDurationButton(m_nTbDuration);
//            }
//            break;
//
//        case k_page_clefs:
//        case lmPAGE_BARLINES:
//        case lmPAGE_SYMBOLS:
//            lmTODO(_T("[ToolBox::RestoreToolBoxSelections] Code to restore this tool"));
//            break;
//
//        default:
//            wxASSERT(false);
//    }
//}
//



//=======================================================================================
// GrpMouseMode implementation
//=======================================================================================
GrpMouseMode::GrpMouseMode(wxPanel* pParent, wxBoxSizer* pMainSizer, ToolboxTheme* pColours)
        : ToolButtonsGroup(pParent, k_group_type_options, lm_NUM_MOUSE_MODE_BUTTONS,
                             lmTBG_ONE_SELECTED, pMainSizer,
                             lmID_BT_MouseMode_Pointer, k_tool_none, pColours)
{
}

//---------------------------------------------------------------------------------------
void GrpMouseMode::CreateGroupControls(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group
    SetGroupTitle(_("Mouse mode"));
    wxBoxSizer* pCtrolsSizer = CreateGroupSizer(pMainSizer);

    wxBoxSizer* pButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
	pCtrolsSizer->Add(pButtonsSizer);
    wxSize btSize(24, 24);

    //Selection tool mode
	m_pButton[0] = new CheckButton(this, lmID_BT_MouseMode_Pointer, wxBitmap(24, 24));
    wxString sBtName = _T("mouse_mode_selection");
    m_pButton[0]->SetBitmapUp(sBtName, _T(""), btSize);
    m_pButton[0]->SetBitmapDown(sBtName, _T("button_selected_flat"), btSize);
    m_pButton[0]->SetBitmapOver(sBtName, _T("button_over_flat"), btSize);
	m_pButton[0]->SetToolTip(_("Mouse will behave as pointer and selection tool"));
	pButtonsSizer->Add(m_pButton[0], wxSizerFlags(0).Border(wxALL, 0) );

    //mouse mode
	m_pButton[1] = new CheckButton(this, lmID_BT_MouseMode_DataEntry, wxBitmap(24, 24));
    sBtName = _T("mouse_mode_data_entry");
    m_pButton[1]->SetBitmapUp(sBtName, _T(""), btSize);
    m_pButton[1]->SetBitmapDown(sBtName, _T("button_selected_flat"), btSize);
    m_pButton[1]->SetBitmapOver(sBtName, _T("button_over_flat"), btSize);
	m_pButton[1]->SetToolTip(_("Mouse will be used to add objects to the score"));
	pButtonsSizer->Add(m_pButton[1], wxSizerFlags(0).Border(wxALL, 0) );

    this->Layout();

	SetMouseMode(k_mouse_mode_pointer);	    //select pointer tool mode
}

//---------------------------------------------------------------------------------------
int GrpMouseMode::GetMouseMode()
{
    switch(m_nSelButton)
    {
        case 0:     return k_mouse_mode_pointer;
        case 1:     return k_mouse_mode_data_entry;
        default:    return k_mouse_mode_undefined;
    }
}

//---------------------------------------------------------------------------------------
void GrpMouseMode::SetMouseMode(int nMouseMode)
{
    switch(nMouseMode)
    {
        case k_mouse_mode_pointer:      SelectButton(0);    break;
        case k_mouse_mode_data_entry:   SelectButton(1);    break;
        default:
            wxASSERT(false);
    }
}


}   //namespace lenmus
