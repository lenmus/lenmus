//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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
//-------------------------------------------------------------------------------------

#ifndef __LM_SPLASHFRAME_H__        //to avoid nested includes
#define __LM_SPLASHFRAME_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "SplashFrame.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/frame.h"
    #include "wx/image.h"
    #include "wx/timer.h"
#endif

// style and options flags
#define lmSPLASH_CENTRE_ON_PARENT   0x01
#define lmSPLASH_CENTRE_ON_SCREEN   0x02
#define lmSPLASH_NO_CENTRE          0x00
#define lmSPLASH_TIMEOUT            0x04
#define lmSPLASH_NO_TIMEOUT         0x00

class wxBitmap;

class lmSplashFrame : public wxFrame
{
public:
    // ctor(s)
    lmSplashFrame(const wxBitmap& bitmap, const wxColour& transparentColor,
                  long splashStyle, int milliseconds,
                  wxWindow* parent, wxWindowID id, const wxPoint& pos,
                  const wxSize& size, long style);
    ~lmSplashFrame();

    void SetWindowShape();
    void AllowDestroy();
    void TerminateSplash();

    // event handlers (these functions should _not_ be virtual)
    void OnPaint(wxPaintEvent& evt);
    void OnWindowCreate(wxWindowCreateEvent& evt);
    void OnNotify(wxTimerEvent& event);
    void OnCloseWindow(wxCloseEvent& event);

private:
    wxBitmap    m_bmp;
    wxPoint     m_delta;
    wxTimer     m_timer;
    bool        m_fHasShape;
    bool        m_fDestroyable;
    bool        m_fTimedOut;
    wxColour    m_transparentColor;

    DECLARE_EVENT_TABLE()
};


#endif    // __LM_SPLASHFRAME_H__

