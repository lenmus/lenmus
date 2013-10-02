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
#endif


namespace lenmus
{


#define lmSPACING 5

//event IDs
const long k_id_barlines_list = wxNewId();



IMPLEMENT_DYNAMIC_CLASS(ToolPageBarlines, ToolPage)


//---------------------------------------------------------------------------------------
ToolPageBarlines::ToolPageBarlines()
{
}

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
    m_sPageBitmapName = _T("tool_barlines");

    //create groups
    CreateGroups();
}

//---------------------------------------------------------------------------------------
ToolPageBarlines::~ToolPageBarlines()
{
}

//---------------------------------------------------------------------------------------
void ToolPageBarlines::CreateGroups()
{
    //Create the groups for this page

    wxBoxSizer *pMainSizer = GetMainSizer();

    m_pGrpBarlines = new GrpBarlines(this, pMainSizer, k_mouse_mode_data_entry);
    AddGroup(m_pGrpBarlines);

	CreateLayout();

    //initialize info about selected group/tool
    m_nCurGroupID = k_grp_BarlineType;
    m_nCurToolID = m_pGrpBarlines->GetCurrentToolID();

    m_fGroupsCreated = true;
}

//---------------------------------------------------------------------------------------
wxString ToolPageBarlines::GetToolShortDescription()
{
    //returns a short description of the selected tool. This description is used to
    //be displayed in the status bar

    return _("Add barline");
}

//---------------------------------------------------------------------------------------
void ToolPageBarlines::synchronize_with_cursor(bool fEnable, DocCursor* pCursor)
{
    m_pGrpBarlines->EnableGroup(fEnable);
}

//---------------------------------------------------------------------------------------
void ToolPageBarlines::synchronize_with_selection(bool fEnable,
                                                  SelectionSet* pSelection)
{
    m_pGrpBarlines->EnableGroup(fEnable);
}


//--------------------------------------------------------------------------------
// GrpBarlines implementation
//--------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(GrpBarlines, ToolGroup)
    EVT_COMBOBOX    (k_id_barlines_list, GrpBarlines::OnBarlinesList)
END_EVENT_TABLE()

static BarlinesDBEntry m_tBarlinesDB[k_max_barline+1];


//---------------------------------------------------------------------------------------
GrpBarlines::GrpBarlines(ToolPage* pParent, wxBoxSizer* pMainSizer,
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
    m_tBarlinesDB[i].sBarlineName = _T("");
}

//---------------------------------------------------------------------------------------
void GrpBarlines::CreateGroupControls(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group
    SetGroupTitle(_("Barline"));
    wxBoxSizer* pCtrolsSizer = CreateGroupSizer(pMainSizer);

    //bitmap combo box to select the clef
    m_pBarlinesList = new wxBitmapComboBox();
    m_pBarlinesList->Create(this, k_id_barlines_list, wxEmptyString, wxDefaultPosition, wxSize(135, 72),
                       0, NULL, wxCB_READONLY);

	pCtrolsSizer->Add( m_pBarlinesList, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	this->Layout();

    //initializations
    ToolBox* pToolBox = ((ToolPage*)m_pParent)->GetToolBox();
    ApplicationScope& appScope = pToolBox->get_app_scope();
	load_barlines_bitmap_combobox(appScope, m_pBarlinesList, m_tBarlinesDB);
	select_barline_in_bitmap_combobox(m_pBarlinesList, k_barline_simple);
}

//---------------------------------------------------------------------------------------
void GrpBarlines::OnBarlinesList(wxCommandEvent& event)
{
    //Notify owner page about the tool change
    WXUNUSED(event);

    ((ToolPage*)m_pParent)->OnToolChanged(GetToolGroupID(), GetCurrentToolID());
}

//---------------------------------------------------------------------------------------
EBarline GrpBarlines::GetSelectedBarline()
{
	int iB = m_pBarlinesList->GetSelection();
    return m_tBarlinesDB[iB].nBarlineType;
}

//---------------------------------------------------------------------------------------
EToolID GrpBarlines::GetCurrentToolID()
{
    return (EToolID)m_pBarlinesList->GetSelection();
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

}   //namespace lenmus
