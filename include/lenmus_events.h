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

#ifndef __LENMUS_EVENTS_H__        //to avoid nested includes
#define __LENMUS_EVENTS_H__

//lomse
#include "lomse_internal_model.h"
#include "lomse_events.h"
using namespace lomse;

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>


namespace lenmus
{


//---------------------------------------------------------------------------------------
// lmScoreHighlightEvent
//      An event to signal different actions related to
//      highlighting / unhighlighting notes while they are being played.
//---------------------------------------------------------------------------------------

DECLARE_EVENT_TYPE( lmEVT_SCORE_HIGHLIGHT, -1 )

class lmScoreHighlightEvent : public wxEvent
{
private:
    EventScoreHighlight* m_pEvent;   //lomse event

public:
    lmScoreHighlightEvent(EventScoreHighlight* pEvent, int id = 0)
        : wxEvent(id, lmEVT_SCORE_HIGHLIGHT)
        , m_pEvent(pEvent)
    {
    }

    // copy constructor
    lmScoreHighlightEvent(const lmScoreHighlightEvent& event) : wxEvent(event)
    {
        m_pEvent = event.m_pEvent;
    }

    // clone constructor. Required for sending with wxPostEvent()
    virtual wxEvent *Clone() const { return new lmScoreHighlightEvent(*this); }

    // accessors
    EventScoreHighlight* get_lomse_event() { return m_pEvent; }
};

typedef void (wxEvtHandler::*ScoreHighlightEventFunction)(lmScoreHighlightEvent&);

#define LM_EVT_SCORE_HIGHLIGHT(fn) \
    DECLARE_EVENT_TABLE_ENTRY( lmEVT_SCORE_HIGHLIGHT, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
    wxStaticCastEvent( ScoreHighlightEventFunction, & fn ), (wxObject *) NULL ),


//---------------------------------------------------------------------------------------
// lmEndOfPlayEvent: An event to signal end of playback
//---------------------------------------------------------------------------------------

DECLARE_EVENT_TYPE( lmEVT_END_OF_PLAY, -1 )

class lmEndOfPlayEvent : public wxEvent
{
public:
    lmEndOfPlayEvent(int id = 0 )    : wxEvent(id, lmEVT_END_OF_PLAY)
        {    m_propagationLevel = wxEVENT_PROPAGATE_MAX; }

    // copy constructor
    lmEndOfPlayEvent(const lmEndOfPlayEvent& event) : wxEvent(event) {}

    // clone constructor. Required for sending with wxPostEvent()
    virtual wxEvent *Clone() const { return new lmEndOfPlayEvent(*this); }

};

typedef void (wxEvtHandler::*EndOfPlayEventFunction)(lmEndOfPlayEvent&);

#define LM_EVT_END_OF_PLAY(fn) \
    DECLARE_EVENT_TABLE_ENTRY( lmEVT_END_OF_PLAY, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
    wxStaticCastEvent( EndOfPlayEventFunction, & fn ), (wxObject *) NULL ),


}   // namespace lenmus


#endif    // __LENMUS_EVENTS_H__
