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

#include "lenmus_score_canvas.h"

#include "lenmus_canvas.h"
#include "lenmus_string.h"
#include "lenmus_midi_server.h"

#include "lomse_score_player.h"


namespace lenmus
{

//=======================================================================================
// ScoreCanvas implementation
//=======================================================================================

BEGIN_EVENT_TABLE(ScoreCanvas, wxWindow)
    EVT_SIZE(ScoreCanvas::on_size)
    EVT_MOUSE_EVENTS(ScoreCanvas::on_mouse_event)
	EVT_KEY_DOWN(ScoreCanvas::on_key_event)
    EVT_PAINT(ScoreCanvas::on_paint)
END_EVENT_TABLE()

ScoreCanvas::ScoreCanvas(ContentFrame* parent, ApplicationScope& appScope,
                         LomseDoorway& lomse)
    : Canvas(parent, wxID_ANY, wxT("ScoreCanvas"))
    , m_appScope(appScope)
    , m_lomse(lomse)
    , m_pPresenter(NULL)
    , m_pInteractor(NULL)
    , m_pDoc(NULL)
    , m_buffer(NULL)
    , m_view_needs_redraw(true)
{
    SetBackgroundColour(wxColour(128, 128, 128));

    //create a bitmap for the View
    wxSize size = this->GetClientSize();
    create_rendering_buffer(size.GetWidth(), size.GetHeight());
}

//---------------------------------------------------------------------------------------
ScoreCanvas::~ScoreCanvas()
{
    //delete the Interactor. This will also delete the Document
    delete m_pInteractor;
}

//---------------------------------------------------------------------------------------
void ScoreCanvas::wrapper_force_redraw(void* pThis)
{
    static_cast<ScoreCanvas*>(pThis)->force_redraw();
}

//---------------------------------------------------------------------------------------
void ScoreCanvas::force_redraw()
{
    // Callback method for Lomse. It can be used also by your application.
    // force_redraw() is an analog of the Win32 InvalidateRect() function.
    // When invoked by Lomse it must it set a flag (or send a message) which
    // results in invoking View->on_paint() and then updating the content of
    // the window when the next event cycle comes.

    m_view_needs_redraw = true;             //force to invoke View->on_paint()
    update_rendering_buffer();
    update_window();
}

//---------------------------------------------------------------------------------------
void ScoreCanvas::wrapper_update_window(void* pThis)
{
    static_cast<ScoreCanvas*>(pThis)->update_window();
}

//---------------------------------------------------------------------------------------
void ScoreCanvas::update_window()
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
void ScoreCanvas::do_update_window(wxDC& dc)
{
    if (!m_buffer || !m_buffer->IsOk())
        return;

    // allocate a DC in memory for using the offscreen bitmaps
    wxMemoryDC memoryDC;
    wxBitmap bitmap(*m_buffer);
    memoryDC.SelectObject(bitmap);

    //copy bitmap onto screen
    dc.Blit(0, 0, m_nBufWidth, m_nBufHeight, &memoryDC, 0, 0);

    // deselect the bitmap
    memoryDC.SelectObject(wxNullBitmap);

    SetFocus();
}

//---------------------------------------------------------------------------------------
void ScoreCanvas::wrapper_on_lomse_event(void* pThis, EventInfo& event)
{
    static_cast<ScoreCanvas*>(pThis)->on_lomse_event(event);
}

//---------------------------------------------------------------------------------------
void ScoreCanvas::on_lomse_event(EventInfo& event)
{
    //Lomse library informs user application via 'notifications' by invoking this
    //callback
    wxMessageBox(_T("Notification received from Lomse"));
}

//---------------------------------------------------------------------------------------
void ScoreCanvas::display_document(std::string& filename)
{
    delete m_pPresenter;
    m_pPresenter = m_lomse.open_document(ViewFactory::k_view_horizontal_book,
                                         filename);

    //get the pointers to the relevant components
    m_pDoc = m_pPresenter->get_document();
    m_pInteractor = m_pPresenter->get_interactor(0);

    //connect the View with the window buffer and set required callbacks
    m_pInteractor->set_rendering_buffer(&m_rbuf_window);
    m_pInteractor->set_force_redraw_callbak(this, wrapper_force_redraw);
    m_pInteractor->set_update_window_callbak(this, wrapper_update_window);
    m_pInteractor->set_notify_callback(this, wrapper_on_lomse_event);

    //render the new score
    force_redraw();
}

//---------------------------------------------------------------------------------------
void ScoreCanvas::on_size(wxSizeEvent& WXUNUSED(event))
{
    wxSize size = this->GetClientSize();
    create_rendering_buffer(size.GetWidth(), size.GetHeight());
    force_redraw();
}

//---------------------------------------------------------------------------------------
void ScoreCanvas::on_paint(wxPaintEvent &WXUNUSED(event))
{
    update_rendering_buffer();
    wxPaintDC dc(this);
    do_update_window(dc);
}

//---------------------------------------------------------------------------------------
void ScoreCanvas::on_mouse_event(wxMouseEvent& event)
{
    if (!m_pInteractor) return;

    wxEventType nEventType = event.GetEventType();
    wxPoint pos = event.GetPosition();
    unsigned flags = get_mouse_flags(event);

    if (nEventType==wxEVT_LEFT_DOWN)
    {
        flags |= k_mouse_left;
        m_pInteractor->on_mouse_button_down(pos.x, pos.y, flags);
    }
    else if (nEventType==wxEVT_LEFT_UP)
    {
        flags |= k_mouse_left;
        m_pInteractor->on_mouse_button_up(pos.x, pos.y, flags);
    }
    else if (nEventType==wxEVT_RIGHT_DOWN)
    {
        flags |= k_mouse_right;
        m_pInteractor->on_mouse_button_down(pos.x, pos.y, flags);
    }
    else if (nEventType==wxEVT_RIGHT_UP)
    {
        flags |= k_mouse_right;
        m_pInteractor->on_mouse_button_up(pos.x, pos.y, flags);
    }
    else if (nEventType==wxEVT_MOTION)
        m_pInteractor->on_mouse_move(pos.x, pos.y, flags);
}

//---------------------------------------------------------------------------------------
unsigned ScoreCanvas::get_mouse_flags(wxMouseEvent& event)
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
unsigned ScoreCanvas::get_keyboard_flags(wxKeyEvent& event)
{
    unsigned flags = 0;
    if (event.ShiftDown())   flags |= k_kbd_shift;
    if (event.AltDown()) flags |= k_kbd_alt;
    if (event.ControlDown()) flags |= k_kbd_ctrl;
    return flags;
}

//---------------------------------------------------------------------------------------
void ScoreCanvas::on_document_updated()
{
    if (!m_pInteractor) return;

    m_pInteractor->on_document_reloaded();
    force_redraw();
}

//---------------------------------------------------------------------------------------
void ScoreCanvas::update_rendering_buffer()
{
    //update buffer if necessary
    if (m_view_needs_redraw)
        update_view_content();

    m_view_needs_redraw = false;
}

//---------------------------------------------------------------------------------------
void ScoreCanvas::delete_rendering_buffer()
{
    delete m_buffer;
}

//---------------------------------------------------------------------------------------
void ScoreCanvas::create_rendering_buffer(int width, int height)
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
void ScoreCanvas::on_key_event(wxKeyEvent& event)
{
//    wxMessageBox(_T("on_key_event"));

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
void ScoreCanvas::on_key(int x, int y, unsigned key, unsigned flags)
{
    switch (key)
    {
        case '1':
            reset_boxes_to_draw();
            m_pInteractor->set_rendering_option(k_option_draw_box_doc_page_content, true);
            break;
        case '2':
            reset_boxes_to_draw();
            m_pInteractor->set_rendering_option(k_option_draw_box_score_page, true);
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
            m_pInteractor->zoom_in(x, y);
            break;
        case '-':
            m_pInteractor->zoom_out(x, y);
            break;
        default:
            return;
    }

    force_redraw();
}

//---------------------------------------------------------------------------------------
void ScoreCanvas::zoom_in()
{
    if (!m_pInteractor) return;

    //do zoom in centered on window center
    wxSize size = this->GetClientSize();
    m_pInteractor->zoom_in(size.GetWidth()/2, size.GetHeight()/2);
    force_redraw();
}

//---------------------------------------------------------------------------------------
void ScoreCanvas::zoom_out()
{
    if (!m_pInteractor) return;

    //do zoom out centered on window center
    wxSize size = this->GetClientSize();
    m_pInteractor->zoom_out(size.GetWidth()/2, size.GetHeight()/2);
    force_redraw();
}

//---------------------------------------------------------------------------------------
void ScoreCanvas::reset_boxes_to_draw()
{
    m_pInteractor->set_rendering_option(k_option_draw_box_doc_page_content, false);
    m_pInteractor->set_rendering_option(k_option_draw_box_score_page, false);
    m_pInteractor->set_rendering_option(k_option_draw_box_system, false);
    m_pInteractor->set_rendering_option(k_option_draw_box_slice, false);
    m_pInteractor->set_rendering_option(k_option_draw_box_slice_instr, false);
}

//---------------------------------------------------------------------------------------
void ScoreCanvas::update_view_content()
{
    //request the view to re-draw the bitmap

    if (!m_pInteractor) return;
    m_pInteractor->on_paint();
}

//---------------------------------------------------------------------------------------
void ScoreCanvas::start_play()
{
    ImoScore* pScore = m_pDoc->get_score();
    MidiServer midi(m_appScope);
    LibraryScope* pLomseScope = m_lomse.get_library_scope();
    ScorePlayer player(*pLomseScope, &midi);
    player.prepare_to_play(pScore);
    player.play();
}

//---------------------------------------------------------------------------------------
void ScoreCanvas::open_test_document()
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

}   //namespace lenmus
