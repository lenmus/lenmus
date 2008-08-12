//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
//
//    This program is free software; you can redistribute it and/or modify it under the 
//    terms of the GNU General Public License as published by the Free Software Foundation;
//    either version 2 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this 
//    program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, 
//    Fifth Floor, Boston, MA  02110-1301, USA.
//
//    For any comment, suggestion or feature request, please contact the manager of 
//    the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------

#ifndef __LM_TOOLBOXEVENTS_H__        //to avoid nested includes
#define __LM_TOOLBOXEVENTS_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ToolBoxEvents.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "ToolsBox.h"

//-----------------------------------------------------------------------------------------
// An event to signal different actions related to selecting tools in the toolbox
//-----------------------------------------------------------------------------------------

DECLARE_EVENT_TYPE( lmEVT_TOOLBOX, -1 )

class lmToolBoxEvent : public wxCommandEvent
{
public:
    lmToolBoxEvent(lmEToolGroupID nToolGroupID, lmEToolPage nToolPage, long nTool,
                   bool fToolSelected, int id=0 ) 
            : wxCommandEvent(lmEVT_TOOLBOX, id)
        {
            m_nToolGroupID = nToolGroupID;
            m_nToolPage = nToolPage;
            m_nTool = nTool;
            m_fToolSelected = fToolSelected;
        }

    // copy constructor
    lmToolBoxEvent(const lmToolBoxEvent& event) : wxCommandEvent(event)
        {
            m_nToolGroupID = event.m_nToolGroupID;
            m_nToolPage = event.m_nToolPage;
            m_nTool = event.m_nTool;
            m_fToolSelected = event.m_fToolSelected;
        }

    // clone constructor. Required for sending with wxPostEvent()
    virtual wxEvent *Clone() const { return new lmToolBoxEvent(*this); }

    // accessors
    inline lmEToolGroupID GetToolGroupID() { return m_nToolGroupID; }
    inline lmEToolPage GetToolPageType() { return m_nToolPage; }
    inline long GetToolID() { return m_nTool; }
    inline bool ToolSelected() { return m_fToolSelected; }

private:
    lmEToolGroupID  m_nToolGroupID;     //Group generating the event
    long            m_nTool;            //ID of the selected tool
    lmEToolPage     m_nToolPage;        //page issuing the event
    bool            m_fToolSelected;    //tool status
};


//define a typedef for the event handler fuction
typedef void (wxEvtHandler::*ToolBoxEventFunction)(lmToolBoxEvent&);

//Define a table of event types for the individual events this event class supports
#define LM_EVT_TOOLBOX(fn) \
    DECLARE_EVENT_TABLE_ENTRY( lmEVT_TOOLBOX, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
    wxStaticCastEvent( ToolBoxEventFunction, & fn ), (wxObject *) NULL ),



#endif    // __LM_TOOLBOXEVENTS_H__
