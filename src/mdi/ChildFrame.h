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

#ifndef __LM_CHILDFRAME_H__
#define __LM_CHILDFRAME_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ChildFrame.cpp"
#endif

#include <wx/panel.h>


class lmTDIParentFrame;
class lmTDIClientWindow;

//-----------------------------------------------------------------------------
// lmTDIChildFrame
// It is a panel on the Notebook (MDIParentFrame)
//-----------------------------------------------------------------------------

class lmTDIChildFrame: public wxPanel
{
public:
    lmTDIChildFrame();
    lmTDIChildFrame( lmTDIParentFrame *parent,
                     wxWindowID winid,
                     const wxString& title,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxDEFAULT_FRAME_STYLE,
                     const wxString& name = wxFrameNameStr );

    virtual ~lmTDIChildFrame();
    bool Create( lmTDIParentFrame *parent,
                 wxWindowID winid,
                 const wxString& title,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxDEFAULT_FRAME_STYLE,
                 const wxString& name = wxFrameNameStr );

    virtual void SetTitle(const wxString& title);
    virtual wxString GetTitle() const;

    virtual void Activate();


    // icons
    void SetIcon( const wxIcon& icon );
    void SetIcons( const wxIconBundle& WXUNUSED(icons) ) { }

    // no maximize etc
    virtual void Maximize( bool WXUNUSED(maximize) = true) { /* Has no effect */ }
    virtual void Restore() { /* Has no effect */ }
    virtual void Iconize(bool WXUNUSED(iconize)  = true) { /* Has no effect */ }
    virtual bool IsMaximized() const { return true; }
    virtual bool IsIconized() const { return false; }
    virtual bool ShowFullScreen(bool WXUNUSED(show), long WXUNUSED(style)) { return false; }
    virtual bool IsFullScreen() const { return false; }

    virtual bool IsTopLevel() const { return false; }


    // The next 2 are copied from top level...
    void OnCloseWindow(wxCloseEvent& event);
    void OnSize(wxSizeEvent& event);

    void SetMDIParentFrame(lmTDIParentFrame* parentFrame);
    lmTDIParentFrame* GetMDIParentFrame() const;

	//New mthods not in wxMDIChildFrame --------------------------
	
	// methods to deal with zooming
	virtual	double GetActiveViewScale() { return 1.0; }
	virtual bool SetActiveViewScale(double rScale) { return false; }

	// call back from lmTDIClientWindow::PageChanged
	virtual void OnChildFrameActivated() {}
    virtual void OnChildFrameDeactivated() {}


protected:
    lmTDIParentFrame *m_pMDIParentFrame;
    wxRect            m_MDIRect;
    wxString          m_Title;


protected:
    void Init();

    virtual void DoMoveWindow(int x, int y, int width, int height);

    // This function needs to be called when a size change is confirmed,
    // we needed this function to prevent any body from the outside
    // changing the panel... it messes the UI layout when we would allow it.
    void ApplyMDIChildFrameRect();

private:
    DECLARE_DYNAMIC_CLASS(lmTDIChildFrame)
    DECLARE_EVENT_TABLE()

    friend class lmTDIClientWindow;
};


#endif      // __LM_CHILDFRAME_H__
