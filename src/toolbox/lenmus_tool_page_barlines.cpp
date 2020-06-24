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
#include "lenmus_tool_page_barlines.h"

#include "lenmus_tool_box.h"
#include "lenmus_tool_group.h"
#include "lenmus_button.h"
#include "lenmus_utilities.h"

//wxWidgets
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
    #include <wx/xrc/xmlres.h>
    #include <wx/bmpcbox.h>
    #include <wx/statline.h>
    #include <wx/clrpicker.h>
	#include <wx/bmpcbox.h>
#endif


namespace lenmus
{


#define lmSPACING 5

//event IDs
const long k_id_barlines_list = wxNewId();

enum {
	k_id_button_barline = 2600,
	k_num_barline_buttons = 7,
};


wxIMPLEMENT_DYNAMIC_CLASS(ToolPageBarlines, ToolPage);


//---------------------------------------------------------------------------------------
ToolPageBarlines::ToolPageBarlines(wxWindow* parent)
{
    Create(parent);
}

//---------------------------------------------------------------------------------------
void ToolPageBarlines::Create(wxWindow* parent)
{
    //base class
    ToolPage::CreatePage(parent, k_page_barlines);

    //initialize data
    m_sPageToolTip = _("Edit tools for barlines and rehearsal marks");
    m_sPageBitmapName = "tool_barlines";
    m_title = _("Barlines");
    m_selector = _("Barlines and rehearsal marks");

    //create groups
    create_tool_groups();
}

//---------------------------------------------------------------------------------------
void ToolPageBarlines::create_tool_groups()
{
    wxBoxSizer *pMainSizer = GetMainSizer();

    add_group( LENMUS_NEW GrpBarlines2(this, pMainSizer, k_mouse_mode_data_entry) );

	create_layout();
	select_group(k_grp_BarlineType);
}

////---------------------------------------------------------------------------------------
//wxString ToolPageBarlines::GetToolShortDescription()
//{
//    //returns a short description of the selected tool. This description is used to
//    //be displayed in the status bar
//
//    return _("Add barline");
//}



//=======================================================================================
// GrpBarlines implementation
//=======================================================================================

wxBEGIN_EVENT_TABLE(GrpBarlines, ToolGroup)
    EVT_COMBOBOX    (k_id_barlines_list, GrpBarlines::OnBarlinesList)
wxEND_EVENT_TABLE()

static BarlinesDBEntry m_tBarlinesDB[k_max_barline+1];


//---------------------------------------------------------------------------------------
GrpBarlines::GrpBarlines(ToolPage* pParent, wxBoxSizer* WXUNUSED(pMainSizer),
                             int nValidMouseModes)
        : ToolGroup(pParent, k_group_type_tool_selector, pParent->GetColors(),
                      nValidMouseModes)
{
    //To avoid having to translate again barline names, we are going to load them
    //by using global function get_barline_name()
    int i;
    for (i = 0; i < k_max_barline; i++)
    {
        m_tBarlinesDB[i].nBarlineType = (EBarline)i;
        m_tBarlinesDB[i].sBarlineName = get_barline_name((EBarline)i);
    }
    //End of table item
    m_tBarlinesDB[i].nBarlineType = k_barline_unknown;
    m_tBarlinesDB[i].sBarlineName = "";
}

//---------------------------------------------------------------------------------------
void GrpBarlines::create_controls_in_group(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group
    set_group_title(_("Barline"));
    wxBoxSizer* pCtrolsSizer = create_main_sizer_for_group(pMainSizer);

    //bitmap combo box to select the clef
    m_pBarlinesList = new wxBitmapComboBox();
    m_pBarlinesList->Create(this, k_id_barlines_list, wxEmptyString, wxDefaultPosition, wxSize(135, 72),
                       0, nullptr, wxCB_READONLY);

	pCtrolsSizer->Add( m_pBarlinesList, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	this->Layout();

    //initializations
    ToolBox* pToolBox = ((ToolPage*)m_pParent)->GetToolBox();
    ApplicationScope& appScope = pToolBox->get_app_scope();
	load_barlines_bitmap_combobox(appScope, m_pBarlinesList, m_tBarlinesDB);
	select_barline_in_bitmap_combobox(m_pBarlinesList, k_barline_simple);
}

//---------------------------------------------------------------------------------------
void GrpBarlines::OnBarlinesList(wxCommandEvent& WXUNUSED(event))
{
    //Notify owner page about the tool change
    WXUNUSED(event);

    ((ToolPage*)m_pParent)->on_tool_changed(get_selected_tool_id(), get_group_id());
}

//---------------------------------------------------------------------------------------
EBarline GrpBarlines::GetSelectedBarline()
{
	int iB = m_pBarlinesList->GetSelection();
    return m_tBarlinesDB[iB].nBarlineType;
}

//---------------------------------------------------------------------------------------
EToolID GrpBarlines::get_selected_tool_id()
{
    return k_tool_barline;
}

//---------------------------------------------------------------------------------------
void GrpBarlines::update_tools_info(ToolsInfo* pInfo)
{
    pInfo->barlineType = GetSelectedBarline();
}

//---------------------------------------------------------------------------------------
void GrpBarlines::synchronize_with_cursor(bool WXUNUSED(fEnable),
                                          DocCursor* WXUNUSED(pCursor))
{
    //TODO
    EnableGroup(true);
}

//---------------------------------------------------------------------------------------
void GrpBarlines::synchronize_with_selection(bool WXUNUSED(fEnable),
                                             SelectionSet* WXUNUSED(pSelection))
{
    //TODO
    EnableGroup(true);
}

////---------------------------------------------------------------------------------------
//void GrpBarlines::OnAddBarline(wxCommandEvent& event)
//{
//    //insert selected barline
//	WXUNUSED(event);
//	int iB = m_pBarlinesList->GetSelection();
//    DocumentWindow* pSC = GetMainFrame()->GetActiveController();
//    if (pSC)
//    {
//        pSC->InsertBarline(m_tBarlinesDB[iB].nBarlineType);
//
//        //return focus to active view
//        GetMainFrame()->SetFocusOnActiveView();
//    }
//}



//=======================================================================================
// GrpBarlines2 implementation
//=======================================================================================
GrpBarlines2::GrpBarlines2(ToolPage* pParent, wxBoxSizer* pMainSizer,
                           int WXUNUSED(nValidMouseModes))
    : ToolButtonsGroup(pParent, k_group_type_options, k_num_barline_buttons,
                         lmTBG_ONE_SELECTED, pMainSizer,
                         k_id_button_barline, k_tool_barline, pParent->GetColors())
{
}

//---------------------------------------------------------------------------------------
void GrpBarlines2::create_controls_in_group(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group
    set_group_title(_("Barline"));
    wxBoxSizer* pCtrolsSizer = create_main_sizer_for_group(pMainSizer);

    //create the specific controls for this group
    wxBoxSizer* pButtonsSizer = nullptr;
	for (int iB=0; iB < k_num_barline_buttons; iB++)
	{
		if (iB % 5 == 0) {
			pButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
			pCtrolsSizer->Add(pButtonsSizer);
		}

		m_pButton[iB] = new CheckButton(this, k_id_button_barline+iB, wxBitmap(24, 24),
                                        wxDefaultPosition, wxSize(24, 24));
		pButtonsSizer->Add(m_pButton[iB], wxSizerFlags(0).Border(wxALL, 2) );
	}
    set_buttons_bitmaps();
	this->Layout();

	SelectButton(0);	//select barline simple
}

//---------------------------------------------------------------------------------------
EBarline GrpBarlines2::get_selected_barline()
{
    return EBarline(m_nSelButton);
}

//---------------------------------------------------------------------------------------
void GrpBarlines2::set_buttons_bitmaps()
{
    //AWARE: buttons must be in the same order than enum EBarline

    const wxString sNoteBmps[k_num_barline_buttons] = {
        "barline_simple",
        "barline_double",
        "barline_start",
        "barline_end",
        "barline_start_repetition",
        "barline_end_repetition",
        "barline_double_repetition",
    };

    wxSize btSize(24, 24);
    for (int iB=0; iB < k_num_barline_buttons; iB++)
    {
        m_pButton[iB]->SetBitmapUp(sNoteBmps[iB], "", btSize);
        m_pButton[iB]->SetBitmapDown(sNoteBmps[iB], "button_selected_flat", btSize);
        m_pButton[iB]->SetBitmapOver(sNoteBmps[iB], "button_over_flat", btSize);
    }
}

//---------------------------------------------------------------------------------------
bool GrpBarlines2::process_key(wxKeyEvent& event)
{
    //returns true if event is accepted and processed

    if (event.GetKeyCode() == 'L')   //select barline
    {
        if (event.CmdDown())
        {
            SelectNextButton();    // Ctrl + 'L' increment barline button
            return true;
        }
        else if (event.AltDown())
        {
            SelectPrevButton();    // Alt + 'L' decrement barline button
            return true;
        }
    }
	return false;
}

//---------------------------------------------------------------------------------------
void GrpBarlines2::update_tools_info(ToolsInfo* pInfo)
{
    pInfo->barlineType = get_selected_barline();
}

//---------------------------------------------------------------------------------------
void GrpBarlines2::synchronize_with_cursor(bool fEnable, DocCursor* WXUNUSED(pCursor))
{
    //TODO
    EnableGroup(fEnable);
}

//---------------------------------------------------------------------------------------
void GrpBarlines2::synchronize_with_selection(bool fEnable, SelectionSet* pSelection)
{
    //enable toolbox options depending on current selected objects
    if (fEnable && !pSelection->empty())
    {
        //find common values for all selected notes, if any.
        //This is necessary for highlighting the duration
        bool fNoteFound = false;
        int  nDuration;
        ColStaffObjs* pCollection = pSelection->get_staffobjs_collection();
        if (pCollection)
        {
            ColStaffObjsIterator it;
            for (it = pCollection->begin(); it != pCollection->end(); ++it)
            {
                ImoObj* pImo = (*it)->imo_object();
                if (pImo->is_note_rest())
                {
                    ImoNoteRest* pNR = static_cast<ImoNoteRest*>(pImo);
                    int nThisDuration = (int)pNR->get_note_type() - 1;
                    if (!fNoteFound)
                    {
                        fNoteFound = true;
                        nDuration = nThisDuration;
                    }
                    else
                    {
                        if (nDuration != nThisDuration)
                            nDuration = -1;
                    }
                }
            }

            //if any note found, proceed to sync. the toolbox buttons for
            //note type
            if (fNoteFound)
                SelectButton( nDuration );
        }
    }
}



//wxBEGIN_EVENT_TABLE(GrpBarlines2, ToolGroup)
//    EVT_COMBOBOX    (k_id_barlines_list, GrpBarlines2::OnBarlinesList)
//wxEND_EVENT_TABLE()
//
////static BarlinesDBEntry m_tBarlinesDB[k_max_barline+1];
//
//
////---------------------------------------------------------------------------------------
//GrpBarlines2::GrpBarlines2(ToolPage* pParent, wxBoxSizer* pMainSizer,
//                             int nValidMouseModes)
//        : ToolGroup(pParent, k_group_type_tool_selector, pParent->GetColors(),
//                      nValidMouseModes)
//{
//    //To avoid having to translate again barline names, we are going to load them
//    //by using global function get_barline_name()
//    int i;
//    for (i = 0; i < k_max_barline; i++)
//    {
//        m_tBarlinesDB[i].nBarlineType = (EBarline)i;
//        m_tBarlinesDB[i].sBarlineName = get_barline_name((EBarline)i);
//    }
//    //End of table item
//    m_tBarlinesDB[i].nBarlineType = k_barline_unknown;
//    m_tBarlinesDB[i].sBarlineName = "";
//}
//
////---------------------------------------------------------------------------------------
//void GrpBarlines2::create_controls_in_group(wxBoxSizer* pMainSizer)
//{
//    //create the common controls for a group
//    set_group_title(_("Barline"));
//    wxBoxSizer* pCtrolsSizer = create_main_sizer_for_group(pMainSizer);
//
//    //bitmap combo box to select the clef
//    m_pBarlinesList = new wxBitmapComboBox();
//    m_pBarlinesList->Create(this, k_id_barlines_list, wxEmptyString, wxDefaultPosition, wxSize(135, 72),
//                       0, nullptr, wxCB_READONLY);
//
//	pCtrolsSizer->Add( m_pBarlinesList, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
//
//	this->Layout();
//
//    //initializations
//    ToolBox* pToolBox = ((ToolPage*)m_pParent)->GetToolBox();
//    ApplicationScope& appScope = pToolBox->get_app_scope();
//	load_barlines_bitmap_combobox(appScope, m_pBarlinesList, m_tBarlinesDB);
//	select_barline_in_bitmap_combobox(m_pBarlinesList, k_barline_simple);
//}
//
////---------------------------------------------------------------------------------------
//void GrpBarlines2::OnBarlinesList(wxCommandEvent& event)
//{
//    //Notify owner page about the tool change
//    WXUNUSED(event);
//
//    ((ToolPage*)m_pParent)->on_tool_changed(get_selected_tool_id(), get_group_id());
//}
//
////---------------------------------------------------------------------------------------
//EBarline GrpBarlines2::GetSelectedBarline()
//{
//	int iB = m_pBarlinesList->GetSelection();
//    return m_tBarlinesDB[iB].nBarlineType;
//}
//
////---------------------------------------------------------------------------------------
//EToolID GrpBarlines2::get_selected_tool_id()
//{
//    return k_tool_barline;
//}
//
////---------------------------------------------------------------------------------------
//void GrpBarlines2::update_tools_info(ToolsInfo* pInfo)
//{
//    pInfo->barlineType = GetSelectedBarline();
//}
//
////---------------------------------------------------------------------------------------
//void GrpBarlines2::synchronize_with_cursor(bool fEnable, DocCursor* pCursor)
//{
//    //TODO
//    EnableGroup(true);
//}
//
////---------------------------------------------------------------------------------------
//void GrpBarlines2::synchronize_with_selection(bool fEnable,
//                                                  SelectionSet* pSelection)
//{
//    //TODO
//    EnableGroup(true);
//}

}   //namespace lenmus
