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
#pragma implementation "ToolNotes.h"
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
#include "ToolNotes.h"
#include "ToolGroup.h"
#include "../ArtProvider.h"        // to use ArtProvider for managing icons
#include "../../widgets/Button.h"
#include "../TheApp.h"              //to use GetMainFrame()
#include "../MainFrame.h"           //to get active lmScoreCanvas


#define lmSPACING 5

//event IDs
enum {
	lmID_BT_NoteDuration = 2600,
    lmID_BT_NoteAcc = lmID_BT_NoteDuration + lmGrpNoteDuration::lm_NUM_ACC_BUTTONS,
    lmID_BT_NoteDots = lmID_BT_NoteAcc + lmGrpNoteDots::lm_NUM_DOT_BUTTONS,
};


lmToolNotes::lmToolNotes(wxWindow* parent)
	: lmToolPage(parent)
{
    wxBoxSizer *pMainSizer = GetMainSizer();

    //create groups  --------------------------------------

    //notes duration group
    m_pGrpNoteDuration = new lmGrpNoteDuration(this, pMainSizer);

	//Note accidentals group
    m_pGrpNoteAcc = new lmGrpNoteAcc(this, pMainSizer);

    //Note dots group
    m_pGrpNoteDots = new lmGrpNoteDots(this, pMainSizer);


        
    ////Palette group --------------------------------------------
	//lmToolGroup oPaletteGroup(this);
 //   wxBoxSizer* pPaletteSizer = oPaletteGroup.CreateGroup(pMainSizer, _("Palette"));
	//pPaletteSizer->Add( new wxColourPickerCtrl(this, wxID_ANY, GetColors()->PrettyDark()),
	//					0, wxGROW|wxLEFT|wxRIGHT, lmSPACING);
	//pPaletteSizer->Add( new wxColourPickerCtrl(this, wxID_ANY, GetColors()->Dark()),
	//					0, wxGROW|wxLEFT|wxRIGHT, lmSPACING);
	//pPaletteSizer->Add( new wxColourPickerCtrl(this, wxID_ANY, GetColors()->LightDark()),
	//					0, wxGROW|wxLEFT|wxRIGHT, lmSPACING);
	//pPaletteSizer->Add( new wxColourPickerCtrl(this, wxID_ANY, GetColors()->Normal()),
	//					0, wxGROW|wxLEFT|wxRIGHT, lmSPACING);
	//pPaletteSizer->Add( new wxColourPickerCtrl(this, wxID_ANY, GetColors()->LightBright()),
	//					0, wxGROW|wxLEFT|wxRIGHT, lmSPACING);
	//pPaletteSizer->Add( new wxColourPickerCtrl(this, wxID_ANY, GetColors()->Bright()),
	//					0, wxGROW|wxLEFT|wxRIGHT, lmSPACING);
	//pPaletteSizer->Add( new wxColourPickerCtrl(this, wxID_ANY, GetColors()->PrettyBright()),
	//					0, wxGROW|wxLEFT|wxRIGHT, lmSPACING);

	//End of groups

	CreateLayout();
}

lmToolNotes::~lmToolNotes()
{
    delete m_pGrpNoteDuration;
}

lmENoteHeads lmToolNotes::GetNoteheadType()
{
    return enh_Quarter; //(lmENoteHeads)m_pCboNotehead->GetSelection();
}


//--------------------------------------------------------------------------------
// lmGrpNoteDuration implementation
//--------------------------------------------------------------------------------


BEGIN_EVENT_TABLE(lmGrpNoteDuration, lmToolGroup)
	EVT_COMMAND_RANGE (lmID_BT_NoteDuration, lmID_BT_NoteDuration+lm_NUM_ACC_BUTTONS-1, wxEVT_COMMAND_BUTTON_CLICKED, lmGrpNoteDuration::OnButton)
END_EVENT_TABLE()

lmGrpNoteDuration::lmGrpNoteDuration(lmToolPage* pParent, wxBoxSizer* pMainSizer)
        : lmToolGroup(pParent)
{
	m_nSelButton = -1;	            //none selected
    CreateControls(pMainSizer);
}

void lmGrpNoteDuration::CreateControls(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group
    wxBoxSizer* pCtrolsSizer = CreateGroup(pMainSizer, _("Note duration"));

    //create the specific controls for this group
    const wxString sButtonBmps[lm_NUM_ACC_BUTTONS] = {
        _T("note_0"),
        _T("note_1"),
        _T("note_2"),
        _T("note_4"),
        _T("note_8"),
        _T("note_16"),
        _T("note_32"),
        _T("note_64"),
        _T("note_128"),
        _T("note_256"),
    };

    wxBoxSizer* pButtonsSizer;
    wxSize btSize(24, 24);
	for (int iB=0; iB < lm_NUM_ACC_BUTTONS; iB++)
	{
		if (iB % 5 == 0) {
			pButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
			pCtrolsSizer->Add(pButtonsSizer);
		}

		m_pButton[iB] = new lmCheckButton(this, lmID_BT_NoteDuration+iB, wxBitmap(24, 24));
        m_pButton[iB]->SetBitmapUp(sButtonBmps[iB] + _T("_off"), _T(""), btSize);
        m_pButton[iB]->SetBitmapDown(sButtonBmps[iB] + _T("_off"), _T("button_selected_flat"), btSize);
        m_pButton[iB]->SetBitmapOver(sButtonBmps[iB] + _T("_off"), _T("button_over_flat"), btSize);
		pButtonsSizer->Add(m_pButton[iB], wxSizerFlags(0).Border(wxALL, 2) );
	}
	this->Layout();

	SelectButton(3);	//select quarter note
}

lmENoteType lmGrpNoteDuration::GetNoteDuration()
{
    return (lmENoteType)(m_nSelButton+1);
}

void lmGrpNoteDuration::OnButton(wxCommandEvent& event)
{
	SelectButton(event.GetId() - lmID_BT_NoteDuration);
}

void lmGrpNoteDuration::SelectButton(int iB)
{
    // Set selected button as 'pressed' and all others as 'released'
	m_nSelButton = iB;
	for(int i=0; i < lm_NUM_ACC_BUTTONS; i++)
	{
		if (i != iB)
			m_pButton[i]->Release();
		else
			m_pButton[i]->Press();
	}

    //return focus to active view
    GetMainFrame()->SetFocusOnActiveView();
}




//--------------------------------------------------------------------------------
// lmGrpNoteAcc implementation
//--------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(lmGrpNoteAcc, lmToolButtonsGroup)
	EVT_COMMAND_RANGE (lmID_BT_NoteAcc, lmID_BT_NoteAcc+lm_NUM_ACC_BUTTONS-1, wxEVT_COMMAND_BUTTON_CLICKED, lmGrpNoteAcc::OnButton)
END_EVENT_TABLE()

lmGrpNoteAcc::lmGrpNoteAcc(lmToolPage* pParent, wxBoxSizer* pMainSizer)
        : lmToolButtonsGroup(pParent, lm_NUM_ACC_BUTTONS, true, pMainSizer)
{
    CreateControls(pMainSizer);
}

void lmGrpNoteAcc::CreateControls(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group
    wxBoxSizer* pCtrolsSizer = CreateGroup(pMainSizer, _("Note accidentals"));

    //create the specific controls for this group
    const wxString sButtonBmps[lm_NUM_ACC_BUTTONS] = {
	    _T("acc_natural"),
	    _T("acc_flat"),
	    _T("acc_sharp"),
	    _T("acc_flat_flat"),
	    _T("acc_double_sharp"),
	    _T("acc_sharp_sharp"),
	    _T("acc_natural_flat"),
	    _T("acc_natural_sharp"),
    };

    wxBoxSizer* pButtonsSizer;
    wxSize btSize(24, 24);
	for (int iB=0; iB < lm_NUM_ACC_BUTTONS; iB++)
	{
		if (iB % 5 == 0) {
			pButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
			pCtrolsSizer->Add(pButtonsSizer);
		}
		m_pButton[iB] = new lmCheckButton(this, lmID_BT_NoteAcc+iB, wxBitmap(24,24));
        m_pButton[iB]->SetBitmapUp(sButtonBmps[iB], _T(""), btSize);
        m_pButton[iB]->SetBitmapDown(sButtonBmps[iB], _T("button_selected_flat"), btSize);
        m_pButton[iB]->SetBitmapOver(sButtonBmps[iB], _T("button_over_flat"), btSize);
		pButtonsSizer->Add(m_pButton[iB], wxSizerFlags(0).Border(wxALL, 2) );
	}
	this->Layout();

	SelectButton(-1);	//select none
}

lmEAccidentals lmGrpNoteAcc::GetNoteAcc()
{
    return (lmEAccidentals)(m_nSelButton+1);
}

void lmGrpNoteAcc::OnButton(wxCommandEvent& event)
{
	SelectButton(event.GetId() - lmID_BT_NoteAcc);
}




//--------------------------------------------------------------------------------
// lmGrpNoteDots implementation
//--------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(lmGrpNoteDots, lmToolButtonsGroup)
	EVT_COMMAND_RANGE (lmID_BT_NoteDots, lmID_BT_NoteDots+lm_NUM_DOT_BUTTONS-1, wxEVT_COMMAND_BUTTON_CLICKED, lmGrpNoteDots::OnButton)
END_EVENT_TABLE()

lmGrpNoteDots::lmGrpNoteDots(lmToolPage* pParent, wxBoxSizer* pMainSizer)
        : lmToolButtonsGroup(pParent, lm_NUM_DOT_BUTTONS, true, pMainSizer)
{
    CreateControls(pMainSizer);
}

void lmGrpNoteDots::CreateControls(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group
    wxBoxSizer* pCtrolsSizer = CreateGroup(pMainSizer, _("Note dots"));

    //create the specific controls for this group
    const wxString sButtonBmps[lm_NUM_DOT_BUTTONS] = {
	    _T("dot_1"),
	    _T("dot_2"),
	    _T("dot_3"),
    };

    wxBoxSizer* pButtonsSizer;
    wxSize btSize(24, 24);
	for (int iB=0; iB < lm_NUM_DOT_BUTTONS; iB++)
	{
		if (iB % 5 == 0) {
			pButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
			pCtrolsSizer->Add(pButtonsSizer);
		}
		m_pButton[iB] = new lmCheckButton(this, lmID_BT_NoteDots+iB, wxBitmap(24,24));
        m_pButton[iB]->SetBitmapUp(sButtonBmps[iB], _T(""), btSize);
        m_pButton[iB]->SetBitmapDown(sButtonBmps[iB], _T("button_selected_flat"), btSize);
        m_pButton[iB]->SetBitmapOver(sButtonBmps[iB], _T("button_over_flat"), btSize);
		pButtonsSizer->Add(m_pButton[iB], wxSizerFlags(0).Border(wxALL, 2) );
	}
	this->Layout();

	SelectButton(-1);       //select none
}

int lmGrpNoteDots::GetNoteDots()
{
    return m_nSelButton + 1;
}

void lmGrpNoteDots::OnButton(wxCommandEvent& event)
{
	SelectButton(event.GetId() - lmID_BT_NoteDots);
}


