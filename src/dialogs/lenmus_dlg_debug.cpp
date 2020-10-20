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

#include "lenmus_dlg_debug.h"
#include "lenmus_main_frame.h"

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
#include <wx/stattext.h>
#include <wx/textctrl.h>


namespace lenmus
{

// IDs for controls
enum
{
	lmID_HTML_WND = 3010,
	lmID_ACCEPT,
	lmID_SAVE,
};


//=======================================================================================
// DlgDebug implementation
//=======================================================================================


wxBEGIN_EVENT_TABLE(DlgDebug, wxDialog)
   EVT_BUTTON(wxID_OK, DlgDebug::OnOK)
   EVT_BUTTON(lmID_SAVE, DlgDebug::OnSave)
wxEND_EVENT_TABLE()

IMPLEMENT_CLASS(DlgDebug, wxDialog)

//---------------------------------------------------------------------------------------
DlgDebug::DlgDebug(wxWindow * parent, wxString sTitle, wxString sData, bool fSave)
    : wxDialog(parent, -1, sTitle, wxDefaultPosition, wxSize(1000, 430),
               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER )
    , m_fSave(fSave)
{
    Centre();

    wxBoxSizer* pMainSizer = LENMUS_NEW wxBoxSizer(wxVERTICAL);

    // use wxTE_RICH2 style to avoid 64kB limit under MSW and display big files
    // faster than with wxTE_RICH
    m_pTxtData = LENMUS_NEW wxTextCtrl(this, wxID_ANY, sData,
                                wxPoint(0, 0), wxDefaultSize,
                                wxTE_MULTILINE | wxTE_READONLY | wxTE_NOHIDESEL
                                | wxTE_RICH2);

    // use fixed-width font
    m_pTxtData->SetFont(wxFont(10, wxFONTFAMILY_TELETYPE,
                               wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

    pMainSizer->Add(m_pTxtData,
                    1,            //vertically stretchable
                    wxEXPAND |    //horizontally stretchable
                    wxALL,        //some space border all around
                    5 );          //set border width to 5 px

    wxBoxSizer* pButtonsSizer = LENMUS_NEW wxBoxSizer(wxHORIZONTAL);

    wxButton *cmdOK = LENMUS_NEW wxButton(this, wxID_OK, _("OK"));
    pButtonsSizer->Add(cmdOK, 0, 0, 1);
    cmdOK->SetDefault();
    cmdOK->SetFocus();

    if (m_fSave)
    {
	    wxButton *cmdSave = LENMUS_NEW wxButton(this, lmID_SAVE, _("Save"));

	    pButtonsSizer->Add(cmdSave, 0, 0, 1);
    }

    pMainSizer->Add(pButtonsSizer, 0, wxALIGN_CENTER | wxALL, 5);

    // set autolayout based on sizers
    SetAutoLayout(true);
    SetSizer(pMainSizer);
}

//---------------------------------------------------------------------------------------
DlgDebug::~DlgDebug()
{
}

//---------------------------------------------------------------------------------------
void DlgDebug::OnOK(wxCommandEvent& WXUNUSED(event))
{
   EndModal(wxID_OK);
}

//---------------------------------------------------------------------------------------
void DlgDebug::AppendText(wxString sText)
{
    m_pTxtData->AppendText(sText);
}

//---------------------------------------------------------------------------------------
void DlgDebug::OnSave(wxCommandEvent& WXUNUSED(event))
{
    wxString sFilename = wxFileSelector("File to save", "", "debug", "txt",
                                        "*.*",  wxFD_SAVE);
	if ( !sFilename.empty() )
	{
		// save the file
		m_pTxtData->SaveFile(sFilename);
	}
	//else: cancelled by user

}



//-------------------------------------------------------------------------------------------
// lmHtmlDlg implementation
//-------------------------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(lmHtmlDlg, wxDialog)
    EVT_BUTTON(lmID_ACCEPT, lmHtmlDlg::OnAcceptClicked )
    EVT_BUTTON(lmID_SAVE, lmHtmlDlg::OnSaveClicked )

wxEND_EVENT_TABLE()


//---------------------------------------------------------------------------------------
lmHtmlDlg::lmHtmlDlg(wxWindow* pParent, const wxString& sTitle, bool fSaveButton)
    : wxDialog(pParent, wxID_ANY, sTitle, wxDefaultPosition, wxSize(600,400),
               wxDEFAULT_DIALOG_STYLE)
{
    // create the dialog controls
    CreateControls(fSaveButton);
    CentreOnScreen();
}

//---------------------------------------------------------------------------------------
void lmHtmlDlg::CreateControls(bool fSaveButton)
{
    //AWARE: Code created with wxFormBuilder and copied here.
    //Modifications:
    // - near line 178: add 'if' to hide Save button

    this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* pMainSizer;
	pMainSizer = LENMUS_NEW wxBoxSizer( wxVERTICAL );

	m_pHtmlWnd = LENMUS_NEW wxHtmlWindow( this, lmID_HTML_WND, wxDefaultPosition, wxDefaultSize, wxHW_SCROLLBAR_AUTO );
	pMainSizer->Add( m_pHtmlWnd, 1, wxALL|wxEXPAND, 5 );

	wxBoxSizer* pButtonsSizer;
	pButtonsSizer = LENMUS_NEW wxBoxSizer( wxHORIZONTAL );

	m_pBtnAccept = LENMUS_NEW wxButton( this, lmID_ACCEPT, _("Accept"), wxDefaultPosition, wxDefaultSize, 0 );
	pButtonsSizer->Add( m_pBtnAccept, 0, wxALL, 5 );


	pButtonsSizer->Add( 0, 0, 1, wxEXPAND, 5 );

    if (fSaveButton)
    {
	    m_pBtnSave = LENMUS_NEW wxButton( this, lmID_SAVE, _("Save"), wxDefaultPosition, wxDefaultSize, 0 );
	    pButtonsSizer->Add( m_pBtnSave, 0, wxALL, 5 );
    }

	pMainSizer->Add( pButtonsSizer, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

	this->SetSizer( pMainSizer );
	this->Layout();
}

//---------------------------------------------------------------------------------------
lmHtmlDlg::~lmHtmlDlg()
{
}

//---------------------------------------------------------------------------------------
void lmHtmlDlg::OnAcceptClicked(wxCommandEvent& WXUNUSED(event))
{
   EndModal(wxID_OK);
}

//---------------------------------------------------------------------------------------
void lmHtmlDlg::OnSaveClicked(wxCommandEvent& WXUNUSED(event))
{
	wxString sFilename = wxFileSelector(_("File to save"));
	if ( !sFilename.empty() )
	{
		// save the file
		//m_pTxtData->SaveFile(sFilename);
	}
	//else: cancelled by user

}

//---------------------------------------------------------------------------------------
void lmHtmlDlg::SetContent(const wxString& sContent)
{
    m_pHtmlWnd->SetPage(sContent);
}


//=======================================================================================
// DlgSpacingParams implementation
//=======================================================================================

wxBEGIN_EVENT_TABLE(DlgSpacingParams, wxDialog)
   EVT_BUTTON(wxID_OK, DlgSpacingParams::on_close)
   EVT_BUTTON(lmID_SAVE, DlgSpacingParams::on_update)
wxEND_EVENT_TABLE()

IMPLEMENT_CLASS(DlgSpacingParams, wxDialog)

//---------------------------------------------------------------------------------------
DlgSpacingParams::DlgSpacingParams(wxWindow * parent, float force, float alpha,
                                   float dmin)
    : wxDialog(parent, -1, "Spacing parameters", wxDefaultPosition, wxSize(400, 230))
    , m_force(force)
    , m_alpha(alpha)
    , m_dmin(dmin)
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* pMainSizer;
	pMainSizer = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* pGridSizer;
	pGridSizer = new wxFlexGridSizer( 3, 2, 0, 0 );
	pGridSizer->SetFlexibleDirection( wxBOTH );
	pGridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_lblForce = new wxStaticText( this, wxID_ANY, _("Optimum force:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblForce->Wrap( -1 );
	pGridSizer->Add( m_lblForce, 0, wxTOP|wxRIGHT|wxLEFT, 5 );

	m_txtForce = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	pGridSizer->Add( m_txtForce, 0, wxTOP|wxRIGHT|wxLEFT, 5 );

	m_lblAlpha = new wxStaticText( this, wxID_ANY, _("Alpha:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblAlpha->Wrap( -1 );
	pGridSizer->Add( m_lblAlpha, 0, wxTOP|wxRIGHT|wxLEFT, 5 );

	m_txtAlpha = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	pGridSizer->Add( m_txtAlpha, 0, wxTOP|wxRIGHT|wxLEFT, 5 );

	m_lblDmin = new wxStaticText( this, wxID_ANY, _("D min:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblDmin->Wrap( -1 );
	pGridSizer->Add( m_lblDmin, 0, wxALL, 5 );

	m_txtDmin = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	pGridSizer->Add( m_txtDmin, 0, wxALL, 5 );

	pMainSizer->Add( pGridSizer, 1, wxEXPAND, 5 );

	wxBoxSizer* pButtonsSizer;
	pButtonsSizer = new wxBoxSizer( wxHORIZONTAL );


	pButtonsSizer->Add( 0, 0, 1, wxEXPAND, 5 );

	m_cmdUpdate = new wxButton( this, wxID_ANY, _("Update"), wxDefaultPosition, wxDefaultSize, 0 );
	m_cmdUpdate->SetDefault();
	pButtonsSizer->Add( m_cmdUpdate, 0, wxALL|wxALIGN_BOTTOM, 5 );

	m_cmdClose = new wxButton( this, wxID_ANY, _("Close"), wxDefaultPosition, wxDefaultSize, 0 );
	pButtonsSizer->Add( m_cmdClose, 0, wxALL|wxALIGN_BOTTOM, 5 );


	pButtonsSizer->Add( 0, 0, 1, wxEXPAND, 5 );

	pMainSizer->Add( pButtonsSizer, 0, wxEXPAND, 5 );

	this->SetSizer( pMainSizer );
	this->Layout();

	// Connect Events
	m_cmdUpdate->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgSpacingParams::on_update ), nullptr, this );
	m_cmdClose->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgSpacingParams::on_close ), nullptr, this );

    //load current values for spacing params
    *m_txtForce << m_force;
    *m_txtAlpha << m_alpha;
    *m_txtDmin << m_dmin;
}

//---------------------------------------------------------------------------------------
DlgSpacingParams::~DlgSpacingParams()
{
	// Disconnect Events
	m_cmdUpdate->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgSpacingParams::on_update ), nullptr, this );
	m_cmdClose->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgSpacingParams::on_close ), nullptr, this );
}

//---------------------------------------------------------------------------------------
void DlgSpacingParams::on_close(wxCommandEvent& WXUNUSED(event))
{
    update_params();
    Hide();
}

//---------------------------------------------------------------------------------------
void DlgSpacingParams::on_update(wxCommandEvent& WXUNUSED(event))
{
    update_params();
}

//---------------------------------------------------------------------------------------
void DlgSpacingParams::update_params()
{
    wxString tmp = m_txtForce->GetValue();
    if ( wxSscanf(tmp, _T("%f\n"), &m_force) != 1 )
    {
        wxMessageBox("Invalid number for force!");
        return;
    }
    tmp = m_txtAlpha->GetValue();
    if ( wxSscanf(tmp, _T("%f"), &m_alpha) != 1 )
    {
        wxMessageBox("Invalid number for alpha!");
        return;
    }
    tmp = m_txtDmin->GetValue();
    if ( wxSscanf(tmp, _T("%f"), &m_dmin) != 1 )
    {
        wxMessageBox("Invalid number for Dmin!");
        return;
    }

//    stringstream out;
//    out << "New values: force= " << m_force
//        << ", alpha= " << m_alpha;
//        << ", Dmin= " << m_dmin;
//    wxMessageBox(out.str());

    MainFrame* pMainFrame = static_cast<MainFrame*>( GetParent() );
    pMainFrame->update_spacing_params(m_force, m_alpha, m_dmin);
}


}   // namespace lenmus
