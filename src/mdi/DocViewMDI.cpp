//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2010 LenMus project
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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "DocViewMDI.h"
#endif

// For compilers that support precompilation, includes <wx.h>.
#include <wx/wxprec.h>

#ifdef __BORLANDC__
  #pragma hdrstop
#endif


#include "DocViewMDI.h"
#include "../app/ScoreDoc.h"
#include <wx/filename.h>
#include "../score/Score.h"

#if lmUSE_LIBRARY_MVC
    #include "../app/TheApp.h"          //to get LibraryScope
    #include "lomse_mvc_builder.h"
    using namespace lomse;
#endif

/*
 * Docview MDI parent frame
 */

IMPLEMENT_CLASS(lmDocTDIParentFrame, lmTDIParentFrame)

BEGIN_EVENT_TABLE(lmDocTDIParentFrame, lmTDIParentFrame)
    EVT_MENU(wxID_EXIT, lmDocTDIParentFrame::OnExit)
    EVT_MENU_RANGE(wxID_FILE1, wxID_FILE9, lmDocTDIParentFrame::OnMRUFile)
    EVT_CLOSE(lmDocTDIParentFrame::OnCloseWindow)
END_EVENT_TABLE()

lmDocTDIParentFrame::lmDocTDIParentFrame()
{
    Init();
}

lmDocTDIParentFrame::lmDocTDIParentFrame(lmDocManager* pDocManager, wxFrame* pFrame, wxWindowID id, const wxString& title,
  const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    Init();
    Create(pDocManager, pFrame, id, title, pos, size, style, name);
}

bool lmDocTDIParentFrame::Create(lmDocManager* pDocManager, wxFrame* pFrame, wxWindowID id, const wxString& title,
  const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    m_pDocManager = pDocManager;
    return lmTDIParentFrame::Create(pFrame, id, title, pos, size, style, name);
}

void lmDocTDIParentFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
    Close();
}

void lmDocTDIParentFrame::Init()
{
    m_pDocManager = NULL;
}

void lmDocTDIParentFrame::OnMRUFile(wxCommandEvent& event)
{
    wxString f(m_pDocManager->GetHistoryFile(event.GetId() - wxID_FILE1));
    if (!f.empty())
        (void)m_pDocManager->CreateDocument(f, wxDOC_SILENT);
}

// Extend event processing to search the view's event table
bool lmDocTDIParentFrame::ProcessEvent(wxEvent& event)
{
    // Try the document manager, then do default processing
    if (!m_pDocManager || !m_pDocManager->ProcessEvent(event))
        return wxEvtHandler::ProcessEvent(event);
    else
        return true;
}

void lmDocTDIParentFrame::OnCloseWindow(wxCloseEvent& event)
{
  if (m_pDocManager->Clear(!event.CanVeto()))
  {
    this->Destroy();
  }
  else
    event.Veto();
}

/*
 * Default document child frame for MDI children
 */

IMPLEMENT_CLASS(lmDocTDIChildFrame, lmTDIChildFrame)

BEGIN_EVENT_TABLE(lmDocTDIChildFrame, lmTDIChildFrame)
    EVT_ACTIVATE(lmDocTDIChildFrame::OnActivate)
    EVT_CLOSE(lmDocTDIChildFrame::OnCloseWindow)
END_EVENT_TABLE()

void lmDocTDIChildFrame::Init()
{
    m_childDocument = (wxDocument*)  NULL;
    m_childView = (wxView*) NULL;
}

lmDocTDIChildFrame::lmDocTDIChildFrame()
{
    Init();
}

lmDocTDIChildFrame::lmDocTDIChildFrame(wxDocument *doc, wxView *view, lmTDIParentFrame *frame, wxWindowID  id,
  const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    Init();
    Create(doc, view, frame, id, title, pos, size, style, name);
}

bool lmDocTDIChildFrame::Create(wxDocument *doc, wxView *view, lmTDIParentFrame *frame, wxWindowID  id,
  const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    m_childDocument = doc;
    m_childView = view;
    if (lmTDIChildFrame::Create(frame, id, title, pos, size, style, name))
    {
        if (view)
            view->SetFrame(this);
        return true;
    }

    return false;
}

lmDocTDIChildFrame::~lmDocTDIChildFrame(void)
{
    m_childView = (wxView *) NULL;
}

// Extend event processing to search the view's event table
bool lmDocTDIChildFrame::ProcessEvent(wxEvent& event)
{
    static wxEvent *ActiveEvent = NULL;

    if (m_childView)
        m_childView->Activate(true);

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

void lmDocTDIChildFrame::OnActivate(wxActivateEvent& event)
{
    if (event.GetActive() && m_childView)
        m_childView->Activate(event.GetActive());
}

void lmDocTDIChildFrame::OnCloseWindow(wxCloseEvent& event)
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



//------------------------------------------------------------------------------------------------
// lmDocManager implementation
//------------------------------------------------------------------------------------------------

enum 
{
    lmDOC_OPEN = 0,         //Open an existing LDP score file
    lmDOC_OPEN_NEW,         //Open an LDP score file, but treat it as New
    lmDOC_LOAD,             //Create an lmDocument from an already created lmScore
    lmDOC_IMPORT,           //Import a MusicXML score file
};

lmDocManager::lmDocManager(long nFlags, bool fInitialize)
    : wxDocManager(nFlags, fInitialize)
{
#if lmUSE_LIBRARY_MVC
    m_pBuilder = new MvcBuilder(wxGetApp().library_scope());
#endif
}

lmDocManager::~lmDocManager()
{
    SaveRecentFiles();
#if lmUSE_LIBRARY_MVC
    delete m_pBuilder;
#endif
}

void lmDocManager::ImportFile(wxString& sPath)
{
    lmDocument* pDoc = DoOpenDocument(sPath, lmDOC_IMPORT);
    if (!pDoc)
    {
        OnOpenFileFailure();
        return;
    }

    //customize controller
    pDoc->OnCustomizeController( (lmEditorMode*)NULL );
}

void lmDocManager::OpenFile(wxString& sPath, bool fAsNew)
{
    lmDocument* pDoc = DoOpenDocument(sPath, (fAsNew ? lmDOC_OPEN_NEW : lmDOC_OPEN));
    if (!pDoc)
    {
        OnOpenFileFailure();
        return;
    }

    //customize controller
    pDoc->OnCustomizeController( (lmEditorMode*)NULL );
}

void lmDocManager::OpenDocument(lmEditorMode* pMode, lmScore* pScore)
{
    //Creates a new document (and its view/controller) in mode pMode

    wxASSERT(pScore);

    //create the document
    lmDocument* pDoc = DoOpenDocument(wxEmptyString, lmDOC_LOAD, pScore);
    if (!pDoc)
    {
        OnOpenFileFailure();
        return;
    }

    //customize controller
    pDoc->OnCustomizeController(pMode);
}

wxDocument* lmDocManager::CreateDocument(const wxString& path, long flags)
{
    //override to disable this method. lmDocument uses different methods so that we can:
    //  - deal with specific import & load score cases
    //  - deal with lmEditorMode, ToolBox and other MVC related issues

    WXUNUSED(path);
    WXUNUSED(flags);

    wxMessageBox(_T("[lmDocManager::CreateDocument] It should never arrive here!"));
    wxASSERT(false);

    return (wxDocument*)NULL;
}

lmDocument* lmDocManager::DoOpenDocument(const wxString& path, long nOperation, lmScore* pScore)
{
    //Replacement for base class CreateDocument() to:
    //  - deal with specific import & load score cases
    //  - deal with lmEditorMode, ToolBox and other MVC related issues

    //if file is already open, just activate its view and return the existing document
    for (size_t i = 0; i < GetDocuments().GetCount(); ++i)
    {
        lmDocument* pCurDoc = (lmDocument*)(GetDocuments().Item(i)->GetData());
#ifdef _LM_WINDOWS_
        //file paths are case-insensitive on Windows
        if (path.CmpNoCase(pCurDoc->GetFilename()) == 0)
#else
        if (path.Cmp(pCurDoc->GetFilename()) == 0)
#endif
        {
            //file already open. Just activate it and return
            if (pCurDoc->GetFirstView())
            {
                ActivateView(pCurDoc->GetFirstView(), true);
                if (pCurDoc->GetDocumentWindow())
                    pCurDoc->GetDocumentWindow()->SetFocus();
                return pCurDoc;
            }
        }
    }

    //the file is not currently open. Open it.

#if lmUSE_LIBRARY_MVC
    MvcElement* pMvc = m_pBuilder->new_document(MvcBuilder::k_edit_view);
    m_docviews.add(pMvc);
    Document* pDoc = pMvc->get_document();

    //if (nOperation == lmDOC_OPEN || nOperation == lmDOC_OPEN_NEW)
    //{
    //    const std::string sPath = lmToStdString(path);
    //    pDoc = m_pBuilder->open_document(sPath);
    //}
    //else if (nOperation == lmDOC_LOAD)
    //{
    //    pDoc = m_pBuilder->new_document();
    //}
    //else if (nOperation == lmDOC_IMPORT)
    //{
    //    ; //TODO
    //}
    //else
    //    wxASSERT(false);
#endif

    //create a new document
#if lmUSE_LIBRARY_MVC
    lmDocument *pNewDoc = new lmDocument(pDoc);
#else
    lmDocument *pNewDoc = new lmDocument();
#endif
    pNewDoc->SetFilename(path);
    wxDocTemplate* pTemplate = FindTemplateForPath(path);
    pNewDoc->SetDocumentTemplate(pTemplate);

    AddDocument(pNewDoc);
    pNewDoc->SetCommandProcessor( pNewDoc->OnCreateCommandProcessor() );

    //Ask the document to create the View/Controller
    if (!pNewDoc->OnCreate(path, nOperation))
    {
        //View/controller creation failed. Undo things
        pNewDoc->DeleteAllViews();      //this also deletes pNewDoc
        pNewDoc = (lmDocument*)NULL;
    }

    //Ask the document to open file and load content
    if (pNewDoc)
    {
        pNewDoc->SetDocumentName(pTemplate->GetDocumentName());  //It's not the name! It is the 'Type name'
        bool fOK = false;
        if (nOperation == lmDOC_OPEN || nOperation == lmDOC_OPEN_NEW)
            fOK = pNewDoc->OnOpenDocument(path);
        else if (nOperation == lmDOC_LOAD)
            fOK = pNewDoc->OnNewDocumentWithContent(pScore);
        else if (nOperation == lmDOC_IMPORT)
            fOK = pNewDoc->OnImportDocument(path);
        else
            wxASSERT(false);

        if (!fOK)
        {
            pNewDoc->DeleteAllViews();
            // delete pNewDoc; // Implicitly deleted by DeleteAllViews
            return (lmDocument*)NULL;
        }
        if (nOperation != lmDOC_OPEN_NEW && path != wxEmptyString)
            AddToHistory(path);
    }
    return pNewDoc;
}

#if lmUSE_LIBRARY_MVC

void lmDocManager::close_document(Document* pDoc)
{
    //the document has been closed. Remove it from the MvcCollection

    m_docviews.close_document(pDoc);
}

#endif


void lmDocManager::LoadRecentFiles(wxConfigBase* pConfig, const wxString& sKeyRoot)
{
    //wxFileHistory is not using a key to save the files, and this causes problems.
    //Therefore I will implement my own function

    //save data about config object and key to use 
    m_pConfig = pConfig;
    m_sConfigKey = sKeyRoot + _T("file%d");

    //file #1 is the newest one one and #9 the oldest one. Therefore, load 
    //them in reverse order
    for (int nFile = 9; nFile > 0; --nFile)
    {
        wxString sKey = wxString::Format(m_sConfigKey, nFile);
        wxString sFile = m_pConfig->Read(sKey, _T(""));
        if (!sFile.empty())
            AddToHistory(sFile);
    }
}

void lmDocManager::SaveRecentFiles()
{
    //wxFileHistory is not using a key to save the files, and this causes problems.
    //Therefore I will implement my own function

    //file #1 is the newest one
    int nNumFiles = m_RecentFiles.GetCount();
    for (int i = 1; i <= wxMin(nNumFiles, 9); ++i)
    {
        wxString buf;
        buf.Printf(m_sConfigKey, i);
        m_pConfig->Write(buf, m_RecentFiles.GetHistoryFile(i-1));
    }
}

void lmDocManager::FileHistoryUsesMenu(wxMenu* pMenu)
{
    m_RecentFiles.UseMenu(pMenu);
    m_RecentFiles.AddFilesToMenu(pMenu);
}

void lmDocManager::AddToHistory(const wxString& sFileWithPath)
{
    m_RecentFiles.AddFileToHistory(sFileWithPath);
}

wxString lmDocManager::GetFromHistory(int iEntry)
{
    return m_RecentFiles.GetHistoryFile(iEntry);
}
