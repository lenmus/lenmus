//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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

const long lmID_NOTEBOOK = ::wxNewId();

BEGIN_EVENT_TABLE(lmDlgProperties, wxDialog)
    EVT_BUTTON(wxID_OK, lmDlgProperties::OnAccept)
    EVT_BUTTON(wxID_CANCEL, lmDlgProperties::OnCancel)
    EVT_NOTEBOOK_PAGE_CHANGED(lmID_NOTEBOOK, lmDlgProperties::OnPageChanged)
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

    m_pNotebook = new wxNotebook( this, lmID_NOTEBOOK, wxDefaultPosition, wxDefaultSize, 0 );
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

    //get current page
    lmPropertiesPage* pCurPage = (lmPropertiesPage*)m_pNotebook->GetCurrentPage() ;

    //Editing an existing object. Do changes by issuing edit commands
    std::list<lmPropertiesPage*>::iterator it;
    for (it = m_pPages.begin(); it != m_pPages.end(); ++it)
        (*it)->OnAcceptChanges(m_pController, pCurPage == *it);

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

void lmDlgProperties::OnPageChanged(wxNotebookEvent& event)
{
    size_t nPage = (size_t)event.GetSelection();
    lmPropertiesPage* pPage = (lmPropertiesPage*)m_pNotebook->GetPage(nPage);
    pPage->OnEnterPage();
}


//---------------------------------------------------------------------------------------
// Abstract class lmPropertiesPage: the property pages to display
//---------------------------------------------------------------------------------------

lmPropertiesPage::lmPropertiesPage(lmDlgProperties* parent)
    : wxPanel(parent->GetNotebook(), wxID_ANY, wxDefaultPosition, wxSize(400, 250),
              wxTAB_TRAVERSAL)
    , m_pParent(parent)
{
}

void lmPropertiesPage::EnableAcceptButton(bool fEnable)
{
    m_pParent->EnableAcceptButton(fEnable);
}

