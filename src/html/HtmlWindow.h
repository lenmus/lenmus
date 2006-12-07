// RCS-ID: $Id: HtmlWindow.h,v 1.3 2006/02/23 19:21:06 cecilios Exp $
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
/*! @file HtmlWindow.h
    @brief Header file for class lmHtmlWindow
    @ingroup html_management
*/
#ifndef __SB_HTMLWINDOW_H__
#define __SB_HTMLWINDOW_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "HtmlWindow.h"
#endif

#include "wx/html/htmlwin.h"

class lmHtmlWindow : public wxHtmlWindow
{
public:
    lmHtmlWindow() : wxHtmlWindow() {}
    lmHtmlWindow(wxWindow *parent, wxWindowID id = wxID_ANY,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxHW_DEFAULT_STYLE | wxSIMPLE_BORDER ,
                 const wxString& name = wxT("htmlWindow"))
        : wxHtmlWindow(parent, id, pos, size, style, name) {}
    ~lmHtmlWindow() {}

    // -- overrides --

    // Called when user clicked on hypertext link. The idea is to intercept and 
    // deal with user commands
    void OnLinkClicked(const wxHtmlLinkInfo& link);

    //AQUI_FALLA
    void OnSetTitle(const wxString& title) {}

};



#endif // __SB_HTMLWINDOW_H__

