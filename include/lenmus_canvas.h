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

#ifndef __LENMUS_CANVAS_H__
#define __LENMUS_CANVAS_H__

//lenmus
#include "lenmus_standard_header.h"

//wxWidgets
#include <wx/panel.h>
#include <wx/aui/aui.h>
#include <wx/aui/auibook.h>
#include <wx/splitter.h>

//other
#include <string>
using namespace std;


namespace lenmus
{

//forward declarations
class ContentFrame;
class ContentWindow;

//---------------------------------------------------------------------------------------
//AWARE
// ContentWindow should be a container for Canvas objects. And Canvas objects should be
// specific classes with multiple inheritance: wxWindow and Canvas. Defining
// this should be simple:
//      class Canvas : virtual public wxWindow
//      class wxSplitterWindow : virtual public wxWindow
// And, for instace:
//      class SplittedCnavas : virtual public Canvas, virtual public wxSplitterWindow
//
// see: http://en.wikipedia.org/wiki/Virtual_inheritance
//
// Unfortunately, wxSplitterWindow is not defined as virtually inheriting from wxWindow.
// Therefore, is not possible to use virtual inheritance.
// To solve the issue, I will define Canvas as a typedef for wxWindow. and require
// that any window wanting to behave as a Canvas must implement CanvasInterface. This
// works but we loose compiler time checks. Only the Canvas name will give you a clue
// about having to implement also the CanvasInterface.


//---------------------------------------------------------------------------------------
// Canvas: a window on the ContentWindow
typedef wxWindow Canvas;

//---------------------------------------------------------------------------------------
// CanvasInterface: mandatory interface that any wxWindow to be used as Canvas must
// implement
class CanvasInterface
{
protected:
    wxString m_title;
    ContentWindow*  m_pClientWindow;

public:
    CanvasInterface(ContentWindow* pClientWindow) : m_pClientWindow(pClientWindow) {}
    virtual ~CanvasInterface() {}

    void set_title(wxWindow* pWnd, const wxString& title);
    inline wxString& get_title() { return m_title; }
};


//---------------------------------------------------------------------------------------
// ContentFrame: a wxFrame for placing the ContentWindow and the Canvas objects
class ContentFrame : public wxFrame
{
protected:
    ContentWindow* m_pContentWindow;
    Canvas* m_pActiveCanvas;

public:
    ContentFrame(wxWindow* parent,
                 wxWindowID winid,
                 const wxString& title,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxDEFAULT_FRAME_STYLE,
                 const wxString& name = wxFrameNameStr);

    virtual ~ContentFrame();

    Canvas* get_active_canvas();
    inline ContentWindow* get_content_window() const { return m_pContentWindow; }
    void add_canvas(Canvas* pCanvas, const wxString& title);
    int get_canvas_index(Canvas* pCanvas);
    Canvas* get_canvas_for_index(int iCanvas) const;
    void close_all();
};

//---------------------------------------------------------------------------------------
// ContentWindow: a place to display all Canvas objects
class ContentWindow : public wxAuiNotebook
{
public:
    ContentWindow(ContentFrame* parent, long style = 0 );
    virtual ~ContentWindow();

    Canvas* get_active_canvas();
    Canvas* get_canvas(int iCanvas) const;
    void add_canvas(Canvas* pCanvas, const wxString& title);
    int get_canvas_index(Canvas* pCanvas);
    void close_all();
    void close_active_canvas();

protected:
    void on_window_closing(wxAuiNotebookEvent& event);

    wxDECLARE_EVENT_TABLE();
};


}   //namespace lenmus

#endif      // __LENMUS_CANVAS_H__
