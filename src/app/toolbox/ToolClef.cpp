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
#pragma implementation "ToolClef.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/xrc/xmlres.h"
#include "wx/bmpcbox.h"
#include "wx/statline.h"
#include "wx/clrpicker.h"

#include "ToolsBox.h"
#include "ToolClef.h"
#include "ToolGroup.h"
#include "../ArtProvider.h"         //to use ArtProvider for managing icons
#include "../TheApp.h"              //to use GetMainFrame()
#include "../MainFrame.h"           //to get active lmScoreCanvas
#include "../ScoreCanvas.h"         //to send commands
#include "../../widgets/Button.h"
#include "../../score/defs.h"
#include "../../score/KeySignature.h"
#include "../../graphic/GraphicManager.h"   //to use GenerateBitmapForKeyCtrol()



#define lmSPACING 5

//event IDs
enum {
    lmID_CLEF_LIST = 2600,
    lmID_CLEF_ADD,

    // Time signature group
    lmID_BT_TimeType = lmID_CLEF_ADD + 1,

    // Key signature group
    lmID_KEY_TYPE = lmID_BT_TimeType + lmGrpTimeType::lm_NUM_BUTTONS,
    lmID_KEY_LIST = lmID_KEY_TYPE + 2,
    lmID_KEY_ADD,
};



IMPLEMENT_DYNAMIC_CLASS(lmToolPageClefs, lmToolPage)


lmToolPageClefs::lmToolPageClefs()
{
}

lmToolPageClefs::lmToolPageClefs(wxWindow* parent)
{
	Create(parent);
}

void lmToolPageClefs::Create(wxWindow* parent)
{
    //base class
    lmToolPage::Create(parent);

    //initialize data
    m_sPageToolTip = _("Edit tools for clefs, keys and time signatures");
    m_sPageBitmapName = _T("tool_clefs");

    //create groups
    CreateGroups();
}

lmToolPageClefs::~lmToolPageClefs()
{
    delete m_pGrpClefType;
    delete m_pGrpTimeType;
    delete m_pGrpKeyType;
}

lmToolGroup* lmToolPageClefs::GetToolGroup(lmEToolGroupID nGroupID)
{
    switch(nGroupID)
    {
        case lmGRP_ClefType:    return m_pGrpClefType;
        case lmGRP_TimeType:    return m_pGrpTimeType;
        case lmGRP_KeyType:     return m_pGrpKeyType;
        default:
            wxASSERT(false);
    }
    return (lmToolGroup*)NULL;      //compiler happy
}

void lmToolPageClefs::CreateGroups()
{
    //Create the groups for this page

    wxBoxSizer *pMainSizer = GetMainSizer();

    m_pGrpClefType = new lmGrpClefType(this, pMainSizer);
    m_pGrpTimeType = new lmGrpTimeType(this, pMainSizer);
    m_pGrpKeyType = new lmGrpKeyType(this, pMainSizer);
    
	CreateLayout();
}



//--------------------------------------------------------------------------------
// lmGrpClefType implementation
//--------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(lmGrpClefType, lmToolGroup)
    EVT_BUTTON      (lmID_CLEF_ADD, lmGrpClefType::OnAddClef)
END_EVENT_TABLE()

enum {
    lm_eNUM_CLEFS = 8,
};

static lmClefData m_tClefs[lm_eNUM_CLEFS];
static bool m_fStringsInitialized = false;

lmGrpClefType::lmGrpClefType(lmToolPage* pParent, wxBoxSizer* pMainSizer)
        : lmToolGroup(pParent, pParent->GetColors())
{
    //load language dependent strings. Can not be statically initiallized because
    //then they do not get translated
    if (!m_fStringsInitialized)
    {
        //AWARE: When addign more clefs, update lm_eNUM_CLEFS;
        m_tClefs[0] = lmClefData( _("G clef on 2nd line"), lmE_Sol );
        m_tClefs[1] = lmClefData( _("F clef on 4th line"), lmE_Fa4 );
        m_tClefs[2] = lmClefData( _("F clef on 3rd line"), lmE_Fa3 );
        m_tClefs[3] = lmClefData( _("C clef on 1st line"), lmE_Do1 );
        m_tClefs[4] = lmClefData( _("C clef on 2nd line"), lmE_Do2 );
        m_tClefs[5] = lmClefData( _("C clef on 3rd line"), lmE_Do3 );
        m_tClefs[6] = lmClefData( _("C clef on 4th line"), lmE_Do4 );
        m_tClefs[7] = lmClefData( _("Percussion clef"), lmE_Percussion );
        //
        m_fStringsInitialized = true;
    }

    CreateControls(pMainSizer);
}

void lmGrpClefType::CreateControls(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group
    wxBoxSizer* pCtrolsSizer = CreateGroup(pMainSizer, _("Add clef"));

    //bitmap combo box to select the clef
    m_pClefList = new wxBitmapComboBox();
    m_pClefList->Create(this, lmID_CLEF_LIST, wxEmptyString, wxDefaultPosition, wxSize(135, 72),
                       0, NULL, wxCB_READONLY);

	pCtrolsSizer->Add( m_pClefList, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

    //button to add the clef
    m_pBtnAddClef = new wxButton(this, lmID_CLEF_ADD, _("Add clef"), wxDefaultPosition, wxDefaultSize, 0 );
	pCtrolsSizer->Add( m_pBtnAddClef, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

    LoadClefList();
	this->Layout();
}

void lmGrpClefType::OnAddClef(wxCommandEvent& event)
{
    //insert selected key
	WXUNUSED(event);
	int iC = m_pClefList->GetSelection();
    lmController* pSC = GetMainFrame()->GetActiveController();
    if (pSC)
    {
        pSC->InsertClef(m_tClefs[iC].nClefType);

        //return focus to active view
        GetMainFrame()->SetFocusOnActiveView();
    }
}

void lmGrpClefType::LoadClefList()
{
    m_pClefList->Clear();
    for (int i=0; i < lm_eNUM_CLEFS; i++)
    {
        m_pClefList->Append(wxEmptyString, 
                            GenerateBitmapForClefCtrol(m_tClefs[i].sClefName,
                                                       m_tClefs[i].nClefType) );
    }
    m_pClefList->SetSelection(0);
}




//--------------------------------------------------------------------------------
// lmGrpTimeType implementation
//--------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(lmGrpTimeType, lmToolGroup)
	EVT_COMMAND_RANGE (lmID_BT_TimeType, lmID_BT_TimeType+lm_NUM_BUTTONS-1, wxEVT_COMMAND_BUTTON_CLICKED, lmGrpTimeType::OnButton)
END_EVENT_TABLE()

static const lmGrpTimeType::lmButton m_tButtons[] = {
    { _T("time_2_2"), 2, 2 },
    { _T("time_2_4"), 2, 4 },
    { _T("time_6_8"), 6, 8 },
    { _T("time_2_8"), 2, 8 },
    { _T("time_3_2"), 3, 2 },
    { _T("time_3_4"), 3, 4 },
    { _T("time_9_8"), 9, 8 },
    { _T("time_3_8"), 3, 8 },
    { _T("time_4_2"), 4, 2 },
    { _T("time_4_4"), 4, 4 },
    { _T("time_12_8"), 12, 8 },
    { _T("time_4_8"), 4, 8 },
};

lmGrpTimeType::lmGrpTimeType(lmToolPage* pParent, wxBoxSizer* pMainSizer)
        : lmToolGroup(pParent, pParent->GetColors())
{
    wxASSERT(sizeof(m_tButtons) / sizeof(lmButton) == lm_NUM_BUTTONS);
    CreateControls(pMainSizer);
}

void lmGrpTimeType::CreateControls(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group
    wxBoxSizer* pCtrolsSizer = CreateGroup(pMainSizer, _("Add a time signature"));

    //create the specific controls for this group
    wxBoxSizer* pButtonsSizer;
	for (int iB=0; iB < lm_NUM_BUTTONS; iB++)
	{
		if (iB % 4 == 0) {
			pButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
			pCtrolsSizer->Add(pButtonsSizer);
		}
		m_pButton[iB] =
            new wxBitmapButton(this, lmID_BT_TimeType+iB,
				wxArtProvider::GetBitmap(m_tButtons[iB].sBitmap, wxART_TOOLBAR, wxSize(24, 24)),
                wxDefaultPosition, wxSize(24, 24) );
		pButtonsSizer->Add(m_pButton[iB], wxSizerFlags(0).Border(wxALL, 2) );
	}
	this->Layout();
}

void lmGrpTimeType::OnButton(wxCommandEvent& event)
{
	int iB = event.GetId() - lmID_BT_TimeType;
    lmController* pSC = GetMainFrame()->GetActiveController();
    if (pSC)
        pSC->InsertTimeSignature(m_tButtons[iB].nBeats, m_tButtons[iB].nBeatType);
}





//--------------------------------------------------------------------------------
// lmGrpKeyType implementation
//--------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(lmGrpKeyType, lmToolGroup)
    EVT_RADIOBUTTON (lmID_KEY_TYPE, lmGrpKeyType::OnKeyType)
    EVT_RADIOBUTTON (lmID_KEY_TYPE+1, lmGrpKeyType::OnKeyType)
    EVT_COMBOBOX    (lmID_KEY_LIST, lmGrpKeyType::OnKeyList)
    EVT_BUTTON      (lmID_KEY_ADD, lmGrpKeyType::OnAddKey)
END_EVENT_TABLE()

#define lmMAX_MINOR_KEYS    lmMAX_MINOR_KEY - lmMIN_MINOR_KEY + 1
#define lmMAX_MAJOR_KEYS    lmMAX_MAJOR_KEY - lmMIN_MAJOR_KEY + 1

static lmGrpKeyType::lmKeysData m_tMajorKeys[lmMAX_MAJOR_KEYS];
static lmGrpKeyType::lmKeysData m_tMinorKeys[lmMAX_MINOR_KEYS];


lmGrpKeyType::lmGrpKeyType(lmToolPage* pParent, wxBoxSizer* pMainSizer)
        : lmToolGroup(pParent, pParent->GetColors())
{
    //To avoid having to translate again key signature names, we are going to load them
    //by using global function GetKeySignatureName()
    for (int j=0, i = lmMIN_MAJOR_KEY; i <= lmMAX_MAJOR_KEY; i++, j++)
    {
        m_tMajorKeys[j].nKeyType = (lmEKeySignatures)i;
        m_tMajorKeys[j].sKeyName = GetKeySignatureName((lmEKeySignatures)i);    //wxString::Format(_T("%s (%d%s)"),;
        m_tMajorKeys[j].nFifths = KeySignatureToNumFifths((lmEKeySignatures)i);
    }

    for (int j=0, i = lmMIN_MINOR_KEY; i <= lmMAX_MINOR_KEY; i++, j++)
    {
        m_tMinorKeys[j].nKeyType = (lmEKeySignatures)i;
        m_tMinorKeys[j].sKeyName = GetKeySignatureName((lmEKeySignatures)i);    //wxString::Format(_T("%s (%d%s)"),;
        m_tMinorKeys[j].nFifths = KeySignatureToNumFifths((lmEKeySignatures)i);
    }

    CreateControls(pMainSizer);
}

void lmGrpKeyType::CreateControls(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group
    wxBoxSizer* pCtrolsSizer = CreateGroup(pMainSizer, _("Add a key signature"));

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

    //button to add the key
    m_pBtnAddKey = new wxButton(this, lmID_KEY_ADD, _("Add key"), wxDefaultPosition, wxDefaultSize, 0 );
	pCtrolsSizer->Add( m_pBtnAddKey, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
    //load initial data
    m_pKeyRad[0]->SetValue(true);
    m_pKeyRad[1]->SetValue(false);
    LoadKeyList(0);

	this->Layout();
}

void lmGrpKeyType::OnKeyType(wxCommandEvent& event)
{
    //load list box with the appropiate keys for selected key type

    LoadKeyList(event.GetSelection());

    //return focus to active view
    GetMainFrame()->SetFocusOnActiveView();
}

void lmGrpKeyType::OnKeyList(wxCommandEvent& event)
{
    WXUNUSED(event);
}

void lmGrpKeyType::OnAddKey(wxCommandEvent& event)
{
    //insert selected key
	WXUNUSED(event);
	int iK = m_pKeyList->GetSelection();
    bool fMajor = m_pKeyRad[0]->GetValue();
    int nFifths = 0;
    if (fMajor)
        nFifths = m_tMajorKeys[iK].nFifths;
    else
        nFifths = m_tMinorKeys[iK].nFifths;

    lmController* pSC = GetMainFrame()->GetActiveController();
    if (pSC)
    {
        pSC->InsertKeySignature(nFifths, fMajor);

        //return focus to active view
        GetMainFrame()->SetFocusOnActiveView();
    }
}

void lmGrpKeyType::LoadKeyList(int nType)
{
    //nType: 0=major, 1=minor

    if (nType==0)
    {
        m_pKeyList->Clear();
        for (int i=0; i < lmMAX_MAJOR_KEYS; i++)
        {
            m_pKeyList->Append(wxEmptyString, 
                               GenerateBitmapForKeyCtrol(m_tMajorKeys[i].sKeyName,
                                                         m_tMajorKeys[i].nKeyType) );
        }
    }
    else
    {
        m_pKeyList->Clear();
        for (int i=0; i < lmMAX_MINOR_KEYS; i++)
        {
            m_pKeyList->Append(wxEmptyString,
                               GenerateBitmapForKeyCtrol(m_tMinorKeys[i].sKeyName,
                                                         m_tMinorKeys[i].nKeyType) );
        }
    }
    m_pKeyList->SetSelection(0);
}

