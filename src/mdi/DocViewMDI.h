/////////////////////////////////////////////////////////////////////////////
// Name:        docmdi.h
// Purpose:     Frame classes for MDI document/view applications
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id: docmdi.h,v 1.21 2006/09/05 20:45:00 VZ Exp $
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __LM_DOCVIEWMDI_H__
#define __LM_DOCVIEWMDI_H__

#include "wx/defs.h"

#include "wx/docview.h"
#include "NotebookMDI.h"

/*
 * Use this instead of lmMDIParentFrame
 */

class lmDocMDIParentFrame: public lmMDIParentFrame
{
public:
    lmDocMDIParentFrame();
    lmDocMDIParentFrame(wxDocManager *manager, wxFrame *parent, wxWindowID id,
        const wxString& title, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = wxT("frame"));

    bool Create(wxDocManager *manager, wxFrame *parent, wxWindowID id,
        const wxString& title, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = wxT("frame"));

    // Extend event processing to search the document manager's event table
    virtual bool ProcessEvent(wxEvent& event);

    wxDocManager *GetDocumentManager(void) const { return m_docManager; }

    void OnExit(wxCommandEvent& event);
    void OnMRUFile(wxCommandEvent& event);
    void OnCloseWindow(wxCloseEvent& event);

protected:
    void Init();
    wxDocManager *m_docManager;

private:
    DECLARE_CLASS(lmDocMDIParentFrame)
    DECLARE_EVENT_TABLE()
    DECLARE_NO_COPY_CLASS(lmDocMDIParentFrame)
};


class lmDocMDIChildFrame: public lmMDIChildFrame
{
public:
    lmDocMDIChildFrame();
    lmDocMDIChildFrame(wxDocument *doc, wxView *view, lmMDIParentFrame *frame, wxWindowID id,
        const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
        long type = wxDEFAULT_FRAME_STYLE, const wxString& name = wxT("frame"));
    virtual ~lmDocMDIChildFrame();

    bool Create(wxDocument *doc,
                wxView *view,
                lmMDIParentFrame *frame,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long type = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxFrameNameStr);

    // Extend event processing to search the view's event table
    virtual bool ProcessEvent(wxEvent& event);

    void OnActivate(wxActivateEvent& event);
    void OnCloseWindow(wxCloseEvent& event);

    inline wxDocument *GetDocument() const { return m_childDocument; }
    inline wxView *GetView(void) const { return m_childView; }
    inline void SetDocument(wxDocument *doc) { m_childDocument = doc; }
    inline void SetView(wxView *view) { m_childView = view; }
    bool Destroy() { m_childView = (wxView *)NULL; return lmMDIChildFrame::Destroy(); }

protected:
    void Init();
    wxDocument*       m_childDocument;
    wxView*           m_childView;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_CLASS(lmDocMDIChildFrame)
    DECLARE_NO_COPY_CLASS(lmDocMDIChildFrame)
};


#endif      // __LM_DOCVIEWMDI_H__
