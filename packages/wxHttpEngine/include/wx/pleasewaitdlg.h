/////////////////////////////////////////////////////////////////////////////
// Name:        pleasewaitdlg.h
// Purpose:     Please Wait dialog with cancel button for use in gui applications.
// Author:      Angelo Mandato
// Created:     2005/08/10
// RCS-ID:      $Id: pleasewaitdlg.h,v 1.2 2005/08/12 03:58:52 amandato Exp $
// Copyright:   (c) 2005 Angelo Mandato (http://www.spaceblue.com)
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PLEASEWAITDLG_H_
#define _WX_PLEASEWAITDLG_H_

// optimization for GCC
#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "pleasewaitdlg.h"
#endif

#include <wx/dialog.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/utils.h>
#include <wx/httpenginedef.h>

/*
 * wxPleaseWaitDlg flags
 */
#define wxPW_APP_MODAL          0x0001
#define wxPW_CAN_CANCEL         0x0002
#define wxPW_SHOW_NOW           0x0004

class WXDLLIMPEXP_HTTPENGINE wxPleaseWaitDlg : public wxDialog
{
public:
	void ReenableOtherWindows(void);
	void Completed(void);
	void UpdateMessage(const wxString &message);
	virtual bool Update( const wxString &newmsg = wxEmptyString);
	void Resume(void);
	void EnableCancel(const bool enable = true);
  void DisableCancel(void) { EnableCancel(false); };


	void OnClose( wxCloseEvent &event);
	void OnCancel( wxCommandEvent &event );
  wxPleaseWaitDlg(const wxString &title, wxString const &message,
                    wxWindow *parent = NULL,
                    int style = wxPW_APP_MODAL|wxPW_SHOW_NOW|wxDEFAULT_DIALOG_STYLE,
                    wxSize minSize = wxSize(200, 60));

  ~wxPleaseWaitDlg();

private:

  enum
  {
    Uncancelable = -1,   // dialog can't be canceled
    Canceled,            // can be cancelled and, in fact, was
    Continue,            // can be cancelled but wasn't
    Finished             // finished, waiting to be removed from screen
  } m_state;

  wxWindowDisabler  *m_winDisabler;
  wxStaticText      *m_msg;
  wxWindow          *m_parentTop;
  wxButton          *m_btnCancel;

  DECLARE_EVENT_TABLE()
};

#endif
