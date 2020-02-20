//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2018 LenMus project
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

#include "lenmus_playback_opt_panel.h"
#include "lenmus_string.h"
#include "lenmus_colors.h"

//wxWidgets
#include <wx/xrc/xmlres.h>
#include <wx/config.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/checkbox.h>
#include <wx/clrpicker.h>
#include <wx/textctrl.h>
#include <wx/statbox.h>

//lomse
#include <lomse_interactor.h>       //enum EVisualTrackingMode



namespace lenmus
{

//---------------------------------------------------------------------------------------
PlaybackOptPanel::PlaybackOptPanel(wxWindow* parent, ApplicationScope& appScope)
    : OptionsPanel(parent, appScope)
{
    create_controls();

    //load icon
    m_pTitleIcon->SetBitmap( wxArtProvider::GetIcon("opt_playback", wxART_TOOLBAR, wxSize(24,24)) );

    //set error icons
    wxBitmap bmpError =
         wxArtProvider::GetBitmap("msg_error", wxART_TOOLBAR, wxSize(16,16));
    m_pBmpLineWidthError->SetBitmap(bmpError);

    //hide all error messages and their associated icons
    m_pTxtLineWidthError->Show(false);
    m_pBmpLineWidthError->Show(false);

        //Select current settings

    Colors* colors= m_appScope.get_colors();
    m_colorHighlight->SetColour( Colors::to_wx_color(colors->highlight_color()) );
    m_colorTempoLine->SetColour( Colors::to_wx_color(colors->tempo_line_color()) );

    int mode = m_appScope.get_visual_tracking_mode();
    m_pChkHighlight->SetValue(mode & k_tracking_highlight_notes);
    m_pChkTempoLine->SetValue(mode & k_tracking_tempo_line);

    LUnits width = m_appScope.get_tempo_line_width();
    m_txtLineWidth->SetValue( wxString::Format("%.02f", width/100.0f) );
}

//---------------------------------------------------------------------------------------
void PlaybackOptPanel::create_controls()
{
    //content generated with wxFormBuilder
    //Manual changes:
    // - none

	this->SetExtraStyle( wxWS_EX_BLOCK_EVENTS );

	wxBoxSizer* pMainSizer;
	pMainSizer = new wxBoxSizer( wxVERTICAL );

	m_pHeaderPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER|wxTAB_TRAVERSAL );
	wxBoxSizer* pHeaderSizer;
	pHeaderSizer = new wxBoxSizer( wxHORIZONTAL );

	m_pTxtTitle = new wxStaticText( m_pHeaderPanel, wxID_ANY, _("Playback options"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	m_pTxtTitle->Wrap( -1 );
	m_pTxtTitle->SetFont( wxFont(12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Tahoma") ) );

	pHeaderSizer->Add( m_pTxtTitle, 0, wxALIGN_TOP|wxALL, 5 );


	pHeaderSizer->Add( 5, 5, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_pTitleIcon = new wxStaticBitmap( m_pHeaderPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	pHeaderSizer->Add( m_pTitleIcon, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_pHeaderPanel->SetSizer( pHeaderSizer );
	m_pHeaderPanel->Layout();
	pHeaderSizer->Fit( m_pHeaderPanel );
	pMainSizer->Add( m_pHeaderPanel, 0, wxEXPAND|wxBOTTOM, 5 );

	wxBoxSizer* pOptionsSizer;
	pOptionsSizer = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* pChecksSizer;
	pChecksSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Visual tracking") ), wxVERTICAL );

	m_pChkHighlight = new wxCheckBox( this, k_id_check_highlight, _("Highlight notes being played back in color"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
	pChecksSizer->Add( m_pChkHighlight, 0, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );


	bSizer4->Add( 0, 0, 1, wxEXPAND, 5 );

	m_staticText2 = new wxStaticText( this, wxID_ANY, _("Highlight color"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer4->Add( m_staticText2, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxLEFT, 5 );

	m_colorHighlight = new wxColourPickerCtrl( this, k_id_highlight_color, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	bSizer4->Add( m_colorHighlight, 0, wxRIGHT|wxLEFT, 5 );

	pChecksSizer->Add( bSizer4, 0, wxBOTTOM|wxEXPAND, 5 );

	m_pChkTempoLine = new wxCheckBox( this, k_id_check_tempo_line, _("Draw a cursor line at current beat"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
	pChecksSizer->Add( m_pChkTempoLine, 0, wxEXPAND|wxALL, 5 );

	wxBoxSizer* bSizer41;
	bSizer41 = new wxBoxSizer( wxHORIZONTAL );


	bSizer41->Add( 0, 0, 1, wxEXPAND, 5 );

	m_staticText21 = new wxStaticText( this, wxID_ANY, _("Cursor line color"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText21->Wrap( -1 );
	bSizer41->Add( m_staticText21, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxLEFT, 5 );

	m_colorTempoLine = new wxColourPickerCtrl( this, k_id_tempo_line_color, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	bSizer41->Add( m_colorTempoLine, 0, wxRIGHT|wxLEFT, 5 );

	pChecksSizer->Add( bSizer41, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxHORIZONTAL );


	bSizer7->Add( 0, 0, 1, wxEXPAND, 5 );

	m_staticText5 = new wxStaticText( this, wxID_ANY, _("Cursor line width (millimeters)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	bSizer7->Add( m_staticText5, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxRIGHT|wxLEFT, 5 );

	m_txtLineWidth = new wxTextCtrl( this, k_id_tempo_line_width, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_txtLineWidth, 0, wxBOTTOM|wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );

	bSizer8->Add( bSizer7, 0, wxEXPAND, 5 );

	pChecksSizer->Add( bSizer8, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );

	m_pBmpLineWidthError = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( m_pBmpLineWidthError, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );

	m_pLblSpacer1 = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_pLblSpacer1->Wrap( -1 );
	bSizer9->Add( m_pLblSpacer1, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );

	m_pTxtLineWidthError = new wxStaticText( this, wxID_ANY, _("Error: invalid width"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pTxtLineWidthError->Wrap( -1 );
	bSizer9->Add( m_pTxtLineWidthError, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );

	pChecksSizer->Add( bSizer9, 0, wxALIGN_RIGHT, 5 );

	pOptionsSizer->Add( pChecksSizer, 0, wxEXPAND|wxALL, 5 );

	pMainSizer->Add( pOptionsSizer, 0, wxEXPAND|wxALL, 5 );

	this->SetSizer( pMainSizer );
	this->Layout();
}

//---------------------------------------------------------------------------------------
PlaybackOptPanel::~PlaybackOptPanel()
{
}

//---------------------------------------------------------------------------------------
bool PlaybackOptPanel::Verify()
{
    //assume no errors
    bool fError = false;
    bool fRelayout = false;

    m_pTxtLineWidthError->Show(false);
    m_pBmpLineWidthError->Show(false);

    //verify tempo line width
    wxString sValue = m_txtLineWidth->GetValue();
    double width;
    if (!sValue.ToDouble(&width))
    {
        m_pTxtLineWidthError->Show(true);
        m_pBmpLineWidthError->Show(true);
        fError = true;
        if (m_fFirstTimeError) {
            fRelayout = true;
            m_fFirstTimeError = false;
        }
    }

    if (fRelayout) {
        // Force a relayout. This code is to by pass a bad behaviour.
        // When the visibility status (hide/show) of error messages is changed
        // they are not properly drawn in their right positions. So we have to force a
        // relayout
        GetSizer()->CalcMin();
        GetSizer()->Layout();
        GetSizer()->Fit(this);
        GetSizer()->SetSizeHints(this);
//        //Now the panel is properly drawn but its width changes and the splitter windows
//        //doesn't get aware. It is necessary to force a redraw of the splitter window
//        m_pParent->SetSashPosition(m_pParent->GetSashPosition(), true);
//        //OK. Now it works.
    }

    return fError;
}

//---------------------------------------------------------------------------------------
void PlaybackOptPanel::Apply()
{
    wxConfigBase* pPrefs = m_appScope.get_preferences();
    Colors* pColors = m_appScope.get_colors();

    wxColour color = m_colorHighlight->GetColour();
    pColors->set_highlight_color( Colors::to_lomse_color(color) );

    color = m_colorTempoLine->GetColour();
    pColors->set_tempo_line_color( Colors::to_lomse_color(color) );

    int mode = 0;
    bool fValue = m_pChkHighlight->GetValue();
    pPrefs->Write("/Playback/HighlightMode", fValue);
    if (fValue)
        mode |= k_tracking_highlight_notes;
    fValue = m_pChkTempoLine->GetValue();
    pPrefs->Write("/Playback/TempoLineMode", fValue);
    if (fValue)
        mode |= k_tracking_tempo_line;
    m_appScope.set_visual_tracking_mode(mode);

    wxString sValue = m_txtLineWidth->GetValue();
    double width;
    sValue.ToDouble(&width);    //already verified in Verify() method
    pPrefs->Write("/Playback/TempoLineWidth", sValue);
    m_appScope.set_tempo_line_width( LUnits(width * 100.0) );
}


}   //namespace lenmus
