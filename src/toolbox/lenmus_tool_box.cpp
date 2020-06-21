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
#include "lenmus_tool_box.h"
#include "lenmus_tool_box_events.h"
#include "lenmus_tool_page.h"
#include "lenmus_button.h"
#include "lenmus_tool_page_selector.h"

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
#include "lenmus_tool_page_rhythmic_dictation.h"
#include "lenmus_tool_page_top_level.h"
//TO_ADD: add here the new tool panel include file


//wxWidgets
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
    #include <wx/cursor.h>
    #include <wx/statline.h>
    #include <wx/sizer.h>
    #include <wx/panel.h>
    #include <wx/notebook.h>
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


wxBEGIN_EVENT_TABLE(ToolBox, wxPanel)
    EVT_MENU_RANGE (ID_BUTTON, ID_BUTTON+NUM_BUTTONS-1, ToolBox::on_page_selected)
    EVT_SIZE (ToolBox::OnResize)
    LM_EVT_UPDATE_UI(ToolBox::on_update_UI)
wxEND_EVENT_TABLE()

IMPLEMENT_CLASS(ToolBox, wxPanel)


//---------------------------------------------------------------------------------------
ToolBox::ToolBox(wxWindow* parent, wxWindowID id, ApplicationScope& appScope)
    : wxPanel(parent, id, wxPoint(0,0), wxSize(170, -1), wxBORDER_NONE)
    , m_appScope(appScope)
	, m_nCurPageID(k_page_none)
	, m_context(k_key_context_notes)
{
	//set colors
	m_colors.SetBaseColor( wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE) );

	//initialize tool pages's array with instances of all defined pages
    for (int i=0; i < (int)k_page_max; i++)
    {
        ToolPage* pPage = CreatePage((EToolPageID)i);
        AddPage(pPage, (EToolPageID)i);
        m_fIsPageActivable[i] = true;
    }

	CreateControls();
}

//---------------------------------------------------------------------------------------
void ToolBox::CreateControls()
{
    //the main sizer, to contain the three areas
    wxBoxSizer* pMainSizer = new wxBoxSizer(wxVERTICAL);

    //the mouse mode block: a panel for the mouse mode group
    m_pMouseModeGroup = new GrpMouseMode(this, pMainSizer, &m_colors);
    m_pMouseModeGroup->create_controls_in_group(pMainSizer);


    //the selection block, to choose the active tool page
	m_pChoice = new PageSelector(this, ID_BUTTON, _("Change tool set ..."), wxDefaultPosition, wxSize(166, 25));
    int iPage = 0;
    std::vector<ToolPage*>::iterator it;
    for(it=m_cPages.begin(); it != m_cPages.end(); ++it, ++iPage)
    {
        wxString& name = (*it)->get_selector();
        if (!name.empty())
            m_pChoice->add_page_selector(iPage, ID_BUTTON+iPage, name);
    }
	pMainSizer->Add(m_pChoice, 0, wxEXPAND|wxRIGHT|wxTOP, 2);

    //the tool page block, initialized with an empty page
	m_pPageSizer = new wxBoxSizer( wxVERTICAL );

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
void ToolBox::save_configuration(ToolBoxConfiguration* pConfig)
{
    //active pages
    for (int i=0; i < k_page_max; i++)
        pConfig->m_fIsPageActivable[i] = m_fIsPageActivable[i];

    //other info
    pConfig->m_activePage = m_nCurPageID;             //selected page
    pConfig->m_fMouseModeVisible = true;    //TODO
    //pConfig->m_fPageSelectorsVisible = m_pSelectPanel->IsShown();
    pConfig->m_context = m_context;
}

//---------------------------------------------------------------------------------------
void ToolBox::load_configuration(const ToolBoxConfiguration& config)
{
    //active pages
    int numActive = 0;
    for (int i=0; i < (int)k_page_max; i++)
    {
        m_fIsPageActivable[i] = config.m_fIsPageActivable[i];
        m_pChoice->enable_page(i, m_fIsPageActivable[i]);
        if (m_fIsPageActivable[i])
            ++numActive;
    }
    m_pChoice->selector_visible(numActive > 1);

    //other info
    m_nCurPageID = config.m_activePage;             //selected page
    //config.m_fMouseModeVisible = true;    //TODO
    //m_pSelectPanel->Show( config.m_fPageSelectorsVisible );


    //GetSizer()->Layout();

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
            pPage->create_tool_groups();
            return pPage;
        }

        case k_page_barlines:
            return new ToolPageBarlines(this);

        case k_page_rhythmic_dictation:
            return new ToolPageRhythmicDictation(this);

        case k_page_symbols:
            return new ToolPageSymbols(this);

        case k_page_top_level:
            return new ToolPageTopLevel(this);

        //TO_ADD: Add a new case block for creating the new tool panel
        default:
            wxASSERT(false);
    }
    return nullptr;
}

//---------------------------------------------------------------------------------------
void ToolBox::AddPage(ToolPage* pPage, int WXUNUSED(nToolId))
{
    //Adds a page to the toolbox.

	//add the page to the panel's array
    pPage->Show(false);
    m_cPages.push_back(pPage);
}

//---------------------------------------------------------------------------------------
void ToolBox::mark_page_as_activable(int pageID)
{
    m_fIsPageActivable[pageID] = true;
}

//---------------------------------------------------------------------------------------
void ToolBox::on_page_selected(wxCommandEvent& event)
{
	SelectToolPage((EToolPageID)(event.GetId() - ID_BUTTON));
}

//---------------------------------------------------------------------------------------
void ToolBox::SelectToolPage(EToolPageID iPage)
{
	if (!(iPage > k_page_none && iPage < k_page_max))
        return;

	m_nCurPageID = iPage;

    //hide current page and save it
    wxPanel* pOldPage = m_pCurPage;
    pOldPage->Hide();

    //show new one
    m_pCurPage = m_cPages[iPage];
    m_pCurPage->Show();
    m_pPageSizer->Replace(pOldPage, m_pCurPage);
    m_pCurPage->SetFocus();
    GetSizer()->Layout();

    //change page title
//    m_pPageTitle->SetLabel(static_cast<ToolPage*>(m_pCurPage)->get_title());
    m_pChoice->set_page_title(static_cast<ToolPage*>(m_pCurPage)->get_title());
    m_pCurPage->SetFocus();

    //post tool box page change event to the active controller
    ToolBoxPageChangedEvent event(iPage);
    ::wxPostEvent(this, event);
}

//---------------------------------------------------------------------------------------
void ToolBox::OnResize(wxSizeEvent& WXUNUSED(event))
{
//TODO TB
    //wxSize newSize = event.GetSize();
    //wxLogMessage("[ToolBox::OnResize] New size: %d, %d", newSize.x, newSize.y);
}

////---------------------------------------------------------------------------------------
//wxMenu* ToolBox::GetContextualMenuForSelectedPage()
//{
////TODO TB
////    ToolPage* pSelPage = m_cPages[m_nCurPageID];
////    return pSelPage->GetContextualMenuForToolPage();
//    return nullptr;
//}

////---------------------------------------------------------------------------------------
//void ToolBox::OnPopUpMenuEvent(wxCommandEvent& event)
//{
////TODO TB
////    ToolPage* pSelPage = m_cPages[m_nCurPageID];
////    pSelPage->OnPopUpMenuEvent(event);
//}

//---------------------------------------------------------------------------------------
int ToolBox::get_mouse_mode()
{
    //Determines selected mouse mode (pointer, data entry, eraser, etc.)

    return m_pMouseModeGroup->get_mouse_mode();
}

////---------------------------------------------------------------------------------------
//wxString ToolBox::GetToolShortDescription()
//{
//    //returns a short description of the selected tool. This description is used to
//    //be displayed in the status bar
//
//    return get_selected_page()->GetToolShortDescription();
//}

//---------------------------------------------------------------------------------------
EToolGroupID ToolBox::GetCurrentGroupID()
{
    return get_selected_page()->GetCurrentGroupID();
}

//---------------------------------------------------------------------------------------
EToolID ToolBox::get_selected_tool_id()
{
    return get_selected_page()->get_selected_tool_id();
}

//---------------------------------------------------------------------------------------
bool ToolBox::process_key(wxKeyEvent& event)
{
    //returns true if event is accepted and processed

	bool fProcessed = true;

    int key = event.GetKeyCode();
    int keyFlags = event.GetModifiers();

    KeyTranslator* pTr = m_appScope.get_key_translator();
    int cmd = pTr->translate(m_context, key, keyFlags);

    if (cmd == k_cmd_null)
        return false;   //not processed

	//page selection commands
    switch (cmd)
	{
		case k_cmd_select_toolpage_clefs:
			SelectToolPage(k_page_clefs);
			break;

		case k_cmd_select_toolpage_notes:
			SelectToolPage(k_page_notes);
			break;

		case k_cmd_select_toolpage_barlines:
			SelectToolPage(k_page_barlines);
			break;

		case k_cmd_select_toolpage_symbols:
			SelectToolPage(k_page_symbols);
			break;

        //TO_ADD: code to select the new page to add

		default:
			fProcessed = false;
	}

	//if not processed, check if specific for current selected tool panel
	if (!fProcessed)
	{
        ToolPage* pCurPage = get_selected_page();
        long context = pCurPage->get_key_translation_context();
        int cmd = pTr->translate(context, key, keyFlags);
        fProcessed = pCurPage->process_command(cmd);
	}

	return fProcessed;
}

//---------------------------------------------------------------------------------------
int ToolBox::translate_key(int key, int keyFlags)
{
    ToolPage* pCurPage = get_selected_page();
    return pCurPage->translate_key(key, keyFlags);
}

//---------------------------------------------------------------------------------------
void ToolBox::on_update_UI(lmUpdateUIEvent& event)
{
    LOMSE_LOG_DEBUG(lomse::Logger::k_events, string(""));

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
}

//---------------------------------------------------------------------------------------
void ToolBox::enable_current_page(bool fEnable)
{
    m_pCurPage->Enable(fEnable);
}

//---------------------------------------------------------------------------------------
void ToolBox::enable_page_selectors(bool fEnable)
{
    m_pChoice->enable(fEnable);
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

    ToolPage* pCurPage = get_selected_page();
    if (pCurPage)
        pCurPage->synchronize_with_cursor(fEnable, pCursor);
}

//---------------------------------------------------------------------------------------
void ToolBox::synchronize_with_selection(bool fEnable, SelectionSet* pSelection)
{
    //enable toolbox options depending on current selected objects

    ToolPage* pCurPage = get_selected_page();
    if (pCurPage)
        pCurPage->synchronize_with_selection(fEnable, pSelection);
}

//---------------------------------------------------------------------------------------
void ToolBox::update_tools_info(ToolsInfo* pInfo)
{
    ToolPage* pCurPage = get_selected_page();
    pCurPage->update_tools_info(pInfo);

    //if page changed or mouse mode changed, reconfigure toolbox for current mouse mode
    //TODO: Refactor. This should be done when ToolPage changed and when Mouse mode changed,
    //      but not here.
//    EToolPageID newPageID = get_selected_page_id();
    int newMouseMode = get_mouse_mode();
	if (newMouseMode != pInfo->mouseMode)   // || pInfo->pageID != newPageID)
    {
        pInfo->mouseMode = newMouseMode;
//        pInfo->pageID = newPageID;
        pCurPage->ReconfigureForMouseMode(newMouseMode);
    }
}



//=======================================================================================
// GrpMouseMode implementation
//=======================================================================================
GrpMouseMode::GrpMouseMode(wxPanel* pParent, wxBoxSizer* pMainSizer, ToolboxTheme* pColours)
        : ToolButtonsGroup(pParent, k_group_type_options, lm_NUM_MOUSE_MODE_BUTTONS,
                             lmTBG_ONE_SELECTED, pMainSizer,
                             lmID_BT_MouseMode_Pointer, k_tool_mouse_mode, pColours)
{
}

//---------------------------------------------------------------------------------------
void GrpMouseMode::create_controls_in_group(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group
    set_group_title(_("Mouse mode"));
    wxBoxSizer* pCtrolsSizer = create_main_sizer_for_group(pMainSizer);

    wxBoxSizer* pButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
	pCtrolsSizer->Add(pButtonsSizer);
    wxSize btSize(24, 24);

    //Selection tool mode
	m_pButton[0] = new CheckButton(this, lmID_BT_MouseMode_Pointer, wxBitmap(24, 24),
                                   wxDefaultPosition, wxSize(24, 24));
    wxString sBtName = "mouse_mode_selection";
    m_pButton[0]->SetBitmapUp(sBtName, "", btSize);
    m_pButton[0]->SetBitmapDown(sBtName, "button_selected_flat", btSize);
    m_pButton[0]->SetBitmapOver(sBtName, "button_over_flat", btSize);
	m_pButton[0]->SetToolTip(_("Mouse will behave as pointer and selection tool"));
	pButtonsSizer->Add(m_pButton[0], wxSizerFlags(0).Border(wxALL, 0) );

    //mouse mode
	m_pButton[1] = new CheckButton(this, lmID_BT_MouseMode_DataEntry, wxBitmap(24, 24),
                                   wxDefaultPosition, wxSize(24, 24));
    sBtName = "mouse_mode_data_entry";
    m_pButton[1]->SetBitmapUp(sBtName, "", btSize);
    m_pButton[1]->SetBitmapDown(sBtName, "button_selected_flat", btSize);
    m_pButton[1]->SetBitmapOver(sBtName, "button_over_flat", btSize);
	m_pButton[1]->SetToolTip(_("Mouse will be used to add objects to the score"));
	pButtonsSizer->Add(m_pButton[1], wxSizerFlags(0).Border(wxALL, 0) );

    this->Layout();

	SetMouseMode(k_mouse_mode_pointer);	    //select pointer tool mode
}

//---------------------------------------------------------------------------------------
int GrpMouseMode::get_mouse_mode()
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

//---------------------------------------------------------------------------------------
void GrpMouseMode::update_tools_info(ToolsInfo* pInfo)
{
    pInfo->mouseMode = get_mouse_mode();
}



//=======================================================================================
// ToolsInfo implementation
//=======================================================================================
ToolsInfo::ToolsInfo()
	: m_pToolBox(nullptr)
    , toolID(k_tool_none)
    , noteType(k_quarter)
	, dots(0)
    , notehead(k_notehead_quarter)
    , acc(k_no_accidentals)
    , octave(4)
    , voice(1)
    , fIsNote(true)
    , clefType(k_clef_G2)
    , barlineType(k_barline_simple)
    , keyType(k_key_C)
    , timeBeatType(4)
    , timeNumBeats(4)
    , mouseMode(k_mouse_mode_pointer)
    , clickCmd(k_cmd_null)

//    //to save options selected by user in ToolBox
//    bool            m_fToolBoxSavedOptions;
//    int             m_nTbAcc;
//    int             m_nTbDots;
//    int             m_nTbDuration;
{
}

//---------------------------------------------------------------------------------------
void ToolsInfo::update_toolbox_info(ToolBox* pToolBox)
{
    //get current toolbox selections: page, group, tool and mouse mode

    m_pToolBox = pToolBox;
    get_toolbox_info();
}

//---------------------------------------------------------------------------------------
void ToolsInfo::get_toolbox_info()
{
    toolID = m_pToolBox->get_selected_tool_id();

    //get values for current page
    m_pToolBox->update_tools_info(this);

    //TDO: Transform this into a method in DocumentWindow to get current drag mark?
//    //set dragging marks for current page
//    switch(pageID)
//    {
//        case k_page_notes:
//            m_nToolMarks = lmMARK_TIME_GRID | lmMARK_LEDGER_LINES;
//            break;
//
//        case k_page_clefs:
//            switch (groupID)
//            {
//                case k_grp_ClefType:
//                case k_grp_TimeType:
//                case k_grp_KeyType:
//                default:
//                    m_nToolMarks = lmMARK_MEASURE;
//            }
//            break;
//
//        default:
//            m_nToolMarks = lmMARK_NONE;
//    }
}

//---------------------------------------------------------------------------------------
void ToolsInfo::enable_tools(bool fEnable)
{
    if (m_pToolBox)
        m_pToolBox->enable_tools(fEnable);
}


}   //namespace lenmus
