//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
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
/*! @file TextBookController.h
    @brief Header file for class lmTextBookController
    @ingroup html_management
*/
#ifndef __TEXTBOOKCONTROLLER_H__
#define __TEXTBOOKCONTROLLER_H__

#include "wx/defs.h"

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "TextBookController.h"
#endif

#if wxUSE_WXHTML_HELP

#include "wx/helpbase.h"
#include "TextBookFrame.h"        //CSG_MODIFIED    wx/html/helpfrm.h"

#define wxID_HTML_HELPFRAME   (wxID_HIGHEST + 1)

class WXDLLIMPEXP_HTML lmTextBookController : public wxHelpControllerBase // wxEvtHandler
{
    DECLARE_DYNAMIC_CLASS(lmTextBookController)

public:
    lmTextBookController(int style = wxHF_DEFAULT_STYLE);
    virtual ~lmTextBookController();

    void SetTitleFormat(const wxString& format);
    void SetTempDir(const wxString& path) { m_helpData.SetTempDir(path); }
    bool AddBook(const wxString& book_url, bool show_wait_msg = false);
    bool AddBook(const wxFileName& book_file, bool show_wait_msg = false);

    bool Display(const wxString& x);
    bool Display(int id);
    bool DisplayContents();
    bool DisplayIndex();
    bool KeywordSearch(const wxString& keyword,
                       wxHelpSearchMode mode = wxHELP_SEARCH_ALL);

    lmTextBookFrame* GetFrame() { return m_helpFrame; }
    void UseConfig(wxConfigBase *config, const wxString& rootpath = wxEmptyString);

    // Assigns config object to the Ctrl. This config is then
    // used in subsequent calls to Read/WriteCustomization of both help
    // Ctrl and it's wxHtmlWindow
    virtual void ReadCustomization(wxConfigBase *cfg, const wxString& path = wxEmptyString);
    virtual void WriteCustomization(wxConfigBase *cfg, const wxString& path = wxEmptyString);

    //// Backward compatibility with wxHelpController API

    virtual bool Initialize(const wxString& file, int WXUNUSED(server) ) { return Initialize(file); }
    virtual bool Initialize(const wxString& file);
    virtual void SetViewer(const wxString& WXUNUSED(viewer), long WXUNUSED(flags) = 0) {}
    virtual bool LoadFile(const wxString& file = wxT(""));
    virtual bool DisplaySection(int sectionNo);
    virtual bool DisplaySection(const wxString& section) { return Display(section); }
    virtual bool DisplayBlock(long blockNo) { return DisplaySection(blockNo); }
    virtual bool DisplayTextPopup(const wxString& text, const wxPoint& pos);

    virtual void SetFrameParameters(const wxString& title,
                               const wxSize& size,
                               const wxPoint& pos = wxDefaultPosition,
                               bool newFrameEachTime = false);
    /// Obtains the latest settings used by the help frame and the help
    /// frame.
    virtual wxFrame *GetFrameParameters(wxSize *size = NULL,
                               wxPoint *pos = NULL,
                               bool *newFrameEachTime = NULL);

    // Get direct access to help data:
    wxHtmlHelpData *GetHelpData() { return &m_helpData; }

    virtual bool Quit() ;
    virtual void OnQuit();                      //CSG_MODIFIED: implemented

    void OnCloseFrame(wxCloseEvent& evt);

    // Make the help controller's frame 'modal' if
    // needed
    void AddGrabIfNeeded();

protected:
    virtual lmTextBookFrame* CreateHelpFrame(wxHtmlHelpData *data);

    virtual void CreateHelpWindow();
    virtual void DestroyHelpWindow();

    wxHtmlHelpData      m_helpData;
    lmTextBookFrame*    m_helpFrame;
    wxConfigBase *      m_Config;
    wxString            m_ConfigRoot;
    wxString            m_titleFormat;
    int                 m_FrameStyle;
    
    //DECLARE_EVENT_TABLE()

    DECLARE_NO_COPY_CLASS(lmTextBookController)
};

#endif // wxUSE_WXHTML_HELP

#endif // __TEXTBOOKCONTROLLER_H__
