/////////////////////////////////////////////////////////////////////////////
// Name:        authdlg.cpp
// Purpose:     HTTP Authentication dialog for use in gui applications
// Author:      Angelo Mandato
// Created:     2005/08/11
// RCS-ID:      $Id: authdlg.cpp,v 1.10 2005/08/13 20:22:38 amandato Exp $
// Copyright:   (c) 2002-2005 Angelo Mandato
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "authdlg.h"
#endif


// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// includes
#ifndef WX_PRECOMP
	// here goes the #include <wx/abc.h> directives for those
	// files which are not included by wxprec.h
	#include <wx/sizer.h>
	#include <wx/statbox.h>
#endif

#include <wx/authdlg.h>

//! Simple Authentication dialog for use in gui applications
//! Use this to retrieve HTTP Authentication from user when
//! web server requires basic authentication.

//! Constructor
wxAuthenticateDlg::wxAuthenticateDlg(wxWindow* parent, wxWindowID id, const wxString &caption, const wxString &message, const bool &showRememberPasswd )
: wxDialog(parent, id, caption, wxDefaultPosition, wxDefaultSize)
{
  m_settings.SetBasicAuth(); // Default to basic authentication
  m_cbRememberPassword = NULL;
  wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );
  wxBoxSizer *hsizer = NULL;
  wxButton *bn = NULL;
  wxStaticText *st = NULL;

  if( message.IsEmpty() == false )
  {
    st = new wxStaticText(this, -1, message );
    sizer->Add(st, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 20);
  }

  sizer->Add(0, 10);

  // Username row
  hsizer = new wxBoxSizer( wxHORIZONTAL );
    st = new wxStaticText( this, -1, _T("Username:"), wxDefaultPosition, wxSize(100, -1) );
    hsizer->Add(st, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 10);
    m_tcUsername = new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition, wxSize( 180, -1) );
    hsizer->Add(m_tcUsername, 0, wxLEFT, 5);
  sizer->Add(hsizer, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 10);
  
  // Password row
  hsizer = new wxBoxSizer( wxHORIZONTAL );
    st = new wxStaticText( this, -1, _T("Password:"), wxDefaultPosition, wxSize(100, -1) );
    hsizer->Add(st, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 10);
    m_tcPassword = new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition, wxSize( 180, -1), wxTE_PASSWORD );
    hsizer->Add(m_tcPassword, 0, wxLEFT, 5);
  sizer->Add(hsizer, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 10);

  if( showRememberPasswd )
  {
    hsizer = new wxBoxSizer( wxHORIZONTAL );
      //st = new wxStaticText( this, -1, _T("Password:"), wxDefaultPosition, wxSize(80, -1) );
      hsizer->Add(100, 0, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 10);
      m_cbRememberPassword = new wxCheckBox(this, -1, _T("Remember password") );
      hsizer->Add(m_cbRememberPassword, 0, wxLEFT, 5);
    sizer->Add(hsizer, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 10);
  }
  
  sizer->Add(0, 10);
  hsizer = new wxBoxSizer( wxHORIZONTAL );
    bn = new wxButton(this, wxID_OK, _T("&OK") );
    hsizer->Add(bn);
    hsizer->Add(0,0,1, wxGROW);
    bn = new wxButton(this, wxID_CANCEL, _T("&Cancel") );
    hsizer->Add(bn);
  sizer->Add(hsizer, 0, wxALL|wxEXPAND, 10);

  SetSizerAndFit( sizer );
  CenterOnParent();
}

//! Destructor
wxAuthenticateDlg::~wxAuthenticateDlg()
{
  // Empty
}

//! Retrieve the password entered
wxString wxAuthenticateDlg::GetPassword(void)
{
  return m_tcPassword->GetValue();
}

//! Get username entered
wxString wxAuthenticateDlg::GetUsername(void)
{
  return m_tcUsername->GetValue();
}

//! Returns true if Remember Password option is checked.
bool wxAuthenticateDlg::GetRememberPassword(void)
{
  if( m_cbRememberPassword )
    return m_cbRememberPassword->GetValue();

  return false;
}

wxHTTPAuthSettings wxAuthenticateDlg::GetAuthSettings()
{
  m_settings.m_bRememberPasswd = GetRememberPassword();
  m_settings.m_strAuthUsername = GetUsername();
  m_settings.m_strAuthPassword = GetPassword();
  return m_settings;
}

//! Sets password in dialog
void wxAuthenticateDlg::SetPassword( const wxString &password )
{
  m_tcPassword->SetValue(password);
}

//! Sets username in dialog
void wxAuthenticateDlg::SetUsername( const wxString &username )
{
  m_tcUsername->SetValue(username);
}

//! Set remember password checkbox in dialog
void wxAuthenticateDlg::SetRememberPassword( const bool &remember )
{
  if( m_cbRememberPassword )
    m_cbRememberPassword->SetValue(remember);
}

void wxAuthenticateDlg::SetAuthSettings(const wxHTTPAuthSettings &settings)
{
  m_settings = settings;
  SetRememberPassword(settings.m_bRememberPasswd);
  SetUsername(settings.m_strAuthUsername);
  SetPassword(settings.m_strAuthPassword);
}

// eof
