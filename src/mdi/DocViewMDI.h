//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
//
//    This file is derived from file src/docmdi.cpp from wxWidgets 2.7.1 project.
//    Author:       Julian Smart
//    Copyright (c) Julian Smart
// 
//    Modified by:
//        Cecilio Salmeron
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
//-------------------------------------------------------------------------------------
#ifndef __LM_DOCVIEWMDI_H__
#define __LM_DOCVIEWMDI_H__

#include "wx/defs.h"
#include "wx/docview.h"

#include "ChildFrame.h"
#include "ParentFrame.h"
#include "ClientWindow.h"

#include <list>


//------------------------------------------------------------------------------------------------
// lmDocManager 
//------------------------------------------------------------------------------------------------
class lmEditorMode;
class lmScore;
class lmDocument;

class lmDocManager : public wxDocManager
{
public:
    lmDocManager(long flags = wxDEFAULT_DOCMAN_FLAGS, bool initialize = true);
    ~lmDocManager();

    //overrides
    wxDocument* CreateDocument(const wxString& path, long flags);


    //specific methods

    void ImportFile(wxString& sPath);
    void OpenFile(wxString& sPath, bool fAsNew);
    void OpenDocument(lmEditorMode* pMode, lmScore* pScore);

protected:
    lmDocument* DoOpenDocument(const wxString& path, long flags, lmScore* pScore=NULL);

};



//------------------------------------------------------------------------------------------------
// lmDocTDIParentFrame: top window for a Tabbed Document Interface
//------------------------------------------------------------------------------------------------
class lmDocTDIParentFrame: public lmTDIParentFrame
{
public:
    lmDocTDIParentFrame();
    lmDocTDIParentFrame(lmDocManager* pDocManager, wxFrame* pParent, wxWindowID id,
        const wxString& title, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = wxT("frame"));

    bool Create(lmDocManager* pDocManager, wxFrame* pParent, wxWindowID id,
        const wxString& title, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = wxT("frame"));

    // Extend event processing to search the document manager's event table
    virtual bool ProcessEvent(wxEvent& event);

    lmDocManager* GetDocumentManager(void) const { return m_pDocManager; }

    void OnExit(wxCommandEvent& event);
    void OnMRUFile(wxCommandEvent& event);
    void OnCloseWindow(wxCloseEvent& event);



protected:
    void Init();

    lmDocManager*   m_pDocManager;

private:
    DECLARE_CLASS(lmDocTDIParentFrame)
    DECLARE_EVENT_TABLE()
    DECLARE_NO_COPY_CLASS(lmDocTDIParentFrame)
};



//------------------------------------------------------------------------------------------------
// lmDocTDIChildFrame 
//------------------------------------------------------------------------------------------------
class lmDocTDIChildFrame: public lmTDIChildFrame
{
public:
    lmDocTDIChildFrame();
    lmDocTDIChildFrame(wxDocument *doc, wxView *view, lmTDIParentFrame *frame, wxWindowID id,
        const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
        long type = wxDEFAULT_FRAME_STYLE, const wxString& name = wxT("frame"));
    virtual ~lmDocTDIChildFrame();

    bool Create(wxDocument *doc,
                wxView *view,
                lmTDIParentFrame *frame,
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
    bool Destroy() { m_childView = (wxView *)NULL; return lmTDIChildFrame::Destroy(); }

protected:
    void Init();
    wxDocument*       m_childDocument;
    wxView*           m_childView;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_CLASS(lmDocTDIChildFrame)
    DECLARE_NO_COPY_CLASS(lmDocTDIChildFrame)
};

#endif      // __LM_DOCVIEWMDI_H__

