/////////////////////////////////////////////////////////////////////////////
// Name:        docmdi.cpp
// Purpose:     Frame classes for MDI document/view applications
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id: docmdi.cpp,v 1.26 2005/09/23 12:52:49 MR Exp $
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif


#include "DocViewMDI.h"

/*
 * Docview MDI parent frame
 */

IMPLEMENT_CLASS(lmDocMDIParentFrame, lmMDIParentFrame)

BEGIN_EVENT_TABLE(lmDocMDIParentFrame, lmMDIParentFrame)
    EVT_MENU(wxID_EXIT, lmDocMDIParentFrame::OnExit)
    EVT_MENU_RANGE(wxID_FILE1, wxID_FILE9, lmDocMDIParentFrame::OnMRUFile)
    EVT_CLOSE(lmDocMDIParentFrame::OnCloseWindow)
END_EVENT_TABLE()

lmDocMDIParentFrame::lmDocMDIParentFrame()
{
    Init();
}

lmDocMDIParentFrame::lmDocMDIParentFrame(wxDocManager *manager, wxFrame *frame, wxWindowID id, const wxString& title,
  const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    Init();
    Create(manager, frame, id, title, pos, size, style, name);
}

bool lmDocMDIParentFrame::Create(wxDocManager *manager, wxFrame *frame, wxWindowID id, const wxString& title,
  const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    m_docManager = manager;
    return lmMDIParentFrame::Create(frame, id, title, pos, size, style, name);
}

void lmDocMDIParentFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
    Close();
}

void lmDocMDIParentFrame::Init()
{
    m_docManager = NULL;
}

void lmDocMDIParentFrame::OnMRUFile(wxCommandEvent& event)
{
    wxString f(m_docManager->GetHistoryFile(event.GetId() - wxID_FILE1));
    if (!f.empty())
        (void)m_docManager->CreateDocument(f, wxDOC_SILENT);
}

// Extend event processing to search the view's event table
bool lmDocMDIParentFrame::ProcessEvent(wxEvent& event)
{
    // Try the document manager, then do default processing
    if (!m_docManager || !m_docManager->ProcessEvent(event))
        return wxEvtHandler::ProcessEvent(event);
    else
        return true;
}

void lmDocMDIParentFrame::OnCloseWindow(wxCloseEvent& event)
{
  if (m_docManager->Clear(!event.CanVeto()))
  {
    this->Destroy();
  }
  else
    event.Veto();
}


/*
 * Default document child frame for MDI children
 */

IMPLEMENT_CLASS(lmDocMDIChildFrame, lmMDIChildFrame)

BEGIN_EVENT_TABLE(lmDocMDIChildFrame, lmMDIChildFrame)
    EVT_ACTIVATE(lmDocMDIChildFrame::OnActivate)
    EVT_CLOSE(lmDocMDIChildFrame::OnCloseWindow)
END_EVENT_TABLE()

void lmDocMDIChildFrame::Init()
{
    m_childDocument = (wxDocument*)  NULL;
    m_childView = (wxView*) NULL;
}

lmDocMDIChildFrame::lmDocMDIChildFrame()
{
    Init();
}

lmDocMDIChildFrame::lmDocMDIChildFrame(wxDocument *doc, wxView *view, lmMDIParentFrame *frame, wxWindowID  id,
  const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    Init();
    Create(doc, view, frame, id, title, pos, size, style, name);
}

bool lmDocMDIChildFrame::Create(wxDocument *doc, wxView *view, lmMDIParentFrame *frame, wxWindowID  id,
  const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    m_childDocument = doc;
    m_childView = view;
    if (lmMDIChildFrame::Create(frame, id, title, pos, size, style, name))
    {
        if (view)
            view->SetFrame(this);
        return true;
    }

    return false;
}

lmDocMDIChildFrame::~lmDocMDIChildFrame(void)
{
    m_childView = (wxView *) NULL;
}

// Extend event processing to search the view's event table
bool lmDocMDIChildFrame::ProcessEvent(wxEvent& event)
{
    static wxEvent *ActiveEvent = NULL;

    // Break recursion loops
    if (ActiveEvent == &event)
        return false;

    ActiveEvent = &event;

    bool ret;
    if ( !m_childView || ! m_childView->ProcessEvent(event) )
    {
        // Only hand up to the parent if it's a menu command
        if (!event.IsKindOf(CLASSINFO(wxCommandEvent)) || !GetParent() || !GetParent()->ProcessEvent(event))
            ret = wxEvtHandler::ProcessEvent(event);
        else
            ret = true;
    }
    else
        ret = true;

    ActiveEvent = NULL;
    return ret;
}

void lmDocMDIChildFrame::OnActivate(wxActivateEvent& event)
{
  if (event.GetActive() && m_childView)
    m_childView->Activate(event.GetActive());
}

void lmDocMDIChildFrame::OnCloseWindow(wxCloseEvent& event)
{
  // Close view but don't delete the frame while doing so!
  // ...since it will be deleted by wxWidgets if we return true.
  if (m_childView)
  {
    bool ans = event.CanVeto()
                ? m_childView->Close(false) // false means don't delete associated window
                : true; // Must delete.

    if (ans)
    {
      m_childView->Activate(false);
      delete m_childView;
      m_childView = (wxView *) NULL;
      m_childDocument = (wxDocument *) NULL;

      this->Destroy();
    }
    else
        event.Veto();
  }
  else
    event.Veto();
}

