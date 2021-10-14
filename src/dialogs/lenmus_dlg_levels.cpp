//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2021 LenMus project
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

#include "lenmus_dlg_levels.h"
#include "lenmus_standard_header.h"

#include "lenmus_paths.h"
#include "lenmus_main_frame.h"      //event identifiers

//lomse
#include <lomse_logger.h>
using namespace lomse;

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/intl.h>
#include <wx/html/htmlwin.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>
#include <wx/dir.h>
#include <wx/filename.h>


namespace lenmus
{



//=======================================================================================
// LevelsDlg implementation
//=======================================================================================
LevelsDlg::LevelsDlg(wxWindow* pParent, ApplicationScope& appScope)
    : wxDialog(pParent, wxID_ANY, _("Choose level"),
               wxDefaultPosition, wxSize(500,200),
               wxDEFAULT_DIALOG_STYLE)
    , m_appScope(appScope)
    , m_pParent(pParent)
{
    CreateControls();
    CentreOnScreen();
}

//---------------------------------------------------------------------------------------
LevelsDlg::~LevelsDlg()
{
	// Disconnect Events
	m_pButtonL1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LevelsDlg::on_click_level1 ), nullptr, this );
	m_pButtonL2->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LevelsDlg::on_click_level2 ), nullptr, this );
	m_pButtonCustom->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LevelsDlg::on_click_custom_level ), nullptr, this );
}

//---------------------------------------------------------------------------------------
void LevelsDlg::CreateControls()
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* pMainSizer;
	pMainSizer = new wxBoxSizer( wxVERTICAL );


	pMainSizer->Add( 0, 0, 1, wxEXPAND, 5 );

	wxBoxSizer* pInnerSizer;
	pInnerSizer = new wxBoxSizer( wxHORIZONTAL );


	pInnerSizer->Add( 0, 0, 5, wxEXPAND|wxALL, 5 );

	m_pLabel = new wxStaticText( this, wxID_ANY, _("Difficulty level:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pLabel->Wrap( -1 );
	pInnerSizer->Add( m_pLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_pButtonL1 = new wxButton( this, wxID_ANY, _("Level 1"), wxDefaultPosition, wxDefaultSize, 0 );
	pInnerSizer->Add( m_pButtonL1, 0, wxALL|wxEXPAND, 5 );

	m_pButtonL2 = new wxButton( this, wxID_ANY, _("Level 2"), wxDefaultPosition, wxDefaultSize, 0 );
	pInnerSizer->Add( m_pButtonL2, 0, wxALL|wxEXPAND, 5 );

	m_pButtonCustom = new wxButton( this, wxID_ANY, _("Customized"), wxDefaultPosition, wxDefaultSize, 0 );
	pInnerSizer->Add( m_pButtonCustom, 0, wxALL|wxEXPAND, 5 );


	pInnerSizer->Add( 0, 0, 5, wxEXPAND|wxALL, 5 );


	pMainSizer->Add( pInnerSizer, 1, wxEXPAND, 5 );


	pMainSizer->Add( 0, 0, 1, wxEXPAND, 5 );


	this->SetSizer( pMainSizer );
	this->Layout();
	pMainSizer->Fit( this );

	this->Centre( wxBOTH );

	// Connect Events
	m_pButtonL1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LevelsDlg::on_click_level1 ), nullptr, this );
	m_pButtonL2->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LevelsDlg::on_click_level2 ), nullptr, this );
	m_pButtonCustom->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LevelsDlg::on_click_custom_level ), nullptr, this );
}

//---------------------------------------------------------------------------------------
void LevelsDlg::on_click_level1(wxCommandEvent& event)
{
    m_level = 1;
    EndModal(wxID_OK);
}

//---------------------------------------------------------------------------------------
void LevelsDlg::on_click_level2(wxCommandEvent& event)
{
    m_level = 2;
    EndModal(wxID_OK);
}

//---------------------------------------------------------------------------------------
void LevelsDlg::on_click_custom_level(wxCommandEvent& event)
{
    m_level = 100;
    EndModal(wxID_OK);
}


}   // namespace lenmus
