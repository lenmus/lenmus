//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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

#ifndef __LM_PARENTFRAME_H__
#define __LM_PARENTFRAME_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ParentFrame.cpp"
#endif

#include "wx/frame.h"


class lmTDIClientWindow;
class lmTDIChildFrame;

//-----------------------------------------------------------------------------
// lmTDIParentFrame
//-----------------------------------------------------------------------------

class lmTDIParentFrame: public wxFrame
{
public:
    lmTDIParentFrame();
    lmTDIParentFrame(wxWindow *parent,
                     wxWindowID winid,
                     const wxString& title,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
                     const wxString& name = wxFrameNameStr);

    virtual ~lmTDIParentFrame();
    bool Create( wxWindow *parent,
                 wxWindowID winid,
                 const wxString& title,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
                 const wxString& name = wxFrameNameStr );


    virtual void SetMenuBar(wxMenuBar *pMenuBar);
    virtual bool ProcessEvent(wxEvent& event);

    lmTDIChildFrame *GetActiveChild() const;
    inline void SetActiveChild(lmTDIChildFrame* pChildFrame);

    lmTDIClientWindow *GetClientWindow() const;

    virtual void ActivateNext();
    virtual void ActivatePrevious();
    virtual bool CloseAll();
    virtual void CloseActive();

    virtual void RemoveChildFrame(lmTDIChildFrame* pChild);
    void OnSize(wxSizeEvent& event);

protected:
    void Init();
    virtual void DoGetClientSize(int *width, int *height) const;


    lmTDIClientWindow   *m_pClientWindow;
    lmTDIChildFrame     *m_pActiveChild;
    wxMenuBar           *m_pMyMenuBar;

private:
    DECLARE_DYNAMIC_CLASS(lmTDIParentFrame)
    DECLARE_EVENT_TABLE()
};

#endif      // __LM_PARENTFRAME_H__
