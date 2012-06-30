//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2012 LenMus project
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
#include "lenmus_document_canvas.h"
#include "lenmus_standard_header.h"

#include "lenmus_canvas.h"
#include "lenmus_string.h"
#include "lenmus_midi_server.h"
#include "lenmus_dyncontrol.h"
#include "lenmus_standard_header.h"
#include "lenmus_status_reporter.h"
#include "lenmus_dlg_debug.h"

//lomse
#include <lomse_shapes.h>
#include <lomse_ldp_exporter.h>
#include <lomse_score_player.h>
#include <lomse_midi_table.h>
#include <lomse_player_gui.h>

//wxWidgets
#include <wx/filename.h>

//other
#include <sstream>
#include <stdexcept>
using namespace std;

namespace lenmus
{

//=======================================================================================
// DocumentWindow implementation
//=======================================================================================

BEGIN_EVENT_TABLE(DocumentWindow, wxWindow)
    EVT_SIZE(DocumentWindow::on_size)
    EVT_MOUSE_EVENTS(DocumentWindow::on_mouse_event)
	EVT_KEY_DOWN(DocumentWindow::on_key_event)
    EVT_PAINT(DocumentWindow::on_paint)
    EVT_SCROLLWIN(DocumentWindow::on_scroll)
    LM_EVT_SCORE_HIGHLIGHT(DocumentWindow::on_visual_highlight)
    EVT_ERASE_BACKGROUND(DocumentWindow::on_erase_background)
    LM_EVT_END_OF_PLAYBACK(DocumentWindow::on_end_of_playback)
END_EVENT_TABLE()

DocumentWindow::DocumentWindow(wxWindow* parent, ApplicationScope& appScope,
                         LomseDoorway& lomse)
    : wxWindow(parent, wxNewId(), wxDefaultPosition, wxDefaultSize,
               wxVSCROLL | wxHSCROLL | wxALWAYS_SHOW_SB |
               wxFULL_REPAINT_ON_RESIZE, _T("DocumentWindow") )
    , m_appScope(appScope)
    , m_lomse(lomse)
    , m_pPresenter(NULL)
    , m_pInteractor(NULL)
    , m_pDoc(NULL)
    , m_buffer(NULL)
    , m_filename(_T(""))
    , m_zoomMode(k_zoom_fit_width)
    , m_fIgnoreOnSize(false)
    , m_fFirstPaint(true)
{
    Hide();     //keep hidden until necessary, to avoid useless repaints
}

//---------------------------------------------------------------------------------------
DocumentWindow::~DocumentWindow()
{
    ScorePlayer* pPlayer  = m_appScope.get_score_player();
    pPlayer->stop();

    //delete the presenter. This will also delete the Document
    delete m_pPresenter;
    //TO_FIX: delete presenter deletes all interactors and the document. But here we are
    //just atempting to delete one view, not the document and all its views.

    m_pInteractor = NULL;
    delete_rendering_buffer();
}

//---------------------------------------------------------------------------------------
void DocumentWindow::wrapper_play_score(void* pThis, SpEventInfo pEvent)
{
    //wxLogMessage(_T("callback: wrapper_play_score"));
    static_cast<DocumentWindow*>(pThis)->on_play_score(pEvent);
}

//---------------------------------------------------------------------------------------
void DocumentWindow::on_play_score(SpEventInfo pEvent)
{
    switch (pEvent->get_event_type())
    {
        case k_do_play_score_event:
            play_score(pEvent);
            return;

        case k_pause_score_event:
            play_pause();
            return;

        default:
            play_stop();
            return;
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::play_score(SpEventInfo pEvent)
{
    SpEventPlayScore pEv = boost::static_pointer_cast<EventPlayScore>(pEvent);
    ImoScore* pScore = pEv->get_score();
    ScorePlayer* pPlayer  = m_appScope.get_score_player();
    PlayerGui* pPlayerGui = pEv->get_player();

    pPlayer->load_score(pScore, pEv->get_player());

    //initialize with default options
    bool fVisualTracking = true;
    bool fCountOff = false;
    int playMode = k_play_normal_instrument;
    long nMM = 60;
    if (pPlayerGui)
    {
        fCountOff = pPlayerGui->get_countoff();
        playMode = pPlayerGui->get_play_mode();
        nMM = long( pPlayerGui->get_metronome_mm() );
    }

    pPlayer->play(fVisualTracking, fCountOff, playMode, nMM, m_pInteractor);
}

//---------------------------------------------------------------------------------------
void DocumentWindow::play_stop()
{
    ScorePlayer* pPlayer  = m_appScope.get_score_player();
    pPlayer->stop();
}

//---------------------------------------------------------------------------------------
void DocumentWindow::play_pause()
{
    ScorePlayer* pPlayer  = m_appScope.get_score_player();
    pPlayer->pause();
}

//---------------------------------------------------------------------------------------
void DocumentWindow::wrapper_update_window(void* pThis, SpEventInfo pEvent)
{
    //wxLogMessage(_T("callback: wrapper_update_window"));
    static_cast<DocumentWindow*>(pThis)->update_window();
}

//---------------------------------------------------------------------------------------
void DocumentWindow::update_window()
{
    // Invoking update_window() results in just putting immediately the content
    // of the currently rendered buffer to the window without neither calling
    // any lomse methods nor generating any events (i.e. window on_paint)

    //wxLogMessage(_T("update_window %0x"), this);

    wxClientDC dc(this);
    copy_buffer_on_dc(dc);
}

//---------------------------------------------------------------------------------------
void DocumentWindow::copy_buffer_on_dc(wxDC& dc)
{
    //wxLogMessage(_T("copy_buffer_on_dc %0x"), this);
    if (!m_buffer || !m_buffer->IsOk())
        return;

    wxBitmap bitmap(*m_buffer);
    dc.DrawBitmap(bitmap, 0, 0, false /* don't use mask */);

    //DEBUG: info about rendering time -------------------------------------
    double renderTime = m_pInteractor->gmodel_rendering_time();
    double buildTime = m_pInteractor->gmodel_build_time();
    wxString msg = wxString::Format(_T("Build time=%.3f, render time=%.3f ms, ticks per second=%d "),
                                    buildTime, renderTime, CLOCKS_PER_SEC );
    StatusReporter* pStatus = m_appScope.get_status_reporter();
    pStatus->report_status(msg);
    //END DEBUG ------------------------------------------------------------

    SetFocus();
}

//---------------------------------------------------------------------------------------
void DocumentWindow::on_visual_highlight(lmScoreHighlightEvent& event)
{
    SpEventScoreHighlight pEv = event.get_lomse_event();
    Interactor* pInteractor = get_interactor();
    pInteractor->on_visual_highlight(pEv);
}

//---------------------------------------------------------------------------------------
void DocumentWindow::on_end_of_playback(lmEndOfPlaybackEvent& event)
{
    wxLogMessage(_T("[DocumentWindow::on_end_of_playback]"));
    SpEventPlayScore pEv = event.get_lomse_event();
    Interactor* pInteractor = get_interactor();
    pInteractor->send_end_of_play_event(pEv->get_score(), pEv->get_player());
}

//---------------------------------------------------------------------------------------
void DocumentWindow::display_document(LdpReader& reader, int viewType,
                                      const string& title)
{
    //wxLogMessage(_T("display_document %0x"), this);

    //get lomse reporter
    ostringstream& reporter = m_appScope.get_lomse_reporter();
    reporter.str(std::string());      //remove any previous content

    try
    {
        ::wxSetCursor(*wxHOURGLASS_CURSOR);
        delete m_pPresenter;
        m_pPresenter = m_lomse.open_document(viewType, reader, reporter);
        set_zoom_mode(k_zoom_fit_width);
        do_display(reporter);
    }
    catch(std::exception& e)
    {
        wxMessageBox( to_wx_string(e.what()) );
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::display_document(const string& filename, int viewType)
{
    wxString sF = to_wx_string(filename);
    wxLogMessage(_T("display_document %s"), sF.c_str());

    //get lomse reporter
    ostringstream& reporter = m_appScope.get_lomse_reporter();
    reporter.str(std::string());      //remove any previous content

    try
    {
        ::wxSetCursor(*wxHOURGLASS_CURSOR);
        delete m_pPresenter;
        m_pPresenter = m_lomse.open_document(viewType, filename, reporter);

        //use filename (without path) as page title
        wxFileName oFN( to_wx_string(filename) );
        m_filename = oFN.GetFullName();

        set_zoom_mode(k_zoom_fit_width);
        do_display(reporter);
    }
    catch(std::exception& e)
    {
        wxMessageBox( to_wx_string(e.what()) );
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::display_errors(ostringstream& reporter)
{
    if (!reporter.str().empty())
    {
        wxString msg = to_wx_string( reporter.str() );
        wxString title = _T("Errors in file ");
        title += m_filename;
        DlgDebug dlg(this, title, msg, true /*show 'Save' button*/);
        dlg.ShowModal();
    }
    reporter.str(std::string());      //remove any previous content
}

//---------------------------------------------------------------------------------------
void DocumentWindow::do_display(ostringstream& reporter)
{
    //wxLogMessage(_T("do_display %0x"), this);

    //get the pointers to the relevant components
    m_pDoc = m_pPresenter->get_document();
    m_pInteractor = m_pPresenter->get_interactor(0);

    //connect the View with the window buffer
    m_pInteractor->set_rendering_buffer(&m_rbuf_window);

    //register to receive desired events
    m_pInteractor->add_event_handler(k_update_window_event, this, wrapper_update_window);
    m_pInteractor->add_event_handler(k_do_play_score_event, this, wrapper_play_score);
    m_pInteractor->add_event_handler(k_pause_score_event, this, wrapper_play_score);

    //set viewport and scale
    m_fFirstPaint = true;
    //create_rendering_buffer();
    //determine_scroll_space_size();
    //m_pInteractor->new_viewport(-m_xMargin, -m_yMargin);
    //adjust_scale_and_scrollbars();

    //AWARE: after creating a pane and loading content on it, wxAuiNotebook / wxFrame
    //will issue an on_size() followed by an on_paint. Therefore, do not force a
    //a repaint here as it will be redundant with the coming events
    Refresh(false /* don't erase background */);

    ////ensure that the rendering buffer is created
    //if (m_nBufWidth == 0 || m_nBufHeight == 0)
    //    create_rendering_buffer();

    display_errors(reporter);
}

//---------------------------------------------------------------------------------------
void DocumentWindow::on_size(wxSizeEvent& WXUNUSED(event))
{
    //wxLogMessage(_T("on_size %s. Visible=%d"), GetLabel().c_str(), (IsShown() ? 1 : 0) );

    if (!m_pInteractor) return;
    if (m_fIgnoreOnSize) return;

    adjust_scale_and_scrollbars();
}

//---------------------------------------------------------------------------------------
void DocumentWindow::on_paint(wxPaintEvent& WXUNUSED(event))
{
    //wxLogMessage(_T("on_paint %s. Visible=%d"), GetLabel().c_str(), (IsShown() ? 1 : 0) );

    //AWARE: According wxWidgets documentation, any paint event handler must always
    //create a wxPaintDC object, even if not used. Otherwise, under MS Windows,
    // refreshing for this and other windows will go wrong.
    wxPaintDC dc(this);

    if (IsShown() && m_pInteractor)
    {
        if (m_fFirstPaint)
        {
            m_fFirstPaint = false;
            create_rendering_buffer();
            determine_scroll_space_size();
            adjust_scale_and_scrollbars();
            m_pInteractor->new_viewport(-m_xMargin, -m_yMargin);
        }
        update_rendering_buffer();
        copy_buffer_on_dc(dc);
        ::wxSetCursor(*wxSTANDARD_CURSOR);
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::on_mouse_event(wxMouseEvent& event)
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
            Refresh(false /* don't erase background */);
        }
        else
        {
            //scroll up/down
            if (event.GetWheelRotation() > 0)
                scroll_line_up();
            else
                scroll_line_down();
            adjust_scrollbars();
        }
    }
    else if (nEventType == wxEVT_MOTION)
        m_pInteractor->on_mouse_move(pos.x, pos.y, flags);
}

////---------------------------------------------------------------------------------------
//void DocumentWindow::on_hyperlink_event(SpEventInfo pEvent)
//{
//    SpEventMouse pEv = boost::static_pointer_cast<EventMouse>(pEvent);
//    ImoLink* pLink = static_cast<ImoLink*>( pEv->get_imo_object() );
//    string& url = pLink->get_url();
//    wxString msg = wxString::Format(_T("[DocumentWindow::on_hyperlink_event] link='%s'"),
//                                    to_wx_string(url).c_str() );
//    wxMessageBox(msg);
//
////    //extract filename
////    //#LenMusPage/L1_MusicReading_mr1_thm12_E1.lms
////    string ebook = "/datos/USR/Desarrollo_wx/lenmus/locale/en/books/GeneralExercises.lmb#zip:";
////    string page = "GeneralExercises_ClefsReading.lms";
////    display_document(ebook + page, ViewFactory::k_view_vertical_book);
//}

//---------------------------------------------------------------------------------------
unsigned DocumentWindow::get_mouse_flags(wxMouseEvent& event)
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
unsigned DocumentWindow::get_keyboard_flags(wxKeyEvent& event)
{
    unsigned flags = 0;
    if (event.ShiftDown())   flags |= k_kbd_shift;
    if (event.AltDown()) flags |= k_kbd_alt;
    if (event.ControlDown()) flags |= k_kbd_ctrl;
    return flags;
}

//---------------------------------------------------------------------------------------
void DocumentWindow::on_document_updated()
{
    if (!m_pInteractor) return;

    //wxLogMessage(_T("on_document_updated %0x"), this);
    m_pInteractor->on_document_reloaded();
    Refresh(false /* don't erase background */);
}

//---------------------------------------------------------------------------------------
void DocumentWindow::update_rendering_buffer()
{
    //wxLogMessage(_T("update_rendering_buffer %0x"), this);

    if (m_pInteractor && (m_pInteractor->view_needs_repaint() || !is_buffer_ok()) )
    {
        create_rendering_buffer();
        m_pInteractor->redraw_bitmap();
    }
}

//---------------------------------------------------------------------------------------
bool DocumentWindow::is_buffer_ok()
{
    wxSize size = this->GetClientSize();
    int width = size.GetWidth();
    int height = size.GetHeight();

    return  m_buffer
        &&  m_buffer->IsOk()
        && m_nBufWidth == width
        && m_nBufHeight == height;
}

//---------------------------------------------------------------------------------------
void DocumentWindow::delete_rendering_buffer()
{
    delete m_buffer;
}

//---------------------------------------------------------------------------------------
void DocumentWindow::create_rendering_buffer()
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

    wxSize size = this->GetClientSize();
    int width = size.GetWidth();
    int height = size.GetHeight();
    //wxLogMessage(_T("create_rendering_buffer %s, w=%d, h=%d"),
    //             GetLabel().c_str(), width, height);

    // allocate a LENMUS_NEW rendering buffer
    delete m_buffer;            //delete any previous buffer
    m_nBufWidth = width;
    m_nBufHeight = height;
    m_buffer = LENMUS_NEW wxImage(width, height);

    int stride = m_nBufWidth * BYTES_PP;        //number of bytes per row

    m_pdata = m_buffer->GetData();
    m_rbuf_window.attach(m_pdata, m_nBufWidth, m_nBufHeight, stride);
}

//---------------------------------------------------------------------------------------
void DocumentWindow::on_key_event(wxKeyEvent& event)
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
void DocumentWindow::set_debug_draw_box(int boxType)
{
    m_pInteractor->reset_boxes_to_draw();
    m_pInteractor->set_box_to_draw(boxType);

    Refresh(false /* don't erase background */);
}

//---------------------------------------------------------------------------------------
void DocumentWindow::on_key(int x, int y, unsigned key, unsigned flags)
{
    switch (key)
    {
        case '1':
            m_pInteractor->reset_boxes_to_draw();
            m_pInteractor->set_rendering_option(k_option_draw_box_doc_page_content, true);
            break;
        case '2':
            m_pInteractor->reset_boxes_to_draw();
            m_pInteractor->set_rendering_option(k_option_draw_box_container, true);
            break;
        case '3':
            m_pInteractor->reset_boxes_to_draw();
            m_pInteractor->set_rendering_option(k_option_draw_box_system, true);
            break;
        case '4':
            m_pInteractor->reset_boxes_to_draw();
            m_pInteractor->set_rendering_option(k_option_draw_box_slice, true);
            break;
        case '5':
            m_pInteractor->reset_boxes_to_draw();
            m_pInteractor->set_rendering_option(k_option_draw_box_slice_instr, true);
            break;
        case '6':
            m_pInteractor->reset_boxes_to_draw();
            m_pInteractor->set_rendering_option(k_option_draw_box_inline_flag, true);
            break;
        case '8':
            m_pInteractor->switch_task(TaskFactory::k_task_drag_view);
            break;
        case '9':
            m_pInteractor->switch_task(TaskFactory::k_task_selection);
            break;
        case '0':
            m_pInteractor->reset_boxes_to_draw();
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

    Refresh(false /* don't erase background */);
}

//---------------------------------------------------------------------------------------
void DocumentWindow::zoom_to(double scale)
{
    if (!m_pInteractor) return;

    //set zoom, centered on window center
    wxSize size = this->GetClientSize();
    m_pInteractor->set_scale(scale, size.GetWidth()/2, 0);
    m_zoomMode = k_zoom_user;
    adjust_scrollbars();
}

//---------------------------------------------------------------------------------------
void DocumentWindow::zoom_in()
{
    if (!m_pInteractor) return;

    wxSize size = this->GetClientSize();
    m_pInteractor->zoom_in(size.GetWidth()/2, 0);
    m_zoomMode = k_zoom_user;
    adjust_scrollbars();
}

//---------------------------------------------------------------------------------------
void DocumentWindow::zoom_out()
{
    if (!m_pInteractor) return;

    wxSize size = this->GetClientSize();
    m_pInteractor->zoom_out(size.GetWidth()/2, 0);
    m_zoomMode = k_zoom_user;
    adjust_scrollbars();
}

//---------------------------------------------------------------------------------------
void DocumentWindow::zoom_fit_width()
{
    if (!m_pInteractor) return;

    //wxLogMessage(_T("zoom_fit_width %0x"), this);
    wxSize size = this->GetClientSize();
    m_pInteractor->zoom_fit_width(size.GetWidth());
    m_zoomMode = k_zoom_fit_width;
    adjust_scrollbars();
}

//---------------------------------------------------------------------------------------
void DocumentWindow::zoom_fit_full()
{
    if (!m_pInteractor) return;

    //wxLogMessage(_T("zoom_fit_full %0x"), this);
    wxSize size = this->GetClientSize();
    m_pInteractor->zoom_fit_full(size.GetWidth(), size.GetHeight());
    m_zoomMode = k_zoom_fit_full;
    adjust_scrollbars();
}

//---------------------------------------------------------------------------------------
ImoScore* DocumentWindow::get_active_score()
{
    return m_pDoc->get_score(0);
}

//---------------------------------------------------------------------------------------
void DocumentWindow::open_test_document()
{
    delete m_pPresenter;
    m_pPresenter = m_lomse.new_document(ViewFactory::k_view_horizontal_book);

    //get the pointers to the relevant components
    m_pDoc = m_pPresenter->get_document();
    m_pInteractor = m_pPresenter->get_interactor(0);

    //connect the View with the window buffer
    m_pInteractor->set_rendering_buffer(&m_rbuf_window);
    m_pInteractor->add_event_handler(k_update_window_event, this, wrapper_update_window);

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


    //render the LENMUS_NEW score
    Refresh(false /* don't erase background */);
}

//---------------------------------------------------------------------------------------
void DocumentWindow::get_pages_info(int* pMinPage, int* pMaxPage,
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
void DocumentWindow::do_print(wxDC* pDC, int page, int paperWidthPixels,
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
    buffer = LENMUS_NEW wxImage(width, height);    // allocate the rendering buffer
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
        //start LENMUS_NEW row
        viewport.x = 0;
        viewport.y -= tileH;
        paperPos.x = 0;
        paperPos.y += tileH;
    }

    delete buffer;
}

//---------------------------------------------------------------------------------------
void DocumentWindow::adjust_scale_and_scrollbars()
{
    //wxLogMessage(_T("adjust_scale_and_scrollbars %0x"), this);

    int zoomMode = get_zoom_mode();
    if (zoomMode == k_zoom_fit_width)
        zoom_fit_width();
    else if (zoomMode == k_zoom_fit_full)
        zoom_fit_full();
    else
        adjust_scrollbars();
}

//---------------------------------------------------------------------------------------
void DocumentWindow::determine_scroll_space_size()
{
    //wxLogMessage(_T("determine_scroll_space_size %0x"), this);

    //total size of the rendered document (whole visual space, all pages)
    m_pInteractor->get_view_size(&m_xScrollSpaceWidth, &m_yScrollSpaceHeight);
    m_xMargin = m_xScrollSpaceWidth/40;              //2.5% margin, at each side
    m_yMargin = m_xMargin;

    //how many pixels per scroll unit?
    //AWARE: In wxWidgets scrollbars uses scroll units (arbitrary user defined units).
    //After some experimentation I concluded taht the best approach is to use pixels as
    //scroll units; otherwise truncation errors create problems. The following variables
    //are used only to define the increment/decrement when scrolling one line
    m_xPixelsPerScrollUnit = 8;
    m_yPixelsPerScrollUnit = 15;

    //get viewport limits (in pixels)
    m_xMinViewport = -m_xMargin;
    m_yMinViewport = -m_yMargin;
}

//---------------------------------------------------------------------------------------
void DocumentWindow::adjust_scrollbars()
{
    //wxLogMessage(_T("adjust_scrollbars %0x"), this);

    determine_scroll_space_size();

    //get size of scroll page (client area size)
    m_xScrollPageWidth = m_nBufWidth;
    m_yScrollPageHeight = m_nBufHeight;

    //get viewport limits (in pixels)
    m_xMaxViewport = (m_xScrollSpaceWidth - m_xMargin) - m_xScrollPageWidth;
    m_yMaxViewport = (m_yScrollSpaceHeight - m_yMargin) - m_yScrollPageHeight;

    //get current viewport position
    int xCurPos, yCurPos;
    m_pInteractor->get_viewport(&xCurPos, &yCurPos);

    //determine thumb size (in scroll units) and position
    int xThumbSize = m_xScrollPageWidth;
    int xStartThumb = max(0, (m_xMargin + xCurPos));
    int yThumbSize = m_yScrollPageHeight;
    int yStartThumb = max(0, (m_yMargin + yCurPos));

    //scroll space size, in scroll units
    m_xMaxScrollUnits = m_xScrollSpaceWidth;
    m_yMaxScrollUnits = m_yScrollSpaceHeight;

    //set scrollbars
    m_fIgnoreOnSize = true;
    SetScrollbar(wxVERTICAL, yStartThumb, yThumbSize, m_yMaxScrollUnits);
    SetScrollbar(wxHORIZONTAL, xStartThumb, xThumbSize, m_xMaxScrollUnits);
    m_fIgnoreOnSize = false;
}

//---------------------------------------------------------------------------------------
void DocumentWindow::on_scroll(wxScrollWinEvent& event)
{
    //wxLogMessage(_T("on_scroll %0x"), this);

    int xPos, yPos;
    m_pInteractor->get_viewport(&xPos, &yPos);

    wxEventType type = event.GetEventType();

    if (event.GetOrientation() == wxVERTICAL)
    {
        if (type == wxEVT_SCROLLWIN_TOP)
            yPos = m_yMinViewport;
        else if (type == wxEVT_SCROLLWIN_BOTTOM)
            yPos = m_yMaxViewport;
        else if (type == wxEVT_SCROLLWIN_LINEUP)
            yPos -= m_yPixelsPerScrollUnit;
        else if (type == wxEVT_SCROLLWIN_LINEDOWN)
            yPos += m_yPixelsPerScrollUnit;
        else if (type == wxEVT_SCROLLWIN_PAGEUP)
            yPos -= m_yScrollPageHeight;
        else if (type == wxEVT_SCROLLWIN_PAGEDOWN)
            yPos += m_yScrollPageHeight;
        else if (type == wxEVT_SCROLLWIN_THUMBTRACK
                 || type == wxEVT_SCROLLWIN_THUMBRELEASE)
        {
            yPos = event.GetPosition() - m_yMargin;
        }

        #if (LENMUS_PLATFORM_WIN32 == 1)  //---------------------------------------------
        {
            //In Windows, up/down buttons remain enabled even when reaching top/bottom
            if (yPos < m_yMinViewport)
                yPos = m_yMinViewport;
            else if (yPos > m_yMaxViewport)
                yPos = m_yMaxViewport;

            //in Windows the scroll thumb remains at top, so we have to
            //reposition it manually
            if (type != wxEVT_SCROLLWIN_THUMBTRACK)
                SetScrollPos(wxVERTICAL, m_yMargin + yPos);
        }
        #endif  //-----------------------------------------------------------------------

        m_pInteractor->new_viewport(xPos, yPos);
        m_pInteractor->force_redraw();
    }

    else
    {
        if (type == wxEVT_SCROLLWIN_TOP)
            xPos = m_xMinViewport;
        else if (type == wxEVT_SCROLLWIN_BOTTOM)
            xPos = m_xMaxViewport;
        else if (type == wxEVT_SCROLLWIN_LINEUP)
            xPos -= m_xPixelsPerScrollUnit;
        else if (type == wxEVT_SCROLLWIN_LINEDOWN)
            xPos += m_xPixelsPerScrollUnit;
        else if (type == wxEVT_SCROLLWIN_PAGEUP)
            xPos -= m_xScrollPageWidth;
        else if (type == wxEVT_SCROLLWIN_PAGEDOWN)
            xPos += m_xScrollPageWidth;
        else if (type == wxEVT_SCROLLWIN_THUMBTRACK
                 || type == wxEVT_SCROLLWIN_THUMBRELEASE)
        {
            xPos = event.GetPosition() - m_xMargin;
        }

        #if (LENMUS_PLATFORM_WIN32 == 1)  //---------------------------------------------
        {
            //In Windows, up/down buttons remain enabled even when reaching top/bottom
            if (xPos < m_xMinViewport)
                xPos = m_xMinViewport;
            else if (xPos > m_xMaxViewport)
                xPos = m_xMaxViewport;

            //in Windows the scroll thumb remains at top, so we have to
            //reposition it manually
            if (type != wxEVT_SCROLLWIN_THUMBTRACK)
                SetScrollPos(wxHORIZONTAL, m_xMargin + xPos);
        }
        #endif  //-----------------------------------------------------------------------

        m_pInteractor->new_viewport(xPos, yPos);
        m_pInteractor->force_redraw();
    }

    event.Skip(false);      //do not propagate event
}

//---------------------------------------------------------------------------------------
void DocumentWindow::scroll_line(bool fUp)
{
    int xPos, yPos;
    m_pInteractor->get_viewport(&xPos, &yPos);
    if (fUp)
        yPos -= m_yPixelsPerScrollUnit;
    else
        yPos += m_yPixelsPerScrollUnit;

    #if (LENMUS_PLATFORM_WIN32 == 1)  //---------------------------------------------
    {
        //In Windows, up/down buttons remain enabled even when reaching top/bottom
        if (yPos < m_yMinViewport)
            yPos = m_yMinViewport;
        else if (yPos > m_yMaxViewport)
            yPos = m_yMaxViewport;
    }
    #endif  //-----------------------------------------------------------------------

    m_pInteractor->new_viewport(xPos, yPos);
    m_pInteractor->force_redraw();
}

//---------------------------------------------------------------------------------------
void DocumentWindow::debug_display_ldp_source()
{
    LdpExporter exporter;
    string source = exporter.get_source( m_pDoc->get_imodoc() );
    DlgDebug dlg(this, _T("Generated source code"), to_wx_string(source));
    dlg.ShowModal();
}


}   //namespace lenmus
