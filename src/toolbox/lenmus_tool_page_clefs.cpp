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
#include "lenmus_tool_page_clefs.h"

#include "lenmus_tool_box.h"
#include "lenmus_tool_group.h"
#include "lenmus_tool_box_events.h"
#include "lenmus_button.h"
#include "lenmus_utilities.h"

//lomse
#include <lomse_score_utilities.h>
using namespace lomse;

//wxWidgets
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/xrc/xmlres.h>
#include <wx/bmpcbox.h>
#include <wx/statline.h>
#include <wx/clrpicker.h>


namespace lenmus
{


#define lmSPACING 5

//event IDs
#define k_num_time_buttons  12

enum {
	k_id_clef_list = 2600,

    // Time signature group
    k_id_button_time,

    // Key signature group
    k_id_key_major = k_id_button_time + k_num_time_buttons,
    k_id_key_minor,
    k_id_key_list,
};


//=======================================================================================
// ToolPageClefs implementation
//=======================================================================================
wxIMPLEMENT_DYNAMIC_CLASS(ToolPageClefs, ToolPage);


//---------------------------------------------------------------------------------------
ToolPageClefs::ToolPageClefs(wxWindow* parent)
{
	Create(parent);
}

//---------------------------------------------------------------------------------------
void ToolPageClefs::Create(wxWindow* parent)
{
    ToolPage::CreatePage(parent, k_page_clefs);

    //initialize data
    m_sPageToolTip = _("Edit tools for clefs, keys and time signatures");
    m_title = _("Clef, key, time");
    m_selector = _("Clefs, keys and time signatures\tF2");
    m_sPageBitmapName = "tool_clefs";

    create_tool_groups();
}

//---------------------------------------------------------------------------------------
void ToolPageClefs::create_tool_groups()
{
    wxBoxSizer *pMainSizer = GetMainSizer();

//    m_pGrpClefType = new GrpClefType(this, pMainSizer, k_mouse_mode_data_entry);
//    m_pGrpTimeType = new GrpTimeType(this, pMainSizer, k_mouse_mode_data_entry);
//    m_pGrpKeyType = new GrpKeyType(this, pMainSizer, k_mouse_mode_data_entry);
//    AddGroup(m_pGrpClefType);
//    AddGroup(m_pGrpKeyType);
//    AddGroup(m_pGrpTimeType);

    add_group( LENMUS_NEW GrpClefType(this, pMainSizer, k_mouse_mode_data_entry) );
    add_group( LENMUS_NEW GrpTimeType(this, pMainSizer, k_mouse_mode_data_entry) );
    add_group( LENMUS_NEW GrpKeyType(this, pMainSizer, k_mouse_mode_data_entry) );

	create_layout();
	select_group(k_grp_ClefType);

//    //Select clef group
//    SelectGroup(m_pGrpClefType);
//
//    //initialize info about selected group/tool
//    m_nCurGroupID = k_grp_ClefType;
//    m_nCurToolID = m_pGrpClefType->get_selected_tool_id();
}

////---------------------------------------------------------------------------------------
//wxString ToolPageClefs::GetToolShortDescription()
//{
//    //returns a short description of the selected tool. This description is used to
//    //be displayed in the status bar
//
//    wxString sDescr;
//    switch( GetCurrentGroupID() )
//    {
//        case k_grp_ClefType:
//            sDescr = _("Add clef");
//            break;
//
//        case k_grp_TimeType:
//            sDescr = _("Add time signature");
//            break;
//
//        case k_grp_KeyType:
//            sDescr = _("Add key signature");
//            break;
//
//        default:
//            sDescr = "";
//    }
//    return sDescr;
//}



//--------------------------------------------------------------------------------
// GrpClefType implementation
//--------------------------------------------------------------------------------

//aux. class to contain clefs data
class ClefData
{
public:
    ClefData() {}
    ClefData(EClef type, wxString name)
        : nClefType(type), sClefName(name) {}

    EClef		nClefType;
    wxString    sClefName;
};

enum {
    lm_eNUM_CLEFS = 8,
};

static ClefData m_tClefs[lm_eNUM_CLEFS];
static wxString m_language = "??";


//---------------------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(GrpClefType, ToolGroup)
    EVT_COMBOBOX    (k_id_clef_list, GrpClefType::OnClefList)
wxEND_EVENT_TABLE()

//---------------------------------------------------------------------------------------
GrpClefType::GrpClefType(ToolPage* pParent, wxBoxSizer* WXUNUSED(pMainSizer),
                             int nValidMouseModes)
        : ToolGroup(pParent, k_group_type_tool_selector, pParent->GetColors(),
                      nValidMouseModes)
{
    //load language dependent strings. Can not be statically initiallized because
    //then they do not get translated
    if (m_language != ApplicationScope::get_language())
    {
        //AWARE: When addign more clefs, update lm_eNUM_CLEFS;
        m_tClefs[0] = ClefData(k_clef_G2, _("G clef on 2nd line"));
        m_tClefs[1] = ClefData(k_clef_F4, _("F clef on 4th line"));
        m_tClefs[2] = ClefData(k_clef_F3, _("F clef on 3rd line"));
        m_tClefs[3] = ClefData(k_clef_C1, _("C clef on 1st line"));
        m_tClefs[4] = ClefData(k_clef_C2, _("C clef on 2nd line"));
        m_tClefs[5] = ClefData(k_clef_C3, _("C clef on 3rd line"));
        m_tClefs[6] = ClefData(k_clef_C4, _("C clef on 4th line"));
        m_tClefs[7] = ClefData(k_clef_percussion, _("Percussion clef"));
        //// other clefs not yet available
//        k_clef_C5,
//        k_clef_F5,
//        k_clef_G1,
//        k_clef_8_G2,        //8 above
//        k_clef_G2_8,        //8 below
//        k_clef_8_F4,        //8 above
//        k_clef_F4_8,        //8 below
//        k_clef_15_G2,       //15 above
//        k_clef_G2_15,       //15 below
//        k_clef_15_F4,       //15 above
//        k_clef_F4_15,       //15 below
        m_language = ApplicationScope::get_language();
    }
}

//---------------------------------------------------------------------------------------
void GrpClefType::create_controls_in_group(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group
    set_group_title(_("Clef"));
    wxBoxSizer* pCtrolsSizer = create_main_sizer_for_group(pMainSizer);

    //bitmap combo box to select the clef
    m_pClefList = new wxBitmapComboBox();
    m_pClefList->Create(this, k_id_clef_list, wxEmptyString, wxDefaultPosition,
                        wxSize(135, 72), 0, nullptr, wxCB_READONLY);
	pCtrolsSizer->Add( m_pClefList, 0, wxALL, 5 );

    LoadClefList();
	this->Layout();
}

//---------------------------------------------------------------------------------------
void GrpClefType::OnClefList(wxCommandEvent& WXUNUSED(event))
{
    //Notify owner page about the tool change
    WXUNUSED(event);

    ((ToolPage*)m_pParent)->on_tool_changed(k_tool_clef, get_group_id());
}

//---------------------------------------------------------------------------------------
void GrpClefType::LoadClefList()
{
    ToolBox* pToolBox = ((ToolPage*)m_pParent)->GetToolBox();
    ApplicationScope& appScope = pToolBox->get_app_scope();
    m_pClefList->Clear();
    for (int i=0; i < lm_eNUM_CLEFS; i++)
    {
        m_pClefList->Append(wxEmptyString,
                            generate_bitmap_for_clef_ctrol(appScope,
                                                           m_tClefs[i].sClefName,
                                                           m_tClefs[i].nClefType) );
    }
    m_pClefList->SetSelection(0);
}

//---------------------------------------------------------------------------------------
void GrpClefType::update_tools_info(ToolsInfo* pInfo)
{
    pInfo->clefType = GetSelectedClef();
}

//---------------------------------------------------------------------------------------
void GrpClefType::synchronize_with_cursor(bool WXUNUSED(fEnable),
                                          DocCursor* WXUNUSED(pCursor))
{
    //TODO
    EnableGroup(true);
}

//---------------------------------------------------------------------------------------
void GrpClefType::synchronize_with_selection(bool WXUNUSED(fEnable),
                                             SelectionSet* WXUNUSED(pSelection))
{
    //TODO
    EnableGroup(true);
}


//--------------------------------------------------------------------------------
// GrpTimeType implementation
//--------------------------------------------------------------------------------

//buttons data
typedef struct lmTimeButtonStruct
{
    wxString    sBitmap;
    int         nBeats;
	int     	nBeatType;

} lmTimeButton;

static const lmTimeButton m_tButtons[] = {
    { "time_2_2", 2, 2 },
    { "time_2_4", 2, 4 },
    { "time_2_8", 2, 8 },
    { "time_3_2", 3, 2 },
    { "time_3_4", 3, 4 },
    { "time_3_8", 3, 8 },
    { "time_4_2", 4, 2 },
    { "time_4_4", 4, 4 },
    { "time_4_8", 4, 8 },
    { "time_6_8", 6, 8 },
    { "time_9_8", 9, 8 },
    { "time_12_8", 12, 8 },
};

//---------------------------------------------------------------------------------------
GrpTimeType::GrpTimeType(ToolPage* pParent, wxBoxSizer* pMainSizer,
                             int nValidMouseModes)
    : ToolButtonsGroup(pParent, k_group_type_tool_selector, k_num_time_buttons,
                         lmTBG_ONE_SELECTED, pMainSizer,
                         k_id_button_time, k_tool_time_signature, pParent->GetColors(),
                         nValidMouseModes)
{
}

//---------------------------------------------------------------------------------------
void GrpTimeType::create_controls_in_group(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group

    set_group_title(_("Time signature"));
    wxBoxSizer* pCtrolsSizer = create_main_sizer_for_group(pMainSizer);

    //create the specific controls for this group
    wxBoxSizer* pButtonsSizer = nullptr;
    wxSize btSize(24, 24);
	for (int iB=0; iB < k_num_time_buttons; iB++)
	{
		if (iB % 6 == 0) {
			pButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
			pCtrolsSizer->Add(pButtonsSizer);
		}
		wxString sBtName = m_tButtons[iB].sBitmap;
		m_pButton[iB] = new CheckButton(this, k_id_button_time+iB, wxBitmap(24, 24),
                                        wxDefaultPosition, wxSize(24, 24));
        m_pButton[iB]->SetBitmapUp(sBtName, "", btSize);
        m_pButton[iB]->SetBitmapDown(sBtName, "button_selected_flat", btSize);
        m_pButton[iB]->SetBitmapOver(sBtName, "button_over_flat", btSize);
        m_pButton[iB]->SetBitmapDisabled(sBtName + "_dis", "", btSize);
        wxString sTip = wxString::Format(_("Select time signature %d/%d"),
            m_tButtons[iB].nBeats, m_tButtons[iB].nBeatType);
		m_pButton[iB]->SetToolTip(sTip);
		pButtonsSizer->Add(m_pButton[iB], wxSizerFlags(0).Border(wxTOP|wxBOTTOM, 2) );
	}
	this->Layout();

	SelectButton(0);
}

//---------------------------------------------------------------------------------------
int GrpTimeType::GetTimeBeats()
{
    //Returns 0 if no button selected

    if (m_nSelButton != -1)
        return m_tButtons[m_nSelButton].nBeats;
    else
        return 0;
}

//---------------------------------------------------------------------------------------
int GrpTimeType::GetTimeBeatType()
{
    //Returns 0 if no button selected

    if (m_nSelButton != -1)
        return m_tButtons[m_nSelButton].nBeatType;
    else
        return 0;
}

//---------------------------------------------------------------------------------------
void GrpTimeType::update_tools_info(ToolsInfo* pInfo)
{
    pInfo->timeNumBeats = GetTimeBeats();
    pInfo->timeBeatType = GetTimeBeatType();
}

//---------------------------------------------------------------------------------------
void GrpTimeType::synchronize_with_cursor(bool WXUNUSED(fEnable),
                                          DocCursor* WXUNUSED(pCursor))
{
    //TODO
    EnableGroup(true);
}

//---------------------------------------------------------------------------------------
void GrpTimeType::synchronize_with_selection(bool WXUNUSED(fEnable),
                                             SelectionSet* WXUNUSED(pSelection))
{
    //TODO
    EnableGroup(true);
}


//--------------------------------------------------------------------------------
// GrpKeyType implementation
//--------------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(GrpKeyType, ToolGroup)
    EVT_RADIOBUTTON (k_id_key_major, GrpKeyType::OnKeyType)
    EVT_RADIOBUTTON (k_id_key_minor, GrpKeyType::OnKeyType)
    EVT_COMBOBOX    (k_id_key_list, GrpKeyType::OnKeyList)
wxEND_EVENT_TABLE()

#define k_num_minor_keys    k_max_minor_key - k_min_minor_key + 1
#define k_num_major_keys    k_max_major_key - k_min_major_key + 1

static GrpKeyType::lmKeysData m_tMajorKeys[k_num_major_keys];
static GrpKeyType::lmKeysData m_tMinorKeys[k_num_minor_keys];


//---------------------------------------------------------------------------------------
GrpKeyType::GrpKeyType(ToolPage* pParent, wxBoxSizer* WXUNUSED(pMainSizer),
                           int nValidMouseModes)
        : ToolGroup(pParent, k_group_type_tool_selector, pParent->GetColors(),
                      nValidMouseModes)
{
    //To avoid having to translate again key signature names, we are going to load them
    //by using global function get_key_signature_name()
    for (int j=0, i = k_min_major_key; i <= k_max_major_key; i++, j++)
    {
        m_tMajorKeys[j].nKeyType = (EKeySignature)i;
        m_tMajorKeys[j].sKeyName = get_key_signature_name((EKeySignature)i);
        m_tMajorKeys[j].nFifths = KeyUtilities::key_signature_to_num_fifths(i);
    }

    for (int j=0, i = k_min_minor_key; i <= k_max_minor_key; i++, j++)
    {
        m_tMinorKeys[j].nKeyType = (EKeySignature)i;
        m_tMinorKeys[j].sKeyName = get_key_signature_name((EKeySignature)i);    //wxString::Format("%s (%d%s)",;
        m_tMinorKeys[j].nFifths = KeyUtilities::key_signature_to_num_fifths(i);
    }
}

//---------------------------------------------------------------------------------------
void GrpKeyType::create_controls_in_group(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group
    set_group_title(_("Key signature"));
    wxBoxSizer* pCtrolsSizer = create_main_sizer_for_group(pMainSizer);

    //create the specific controls for this group

    //radio buttons for major/minor key signature selction
    wxBoxSizer* pRadioSizer = new wxBoxSizer( wxHORIZONTAL );
    m_pKeyRad[0] = new wxRadioButton(this, k_id_key_major, _("Major") );
    m_pKeyRad[1] = new wxRadioButton(this, k_id_key_minor, _("Minor") );
	pRadioSizer->Add( m_pKeyRad[0], 0, wxALL, 5 );
	pRadioSizer->Add( m_pKeyRad[1], 0, wxALL, 5 );
	pCtrolsSizer->Add( pRadioSizer, 1, wxEXPAND, 5 );

    //bitmap combo box to select the key signature
    m_pKeyList = new wxBitmapComboBox();
    m_pKeyList->Create(this, k_id_key_list, wxEmptyString, wxDefaultPosition, wxSize(135, 72),
                       0, nullptr, wxCB_READONLY);

	pCtrolsSizer->Add( m_pKeyList, 0, wxALL, 5 );

    //load initial data
    m_pKeyRad[0]->SetValue(true);
    m_pKeyRad[1]->SetValue(false);
    LoadKeyList(0);

	this->Layout();
}

//---------------------------------------------------------------------------------------
void GrpKeyType::OnKeyType(wxCommandEvent& WXUNUSED(event))
{
    //load list box with the appropiate keys for selected key type

    LoadKeyList( m_pKeyRad[1]->GetValue() );
//
//    //return focus to active view
//    GetMainFrame()->SetFocusOnActiveView();
}

//---------------------------------------------------------------------------------------
void GrpKeyType::OnKeyList(wxCommandEvent& WXUNUSED(event))
{
    //An item has been selected in keys combo. Notify owner page about the tool change

    WXUNUSED(event);
    NotifyToolChange();
}

//---------------------------------------------------------------------------------------
void GrpKeyType::NotifyToolChange()
{
    //Notify owner page about the tool change

    ((ToolPage*)m_pParent)->on_tool_changed(k_tool_key_signature, get_group_id());
}

//---------------------------------------------------------------------------------------
bool GrpKeyType::IsMajorKeySignature()
{
    return m_pKeyRad[0]->GetValue();
}

//---------------------------------------------------------------------------------------
int GrpKeyType::GetFifths()
{
    int iK = m_pKeyList->GetSelection();
    if (m_pKeyRad[0]->GetValue())       //if is major key signature
        return m_tMajorKeys[iK].nFifths;
    else
        return m_tMinorKeys[iK].nFifths;
}

//---------------------------------------------------------------------------------------
void GrpKeyType::LoadKeyList(int nType)
{
    //nType: 0=major, 1=minor

    ToolBox* pToolBox = ((ToolPage*)m_pParent)->GetToolBox();
    ApplicationScope& appScope = pToolBox->get_app_scope();
    if (nType==0)
    {
        m_pKeyList->Clear();
        for (int i=0; i < k_num_major_keys; i++)
        {
            m_pKeyList->Append(wxEmptyString,
                               generate_bitmap_for_key_ctrol(appScope,
                                                             m_tMajorKeys[i].sKeyName,
                                                             m_tMajorKeys[i].nKeyType) );
        }
    }
    else
    {
        m_pKeyList->Clear();
        for (int i=0; i < k_num_minor_keys; i++)
        {
            m_pKeyList->Append(wxEmptyString,
                               generate_bitmap_for_key_ctrol(appScope,
                                                             m_tMinorKeys[i].sKeyName,
                                                             m_tMinorKeys[i].nKeyType) );
        }
    }
    m_pKeyList->SetSelection(0);
    NotifyToolChange();
}

//---------------------------------------------------------------------------------------
void GrpKeyType::update_tools_info(ToolsInfo* pInfo)
{
    int iK = m_pKeyList->GetSelection();
    if (m_pKeyRad[0]->GetValue())   //if is major key signature
        pInfo->keyType = m_tMajorKeys[iK].nKeyType;
    else
        pInfo->keyType = m_tMinorKeys[iK].nKeyType;
}

//---------------------------------------------------------------------------------------
void GrpKeyType::synchronize_with_cursor(bool WXUNUSED(fEnable), DocCursor* WXUNUSED(pCursor))
{
    //TODO
    EnableGroup(true);
}

//---------------------------------------------------------------------------------------
void GrpKeyType::synchronize_with_selection(bool WXUNUSED(fEnable),
                                            SelectionSet* WXUNUSED(pSelection))
{
    //TODO
    EnableGroup(true);
}



}   //namespace lenmus
