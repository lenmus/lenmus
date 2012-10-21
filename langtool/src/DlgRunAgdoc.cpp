//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 Cecilio Salmeron
//
//    This program is free software; you can redistribute it and/or modify it under the
//    terms of the GNU General Public License as published by the Free Software Foundation;
//    either version 3 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this
//    program. If not, see <http://www.gnu.org/licenses/>.
//
//
//    for (any comment, suggestion or feature request, please contact the manager of
//    the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------

// for (compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/dialog.h>
#include <wx/button.h>
#include <wx/xrc/xmlres.h>
#include <wx/filedlg.h>

#include "DlgRunAgdoc.h"

enum
{
	ltID_SELECT = 2600,
};


//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ltDlgRunAgdoc, wxDialog)
    EVT_BUTTON(wxID_OK, ltDlgRunAgdoc::OnAcceptClicked )
    EVT_BUTTON(wxID_CANCEL, ltDlgRunAgdoc::OnCancelClicked )
    EVT_BUTTON(ltID_SELECT, ltDlgRunAgdoc::OnSelectProject )

END_EVENT_TABLE()



ltDlgRunAgdoc::ltDlgRunAgdoc(wxWindow* parent, wxString* pProject)
    : wxDialog(parent, wxID_ANY, wxT("agdoc project selection"), wxDefaultPosition,
               wxDefaultSize, wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX )
    , m_pProject(pProject)
{
    Create();
    m_pTxtProject->SetValue(*m_pProject);
}

ltDlgRunAgdoc::~ltDlgRunAgdoc()
{
}

void ltDlgRunAgdoc::Create()
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* pMainSizer;
	pMainSizer = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );

	m_pLblProject = new wxStaticText( this, wxID_ANY, wxT("Project:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pLblProject->Wrap( -1 );
	bSizer3->Add( m_pLblProject, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_pTxtProject = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 400,-1 ), 0 );
	bSizer3->Add( m_pTxtProject, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_pBtSelect = new wxButton( this, ltID_SELECT, wxT("Select..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_pBtSelect, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	pMainSizer->Add( bSizer3, 1, wxEXPAND|wxALL, 5 );

	wxBoxSizer* pButtonsSizer;
	pButtonsSizer = new wxBoxSizer( wxHORIZONTAL );


	pButtonsSizer->Add( 0, 0, 1, wxEXPAND, 5 );

	m_pBtAccept = new wxButton( this, wxID_OK, wxT("Accept"), wxDefaultPosition, wxDefaultSize, 0 );
	pButtonsSizer->Add( m_pBtAccept, 0, wxALL, 5 );

	m_pBtCancel = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	pButtonsSizer->Add( m_pBtCancel, 0, wxALL, 5 );


	pButtonsSizer->Add( 0, 0, 1, wxEXPAND, 5 );

	pMainSizer->Add( pButtonsSizer, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxTOP|wxBOTTOM, 5 );

	this->SetSizer( pMainSizer );
	this->Layout();
	pMainSizer->Fit( this );

	this->Centre( wxBOTH );
}

void ltDlgRunAgdoc::OnAcceptClicked(wxCommandEvent& WXUNUSED(event))
{
    EndModal(wxID_OK);
}

void ltDlgRunAgdoc::OnCancelClicked(wxCommandEvent& WXUNUSED(event))
{
    EndDialog(wxID_CANCEL);
}

void ltDlgRunAgdoc::OnSelectProject(wxCommandEvent& WXUNUSED(event))
{
    // ask for the file to covert
    wxString sFilter = wxT("*.agproj");
    wxString sPath = ::wxFileSelector(_T("Choose the project file"),
                                        wxT(""),    //default path
                                        wxT(""),    //default filename
                                        wxT("agproj"),    //default_extension
                                        sFilter,
                                        wxFD_OPEN,        //flags
                                        this);
    if ( sPath.IsEmpty() ) return;
    *m_pProject = sPath;
    m_pTxtProject->SetValue(sPath);

}

