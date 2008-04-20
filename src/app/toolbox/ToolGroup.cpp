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
#include "../ArtProvider.h"        // to use ArtProvider for managing icons

#define lmTOOLGROUP_SIZE wxSize(160, 90)
#define lmTOOLGROUP_STYLE wxCAPTION | wxRESIZE_BORDER


lmToolGroup::lmToolGroup(lmToolPage* pParent)
	: wxPanel(pParent, wxID_ANY, wxDefaultPosition, lmTOOLGROUP_SIZE)
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

wxBoxSizer* lmToolGroup::CreateGroup(wxBoxSizer* pMainSizer, wxString sTitle)
{    
	//create common controls for a lmToolGroup
	wxStaticBoxSizer* pAuxSizer =
	    new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, sTitle ), wxVERTICAL );
	pMainSizer->Add( this, 0, wxALL|wxEXPAND, 5 );

    this->SetFont(wxFont(8, wxSWISS, wxNORMAL, wxBOLD, false, wxT("Tahoma")));
	this->SetForegroundColour(m_pParent->GetColors()->PrettyDark());
	this->SetBackgroundColour(m_pParent->GetColors()->Normal());

	wxBoxSizer* pCtrolsSizer = new wxBoxSizer( wxVERTICAL );
	pAuxSizer->Add( pCtrolsSizer, 1, wxEXPAND, 5 );
	pAuxSizer->Fit( this );
	this->SetSizer( pAuxSizer );

	return pCtrolsSizer;
}

int lmToolGroup::GetGroupWitdh()
{	
	int width, height;
	m_pParent->GetClientSize(&width, &height);
	return width;
}

wxBitmap lmToolGroup::CreateOnBitmap(wxString sBmpName)
{
    //get off bitmap
	wxBitmap bmpOff = wxArtProvider::GetBitmap(sBmpName, wxART_TOOLBAR, wxSize(24, 24));
    wxMemoryDC dcOff;
    dcOff.SelectObject(bmpOff);

    //get the on bitmap background
	wxBitmap bmpOn = wxArtProvider::GetBitmap(_T("button_normal"), wxART_TOOLBAR, wxSize(24, 24));
    wxMemoryDC dcOn;
    dcOn.SelectObject(bmpOn);

    //blend both bitmaps
    dcOn.Blit(0, 0, 24, 24, &dcOff, 0, 0, wxCOPY, true);

    //clean up and return new bitmap
    dcOn.SelectObject(wxNullBitmap);
    dcOff.SelectObject(wxNullBitmap);

    return bmpOn;
} 
