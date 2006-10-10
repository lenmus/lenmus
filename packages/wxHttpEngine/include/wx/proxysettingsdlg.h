/////////////////////////////////////////////////////////////////////////////
// Name:        proxysettingsdlg.h
// Purpose:     Simple proxy settings dialog for use with gui and wxHTTPBuilder
// Author:      Angelo Mandato
// Created:     2005/08/10
// RCS-ID:      $Id: proxysettingsdlg.h,v 1.2 2005/08/12 21:40:15 amandato Exp $
// Copyright:   (c) 2005 Angelo Mandato (http://www.spaceblue.com)
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PROXYSETTINGSDLG_H_
#define _WX_PROXYSETTINGSDLG_H_

// optimization for GCC
#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "proxysettingsdlg.h"
#endif

#include <wx/dialog.h>
#include <wx/checklst.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/intl.h>
#include "wx/httpenginedef.h"
#include "wx/httpbuilder.h"

class WXDLLIMPEXP_HTTPENGINE wxProxySettingsDlg : public wxDialog
{

public:
	void OnRequiresAuthentication( wxCommandEvent &event );
	bool IsAuthProxy(void);
	void SetAuthProxy( const bool auth = true );

  // Properties
	wxString GetPassword(void);
	wxString GetUsername(void);
	wxString GetHost(void);
  long GetPortNumber(void);
  wxString  GetExceptions(void);
  wxProxySettings GetProxySettings(void);

	void SetPassword( const wxString &password );
	void SetUsername( const wxString &username );
	void SetHost( const wxString &host );
	void SetPortNumber( const long port );
  void SetExceptions( const wxString &exceptions );
  void SetProxySettings(const wxProxySettings &settings);

  // Additional settings:
  void SetExceptionsDesc(const wxString &text);
  void SetExceptionsNote(const wxString &text);

  wxProxySettingsDlg(wxWindow* parent, wxWindowID id, const wxString& caption = _T("Proxy Settings"), const wxString message = _T("Specify proxy settings"), const bool bShowNoProxyFor = false );
  virtual ~wxProxySettingsDlg();

private:

  wxTextCtrl        *m_tcHost;
  wxTextCtrl        *m_tcPort;
  wxCheckBox        *m_cbAuthenticate;
  wxTextCtrl        *m_tcUserName;
  wxTextCtrl        *m_tcPassword;

  wxTextCtrl        *m_tcExceptions;

  wxStaticText      *m_stExceptionsDesc;
  wxStaticText      *m_stExceptionsNote;

  wxProxySettings   m_settings;

  DECLARE_EVENT_TABLE()
};

#endif
