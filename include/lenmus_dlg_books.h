//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2010-2015 LenMus project
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
//---------------------------------------------------------------------------------------

#ifndef __LENMUS_DLG_BOOKS_H__        //to avoid nested includes
#define __LENMUS_DLG_BOOKS_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_injectors.h"

//wxWidgets
#include <wx/dialog.h>
#include <wx/html/htmlwin.h>        //GCC complains if forward declaration of wxHtmlWindow !

//forward declarations
class wxButton;
class wxWindow;
class wxFileName;

namespace lenmus
{


//---------------------------------------------------------------------------------------
class BooksDlg : public wxDialog
{
protected:
    ApplicationScope& m_appScope;
	wxHtmlWindow*   m_pHtmlWnd;
	wxButton*       m_pBtnAccept;
    wxWindow*       m_pParent;
    wxString        m_fullName;

public:
	BooksDlg(wxWindow* pParent, ApplicationScope& appScope);
	~BooksDlg();

    void CreateControls();
    void load_available_books();

    //overrides
    void OnLinkClicked(wxHtmlLinkEvent& event);

    // event handlers
    void OnClose(wxCommandEvent& WXUNUSED(event));

    //access to information
    inline wxString get_full_path() { return m_fullName; }


protected:
	wxString get_book_name(wxFileName& oFilename);
    void show_html_document(const wxString& sDocName);


    wxDECLARE_EVENT_TABLE();
};


}   // namespace lenmus

#endif    // __LENMUS_DLG_BOOKS_H__
