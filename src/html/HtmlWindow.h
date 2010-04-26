//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2010 LenMus project
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

#ifndef __LM_HTMLWINDOW_H__
#define __LM_HTMLWINDOW_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "HtmlWindow.cpp"
#endif

#include <wx/html/htmlwin.h>

class lmHtmlWindow : public wxHtmlWindow
{
public:
    lmHtmlWindow() : wxHtmlWindow() { m_rScale = 1.0; }
    lmHtmlWindow(wxWindow *parent, wxWindowID id = wxID_ANY,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxHW_DEFAULT_STYLE | wxBORDER_SIMPLE ,
                 const wxString& name = wxT("htmlWindow"))
        : wxHtmlWindow(parent, id, pos, size, style, name) { m_rScale = 1.0; }
    ~lmHtmlWindow() {}

    // -- overrides --

    // Called when user clicked on hypertext link. The idea is to intercept and
    // deal with user commands
    void OnLinkClicked(const wxHtmlLinkInfo& link);

    //In tabbed interface this method fails. I suppose it fails as it is not possible
    // to set up the frame title. So I override it to save the title but not change
    // the frame title
    void OnSetTitle(const wxString& title)
        {
            m_OpenedPageTitle = title;
        }

    // to deal with scale for scores
    void SetScale(double rScale) { m_rScale = rScale; }
    double GetScale() { return m_rScale; }


private:
    double      m_rScale;

};



#endif // __LM_HTMLWINDOW_H__

