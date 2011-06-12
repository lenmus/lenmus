//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2011 LenMus project
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

#ifndef __LENMUS_CANVAS_H__
#define __LENMUS_CANVAS_H__

#include <wx/panel.h>
#include <wx/aui/aui.h>


namespace lenmus
{

//forward declarations
class ContentFrame;
class ContentWindow;
class Canvas;

//---------------------------------------------------------------------------------------
// ContentFrame: a wxFrame for placing the ContentWindow and the Canvas objects
class ContentFrame : public wxFrame
{
protected:
    ContentWindow* m_pContentWindow;
    Canvas* m_pActiveCanvas;
//    wxMenuBar* m_pMyMenuBar;

public:
    ContentFrame(wxWindow* parent,
                 wxWindowID winid,
                 const wxString& title,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
                 const wxString& name = wxFrameNameStr);

    virtual ~ContentFrame();

//    virtual void SetMenuBar(wxMenuBar* pMenuBar);
//    virtual bool ProcessEvent(wxEvent& event);

    Canvas* get_active_canvas();
    //void set_active_canvas(Canvas* pCanvas);
    inline ContentWindow* get_content_window() const { return m_pContentWindow; }

//    virtual void ActivateNext();
//    virtual void ActivatePrevious();
//    virtual bool CloseAll();
//    virtual void CloseActive();

    virtual void add_canvas(Canvas* pCanvas, const wxString& title);
//    virtual void remove_canvas(Canvas* pCanvas);
//    void OnSize(wxSizeEvent& event);

protected:
//    virtual void DoGetClientSize(int* width, int* height) const;



//private:
//    DECLARE_DYNAMIC_CLASS(ContentFrame)
//    DECLARE_EVENT_TABLE()
};

//---------------------------------------------------------------------------------------
// ContentWindow: a place to display all Canvas objects
class ContentWindow : public wxAuiNotebook
{
public:
    //ContentWindow();
    ContentWindow(ContentFrame* parent, long style = 0 );
    virtual ~ContentWindow();

    Canvas* get_active_canvas();
    void add_canvas(Canvas* pCanvas, const wxString& title);
//    int SetSelection(size_t nPage);
//    void OnSize(wxSizeEvent& event);
//    void OnChildClose(wxAuiNotebookEvent& evt);

//protected:
//	//event handlers
//	void OnPageChanged(wxAuiNotebookEvent& event);
//
//private:
//    DECLARE_DYNAMIC_CLASS(ContentWindow)
//    DECLARE_EVENT_TABLE()
};

//---------------------------------------------------------------------------------------
// Canvas: a panel on the ContentWindow
class Canvas: public wxPanel
{
protected:
    ContentFrame* m_pContentFrame;
    wxString m_Title;

public:
    Canvas(ContentFrame* parent, wxWindowID winid, const wxString& title,
           const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
           long style = wxDEFAULT_FRAME_STYLE, const wxString& name = wxFrameNameStr);
    virtual ~Canvas();

//    virtual void SetTitle(const wxString& title);
//    virtual wxString GetTitle() const;
//
//    virtual void Activate();

    inline void set_content_frame(ContentFrame* parentFrame) {
        m_pContentFrame = parentFrame;
    }
    inline ContentFrame* get_content_frame() const { return m_pContentFrame; }
//
//	//New mthods not in wxMDIChildFrame --------------------------
//
//	// methods to deal with zooming
//	virtual	double GetActiveViewScale() { return 1.0; }
//	virtual bool SetActiveViewScale(double rScale) { return false; }
//
//	// call back from ContentWindow::PageChanged
//	virtual void OnChildFrameActivated() {}
//    virtual void OnChildFrameDeactivated() {}

protected:
    //DECLARE_DYNAMIC_CLASS(Canvas)
    //DECLARE_EVENT_TABLE()
};


}   //namespace lenmus

#endif      // __LENMUS_CANVAS_H__
