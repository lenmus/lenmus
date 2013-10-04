//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2013 LenMus project
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
#include "lenmus_edit_interface.h"
#include "lenmus_command_window.h"      //to be replaced by lomse command parser
#include "lenmus_command_event_handler.h"
#include "lenmus_dlg_properties.h"
#include "lenmus_art_provider.h"
#include "lenmus_tool_box.h"            //enum for mouse modes
#include "lenmus_score_wizard.h"

//lomse
#include <lomse_ldp_exporter.h>
#include <lomse_lmd_exporter.h>
#include <lomse_score_player.h>
#include <lomse_midi_table.h>
#include <lomse_player_gui.h>
#include <lomse_command.h>
#include <lomse_logger.h>
#include <lomse_tasks.h>
#include <lomse_interactor.h>
#include <lomse_graphical_model.h>

//wxWidgets
#include <wx/filename.h>
#include <wx/event.h>

//other
#include <sstream>
#include <stdexcept>
using namespace std;

#include <boost/shared_ptr.hpp>

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
BEGIN_EVENT_TABLE(DocumentWindow, wxWindow)
    EVT_SIZE(DocumentWindow::on_size)
    EVT_MOUSE_EVENTS(DocumentWindow::on_mouse_event)
	EVT_CHAR(DocumentWindow::on_key_press)
	EVT_KEY_DOWN(DocumentWindow::on_key_down)
    EVT_PAINT(DocumentWindow::on_paint)
    EVT_SCROLLWIN(DocumentWindow::on_scroll)
    LM_EVT_SCORE_HIGHLIGHT(DocumentWindow::on_visual_highlight)
    EVT_ERASE_BACKGROUND(DocumentWindow::on_erase_background)
    LM_EVT_END_OF_PLAYBACK(DocumentWindow::on_end_of_playback)
    LM_EVT_SHOW_CONTEXTUAL_MENU(DocumentWindow::on_show_contextual_menu)

	//events for contextual menus
	EVT_MENU	(k_popup_menu_Cut, DocumentWindow::on_popup_cut)
//	EVT_MENU	(k_popup_menu_Copy, DocumentWindow::OnCopy)
//	EVT_MENU	(k_popup_menu_Paste, DocumentWindow::OnPaste)
//	EVT_MENU	(k_popup_menu_Color, DocumentWindow::OnColor)
	EVT_MENU	(k_popup_menu_Properties, DocumentWindow::on_popup_properties)
//	EVT_MENU	(k_popup_menu_DeleteTiePrev, DocumentWindow::OnDeleteTiePrev)
//	EVT_MENU	(k_popup_menu_AttachText, DocumentWindow::OnAttachText)
//	EVT_MENU	(k_popup_menu_Score_Titles, DocumentWindow::OnScoreTitles)
//	EVT_MENU	(k_popup_menu_View_Page_Margins, DocumentWindow::OnViewPageMargins)
//	EVT_MENU	(k_popup_menu_ToggleStem, DocumentWindow::OnToggleStem)
//#ifdef (LOMSE_DEBUG == 1)
//	EVT_MENU	(k_popup_menu_DumpShape, DocumentWindow::OnDumpShape)
//#endif
//  EVT_MENU	(lmTOOL_VOICE_SOPRANO, DocumentWindow::OnToolPopUpMenuEvent)
//	EVT_MENU	(lmTOOL_VOICE_ALTO, DocumentWindow::OnToolPopUpMenuEvent)
//	EVT_MENU	(lmTOOL_VOICE_TENOR, DocumentWindow::OnToolPopUpMenuEvent)
//	EVT_MENU	(lmTOOL_VOICE_BASS, DocumentWindow::OnToolPopUpMenuEvent)
    EVT_CLOSE   (DocumentWindow::on_window_closing)

END_EVENT_TABLE()

//---------------------------------------------------------------------------------------
DocumentWindow::DocumentWindow(wxWindow* parent, ApplicationScope& appScope,
                         LomseDoorway& lomse)
    : wxWindow(parent, wxNewId(), wxDefaultPosition, wxDefaultSize,
               wxVSCROLL | wxHSCROLL | wxALWAYS_SHOW_SB | wxWANTS_CHARS |
               wxFULL_REPAINT_ON_RESIZE, _T("DocumentWindow") )
    , m_appScope(appScope)
    , m_lomse(lomse)
    , m_pPresenter(NULL)
    , m_buffer(NULL)
    , m_filename(_T(""))
    , m_zoomMode(k_zoom_fit_width)
    , m_fIgnoreOnSize(false)
    , m_fFirstPaint(true)
    , m_fEditionEnabled(false)
//    , m_mouseMode(k_mouse_mode_pointer)
    , m_fLoadingDocument(false)
    , m_pContextualMenu(NULL)
    , m_pMenuOwner(NULL)
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
    delete m_pContextualMenu;
}

//---------------------------------------------------------------------------------------
Document* DocumentWindow::get_document() const
{
    return m_pPresenter->get_document_raw_ptr();
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

        SpEventPlayScore pEv = boost::static_pointer_cast<EventPlayScore>(pEvent);
        ImoScore* pScore = pEv->get_score();
        ScorePlayer* pPlayer  = m_appScope.get_score_player();
        PlayerGui* pPlayerGui = pEv->get_player();

        pPlayer->load_score(pScore, pEv->get_player());

        //initialize with default options
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

        ImoScore* pScore = get_active_score();
        if (pScore)
        {
            ScorePlayer* pPlayer  = m_appScope.get_score_player();
            pPlayer->load_score(pScore, pGUI);
            pPlayer->play(k_do_visual_tracking, 0, spInteractor.get());
        }
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::play_stop()
{
    ScorePlayer* pPlayer  = m_appScope.get_score_player();
    pPlayer->stop();

    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        spInteractor->set_operating_mode(m_fEditionEnabled ? Interactor::k_mode_edition
                                                           : Interactor::k_mode_read_only);
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::play_pause()
{
    ScorePlayer* pPlayer  = m_appScope.get_score_player();
    pPlayer->pause();
}

//---------------------------------------------------------------------------------------
void DocumentWindow::wrapper_on_click_event(void* pThis, SpEventInfo pEvent)
{
    static_cast<DocumentWindow*>(pThis)->on_click_event(pEvent);
}

//---------------------------------------------------------------------------------------
void DocumentWindow::on_click_event(SpEventInfo pEvent)
{
    //wxMessageBox(_T("DocumentWindow::on_click_event"));
    SpEventMouse pEv( boost::static_pointer_cast<EventMouse>(pEvent) );
    if (!pEv->is_still_valid())
        return;

    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        SelectionSet& selection = spInteractor->get_selection_set();
        DocCursor* cursor = spInteractor->get_cursor();
        CommandEventHandler handler(this, m_toolsInfo, selection, cursor);
        handler.process_on_click_event(pEv);
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::wrapper_on_command_event(void* pThis, SpEventInfo pEvent)
{
    static_cast<DocumentWindow*>(pThis)->on_command_event(pEvent);
}

//---------------------------------------------------------------------------------------
void DocumentWindow::on_command_event(SpEventInfo pEvent)
{
//    wxMessageBox(_T("DocumentWindow::on_command_event"));
    SpEventCommand pEv( boost::static_pointer_cast<EventCommand>(pEvent) );
    if (!pEv->is_still_valid())
        return;

    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        SelectionSet& selection = spInteractor->get_selection_set();
        DocCursor* cursor = spInteractor->get_cursor();
        CommandEventHandler handler(this, m_toolsInfo, selection, cursor);
        handler.process_command_event(pEv);
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::wrapper_update_window(void* pThis, SpEventInfo pEvent)
{
    //wxLogMessage(_T("callback: wrapper_update_window"));
    SpEventPaint pEv( boost::static_pointer_cast<EventPaint>(pEvent) );
    static_cast<DocumentWindow*>(pThis)->update_window(pEv->get_damaged_rectangle());
}

//---------------------------------------------------------------------------------------
void DocumentWindow::update_window(VRect damagedRect)
{
    // Invoking update_window() results in just putting immediately the content
    // of the currently rendered buffer to the window without neither calling
    // any lomse methods nor generating any events (i.e. window on_paint)

    LOMSE_LOG_DEBUG(Logger::k_mvc, "");

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
    LOMSE_LOG_DEBUG(Logger::k_mvc, "");

    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        if (!m_buffer || !m_buffer->IsOk())
        {
            LOMSE_LOG_DEBUG(Logger::k_mvc, "No buffer or it is not OK.");
            return;
        }

        wxBitmap bitmap(*m_buffer);
        dc.DrawBitmap(bitmap, 0, 0, false /* don't use mask */);

        //DEBUG: info about rendering time -------------------------------------
        spInteractor->timing_repaint_done();
        double* pTimes = spInteractor->get_ellapsed_times();
        wxString msg = wxString::Format(
            _T("gm=%.1f vf=%.1f render=%.1f paint=%.1f ms "),
            *(pTimes + Interactor::k_timing_gmodel_draw_time),
            *(pTimes + Interactor::k_timing_visual_effects_draw_time),
            *(pTimes + Interactor::k_timing_total_render_time),
            *(pTimes + Interactor::k_timing_repaint_time) );

        StatusReporter* pStatus = m_appScope.get_status_reporter();
        pStatus->report_status(msg);
        LOMSE_LOG_DEBUG(Logger::k_mvc, to_std_string(msg));
        //END DEBUG ------------------------------------------------------------

        SetFocus();
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::blt_buffer_on_dc(wxDC& dc, VRect damagedRect)
{
    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        //wxLogMessage(_T("blt_buffer_on_dc %0x"), this);
        if (!m_buffer || !m_buffer->IsOk())
            return;

        //blit this memory buffer onto screen buffer
        wxCoord xsrc(damagedRect.x);
        wxCoord ysrc(damagedRect.y);
        wxCoord width(damagedRect.width);
        wxCoord height(damagedRect.height);
        if (width > 0 and height > 0)
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

        //DEBUG: info about rendering time -------------------------------------
        spInteractor->timing_repaint_done();
        double* pTimes = spInteractor->get_ellapsed_times();
        wxString msg = wxString::Format(
            _T("gm=%.1f vf=%.1f render=%.1f paint=%.1f ms sz(%d,%d) "),
            *(pTimes + Interactor::k_timing_gmodel_draw_time),
            *(pTimes + Interactor::k_timing_visual_effects_draw_time),
            *(pTimes + Interactor::k_timing_total_render_time),
            *(pTimes + Interactor::k_timing_repaint_time),
            damagedRect.width, damagedRect.height );

        StatusReporter* pStatus = m_appScope.get_status_reporter();
        pStatus->report_status(msg);
        //END DEBUG ------------------------------------------------------------

        SetFocus();
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::on_visual_highlight(lmScoreHighlightEvent& event)
{
    SpEventScoreHighlight pEv = event.get_lomse_event();
    WpInteractor wpInteractor = pEv->get_interactor();
    if (SpInteractor sp = wpInteractor.lock())
        sp->on_visual_highlight(pEv);
}

//---------------------------------------------------------------------------------------
void DocumentWindow::on_end_of_playback(lmEndOfPlaybackEvent& event)
{
    LOMSE_LOG_DEBUG(lomse::Logger::k_events | lomse::Logger::k_score_player, "");

    SpEventPlayScore pEv = event.get_lomse_event();
    WpInteractor wpInteractor = pEv->get_interactor();
    if (SpInteractor spInteractor = wpInteractor.lock())
    {
        LOMSE_LOG_TRACE(lomse::Logger::k_events | lomse::Logger::k_score_player,
                        "Interactor is valid");
        spInteractor->send_end_of_play_event(pEv->get_score(), pEv->get_player());
        spInteractor->set_operating_mode(m_fEditionEnabled ? Interactor::k_mode_edition
                                                           : Interactor::k_mode_read_only);
    }
    else
        LOMSE_LOG_TRACE(lomse::Logger::k_events | lomse::Logger::k_score_player,
                        "Event is obsolete");
}

//---------------------------------------------------------------------------------------
void DocumentWindow::on_show_contextual_menu(lmShowContextualMenuEvent& event)
{
    LOMSE_LOG_DEBUG(lomse::Logger::k_events, "");

    SpEventMouse pEvent = event.get_lomse_event();
    if (pEvent->is_still_valid() && m_fEditionEnabled)
    {
        m_pMenuOwner = pEvent->get_imo_object();
        get_contextual_menu(true);
        PopupMenu(m_pContextualMenu, pEvent->get_x(), pEvent->get_y());
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::display_document(LdpReader& reader, int viewType,
                                      const string& title)
{
    LOMSE_LOG_DEBUG(Logger::k_mvc, "");

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
    pPlayer->stop();

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
    LOMSE_LOG_DEBUG(Logger::k_mvc, "");

    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        //connect the View with the window buffer
        spInteractor->set_rendering_buffer(&m_rbuf_window);

        //register to receive desired events
        spInteractor->add_event_handler(k_update_window_event, this, wrapper_update_window);
        spInteractor->add_event_handler(k_do_play_score_event, this, wrapper_play_score);
        spInteractor->add_event_handler(k_pause_score_event, this, wrapper_play_score);
        spInteractor->add_event_handler(k_stop_playback_event, this, wrapper_play_score);
        spInteractor->add_event_handler(k_control_point_moved_event, this, wrapper_on_command_event);
        Document* pDoc = m_pPresenter->get_document_raw_ptr();
        pDoc->add_event_handler(k_on_click_event, this, wrapper_on_click_event);

        //set viewport and scale
        m_fFirstPaint = true;
        //create_rendering_buffer();
        //determine_scroll_space_size();
        //spInteractor->new_viewport(-m_xMargin, -m_yMargin, k_no_redraw);
        //adjust_scale_and_scrollbars();

        //AWARE: after creating a pane and loading content on it, wxAuiNotebook / wxFrame
        //will issue an on_size() followed by an on_paint. Therefore, do not force a
        //a repaint here as it will be redundant with the coming events
        Refresh(false /* don't erase background */);

        spInteractor->set_operating_mode(m_fEditionEnabled ? Interactor::k_mode_edition
                                                           : Interactor::k_mode_read_only);

        ////ensure that the rendering buffer is created
        //if (m_nBufWidth == 0 || m_nBufHeight == 0)
        //    create_rendering_buffer();

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
        return NULL;
}

//---------------------------------------------------------------------------------------
SpInteractor DocumentWindow::get_interactor_shared_ptr() const
{
    return m_pPresenter->get_interactor(0).lock();
}

//---------------------------------------------------------------------------------------
void DocumentWindow::on_size(wxSizeEvent& WXUNUSED(event))
{
    LOMSE_LOG_DEBUG(Logger::k_mvc, "");

    if (m_pPresenter)
    {
        if(SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
        {
            if (!spInteractor.get()) return;
            if (m_fIgnoreOnSize) return;

            adjust_scale_and_scrollbars();
        }
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::on_paint(wxPaintEvent& WXUNUSED(event))
{
    LOMSE_LOG_DEBUG(Logger::k_mvc, "");

    if (!IsShown() || m_pPresenter == NULL)
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
                determine_scroll_space_size();
                adjust_scale_and_scrollbars();
                pInteractor->new_viewport(-m_xMargin, -m_yMargin, k_no_redraw);
            }
            update_rendering_buffer();
            copy_buffer_on_dc(dc);
            ::wxSetCursor(*wxSTANDARD_CURSOR);
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
        {
            UPoint uPos = pInteractor->screen_point_to_model_point(pos.x, pos.y);
            int nPage = 0;  //TODO
            TimeUnits rTime = 0.0;  //TODO
            int nMeasure = 0;   //TODO
            StatusReporter* pStatus = m_appScope.get_status_reporter();
            pStatus->report_mouse_data(nPage, rTime, nMeasure, uPos);

            pInteractor->on_mouse_move(pos.x, pos.y, flags);
        }
    }
}

//---------------------------------------------------------------------------------------
// ToolBox events:
//      * Click on tool
//      * Change tool page
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
void DocumentWindow::on_page_changed_in_toolbox(ToolBoxPageChangedEvent& event,
                                                ToolBox* pToolBox)
{
    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        SelectionSet& selection = spInteractor->get_selection_set();
        DocCursor* cursor = spInteractor->get_cursor();
        CommandEventHandler handler(this, m_toolsInfo, selection, cursor);
        handler.process_page_changed_in_toolbox_event(pToolBox);
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::on_tool_selected_in_toolbox(ToolBoxToolSelectedEvent& event,
                                                 ToolBox* pToolBox)
{
    if (!is_edition_enabled())
        return;

    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        SelectionSet& selection = spInteractor->get_selection_set();
        DocCursor* cursor = spInteractor->get_cursor();
        CommandEventHandler handler(this, m_toolsInfo, selection, cursor);
        //AWARE: Information provided by toolbox refers to options groups, not to clicks
        //on a command group. Therefore, it is necessary to pass information from event
        handler.process_tool_event(EToolID(event.GetToolID()), event.GetToolGroupID(),
                                   pToolBox);
    }
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
    LOMSE_LOG_DEBUG(Logger::k_mvc, "");

    if (!m_pPresenter)
        return;

    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        //wxLogMessage(_T("on_document_updated %0x"), this);
        spInteractor->on_document_reloaded();
        Refresh(false /* don't erase background */);
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::update_rendering_buffer()
{
    LOMSE_LOG_DEBUG(Logger::k_mvc, "");

    if (!m_pPresenter)
        return;

    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        Interactor* pInteractor = spInteractor.get();
        if (pInteractor && (pInteractor->view_needs_repaint() || !is_buffer_ok()) )
        {
            if (!is_buffer_ok())
                create_rendering_buffer();
            pInteractor->redraw_bitmap();
        }
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
    LOMSE_LOG_DEBUG(Logger::k_mvc, "");

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

    // allocate a new rendering buffer
    delete m_buffer;            //delete any previous buffer
    m_nBufWidth = width;
    m_nBufHeight = height;
    m_buffer = LENMUS_NEW wxImage(width, height);

    int stride = m_nBufWidth * BYTES_PP;        //number of bytes per row

    m_pdata = m_buffer->GetData();
    m_rbuf_window.attach(m_pdata, m_nBufWidth, m_nBufHeight, stride);
}

//---------------------------------------------------------------------------------------
void DocumentWindow::on_key_down(wxKeyEvent& event)
{
    //wxLogMessage(_T("EVT_KEY_DOWN"));
    switch ( event.GetKeyCode() )
    {
        case WXK_SHIFT:
        case WXK_ALT:
        case WXK_CONTROL:
            break;      //do nothing

        default:
            //If a key down (EVT_KEY_DOWN) event is caught and the event handler does not
            //call event.Skip() then the corresponding char event (EVT_CHAR) will not happen.
            //This is by design of wxWidgets and enables the programs that handle both types of
            //events to be a bit simpler.

            //event.Skip();       //to generate Key char event
            process_key(event);
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::on_key_press(wxKeyEvent& event)
{
    //wxLogMessage(_T("[DocumentWindow::on_key_press] KeyCode=%s (%d), KeyDown data: Keycode=%s (%d), (flags = %c%c%c%c)"),
    //        KeyCodeToName(event.GetKeyCode()).c_str(), event.GetKeyCode(),
    //        KeyCodeToName(m_nKeyDownCode).c_str(), m_nKeyDownCode,
    //        (m_fCmd ? _T('C') : _T('-') ),
    //        (m_fAlt ? _T('A') : _T('-') ),
    //        (m_fShift ? _T('S') : _T('-') ),
    //        (event.MetaDown() ? _T('M') : _T('-') )
    //        );
    //process_key(event);
}

//---------------------------------------------------------------------------------------
void DocumentWindow::process_key(wxKeyEvent& event)
{
    //check if it is a command for ToolBox
    EditInterface* pEditGui  = m_appScope.get_edit_gui();
    if ( is_edition_enabled() && pEditGui && pEditGui->process_key_in_toolbox(event) )
        return;

    //check if it is a command on document
    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        SelectionSet& selection = spInteractor->get_selection_set();
        DocCursor* cursor = spInteractor->get_cursor();
        CommandEventHandler handler(this, m_toolsInfo, selection, cursor);
        handler.process_key_event(event);
        if (handler.event_processed())
            return;
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::exec_lomse_command(DocCommand* pCmd, bool fShowBusy)
{
    if (!m_pPresenter)
        return;

    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        if (fShowBusy)
            ::wxBeginBusyCursor();
        spInteractor->exec_command(pCmd);
        update_status_bar_caret_timepos();
        if (fShowBusy)
            ::wxEndBusyCursor();
    }
}

//---------------------------------------------------------------------------------------
DiatonicPitch DocumentWindow::get_pitch_at(Pixels x, Pixels y)
{
    if (m_pPresenter)
    {
        if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
        {
            return spInteractor->get_pitch_at(x, y);
        }
    }
    return DiatonicPitch(k_no_pitch);
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
void DocumentWindow::on_key(int x, int y, unsigned key, unsigned flags)
{
    if (!m_pPresenter)
        return;

    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        switch (key)
        {
            case '1':
                spInteractor->reset_boxes_to_draw();
                spInteractor->set_rendering_option(k_option_draw_box_doc_page_content, true);
                break;
            case '2':
                spInteractor->reset_boxes_to_draw();
                spInteractor->set_rendering_option(k_option_draw_box_container, true);
                break;
            case '3':
                spInteractor->reset_boxes_to_draw();
                spInteractor->set_rendering_option(k_option_draw_box_system, true);
                break;
            case '4':
                spInteractor->reset_boxes_to_draw();
                spInteractor->set_rendering_option(k_option_draw_box_slice, true);
                break;
            case '5':
                spInteractor->reset_boxes_to_draw();
                spInteractor->set_rendering_option(k_option_draw_box_slice_instr, true);
                break;
            case '6':
                spInteractor->reset_boxes_to_draw();
                spInteractor->set_rendering_option(k_option_draw_box_inline_flag, true);
                break;
            case '8':
                spInteractor->switch_task(TaskFactory::k_task_drag_view);
                break;
            case '9':
                spInteractor->switch_task(TaskFactory::k_task_selection);
                break;
            case '0':
                spInteractor->reset_boxes_to_draw();
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
        //wxLogMessage(_T("zoom_fit_width %0x"), this);
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
        //wxLogMessage(_T("zoom_fit_full %0x"), this);
        wxSize size = this->GetClientSize();
        spInteractor->zoom_fit_full(size.GetWidth(), size.GetHeight(), k_no_redraw);
        m_zoomMode = k_zoom_fit_full;
        adjust_scrollbars();
    }
}

////---------------------------------------------------------------------------------------
//void DocumentWindow::change_mouse_mode(EMouseMode mode)
//{
//    if (!m_pPresenter)
//        return;
//
//    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
//    {
//        switch(mode)
//        {
//            case k_mouse_mode_data_entry:
//                spInteractor->switch_task(TaskFactory::k_task_data_entry);
//                break;
//
//            case k_mouse_mode_pointer:
//            default:
//                spInteractor->switch_task(TaskFactory::k_task_selection);
//                spInteractor->show_drag_image(false);
//        }
//        m_mouseMode = mode;
//    }
//}

//---------------------------------------------------------------------------------------
wxString DocumentWindow::exec_command(const string& cmd)
{
    LOMSE_LOG_INFO( cmd );

    m_errorCode = 0;    //assume no error
    const wxString errorMsg = _T("Unknown command.");
    static string m_lastChk = "";

    if (cmd == "help")
        return help_for_console_commands();

    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        //debug commands
        if (cmd == "cmp")   //generate checkpoint data and compare with last "s chk" command issued
        {
            Document* pDoc = m_pPresenter->get_document_raw_ptr();
            string newChk = pDoc->get_checkpoint_data();
            size_t start1 = m_lastChk.find("-->");
            size_t start2 = newChk.find("-->");
            if (m_lastChk.substr(start1) == newChk.substr(start2))
                return _T("OK. Both are equal");
            else
            {
                m_errorCode = 1;
                return _T("Checkpoint data is different!");
            }
        }

        //show data commands
        if (cmd.at(0) == 's')
        {
            if (cmd == "s lmd")             //display source code in LMD format
            {
                return to_wx_string( generate_lmd_source(LmdExporter::k_format_lmd) );
            }
            else if (cmd == "s ldp")        //display source code in LDP format
            {
                return to_wx_string( generate_ldp_source() );
            }
            else if (cmd == "s chk")        //display checkpoint data
            {
                m_lastChk = generate_checkpoint_data();
                return to_wx_string(m_lastChk);
            }
            m_errorCode = 1;
            return errorMsg;
        }

//        //playback commands
//        else if (cmd == "play")
//        {
//        }

        //edition commands
        else if (cmd == "undo")
        {
            if (is_edition_enabled())
            {
                ::wxBeginBusyCursor();
                spInteractor->exec_undo();
                update_status_bar_caret_timepos();
                ::wxEndBusyCursor();
                return wxEmptyString;
            }
            m_errorCode = 1;
            return _T("Document is protected. Edition is not allowed!");
        }
        else if (cmd == "redo")
        {
            if (is_edition_enabled())
            {
                spInteractor->exec_redo();
                update_status_bar_caret_timepos();
                return wxEmptyString;
            }
            m_errorCode = 1;
            return _T("Document is protected. Edition is not allowed!");
        }
        else
        {
            CommandParser parser;
            if (DocCommand* pCmd = parser.create_command(cmd))
            {
                if (is_edition_enabled())
                {
                    exec_lomse_command(pCmd);
                    return wxEmptyString;
                }
                m_errorCode = 1;
                return _T("Document is protected. Edition is not allowed!");
            }
            else
            {
                m_errorCode = 1;
                return( to_wx_string(parser.get_last_error()) );
            }
        }
    }
    m_errorCode = 1;
    return _T("No valid Interactor!");
}

//---------------------------------------------------------------------------------------
wxString DocumentWindow::help_for_console_commands()
{
    return  _T("Available commands:\n\n")
            _T("Show info commands:\n")
            _T("\t s lmd   \t\t\t Display source code in LMD format\n")
            _T("\t s ldp   \t\t\t Display source code in LDP format\n")
            _T("\t s chk   \t\t\t Display checkpoint data\n")
            _T("\n")
            _T("Cursor commands:\n")
            _T("\t c+      \t\t\t Cursor: move next\n")
            _T("\t c-      \t\t\t Cursor: move back\n")
            _T("\t cin     \t\t\t Cursor: enter into element\n")
            _T("\t cout    \t\t\t Cursor: move out of element\n")
            _T("\n")
            _T("Insert commands:\n")
            _T("\t ih <text> \t\t Insert section title (header)\n")
            _T("\t ip <text> \t\t Insert paragraph\n")
            _T("\t is      \t\t\t Insert empty score\n")
            _T("\t i so <ldp> \t\t Insert staffobj. i.e. 'i so (n c4 q)'\n")
            _T("\t i mso <ldp>\t Insert many staffobjs. i.e. 'i mso (n c4 e g+)(n d4 e g-)'\n")
            _T("\t i blo <ldp>\t Insert top level object (block)\n")
            _T("\n")
            _T("Delete commands:\n")
            _T("\t d       \t\t\t Delete block level object\n")
            _T("\n")
            _T("Miscellaneous commands:\n")
            _T("\t undo    \t\t\t Undo one step\n")
            _T("\t redo    \t\t\t Redo one step\n")
            _T("\t quit | exit \t\t\t Close console\n")
            _T("\n")
            _T("Debug commands:\n")
            _T("\t cmp    \t\t\t Generate checkpoint data and compare it with\n")
            _T("        \t\t\t\t data from last 's chk' issued command.")
            _T("\n");
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
    return exporter.get_source( pDoc->get_imodoc() );
}

//---------------------------------------------------------------------------------------
string DocumentWindow::generate_ldp_source()
{
    Document* pDoc = m_pPresenter->get_document_raw_ptr();
    LdpExporter exporter;
    return exporter.get_source( pDoc->get_imodoc() );
}

//---------------------------------------------------------------------------------------
void DocumentWindow::update_status_bar_caret_timepos()
{
    if (is_edition_enabled())
    {
        if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
        {
            StatusReporter* pStatus = m_appScope.get_status_reporter();
            pStatus->report_caret_time( spInteractor->get_caret_timecode() );
        }
    }
}

//---------------------------------------------------------------------------------------
ImoScore* DocumentWindow::get_active_score()
{
    Document* pDoc = m_pPresenter->get_document_raw_ptr();
    return dynamic_cast<ImoScore*>( pDoc->get_imodoc()->get_content_item(0) );
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
        GraphicModel* pGModel = spInteractor->get_graphic_model();
        *pMaxPage = pGModel->get_num_pages();
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

        //get page size in pixels
        VSize size = spInteractor->get_page_size_in_pixels(page);
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
        spInteractor->set_printing_buffer(&rbuf_print);

        //loop to print tiles.
        wxMemoryDC memoryDC;
        for (int iRow=0; iRow < rows; ++iRow)
        {
            for (int iCol=0; iCol < cols; ++iCol)
            {
                spInteractor->on_print_page(page-1, scale, viewport);

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
}

//---------------------------------------------------------------------------------------
void DocumentWindow::adjust_scale_and_scrollbars()
{
    LOMSE_LOG_DEBUG(Logger::k_mvc, "");

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
    LOMSE_LOG_DEBUG(Logger::k_mvc, "");

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
    LOMSE_LOG_DEBUG(Logger::k_mvc, "");

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
    LOMSE_LOG_DEBUG(Logger::k_mvc, "");

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

            spInteractor->new_viewport(xPos, yPos); //, k_no_redraw);    //BUG_001
        }
    }
    event.Skip(false);      //do not propagate event
}

//---------------------------------------------------------------------------------------
void DocumentWindow::scroll_line(bool fUp)
{
    LOMSE_LOG_DEBUG(Logger::k_mvc, "");

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

        #if (LENMUS_PLATFORM_WIN32 == 1)  //---------------------------------------------
        {
            //In Windows, up/down buttons remain enabled even when reaching top/bottom
            if (yPos < m_yMinViewport)
                yPos = m_yMinViewport;
            else if (yPos > m_yMaxViewport)
                yPos = m_yMaxViewport;
        }
        #endif  //-----------------------------------------------------------------------

        spInteractor->new_viewport(xPos, yPos);     //, k_no_redraw);    //BUG_001
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::debug_display_ldp_source()
{
    DlgDebug dlg(this, _T("Generated source code"),
                 to_wx_string(generate_ldp_source()) );
    dlg.ShowModal();
}

//---------------------------------------------------------------------------------------
void DocumentWindow::debug_display_lmd_source()
{
    DlgDebug dlg(this, _T("Generated source code"),
                 to_wx_string(generate_lmd_source(LmdExporter::k_format_lmd)) );
    dlg.ShowModal();
}

//---------------------------------------------------------------------------------------
void DocumentWindow::debug_display_checkpoint_data()
{
    DlgDebug dlg(this, _T("Checkpoint data"),
                 to_wx_string(generate_checkpoint_data()) );
    dlg.ShowModal();
}

//---------------------------------------------------------------------------------------
void DocumentWindow::enable_edition(bool value)
{
    if (SpInteractor spIntor = m_pPresenter->get_interactor(0).lock())
    {
        spIntor->set_operating_mode(value ? Interactor::k_mode_edition
                                          : Interactor::k_mode_read_only);
        m_fEditionEnabled = spIntor->get_operating_mode() == Interactor::k_mode_edition;
        if (value && !m_fEditionEnabled)
        {
            wxMessageBox(_("The document contains scores created with a "
                           "previous version. You must convert the "
                           "document to current format (Menu > File > Convert) "
                           "and edit the converted document."));
        }
        m_toolsInfo.enable_tools(m_fEditionEnabled);
    }
}

//---------------------------------------------------------------------------------------
bool DocumentWindow::should_enable_edit_undo()
{
    if (m_fEditionEnabled && m_pPresenter)
    {
        if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
            return spInteractor->should_enable_edit_undo();
    }
    return false;
}

//---------------------------------------------------------------------------------------
bool DocumentWindow::should_enable_edit_redo()
{
    if (m_fEditionEnabled && m_pPresenter)
    {
        if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
            return spInteractor->should_enable_edit_redo();
    }
    return false;
}

//---------------------------------------------------------------------------------------
bool DocumentWindow::is_document_modified()
{
    Document* pDoc = get_document();
    return pDoc && pDoc->is_modified();
}

//---------------------------------------------------------------------------------------
void DocumentWindow::save_document_as(const wxString& sFilename)
{
    ofstream outfile;
    wxCharBuffer name = sFilename.ToUTF8();
    outfile.open( name.data() );
    outfile << generate_lmd_source(LmdExporter::k_format_ldp);
    outfile.close();

    clear_document_modified_flag();

    m_fullNameWithPath = sFilename;
    wxFileName oFN(m_fullNameWithPath);
    m_filename = oFN.GetFullName();
}

//---------------------------------------------------------------------------------------
void DocumentWindow::save_document()
{
    ofstream outfile;
    wxCharBuffer name = m_fullNameWithPath.ToUTF8();
    outfile.open( name.data() );
    outfile << generate_lmd_source(LmdExporter::k_format_ldp);
    outfile.close();

    clear_document_modified_flag();
}

//---------------------------------------------------------------------------------------
void DocumentWindow::clear_document_modified_flag()
{
    if (m_pPresenter)
    {
        Document* pDoc = m_pPresenter->get_document_raw_ptr();
        pDoc->clear_modified();
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::on_window_closing(wxCloseEvent& WXUNUSED(event))
{
    if (is_document_modified())
    {
        wxString msg = wxString::Format(
                _("Document %s has been modified. Would you like "
                  "to save it before closing?"), m_filename.c_str());
        if (wxMessageBox(msg, _("Warning"), wxYES_NO) == wxYES)
            save_document();
    }
}

//---------------------------------------------------------------------------------------
void DocumentWindow::insert_new_top_level(int type)
{
    switch (type)
    {
        case k_imo_heading:
        {
            wxTextEntryDialog dlg(this, _("Text:"), _("Header"));
            dlg.ShowModal();
            if (dlg.GetValue() != wxEmptyString)
            {
                stringstream cmd;
                cmd << "ih " << to_std_string(dlg.GetValue());
                exec_command(cmd.str());
            }
        }
        break;

        case k_imo_para:
        {
            wxTextEntryDialog dlg(this, _("Text:"), _("Paragraph"));
            dlg.ShowModal();
            if (dlg.GetValue() != wxEmptyString)
            {
                stringstream cmd;
                cmd << "ip " << to_std_string(dlg.GetValue());
                exec_command(cmd.str());
            }
        }
        break;

        case k_imo_score:
        {
            ScoreWizard wizard(this, m_appScope);
            wizard.Run();
            wxString score = wizard.get_score();
            if (score != wxEmptyString)
            {
                stringstream cmd;
                cmd << "i blo " << "<ldpmusic>" << to_std_string(score) << "</ldpmusic>";
                exec_command(cmd.str());
            }
        }
        break;

        default:
            ;
    }
}

//=======================================================================================
//contextual menu and related
//=======================================================================================

//---------------------------------------------------------------------------------------
wxMenu* DocumentWindow::get_contextual_menu(bool fInitialize)
{
	delete m_pContextualMenu;
	m_pContextualMenu = new wxMenu();

	if (!fInitialize)
		return m_pContextualMenu;

    wxMenuItem* pItem;
    wxSize nIconSize(16, 16);

#if (LENMUS_PLATFORM_WIN32 == 1 || LENMUS_PLATFORM_UNIX == 1)
    pItem = new wxMenuItem(m_pContextualMenu, k_popup_menu_Cut, _("&Cut"));
    pItem->SetBitmap( wxArtProvider::GetBitmap(_T("tool_cut"), wxART_TOOLBAR, nIconSize) );
    m_pContextualMenu->Append(pItem);

    //pItem = new wxMenuItem(m_pContextualMenu, k_popup_menu_Copy, _("&Copy"));
    //pItem->SetBitmap( wxArtProvider::GetBitmap(_T("tool_copy"), wxART_TOOLBAR, nIconSize) );
    //m_pContextualMenu->Append(pItem);

    //pItem = new wxMenuItem(m_pContextualMenu, k_popup_menu_Paste, _("&Paste"));
    //pItem->SetBitmap( wxArtProvider::GetBitmap(_T("tool_paste"), wxART_TOOLBAR, nIconSize) );
    //m_pContextualMenu->Append(pItem);

	m_pContextualMenu->AppendSeparator();

    //pItem = new wxMenuItem(m_pContextualMenu, k_popup_menu_Color, _("Colour"));
    //pItem->SetBitmap( wxArtProvider::GetBitmap(_T("opt_colors"), wxART_TOOLBAR, nIconSize) );
    //m_pContextualMenu->Append(pItem);

    pItem = new wxMenuItem(m_pContextualMenu, k_popup_menu_Properties, _("Edit"));
    pItem->SetBitmap( wxArtProvider::GetBitmap(_T("tool_properties"), wxART_TOOLBAR, nIconSize) );
    m_pContextualMenu->Append(pItem);

	//m_pContextualMenu->AppendSeparator();

#else
	m_pContextualMenu->Append(k_popup_menu_Cut, _("&Cut"));
//	//m_pContextualMenu->Append(k_popup_menu_Copy, _("&Copy"));
//	//m_pContextualMenu->Append(k_popup_menu_Paste, _("&Paste"));
	m_pContextualMenu->AppendSeparator();
//	//m_pContextualMenu->Append(k_popup_menu_Color, _("Colour"));
    m_pContextualMenu->Append(k_popup_menu_Properties, _("Edit"));
#endif


//#ifdef _LM_DEBUG_
//
//	//m_pContextualMenu->AppendSeparator();
//    m_pContextualMenu->Append(k_popup_menu_DumpShape, _T("Dump shape"));
//#endif

	return m_pContextualMenu;
}

//---------------------------------------------------------------------------------------
void DocumentWindow::on_popup_cut(wxCommandEvent& event)
{
	WXUNUSED(event);
    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        SelectionSet& selection = spInteractor->get_selection_set();
        DocCursor* cursor = spInteractor->get_cursor();
        CommandGenerator executer(this, selection, cursor);
        executer.delete_selection();
    }
}

////---------------------------------------------------------------------------------------
//void DocumentWindow::OnCopy(wxCommandEvent& event)
//{
//	WXUNUSED(event);
//}
//
//void DocumentWindow::OnPaste(wxCommandEvent& event)
//{
//	WXUNUSED(event);
//}
//
//void DocumentWindow::OnColor(wxCommandEvent& event)
//{
//	WXUNUSED(event);
//}

//---------------------------------------------------------------------------------------
void DocumentWindow::on_popup_properties(wxCommandEvent& event)
{
	WXUNUSED(event);
    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        SelectionSet& selection = spInteractor->get_selection_set();
        DocCursor* cursor = spInteractor->get_cursor();
        CommandGenerator executer(this, selection, cursor);
        DlgProperties dlg(this, m_appScope, &executer);
        dlg.add_specific_panels_for(m_pMenuOwner);
        dlg.Layout();
        dlg.ShowModal();
    }
}

////---------------------------------------------------------------------------------------
//void DocumentWindow::OnDeleteTiePrev(wxCommandEvent& event)
//{
//	WXUNUSED(event);
//	wxASSERT(m_pMenuOwner->IsComponentObj());
//    wxASSERT( ((lmComponentObj*)m_pMenuOwner)->IsStaffObj());
//    wxASSERT( ((lmStaffObj*)m_pMenuOwner)->IsNote());
//
//    DeleteTie( (lmNote*)m_pMenuOwner );
//}
//
//void DocumentWindow::OnToggleStem(wxCommandEvent& event)
//{
//	WXUNUSED(event);
//
//    ToggleStem();
//}
//
//void DocumentWindow::OnAttachText(wxCommandEvent& event)
//{
//	WXUNUSED(event);
//	wxASSERT(m_pMenuOwner->IsComponentObj());
//
//    AttachNewText( (lmComponentObj*)m_pMenuOwner );
//}
//
//void DocumentWindow::OnScoreTitles(wxCommandEvent& event)
//{
//	WXUNUSED(event);
//	AddTitle();
//}
//
//void DocumentWindow::OnViewPageMargins(wxCommandEvent& event)
//{
//    g_fShowMargins = !g_fShowMargins;
//    lmDocument* pDoc = GetMainFrame()->GetActiveDoc();
//    if (pDoc)
//    {
//	    pDoc->Modify(true);
//        pDoc->UpdateAllViews((wxView*)NULL, new lmUpdateHint() );
//    }
//}
//
////#ifdef _LM_DEBUG_
////void DocumentWindow::OnDumpShape(wxCommandEvent& event)
////{
////	WXUNUSED(event);
////    lmDlgDebug dlg(this, _T("GMObject dump"), m_pMenuGMO->Dump(0));
////    dlg.ShowModal();
////}
////#endif
//
//void DocumentWindow::OnToolPopUpMenuEvent(wxCommandEvent& event)
//{
//    //redirect the event to the menu owner page
//
//	lmToolBox* pToolBox = GetMainFrame()->GetActiveToolBox();
//	wxASSERT(pToolBox);
//	pToolBox->OnPopUpMenuEvent(event);
//}


}   //namespace lenmus
