//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2014 LenMus project
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

#include "lenmus_toolbar_opt_panel.h"

#include "lenmus_app.h"
#include "lenmus_main_frame.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/xrc/xmlres.h>
#include <wx/config.h>


namespace lenmus
{

//---------------------------------------------------------------------------------------
ToolbarsOptPanel::ToolbarsOptPanel(wxWindow* parent, ApplicationScope& appScope)
    : OptionsPanel(parent, appScope)
{
    create_controls();

    //load icon
    m_pBmpIconTitle->SetBitmap( wxArtProvider::GetIcon("tool_properties", wxART_TOOLBAR, wxSize(24,24)) );

    //
    // set panel controls according to current user selected options
    //

    wxConfigBase* pPrefs = m_appScope.get_preferences();

    //icons' size
    long nIconSize = pPrefs->Read("/Toolbars/IconSize", 16);
    if (nIconSize == 32)
        m_nSizeIndex = 2;
    else if (nIconSize == 24)
        m_nSizeIndex = 1;
    else
        m_nSizeIndex = 0;
    m_pOptIconSize->SetSelection(m_nSizeIndex);

    //For now isable large size option
    //TODO create large icons set for toolbar buttons
    m_pOptIconSize->Enable(2, false);


    // labels
    m_nLabelsIndex = (int) pPrefs->Read("/Toolbars/Labels", 0L);
    m_pOptLabels->SetSelection(m_nLabelsIndex);
}

//---------------------------------------------------------------------------------------
ToolbarsOptPanel::~ToolbarsOptPanel()
{
}

//---------------------------------------------------------------------------------------
void ToolbarsOptPanel::create_controls()
{
    //content generated with wxFormBuilder

	this->SetExtraStyle( wxWS_EX_BLOCK_EVENTS );

	wxBoxSizer* pMainSizer;
	pMainSizer = LENMUS_NEW wxBoxSizer( wxVERTICAL );

	m_pMainPanel = LENMUS_NEW wxPanel( this, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxRAISED_BORDER|wxTAB_TRAVERSAL );
	wxBoxSizer* pTitleSizer;
	pTitleSizer = LENMUS_NEW wxBoxSizer( wxHORIZONTAL );

	m_pLblTitle = LENMUS_NEW wxStaticText( m_pMainPanel, wxID_ANY, _("Preferences for toolbars"), wxDefaultPosition, wxSize( -1,-1 ), wxALIGN_LEFT );
	m_pLblTitle->Wrap( -1 );
	m_pLblTitle->SetFont( wxFont(12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Tahoma" ) );

	pTitleSizer->Add( m_pLblTitle, 0, wxALIGN_TOP|wxALL, 5 );


	pTitleSizer->Add( 5, 5, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_pBmpIconTitle = LENMUS_NEW wxStaticBitmap( m_pMainPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( -1,-1 ), 0 );
	pTitleSizer->Add( m_pBmpIconTitle, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_pMainPanel->SetSizer( pTitleSizer );
	m_pMainPanel->Layout();
	pTitleSizer->Fit( m_pMainPanel );
	pMainSizer->Add( m_pMainPanel, 0, wxEXPAND|wxBOTTOM, 5 );

	wxBoxSizer* pBoxSize;
	pBoxSize = LENMUS_NEW wxBoxSizer( wxHORIZONTAL );

	wxString m_pOptIconSizeChoices[] = { _("&Small"), _("&Medium"), _("&Large") };
	int m_pOptIconSizeNChoices = sizeof( m_pOptIconSizeChoices ) / sizeof( wxString );
	m_pOptIconSize = LENMUS_NEW wxRadioBox( this, wxID_ANY, _("Icons' size"), wxDefaultPosition, wxSize( -1,-1 ), m_pOptIconSizeNChoices, m_pOptIconSizeChoices, 0, wxRA_SPECIFY_ROWS );
	m_pOptIconSize->SetSelection( 0 );
	pBoxSize->Add( m_pOptIconSize, 0, wxEXPAND|wxALL, 5 );

	pMainSizer->Add( pBoxSize, 0, wxEXPAND|wxALL, 5 );

	wxBoxSizer* pBoxLabels;
	pBoxLabels = LENMUS_NEW wxBoxSizer( wxHORIZONTAL );

	wxString m_pOptLabelsChoices[] = { _("&No labels"), _("Labels &bottom"), _("Labels &right") };
	int m_pOptLabelsNChoices = sizeof( m_pOptLabelsChoices ) / sizeof( wxString );
	m_pOptLabels = LENMUS_NEW wxRadioBox( this, wxID_ANY, _("Labels"), wxDefaultPosition, wxSize( -1,-1 ), m_pOptLabelsNChoices, m_pOptLabelsChoices, 0, wxRA_SPECIFY_ROWS );
	m_pOptLabels->SetSelection( 2 );
	pBoxLabels->Add( m_pOptLabels, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	pMainSizer->Add( pBoxLabels, 0, wxALIGN_LEFT|wxALL, 5 );

	this->SetSizer( pMainSizer );
	this->Layout();
}

//---------------------------------------------------------------------------------------
bool ToolbarsOptPanel::Verify()
{
    return false;
}

//---------------------------------------------------------------------------------------
void ToolbarsOptPanel::Apply()
{
    wxConfigBase* pPrefs = m_appScope.get_preferences();

    // icons' size
    int nSizeIndex = m_pOptIconSize->GetSelection();
    if (nSizeIndex != m_nSizeIndex)
    {
        long nIconSize;
        if (nSizeIndex == 2)
            nIconSize = 32;
        else if (nSizeIndex == 1)
            nIconSize = 24;
        else
            nIconSize = 16;
        pPrefs->Write("/Toolbars/IconSize", nIconSize);
    }

    // labels
    int nLabelsIndex = m_pOptLabels->GetSelection();
    if (nLabelsIndex != m_nLabelsIndex)
        pPrefs->Write("/Toolbars/Labels", nLabelsIndex);

    // update toolbars
    if ((nSizeIndex != m_nSizeIndex) || (nLabelsIndex != m_nLabelsIndex))
    {
        MainFrame* pMainFrame = dynamic_cast<MainFrame*>( wxGetApp().GetTopWindow());
        pMainFrame->update_toolbars_layout();
    }

}


}   //namespace lenmus
