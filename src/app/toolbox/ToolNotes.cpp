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


#define lmSPACING 5


// an entry for the buttons data table
typedef struct lmButtonsDataStruct {
    wxString    sBitmap;		// bitmap name
	wxString	sToolTip;		// tool tip
} lmButtonsData;


// Note durations table
static const wxString m_sNotesBitmaps[10] = {
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

enum {
	lmID_NOTE_BUTTON = 2400,
};

BEGIN_EVENT_TABLE(lmToolNotes, lmToolPage)
	EVT_COMMAND_RANGE (lmID_NOTE_BUTTON, lmID_NOTE_BUTTON+9, wxEVT_COMMAND_BUTTON_CLICKED, lmToolNotes::OnNoteButton)
END_EVENT_TABLE()


lmToolNotes::lmToolNotes(wxWindow* parent)
	: lmToolPage(parent)
{

	m_nNoteDuration = -1;	//none selected

	//main sizer
    wxBoxSizer *pMainSizer = GetMainSizer();

    //notes duration group --------------------------------------
	lmToolGroup oDurationGroup(this);
    wxBoxSizer* pGroupSizer = oDurationGroup.CreateGroup(pMainSizer, _("Note duration"));
    wxBoxSizer* pButtonsSizer;
	for (int iB=0; iB < 10; iB++)
	{
		if (iB % 5 == 0) {
			pButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
			pGroupSizer->Add(pButtonsSizer);
		}
		m_pBtDurations[iB] =
				new lmCheckButton(this, lmID_NOTE_BUTTON+iB,
				wxArtProvider::GetBitmap(m_sNotesBitmaps[iB] + _T("_off"), wxART_TOOLBAR, wxSize(24, 24)) );
		pButtonsSizer->Add(m_pBtDurations[iB], wxSizerFlags(0).Border(wxALL, 2) );
		m_pBtDurations[iB]->SetBitmapSelected( wxArtProvider::GetBitmap(m_sNotesBitmaps[iB] + _T("_on"), wxART_TOOLBAR, wxSize(24, 24)) );
		m_pBtDurations[iB]->SetBorderOver(lm_eBorderOver);
	}
	SelectNoteButton(3);	//select quarter note


	//Notehead type group ------------------------------------------
	lmToolGroup oNoteheadGroup(this);
    wxBoxSizer* pNHSizer = oNoteheadGroup.CreateGroup(pMainSizer, _("Notehead type"));
	int nPanelWidth = oNoteheadGroup.GetGroupWitdh();
    m_pCboNotehead = new wxBitmapComboBox(this, wxID_ANY, _T(""), wxDefaultPosition,
										  wxSize(nPanelWidth - 2 * lmSPACING, 24),
										  0, NULL, wxCB_READONLY );
    pNHSizer->Add(m_pCboNotehead, 0, wxGROW|wxLEFT|wxRIGHT, lmSPACING);

	//populate combo boxand select 'normal' notehead
	//AWARE: Must be loaded following lmENoteHeads enum
	m_pCboNotehead->Append(_T("normal"), wxArtProvider::GetBitmap(_T("tool_notes"), wxART_TOOLBAR, wxSize(24,24) ));
	m_pCboNotehead->Append(_T("cross"), wxArtProvider::GetBitmap(_T("tool_clefs"), wxART_TOOLBAR, wxSize(24,24) ));
	m_pCboNotehead->Select(0);

	//Accidentals group --------------------------------------------
	lmToolGroup oAccidentalsGroup(this);
    wxBoxSizer* pAccSizer = oAccidentalsGroup.CreateGroup(pMainSizer, _("Accidentals"));
    m_pCboAccidentals = new wxBitmapComboBox(this, wxID_ANY, _T(""), wxDefaultPosition,
										  wxSize(nPanelWidth - 2 * lmSPACING, 24),
										  0, NULL, wxCB_READONLY );
    pAccSizer->Add(m_pCboAccidentals, 0, wxGROW|wxLEFT|wxRIGHT, lmSPACING);

	//populate combo box for note durations and select 'normal' note
	//AWARE: Must be loaded following lmEAccidentals enum
	m_pCboAccidentals->Append(_T("none"), wxArtProvider::GetBitmap(_T("none"), wxART_TOOLBAR, wxSize(24,24) ));
	m_pCboAccidentals->Append(_T("natural"), wxArtProvider::GetBitmap(_T("acc_natural"), wxART_TOOLBAR, wxSize(24,24) ));
	m_pCboAccidentals->Append(_T("flat"), wxArtProvider::GetBitmap(_T("acc_flat"), wxART_TOOLBAR, wxSize(24,24) ));
	m_pCboAccidentals->Append(_T("sharp"), wxArtProvider::GetBitmap(_T("acc_sharp"), wxART_TOOLBAR, wxSize(24,24) ));
	m_pCboAccidentals->Append(_T("flat-flat"), wxArtProvider::GetBitmap(_T("acc_flat_flat"), wxART_TOOLBAR, wxSize(24,24) ));
	m_pCboAccidentals->Append(_T("double-sharp"), wxArtProvider::GetBitmap(_T("acc_double_sharp"), wxART_TOOLBAR, wxSize(24,24) ));
	m_pCboAccidentals->Append(_T("sharp-sharp"), wxArtProvider::GetBitmap(_T("acc_sharp_sharp"), wxART_TOOLBAR, wxSize(24,24) ));
	m_pCboAccidentals->Append(_T("natural-flat"), wxArtProvider::GetBitmap(_T("acc_natural_flat"), wxART_TOOLBAR, wxSize(24,24) ));
	m_pCboAccidentals->Append(_T("natural-sharp"), wxArtProvider::GetBitmap(_T("acc_natural_sharp"), wxART_TOOLBAR, wxSize(24,24) ));
	m_pCboAccidentals->Select(0);


    pAccSizer->Add(new wxStaticText(this, wxID_STATIC, _("'-'  Down a flat"),
									 wxDefaultPosition, wxDefaultSize, 0),
									 0, wxGROW|wxLEFT|wxRIGHT|wxTOP, lmSPACING);
    pAccSizer->Add(new wxStaticText(this, wxID_STATIC, _("'+'  Up a sharp"),
									 wxDefaultPosition, wxDefaultSize, 0),
									 0, wxGROW|wxLEFT|wxRIGHT|wxTOP, lmSPACING);
    pAccSizer->Add(new wxStaticText(this, wxID_STATIC, _("'='  Remove accidentals"),
									 wxDefaultPosition, wxDefaultSize, 0),
									 0, wxGROW|wxLEFT|wxRIGHT|wxTOP, lmSPACING);

	//Palette group --------------------------------------------
	lmToolGroup oPaletteGroup(this);
    wxBoxSizer* pPaletteSizer = oPaletteGroup.CreateGroup(pMainSizer, _("Palette"));
	pPaletteSizer->Add( new wxColourPickerCtrl(this, wxID_ANY, GetColors()->PrettyDark()),
						0, wxGROW|wxLEFT|wxRIGHT, lmSPACING);
	pPaletteSizer->Add( new wxColourPickerCtrl(this, wxID_ANY, GetColors()->Dark()),
						0, wxGROW|wxLEFT|wxRIGHT, lmSPACING);
	pPaletteSizer->Add( new wxColourPickerCtrl(this, wxID_ANY, GetColors()->LightDark()),
						0, wxGROW|wxLEFT|wxRIGHT, lmSPACING);
	pPaletteSizer->Add( new wxColourPickerCtrl(this, wxID_ANY, GetColors()->Normal()),
						0, wxGROW|wxLEFT|wxRIGHT, lmSPACING);
	pPaletteSizer->Add( new wxColourPickerCtrl(this, wxID_ANY, GetColors()->LightBright()),
						0, wxGROW|wxLEFT|wxRIGHT, lmSPACING);
	pPaletteSizer->Add( new wxColourPickerCtrl(this, wxID_ANY, GetColors()->Bright()),
						0, wxGROW|wxLEFT|wxRIGHT, lmSPACING);
	pPaletteSizer->Add( new wxColourPickerCtrl(this, wxID_ANY, GetColors()->PrettyBright()),
						0, wxGROW|wxLEFT|wxRIGHT, lmSPACING);

	//End of groups

	CreateLayout();
}


void lmToolNotes::OnNoteButton(wxCommandEvent& event)
{
	SelectNoteButton(event.GetId() - lmID_NOTE_BUTTON);
}

void lmToolNotes::SelectNoteButton(int iB)
{
	// Set selected button as 'pressed' and all others as 'released'
	m_nNoteDuration = iB;
	for(int i=0; i < 10; i++)
	{
		if (i != iB)
			m_pBtDurations[i]->Release();
		else
			m_pBtDurations[i]->Press();
	}
}

lmToolNotes::~lmToolNotes()
{
}

lmENoteType lmToolNotes::GetNoteDuration()
{
    return (lmENoteType)(m_nNoteDuration+1);
}

lmENoteHeads lmToolNotes::GetNoteheadType()
{
    return (lmENoteHeads)m_pCboNotehead->GetSelection();
}

lmEAccidentals lmToolNotes::GetNoteAccidentals()
{
    return (lmEAccidentals)m_pCboAccidentals->GetSelection();
}

