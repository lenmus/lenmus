//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2018 LenMus project
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
#include "lenmus_dlg_debug.h"
#include "lenmus_art_provider.h"
//#include "lenmus_msg_box.h"
#include "lenmus_colors.h"

//lomse
#include <lomse_ldp_exporter.h>
#include <lomse_lmd_exporter.h>
#include <lomse_mnx_exporter.h>
#include <lomse_score_player.h>
#include <lomse_midi_table.h>
#include <lomse_player_gui.h>
#include <lomse_command.h>
#include <lomse_logger.h>
#include <lomse_tasks.h>
#include <lomse_interactor.h>
#include <lomse_graphical_model.h>
#include <lomse_internal_model.h>
#include <lomse_im_factory.h>
#include <lomse_visual_effect.h>
#include <lomse_tempo_line.h>
#include <lomse_fragment_mark.h>
//tests
#include "lomse_score_algorithms.h"

//wxWidgets
#include <wx/filename.h>
#include <wx/event.h>

//other
#include <sstream>
#include <stdexcept>
using namespace std;

namespace lenmus
{

//=======================================================================================
// DocumentWindow implementation
//=======================================================================================

//---------------------------------------------------------------------------------------
//constants for menus
const long k_popup_menu_Cut = wxNewId();
//const long k_popup_menu_Copy = wxNewId();
//const long k_popup_menu_Paste = wxNewId();
//const long k_popup_menu_Color = wxNewId();
const long k_popup_menu_Properties = wxNewId();
//const long k_popup_menu_DeleteTiePrev = wxNewId();
//const long k_popup_menu_AttachText = wxNewId();
//const long k_popup_menu_Score_Titles = wxNewId();
//const long k_popup_menu_View_Page_Margins = wxNewId();
//const long k_popup_menu_ToggleStem = wxNewId();
//#ifdef (LOMSE_DEBUG == 1)
//const long k_popup_menu_DumpShape = wxNewId();
//#endif
//const long lmTOOL_VOICE_SOPRANO = wxNewId();
//const long lmTOOL_VOICE_ALTO = wxNewId();
//const long lmTOOL_VOICE_TENOR = wxNewId();
//const long lmTOOL_VOICE_BASS = wxNewId();


//---------------------------------------------------------------------------------------
wxBEGIN_EVENT_TABLE(DocumentWindow, wxWindow)
    EVT_SIZE(DocumentWindow::on_size)
    EVT_MOUSE_EVENTS(DocumentWindow::on_mouse_event)
	EVT_KEY_DOWN(DocumentWindow::on_key_down)
    EVT_PAINT(DocumentWindow::on_paint)
    EVT_SCROLLWIN(DocumentWindow::on_scroll)
    LM_EVT_SCORE_HIGHLIGHT(DocumentWindow::on_visual_tracking)
    EVT_ERASE_BACKGROUND(DocumentWindow::on_erase_background)
    LM_EVT_END_OF_PLAYBACK(DocumentWindow::on_end_of_playback)
    LM_EVT_UPDATE_VIEWPORT(DocumentWindow::on_update_viewport)
wxEND_EVENT_TABLE()

//---------------------------------------------------------------------------------------
DocumentWindow::DocumentWindow(wxWindow* parent, ApplicationScope& appScope,
                         LomseDoorway& lomse)
    : wxWindow(parent, wxNewId(), wxDefaultPosition, wxDefaultSize,
               wxVSCROLL | wxHSCROLL | wxALWAYS_SHOW_SB | wxWANTS_CHARS |
               wxFULL_REPAINT_ON_RESIZE, "DocumentWindow" )
    , m_appScope(appScope)
    , m_lomse(lomse)
    , m_pPresenter(nullptr)
    , m_buffer(nullptr)
    , m_fInvalidBuffer(true)
    , m_filename("")
    , m_zoomMode(k_zoom_fit_width)
    , m_fIgnoreOnSize(false)
    , m_fFirstPaint(true)
    , m_fLoadingDocument(false)
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

    delete_rendering_buffer();
}

//---------------------------------------------------------------------------------------
ADocument DocumentWindow::get_document() const
{
    return m_pPresenter->get_document();
}

//---------------------------------------------------------------------------------------
void DocumentWindow::wrapper_play_score(void* pThis, SpEventInfo pEvent)
{
    //wxLogMessage("callback: wrapper_play_score");
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

        case k_stop_playback_event:
        default:
            play_stop();
            return;
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::play_score(SpEventInfo pEvent)
{
    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        spInteractor->set_operating_mode(Interactor::k_mode_playback);

        SpEventPlayCtrl pEv = static_pointer_cast<EventPlayCtrl>(pEvent);
        ImoScore* pScore = pEv->get_score();
        ScorePlayer* pPlayer  = m_appScope.get_score_player();
        PlayerGui* pPlayerGui = pEv->get_player();

        pPlayer->load_score(pScore, pEv->get_player());
        customize_playback(spInteractor);

        bool fVisualTracking = true;
        long nMM = pPlayerGui->get_metronome_mm();
        pPlayer->play(fVisualTracking, nMM, spInteractor.get());
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::play_active_score(PlayerGui* pGUI)
{
    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        spInteractor->set_operating_mode(Interactor::k_mode_playback);

        AScore score = get_active_score();
        if (score.is_valid())
        {
            ScorePlayer* pPlayer  = m_appScope.get_score_player();
            pPlayer->load_score(score, pGUI);
            customize_playback(spInteractor);
            pPlayer->play(k_do_visual_tracking, 0, spInteractor.get());
        }
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::customize_playback(SpInteractor spInteractor)
{
    //visual tracking during playback
    int trackingMode = m_appScope.get_visual_tracking_mode();
    spInteractor->set_visual_tracking_mode(trackingMode);
    Colors* pColors = m_appScope.get_colors();
    VisualEffect* pVE = spInteractor->get_tracking_effect(k_tracking_tempo_line);
    if (pVE)
    {
        TempoLine* pTL = static_cast<TempoLine*>(pVE);
        pTL->set_color( pColors->tempo_line_color() );
        pTL->set_width(m_appScope.get_tempo_line_width());		            //logical units: 2 mm
    }

    pVE = spInteractor->get_tracking_effect(k_tracking_highlight_notes);
    if (pVE)
    {
        PlaybackHighlight* pPH = static_cast<PlaybackHighlight*>(pVE);
        pPH->set_color( pColors->highlight_color() );
    }
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
void DocumentWindow::on_update_viewport(lmUpdateViewportEvent& event)
{
    LOMSE_LOG_DEBUG(Logger::k_events | Logger::k_score_player, string(""));

    SpEventUpdateViewport pEv = event.get_lomse_event();
    WpInteractor wpInteractor = pEv->get_interactor();
    if (SpInteractor sp = wpInteractor.lock())
    {
        int xPos = pEv->get_new_viewport_x();
        int yPos = pEv->get_new_viewport_y();

        //ensure vertical limits
        if (yPos < m_yMinViewport)
            yPos = m_yMinViewport;
        else if (yPos > m_yMaxViewport)
            yPos = m_yMaxViewport;

        //change viewport
        sp->new_viewport(xPos, yPos);

        //reposition scroll thumb
        SetScrollPos(wxVERTICAL, yPos);
    }
    event.Skip(false);      //do not propagate event
}

//---------------------------------------------------------------------------------------
void DocumentWindow::wrapper_update_window(void* pThis, SpEventInfo pEvent)
{
    if (pEvent->get_event_type() == k_update_window_event)
    {
        LOMSE_LOG_DEBUG(lomse::Logger::k_events | lomse::Logger::k_score_player, string(""));

        DocumentWindow* pWnd = static_cast<DocumentWindow*>(pThis);
        //If this wondow has been hidden (i.e., when opening another eBook)
        //ignore any paint event caused by the open ebooks dialog or other
        if (pWnd->IsShownOnScreen())
        {
            SpEventPaint pEv( static_pointer_cast<EventPaint>(pEvent) );
            static_cast<DocumentWindow*>(pThis)->update_window(pEv->get_damaged_rectangle());
        }
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::update_window(VRect WXUNUSED(damagedRect))
{
    // Invoking update_window() results in just putting immediately the content
    // of the currently rendered buffer to the window without neither calling
    // any lomse methods nor generating any events (i.e. window on_paint)

    LOMSE_LOG_DEBUG(Logger::k_mvc | Logger::k_score_player,
                    "File: %s", m_fullNameWithPath.ToStdString().c_str());

    wxClientDC dc(this);
    //AWARE: use always copy method: no speed gain and no need to debug OverlaysGenerator
    //in relation to Caret rectangle
//    if (damagedRect == VRect(0,0,0,0))
        copy_buffer_on_dc(dc);
//    else
//        blt_buffer_on_dc(dc, damagedRect);
}

//---------------------------------------------------------------------------------------
void DocumentWindow::copy_buffer_on_dc(wxDC& dc)
{
    LOMSE_LOG_DEBUG(Logger::k_mvc, string(""));

    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        if (!m_buffer || !m_buffer->IsOk() || m_buffer->GetSize() != GetClientSize() )
        {
            LOMSE_LOG_DEBUG(Logger::k_mvc, "No buffer or it is not OK.");
            return;
        }

        wxBitmap bitmap(*m_buffer);
        dc.DrawBitmap(bitmap, 0, 0, false /* don't use mask */);

        SetFocus();
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::blt_buffer_on_dc(wxDC& dc, VRect damagedRect)
{
    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        //wxLogMessage("blt_buffer_on_dc %0x", this);
        if (!m_buffer || !m_buffer->IsOk())
            return;

        //blit this memory buffer onto screen buffer
        wxCoord xsrc(damagedRect.x);
        wxCoord ysrc(damagedRect.y);
        wxCoord width(damagedRect.width);
        wxCoord height(damagedRect.height);
        if (width > 0 && height > 0)
        {
            wxBitmap bitmap(*m_buffer);
            wxMemoryDC tempDC;
            tempDC.SelectObject(bitmap);
            dc.Blit (xsrc, ysrc, width, height, &tempDC, xsrc, ysrc);

//            //DEBUG: draw blit rectangle --------------------------------------
//            dc.SetPen(*wxCYAN_PEN);
//            wxCoord x1 = xsrc+4;
//            wxCoord y1 = ysrc+4;
//            wxCoord x2 = xsrc+width-8;
//            wxCoord y2 = ysrc+height-8;
//            dc.DrawLine(x1, y1, x2, y1);
//            dc.DrawLine(x2, y1, x2, y2);
//            dc.DrawLine(x1, y2, x2, y2);
//            dc.DrawLine(x1, y1, x1, y2);
//            //END DEBUG -------------------------------------------------------
        }

        SetFocus();
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::on_visual_tracking(lmVisualTrackingEvent& event)
{
    LOMSE_LOG_DEBUG(lomse::Logger::k_events | Logger::k_score_player, string(""));

    SpEventVisualTracking pEv = event.get_lomse_event();
    WpInteractor wpInteractor = pEv->get_interactor();
    if (SpInteractor sp = wpInteractor.lock())
        sp->on_visual_tracking(pEv);
}

//---------------------------------------------------------------------------------------
void DocumentWindow::on_end_of_playback(lmEndOfPlaybackEvent& event)
{
    LOMSE_LOG_DEBUG(lomse::Logger::k_events | lomse::Logger::k_score_player, string(""));

    SpEventEndOfPlayback pEv = event.get_lomse_event();
    WpInteractor wpInteractor = pEv->get_interactor();
    if (SpInteractor spInteractor = wpInteractor.lock())
    {
        LOMSE_LOG_TRACE(lomse::Logger::k_events | lomse::Logger::k_score_player,
                        "Interactor is valid");
        spInteractor->on_end_of_play_event(pEv->get_score(), pEv->get_player());
    }
    else
        LOMSE_LOG_TRACE(lomse::Logger::k_events | lomse::Logger::k_score_player,
                        "Event is obsolete");
}

//---------------------------------------------------------------------------------------
void DocumentWindow::display_document(LdpReader& reader, int viewType,
                                      const string& WXUNUSED(title))
{
    LOMSE_LOG_DEBUG(Logger::k_mvc, string(""));

    ScorePlayer* pPlayer  = m_appScope.get_score_player();
    pPlayer->stop();

    //get lomse reporter
    ostringstream& reporter = m_appScope.get_lomse_reporter();
    reporter.str(std::string());      //remove any previous content

    try
    {
        ::wxSetCursor(*wxHOURGLASS_CURSOR);
        delete m_pPresenter;
        m_fLoadingDocument = true;
        m_pPresenter = m_lomse.open_document(viewType, reader, reporter);
        m_fLoadingDocument = false;
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
    ScorePlayer* pPlayer  = m_appScope.get_score_player();
    pPlayer->quit();
//    //wait 500 ms for termination
//    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    LOMSE_LOG_DEBUG(Logger::k_score_player, "Going to display %s", filename.c_str());

    //get lomse reporter
    ostringstream& reporter = m_appScope.get_lomse_reporter();
    reporter.str(std::string());      //remove any previous content

    try
    {
        ::wxSetCursor(*wxHOURGLASS_CURSOR);
        delete m_pPresenter;
        m_fLoadingDocument = true;
        m_pPresenter = m_lomse.open_document(viewType, filename, reporter);
        m_fLoadingDocument = false;

        //use filename (without path) as page title
        m_fullNameWithPath = to_wx_string(filename);
        wxFileName oFN(m_fullNameWithPath);
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
void DocumentWindow::display_new_document(const wxString& filename, int viewType)
{
    ScorePlayer* pPlayer  = m_appScope.get_score_player();
    pPlayer->stop();

    //get lomse reporter
    ostringstream& reporter = m_appScope.get_lomse_reporter();
    reporter.str(std::string());      //remove any previous content

    try
    {
        ::wxSetCursor(*wxHOURGLASS_CURSOR);
        delete m_pPresenter;
        m_fLoadingDocument = true;
        m_pPresenter = m_lomse.new_document(viewType);
        m_fLoadingDocument = false;

        //use filename (without path) as page title
        m_fullNameWithPath = filename;
        wxFileName oFN(m_fullNameWithPath);
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
        wxString title = "Errors in file ";
        title += m_filename;
        DlgDebug dlg(this, title, msg, true /*show 'Save' button*/);
        dlg.ShowModal();
    }
    reporter.str(std::string());      //remove any previous content
}

//---------------------------------------------------------------------------------------
void DocumentWindow::do_display(ostringstream& reporter)
{
    LOMSE_LOG_DEBUG(Logger::k_mvc, string(""));

    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        //register to receive desired events
        spInteractor->add_event_handler(k_update_window_event, this, wrapper_update_window);
        spInteractor->add_event_handler(k_do_play_score_event, this, wrapper_play_score);
        spInteractor->add_event_handler(k_pause_score_event, this, wrapper_play_score);
        spInteractor->add_event_handler(k_stop_playback_event, this, wrapper_play_score);

        //set viewport and scale
        m_fFirstPaint = true;


        //AWARE: after creating a pane and loading content on it, wxAuiNotebook / wxFrame
        //will issue an on_size() followed by an on_paint. Therefore, do not force a
        //a repaint here as it will be redundant with the coming events
        Refresh(false /* don't erase background */);

        display_errors(reporter);
    }
}

//---------------------------------------------------------------------------------------
Interactor* DocumentWindow::get_interactor() const
{
    WpInteractor wp = m_pPresenter->get_interactor(0);
    if (SpInteractor sp = wp.lock())
        return sp.get();
    else
        return nullptr;
}

//---------------------------------------------------------------------------------------
SpInteractor DocumentWindow::get_interactor_shared_ptr() const
{
    return m_pPresenter->get_interactor(0).lock();
}

//---------------------------------------------------------------------------------------
void DocumentWindow::on_size(wxSizeEvent& WXUNUSED(event))
{
    LOMSE_LOG_DEBUG(Logger::k_mvc, string(""));

    if (m_pPresenter)
    {
        if(SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
        {
            if (!spInteractor.get()) return;
            if (m_fIgnoreOnSize || m_fInvalidBuffer) return;

            adjust_scale_and_scrollbars();
        }
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::on_paint(wxPaintEvent& WXUNUSED(event))
{
    LOMSE_LOG_DEBUG(Logger::k_mvc, string(""));

    if (!IsShown() || m_pPresenter == nullptr)
        return;

    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        //AWARE: According wxWidgets documentation, any paint event handler must always
        //create a wxPaintDC object, even if not used. Otherwise, under MS Windows,
        // refreshing for this and other windows will go wrong.
        wxPaintDC dc(this);

        Interactor* pInteractor = spInteractor.get();
        if (pInteractor)
        {
            if (m_fFirstPaint)
            {
                m_fFirstPaint = false;
                create_rendering_buffer();
                if (m_fInvalidBuffer)
                    return;
                determine_scroll_space_size();
                adjust_scale_and_scrollbars();
                pInteractor->new_viewport(-m_xMargin, -m_yMargin, k_no_redraw);
            }
            update_rendering_buffer();
            if (!m_fInvalidBuffer)
            {
                copy_buffer_on_dc(dc);
                ::wxSetCursor(*wxSTANDARD_CURSOR);
            }
        }
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::on_mouse_event(wxMouseEvent& event)
{
    if (!m_pPresenter)
        return;

    //filter out non-handled events
    wxEventType nEventType = event.GetEventType();
    if (nEventType==wxEVT_MIDDLE_DOWN || nEventType==wxEVT_MIDDLE_UP ||
        nEventType==wxEVT_MIDDLE_DCLICK)
    {
        return;
    }

    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        Interactor* pInteractor = spInteractor.get();
        if (!pInteractor) return;

        wxPoint pos = event.GetPosition();
        unsigned flags = get_mouse_flags(event);

        if (nEventType == wxEVT_ENTER_WINDOW)
        {
                pInteractor->on_mouse_enter_window(pos.x, pos.y, flags);
                //TODO: Change mouse icon as appropriate
        }

        else if (nEventType == wxEVT_LEAVE_WINDOW)
        {
                pInteractor->on_mouse_leave_window(pos.x, pos.y, flags);
                //TODO: Change mouse icon as appropriate
        }

        else if (nEventType == wxEVT_LEFT_DOWN)
        {
                flags |= k_mouse_left;
                pInteractor->on_mouse_button_down(pos.x, pos.y, flags);
        }

        else if (nEventType == wxEVT_LEFT_UP)
        {
                flags |= k_mouse_left;
                pInteractor->on_mouse_button_up(pos.x, pos.y, flags);
        }

        else if (nEventType == wxEVT_RIGHT_DOWN)
        {
                flags |= k_mouse_right;
                pInteractor->on_mouse_button_down(pos.x, pos.y, flags);
        }

        else if (nEventType == wxEVT_RIGHT_UP)
        {
                flags |= k_mouse_right;
                pInteractor->on_mouse_button_up(pos.x, pos.y, flags);
        }

        else if (nEventType == wxEVT_MOUSEWHEEL)
        {
            if (flags & k_kbd_ctrl)
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
        {
            pInteractor->screen_point_to_model_point(pos.x, pos.y);
            pInteractor->on_mouse_move(pos.x, pos.y, flags);
        }
    }
}

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
    LOMSE_LOG_DEBUG(Logger::k_mvc, string(""));

    if (!m_pPresenter)
        return;

    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        //wxLogMessage("on_document_updated %0x", this);
        spInteractor->on_document_updated();
        Refresh(false /* don't erase background */);
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::update_rendering_buffer()
{
    LOMSE_LOG_DEBUG(Logger::k_mvc, string(""));

    if (!m_pPresenter)
        return;

    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        Interactor* pInteractor = spInteractor.get();
        if (pInteractor && (pInteractor->view_needs_repaint() || !is_buffer_ok()) )
        {
            if (!is_buffer_ok())
                create_rendering_buffer();
            if (!m_fInvalidBuffer)
                pInteractor->redraw_bitmap();
        }
    }
}

//---------------------------------------------------------------------------------------
bool DocumentWindow::is_buffer_ok()
{
    if (m_fInvalidBuffer)
        return false;

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
    //LOMSE_LOG_DEBUG(Logger::k_mvc, string(""));

    //creates a bitmap of specified size and associates it to the rendering
    //buffer for the view. Any existing buffer is automatically deleted

    //I will use a wxImage as the rendering  buffer. wxImage is platform independent
    //and its buffer is an array of characters in RGBRGBRGB... format,  in the
    //top-to-bottom, left-to-right order. That is, the first RGB triplet corresponds
    //to the first pixel of the first row; the second RGB triplet, to the second
    //pixel of the first row, and so on until the end of the first row,
    //with second row following after it and so on.

    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        wxSize size = this->GetClientSize();
        int width = size.GetWidth();
        int height = size.GetHeight();

        //LOMSE_LOG_DEBUG(Logger::k_mvc, "w=%d, h=%d", width, height);

        m_fInvalidBuffer = (width <= 10 || height <= 10);
        if (m_fInvalidBuffer)
            return;

        // allocate a new rendering buffer
        delete m_buffer;            //delete any previous buffer
        m_nBufWidth = width;
        m_nBufHeight = height;
        m_buffer = LENMUS_NEW wxImage(width, height);

        unsigned char* pdata = m_buffer->GetData();   //ptr to the real bytes buffer

        spInteractor->set_rendering_buffer(pdata, m_nBufWidth, m_nBufHeight);
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::on_key_down(wxKeyEvent& event)
{
    switch ( event.GetKeyCode() )
    {
        case WXK_SHIFT:
        case WXK_ALT:
        case WXK_CONTROL:
            break;      //do nothing

        default:
            process_key(event);
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::process_key(wxKeyEvent& event)
{
    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        //check for document scroll commands
        if (process_document_navigation_commands(event))
            return;
    }

    //If a key down (EVT_KEY_DOWN) event is caught and the event handler does not
    //call event.Skip() then the corresponding char event (EVT_CHAR) will not happen.
    //This is by design of wxWidgets and enables programs that handle both types of
    //events to be a bit simpler.
    event.Skip();       //to generate Key char event
}

//---------------------------------------------------------------------------------------
bool DocumentWindow::process_document_navigation_commands(wxKeyEvent& event)
{
    //returns true if command processed

    bool fEventProcessed = true;
    switch (event.GetKeyCode())
    {
        case WXK_PAGEUP:
            scroll_page(true /*true=up*/);
            break;

        case WXK_PAGEDOWN:
            scroll_page(false /*false=down*/);
            break;

        default:
            fEventProcessed = false;
    }
    return fEventProcessed;
}

//---------------------------------------------------------------------------------------
void DocumentWindow::set_debug_draw_box(int boxType)
{
    if (!m_pPresenter)
        return;

    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        spInteractor->reset_boxes_to_draw();
        spInteractor->set_box_to_draw(boxType);

        Refresh(false /* don't erase background */);
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::remove_drawn_boxes()
{
    if (!m_pPresenter)
        return;

    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        spInteractor->reset_boxes_to_draw();
        Refresh(false /* don't erase background */);
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::set_rendering_option(int option, bool value)
{
    if (!m_pPresenter)
        return;

    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        spInteractor->set_rendering_option(option, value);
        Refresh(false /* don't erase background */);
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::zoom_to(double scale)
{
    //do zoom, centered on window center

    if (!m_pPresenter)
        return;

    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        wxSize size = this->GetClientSize();
        spInteractor->set_scale(scale, size.GetWidth()/2, 0, k_no_redraw);
        m_zoomMode = k_zoom_user;
        adjust_scrollbars();
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::zoom_in()
{
    if (!m_pPresenter)
        return;

    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        wxSize size = this->GetClientSize();
        spInteractor->zoom_in(size.GetWidth()/2, 0, k_no_redraw);
        m_zoomMode = k_zoom_user;
        adjust_scrollbars();
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::zoom_out()
{
    if (!m_pPresenter)
        return;

    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        wxSize size = this->GetClientSize();
        spInteractor->zoom_out(size.GetWidth()/2, 0, k_no_redraw);
        m_zoomMode = k_zoom_user;
        adjust_scrollbars();
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::zoom_fit_width()
{
    if (!m_pPresenter)
        return;

    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        //wxLogMessage("zoom_fit_width %0x", this);
        wxSize size = this->GetClientSize();
        spInteractor->zoom_fit_width(size.GetWidth(), k_no_redraw);
        m_zoomMode = k_zoom_fit_width;
        adjust_scrollbars();
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::zoom_fit_full()
{
    if (!m_pPresenter)
        return;

    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        //wxLogMessage("zoom_fit_full %0x", this);
        wxSize size = this->GetClientSize();
        spInteractor->zoom_fit_full(size.GetWidth(), size.GetHeight(), k_no_redraw);
        m_zoomMode = k_zoom_fit_full;
        adjust_scrollbars();
    }
}

//---------------------------------------------------------------------------------------
string DocumentWindow::generate_checkpoint_data()
{
    Document* pDoc = m_pPresenter->get_document_raw_ptr();
    return pDoc->get_checkpoint_data();
}

//---------------------------------------------------------------------------------------
string DocumentWindow::generate_lmd_source(int scoreFormat)
{
    Document* pDoc = m_pPresenter->get_document_raw_ptr();
    LmdExporter exporter( *(m_lomse.get_library_scope()) );;
    exporter.set_score_format(scoreFormat);
    return exporter.get_source( pDoc->get_im_root() );
}

//---------------------------------------------------------------------------------------
string DocumentWindow::generate_mnx_source()
{
    Document* pDoc = m_pPresenter->get_document_raw_ptr();
    MnxExporter exporter( *(m_lomse.get_library_scope()) );;
    return exporter.get_source( pDoc->get_im_root() );
}

//---------------------------------------------------------------------------------------
string DocumentWindow::generate_ldp_source()
{
    Document* pDoc = m_pPresenter->get_document_raw_ptr();
    LdpExporter exporter;
    exporter.set_add_id(true);
    return exporter.get_source( pDoc->get_im_root() );
}

//---------------------------------------------------------------------------------------
string DocumentWindow::dump_cursor()
{
    if (!m_pPresenter)
        return "dump_cursor(): Error. No Presenter available!";

    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        return spInteractor->dump_cursor();
    }
    else
        return "dump_cursor(): Error. No access to Interactor!";
}

//---------------------------------------------------------------------------------------
string DocumentWindow::dump_selection()
{
    if (!m_pPresenter)
        return "dump_cursor(): Error. No Presenter available!";

    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        return spInteractor->dump_selection();
    }
    else
        return "dump_cursor(): Error. No access to Interactor!";
}

//---------------------------------------------------------------------------------------
AScore DocumentWindow::get_active_score()
{
    //TO_FIX: This method assumes that the active score is the first one and that
    //        the document contains at least one score.
    ADocument doc = m_pPresenter->get_document();
    return doc.first_score();
}

//---------------------------------------------------------------------------------------
void DocumentWindow::get_pages_info(int* pMinPage, int* pMaxPage,
                                 int* pSelPageFrom, int* pSelPageTo)
{
    //Return the default page range to be printed and the page range the user can
    //select for printing.

    if (!m_pPresenter)
        return;

    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        *pMinPage = 1;
        *pMaxPage = spInteractor->get_num_pages();
        *pSelPageFrom = 1;
        *pSelPageTo = *pMaxPage;
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::do_print(wxDC* pDC, int page, int paperWidthPixels,
                              int paperHeightPixels)
{
    if (!m_pPresenter)
        return;

    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        pDC->SetBackground(*wxWHITE_BRUSH);
        pDC->Clear();

        ADocument doc = get_document();
        float scale = doc.page_content_scale();
        pDC->SetUserScale(scale, scale);
        paperWidthPixels = int(float(paperWidthPixels) / scale);
        paperHeightPixels = int(float(paperHeightPixels) / scale);


        //set print resolution
        wxSize dpi = pDC->GetPPI();
        spInteractor->set_print_ppi( double( max(dpi.x, dpi.y) ) );

#if 0   //1=print in a single bitmap, 0-print in tiles. Both solutions tested and both work

        //allocate print buffer
        wxImage* buffer = LENMUS_NEW wxImage(paperWidthPixels, paperHeightPixels);
        unsigned char* pdata = buffer->GetData();       //ptr to the real bytes buffer
        spInteractor->set_print_buffer(pdata, paperWidthPixels, paperHeightPixels);

        //print page
        spInteractor->print_page(page-1);

        wxBitmap bitmap = *buffer;
        if (bitmap.Ok())
        {
            pDC->DrawBitmap(bitmap, 0, 0, false /* don't use mask */);
        }

        delete buffer;
    }

#else

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
        int rows = int( ceil(float(paperHeightPixels) / float(tileH)) );
        int cols = int( ceil(float(paperWidthPixels) / float(tileW)) );

        //determine last row and last column tile sizes
        int lastW = paperWidthPixels - tileW * (cols - 1);
        int lastH = paperHeightPixels - tileH * (rows - 1);

//        wxMessageBox( wxString::Format("paper (%d, %d), tile (%d, %d), last tile (%d, %d), cols=%d, rows=%d",
//            paperWidthPixels, paperHeightPixels, tileW, tileH, lastW, lastH, cols, rows));

        //allocate tile buffer
        wxImage* buffer = LENMUS_NEW wxImage(width, height);
        unsigned char* pdata = buffer->GetData();       //ptr to the real bytes buffer
        spInteractor->set_print_buffer(pdata, width, height);

        //loop to print tiles.
        wxMemoryDC memoryDC;
        for (int iRow=0; iRow < rows; ++iRow)
        {
            for (int iCol=0; iCol < cols; ++iCol)
            {
                spInteractor->print_page(page-1, viewport);

                //print this tile
                int tileWidth = (iCol == cols-1 ? lastW : tileW);
                int tileHeight = (iRow == rows-1 ? lastH : tileH);

                #if 1       //AWARE: both solutions tested and both work
                    wxBitmap bitmap = *buffer;
                    if (bitmap.Ok())
                    {
                        if (border > 0)
                        {
                            memoryDC.SelectObjectAsSource(bitmap);
                            pDC->Blit(paperPos.x, paperPos.y, tileWidth, tileHeight,
                                    &memoryDC, border, border);
//                            //print tile borders
//                            pDC->DrawLine(paperPos.x, paperPos.y,
//                                          paperPos.x+tileWidth, paperPos.y);
//                            pDC->DrawLine(paperPos.x+tileWidth, paperPos.y,
//                                          paperPos.x+tileWidth, paperPos.y+tileHeight);

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
                viewport.x += tileW;
                paperPos.x += tileW;
            }
            //start new row
            viewport.x = 0;
            viewport.y += tileH;
            paperPos.x = 0;
            paperPos.y += tileH;
        }

        delete buffer;
    }
#endif // 1
}

//---------------------------------------------------------------------------------------
void DocumentWindow::adjust_scale_and_scrollbars()
{
    LOMSE_LOG_DEBUG(Logger::k_mvc, string(""));

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
    LOMSE_LOG_DEBUG(Logger::k_mvc, string(""));

    if (!m_pPresenter)
        return;

    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        //total size of the rendered document (whole visual space, all pages)
        spInteractor->get_view_size(&m_xScrollSpaceWidth, &m_yScrollSpaceHeight);
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
}

//---------------------------------------------------------------------------------------
void DocumentWindow::adjust_scrollbars()
{
    LOMSE_LOG_DEBUG(Logger::k_mvc, string(""));

    if (!m_pPresenter)
        return;

    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        determine_scroll_space_size();

        //get size of scroll page (client area size)
        m_xScrollPageWidth = m_nBufWidth;
        m_yScrollPageHeight = m_nBufHeight;

        //get viewport limits (in pixels)
        m_xMaxViewport = (m_xScrollSpaceWidth - m_xMargin) - m_xScrollPageWidth;
        m_yMaxViewport = (m_yScrollSpaceHeight - m_yMargin) - m_yScrollPageHeight;

        //get current viewport position
        int xCurPos, yCurPos;
        spInteractor->get_viewport(&xCurPos, &yCurPos);

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
}

//---------------------------------------------------------------------------------------
void DocumentWindow::on_scroll(wxScrollWinEvent& event)
{
    LOMSE_LOG_DEBUG(Logger::k_mvc, string(""));

    if (!m_pPresenter)
        return;

    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        int xPos, yPos;
        spInteractor->get_viewport(&xPos, &yPos);

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

            //limit scrolling when reaching document top/bottom
            if (yPos < m_yMinViewport)
                yPos = m_yMinViewport;
            else if (yPos > m_yMaxViewport)
                yPos = m_yMaxViewport;

            //reposition the scroll thumb
            SetScrollPos(wxVERTICAL, m_yMargin + yPos);

            //reposition the document viewport
            spInteractor->new_viewport(xPos, yPos); //, k_no_redraw);    //BUG_001
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

            //limit scrolling when reaching document left/right
            if (xPos < m_xMinViewport)
                xPos = m_xMinViewport;
            else if (xPos > m_xMaxViewport)
                xPos = m_xMaxViewport;

            //reposition the scroll thumb
            SetScrollPos(wxHORIZONTAL, m_xMargin + xPos);

            //reposition the document viewport
            spInteractor->new_viewport(xPos, yPos); //, k_no_redraw);    //BUG_001
        }
    }
    event.Skip(false);      //do not propagate event
}

//---------------------------------------------------------------------------------------
void DocumentWindow::scroll_line(bool fUp)
{
    LOMSE_LOG_DEBUG(Logger::k_mvc, string(""));

    if (!m_pPresenter)
        return;

    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        int xPos, yPos;
        spInteractor->get_viewport(&xPos, &yPos);
        if (fUp)
            yPos -= m_yPixelsPerScrollUnit;
        else
            yPos += m_yPixelsPerScrollUnit;

        //limit scrolling when reaching document top/bottom
        if (yPos < m_yMinViewport)
            yPos = m_yMinViewport;
        else if (yPos > m_yMaxViewport)
            yPos = m_yMaxViewport;

        //reposition the document viewport
        spInteractor->new_viewport(xPos, yPos);     //, k_no_redraw);    //BUG_001
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::scroll_page(bool fUp)
{
    if (!m_pPresenter)
        return;

    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        int xPos, yPos;
        spInteractor->get_viewport(&xPos, &yPos);

        if (fUp)
            yPos -= m_yScrollPageHeight;
        else
            yPos += m_yScrollPageHeight;

        //limit scrolling when reaching document top/bottom
        if (yPos < m_yMinViewport)
            yPos = m_yMinViewport;
        else if (yPos > m_yMaxViewport)
            yPos = m_yMaxViewport;

        //reposition the scroll thumb
        SetScrollPos(wxVERTICAL, m_yMargin + yPos);

        //reposition the document viewport
        spInteractor->new_viewport(xPos, yPos); //, k_no_redraw);    //BUG_001
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::debug_dump_spacing_data()
{
    DlgDebug dlg(this, "Spacing data", "??????");
                 //to_wx_string(pDoc->dump_spacing_data()) );
    dlg.ShowModal();
}

//---------------------------------------------------------------------------------------
void DocumentWindow::debug_do_api_test()
{
    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        Document* pDoc = m_pPresenter->get_document_raw_ptr();
        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );

        if (pScore)
        {
            ImoId scoreId = pScore->get_id();

            //green mark: open rounded at staffobj (key signature).
            //From second instrument to last one
            ScoreCursor cursor(pDoc, pScore);     //cursor points to clef
            cursor.move_next();         //now points to key signature
            ImoStaffObj* pSO = dynamic_cast<ImoStaffObj*>(*(cursor));
            FragmentMark* mark = spInteractor->add_fragment_mark_at_staffobj(pSO);
            mark->color(Color(0,255,0))->top(1);
            mark->type(k_mark_open_rounded);

            cursor.move_next();         //now points to time signature
            pSO = dynamic_cast<ImoStaffObj*>(*(cursor));
            mark = spInteractor->add_fragment_mark_at_staffobj(pSO);
            mark->color(Color(0,255,0))->top(1);
            mark->type(k_mark_close_rounded);

            //blue curly marks: at first barline (measure 1, beat 0) and second
            //barline  (measure 2, beat 0). The whole system
            TimeUnits timepos = ScoreAlgorithms::get_timepos_for(pScore, 1, 0);
            mark = spInteractor->add_fragment_mark_at_barline(scoreId, timepos);
            mark->color(Color(0,0,255,128))->type(k_mark_open_curly);

            timepos = ScoreAlgorithms::get_timepos_for(pScore, 2, 0);
            mark = spInteractor->add_fragment_mark_at_barline(scoreId, timepos);
            mark->color(Color(0,0,255,128))->type(k_mark_close_curly);

            //red line mark: first note after first barline (measure 1, beat 0)
            timepos = ScoreAlgorithms::get_timepos_for(pScore, 1, 0);
            spInteractor->add_fragment_mark_at_note_rest(scoreId, timepos);

            //magenta line mark: at interpolated timepos. Only third instrument
            timepos += k_duration_eighth;    //at first quarter dotted note position
            mark = spInteractor->add_fragment_mark_at_note_rest(scoreId, timepos);
            mark->color(Color(255,0,255,128))->type(k_mark_line)->top(2);

            //solid blue curly mark: instruments 2,3 & 4
            timepos = ScoreAlgorithms::get_timepos_for(pScore, 1, 1);
            mark = spInteractor->add_fragment_mark_at_note_rest(scoreId, timepos);
            mark->color(Color(0,0,255))->top(1,0)->bottom(3);
            mark->type(k_mark_open_curly)->x_shift(-5.0);   //some space before

            timepos = ScoreAlgorithms::get_timepos_for(pScore, 1, 2);
            mark = spInteractor->add_fragment_mark_at_note_rest(scoreId, timepos);
            mark->color(Color(0,0,255))->top(1,0)->bottom(3);
            mark->type(k_mark_close_curly)->x_shift(25.0);  //some space to skip noteheads

            //magenta squared mark: instruments 2,3 & 4
            timepos = ScoreAlgorithms::get_timepos_for(pScore, 2,1);
            mark = spInteractor->add_fragment_mark_at_note_rest(scoreId, timepos);
            mark->color(Color(255,0,255,128))->top(1,0)->bottom(3);
            mark->type(k_mark_open_squared);

            timepos = ScoreAlgorithms::get_timepos_for(pScore, 2,2);
            mark = spInteractor->add_fragment_mark_at_note_rest(scoreId, timepos);
            mark->color(Color(255,0,255,128))->top(1,0)->bottom(3);
            mark->type(k_mark_close_squared);

            //green line mark: instruments 2,3 & 4
            timepos = ScoreAlgorithms::get_timepos_for(pScore, 3,0);
            mark = spInteractor->add_fragment_mark_at_note_rest(scoreId, timepos);
            mark->color(Color(0, 255,0))->top(1,0)->bottom(3);
            mark->type(k_mark_line);

            //red rounded mark: instruments 2,3 & 4
            timepos = ScoreAlgorithms::get_timepos_for(pScore, 4,0);
            mark = spInteractor->add_fragment_mark_at_note_rest(scoreId, timepos);
            mark->color(Color(255,0,0))->top(1,0)->bottom(3);
            mark->type(k_mark_open_rounded);

            timepos = ScoreAlgorithms::get_timepos_for(pScore, 4,1);
            mark = spInteractor->add_fragment_mark_at_note_rest(scoreId, timepos);
            mark->color(Color(255,0,0))->top(1,0)->bottom(3);
            mark->type(k_mark_close_rounded);

//            spInteractor->remove_mark(mark);
        }
    }

    //Test 2 ===================
    //create empty document
//    delete m_pPresenter;
//    m_pPresenter = m_lomse.new_document(k_view_vertical_book);

//    //Add content to the document
//    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
//    {
//        //get the document to edit
//        Document* pDoc = m_pPresenter->get_document_raw_ptr();
//
//        //add an empty score to the document
//        ImoScore* pScore = pDoc->add_score();
//
//        //add an instrument (an score part) to the score
//        ImoInstrument* pInstr = pScore->add_instrument();
//
//        //add some content to this instrument
//        pInstr->add_clef(k_clef_G2);			//G clef on second line
//        pInstr->add_key_signature(k_key_D);		//D major key signature
//        pInstr->add_time_signature(4, 4);		//4/4 time signature
//
//        //create the first note to insert: D4 half note
//        //The following code shows the right way for creating nodes before
//        //adding them to the model by using the ImFactory object (in lomse_im_factory.h).
//        ImoNote* pNote = static_cast<ImoNote*>( ImFactory::inject(k_imo_note, pDoc) );
//        pNote->set_note_type_and_dots(k_half, 0);
//        pNote->set_voice(1);
//        pNote->set_notated_pitch(k_step_D, 4, k_no_accidentals);
//        pInstr->insert_staffobj_at(nullptr, pNote);		//append at end
//
//        pInstr->add_object("(r h)");            //add a rest, duration: half
//        pInstr->add_barline(k_barline_simple);  //add barline to finish first measure
//        pInstr->add_object("(n f4 q.)");        //add note: flat F4 dotted quarter note
//        pInstr->add_object("(n a4 e)");         //add the third note: A4 8th note
//
//        //update the internal data structures
//        pScore->end_of_changes();
//
////        DlgDebug dlg(this, "Low level API",
////                     to_wx_string(pScore->get_version_string()) );
////        dlg.ShowModal();
//    }

    //Test 1 ===================
//    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
//    {
//        //get the document to edit
//        Document* pDoc = m_pPresenter->get_document_raw_ptr();
//
//        //Use the document to get the score to edit
//        //The next line of code is just an example, in which it is assumed that
//        //the score to edit is the first element in the document, as it is the case if we
//        //use the document created in previous example. Also, this will be always the
//        //case when editing MusicXML imported files.
//        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
//
//        if (pScore)
//        {
//            //get the instrument (score part) to be edited. In this example, it is
//            //the first instrument.
//            ImoInstrument* pInstr = pScore->get_instrument(0);
//
//            //for inserting notes or other objects it is necessary to determine the
//            //insertion point. For locating the insertion point there are
//            //many possibilities, usually involving an iterator or a cursor object.
//            //Lets use a cursor object:
//            ScoreCursor cursor(pDoc, pScore);
//
//            //after creation the cursor will be pointing to the first object in
//            //the score, the clef in this example
//            cursor.move_next();		//now points to key signature
//            cursor.move_next();		//now points to time signature
//            cursor.move_next();		//done! now points to the first note, the D5 quarter note
//            ImoStaffObj* pAt = cursor.staffobj();        //get pointed object
//
//            //change stem of note D5 to go up
//            ImoNote* pNote = static_cast<ImoNote*>(pAt);
//            pNote->set_stem_direction(k_stem_up);
//
//            //insert a half D4 note in voice 2, with stem down
//            stringstream errormsg;
//            if (!pInstr->insert_staffobj_at(pAt, "(n d4 h v2 (stem down))", errormsg))
//            {
//                //error. An explanation is in errormsg
//            }
//
//            //once the updates are finished, invoke close() method for
//            //for updating the internal data structures
//            pScore->end_of_changes();
//        }
//    }


    Refresh(false /* don't erase background */);
}

//---------------------------------------------------------------------------------------
void DocumentWindow::debug_display_ldp_source()
{
    DlgDebug dlg(this, "Generated source code",
                 to_wx_string(generate_ldp_source()) );
    dlg.ShowModal();
}

//---------------------------------------------------------------------------------------
void DocumentWindow::debug_display_lmd_source()
{
    DlgDebug dlg(this, "Generated source code",
                 to_wx_string(generate_lmd_source(LmdExporter::k_format_lmd)) );
    dlg.ShowModal();
}

//---------------------------------------------------------------------------------------
void DocumentWindow::debug_display_mnx_source()
{
    DlgDebug dlg(this, "Generated source code",
                 to_wx_string(generate_mnx_source()) );
    dlg.ShowModal();
}

//---------------------------------------------------------------------------------------
void DocumentWindow::debug_display_checkpoint_data()
{
    DlgDebug dlg(this, "Checkpoint data",
                 to_wx_string(generate_checkpoint_data()) );
    dlg.ShowModal();
}

//---------------------------------------------------------------------------------------
void DocumentWindow::debug_display_cursor_state()
{
    if (SpInteractor spIntor = m_pPresenter->get_interactor(0).lock())
    {
        DocCursor* cursor = spIntor->get_cursor();
        DlgDebug dlg(this, "Cursor state",
                     to_wx_string(cursor->dump_cursor()) );
        dlg.ShowModal();
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::debug_dump_internal_model()
{
    if (m_pPresenter)
    {
        ADocument doc = get_document();
        DlgDebug dlg(this, "Internal Model Dump",
                     to_wx_string(doc.internal_object()->dump_tree()) );
        dlg.ShowModal();
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::debug_display_document_ids()
{
    if (m_pPresenter)
    {
        ADocument doc = get_document();
        DlgDebug dlg(this, "Document Ids dump",
                     to_wx_string(doc.internal_object()->dump_ids()) );
        dlg.ShowModal();
    }
}


}   //namespace lenmus
