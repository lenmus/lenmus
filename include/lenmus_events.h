//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2010-2018 LenMus project
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

#ifndef __LENMUS_EVENTS_H__        //to avoid nested includes
#define __LENMUS_EVENTS_H__

//lenmus
#include "lenmus_standard_header.h"

//lomse
#include <lomse_internal_model.h>
#include <lomse_events.h>
using namespace lomse;

//wxWidgets
//#define system ::system         //bypass for bug in wxcrtbase.h: "reference to 'system' is ambiguous"
#include <wx/wxprec.h>
#include <wx/wx.h>
#undef system                   //bypass for bug in wxcrtbase.h: "reference to 'system' is ambiguous"


namespace lenmus
{
class ExerciseCtrol;
class ExerciseOptions;
class ProblemManager;
class DlgCounters;

//---------------------------------------------------------------------------------------
// lmUpdateViewportEvent
//      An event to signal the need to repaint the window and to update scrollbars
//      due to an auto-scroll while the score is being played back.
//---------------------------------------------------------------------------------------

DECLARE_EVENT_TYPE( lmEVT_UPDATE_VIEWPORT, -1 )

class lmUpdateViewportEvent : public wxEvent
{
private:
    SpEventUpdateViewport m_pEvent;   //lomse event

public:
    lmUpdateViewportEvent(SpEventUpdateViewport pEvent, int id = 0)
        : wxEvent(id, lmEVT_UPDATE_VIEWPORT)
        , m_pEvent(pEvent)
    {
    }

    // copy constructor
    lmUpdateViewportEvent(const lmUpdateViewportEvent& event)
        : wxEvent(event)
        , m_pEvent( event.m_pEvent )
    {
    }

    // clone constructor. Required for sending with wxPostEvent()
    virtual wxEvent *Clone() const { return LENMUS_NEW lmUpdateViewportEvent(*this); }

    // accessors
    SpEventUpdateViewport get_lomse_event() { return m_pEvent; }
};

typedef void (wxEvtHandler::*UpdateViewportEventFunction)(lmUpdateViewportEvent&);

#define LM_EVT_UPDATE_VIEWPORT(fn) \
    DECLARE_EVENT_TABLE_ENTRY( lmEVT_UPDATE_VIEWPORT, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
    wxStaticCastEvent( UpdateViewportEventFunction, & fn ), (wxObject *) nullptr ),


//---------------------------------------------------------------------------------------
// lmVisualTrackingEvent
//      An event to signal different actions related to
//      highlighting / unhighlighting notes while they are being played.
//---------------------------------------------------------------------------------------

DECLARE_EVENT_TYPE( lmEVT_SCORE_HIGHLIGHT, -1 )

class lmVisualTrackingEvent : public wxEvent
{
private:
    SpEventVisualTracking m_pEvent;   //lomse event

public:
    lmVisualTrackingEvent(SpEventVisualTracking pEvent, int id = 0)
        : wxEvent(id, lmEVT_SCORE_HIGHLIGHT)
        , m_pEvent(pEvent)
    {
    }

    // copy constructor
    lmVisualTrackingEvent(const lmVisualTrackingEvent& event)
        : wxEvent(event)
        , m_pEvent( event.m_pEvent )
    {
    }

    // clone constructor. Required for sending with wxPostEvent()
    virtual wxEvent *Clone() const { return LENMUS_NEW lmVisualTrackingEvent(*this); }

    // accessors
    SpEventVisualTracking get_lomse_event() { return m_pEvent; }
};

typedef void (wxEvtHandler::*VisualTrackingEventFunction)(lmVisualTrackingEvent&);

#define LM_EVT_SCORE_HIGHLIGHT(fn) \
    DECLARE_EVENT_TABLE_ENTRY( lmEVT_SCORE_HIGHLIGHT, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
    wxStaticCastEvent( VisualTrackingEventFunction, & fn ), (wxObject *) nullptr ),


//---------------------------------------------------------------------------------------
// lmEndOfPlaybackEvent: An event to signal end of playback
//---------------------------------------------------------------------------------------

DECLARE_EVENT_TYPE( lmEVT_END_OF_PLAYBACK, -1 )

class lmEndOfPlaybackEvent : public wxEvent
{
private:
    SpEventEndOfPlayback m_pEvent;   //lomse event

public:
    lmEndOfPlaybackEvent(SpEventEndOfPlayback pEvent, int id = 0 )
        : wxEvent(id, lmEVT_END_OF_PLAYBACK)
        , m_pEvent(pEvent)
    {
    }

    // copy constructor
    lmEndOfPlaybackEvent(const lmEndOfPlaybackEvent& event)
        : wxEvent(event)
        , m_pEvent( event.m_pEvent )
    {
    }

    // clone constructor. Required for sending with wxPostEvent()
    virtual wxEvent *Clone() const { return LENMUS_NEW lmEndOfPlaybackEvent(*this); }

    // accessors
    SpEventEndOfPlayback get_lomse_event() { return m_pEvent; }
};

typedef void (wxEvtHandler::*EndOfPlayEventFunction)(lmEndOfPlaybackEvent&);

#define LM_EVT_END_OF_PLAYBACK(fn) \
    DECLARE_EVENT_TABLE_ENTRY( lmEVT_END_OF_PLAYBACK, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
    wxStaticCastEvent( EndOfPlayEventFunction, & fn ), (wxObject *) nullptr ),


//---------------------------------------------------------------------------------------
// lmUpdateUIEvent: An event for updating ToolBox UI
//---------------------------------------------------------------------------------------

DECLARE_EVENT_TYPE( lmEVT_UPDATE_UI, -1 )

class lmUpdateUIEvent : public wxEvent
{
private:
    SpEventUpdateUI m_pEvent;   //lomse event

public:
    lmUpdateUIEvent(SpEventUpdateUI pEvent, int id = 0 )
        : wxEvent(id, lmEVT_UPDATE_UI)
        , m_pEvent(pEvent)
    {
    }

    // copy constructor
    lmUpdateUIEvent(const lmUpdateUIEvent& event)
        : wxEvent(event)
        , m_pEvent( event.m_pEvent )
    {
    }

    // clone constructor. Required for sending with wxPostEvent()
    virtual wxEvent *Clone() const { return LENMUS_NEW lmUpdateUIEvent(*this); }

    // accessors
    SpEventUpdateUI get_lomse_event() { return m_pEvent; }
};

typedef void (wxEvtHandler::*UpdateUIEventFunction)(lmUpdateUIEvent&);

#define LM_EVT_UPDATE_UI(fn) \
    DECLARE_EVENT_TABLE_ENTRY( lmEVT_UPDATE_UI, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
    wxStaticCastEvent( UpdateUIEventFunction, & fn ), (wxObject *) nullptr ),

//---------------------------------------------------------------------------------------
// lmShowContextualMenuEvent: An event for requesting to show a contextual menu
//---------------------------------------------------------------------------------------

DECLARE_EVENT_TYPE( lmEVT_SHOW_CONTEXTUAL_MENU, -1 )

class lmShowContextualMenuEvent : public wxEvent
{
private:
    SpEventMouse m_pEvent;   //lomse event

public:
    lmShowContextualMenuEvent(SpEventMouse pEvent, int id = 0 )
        : wxEvent(id, lmEVT_SHOW_CONTEXTUAL_MENU)
        , m_pEvent(pEvent)
    {
    }

    // copy constructor
    lmShowContextualMenuEvent(const lmShowContextualMenuEvent& event)
        : wxEvent(event)
        , m_pEvent( event.m_pEvent )
    {
    }

    // clone constructor. Required for sending with wxPostEvent()
    virtual wxEvent *Clone() const { return LENMUS_NEW lmShowContextualMenuEvent(*this); }

    // accessors
    SpEventMouse get_lomse_event() { return m_pEvent; }
};

typedef void (wxEvtHandler::*ShowContextualMenuEventFunction)(lmShowContextualMenuEvent&);

#define LM_EVT_SHOW_CONTEXTUAL_MENU(fn) \
    DECLARE_EVENT_TABLE_ENTRY( lmEVT_SHOW_CONTEXTUAL_MENU, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
    wxStaticCastEvent( ShowContextualMenuEventFunction, & fn ), (wxObject *) nullptr ),


//---------------------------------------------------------------------------------------
// CountersEvent
//      An event to signal different actions related to DlgCounters
//---------------------------------------------------------------------------------------

DECLARE_EVENT_TYPE( EVT_COUNTERS_DLG, -1 )

class CountersEvent : public wxEvent
{
private:
    ExerciseCtrol* m_pExercise;
    ExerciseOptions* m_pConstrains;
    ProblemManager* m_pProblemManager;
    DlgCounters* m_pDlg;

public:
    CountersEvent(ExerciseCtrol* pExercise, ExerciseOptions* pConstrains,
                  ProblemManager* pProblemManager, DlgCounters* pDlg, int id = 0)
        : wxEvent(id, EVT_COUNTERS_DLG)
        , m_pExercise(pExercise)
        , m_pConstrains(pConstrains)
        , m_pProblemManager(pProblemManager)
        , m_pDlg(pDlg)
    {
        m_propagationLevel = wxEVENT_PROPAGATE_MAX;
    }

    // copy constructor
    CountersEvent(const CountersEvent& event) : wxEvent(event)
    {
        m_pExercise = event.m_pExercise;
        m_pConstrains = event.m_pConstrains;
        m_pProblemManager = event.m_pProblemManager;
        m_pDlg = event.m_pDlg;
    }

    // clone constructor. Required for sending with wxPostEvent()
    virtual wxEvent *Clone() const { return LENMUS_NEW CountersEvent(*this); }

    // accessors
    ExerciseCtrol* get_exercise() { return m_pExercise; }
    ExerciseOptions* get_constrains() { return m_pConstrains; }
    ProblemManager* get_problem_manager() { return m_pProblemManager; }
    DlgCounters* get_dialog() { return m_pDlg; }

};

typedef void (wxEvtHandler::*CountersEventFunction)(CountersEvent&);

#define LM_EVT_COUNTERS_DLG(fn) \
    DECLARE_EVENT_TABLE_ENTRY( EVT_COUNTERS_DLG, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
    wxStaticCastEvent( CountersEventFunction, & fn ), (wxObject *) nullptr ),


//---------------------------------------------------------------------------------------
// PageRequestEvent: An event for requesting to display an eBook page
//---------------------------------------------------------------------------------------

DECLARE_EVENT_TYPE( lmEVT_PAGE_REQUEST, -1 )

class PageRequestEvent : public wxEvent
{
private:
    string m_url;

public:
    PageRequestEvent(const string& url, int id = 0 )
        : wxEvent(id, lmEVT_PAGE_REQUEST)
        , m_url(url)
    {
    }

    // copy constructor
    PageRequestEvent(const PageRequestEvent& event)
        : wxEvent(event)
        , m_url( event.m_url )
    {
    }

    // clone constructor. Required for sending with wxPostEvent()
    virtual wxEvent *Clone() const { return LENMUS_NEW PageRequestEvent(*this); }

    // accessors
    string& get_url() { return m_url; }
};

typedef void (wxEvtHandler::*PageRequestEventFunction)(PageRequestEvent&);

#define LM_EVT_PAGE_REQUEST(fn) \
    DECLARE_EVENT_TABLE_ENTRY( lmEVT_PAGE_REQUEST, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
    wxStaticCastEvent( PageRequestEventFunction, & fn ), (wxObject *) nullptr ),


}   // namespace lenmus


#endif    // __LENMUS_EVENTS_H__
