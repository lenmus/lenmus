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

#ifndef __LENMUS_DOCUMENT_CANVAS_H__        //to avoid nested includes
#define __LENMUS_DOCUMENT_CANVAS_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_canvas.h"
#include "lenmus_injectors.h"
#include "lenmus_events.h"

//wxWidgets
#include "wx/wxprec.h"
#include <wx/app.h>
#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/filedlg.h>
#include <wx/image.h>
#include <wx/dc.h>
#include <wx/dcmemory.h>
#include <wx/event.h>
#include <wx/aui/aui.h>

//lomse
#include "lomse_doorway.h"
#include "lomse_document.h"
#include "lomse_graphic_view.h"
#include "lomse_interactor.h"
#include "lomse_presenter.h"
#include "lomse_events.h"
#include "lomse_internal_model.h"
#include "lomse_analyser.h"
#include "lomse_reader.h"
using namespace lomse;

//other
#include <iostream>
#include <UnitTest++.h>


namespace lenmus
{

//---------------------------------------------------------------------------------------
// DocumentCanvas is a window on which we show an LDP simple Document (score, book page)
class DocumentWindow : public wxWindow
{
public:
    DocumentWindow(wxWindow* parent, ApplicationScope& appScope, LomseDoorway& lomse);
    virtual ~DocumentWindow();

    //callback wrappers
    static void wrapper_update_window(void* pThis, SpEventInfo pEvent);
    static void wrapper_play_score(void* pThis, SpEventInfo pEvent);

    //commands from main frame
    void display_document(const string& filename, int viewType);
    void display_document(LdpReader& reader, int viewType, const string& title);
    void zoom_in();
    void zoom_out();
    void zoom_fit_full();
    void zoom_fit_width();
    void zoom_to(double scale);
    inline void scroll_line_up() { scroll_line(true); }
    inline void scroll_line_down() { scroll_line(false); }

    void on_key(int x, int y, unsigned key, unsigned flags);
    void set_debug_draw_box(int boxType);
    void on_document_updated();

    void open_test_document();
    void on_key_event(wxKeyEvent& event);
    void on_hyperlink_event(SpEventInfo pEvent);

    //accessors
    ImoScore* get_active_score();
    inline Interactor* get_interactor() { return m_pInteractor; }
    inline Document* get_document() { return m_pDoc; }
    inline wxString& get_filename() { return m_filename; }
    inline int get_zoom_mode() { return m_zoomMode; }

    //printing
    void do_print(wxDC* pDC, int page, int paperWidthPixels, int paperHeightPixels);
    void get_pages_info(int* pMinPage, int* pMaxPage, int* pSelPageFrom, int* pSelPageTo);

    enum {
        k_zoom_fit_full = 0,
        k_zoom_fit_width,
        k_zoom_user,
    };
    inline void set_zoom_mode(int zoomMode) { m_zoomMode = zoomMode; }

    //debug. Commands from MainFrame
    void debug_display_ldp_source();

protected:
    ApplicationScope& m_appScope;

    // In this example we are going to display an score on a canvas window.
    // Let's define the necessary variables:
    LomseDoorway&   m_lomse;        //the Lomse library doorway
    Presenter*      m_pPresenter;
    Interactor*     m_pInteractor;  //to interact with the View
    Document*       m_pDoc;         //the document to display

    //the Lomse View renders its content on a bitmap. To manage it, Lomse
    //associates the bitmap to a RenderingBuffer object.
    //It is your responsibility to render the bitmap on a window.
    //Here you define the rendering buffer and its associated bitmap to be
    //used by the previously defined View.
    RenderingBuffer     m_rbuf_window;
    wxImage*            m_buffer;               //the image to serve as buffer
    unsigned char*      m_pdata;                //ptr to the real bytes buffer
    int                 m_nBufWidth, m_nBufHeight;      //size of the bitmap

    //some additinal variables
    wxString    m_filename;             //with extension but without path
    int         m_zoomMode;

    //in some platformts (i.e. MS Windows) updating scrollbars triggers
    //on_size() events. And this, in turn, updates scroolbars and forces
    //a window redraw. To avoid these redundant behaviour we define a flag
    //to prevent handling on_size events
    bool    m_fIgnoreOnSize;
    bool    m_fFirstPaint;

    //scrolling
    Pixels m_xScrollSpaceWidth, m_yScrollSpaceHeight;
    int m_xScrollPageWidth, m_yScrollPageHeight;
    int m_xPixelsPerScrollUnit, m_yPixelsPerScrollUnit;
    int m_xMargin, m_yMargin;
    int m_xMaxScrollUnits, m_yMaxScrollUnits;
    int m_xMinViewport, m_yMinViewport;
    int m_xMaxViewport, m_yMaxViewport;


    // wxWidgets event handlers
    void on_paint(wxPaintEvent& WXUNUSED(event));
    void on_erase_background(wxEraseEvent& WXUNUSED(event)) {}
    void on_size(wxSizeEvent& WXUNUSED(event));
    void on_mouse_event(wxMouseEvent& event);
    void on_visual_highlight(lmScoreHighlightEvent& event);
    void on_end_of_playback(lmEndOfPlaybackEvent& event);
    void on_scroll(wxScrollWinEvent& event);

    //playback
    void on_play_score(SpEventInfo pEvent);
    void play_score(SpEventInfo pEvent);
    void play_pause();
    void play_stop();

    void set_viewport_at_page_center();
    void scroll_line(bool fUp);
    void update_window();

    void delete_rendering_buffer();
    void create_rendering_buffer();
    void copy_buffer_on_dc(wxDC& dc);
    void update_rendering_buffer();
    bool is_buffer_ok();

    unsigned get_keyboard_flags(wxKeyEvent& event);
    unsigned get_mouse_flags(wxMouseEvent& event);
    void determine_scroll_space_size();
    void adjust_scrollbars();
    void adjust_scale_and_scrollbars();
    void do_display(ostringstream& reporter);
    void display_errors(ostringstream& reporter);

    DECLARE_EVENT_TABLE()
};

//---------------------------------------------------------------------------------------
// DocumentCanvas is a DocumentWindow to be used as Canvas for simple Documents
class DocumentCanvas : public DocumentWindow
                     , public CanvasInterface
{
public:
    DocumentCanvas(ContentWindow* parent, ApplicationScope& appScope,
                   LomseDoorway& lomse)
        : DocumentWindow(parent, appScope, lomse)
        , CanvasInterface(parent)
    {
    }
    virtual ~DocumentCanvas() {}
};



}   // namespace lenmus

#endif    // __LENMUS_DOCUMENT_CANVAS_H__

