//--------------------------------------------------------------------------------------
//    LenMus project: free software for music theory and language
//    Copyright (c) 2002-2009 Cecilio Salmeron
//
//    This program is free software; you can redistribute it and/or modify it under the 
//    terms of the GNU General Public License as published by the Free Software Foundation;
//    either version 3 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this 
//    program. If not, see <http://www.gnu.org/licenses/>. 
//
//
//    For any comment, suggestion or feature request, please contact the manager of 
//    the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------

#ifndef __LT_MAINFRAME_H__        //to avoid nested includes
#define __LT_MAINFRAME_H__

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "agdoc/agdoc_basics.h"

class ltMainFrame : public wxFrame
{
public:
    // ctor(s)
    ltMainFrame(const wxString& title, const wxString& sRootPath);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnInstaller(wxCommandEvent& event);
    void OnGeneratePO(wxCommandEvent& WXUNUSED(event));
    void OnCompileBook(wxCommandEvent& WXUNUSED(event));
    void OnCompileHelp(wxCommandEvent& WXUNUSED(event));
    void OnToggleUTF8(wxCommandEvent& WXUNUSED(event));
    void OnRunAgdoc(wxCommandEvent& WXUNUSED(event));

    void LogMessage(const wxChar* szFormat, ...);

private:
    void PutContentIntoFile(wxString sPath, wxString sContent);
    void GenerateLanguage(int i);

    //agdoc
    const char* find_file_name(int argc, char* argv[], int idx);
    void build_the_project(const char* project_name, bool index_only,
                           bool quick_mode);
    void write_index(const char* file_name, 
                     agdoc::content_storage& index, 
                     agdoc::log_file& log);

                 
    wxTextCtrl*     m_pText;
    wxString        m_sRootPath;
    wxString        m_sLenMusPath;
    wxMenu*         m_pOptMenu;
    wxString        m_sLastAgdocProject;

    DECLARE_EVENT_TABLE()
};

extern void ltLogMessage(const char* msg);
extern void ltLogMessage(const char* msg, const char* arg1);
extern void ltLogMessage(const char* msg, const char* arg1, const char* arg2);
extern void ltLogDbg(wxString& msg);

    
#endif    // __LT_MAINFRAME_H__
