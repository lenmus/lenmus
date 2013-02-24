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
#include "lenmus_tool_page_clefs.h"
#include "lenmus_tool_group.h"
#include "lenmus_tool_box_events.h"
#include "lenmus_button.h"
#include "lenmus_utilities.h"
//#include "../ArtProvider.h"         //to use ArtProvider for managing icons
//#include "../TheApp.h"              //to use GetMainFrame()
//#include "../MainFrame.h"           //to get active lmScoreCanvas
//#include "../ScoreCanvas.h"         //to send commands
//#include "../../score/defs.h"
//#include "../../score/KeySignature.h"
//#include "../../graphic/GraphicManager.h"   //to use GenerateBitmapForKeyCtrol()

//lomse
#include "lomse_score_utilities.h"
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
#define lm_NUM_CLEF_BUTTONS  8
#define lm_NUM_TIME_BUTTONS  12

enum {
	lmID_BT_ClefType = 2600,
    lmID_CLEF_LIST = lmID_BT_ClefType + lm_NUM_CLEF_BUTTONS,

    // Time signature group
    lmID_BT_TIME = lmID_CLEF_LIST + 1,

    // Key signature group
    lmID_KEY_TYPE = lmID_BT_TIME + lm_NUM_TIME_BUTTONS,
    lmID_KEY_LIST = lmID_KEY_TYPE + 2,
};


//=======================================================================================
// ToolPageClefs implementation
//=======================================================================================
IMPLEMENT_DYNAMIC_CLASS(ToolPageClefs, ToolPage)


//---------------------------------------------------------------------------------------
ToolPageClefs::ToolPageClefs()
{
}

//---------------------------------------------------------------------------------------
ToolPageClefs::ToolPageClefs(wxWindow* parent)
{
	Create(parent);
}

//---------------------------------------------------------------------------------------
void ToolPageClefs::Create(wxWindow* parent)
{
    //base class
    ToolPage::CreatePage(parent, k_page_clefs);

    //initialize data
    m_sPageToolTip = _("Edit tools for clefs, keys and time signatures");
    m_sPageBitmapName = _T("tool_clefs");

    //create groups
    CreateGroups();
}

//---------------------------------------------------------------------------------------
ToolPageClefs::~ToolPageClefs()
{
}

//---------------------------------------------------------------------------------------
void ToolPageClefs::CreateGroups()
{
    //Create the groups for this page

    wxBoxSizer *pMainSizer = GetMainSizer();

    m_pGrpClefType = new GrpClefType(this, pMainSizer, lmMM_DATA_ENTRY);
    m_pGrpTimeType = new GrpTimeType(this, pMainSizer, lmMM_DATA_ENTRY);
    m_pGrpKeyType = new GrpKeyType(this, pMainSizer, lmMM_DATA_ENTRY);
    AddGroup(m_pGrpClefType);
    AddGroup(m_pGrpKeyType);
    AddGroup(m_pGrpTimeType);

	CreateLayout();

    //Select clef group
    SelectGroup(m_pGrpClefType);

    //initialize info about selected group/tool
    m_nCurGroupID = k_grp_ClefType;
    m_nCurToolID = m_pGrpClefType->GetCurrentToolID();

    m_fGroupsCreated = true;
}

//---------------------------------------------------------------------------------------
wxString ToolPageClefs::GetToolShortDescription()
{
    //returns a short description of the selected tool. This description is used to
    //be displayed in the status bar

    wxString sDescr;
    switch( GetCurrentGroupID() )
    {
        case k_grp_ClefType:
            sDescr = _("Add clef");
            break;

        case k_grp_TimeType:
            sDescr = _("Add time signature");
            break;

        case k_grp_KeyType:
            sDescr = _("Add key signature");
            break;

        default:
            sDescr = _T("");
    }
    return sDescr;
}



//--------------------------------------------------------------------------------
// GrpClefType implementation
//--------------------------------------------------------------------------------

//aux. class to contain clefs data
class ClefData
{
public:
    ClefData() {}
    ClefData(EClef type, wxString name, wxString sBmp)
        : nClefType(type), sClefName(name), sButtonBmp(sBmp) {}

    EClef		nClefType;
    wxString		sClefName;
    wxString        sButtonBmp;
};

enum {
    lm_eNUM_CLEFS = 8,
};

static ClefData m_tClefs[lm_eNUM_CLEFS];
static bool m_fStringsInitialized = false;


#if lmUSE_CLEF_COMBO
//---------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(GrpClefType, ToolGroup)
    EVT_COMBOBOX    (lmID_CLEF_LIST, GrpClefType::OnClefList)
END_EVENT_TABLE()

//---------------------------------------------------------------------------------------
GrpClefType::GrpClefType(ToolPage* pParent, wxBoxSizer* pMainSizer,
                             int nValidMouseModes)
        : ToolGroup(pParent, k_group_type_tool_selector, pParent->GetColors(),
                      nValidMouseModes)
{
    //load language dependent strings. Can not be statically initiallized because
    //then they do not get translated
    if (!m_fStringsInitialized)
    {
        //AWARE: When addign more clefs, update lm_eNUM_CLEFS;
        m_tClefs[0] = ClefData(k_clef_G2, _("G clef on 2nd line"), _T("clef_g") );
        m_tClefs[1] = ClefData(k_clef_F4, _("F clef on 4th line"), _T("clef_g") );
        m_tClefs[2] = ClefData(k_clef_F3, _("F clef on 3rd line"), _T("clef_g") );
        m_tClefs[3] = ClefData(k_clef_C1, _("C clef on 1st line"), _T("clef_g") );
        m_tClefs[4] = ClefData(k_clef_C2, _("C clef on 2nd line"), _T("clef_g") );
        m_tClefs[5] = ClefData(k_clef_C3, _("C clef on 3rd line"), _T("clef_g") );
        m_tClefs[6] = ClefData(k_clef_C4, _("C clef on 4th line"), _T("clef_g") );
        m_tClefs[7] = ClefData(k_clef_percussion, _("Percussion clef"), _T("clef_g") );
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
        m_fStringsInitialized = true;
    }
}

//---------------------------------------------------------------------------------------
void GrpClefType::CreateGroupControls(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group
    SetGroupTitle(_("Clef"));
    wxBoxSizer* pCtrolsSizer = CreateGroupSizer(pMainSizer);

    //bitmap combo box to select the clef
    m_pClefList = new wxBitmapComboBox();
    m_pClefList->Create(this, lmID_CLEF_LIST, wxEmptyString, wxDefaultPosition, wxSize(135, 72),
                       0, NULL, wxCB_READONLY);

	pCtrolsSizer->Add( m_pClefList, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

    LoadClefList();
	this->Layout();
}

//---------------------------------------------------------------------------------------
void GrpClefType::OnClefList(wxCommandEvent& event)
{
    //Notify owner page about the tool change
    WXUNUSED(event);

    ((ToolPage*)m_pParent)->OnToolChanged(this->GetToolGroupID(),
                                            (EToolID)m_pClefList->GetSelection() );
}

//---------------------------------------------------------------------------------------
void GrpClefType::LoadClefList()
{
//TODO TB
//    m_pClefList->Clear();
//    for (int i=0; i < lm_eNUM_CLEFS; i++)
//    {
//        m_pClefList->Append(wxEmptyString,
//                            GenerateBitmapForClefCtrol(m_tClefs[i].sClefName,
//                                                       m_tClefs[i].nClefType) );
//    }
//    m_pClefList->SetSelection(0);
}



#else   //USE BUTTONS FOR CLEFS
//---------------------------------------------------------------------------------------

GrpClefType::GrpClefType(ToolPage* pParent, wxBoxSizer* pMainSizer)
        : ToolButtonsGroup(pParent, k_group_type_tool_selector, lm_NUM_CLEF_BUTTONS,
                             lmTBG_ONE_SELECTED, pMainSizer,
                             lmID_BT_ClefType, k_tool_none, pParent->GetColors())
{
    //load language dependent strings. Can not be statically initiallized because
    //then they do not get translated
    if (!m_fStringsInitialized)
    {
        //AWARE: When addign more clefs, update lm_eNUM_CLEFS;
        m_tClefs[0] = ClefData(lmE_Sol, _("G clef on 2nd line"), _T("clef_g") );
        m_tClefs[1] = ClefData(lmE_Fa4, _("F clef on 4th line"), _T("clef_g") );
        m_tClefs[2] = ClefData(lmE_Fa3, _("F clef on 3rd line"), _T("clef_g") );
        m_tClefs[3] = ClefData(lmE_Do1, _("C clef on 1st line"), _T("clef_g") );
        m_tClefs[4] = ClefData(lmE_Do2, _("C clef on 2nd line"), _T("clef_g") );
        m_tClefs[5] = ClefData(lmE_Do3, _("C clef on 3rd line"), _T("clef_g") );
        m_tClefs[6] = ClefData(lmE_Do4, _("C clef on 4th line"), _T("clef_g") );
        m_tClefs[7] = ClefData(lmE_Percussion, _("Percussion clef"), _T("clef_g") );
        //// other clefs not yet available
        //lmE_Do5,
        //lmE_Fa5,
        //lmE_Sol1,
        //lmE_8Sol,       //8 above
        //lmE_Sol8,       //8 below
        //lmE_8Fa,        //8 above
        //lmE_Fa8,        //8 below
        //lmE_15Sol,      //15 above
        //lmE_Sol15,      //15 below
        //lmE_15Fa,       //15 above
        //lmE_Fa15,       //15 below
        m_fStringsInitialized = true;
    }
}

//---------------------------------------------------------------------------------------
void GrpClefType::CreateGroupControls(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group
    SetGroupTitle(_("Clef"));
    wxBoxSizer* pCtrolsSizer = CreateGroupSizer(pMainSizer);

    wxBoxSizer* pButtonsSizer;
    wxSize btSize(32, 48);
	for (int iB=0; iB < lm_NUM_CLEF_BUTTONS; iB++)
	{
		if (iB % 4 == 0) {
			pButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
			pCtrolsSizer->Add(pButtonsSizer);
		}

		m_pButton[iB] = new CheckButton(this, lmID_BT_ClefType+iB, wxBitmap(32, 48));
        m_pButton[iB]->SetBitmapUp(m_tClefs[iB].sButtonBmp, _T(""), btSize);
        m_pButton[iB]->SetBitmapDown(m_tClefs[iB].sButtonBmp, _T("button_selected_flat"), btSize);
        m_pButton[iB]->SetBitmapOver(m_tClefs[iB].sButtonBmp, _T("button_over_flat"), btSize);
        m_pButton[iB]->SetToolTip(m_tClefs[iB].sClefName);
		pButtonsSizer->Add(m_pButton[iB], wxSizerFlags(0).Border(wxALL, 2) );
	}
	this->Layout();

	SelectButton(0);	//select G clef
}

//---------------------------------------------------------------------------------------
EClef GrpClefType::GetSelectedClef()
{
    return (EClef)m_nSelButton;
}

#endif  //lmUSE_CLEF_COMBO



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
    { _T("time_2_2"), 2, 2 },
    { _T("time_2_4"), 2, 4 },
    { _T("time_2_8"), 2, 8 },
    { _T("time_3_2"), 3, 2 },
    { _T("time_3_4"), 3, 4 },
    { _T("time_3_8"), 3, 8 },
    { _T("time_4_2"), 4, 2 },
    { _T("time_4_4"), 4, 4 },
    { _T("time_4_8"), 4, 8 },
    { _T("time_6_8"), 6, 8 },
    { _T("time_9_8"), 9, 8 },
    { _T("time_12_8"), 12, 8 },
};

#if lmUSE_TIME_OLD
//--------------------

BEGIN_EVENT_TABLE(GrpTimeType, ToolGroup)
	EVT_COMMAND_RANGE (lmID_BT_TIME, lmID_BT_TIME+lm_NUM_TIME_BUTTONS-1, wxEVT_COMMAND_BUTTON_CLICKED, GrpTimeType::OnButton)
END_EVENT_TABLE()

//---------------------------------------------------------------------------------------
GrpTimeType::GrpTimeType(ToolPage* pParent, wxBoxSizer* pMainSizer,
                             int nValidMouseModes)
        : ToolGroup(pParent, k_group_type_tool_selector, pParent->GetColors(),
                      nValidMouseModes)
        , m_nSelButton(-1)  //none selected
{
    wxASSERT(sizeof(m_tButtons) / sizeof(lmButton) == lm_NUM_TIME_BUTTONS);
}

//---------------------------------------------------------------------------------------
void GrpTimeType::CreateGroupControls(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group
    SetGroupTitle(_("Time signature"));
    wxBoxSizer* pCtrolsSizer = CreateGroupSizer(pMainSizer);

    //create the specific controls for this group
    wxBoxSizer* pButtonsSizer;
	for (int iB=0; iB < lm_NUM_TIME_BUTTONS; iB++)
	{
		if (iB % 4 == 0) {
			pButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
			pCtrolsSizer->Add(pButtonsSizer);
		}
		m_pButton[iB] =
            new wxBitmapButton(this, lmID_BT_TIME+iB,
				wxArtProvider::GetBitmap(m_tButtons[iB].sBitmap, wxART_TOOLBAR, wxSize(24, 24)),
                wxDefaultPosition, wxSize(24, 24) );
		pButtonsSizer->Add(m_pButton[iB], wxSizerFlags(0).Border(wxALL, 2) );
	}
	this->Layout();
}

//---------------------------------------------------------------------------------------
void GrpTimeType::OnButton(wxCommandEvent& event)
{
    //Notify owner page about the tool change

	m_nSelButton = event.GetId() - lmID_BT_TIME;
    ((ToolPage*)m_pParent)->OnToolChanged(this->GetToolGroupID(), (EToolID)m_nSelButton );
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

//-------------------------------
#else
//-------------------------------

//---------------------------------------------------------------------------------------
GrpTimeType::GrpTimeType(ToolPage* pParent, wxBoxSizer* pMainSizer,
                             int nValidMouseModes)
    : ToolButtonsGroup(pParent, k_group_type_tool_selector, lm_NUM_TIME_BUTTONS,
                         lmTBG_ONE_SELECTED, pMainSizer,
                         lmID_BT_TIME, k_tool_none, pParent->GetColors(),
                         nValidMouseModes)
{
}

//---------------------------------------------------------------------------------------
void GrpTimeType::CreateGroupControls(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group

    SetGroupTitle(_("Time signature"));
    wxBoxSizer* pCtrolsSizer = CreateGroupSizer(pMainSizer);

    //create the specific controls for this group
    wxBoxSizer* pButtonsSizer;
    wxSize btSize(24, 24);
	for (int iB=0; iB < lm_NUM_TIME_BUTTONS; iB++)
	{
		if (iB % 6 == 0) {
			pButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
			pCtrolsSizer->Add(pButtonsSizer);
		}
		wxString sBtName = m_tButtons[iB].sBitmap;
		m_pButton[iB] = new CheckButton(this, lmID_BT_TIME+iB, wxBitmap(24, 24));
        m_pButton[iB]->SetBitmapUp(sBtName, _T(""), btSize);
        m_pButton[iB]->SetBitmapDown(sBtName, _T("button_selected_flat"), btSize);
        m_pButton[iB]->SetBitmapOver(sBtName, _T("button_over_flat"), btSize);
        m_pButton[iB]->SetBitmapDisabled(sBtName + _T("_dis"), _T(""), btSize);
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

#endif


//--------------------------------------------------------------------------------
// GrpKeyType implementation
//--------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(GrpKeyType, ToolGroup)
    EVT_RADIOBUTTON (lmID_KEY_TYPE, GrpKeyType::OnKeyType)
    EVT_RADIOBUTTON (lmID_KEY_TYPE+1, GrpKeyType::OnKeyType)
    EVT_COMBOBOX    (lmID_KEY_LIST, GrpKeyType::OnKeyList)
END_EVENT_TABLE()

#define k_num_minor_keys    k_max_minor_key - k_min_minor_key + 1
#define k_num_major_keys    k_max_major_key - k_min_major_key + 1

static GrpKeyType::lmKeysData m_tMajorKeys[k_num_major_keys];
static GrpKeyType::lmKeysData m_tMinorKeys[k_num_minor_keys];


//---------------------------------------------------------------------------------------
GrpKeyType::GrpKeyType(ToolPage* pParent, wxBoxSizer* pMainSizer,
                           int nValidMouseModes)
        : ToolGroup(pParent, k_group_type_tool_selector, pParent->GetColors(),
                      nValidMouseModes)
{
    //To avoid having to translate again key signature names, we are going to load them
    //by using global function lmGetKeySignatureName()
    for (int j=0, i = k_min_major_key; i <= k_max_major_key; i++, j++)
    {
        m_tMajorKeys[j].nKeyType = (EKeySignature)i;
        m_tMajorKeys[j].sKeyName = get_key_signature_name((EKeySignature)i);    //wxString::Format(_T("%s (%d%s)"),;
        m_tMajorKeys[j].nFifths = lomse::key_signature_to_num_fifths(i);
    }

    for (int j=0, i = k_min_minor_key; i <= k_max_minor_key; i++, j++)
    {
        m_tMinorKeys[j].nKeyType = (EKeySignature)i;
        m_tMinorKeys[j].sKeyName = get_key_signature_name((EKeySignature)i);    //wxString::Format(_T("%s (%d%s)"),;
        m_tMinorKeys[j].nFifths = lomse::key_signature_to_num_fifths(i);
    }
}

//---------------------------------------------------------------------------------------
void GrpKeyType::CreateGroupControls(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group
    SetGroupTitle(_("Key signature"));
    wxBoxSizer* pCtrolsSizer = CreateGroupSizer(pMainSizer);

    //create the specific controls for this group

    //radio buttons for major/minor key signature selction
    wxBoxSizer* pRadioSizer = new wxBoxSizer( wxHORIZONTAL );
    m_pKeyRad[0] = new wxRadioButton(this, lmID_KEY_TYPE, _("Major") );
    m_pKeyRad[1] = new wxRadioButton(this, lmID_KEY_TYPE+1, _("Minor") );
	pRadioSizer->Add( m_pKeyRad[0], 0, wxALL, 5 );
	pRadioSizer->Add( m_pKeyRad[1], 0, wxALL, 5 );
	pCtrolsSizer->Add( pRadioSizer, 1, wxEXPAND, 5 );

    //bitmap combo box to select the key signature
    m_pKeyList = new wxBitmapComboBox();
    m_pKeyList->Create(this, lmID_KEY_LIST, wxEmptyString, wxDefaultPosition, wxSize(135, 72),
                       0, NULL, wxCB_READONLY);

	pCtrolsSizer->Add( m_pKeyList, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

    //load initial data
    m_pKeyRad[0]->SetValue(true);
    m_pKeyRad[1]->SetValue(false);
    LoadKeyList(0);

	this->Layout();
}

//---------------------------------------------------------------------------------------
void GrpKeyType::OnKeyType(wxCommandEvent& event)
{
//TODO TB
//    //load list box with the appropiate keys for selected key type
//
//    LoadKeyList(event.GetSelection());
//
//    //return focus to active view
//    GetMainFrame()->SetFocusOnActiveView();
}

//---------------------------------------------------------------------------------------
void GrpKeyType::OnKeyList(wxCommandEvent& event)
{
    //An item has been selected in keys combo. Notify owner page about the tool change

    WXUNUSED(event);
    NotifyToolChange();
}

//---------------------------------------------------------------------------------------
void GrpKeyType::NotifyToolChange()
{
    //Notify owner page about the tool change

    ((ToolPage*)m_pParent)->OnToolChanged(this->GetToolGroupID(),
                                            (EToolID)m_pKeyList->GetSelection() );
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
//TODO TB
//    //nType: 0=major, 1=minor
//
//    if (nType==0)
//    {
//        m_pKeyList->Clear();
//        for (int i=0; i < k_num_major_keys; i++)
//        {
//            m_pKeyList->Append(wxEmptyString,
//                               GenerateBitmapForKeyCtrol(m_tMajorKeys[i].sKeyName,
//                                                         m_tMajorKeys[i].nKeyType) );
//        }
//    }
//    else
//    {
//        m_pKeyList->Clear();
//        for (int i=0; i < k_num_minor_keys; i++)
//        {
//            m_pKeyList->Append(wxEmptyString,
//                               GenerateBitmapForKeyCtrol(m_tMinorKeys[i].sKeyName,
//                                                         m_tMinorKeys[i].nKeyType) );
//        }
//    }
//    m_pKeyList->SetSelection(0);
//    NotifyToolChange();
}



}   //namespace lenmus
