//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2010 LenMus project
//
//    This file is a modified copy of file helpctrl.h from wxWidgets 2.6.2 project.
//    wxWidgets licence is compatible with GNU GPL.
//    Author:      Harm van der Heijden and Vaclav Slavik
//    Copyright (c) Harm van der Heijden and Vaclav Slavik
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

#ifndef __LM_TEXTBOOKCONTROLLER_H__
#define __LM_TEXTBOOKCONTROLLER_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "TextBookController.cpp"
#endif

#include <wx/defs.h>

#include "BookData.h"
#include "TextBookFrame.h"

#define wxID_HTML_HELPFRAME   (wxID_HIGHEST + 1)

class lmTextBookController : public wxObject
{
    DECLARE_DYNAMIC_CLASS(lmTextBookController)

public:
    lmTextBookController();
    virtual ~lmTextBookController();

    void SetTitleFormat(const wxString& format);
    void SetTempDir(const wxString& path) { m_oBookData.SetTempDir(path); }
    bool AddBook(const wxString& book_url, bool show_wait_msg = false);
    bool AddBook(const wxFileName& book_file, bool show_wait_msg = false);

    bool Display(const wxString& x);
    bool Display(int id);
    bool DisplayContents();
    bool DisplayIndex();
    bool KeywordSearch(const wxString& keyword,
                       wxHelpSearchMode mode = wxHELP_SEARCH_ALL);

    lmTextBookFrame* GetFrame() { return m_pBookFrame; }
    void UseConfig(wxConfigBase *config, const wxString& rootpath = wxEmptyString);

    // Assigns config object to the Ctrl. This config is then
    // used in subsequent calls to Read/WriteCustomization of both help
    // Ctrl and it's wxHtmlWindow
    virtual void ReadCustomization(wxConfigBase *cfg, const wxString& path = wxEmptyString);
    virtual void WriteCustomization(wxConfigBase *cfg, const wxString& path = wxEmptyString);

    // Get direct access to help data:
    lmBookData *GetHelpData() { return &m_oBookData; }

    bool Quit() ;
    void OnCloseFrame(wxCloseEvent& evt);


    // Make the help controller's frame 'modal' if needed
    void AddGrabIfNeeded();

protected:
    virtual void CreateBookFrame();
    virtual void DestroyBookFrame();

    lmBookData          m_oBookData;
    lmTextBookFrame*    m_pBookFrame;
    wxConfigBase *      m_Config;
    wxString            m_ConfigRoot;
    wxString            m_titleFormat;
    int                 m_FrameStyle;
    
    DECLARE_NO_COPY_CLASS(lmTextBookController)
};

#endif // __LM_TEXTBOOKCONTROLLER_H__
