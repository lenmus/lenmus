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


#define lmSPACING 5

//event IDs
enum {
	lmID_BT_ClefType = 2600,

    // Time signature group
    lmID_BT_TimeType = lmID_BT_ClefType + lmGrpClefType::lm_NUM_BUTTONS,

    // Key signature group
    lmID_KEY_TYPE = lmID_BT_TimeType + lmGrpTimeType::lm_NUM_BUTTONS,
    lmID_KEY_LIST = lmID_KEY_TYPE + 2,
};



lmToolClef::lmToolClef(wxWindow* parent)
	: lmToolPage(parent)
{
	wxBoxSizer* pMainSizer = GetMainSizer();

    //create groups
    m_pGrpClefType = new lmGrpClefType(this, pMainSizer);
    m_pGrpTimeType = new lmGrpTimeType(this, pMainSizer);
    m_pGrpKeyType = new lmGrpKeyType(this, pMainSizer);
    
	CreateLayout();
}


lmToolClef::~lmToolClef()
{
    delete m_pGrpClefType;
    delete m_pGrpTimeType;
    delete m_pGrpKeyType;
}


//--------------------------------------------------------------------------------
// lmGrpClefType implementation
//--------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(lmGrpClefType, lmToolGroup)
	EVT_COMMAND_RANGE (lmID_BT_ClefType, lmID_BT_ClefType+lm_NUM_BUTTONS-1, wxEVT_COMMAND_BUTTON_CLICKED, lmGrpClefType::OnButton)
END_EVENT_TABLE()

lmGrpClefType::lmGrpClefType(lmToolPage* pParent, wxBoxSizer* pMainSizer)
        : lmToolGroup(pParent)
{
	m_nSelButton = -1;	//none selected
    CreateControls(pMainSizer);
}

void lmGrpClefType::CreateControls(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group
    wxBoxSizer* pCtrolsSizer = CreateGroup(pMainSizer, _("Clef type"));

    //create the specific controls for this group
    const wxString sButtonBmps[lm_NUM_BUTTONS] = {
        _T("clef_g"),
        _T("clef_f"),
        _T("clef_c"),
    };

    wxBoxSizer* pButtonsSizer;
    wxSize btSize(24, 24);
	for (int iB=0; iB < lm_NUM_BUTTONS; iB++)
	{
		if (iB % 5 == 0) {
			pButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
			pCtrolsSizer->Add(pButtonsSizer);
		}

		m_pButton[iB] = new lmCheckButton(this, lmID_BT_ClefType+iB, wxBitmap(24,24));
        m_pButton[iB]->SetBitmapUp(sButtonBmps[iB], _T(""), btSize);
        m_pButton[iB]->SetBitmapDown(sButtonBmps[iB], _T("button_selected_flat"), btSize);
        m_pButton[iB]->SetBitmapOver(sButtonBmps[iB], _T("button_over_flat"), btSize);
		pButtonsSizer->Add(m_pButton[iB], wxSizerFlags(0).Border(wxALL, 2) );
	}
	this->Layout();

	SelectClef(0);	//select G clef
}

lmEClefType lmGrpClefType::GetClefType()
{
    return (lmEClefType)(m_nSelButton+1);
}

void lmGrpClefType::OnButton(wxCommandEvent& event)
{
	SelectClef(event.GetId() - lmID_BT_ClefType);
}

void lmGrpClefType::SelectClef(int iB)
{
    // Set selected button as 'pressed' and all others as 'released'
	m_nSelButton = iB;
	for(int i=0; i < lm_NUM_BUTTONS; i++)
	{
		if (i != iB)
			m_pButton[i]->Release();
		else
			m_pButton[i]->Press();
	}
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
        : lmToolGroup(pParent)
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
    pSC->InsertTimeSignature(m_tButtons[iB].nBeats, m_tButtons[iB].nBeatType);
}





//--------------------------------------------------------------------------------
// lmGrpKeyType implementation
//--------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(lmGrpKeyType, lmToolGroup)
    EVT_RADIOBUTTON (lmID_KEY_TYPE, lmGrpKeyType::OnKeyType)
    EVT_RADIOBUTTON (lmID_KEY_TYPE+1, lmGrpKeyType::OnKeyType)
    EVT_COMBOBOX    (lmID_KEY_LIST, lmGrpKeyType::OnKeyList)
END_EVENT_TABLE()

#define lmMAX_MINOR_KEYS    lmMAX_MINOR_KEY - lmMIN_MINOR_KEY + 1
#define lmMAX_MAJOR_KEYS    lmMAX_MAJOR_KEY - lmMIN_MAJOR_KEY + 1

static lmGrpKeyType::lmKeysData m_tMajorKeys[lmMAX_MAJOR_KEYS];
static lmGrpKeyType::lmKeysData m_tMinorKeys[lmMAX_MINOR_KEYS];


lmGrpKeyType::lmGrpKeyType(lmToolPage* pParent, wxBoxSizer* pMainSizer)
        : lmToolGroup(pParent)
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
    pSC->InsertKeySignature(nFifths, fMajor);

    //return focus to active view
    GetMainFrame()->SetFocusOnActiveView();
}

void lmGrpKeyType::LoadKeyList(int nType)
{
    //nType: 0=major, 1=minor

    if (nType==0)
    {
        m_pKeyList->Clear();
        for (int i=0; i < lmMAX_MAJOR_KEYS; i++)
        {
            wxString sKeyName = m_tMajorKeys[i].sKeyName;
            m_pKeyList->Append(wxEmptyString, GenerateBitmap(sKeyName));
        }
    }
    else
    {
        m_pKeyList->Clear();
        for (int i=0; i < lmMAX_MINOR_KEYS; i++)
        {
            wxString sKeyName = m_tMinorKeys[i].sKeyName;
            m_pKeyList->Append(wxEmptyString, GenerateBitmap(sKeyName));
        }
    }
    m_pKeyList->SetSelection(0);
}

wxBitmap lmGrpKeyType::GenerateBitmap(wxString sKeyName)
{
    wxMemoryDC dc;
    wxSize size(108, 64);
	wxBitmap bmp(size.x, size.y);

    //fill bitmap in white
    dc.SelectObject(bmp);
    dc.SetBrush(*wxWHITE_BRUSH);
	dc.SetBackground(*wxWHITE_BRUSH);
	dc.Clear();

    //draw rectangle and two red diagonals
    dc.SetPen(*wxBLACK);
    dc.DrawRectangle(0, 0, size.x, size.y);
    dc.SetPen(*wxRED);
    dc.DrawLine(0, 0, size.x, size.y);
    dc.DrawLine(0, size.y, size.x, 0);

    //write key signature name in black
    int h, w;
    dc.SetPen(*wxBLACK);
    dc.SetFont(*wxNORMAL_FONT);
    dc.GetTextExtent(sKeyName, &w, &h);
    dc.DrawText(sKeyName, (size.x-w)/2, (size.y-h)/2);

    //clean up and return new bitmap
    dc.SelectObject(wxNullBitmap);

    return bmp;
}
