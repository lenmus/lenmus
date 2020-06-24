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

#ifndef __LENMUS_TOOL_BOXEVENTS_H__        //to avoid nested includes
#define __LENMUS_TOOL_BOXEVENTS_H__

////wxWidgets
//#include <wx/wxprec.h>
//#ifndef WX_PRECOMP
//    #include <wx/wx.h>
//#endif

//lenmus
#include "lenmus_tool_box.h"
#include "lenmus_tool_group.h"

using namespace std;


namespace lenmus
{


//---------------------------------------------------------------------------------------
// An event to signal different actions related to selecting tools in the toolbox
//---------------------------------------------------------------------------------------
DECLARE_EVENT_TYPE( lmEVT_TOOLBOX_TOOL_SELECTED, -1 )

class ToolBoxToolSelectedEvent : public wxCommandEvent
{
private:
    long    m_nToolID;          //ID of the selected tool
    bool    m_fToolSelected;    //tool status

public:
    ToolBoxToolSelectedEvent(long nToolID, bool fToolSelected, int id=0 )
        : wxCommandEvent(lmEVT_TOOLBOX_TOOL_SELECTED, id)
        , m_nToolID(nToolID)
        , m_fToolSelected(fToolSelected)
    {
    }

    // copy constructor
    ToolBoxToolSelectedEvent(const ToolBoxToolSelectedEvent& event)
        : wxCommandEvent(event)
    {
        m_nToolID = event.m_nToolID;
        m_fToolSelected = event.m_fToolSelected;
    }

    // clone constructor. Required for sending with wxPostEvent()
    virtual wxEvent *Clone() const { return new ToolBoxToolSelectedEvent(*this); }

    // accessors
    inline long GetToolID() { return m_nToolID; }
    inline bool ToolSelected() { return m_fToolSelected; }

};


//define a typedef for the event handler fuction
typedef void (wxEvtHandler::*ToolBoxToolSelectedEventFunction)(ToolBoxToolSelectedEvent&);

//Define a table of event types for the individual events this event class supports
#define LM_EVT_TOOLBOX_TOOL_SELECTED(fn) \
    DECLARE_EVENT_TABLE_ENTRY( lmEVT_TOOLBOX_TOOL_SELECTED, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
    wxStaticCastEvent( ToolBoxToolSelectedEventFunction, & fn ), (wxObject *) nullptr ),


//---------------------------------------------------------------------------------------
// An event to notify that user has selected another page in the toolbox
//---------------------------------------------------------------------------------------

DECLARE_EVENT_TYPE( lmEVT_TOOLBOX_PAGE_CHANGED, -1 )

class ToolBoxPageChangedEvent : public wxCommandEvent
{
public:
    ToolBoxPageChangedEvent(EToolPageID nToolPage, int id=0 )
            : wxCommandEvent(lmEVT_TOOLBOX_PAGE_CHANGED, id)
            , m_nToolPage(nToolPage)
        {
        }

    // copy constructor
    ToolBoxPageChangedEvent(const ToolBoxPageChangedEvent& event) : wxCommandEvent(event)
        {
            m_nToolPage = event.m_nToolPage;
        }

    // clone constructor. Required for sending with wxPostEvent()
    virtual wxEvent *Clone() const { return new ToolBoxPageChangedEvent(*this); }

    // accessors
    inline EToolPageID GetToolPageType() { return m_nToolPage; }

private:
    EToolPageID     m_nToolPage;        //page issuing the event
};


//define a typedef for the event handler fuction
typedef void (wxEvtHandler::*ToolBoxPageChangedEventFunction)(ToolBoxPageChangedEvent&);

//Define a table of event types for the individual events this event class supports
#define LM_EVT_TOOLBOX_PAGE_CHANGED(fn) \
    DECLARE_EVENT_TABLE_ENTRY( lmEVT_TOOLBOX_PAGE_CHANGED, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
    wxStaticCastEvent( ToolBoxPageChangedEventFunction, & fn ), (wxObject *) nullptr ),


}   // namespace lenmus

#endif    // __LENMUS_TOOL_BOXEVENTS_H__
