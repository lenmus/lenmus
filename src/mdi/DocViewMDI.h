//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
//
//    This file is derived from file src/docmdi.cpp from wxWidgets 2.7.1 project.
//    Author:       Julian Smart
//    Copyright (c) Julian Smart
// 
//    Modified by:
//        Cecilio Salmeron
//
//    This program is free software; you can redistribute it and/or modify it under the 
//    terms of the GNU General Public License as published by the Free Software Foundation;
//    either version 2 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this 
//    program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, 
//    Fifth Floor, Boston, MA  02110-1301, USA.
//
//    For any comment, suggestion or feature request, please contact the manager of 
//    the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------
#ifndef __LM_DOCVIEWMDI_H__
#define __LM_DOCVIEWMDI_H__


#include "../app/global.h"

#if lmUSE_NOTEBOOK_MDI


#include "wx/defs.h"
#include "wx/docview.h"

#include "ChildFrame.h"
#include "ParentFrame.h"
#include "ClientWindow.h"


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


#else   // do not lmUSE_NOTEBOOK_MDI

//use standard wxWidgets classes
#define lmDocMDIParentFrame wxDocMDIParentFrame
#define lmDocMDIChildFrame wxDocMDIChildFrame


#endif  //lmUSE_NOTEBOOK_MDI


#endif      // __LM_DOCVIEWMDI_H__


