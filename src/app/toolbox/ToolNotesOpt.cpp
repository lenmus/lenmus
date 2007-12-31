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

    wxArrayString itemBitmapComboBox5Strings;

    wxBitmapComboBox* pCboDurations =
		new wxBitmapComboBox(this, wxID_ANY, _T(""), wxDefaultPosition,
				wxSize(lmPANEL_WIDTH - 2 * lmSPACING, 24),
				itemBitmapComboBox5Strings, wxCB_READONLY );
    pMainSizer->Add(pCboDurations, 0, wxGROW|wxLEFT|wxRIGHT, lmSPACING);

    SetAutoLayout(true);
    pMainSizer->Fit(this);
    pMainSizer->SetSizeHints(this);


	//populate combo box for note durations and select 'quarter' note
	pCboDurations->Append(_("Whole"),
		            wxArtProvider::GetBitmap(_T("tool_notes"), wxART_TOOLBAR, wxSize(24,24) ));
	pCboDurations->Append(_("Half"),
		            wxArtProvider::GetBitmap(_T("tool_notes"), wxART_TOOLBAR, wxSize(24,24) ));
	pCboDurations->Append(_("Quarter"),
		            wxArtProvider::GetBitmap(_T("tool_notes"), wxART_TOOLBAR, wxSize(24,24) ));
	pCboDurations->Append(_("Eighth"), 
		            wxArtProvider::GetBitmap(_T("tool_notes"), wxART_TOOLBAR, wxSize(24,24) ));
	pCboDurations->Append(_("16th"),
		            wxArtProvider::GetBitmap(_T("tool_notes"), wxART_TOOLBAR, wxSize(24,24) ));
	pCboDurations->Append(_("32nd"),
		            wxArtProvider::GetBitmap(_T("tool_notes"), wxART_TOOLBAR, wxSize(24,24) ));

	//pCboDurations->SetValue(_("Quarter"));
	pCboDurations->Select(2);

}

lmToolNotesOpt::~lmToolNotesOpt()
{
}

bool lmToolNotesOpt::Verify()
{
    return false;
}

void lmToolNotesOpt::Apply()
{
}
