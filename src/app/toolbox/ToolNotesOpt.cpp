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
#pragma implementation "ToolNotesOpt.h"
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

#include "ToolNotesOpt.h"
#include "../ArtProvider.h"        // to use ArtProvider for managing icons


#define lmSPACING 5
#define lmPANEL_WIDTH 150


lmToolNotesOpt::lmToolNotesOpt(wxWindow* parent)
	: wxPanel(parent, -1, wxDefaultPosition, wxSize(lmPANEL_WIDTH, 300), wxNO_BORDER )
{
	//main sizer
    wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(pMainSizer);

	//Note duration
    pMainSizer->Add(
		new wxStaticText(this, wxID_STATIC, _("Note type"),
						 wxDefaultPosition, wxDefaultSize, 0),
		0, wxGROW|wxLEFT|wxRIGHT|wxTOP, lmSPACING);

    m_pCboDurations = new wxComboBox(this, wxID_ANY, _T(""), wxDefaultPosition, 
									 wxSize(lmPANEL_WIDTH - 2 * lmSPACING, -1),
									 0, (wxString*)NULL, wxCB_READONLY);
    pMainSizer->Add(m_pCboDurations, 0, wxGROW|wxLEFT|wxRIGHT, lmSPACING);

	//populate combo box for note durations and select 'quarter' note
	//AWARE: Must be loaded following lmENoteType enum
    m_pCboDurations->Append(_("Long"));
    m_pCboDurations->Append(_("Double whole"));
	m_pCboDurations->Append(_("Whole"));
	m_pCboDurations->Append(_("Half"));
	m_pCboDurations->Append(_("Quarter"));
	m_pCboDurations->Append(_("Eighth"));
	m_pCboDurations->Append(_("Sixteenth"));
	m_pCboDurations->Append(_("32nd"));
	m_pCboDurations->Append(_("64th"));
	m_pCboDurations->Append(_("128th"));
	m_pCboDurations->Append(_("256th"));
	m_pCboDurations->Select(eQuarter);


	//Notehead type
    pMainSizer->Add(
		new wxStaticText(this, wxID_STATIC, _("Notehead type"),
						 wxDefaultPosition, wxDefaultSize, 0),
		0, wxGROW|wxLEFT|wxRIGHT|wxTOP, lmSPACING);

    wxBitmapComboBox* pCboNoteheads =
		new wxBitmapComboBox(this, wxID_ANY, _T(""), wxDefaultPosition,
				wxSize(lmPANEL_WIDTH - 2 * lmSPACING, 24),
				NULL, wxCB_READONLY );
    pMainSizer->Add(pCboNoteheads, 0, wxGROW|wxLEFT|wxRIGHT, lmSPACING);

	//populate combo box for note durations and select 'normal' note
	pCboNoteheads->Append(_T(""), wxArtProvider::GetBitmap(_T("tool_notes"), wxART_TOOLBAR, wxSize(24,24) ));
	pCboNoteheads->Append(_T(""), wxArtProvider::GetBitmap(_T("tool_clefs"), wxART_TOOLBAR, wxSize(24,24) ));
	pCboNoteheads->Select(0);


	//separation line
	wxStaticLine* pLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition,
										   wxDefaultSize, wxLI_HORIZONTAL );
    pMainSizer->Add(pLine, wxSizerFlags(0).Left().Border(wxGROW|wxTOP|wxBOTTOM, 5));

    pMainSizer->Add(new wxStaticText(this, wxID_STATIC, _("Accidentals"),
									 wxDefaultPosition, wxDefaultSize, 0),
									 0, wxGROW|wxLEFT|wxRIGHT|wxTOP, lmSPACING);
    pMainSizer->Add(new wxStaticText(this, wxID_STATIC, _("'-'  Down a flat"),
									 wxDefaultPosition, wxDefaultSize, 0),
									 0, wxGROW|wxLEFT|wxRIGHT|wxTOP, lmSPACING);
    pMainSizer->Add(new wxStaticText(this, wxID_STATIC, _("'+'  Up a sharp"),
									 wxDefaultPosition, wxDefaultSize, 0),
									 0, wxGROW|wxLEFT|wxRIGHT|wxTOP, lmSPACING);
    pMainSizer->Add(new wxStaticText(this, wxID_STATIC, _("'='  Remove accidentals"),
									 wxDefaultPosition, wxDefaultSize, 0),
									 0, wxGROW|wxLEFT|wxRIGHT|wxTOP, lmSPACING);



    SetAutoLayout(true);
    pMainSizer->Fit(this);
    pMainSizer->SetSizeHints(this);


}

lmToolNotesOpt::~lmToolNotesOpt()
{
}

lmENoteType lmToolNotesOpt::GetNoteDuration()
{
    return (lmENoteType)m_pCboDurations->GetSelection();
}

