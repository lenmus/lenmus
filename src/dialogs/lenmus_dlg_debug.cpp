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

#include "lenmus_dlg_debug.h"

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


namespace lenmus
{

// IDs for controls
enum
{
	lmID_HTML_WND = 3010,
	lmID_ACCEPT,
	lmID_SAVE,
};


BEGIN_EVENT_TABLE(DlgDebug, wxDialog)
   EVT_BUTTON(wxID_OK, DlgDebug::OnOK)
   EVT_BUTTON(lmID_SAVE, DlgDebug::OnSave)
END_EVENT_TABLE()

IMPLEMENT_CLASS(DlgDebug, wxDialog)

DlgDebug::DlgDebug(wxWindow * parent, wxString sTitle, wxString sData, bool fSave)
    : wxDialog(parent, -1, sTitle, wxDefaultPosition, wxSize(800, 430),
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

DlgDebug::~DlgDebug()
{
}

void DlgDebug::OnOK(wxCommandEvent& WXUNUSED(event))
{
   EndModal(wxID_OK);
}

void DlgDebug::AppendText(wxString sText)
{
    m_pTxtData->AppendText(sText);
}

void DlgDebug::OnSave(wxCommandEvent& WXUNUSED(event))
{
    wxString sFilename = wxFileSelector(_T("File to save"), _T(""), _T("debug"), _T("txt"),
                                        _T("*.*"),  wxFD_SAVE);
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

BEGIN_EVENT_TABLE(lmHtmlDlg, wxDialog)
    EVT_BUTTON(lmID_ACCEPT, lmHtmlDlg::OnAcceptClicked )
    EVT_BUTTON(lmID_SAVE, lmHtmlDlg::OnSaveClicked )

END_EVENT_TABLE()



lmHtmlDlg::lmHtmlDlg(wxWindow* pParent, const wxString& sTitle, bool fSaveButton)
    : wxDialog(pParent, wxID_ANY, sTitle, wxDefaultPosition, wxSize(600,400),
               wxDEFAULT_DIALOG_STYLE)
{
    // create the dialog controls
    CreateControls(fSaveButton);
    CentreOnScreen();
}

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

lmHtmlDlg::~lmHtmlDlg()
{
}

void lmHtmlDlg::OnAcceptClicked(wxCommandEvent& WXUNUSED(event))
{
   EndModal(wxID_OK);
}

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

void lmHtmlDlg::SetContent(const wxString& sContent)
{
    m_pHtmlWnd->SetPage(sContent);
}


}   // namespace lenmus
