//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2010 LenMus project
//
//    This file is derived from file src/generic/mdig.h from wxWidgets 2.7.1 project.
//    Author:       Hans Van Leemputten
//    Copyright (c) Hans Van Leemputten
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

#ifndef __LM_CLIENTWINDOW_H__
#define __LM_CLIENTWINDOW_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ClientWindow.cpp"
#endif

#include <wx/aui/auibook.h>

class lmTDIParentFrame;
class lmTDIChildFrame;

//-----------------------------------------------------------------------------
// lmTDIClientWindow
//-----------------------------------------------------------------------------

class lmTDIClientWindow: public wxAuiNotebook
{
public:
    lmTDIClientWindow();
    lmTDIClientWindow( lmTDIParentFrame *parent, long style = 0 );
    virtual ~lmTDIClientWindow();

    lmTDIChildFrame* GetSelectedPage();
    int SetSelection(size_t nPage);
    void OnSize(wxSizeEvent& event);
    void OnChildClose(wxAuiNotebookEvent& evt);

protected:
	//event handlers
	void OnPageChanged(wxAuiNotebookEvent& event);

private:
    DECLARE_DYNAMIC_CLASS(lmTDIClientWindow)
    DECLARE_EVENT_TABLE()
};


#endif      // __LM_CLIENTWINDOW_H__
