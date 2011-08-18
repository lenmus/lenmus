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

//lenmus
#include "lenmus_score_canvas.h"
#include "lenmus_standard_header.h"

#include "lenmus_canvas.h"
#include "lenmus_string.h"
#include "lenmus_midi_server.h"
#include "lenmus_dyncontrol.h"
#include "lenmus_standard_header.h"

//lomse
#include <lomse_shapes.h>

//wxWidgets
#include <wx/filename.h>


namespace lenmus
{

//=======================================================================================
// DocumentCanvas implementation
//=======================================================================================

BEGIN_EVENT_TABLE(DocumentCanvas, wxWindow)
    EVT_SIZE(DocumentCanvas::on_size)
    EVT_MOUSE_EVENTS(DocumentCanvas::on_mouse_event)
	EVT_KEY_DOWN(DocumentCanvas::on_key_event)
    EVT_PAINT(DocumentCanvas::on_paint)
    EVT_SCROLLWIN(DocumentCanvas::on_scroll)
    LM_EVT_SCORE_HIGHLIGHT(DocumentCanvas::on_visual_highlight)
END_EVENT_TABLE()

DocumentCanvas::DocumentCanvas(wxWindow* parent, ApplicationScope& appScope,
                         LomseDoorway& lomse)
    : wxWindow(parent, wxNewId(), wxDefaultPosition, wxDefaultSize,
               wxVSCROLL | wxHSCROLL, _T("DocumentCanvas") )
    , m_appScope(appScope)
    , m_lomse(lomse)
    , m_pPresenter(NULL)
    , m_pInteractor(NULL)
    , m_pDoc(NULL)
    , m_buffer(NULL)
    , m_view_needs_redraw(true)
    , m_filename(_T(""))
{
    //create a bitmap for the View
    wxSize size = this->GetClientSize();
    create_rendering_buffer(size.GetWidth(), size.GetHeight());
}

//---------------------------------------------------------------------------------------
DocumentCanvas::~DocumentCanvas()
{
    //delete the Interactor. This will also delete the Document
    delete m_pInteractor;
}

//---------------------------------------------------------------------------------------
void DocumentCanvas::wrapper_force_redraw(void* pThis)
{
    static_cast<DocumentCanvas*>(pThis)->force_redraw();
}

//---------------------------------------------------------------------------------------
void DocumentCanvas::force_redraw()
{
    // Callback method for Lomse. It can be used also by your application.
    // force_redraw() is an analog of the Win32 InvalidateRect() function.
    // When invoked by Lomse it must set a flag (or send a message) which
    // results in invoking View->on_paint() and then updating the content of
    // the window when the next event cycle comes.

    m_view_needs_redraw = true;             //force to invoke View->on_paint()
    update_rendering_buffer();
    update_window();
}

//---------------------------------------------------------------------------------------
void DocumentCanvas::wrapper_update_window(void* pThis)
{
    static_cast<DocumentCanvas*>(pThis)->update_window();
}

//---------------------------------------------------------------------------------------
void DocumentCanvas::update_window()
{
    // Callback method for Lomse. It can be used also by your application.
    // Invoking update_window() results in just putting immediately the content
    // of the currently rendered buffer to the window without calling
    // any View methods (i.e. on_paint)

    m_view_needs_redraw = false;

    wxClientDC dc(this);
    do_update_window(dc);
}

//---------------------------------------------------------------------------------------
void DocumentCanvas::do_update_window(wxDC& dc)
{
    if (!m_buffer || !m_buffer->IsOk())
        return;

    wxBitmap bitmap(*m_buffer);
    dc.DrawBitmap(bitmap, 0, 0, false /* don't use mask */);
    SetFocus();
}

//---------------------------------------------------------------------------------------
void DocumentCanvas::wrapper_on_lomse_event(void* pThis, EventInfo* event)
{
    static_cast<DocumentCanvas*>(pThis)->on_lomse_event(event);
}

//---------------------------------------------------------------------------------------
void DocumentCanvas::on_lomse_event(EventInfo* pEvent)
{
    //if (pEvent->is_on_click_event())
    //{
    //    EventOnClick* pEv = dynamic_cast<EventOnClick*>( pEvent );
    //    DynControl* pCtrl = dynamic_cast<DynControl*>( pEv->get_generator() );
    //    if (pCtrl)
    //    {
    //        pCtrl->on_event(pEvent);
    //        return;
    //    }
    //    else
    //        wxMessageBox(_T("Has ImoDynamic but no generator!"));
    //}
    //else
    {
        wxString msg = wxString::Format(_T("Event %d received from Lomse"),
                                        pEvent->get_event_type());
        wxMessageBox(msg);
    }
    delete pEvent;
}

//---------------------------------------------------------------------------------------
void DocumentCanvas::on_visual_highlight(lmScoreHighlightEvent& event)
{
    EventScoreHighlight* pEv = event.get_lomse_event();
    Interactor* pInteractor = get_interactor();
    static Pixels xPos = 100;

    std::list< pair<int, ImoStaffObj*> >& items = pEv->get_items();
    std::list< pair<int, ImoStaffObj*> >::iterator it;
    for (it = items.begin(); it != items.end(); ++it)
    {
        ImoStaffObj* pSO = (*it).second;
        switch ((*it).first)
        {
            case k_end_of_higlight_event:
                pInteractor->hide_tempo_line();
                //pScore->RemoveAllHighlight((wxWindow*)m_pCanvas);
                break;

            case k_highlight_off_event:
                pInteractor->remove_highlight_from_object(pSO);
                break;

            case k_highlight_on_event:
                pInteractor->highlight_object(pSO);
                break;

            case k_advance_tempo_line_event:
                xPos += 20;
                pInteractor->show_tempo_line(xPos, 150, xPos+1, 200);
                break;

            default:
                wxASSERT(false);
        }
    }
    delete pEv;
    force_redraw();
}

//---------------------------------------------------------------------------------------
void DocumentCanvas::display_document(LdpReader& reader, int viewType,
                                      const string& title)
{
    delete m_pPresenter;
    m_pPresenter = m_lomse.open_document(viewType, reader);
    do_display();
}

//---------------------------------------------------------------------------------------
void DocumentCanvas::display_document(const string& filename, int viewType)
{
    delete m_pPresenter;
    m_pPresenter = m_lomse.open_document(viewType, filename);

    //use filename (without path) as page title
    wxFileName oFN( to_wx_string(filename) );
    m_filename = oFN.GetFullName();
    //set_title( oFN.GetName() );

    do_display();
}

//---------------------------------------------------------------------------------------
void DocumentCanvas::do_display()
{
    //get the pointers to the relevant components
    m_pDoc = m_pPresenter->get_document();
    m_pInteractor = m_pPresenter->get_interactor(0);

    //set scrollbars
    adjust_scrollbars();

    //connect the View with the window buffer and set required callbacks
    m_pInteractor->set_rendering_buffer(&m_rbuf_window);
    m_pInteractor->set_force_redraw_callbak(this, wrapper_force_redraw);
    m_pInteractor->set_update_window_callbak(this, wrapper_update_window);
    m_pInteractor->set_notify_callback(this, wrapper_on_lomse_event);

    //render the new score
    wxSize size = this->GetClientSize();
    //    m_pInteractor->zoom_fit_full(size.GetWidth(), size.GetHeight());
    //    zoom_fit_width();
    m_pInteractor->set_viewport_at_page_center(size.GetWidth());
    force_redraw();
}

//---------------------------------------------------------------------------------------
void DocumentCanvas::on_size(wxSizeEvent& WXUNUSED(event))
{
    wxSize size = this->GetClientSize();
    adjust_scrollbars();
    create_rendering_buffer(size.GetWidth(), size.GetHeight());
    m_pInteractor->set_viewport_at_page_center(size.GetWidth());
    force_redraw();
}

//---------------------------------------------------------------------------------------
void DocumentCanvas::on_paint(wxPaintEvent &WXUNUSED(event))
{
    update_rendering_buffer();
    wxPaintDC dc(this);
    do_update_window(dc);
}

//---------------------------------------------------------------------------------------
void DocumentCanvas::on_mouse_event(wxMouseEvent& event)
{
    if (!m_pInteractor) return;

    wxEventType nEventType = event.GetEventType();
    wxPoint pos = event.GetPosition();
    unsigned flags = get_mouse_flags(event);

    if (nEventType == wxEVT_LEFT_DOWN)
    {
        flags |= k_mouse_left;
        m_pInteractor->on_mouse_button_down(pos.x, pos.y, flags);
    }
    else if (nEventType == wxEVT_LEFT_UP)
    {
        flags |= k_mouse_left;
        m_pInteractor->on_mouse_button_up(pos.x, pos.y, flags);
    }
    else if (nEventType == wxEVT_RIGHT_DOWN)
    {
        flags |= k_mouse_right;
        m_pInteractor->on_mouse_button_down(pos.x, pos.y, flags);
    }
    else if (nEventType == wxEVT_RIGHT_UP)
    {
        flags |= k_mouse_right;
        m_pInteractor->on_mouse_button_up(pos.x, pos.y, flags);
    }
    else if (nEventType == wxEVT_MOUSEWHEEL)
    {
        if (flags && k_kbd_ctrl)
        {
            // Ctrl + mouse wheel --> zoom in/out
            if (event.GetWheelRotation() > 0)
                zoom_in();
            else
                zoom_out();
            force_redraw();
        }
        else
        {
            //scroll up/down
            if (event.GetWheelRotation() > 0)
                scroll_line_up();
            else
                scroll_line_down();
        }
    }
    else if (nEventType == wxEVT_MOTION)
        m_pInteractor->on_mouse_move(pos.x, pos.y, flags);
}

//---------------------------------------------------------------------------------------
unsigned DocumentCanvas::get_mouse_flags(wxMouseEvent& event)
{
    unsigned flags = 0;
    if (event.LeftIsDown())     flags |= k_mouse_left;
    if (event.RightIsDown())    flags |= k_mouse_right;
    if (event.MiddleDown())     flags |= k_mouse_middle;
    if (event.ShiftDown())      flags |= k_kbd_shift;
    if (event.AltDown())        flags |= k_kbd_alt;
    if (event.ControlDown())    flags |= k_kbd_ctrl;
    return flags;
}

//---------------------------------------------------------------------------------------
unsigned DocumentCanvas::get_keyboard_flags(wxKeyEvent& event)
{
    unsigned flags = 0;
    if (event.ShiftDown())   flags |= k_kbd_shift;
    if (event.AltDown()) flags |= k_kbd_alt;
    if (event.ControlDown()) flags |= k_kbd_ctrl;
    return flags;
}

//---------------------------------------------------------------------------------------
void DocumentCanvas::on_document_updated()
{
    if (!m_pInteractor) return;

    m_pInteractor->on_document_reloaded();
    force_redraw();
}

//---------------------------------------------------------------------------------------
void DocumentCanvas::update_rendering_buffer()
{
    //update buffer if necessary
    if (m_view_needs_redraw)
        update_view_content();

    m_view_needs_redraw = false;
}

//---------------------------------------------------------------------------------------
void DocumentCanvas::delete_rendering_buffer()
{
    delete m_buffer;
}

//---------------------------------------------------------------------------------------
void DocumentCanvas::create_rendering_buffer(int width, int height)
{
    //creates a bitmap of specified size and associates it to the rendering
    //buffer for the view. Any existing buffer is automatically deleted

    //I will use a wxImage as the rendering  buffer. wxImage is platform independent
    //and its buffer is an array of characters in RGBRGBRGB... format,  in the
    //top-to-bottom, left-to-right order. That is, the first RGB triplet corresponds
    //to the first pixel of the first row; the second RGB triplet, to the second
    //pixel of the first row, and so on until the end of the first row,
    //with second row following after it and so on.

    #define BYTES_PP 3      // Bytes per pixel

    // allocate a new rendering buffer
    delete m_buffer;            //delete any previous buffer
    m_nBufWidth = width;
    m_nBufHeight = height;
    m_buffer = new wxImage(width, height);

    int stride = m_nBufWidth * BYTES_PP;        //number of bytes per row

    m_pdata = m_buffer->GetData();
    m_rbuf_window.attach(m_pdata, m_nBufWidth, m_nBufHeight, stride);

    m_view_needs_redraw = true;
}

//---------------------------------------------------------------------------------------
void DocumentCanvas::on_key_event(wxKeyEvent& event)
{
    if (!m_pInteractor) return;

    int nKeyCode = event.GetKeyCode();
    unsigned flags = get_keyboard_flags(event);

    //fix ctrol+key codes
    if (nKeyCode > 0 && nKeyCode < 27)
    {
        nKeyCode += int('A') - 1;
        flags |= k_kbd_ctrl;
    }

    //process key
    switch (nKeyCode)
    {
        case WXK_SHIFT:
        case WXK_ALT:
        case WXK_CONTROL:
            return;      //do nothing

		default:
			on_key(event.GetX(), event.GetY(), nKeyCode, flags);;
	}
}

//---------------------------------------------------------------------------------------
void DocumentCanvas::on_key(int x, int y, unsigned key, unsigned flags)
{
    switch (key)
    {
        case '1':
            reset_boxes_to_draw();
            m_pInteractor->set_rendering_option(k_option_draw_box_doc_page_content, true);
            break;
        case '2':
            reset_boxes_to_draw();
            m_pInteractor->set_rendering_option(k_option_draw_box_container, true);
            break;
        case '3':
            reset_boxes_to_draw();
            m_pInteractor->set_rendering_option(k_option_draw_box_system, true);
            break;
        case '4':
            reset_boxes_to_draw();
            m_pInteractor->set_rendering_option(k_option_draw_box_slice, true);
            break;
        case '5':
            reset_boxes_to_draw();
            m_pInteractor->set_rendering_option(k_option_draw_box_slice_instr, true);
            break;
        case '6':
            reset_boxes_to_draw();
            m_pInteractor->set_rendering_option(k_option_draw_box_inline_flag, true);
            break;
        case '8':
            m_pInteractor->switch_task(TaskFactory::k_task_drag_view);
            break;
        case '9':
            m_pInteractor->switch_task(TaskFactory::k_task_selection);
            break;
        case '0':
            reset_boxes_to_draw();
            break;
        case '+':
            if (flags && k_kbd_ctrl)
                zoom_in();
            break;
        case '-':
            if (flags && k_kbd_ctrl)
                zoom_out();
            break;
        default:
            return;
    }

    force_redraw();
}

//---------------------------------------------------------------------------------------
void DocumentCanvas::zoom_to(double scale)
{
    if (!m_pInteractor) return;

    //set zoom, centered on window center
    wxSize size = this->GetClientSize();
    m_pInteractor->set_scale(scale, size.GetWidth()/2, 0);
    adjust_scrollbars();
}

//---------------------------------------------------------------------------------------
void DocumentCanvas::zoom_in()
{
    if (!m_pInteractor) return;

    wxSize size = this->GetClientSize();
    m_pInteractor->zoom_in(size.GetWidth()/2, 0);
    adjust_scrollbars();
}

//---------------------------------------------------------------------------------------
void DocumentCanvas::zoom_out()
{
    if (!m_pInteractor) return;

    wxSize size = this->GetClientSize();
    m_pInteractor->zoom_out(size.GetWidth()/2, 0);
    adjust_scrollbars();
}

//---------------------------------------------------------------------------------------
void DocumentCanvas::zoom_fit_width()
{
    if (!m_pInteractor) return;

    wxSize size = this->GetClientSize();
    m_pInteractor->zoom_fit_width(size.GetWidth());
    adjust_scrollbars();
}

//---------------------------------------------------------------------------------------
void DocumentCanvas::zoom_fit_full()
{
    if (!m_pInteractor) return;

    wxSize size = this->GetClientSize();
    m_pInteractor->zoom_fit_full(size.GetWidth(), size.GetHeight());
    adjust_scrollbars();
}

//---------------------------------------------------------------------------------------
void DocumentCanvas::reset_boxes_to_draw()
{
    m_pInteractor->set_rendering_option(k_option_draw_box_doc_page_content, false);
    m_pInteractor->set_rendering_option(k_option_draw_box_container, false);
    m_pInteractor->set_rendering_option(k_option_draw_box_system, false);
    m_pInteractor->set_rendering_option(k_option_draw_box_slice, false);
    m_pInteractor->set_rendering_option(k_option_draw_box_slice_instr, false);
    m_pInteractor->set_rendering_option(k_option_draw_box_inline_flag, false);
}

//---------------------------------------------------------------------------------------
void DocumentCanvas::update_view_content()
{
    //request the view to re-draw the bitmap

    if (!m_pInteractor) return;
    m_pInteractor->on_paint();
}

//---------------------------------------------------------------------------------------
ImoScore* DocumentCanvas::get_active_score()
{
    return m_pDoc->get_score();
}

//---------------------------------------------------------------------------------------
void DocumentCanvas::open_test_document()
{
    delete m_pPresenter;
    m_pPresenter = m_lomse.new_document(ViewFactory::k_view_horizontal_book);

    //get the pointers to the relevant components
    m_pDoc = m_pPresenter->get_document();
    m_pInteractor = m_pPresenter->get_interactor(0);

    //connect the View with the window buffer and set required callbacks
    m_pInteractor->set_rendering_buffer(&m_rbuf_window);
    m_pInteractor->set_force_redraw_callbak(this, wrapper_force_redraw);
    m_pInteractor->set_update_window_callbak(this, wrapper_update_window);

    //Now let's place content on the created document
    m_pDoc->from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) "
        //"(instrument (musicData (clef G)(clef F3)(clef C1)(clef F4) )) )))" );

//        //instrument name
//        "(instrument (name \"Violin\")(musicData (clef G)(clef F4)(clef C1) )) )))" );

        //"(instrument (musicData )) )))" );

        //"(instrument (staves 2) (musicData )) )))" );
        //"(instrument (musicData )) (instrument (musicData )) )))" );

//    //Staves of different sizes
//    "(instrument (name \"Violin\")(abbrev \"Vln.\")(staff 1 (staffSpacing 400))(musicData (clef G)(n c4 e.))) "
//    "(instrument (name \"pilano\")(abbrev \"P\")(staves 2)(musicData (clef G p1)(clef F4 p2))) )))" );

//        //beamed chord. Simplest case
//        "(instrument (musicData "
//        "(clef F)(key C)(time 4 4)"
//        "(chord (n a3 e (beam 1 begin)) (n d3 e))"
//        "(chord (n g3 e (beam 1 end)) (n e3 e))"
//        "))"
//        ")))" );

//        //beams
//        "(instrument (name \"Violin\")(abbrev \"Vln.\")(musicData "
//        "(clef F4)(key E)(time 2 4)(n +c3 e.)(barline)"
//        "(n e2 q)(n e3 q)(barline)"
//        "(n f2 e (beam 1 +))(n g2 e (beam 1 -))"
//            "(n f3 e (beam 3 +))(n g3 e (beam 3 -))(barline)"
//        "(n f2 e. (beam 4 +))(n g2 s (beam 4 -b))"
//            "(n f3 s (beam 5 +f))(n g3 e. (beam 5 -))(barline)"
//        "(n g2 e. (beam 2 +))(n e3 s (beam 2 -b))(n g3 q)(barline)"
//        "(n a2 e (beam 6 +))(n g2 e (beam 6 -))(n a3 q)(barline)"
//        "(n -b2 q)(n =b3 q)(barline)"
//        "(n xc3 q)(n ++c4 q)(barline)"
//        "(n d3 q)(n --d4 q)(barline)"
//        "(n e3 q)(n e4 q)(barline)"
//        "(n f3 q)(n f4 q)(barline -)"
//        "))"
//        "(instrument (name \"pilano\")(abbrev \"P\")(staves 2)(musicData "
//        "(clef G p1)(clef F4 p2)(key F)(time 12 8)"
//        "(n c5 e. p1)(barline)"
//        "(n e4 e p1 (beam 10 +))(n g3 e p2 (beam 10 -))"
//        "(n e4 e p1 (stem up)(beam 11 +))(n e5 e p1 (stem down)(beam 11 -))(barline)"
//        "(n e4 s p1 (beam 12 ++))(n f4 s p1 (beam 12 ==))"
//            "(n g4 s p1 (beam 12 ==))(n a4 s p1 (beam 12 --))"
//        "(n c5 q p1)(barline)"
////        "(chord (n c4 q p1)(n e4 q p1)(n g4 q p1))"
////        "(chord (n c4 q p1)(n d4 q p1)(n g4 q p1))"
//        "))"
//        ")))" );

//        //beamed chord. Beam determines stem direction
//        "(instrument (musicData "
//        "(clef G)(key C)(time 2 4)"
//        "(chord (n c5 s (beam 2 begin begin))(n e5 s)(n g5 s))"
//        "(chord (n c5 s (beam 2 continue continue))(n f5 s)(n a5 s))"
//        "(chord (n d5 s (beam 2 continue continue))(n g5 s)(n b5 s))"
//        "(chord (n g4 s (beam 2 end end))(n e5 s)(n g5 s))"
//        "))"
//        ")))" );

//        //tuplet
//        "(instrument (musicData "
//        "(clef G)(key A)(time 2 4)"
//        "(n c4 e g+ t3/2)(n e4 e)(n d4 e g- t-)"
//        "(n e5 e g+ t3/2)(n c5 e)(n d5 e g- t-)"
//        "))"
//        ")))" );

//        //tuplets-engraving-rule-a-1
//        "(instrument (musicData "
//        "(time 2 4)"
//        "(n a4 e g+ t3)(n a4 e)(n a4 e g- t-)"
//        "(n a4 e g+)(n a4 e g-)"
//        "(barline)"
//        "(time 3 8)"
//        "(n a4 e g+ t4)(n a4 e)(n a4 e)(n a4 e g- t-)"
//        "(barline)"
//        "))"
//        ")))" );

        //tuplets-engraving-rule-d-1
//        "(instrument (musicData "
//        "(time 4 4)"
//        "(n e4 h t3)(n e4 h)(n e4 h t-)"
//        "(barline)"
//        "(n e5 h t3)(n e5 h)(n e5 h t-)"
//        "(barline)"
//        "(time 2 4)"
//        "(n e4 q t3)(n e4 e t-)"
//        "(barline)"
//        "(n e5 q t3)(n e5 e t-)"
//        "(barline)"
//        "(time 6 8)"
//        "(n e4 e g+ t4)(n e4 e g-)"
//        "(n e4 e g+)(n e4 e g-)"
//        "(n e4 e g+)(n e4 e g-)"
//        "(n e4 e g+)(n e4 e g- t-)"
//        "(barline)"
//        "(n e5 e g+ t4)(n e5 e g-)"
//        "(n e5 e g+)(n e5 e g-)"
//        "(n e5 e g+)(n e5 e g-)"
//        "(n e5 e g+)(n e5 e g- t-)"
//        "(barline)"
//        "))"
//        ")))" );


//        //tuplets-engraving-rule-b-1
//        "(instrument (musicData "
//        "(time 4 4)"
//        "(n e4 e g+ t3)(n e4 e g-)(r e t-)"
//        "(r e t3)(n e5 e)(r e t-)"
//        "(n e5 e t3)(r e)(r e t-)"
//        "(r e t3)(r e)(n e5 e t-)"
//        "))"
//        ")))" );

        //tie
        "(instrument (musicData "
        "(clef G)(key C)(time 4 4)"
        "(n e4 q l)(n e4 q)"
        "))"
        ")))" );


    //render the new score
    force_redraw();
}

//---------------------------------------------------------------------------------------
void DocumentCanvas::get_pages_info(int* pMinPage, int* pMaxPage,
                                 int* pSelPageFrom, int* pSelPageTo)
{
    //Return the default page range to be printed and the page range the user can
    //select for printing.

    *pMinPage = 1;
    GraphicModel* pGModel = m_pInteractor->get_graphic_model();
    *pMaxPage = pGModel->get_num_pages();
    *pSelPageFrom = 1;
    *pSelPageTo = *pMaxPage;
}

//---------------------------------------------------------------------------------------
void DocumentCanvas::do_print(wxDC* pDC, int page, int paperWidthPixels,
                           int paperHeightPixels)
{
    pDC->SetBackground(*wxWHITE_BRUSH);
    pDC->Clear();

    //get page size in pixels
    VSize size = m_pInteractor->get_page_size_in_pixels(page);
    double vPageWidth( size.width );
    double vPageHeigh( size.height );

    //determine view scaling
    double scaleX = double(paperWidthPixels) / vPageWidth;
    double scaleY = double(paperHeightPixels) / vPageHeigh;
    double scale = max(scaleX, scaleY);

    //determine required buffer size (pixels)
    float wReq = float(paperWidthPixels);
    float hReq = float(paperHeightPixels);

    //determine tile size (pixels)
    int width = min(1024, paperWidthPixels);
    int height = min(1024, paperHeightPixels);
    int border = 8;
    if (width < 1024 && height < 1024)
        border = 0;

    //From paper viewpoint, for copying a tile into paper, copy origin is
    //at (B, B) and copy size is (w-2B, h-2B). Initial paper org is at (0,0).
    //From render viewpoint, initial viewport origin is at (-B, -B) and tiles
    //size (for advancing viewport origin) is also (w-2B, h-2B).
    VPoint viewport(0,0);
    VPoint paperPos(0,0);
    int tileW = width - 2 * border;
    int tileH = height - 2 * border;

    //determine how many tiles to print
    int rows = int(hReq / float(tileW) + 0.5f);
    int cols = int(wReq / float(tileH) + 0.5f);

    //determine last row and last column tile sizes
    int lastW = paperWidthPixels - tileW * (cols - 1);
    int lastH = paperHeightPixels - tileH * (rows - 1);

    //allocate tile buffer
    RenderingBuffer rbuf_print;
    wxImage* buffer;                        //the image to serve as buffer
    unsigned char* pdata;                   //ptr to the real bytes buffer
    #define BYTES_PP 3                      // Bytes per pixel
    buffer = new wxImage(width, height);    // allocate the rendering buffer
    int stride = width * BYTES_PP;          //number of bytes per row
    pdata = buffer->GetData();
    rbuf_print.attach(pdata, width, height, stride);
    m_pInteractor->set_printing_buffer(&rbuf_print);

    //loop to print tiles.
    wxMemoryDC memoryDC;
    for (int iRow=0; iRow < rows; ++iRow)
    {
        for (int iCol=0; iCol < cols; ++iCol)
        {
            m_pInteractor->on_print_page(page-1, scale, viewport);

            //print this tile
            int tileWidth = (iCol == cols-1 ? lastW : tileW);
            int tileHeight = (iRow == rows-1 ? lastH : tileH);

            #if 1
                wxBitmap bitmap = *buffer;
                if (bitmap.Ok())
                {
                    if (border > 0)
                    {
                        memoryDC.SelectObjectAsSource(bitmap);
                        pDC->Blit(paperPos.x, paperPos.y, tileWidth, tileHeight,
                                &memoryDC, border, border);
                        memoryDC.SelectObjectAsSource(wxNullBitmap);
                    }
                    else
                        pDC->DrawBitmap(bitmap, paperPos.x, paperPos.y, false /* don't use mask */);
                }
            #else
                //to try to save time, instead of blitting, get subimage
                //Results: it takes the same time and prints a black line at bottom
                if (border > 0)
                {
                    wxRect rect(border, border, tileWidth, tileHeight);
                    wxBitmap bitmap( buffer->GetSubImage(rect) );
                    if (bitmap.Ok())
                        pDC->DrawBitmap(bitmap, paperPos.x, paperPos.y, false /* don't use mask */);
                }
                else
                {
                    wxBitmap bitmap( *buffer );
                    if (bitmap.Ok())
                        pDC->DrawBitmap(bitmap, paperPos.x, paperPos.y, false /* don't use mask */);
                }
            #endif

            //advance origin by tile size
            viewport.x -= tileW;
            paperPos.x += tileW;
        }
        //start new row
        viewport.x = 0;
        viewport.y -= tileH;
        paperPos.x = 0;
        paperPos.y += tileH;
    }

    delete buffer;
}

//---------------------------------------------------------------------------------------
void DocumentCanvas::adjust_scrollbars()
{
    //total size of the rendered document
    Pixels xWidth, yHeight;
    m_pInteractor->get_view_size(&xWidth, &yHeight);

    //pixels range
    m_xMargin = xWidth/20;              //5% margin, at each side
    m_xPxPerUnit = 8;
    m_xPageSize = m_nBufWidth;

    m_xMinPxPos = -(-m_xMargin);
    m_xMaxPxPos = -((xWidth + m_xMargin) - m_xPageSize);

    //scroll units
    m_xThumb = m_xPageSize / m_xPxPerUnit;
    m_xMaxUnits = (m_xMargin + xWidth + m_xMargin) / m_xPxPerUnit;


    //compute scroll steps
    m_yLineScroll = 15;     //number of pixels per vertical scroll unit
    m_yPageScroll = m_nBufHeight;
    m_yBottom = yHeight;

    //determine thumb size (in scroll units)
    int vThumb = m_nBufHeight / m_yLineScroll;

    //set scrollbars (in scroll units)
    SetScrollbar(wxVERTICAL, 0, vThumb, m_yBottom / m_yLineScroll);
    SetScrollbar(wxHORIZONTAL, 0, m_xThumb, m_xMaxUnits);
}

//---------------------------------------------------------------------------------------
void DocumentCanvas::on_scroll(wxScrollWinEvent& event)
{

    int xPos, yPos;
    m_pInteractor->get_viewport(&xPos, &yPos);

    wxEventType type = event.GetEventType();

    if (event.GetOrientation() == wxVERTICAL)
    {
        if (type == wxEVT_SCROLLWIN_TOP)
            yPos = 0;
        else if (type == wxEVT_SCROLLWIN_BOTTOM)
            yPos = -m_yBottom;
        else if (type == wxEVT_SCROLLWIN_LINEUP)
            yPos += m_yLineScroll;
        else if (type == wxEVT_SCROLLWIN_LINEDOWN)
            yPos -= m_yLineScroll;
        else if (type == wxEVT_SCROLLWIN_PAGEUP)
            yPos += m_yPageScroll;
        else if (type == wxEVT_SCROLLWIN_PAGEDOWN)
            yPos -= m_yPageScroll;
        else if (type == wxEVT_SCROLLWIN_THUMBTRACK
                 || type == wxEVT_SCROLLWIN_THUMBRELEASE)
        {
            yPos = - (event.GetPosition() * m_yLineScroll);
        }

        #if (LENMUS_PLATFORM_WIN32 == 1)  //---------------------------------------------
        {
            //In Windows, up/down buttons remain enabled even when reaching top/bottom
            if (yPos > 0)
                yPos = 0;
            else if (yPos < -m_yBottom)
                yPos = -m_yBottom;

            //in Windows the scroll thumb remains on top, so we have to
            //reposition it manually
            if (type != wxEVT_SCROLLWIN_THUMBTRACK)
                SetScrollPos(wxVERTICAL, -yPos / m_yLineScroll);
        }
        #endif  //-----------------------------------------------------------------------

        m_pInteractor->new_viewport(xPos, yPos);
    }

    else
    {
        if (type == wxEVT_SCROLLWIN_TOP)
            xPos = m_xMinPxPos;
        else if (type == wxEVT_SCROLLWIN_BOTTOM)
            xPos = m_xMaxPxPos;
        else if (type == wxEVT_SCROLLWIN_LINEUP)
            xPos += m_xPxPerUnit;
        else if (type == wxEVT_SCROLLWIN_LINEDOWN)
            xPos -= m_xPxPerUnit;
        else if (type == wxEVT_SCROLLWIN_PAGEUP)
            xPos += m_xPageSize;
        else if (type == wxEVT_SCROLLWIN_PAGEDOWN)
            xPos -= m_xPageSize;
        else if (type == wxEVT_SCROLLWIN_THUMBTRACK
                 || type == wxEVT_SCROLLWIN_THUMBRELEASE)
        {
            int units = event.GetPosition();
            xPos = m_xMargin - (units * m_xPxPerUnit);
        }

        #if (LENMUS_PLATFORM_WIN32 == 1)  //---------------------------------------------
        {
            //In Windows, up/down buttons remain enabled even when reaching top/bottom
            if (xPos > m_xMargin)
                xPos = m_xMargin;
            else if (xPos < m_xMaxPxPos)
                xPos = m_xMaxPxPos;

            //in Windows the scroll thumb remains on top, so we have to
            //reposition it manually
            if (type != wxEVT_SCROLLWIN_THUMBTRACK)
                SetScrollPos(wxHORIZONTAL, (m_xMargin - xPos) / m_xPxPerUnit);
        }
        #endif  //-----------------------------------------------------------------------

        m_pInteractor->new_viewport(xPos, yPos);
    }

    event.Skip(false);      //do not propagate event
}

//---------------------------------------------------------------------------------------
void DocumentCanvas::scroll_line(bool fUp)
{
    int xPos, yPos;
    m_pInteractor->get_viewport(&xPos, &yPos);
    if (fUp)
        yPos += m_yLineScroll;
    else
        yPos -= m_yLineScroll;

    #if (LENMUS_PLATFORM_WIN32 == 1)  //---------------------------------------------
    {
        //In Windows, up/down buttons remain enabled even when reaching top/bottom
        if (yPos > 0)
            yPos = 0;
        else if (yPos < -m_yBottom)
            yPos = -m_yBottom;
    }
    #endif  //-----------------------------------------------------------------------

    m_pInteractor->new_viewport(xPos, yPos);
}


}   //namespace lenmus
