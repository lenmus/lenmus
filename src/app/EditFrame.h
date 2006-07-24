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
/*! @file EditFrame.h
    @brief Header file for class lmEditFrame
    @ingroup app_gui
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef _EDITFRAME_H        //to avoid nested includes
#define _EDITFRAME_H

#include "wx/docmdi.h"
#include "wx/docview.h"

class lmMainFrame;
class lmScoreDocument;
class lmScoreView;

class lmEditFrame: public wxDocMDIChildFrame
{
    DECLARE_DYNAMIC_CLASS(lmEditFrame)
public:
    lmEditFrame(wxDocument* doc, wxView* view, lmMainFrame* mainFrame, wxPoint& pos, wxSize& size);

    // event handlers
    void OnSize(wxSizeEvent& WXUNUSED(event));
    void OnClose(wxCloseEvent& event);

private:
    lmScoreView*        m_pView;    //the owner view of this frame

    DECLARE_EVENT_TABLE()
};

#endif    // _EDITFRAME_H
