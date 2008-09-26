//--------------------------------------------------------------------------------------
//    LenMus project: free software for music theory and language
//    Copyright (c) 2002-2008 Cecilio Salmeron
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
    void OnToggleUTF8(wxCommandEvent& WXUNUSED(event));

private:
    void PutContentIntoFile(wxString sPath, wxString sContent);
    void GenerateLanguage(int i);
    void LogMessage(const wxChar* szFormat, ...);

	wxTextCtrl*     m_pText;
    wxString        m_sRootPath;
    wxString        m_sLenMusPath;
    wxMenu*         m_pOptMenu;

    DECLARE_EVENT_TABLE()
};

    
#endif    // __LT_MAINFRAME_H__
