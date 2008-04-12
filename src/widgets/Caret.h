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

#ifndef __LM_CARET_H__        //to avoid nested includes
#define __LM_CARET_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Caret.cpp"
#endif

#include "wx/event.h"
#include "wx/thread.h"

class lmView;
class lmCanvas;
class lmScore;
class lmStaffObj;
class lmStaff;

#include "../score/defs.h"

//A caret is a blinking cursor showing the position where the edition will take place

class lmCaret : public wxEvtHandler
{
    DECLARE_DYNAMIC_CLASS(lmCaret)

public:
    lmCaret(lmView* pParent, lmCanvas* pCanvas, lmScore* pScore);
    ~lmCaret();

    //event handlers
	void OnCaretTimer(wxTimerEvent& event);

    void SetCaretPosition(lmUPoint uPos, lmStaff* pStaff);
    void RemoveCaret();
    void DisplayCaret(double rScale, lmUPoint uPos, lmStaff* pStaff);

    //aspect
    void SetBlinkingRate(int nMillisecs);
    void SetColour(wxColour color);

    //status
        //caret displayed, but not necessarily visible at this moment, as it could
        //have been blinked out
    inline bool IsDisplayed() const { return m_fDisplayed; } 
        //caret displayed and currently visible
    inline bool IsVisible() const { return m_nCountVisible > 0; }

    // operations
    void Show(bool fShow = true);
    inline void Hide() { Show(false); }

private:
	void RenderCaret(bool fVisible);
    void Refresh();
    void DoShow();
    void DoHide();

    lmCanvas*       m_pCanvas;          //the canvas
    lmView*         m_pView;
    lmScore*        m_pScore;
    double          m_rScale;           //view presentation scale

    //caret display status
    bool                m_fDisplayed;   //caret displayed, but not necessarily visible at this time as it could be blinked out
    bool                m_fVisible;     //caret visible on screen (it implies it is displayed)
    wxCriticalSection   m_locker;       //locker for accesing previous flag
    int                 m_nCountVisible;    //

    //timer for caret blinking
	wxTimer			m_oCaretTimer;			//for caret blinking

    //caret position
    lmUPoint        m_oCaretPos;           //caret position on screen

    //caret layout
    wxColour        m_color;                //caret colour
    int             m_nBlinkingRate;        //milliseconds
	lmLUnits        m_udyLength;
	lmLUnits        m_udxSegment;

    DECLARE_EVENT_TABLE()
};


#endif    // __LM_CARET_H__
