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


#define lmSPACING 5

//event IDs
enum {
	lmID_BT_ClefType = 2600,
    lmID_BT_TimeType = lmID_BT_ClefType + lmGrpClefType::lm_NUM_BUTTONS,
};



lmToolClef::lmToolClef(wxWindow* parent)
	: lmToolPage(parent)
{
	wxBoxSizer* pMainSizer = GetMainSizer();

    //create groups
    m_pGrpClefType = new lmGrpClefType(this, pMainSizer);
    m_pGrpTimeType = new lmGrpTimeType(this, pMainSizer);
    
	CreateLayout();
}


lmToolClef::~lmToolClef()
{
    delete m_pGrpClefType;
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
	for (int iB=0; iB < lm_NUM_BUTTONS; iB++)
	{
		if (iB % 5 == 0) {
			pButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
			pCtrolsSizer->Add(pButtonsSizer);
		}
		m_pButton[iB] =
				new lmCheckButton(this, lmID_BT_ClefType+iB,
				wxArtProvider::GetBitmap(sButtonBmps[iB] + _T("_off"), wxART_TOOLBAR, wxSize(24, 24)) );
		pButtonsSizer->Add(m_pButton[iB], wxSizerFlags(0).Border(wxALL, 2) );
		m_pButton[iB]->SetBitmapSelected( wxArtProvider::GetBitmap(sButtonBmps[iB] + _T("_on"), wxART_TOOLBAR, wxSize(24, 24)) );
		m_pButton[iB]->SetBorderOver(lm_eBorderOver);
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
	WXUNUSED(event);
	int iB = event.GetId() - lmID_BT_TimeType;
    lmController* pSC = GetMainFrame()->GetActiveController();
    pSC->InsertTimeSignature(m_tButtons[iB].nBeats, m_tButtons[iB].nBeatType);
}



