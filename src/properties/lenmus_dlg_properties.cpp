//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2010-2015 LenMus project
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
//---------------------------------------------------------------------------------------

//lenmus headers
#include "lenmus_dlg_properties.h"
#include "lenmus_standard_header.h"

#include "lenmus_document_canvas.h"
#include "lenmus_properties_barline.h"
#include "lenmus_properties_general.h"
#include "lenmus_string.h"


//wxWidgets
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif


namespace lenmus
{

//=======================================================================================
// DlgProperties implementation
//=======================================================================================

const long k_id_notebook = ::wxNewId();

wxBEGIN_EVENT_TABLE(DlgProperties, wxDialog)
    EVT_BUTTON(wxID_OK, DlgProperties::OnAccept)
    EVT_BUTTON(wxID_CANCEL, DlgProperties::OnCancel)
    EVT_NOTEBOOK_PAGE_CHANGED(k_id_notebook, DlgProperties::OnPageChanged)
wxEND_EVENT_TABLE()

//---------------------------------------------------------------------------------------
DlgProperties::DlgProperties(wxWindow* pParent, ApplicationScope& appScope,
                             CommandGenerator* pExecuter)
    : wxDialog(pParent, wxID_ANY, _("Properties"), wxDefaultPosition, wxSize(400, 250),
               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER )
    , m_appScope(appScope)
    , m_pExecuter(pExecuter)
{
    CreateControls();
}

//---------------------------------------------------------------------------------------
void DlgProperties::CreateControls()
{
    this->SetSizeHints( 500,350 );

    m_pMainSizer = new wxBoxSizer( wxVERTICAL );

    m_pNotebook = new wxNotebook( this, k_id_notebook, wxDefaultPosition, wxDefaultSize, 0 );
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

//---------------------------------------------------------------------------------------
DlgProperties::~DlgProperties()
{
    //AWARE: Do not delete the notebook pages, they will be deleted by the notebook
    m_pPages.clear();
}

//---------------------------------------------------------------------------------------
void DlgProperties::OnAccept(wxCommandEvent& WXUNUSED(event))
{
    //apply changes

    //get current page
    PropertiesPage* pCurPage = (PropertiesPage*)m_pNotebook->GetCurrentPage() ;

    //Editing an existing object. Do changes by issuing edit commands
    std::list<PropertiesPage*>::iterator it;
    for (it = m_pPages.begin(); it != m_pPages.end(); ++it)
        (*it)->OnAcceptChanges(m_pExecuter, pCurPage == *it);

    EndModal(wxID_OK);
}

//---------------------------------------------------------------------------------------
void DlgProperties::OnCancel(wxCommandEvent& WXUNUSED(event))
{
    //inform all panels
    std::list<PropertiesPage*>::iterator it;
    for (it = m_pPages.begin(); it != m_pPages.end(); ++it)
        (*it)->OnCancelChanges();

    EndDialog(wxID_CANCEL);
}

//---------------------------------------------------------------------------------------
void DlgProperties::AddPanel(PropertiesPage* pPanel, const wxString& sTabName)
{
    m_pPages.push_back(pPanel);
    m_pNotebook->AddPage(pPanel, sTabName);
    pPanel->Layout();
    this->Layout();
    m_pMainSizer->Fit( this );
}

//---------------------------------------------------------------------------------------
void DlgProperties::OnPageChanged(wxNotebookEvent& event)
{
    size_t nPage = (size_t)event.GetSelection();
    PropertiesPage* pPage = (PropertiesPage*)m_pNotebook->GetPage(nPage);
    pPage->OnEnterPage();
}

//---------------------------------------------------------------------------------------
void DlgProperties::add_specific_panels_for(ImoObj* pImo)
{
    //factory method to create panels for each ImoObj class

    add_general_panel_for(pImo);

    m_pOwnerImo = pImo;
    PropertiesPage* pPanel = nullptr;
    wxString tabName;

    switch(pImo->get_obj_type())
    {
        case k_imo_barline:
            pPanel = LENMUS_NEW BarlineProperties(this, static_cast<ImoBarline*>(pImo));
            tabName = _("Barline");
            break;

        default:
            ;
    }

    if (pPanel)
        AddPanel(pPanel, tabName);
}

//---------------------------------------------------------------------------------------
void DlgProperties::add_general_panel_for(ImoObj* pImo)
{
    PropertiesPage* pPanel = LENMUS_NEW GeneralProperties(this, pImo);
    AddPanel(pPanel, _("General"));
    SetTitle(wxGetTranslation( to_wx_string(pImo->get_name()) ));
}


//=======================================================================================
// PropertiesPage implementation
//=======================================================================================
PropertiesPage::PropertiesPage(DlgProperties* parent)
    : wxPanel(parent->GetNotebook(), wxID_ANY, wxDefaultPosition, wxSize(400, 250),
              wxTAB_TRAVERSAL)
    , m_pParent(parent)
{
}

//---------------------------------------------------------------------------------------
void PropertiesPage::EnableAcceptButton(bool fEnable)
{
    m_pParent->EnableAcceptButton(fEnable);
}


}   //namespace lenmus
