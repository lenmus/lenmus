//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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

#ifndef __LM_PARENTFRAME_H__
#define __LM_PARENTFRAME_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ParentFrame.cpp"
#endif

#include "../app/global.h"      //access to lmUSE_NOTEBOOK_MDI flag

#if lmUSE_NOTEBOOK_MDI

#include "wx/frame.h"


class lmMDIClientWindow;
class lmMDIChildFrame;

//-----------------------------------------------------------------------------
// lmMDIParentFrame
//-----------------------------------------------------------------------------

class lmMDIParentFrame: public wxFrame
{
public:
    lmMDIParentFrame();
    lmMDIParentFrame(wxWindow *parent,
                     wxWindowID winid,
                     const wxString& title,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
                     const wxString& name = wxFrameNameStr);

    virtual ~lmMDIParentFrame();
    bool Create( wxWindow *parent,
                 wxWindowID winid,
                 const wxString& title,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
                 const wxString& name = wxFrameNameStr );


    virtual void SetMenuBar(wxMenuBar *pMenuBar);
    virtual bool ProcessEvent(wxEvent& event);

    lmMDIChildFrame *GetActiveChild() const;
    inline void SetActiveChild(lmMDIChildFrame* pChildFrame);

    lmMDIClientWindow *GetClientWindow() const;

    virtual void ActivateNext();
    virtual void ActivatePrevious();
    virtual bool CloseAll();
    virtual void CloseActive();

    virtual void RemoveChildFrame(lmMDIChildFrame* pChild);
    void OnSize(wxSizeEvent& event);

protected:
    void Init();
    virtual void DoGetClientSize(int *width, int *height) const;


    lmMDIClientWindow   *m_pClientWindow;
    lmMDIChildFrame     *m_pActiveChild;
    wxMenuBar           *m_pMyMenuBar;

private:
    DECLARE_DYNAMIC_CLASS(lmMDIParentFrame)
    DECLARE_EVENT_TABLE()
};

#else   // do not lmUSE_NOTEBOOK_MDI

//use standard wxWidgets classes
#define lmMDIParentFrame wxMDIParentFrame

#endif  //lmUSE_NOTEBOOK_MDI


#endif      // __LM_PARENTFRAME_H__
