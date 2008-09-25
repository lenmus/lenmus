//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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
#pragma implementation "DlgProperties.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "DlgProperties.h"

#include "../Text.h"
#include "../../app/ArtProvider.h"
#include "../../app/ScoreCanvas.h"

//--------------------------------------------------------------------------------------
// Implementation of lmDlgProperties
//--------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(lmDlgProperties, wxDialog)
    EVT_BUTTON(wxID_OK, lmDlgProperties::OnAccept)
    EVT_BUTTON(wxID_CANCEL, lmDlgProperties::OnCancel)
END_EVENT_TABLE()


lmDlgProperties::lmDlgProperties(lmController* pController)
    : wxDialog(pController, wxID_ANY, _("Properties"), wxDefaultPosition, wxSize(400, 250),
               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER )
{
    m_pController = pController;
    CreateControls();
}

void lmDlgProperties::CreateControls()
{
    this->SetSizeHints( 500,350 );
    
    m_pMainSizer = new wxBoxSizer( wxVERTICAL );
    
    m_pNotebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
    //m_pPages[0] = new wxPanel( m_pNotebook, wxID_ANY, wxDefaultPosition, wxSize( 500,350 ), wxTAB_TRAVERSAL );
    //wxBoxSizer* bSizer3;
    //bSizer3 = new wxBoxSizer( wxVERTICAL );
    //
    //wxTextCtrl* m_textCtrl1 = new wxTextCtrl( m_pPages[0], wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 450,300 ), 0 );
    //bSizer3->Add( m_textCtrl1, 0, wxALL, 5 );
    //
    //m_pPages[0]->SetSizer( bSizer3 );
    //m_pPages[0]->Layout();
    //m_pNotebook->AddPage( m_pPages[0], _("a page"), false );
    //m_pPages[1] = new wxPanel( m_pNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    //m_pNotebook->AddPage( m_pPages[1], _("a page"), false );
    
    m_pMainSizer->Add( m_pNotebook, 1, wxEXPAND | wxALL, 5 );
    
    wxBoxSizer* pButtonsSizer;
    pButtonsSizer = new wxBoxSizer( wxHORIZONTAL );
    
    
    pButtonsSizer->Add( 0, 0, 1, wxEXPAND, 5 );
    
    m_pBtAccept = new wxButton( this, wxID_OK, _("Accept"), wxDefaultPosition, wxDefaultSize, 0 );
    pButtonsSizer->Add( m_pBtAccept, 0, wxALL, 5 );
    
    
    pButtonsSizer->Add( 0, 0, 1, 0, 5 );
    
    m_pBtCancel = new wxButton( this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    pButtonsSizer->Add( m_pBtCancel, 0, wxALL, 5 );
    
    
    pButtonsSizer->Add( 0, 0, 1, wxEXPAND, 5 );
    
    m_pMainSizer->Add( pButtonsSizer, 0, wxEXPAND, 5 );
    
    this->SetSizer( m_pMainSizer );
    this->Layout();
    m_pMainSizer->Fit( this );
}

lmDlgProperties::~lmDlgProperties()
{
    //AWARE: Do not delete the notebook pages, they will be deleted by the notebook
    m_pPages.clear();
}

void lmDlgProperties::OnAccept(wxCommandEvent& WXUNUSED(event))
{
    //apply changes

    //Editing an existing object. Do changes by issuing edit commands
    std::list<lmPropertiesPage*>::iterator it;
    for (it = m_pPages.begin(); it != m_pPages.end(); ++it)
        (*it)->OnAcceptChanges(m_pController);

    EndModal(wxID_OK);
}

void lmDlgProperties::OnCancel(wxCommandEvent& WXUNUSED(event))
{
    //inform all panels
    std::list<lmPropertiesPage*>::iterator it;
    for (it = m_pPages.begin(); it != m_pPages.end(); ++it)
        (*it)->OnCancelChanges();

    EndDialog(wxID_CANCEL);
}

void lmDlgProperties::AddPanel(lmPropertiesPage* pPanel, const wxString& sTabName)
{
    m_pPages.push_back(pPanel);
    m_pNotebook->AddPage(pPanel, sTabName);
    pPanel->Layout();
    this->Layout();
    m_pMainSizer->Fit( this );
}