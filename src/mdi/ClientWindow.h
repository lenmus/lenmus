//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
//
//    This file is derived from file src/generic/mdig.h from wxWidgets 2.7.1 project.
//    Author:       Hans Van Leemputten
//    Copyright (c) Hans Van Leemputten
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

#ifndef __LM_CLIENTWINDOW_H__
#define __LM_CLIENTWINDOW_H__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "ClientWindow.cpp"
#endif

#include "../app/global.h"      //access to lmUSE_NOTEBOOK_MDI flag

#if lmUSE_NOTEBOOK_MDI

#include "wx/aui/auibook.h"

class lmMDIParentFrame;
class lmMDIChildFrame;

//-----------------------------------------------------------------------------
// lmMDIClientWindow
//-----------------------------------------------------------------------------

class lmMDIClientWindow: public wxAuiNotebook
{
public:
    lmMDIClientWindow();
    lmMDIClientWindow( lmMDIParentFrame *parent, long style = 0 );
    virtual ~lmMDIClientWindow();

    lmMDIChildFrame* GetSelectedPage();
    int SetSelection(size_t nPage);
    void OnSize(wxSizeEvent& event);
    void OnChildClose(wxAuiNotebookEvent& evt);

protected:
	//event handlers
	void OnPageChanged(wxAuiNotebookEvent& event);

private:
    DECLARE_DYNAMIC_CLASS(lmMDIClientWindow)
    DECLARE_EVENT_TABLE()
};


#else   // do not lmUSE_NOTEBOOK_MDI

//use standard wxWidgets classes
#define lmMDIClientWindow wxMDIClientWindow

#endif  //lmUSE_NOTEBOOK_MDI


#endif      // __LM_CLIENTWINDOW_H__
