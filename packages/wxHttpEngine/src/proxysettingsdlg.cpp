/////////////////////////////////////////////////////////////////////////////
// Name:        proxysettingsdlg.cpp
// Purpose:     wxProxySettingsDlg
// Author:      Angelo Mandato
// Created:     2005/08/11
// RCS-ID:      $Id: proxysettingsdlg.cpp,v 1.2 2005/08/12 21:43:46 amandato Exp $
// Copyright:   (c) 2001-2005 Angelo Mandato
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////



#ifdef __GNUG__
#pragma implementation "proxysettingsdlg.h"
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
	#include <wx/wx.h>
#endif

#include <wx/proxysettingsdlg.h>

enum
{
    // menu items
    ProxySettings_RequiresAuthentication = 1,
};


BEGIN_EVENT_TABLE(wxProxySettingsDlg, wxDialog)
    EVT_CHECKBOX(ProxySettings_RequiresAuthentication,  wxProxySettingsDlg::OnRequiresAuthentication)
END_EVENT_TABLE()

//! wxProxySettingsDlg constructor
wxProxySettingsDlg::wxProxySettingsDlg(wxWindow* parent, wxWindowID id, const wxString& caption, const wxString message, const bool bShowNoProxyFor)
: wxDialog(parent, id, caption, wxDefaultPosition, wxDefaultSize)
{
  m_stExceptionsDesc = NULL;
  m_stExceptionsNote = NULL;

  wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );
  wxBoxSizer *hsizer = NULL;
  wxButton *bn = NULL;
  wxStaticBox *sb = NULL;
  wxStaticBoxSizer *sbSizer = NULL;
  wxStaticText *st = NULL;

  if( message.IsEmpty() == false )
  {
    st = new wxStaticText(this, -1, message );
    sizer->Add(st, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 10);
  }

  // Settings
  sb = new wxStaticBox( this, -1, _T("Server Settings") );
    // StaticBox
    sbSizer = new wxStaticBoxSizer(sb, wxVERTICAL);
      // Host row
      hsizer = new wxBoxSizer( wxHORIZONTAL );
      st = new wxStaticText( this, -1, _T("Host:"), wxDefaultPosition, wxSize(100, -1) );
      hsizer->Add(st, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 10);
      m_tcHost = new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition, wxSize( 180, -1) );
      hsizer->Add(m_tcHost, 0, wxLEFT, 5);
    sbSizer->Add(hsizer, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND, 5);
      // Port row
      hsizer = new wxBoxSizer( wxHORIZONTAL );
      st = new wxStaticText( this, -1, _T("Port:"), wxDefaultPosition, wxSize(100, -1) );
      hsizer->Add(st, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 10);
      m_tcPort = new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition, wxSize( 180, -1) );
      hsizer->Add(m_tcPort, 0, wxLEFT, 5);

    sbSizer->Add(hsizer, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND, 5);
  sizer->Add(sbSizer, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 10);


  // Authentication
  sb = new wxStaticBox( this, -1, _T("Authentication") );
    // StaticBox
    sbSizer = new wxStaticBoxSizer(sb, wxVERTICAL);
      m_cbAuthenticate = new wxCheckBox( this, ProxySettings_RequiresAuthentication, _T("Proxy Server Requires Authentication") );
      sbSizer->Add(m_cbAuthenticate, 0, wxTOP|wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND, 10);
      // Host row
      hsizer = new wxBoxSizer( wxHORIZONTAL );
      st = new wxStaticText( this, -1, _T("Username:"), wxDefaultPosition, wxSize(100, -1) );
      hsizer->Add(st, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 10);
      m_tcUserName = new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition, wxSize( 180, -1) );
      m_tcUserName->Enable(false);
      hsizer->Add(m_tcUserName, 0, wxLEFT, 5);
    sbSizer->Add(hsizer, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND, 5);
      // Port row
      hsizer = new wxBoxSizer( wxHORIZONTAL );
      st = new wxStaticText( this, -1, _T("Password:"), wxDefaultPosition, wxSize(100, -1) );
      hsizer->Add(st, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 10);
      m_tcPassword = new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition, wxSize( 180, -1), wxTE_PASSWORD );
      hsizer->Add(m_tcPassword, 0, wxLEFT, 5);
      m_tcPassword->Enable(false);
    sbSizer->Add(hsizer, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND, 5);
  sizer->Add(sbSizer, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 10);

  // No Proxy for
  if( bShowNoProxyFor )
  {
    sb = new wxStaticBox( this, -1, _T("Exceptions") );
      // StaticBox
      sbSizer = new wxStaticBoxSizer(sb, wxVERTICAL);

      hsizer = new wxBoxSizer( wxHORIZONTAL );
      m_stExceptionsDesc = new wxStaticText( this, -1, _T("No proxy for addresses containing:"), wxDefaultPosition, wxDefaultSize );
      hsizer->Add(m_stExceptionsDesc, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 10);
    sbSizer->Add(hsizer, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND, 5);
      
      hsizer = new wxBoxSizer( wxHORIZONTAL );
      m_tcExceptions = new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition, wxDefaultSize );
      //hsizer->Add(5, 0);
      hsizer->Add(m_tcExceptions, 1, wxLEFT|wxALIGN_CENTER_VERTICAL, 10);
    sbSizer->Add(hsizer, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND, 5);

      hsizer = new wxBoxSizer( wxHORIZONTAL );
      m_stExceptionsNote = new wxStaticText( this, -1, _T("Separate addresses with commas."), wxDefaultPosition, wxDefaultSize );
      hsizer->Add(m_stExceptionsNote, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 10);
    sbSizer->Add(hsizer, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND, 5);

    sizer->Add(sbSizer, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 10);
  }

  // Button row:
  hsizer = new wxBoxSizer( wxHORIZONTAL );
    bn = new wxButton(this, wxID_OK, _T("&OK"));
    hsizer->Add(bn);
    hsizer->Add(0,0,1, wxGROW);
    bn = new wxButton(this, wxID_CANCEL, _T("&Cancel") );
    hsizer->Add(bn);
  sizer->Add(hsizer, 0, wxALL|wxEXPAND, 10);

  SetSizerAndFit( sizer );
  CenterOnParent();
}

//! wxProxySettingsDlg destructor
wxProxySettingsDlg::~wxProxySettingsDlg()
{
  // Empty
}

//! Set proxy port number
void wxProxySettingsDlg::SetPortNumber(const long port)
{
  m_tcPort->SetValue( wxString::Format(_T("%ld"), port) );
}

//! Set Proxy host
void wxProxySettingsDlg::SetHost(const wxString &host)
{
  m_tcHost->SetValue( host );
}

//! Set proxy username
void wxProxySettingsDlg::SetUsername(const wxString &username)
{
  m_tcUserName->SetValue( username );
}

//! set proxy password
void wxProxySettingsDlg::SetPassword(const wxString &password)
{
  m_tcPassword->SetValue( password );
}

//! Set exceptions list as string
void wxProxySettingsDlg::SetExceptions(const wxString &exceptions)
{
  m_tcExceptions->SetValue( exceptions );
}

void wxProxySettingsDlg::SetProxySettings(const wxProxySettings &settings)
{
  m_settings = settings;
  SetPortNumber(m_settings.m_nProxyPort);
  SetExceptions(m_settings.m_strProxyExceptions);
  SetHost(m_settings.m_strProxyHostname);
  SetUsername(m_settings.m_strProxyUsername);
  SetPassword(m_settings.m_strProxyPassword);
  SetAuthProxy(m_settings.m_bRequiresAuth);
}

//! Get proxy port number
long wxProxySettingsDlg::GetPortNumber()
{
  long port;
  m_tcPort->GetValue().ToLong(&port);
  return port;
}

//! Get proxy host
wxString wxProxySettingsDlg::GetHost()
{
  return m_tcHost->GetValue();
}

// Get proxy username
wxString wxProxySettingsDlg::GetUsername()
{
  return m_tcUserName->GetValue();
}

// Get Proxy password
wxString wxProxySettingsDlg::GetPassword()
{
  return m_tcPassword->GetValue();
}

//! Get exceptions string
wxString wxProxySettingsDlg::GetExceptions()
{
  return m_tcExceptions->GetValue();
}

wxProxySettings wxProxySettingsDlg::GetProxySettings(void)
{
  m_settings.m_nProxyPort = GetPortNumber();
  m_settings.m_strProxyExceptions = GetExceptions();
  m_settings.m_strProxyHostname = GetHost();
  m_settings.m_strProxyUsername = GetUsername();
  m_settings.m_strProxyPassword = GetPassword();
  m_settings.m_bRequiresAuth = IsAuthProxy();
  return m_settings;
}

//! Set proxy requires authentication (requires username and password)
void wxProxySettingsDlg::SetAuthProxy(const bool auth)
{
  m_cbAuthenticate->SetValue(auth);
  if( auth )
  {
    m_tcUserName->Enable(true);
    m_tcPassword->Enable(true);
  }
  else
  {
    m_tcUserName->Enable(false);
    m_tcPassword->Enable(false);
  }
}

//! Return true if proxy requires authentication
bool wxProxySettingsDlg::IsAuthProxy()
{
  return m_cbAuthenticate->GetValue();
}

//! Set description to describe what constitutes as exceptions host for your application
void wxProxySettingsDlg::SetExceptionsDesc(const wxString &text)
{
  if( m_stExceptionsDesc )
    m_stExceptionsDesc->SetLabel(text);
}

//! Additional information which may be an example or note
void wxProxySettingsDlg::SetExceptionsNote(const wxString &text)
{
  if( m_stExceptionsNote )
    m_stExceptionsNote->SetLabel(text);
}

//! Command event when proxy requires authentication.  Toggles the necessary text boxes enabled/disabled
void wxProxySettingsDlg::OnRequiresAuthentication(wxCommandEvent &event)
{
  if( event.IsChecked() )
  {
    m_tcUserName->Enable(true);
    m_tcPassword->Enable(true);
  }
  else
  {
    m_tcUserName->Enable(false);
    m_tcPassword->Enable(false);
  }
}

// eof
