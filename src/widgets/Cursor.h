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

#ifndef __LM_CURSOR_H__        //to avoid nested includes
#define __LM_CURSOR_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Cursor.cpp"
#endif

#include "wx/event.h"
#include "wx/thread.h"

class lmView;
class lmCanvas;
class lmScore;
class lmStaffObj;
class lmStaff;

#include "../score/defs.h"

//helper class to display and manage a score cursor

class lmScoreViewCursor : public wxEvtHandler
{
    DECLARE_DYNAMIC_CLASS(lmScoreViewCursor)

public:
    lmScoreViewCursor(lmView* pParent, lmCanvas* pCanvas, lmScore* pScore);
    ~lmScoreViewCursor();

    //event handlers
	void OnCursorTimer(wxTimerEvent& event);

    void SetCursorPosition(lmUPoint uPos, lmStaff* pStaff);
    void RemoveCursor();
    void DisplayCursor(double rScale, lmUPoint uPos, lmStaff* pStaff);

    //aspect
    void SetBlinkingRate(int nMillisecs);
    void SetColour(wxColour color);

    //status
        //cursor displayed, but not necessarily visible at this time as it could be blinking
    inline bool IsDisplayed() const { return m_fDisplayed; } 
    //    //cursor displayed and visible on virtual screen (it can be hidden due to scrolling)
    //inline bool IsVisible() const { return m_fVisible; } 
 

private:
	void RenderCursor(bool fVisible);

    lmCanvas*       m_pCanvas;          //the canvas
    lmView*         m_pView;
    lmScore*        m_pScore;
    double          m_rScale;           //view presentation scale

    //cursor display status
    bool                m_fDisplayed;   //cursor displayed, but not necessarily visible at this time as it could be blinking
    bool                m_fVisible;     //cursor visible on screen (it implies it is displayed)
    wxCriticalSection   m_locker;       //locker for accesing previous flag

    //timer for cursor blinking
	wxTimer			m_oCursorTimer;			//for cursor blinking

    //cursor position
    lmUPoint        m_oCursorPos;           //cursor position on screen

    //cursor layout
    wxColour        m_color;                //cursor colour
    int             m_nBlinkingRate;        //milliseconds
	lmLUnits        m_udyLength;
	lmLUnits        m_udxSegment;

    DECLARE_EVENT_TABLE()
};


#endif    // __LM_CURSOR_H__
