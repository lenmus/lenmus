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
//#include "wx/thread.h"

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

    //operations
    void SetCaretPosition(double rScale, lmUPoint uPos, lmStaff* pStaff);
        //Hide/show the caret. If the caret is hidden N times, it must be shown
        //also N times to reappear again on the screen.
    void Show(bool fShow = true);
	void Show(double rScale, lmUPoint uPos, lmStaff* pStaff);
    inline void Hide() { Show(false); }
        //When shown, the caret can be made invisible
    void SetInvisible(bool fInvisible);

    //aspect
    void SetBlinkingRate(int nMillisecs);
    void SetColour(wxColour color);

    //status
        //caret permanently hidden
    inline bool IsHidden() const { return m_nCountVisible <= 0;; } 
        //caret shown, but not necessarily visible at this moment. That dependens on
        //current blinking state and visibility state. Is always false if caret
        //is hidden or if its visibility is false. It is true if caret not hidden
        //and it has not blinket out at this moment.
    inline bool IsVisible() const { return m_fCaretDrawn; }
        //invisibility status
    inline bool IsInvisible() { return m_fInvisible; }


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
    bool            m_fCaretDrawn;      //caret visible on screen (it implies it is displayed)
    bool            m_fInvisible;       //true = invisible
    int             m_nCountVisible;    //number of times Show() - number of times Hide()


    //timer for caret blinking
	wxTimer			m_oCaretTimer;      //for caret blinking

    //caret position
    lmUPoint        m_oCaretPos;        //caret position on screen

    //caret layout
    wxColour        m_color;            //caret colour
    int             m_nBlinkingRate;    //milliseconds
	lmLUnits        m_udyLength;
	lmLUnits        m_udxSegment;

    DECLARE_EVENT_TABLE()
};


#endif    // __LM_CARET_H__
