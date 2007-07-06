//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "TextBookController.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/intl.h"
#endif // WX_PRECOMP

#include "TextBookController.h"
#include "TextBookFrame.h"
#include "wx/busyinfo.h"

#ifdef __WXGTK__
    // for the hack in AddGrabIfNeeded()
    #include "wx/dialog.h"
#endif // __WXGTK__

#if wxUSE_HELP
    #include "wx/tipwin.h"
#endif

#include "../app/MainFrame.h"
extern lmMainFrame* g_pMainFrame;


#if wxUSE_LIBMSPACK
#include "wx/html/forcelnk.h"
FORCE_LINK(wxhtml_chm_support)
#endif


IMPLEMENT_DYNAMIC_CLASS(lmTextBookController, wxObject)


lmTextBookController::lmTextBookController()
{
    m_pBookFrame = NULL;
    m_Config = NULL;
    m_ConfigRoot = wxEmptyString;
    m_titleFormat = _T("eMusicBooks");  //_("Help: %s");
    m_FrameStyle = wxHF_TOOLBAR | wxHF_CONTENTS | wxHF_BOOKMARKS | wxHF_PRINT; //wxHF_DEFAULT_STYLE;
}

lmTextBookController::~lmTextBookController()
{
    if (m_Config)
        WriteCustomization(m_Config, m_ConfigRoot);
    if (m_pBookFrame)
        DestroyBookFrame();
}


void lmTextBookController::DestroyBookFrame()
{
    if (m_pBookFrame)
        m_pBookFrame->Destroy();
}

void lmTextBookController::OnCloseFrame(wxCloseEvent& evt)
{
    evt.Skip();

    lmMainFrame* pMainFrame = (lmMainFrame*)m_pBookFrame->GetMDIParentFrame();

    //clear pointers
    m_pBookFrame->SetController((lmTextBookController*) NULL);
    m_pBookFrame = NULL;

    //inform parent
    pMainFrame->OnCloseBookFrame();

}

void lmTextBookController::SetTitleFormat(const wxString& title)
{
    m_titleFormat = title;
    if (m_pBookFrame)
        m_pBookFrame->SetTitleFormat(title);
}


bool lmTextBookController::AddBook(const wxString& book, bool show_wait_msg)
{
    wxFileName oFN(book);
    return AddBook(oFN, show_wait_msg);
}

bool lmTextBookController::AddBook(const wxFileName& book, bool show_wait_msg)
{
//    wxBusyCursor cur;
//#if wxUSE_BUSYINFO
//    wxBusyInfo* busy = NULL;
//    wxString info;
//    if (show_wait_msg)
//    {
//        info.Printf(_("Adding book %s"), book.GetFullPath().c_str());
//        busy = new wxBusyInfo(info);
//    }
//#endif
    bool retval = m_oBookData.AddBook(book);
//#if wxUSE_BUSYINFO
//    if (show_wait_msg)
//        delete busy;
//#else
    wxUnusedVar(show_wait_msg);
//#endif
    if (m_pBookFrame)
        m_pBookFrame->RefreshLists();
    return retval;
}



void lmTextBookController::CreateBookFrame()
{
    //Creates the  book frame. It is created when a request to display a page

    if (m_pBookFrame) return ;

    if (m_Config == NULL)
    {
        m_Config = wxConfigBase::Get(false);
        if (m_Config != NULL)
            m_ConfigRoot = _T("wxWindows/lmTextBookController");
    }

    m_pBookFrame = new lmTextBookFrame(&m_oBookData);
    m_pBookFrame->SetController(this);

    if (m_Config)
        m_pBookFrame->UseConfig(m_Config, m_ConfigRoot);

    m_pBookFrame->Create(g_pMainFrame, wxID_HTML_HELPFRAME, wxEmptyString, m_FrameStyle);

    m_pBookFrame->SetTitleFormat(m_titleFormat);

    m_pBookFrame->Show(true);
}

void lmTextBookController::ReadCustomization(wxConfigBase* cfg, const wxString& path)
{
    /* should not be called by the user; call UseConfig, and the controller
     * will do the rest */
    if (m_pBookFrame && cfg)
        m_pBookFrame->ReadCustomization(cfg, path);
}

void lmTextBookController::WriteCustomization(wxConfigBase* cfg, const wxString& path)
{
    /* typically called by the controllers OnCloseFrame handler */
    if (m_pBookFrame && cfg)
        m_pBookFrame->WriteCustomization(cfg, path);
}

void lmTextBookController::UseConfig(wxConfigBase *config, const wxString& rootpath)
{
    m_Config = config;
    m_ConfigRoot = rootpath;
    if (m_pBookFrame) m_pBookFrame->UseConfig(config, rootpath);
    ReadCustomization(config, rootpath);
}

bool lmTextBookController::Quit()
{
    DestroyBookFrame();
    return true;
}

// Make the help controller's frame 'modal' if
// needed
void lmTextBookController::AddGrabIfNeeded()
{
//    // So far, wxGTK only
//#ifdef __WXGTK__
//    bool needGrab = false;
//
//    // Check if there are any modal windows present,
//    // in which case we need to add a grab.
//    for ( wxWindowList::compatibility_iterator node = wxTopLevelWindows.GetFirst();
//          node;
//          node = node->GetNext() )
//    {
//        wxWindow *win = node->GetData();
//        wxDialog *dialog = wxDynamicCast(win, wxDialog);
//
//        if (dialog && dialog->IsModal())
//            needGrab = true;
//    }
//
//    if (needGrab && m_pBookFrame)
//        m_pBookFrame->AddGrab();
//#endif // __WXGTK__
}

bool lmTextBookController::Display(const wxString& x)
{
    //Creates the  book frame, if not created yet, and shows the requested file

    CreateBookFrame();
    bool fSuccess = m_pBookFrame->Display(x);
    AddGrabIfNeeded();
    return fSuccess;
}

bool lmTextBookController::Display(int id)
{
    //Creates the  book frame, if not created yet, and shows the requested file
    CreateBookFrame();
    bool fSuccess = m_pBookFrame->Display(id);
    AddGrabIfNeeded();
    return fSuccess;
}

bool lmTextBookController::DisplayContents()
{
    CreateBookFrame();
    bool success = m_pBookFrame->DisplayContents();
    AddGrabIfNeeded();
    return success;
}

bool lmTextBookController::DisplayIndex()
{
    CreateBookFrame();
    bool success = m_pBookFrame->DisplayIndex();
    AddGrabIfNeeded();
    return success;
}

bool lmTextBookController::KeywordSearch(const wxString& keyword,
                                         wxHelpSearchMode mode)
{
    CreateBookFrame();
    bool success = m_pBookFrame->KeywordSearch(keyword, mode);
    AddGrabIfNeeded();
    return success;
}

