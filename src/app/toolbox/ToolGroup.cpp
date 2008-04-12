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
#pragma implementation "ToolGroup.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "ToolGroup.h"
#include "ToolPage.h"

#define lmTOOLGROUP_SIZE wxSize(400, 300)
#define lmTOOLGROUP_STYLE wxCAPTION | wxRESIZE_BORDER


lmToolGroup::lmToolGroup(lmToolPage* pParent)
	: wxBoxSizer(wxVERTICAL)
{
    Init();
	m_pParent = pParent;
}

lmToolGroup::~lmToolGroup()
{
}

void lmToolGroup::Init()
{
	//member initialisation
}

wxBoxSizer* lmToolGroup::CreateGroup(wxBoxSizer* pParentSizer, wxString sTitle)
{    
	//create the controls for this lmToolGroup

    wxStaticBox* pMainSizer = new wxStaticBox(m_pParent, wxID_ANY, sTitle);
    wxStaticBoxSizer* pAuxSizer = new wxStaticBoxSizer(pMainSizer, wxVERTICAL);
    pMainSizer->SetFont(wxFont(8, wxSWISS, wxNORMAL, wxBOLD, false, wxT("Tahoma")));
	pMainSizer->SetForegroundColour(m_pParent->GetColors()->PrettyDark());
	pMainSizer->SetBackgroundColour(m_pParent->GetColors()->Normal());
    pParentSizer->Add(pAuxSizer, 0, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxBoxSizer* pCtrolsSizer = new wxBoxSizer(wxVERTICAL);
    pAuxSizer->Add(pCtrolsSizer, 0, wxGROW, 5);

	return pCtrolsSizer;
}

int lmToolGroup::GetGroupWitdh()
{	
	int width, height;
	m_pParent->GetClientSize(&width, &height);
	return width;
}
