/////////////////////////////////////////////////////////////////////////////
// Name:        pleasewaitdlg.cpp
// Purpose:     wxPleaseWaitDlg
// Author:      Angelo Mandato
// Created:     2005/08/11
// RCS-ID:      $Id: pleasewaitdlg.cpp,v 1.1 2005/08/12 04:12:49 amandato Exp $
// Copyright:   (c) 2001-2005 Angelo Mandato
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "pleasewaitdlg.h"
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
	#include <wx/sizer.h>
	#include <wx/string.h>
	#include <wx/tooltip.h>
#endif

#include <wx/pleasewaitdlg.h>

BEGIN_EVENT_TABLE(wxPleaseWaitDlg, wxDialog)
    EVT_BUTTON(wxID_CANCEL, wxPleaseWaitDlg::OnCancel)
    EVT_CLOSE(wxPleaseWaitDlg::OnClose)
END_EVENT_TABLE()

//! wxPleaseWaitDlg constructor
wxPleaseWaitDlg::wxPleaseWaitDlg(const wxString &title, wxString const &message,
                    wxWindow *parent, int style, wxSize minSize  )

                    : wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxCAPTION)
{
  // we may disappear at any moment, let the others know about it
  SetExtraStyle(GetExtraStyle() | wxWS_EX_TRANSIENT);
  m_windowStyle |= style;

  m_winDisabler = NULL;
  m_msg = NULL;
  m_btnCancel = NULL;
  m_parentTop = wxGetTopLevelParent(parent);

  wxYieldIfNeeded();

  wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

  wxBoxSizer *sizerRow = new wxBoxSizer(wxHORIZONTAL);

  m_msg = new wxStaticText(this, wxID_ANY, message);
  sizerRow->Add(m_msg, 1, wxTOP | wxBOTTOM, 8 );
  sizer->Add(sizerRow, 1, wxLEFT | wxRIGHT, 8 );

  if ( style & wxPW_CAN_CANCEL )
  {
    m_state = Continue;

    sizerRow = new wxBoxSizer(wxHORIZONTAL);
    m_btnCancel = new wxButton(this, wxID_CANCEL, _T("Cancel"));
    sizerRow->Add(m_btnCancel, 0, wxBOTTOM, 8 );
    sizer->Add(sizerRow, 0, wxLEFT | wxRIGHT|wxALIGN_CENTER, 8 );
  }
  else
  {
    m_state = Uncancelable;
  }

  sizer->SetMinSize(minSize);
  SetSizerAndFit(sizer);

  CentreOnParent();

  if ( style & wxPW_APP_MODAL )
  {
    m_winDisabler = new wxWindowDisabler(this);
  }
  else
  {
    if ( m_parentTop )
      m_parentTop->Disable();
    m_winDisabler = NULL;
  }

  if ( style & wxPW_SHOW_NOW )
  {
    Show();
    Enable();
    wxYieldIfNeeded();
    Update();
  }
}

//! wxPleaseWaitDlg destructor
wxPleaseWaitDlg::~wxPleaseWaitDlg()
{
  ReenableOtherWindows();
}

//! On cancel event
void wxPleaseWaitDlg::OnCancel(wxCommandEvent &event)
{
  if ( m_state == Finished )
  {
    // this means that the count down is already finished and we're being
    // shown as a modal dialog - so just let the default handler do the job
    event.Skip();
  }
  else
  {
    m_state = Canceled;
    DisableCancel();
  }
}

//! On close event
void wxPleaseWaitDlg::OnClose(wxCloseEvent &event)
{
  if ( m_state == Uncancelable )
  {
    // can't close this dialog
    event.Veto();
  }
  else if ( m_state == Finished )
  {
    // let the default handler close the window as we already terminated
    event.Skip();
  }
  else
  {
    // next Update() will notice it
    m_state = Canceled;
    DisableCancel();
  }
}

//! Enable cancel button in dialog
void wxPleaseWaitDlg::EnableCancel(const bool enable)
{
  if( m_state != Uncancelable && m_state != Canceled && m_btnCancel )
  {
    m_btnCancel->Enable(enable);
  }
}

//! Resume recently cancel press
void wxPleaseWaitDlg::Resume()
{
  m_state = Continue;
  EnableCancel(true);
}

//! Update the displayed message in the dialog and refresh screen
bool wxPleaseWaitDlg::Update(const wxString &newmsg)
{
  if( !newmsg.IsEmpty() )
  {
    UpdateMessage(newmsg);
  }

  wxYieldIfNeeded();
  wxDialog::Update();

  return( m_state != Canceled);
}

//! Update the displayed message in the dialog
void wxPleaseWaitDlg::UpdateMessage(const wxString &message)
{
  if ( message != m_msg->GetLabel() )
  {
    m_msg->SetLabel(message);
    Fit();
  }
}

//! Set dialog to completed state
void wxPleaseWaitDlg::Completed(void)
{
  if( m_state != Finished )
  {
    m_state = Finished;
    ReenableOtherWindows();
    Hide();
    wxDialog::Update();
  }
}

//! Reenable the window which created this dialog.
void wxPleaseWaitDlg::ReenableOtherWindows()
{
  if ( GetWindowStyle() & wxPW_APP_MODAL )
  {
    delete m_winDisabler;
    m_winDisabler = (wxWindowDisabler *)NULL;
  }
  else
  {
    if ( m_parentTop )
      m_parentTop->Enable();
  }
}

// eof
