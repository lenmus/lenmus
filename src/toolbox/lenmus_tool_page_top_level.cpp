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
#include "lenmus_tool_page_top_level.h"
#include "lenmus_tool_group.h"
#include "lenmus_button.h"
#include "lenmus_tool_page_symbols.h"

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

#define lm_NUM_HARMONY_BUTTONS  2
#define lm_NUM_SYMBOL_BUTTONS   3

//event IDs
enum {
    lmID_BT_Harmony = 2600,
    lmID_BT_Symbols = lmID_BT_Harmony + lm_NUM_HARMONY_BUTTONS,
};


wxIMPLEMENT_DYNAMIC_CLASS(ToolPageTopLevel, ToolPage);


//---------------------------------------------------------------------------------------
ToolPageTopLevel::ToolPageTopLevel(wxWindow* parent)
{
    Create(parent);
}

//---------------------------------------------------------------------------------------
void ToolPageTopLevel::Create(wxWindow* parent)
{
    //base class
    ToolPage::CreatePage(parent, k_page_symbols);

    //initialize data
    m_sPageToolTip = _("Edit tools to use when cursor at top level");
    m_sPageBitmapName = "tool_symbols";
    m_title = _("Top level tools");
    m_selector = _("Top level tools");

    //create groups
    create_tool_groups();
}

//---------------------------------------------------------------------------------------
void ToolPageTopLevel::create_tool_groups()
{
    wxBoxSizer *pMainSizer = GetMainSizer();

    add_group( LENMUS_NEW GrpTopLevel(this, pMainSizer, k_mouse_mode_data_entry) );

	create_layout();
    select_group(k_grp_Harmony);
}

////---------------------------------------------------------------------------------------
//wxString ToolPageTopLevel::GetToolShortDescription()
//{
//    //returns a short description of the selected tool. This description is used to
//    //be displayed in the status bar
//
//    wxString sDescr;
//    switch( get_selected_tool_id() )
//    {
//        case k_tool_figured_bass:
//            sDescr = _("Add figured bass");
//            break;
//
//        case k_tool_fb_line:
//            sDescr = _("Add 'hold chord' figured bass line");
//            break;
//
//        case k_tool_text:
//            sDescr = _("Add text");
//            break;
//
//        case k_tool_lines:
//            sDescr = _("Add line or arrow");
//            break;
//
//        case k_tool_textbox:
//            sDescr = _("Add textbox");
//            break;
//
//        default:
//            sDescr = "";
//    }
//    return sDescr;
//}



//--------------------------------------------------------------------------------
// GrpTopLevel implementation
//--------------------------------------------------------------------------------

GrpTopLevel::GrpTopLevel(ToolPage* pParent, wxBoxSizer* pMainSizer,
                           int nValidMouseModes)
    : ToolButtonsGroup(pParent, k_group_type_tool_selector, lm_NUM_SYMBOL_BUTTONS,
                         lmTBG_ONE_SELECTED, pMainSizer,
                         lmID_BT_Symbols, k_tool_text, pParent->GetColors(),
                         nValidMouseModes)
{
}

//---------------------------------------------------------------------------------------
void GrpTopLevel::create_controls_in_group(wxBoxSizer* pMainSizer)
{
    //create the buttons for the group

    ToolButtonData cButtons[] =
    {
        { k_tool_text,          _("Text"),                  "symbols_text" },
        { k_tool_lines,         _("Lines and arrows"),      "symbols_line" },
        { k_tool_textbox,       _("Text boxes"),            "symbols_textbox" },
    };

    int nNumButtons = sizeof(cButtons) / sizeof(ToolButtonData);

    set_group_title(_("Text and graphics"));
    wxBoxSizer* pCtrolsSizer = create_main_sizer_for_group(pMainSizer);

    SetFont(wxFont(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Tahoma"));

    wxBoxSizer* pButtonsSizer;
    wxSize btSize(24, 24);
	for (int iB=0; iB < nNumButtons; iB++)
	{
		//if (iB % 9 == 0) {
			pButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
			pCtrolsSizer->Add(pButtonsSizer);
		//}

		wxString sBtName = cButtons[iB].sBitmapName;
		m_pButton[iB] = new CheckButton(this, lmID_BT_Symbols+iB, wxBitmap(24, 24),
                                        wxDefaultPosition, wxSize(24, 24));
        m_pButton[iB]->SetBitmapUp(sBtName, "", btSize);
        m_pButton[iB]->SetBitmapDown(sBtName, "button_selected_flat", btSize);
        m_pButton[iB]->SetBitmapOver(sBtName, "button_over_flat", btSize);
        m_pButton[iB]->SetBitmapDisabled(sBtName + "_dis", "", btSize);
		m_pButton[iB]->SetToolTip(cButtons[iB].sToolTip);
		pButtonsSizer->Add(m_pButton[iB], wxSizerFlags(0).Border(wxALL, 0) );
		pButtonsSizer->Add( new wxStaticText(this, wxID_ANY, cButtons[iB].sToolTip),
                            wxSizerFlags(0).Border(wxLEFT|wxTOP|wxBOTTOM, 5) );
	}

	this->Layout();

	SelectButton(0);	//select text button
}

//---------------------------------------------------------------------------------------
void GrpTopLevel::update_tools_info(ToolsInfo* WXUNUSED(pInfo))
{
    //TODO: define ToolsInfo options and update them
}

//---------------------------------------------------------------------------------------
void GrpTopLevel::synchronize_with_cursor(bool fEnable, DocCursor* WXUNUSED(pCursor))
{
    //TODO
    EnableGroup(fEnable);
}

//---------------------------------------------------------------------------------------
void GrpTopLevel::synchronize_with_selection(bool fEnable, SelectionSet* WXUNUSED(pSelection))
{
    //TODO
    EnableGroup(fEnable);
}



}   //namespace lenmus
