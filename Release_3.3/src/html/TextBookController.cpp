//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
//
//    This file is a modified copy of file helpctrl.cpp from wxWidgets 2.6.2 project.
//    wxWidgets licence is compatible with GNU GPL.
//    Author:      Harm van der Heijden and Vaclav Slavik
//    Copyright (c) Harm van der Heijden and Vaclav Slavik
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
/*! @file TextBookController.cpp
    @brief Implementation file for class lmTextBookController
    @ingroup html_management
*/
//===============================================================================
/*
    Code copied from wxWidgets, file helpctrl.cpp

    Things deleted are commented with //CSG_DELETED and, if applicable, a comment with
    additional explanation

    Things modified are marked as //CSG_MODIFIED and an explanation

    Global changes:
      -    TextBookHelpFrame changed to lmTextBookFrame
*/
//===============================================================================

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "TextBookController.h"        //CSG_MODIFIED
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if wxUSE_WXHTML_HELP

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/intl.h"
#endif // WX_PRECOMP

#include "TextBookController.h"                //CSG_MODIFIED
#include "wx/busyinfo.h"

#ifdef __WXGTK__
    // for the hack in AddGrabIfNeeded()
    #include "wx/dialog.h"
#endif // __WXGTK__

#if wxUSE_HELP
    #include "wx/tipwin.h"
#endif

#include "../app/MainFrame.h"           //CSG_ADDED
extern lmMainFrame* g_pMainFrame;       //CSG_ADDED


#if wxUSE_LIBMSPACK
#include "wx/html/forcelnk.h"
FORCE_LINK(wxhtml_chm_support)
#endif


IMPLEMENT_DYNAMIC_CLASS(lmTextBookController, wxHelpControllerBase)


lmTextBookController::lmTextBookController(int style)
{
    m_helpFrame = NULL;
    m_Config = NULL;
    m_ConfigRoot = wxEmptyString;
    m_titleFormat = _("Help: %s");
    m_FrameStyle = style;
}

lmTextBookController::~lmTextBookController()
{
    if (m_Config)
        WriteCustomization(m_Config, m_ConfigRoot);
    if (m_helpFrame)
        DestroyHelpWindow();
}


void lmTextBookController::DestroyHelpWindow()
{
    //if (m_Config) WriteCustomization(m_Config, m_ConfigRoot);
    if (m_helpFrame)
        m_helpFrame->Destroy();
}

void lmTextBookController::OnCloseFrame(wxCloseEvent& evt)
{
    evt.Skip();

    OnQuit();

    m_helpFrame->SetController((wxHelpControllerBase*) NULL);
    m_helpFrame = NULL;
}

void lmTextBookController::SetTitleFormat(const wxString& title)
{
    m_titleFormat = title;
    if (m_helpFrame)
        m_helpFrame->SetTitleFormat(title);
}


bool lmTextBookController::AddBook(const wxFileName& book_file, bool show_wait_msg)
{
    return AddBook(wxFileSystem::FileNameToURL(book_file), show_wait_msg);
}

bool lmTextBookController::AddBook(const wxString& book, bool show_wait_msg)
{
    wxBusyCursor cur;
#if wxUSE_BUSYINFO
    wxBusyInfo* busy = NULL;
    wxString info;
    if (show_wait_msg)
    {
        info.Printf(_("Adding book %s"), book.c_str());
        busy = new wxBusyInfo(info);
    }
#endif
    bool retval = m_helpData.AddBook(book);
#if wxUSE_BUSYINFO
    if (show_wait_msg)
        delete busy;
#else
    wxUnusedVar(show_wait_msg);
#endif
    if (m_helpFrame)
        m_helpFrame->RefreshLists();
    return retval;
}



lmTextBookFrame *lmTextBookController::CreateHelpFrame(wxHtmlHelpData *data)
{
    return new lmTextBookFrame(data);
}


void lmTextBookController::CreateHelpWindow()
{
    if (m_helpFrame)
    {
        m_helpFrame->Raise();
        return ;
    }

    if (m_Config == NULL)
    {
        m_Config = wxConfigBase::Get(false);
        if (m_Config != NULL)
            m_ConfigRoot = _T("wxWindows/lmTextBookController");
    }

    m_helpFrame = CreateHelpFrame(&m_helpData);
    m_helpFrame->SetController(this);

    if (m_Config)
        m_helpFrame->UseConfig(m_Config, m_ConfigRoot);

    //CSG modified to make frame MDIChild
    //m_helpFrame->Create(NULL, wxID_HTML_HELPFRAME, wxEmptyString, m_FrameStyle);
    m_helpFrame->Create(g_pMainFrame, wxID_HTML_HELPFRAME, wxEmptyString, m_FrameStyle);

    m_helpFrame->SetTitleFormat(m_titleFormat);

    m_helpFrame->Show(true);
}

void lmTextBookController::ReadCustomization(wxConfigBase* cfg, const wxString& path)
{
    /* should not be called by the user; call UseConfig, and the controller
     * will do the rest */
    if (m_helpFrame && cfg)
        m_helpFrame->ReadCustomization(cfg, path);
}

void lmTextBookController::WriteCustomization(wxConfigBase* cfg, const wxString& path)
{
    /* typically called by the controllers OnCloseFrame handler */
    if (m_helpFrame && cfg)
        m_helpFrame->WriteCustomization(cfg, path);
}

void lmTextBookController::UseConfig(wxConfigBase *config, const wxString& rootpath)
{
    m_Config = config;
    m_ConfigRoot = rootpath;
    if (m_helpFrame) m_helpFrame->UseConfig(config, rootpath);
    ReadCustomization(config, rootpath);
}

//// Backward compatibility with wxHelpController API

bool lmTextBookController::Initialize(const wxString& file)
{
    wxString dir, filename, ext;
    wxSplitPath(file, & dir, & filename, & ext);

    if (!dir.empty())
        dir = dir + wxFILE_SEP_PATH;

    // Try to find a suitable file
    wxString actualFilename = dir + filename + wxString(wxT(".zip"));
    if (!wxFileExists(actualFilename))
    {
        actualFilename = dir + filename + wxString(wxT(".htb"));
        if (!wxFileExists(actualFilename))
        {
            actualFilename = dir + filename + wxString(wxT(".hhp"));
            if (!wxFileExists(actualFilename))
            {
#if wxUSE_LIBMSPACK
                actualFilename = dir + filename + wxString(wxT(".chm"));
                if (!wxFileExists(actualFilename))
#endif
                    return false;
            }
        }
    }
    return AddBook(wxFileName(actualFilename));
}

bool lmTextBookController::LoadFile(const wxString& WXUNUSED(file))
{
    // Don't reload the file or we'll have it appear again, presumably.
    return true;
}

bool lmTextBookController::DisplaySection(int sectionNo)
{
    return Display(sectionNo);
}

bool lmTextBookController::DisplayTextPopup(const wxString& text, const wxPoint& WXUNUSED(pos))
{
#if wxUSE_TIPWINDOW
    static wxTipWindow* s_tipWindow = NULL;

    if (s_tipWindow)
    {
        // Prevent s_tipWindow being nulled in OnIdle,
        // thereby removing the chance for the window to be closed by ShowHelp
        s_tipWindow->SetTipWindowPtr(NULL);
        s_tipWindow->Close();
    }
    s_tipWindow = NULL;

    if ( !text.empty() )
    {
        s_tipWindow = new wxTipWindow(wxTheApp->GetTopWindow(), text, 100, & s_tipWindow);

        return true;
    }
#else
    wxUnusedVar(text);
#endif // wxUSE_TIPWINDOW

    return false;
}


bool lmTextBookController::Quit()
{
    DestroyHelpWindow();
    return true;
}

// Make the help controller's frame 'modal' if
// needed
void lmTextBookController::AddGrabIfNeeded()
{
    // So far, wxGTK only
#ifdef __WXGTK__
    bool needGrab = false;

    // Check if there are any modal windows present,
    // in which case we need to add a grab.
    for ( wxWindowList::compatibility_iterator node = wxTopLevelWindows.GetFirst();
          node;
          node = node->GetNext() )
    {
        wxWindow *win = node->GetData();
        wxDialog *dialog = wxDynamicCast(win, wxDialog);

        if (dialog && dialog->IsModal())
            needGrab = true;
    }

    if (needGrab && m_helpFrame)
        m_helpFrame->AddGrab();
#endif // __WXGTK__
}

bool lmTextBookController::Display(const wxString& x)
{
    //wxHtmlBookRecArray m_bookRecords = m_helpData.GetBookRecArray();
    //int i, cnt = m_bookRecords.GetCount();
    //for (i = 0; i < cnt; i++)
    //{
    //    wxLogMessage(_T("Paginas = %s"), m_bookRecords[i].GetFullPath(x));
    //    wxLogMessage(_T("Libros = %s"), m_bookRecords[i].GetTitle());
    //}

    CreateHelpWindow();
    bool success = m_helpFrame->Display(x);
    AddGrabIfNeeded();
    return success;
}

bool lmTextBookController::Display(int id)
{
    CreateHelpWindow();
    bool success = m_helpFrame->Display(id);
    AddGrabIfNeeded();
    return success;
}

bool lmTextBookController::DisplayContents()
{
    CreateHelpWindow();
    bool success = m_helpFrame->DisplayContents();
    AddGrabIfNeeded();
    return success;
}

bool lmTextBookController::DisplayIndex()
{
    CreateHelpWindow();
    bool success = m_helpFrame->DisplayIndex();
    AddGrabIfNeeded();
    return success;
}

bool lmTextBookController::KeywordSearch(const wxString& keyword,
                                         wxHelpSearchMode mode)
{
    CreateHelpWindow();
    bool success = m_helpFrame->KeywordSearch(keyword, mode);
    AddGrabIfNeeded();
    return success;
}

//------------------------------------------------------------------------------------
//CSG_ADDED:
//  - implemented to un-press 'Open book' toolbar button
//------------------------------------------------------------------------------------

void lmTextBookController::OnQuit()
{
    g_pMainFrame->SetOpenBookButton(false);
}


#endif // wxUSE_WXHTML_HELP

