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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "SplashFrame.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/splash.h"      // to use splash style constants
#include "wx/stattext.h"

#include "SplashFrame.h"
#include "TheApp.h"         //to get version.

#define wxSPLASH_TIMER_ID 9999

BEGIN_EVENT_TABLE(lmSplashFrame, wxFrame)
    EVT_PAINT(lmSplashFrame::OnPaint)

    EVT_TIMER(wxSPLASH_TIMER_ID, lmSplashFrame::OnNotify)
    EVT_CLOSE(lmSplashFrame::OnCloseWindow)

#ifdef __WXGTK__
    EVT_WINDOW_CREATE(lmSplashFrame::OnWindowCreate)
#endif
END_EVENT_TABLE()


// frame constructor
lmSplashFrame::lmSplashFrame(const wxBitmap& bitmap, const wxColour& transparentColor,
                             long splashStyle, int milliseconds, 
                             wxWindow* parent, wxWindowID id, const wxPoint& pos, 
                             const wxSize& size, long style)
       : wxFrame(parent, id, wxEmptyString, wxDefaultPosition, wxSize(100, 100),
                 wxFRAME_SHAPED
                 | wxBORDER_NONE     //wxBORDER_SIMPLE
                 | wxFRAME_NO_TASKBAR
                 | wxSTAY_ON_TOP
            )
{
    m_fHasShape = false;
    m_fDestroyable = false;

    m_transparentColor = transparentColor;
    m_bmp = bitmap;
    SetSize(wxSize(m_bmp.GetWidth(), m_bmp.GetHeight()));

	// message area
    //AWARE: As user could have different settings for normal font size it is
    //better not to do this here. Use a picture instead. Also I have problems
    //to have transparent text background
#if 0
    wxString sMsg = _T("Version ");
    sMsg += wxGetApp().GetVersionNumber();

    wxStaticText* pText1 = new wxStaticText(this, wxID_ANY, sMsg, wxPoint(370, 150), 
		wxDefaultSize);
    wxFont font(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL,
        false, _T("Verdana"));      //false = underline parameter

    wxColour color(255, 255, 255, wxALPHA_TRANSPARENT);
    font.SetWeight( wxFONTWEIGHT_BOLD );
    pText1->SetFont(font);
    pText1->SetBackgroundColour(color);

    // copyright message
    wxString sCopy = _T("Copyright ");
    sCopy += 169;       //copyright symbol
    sCopy += _T(" 2002-2009 LenMus project");
    wxStaticText* pText3 = new wxStaticText(this, wxID_ANY, sCopy, wxPoint(180, 200), 
		wxDefaultSize);
    font.SetWeight( wxFONTWEIGHT_BOLD );
    pText3->SetFont(font);
    pText3->SetBackgroundColour(color);

    // licence message
    wxString sLicense = _("Free software under GNU General Public License, version 3 or later.");
    wxStaticText* pText2 = new wxStaticText(this, wxID_ANY, sLicense, wxPoint(70, 310), 
		wxDefaultSize);
    font.SetPointSize(7);
    font.SetWeight( wxFONTWEIGHT_NORMAL );
    pText2->SetFont(font);
    pText2->SetBackgroundColour(color);
#endif


#ifndef __WXGTK__
    // On wxGTK we can't do this yet because the window hasn't been created
    // yet so we wait until the EVT_WINDOW_CREATE event happens.  On wxMSW and
    // wxMac the window has been created at this point so we go ahead and set
    // the shape now.
    SetWindowShape();
#endif

    // reposition according to splash style
    if (splashStyle & lmSPLASH_CENTRE_ON_PARENT)
        CentreOnParent();
    else if (splashStyle & lmSPLASH_CENTRE_ON_SCREEN)
        CentreOnScreen();

    // start timer if requested
    if (splashStyle & lmSPLASH_TIMEOUT) {
        m_timer.SetOwner(this, wxSPLASH_TIMER_ID);
        m_timer.Start(milliseconds, true);
        m_fTimedOut = false;
    }
    else {
        m_fTimedOut = true;
    }


    // show it
    Show(true);
#if defined( __WXMSW__ ) || defined(__WXMAC__)
    Update(); // Without this, you see a grey splash for an instant
#endif

}

lmSplashFrame::~lmSplashFrame()
{
    m_timer.Stop();
}

void lmSplashFrame::SetWindowShape()
{
    wxRegion region(m_bmp, m_transparentColor);
    m_fHasShape = SetShape(region);
}

void lmSplashFrame::OnPaint(wxPaintEvent& WXUNUSED(evt))
{
    wxPaintDC dc(this);
    dc.DrawBitmap(m_bmp, 0, 0, true);       //true->transparent
}

void lmSplashFrame::OnWindowCreate(wxWindowCreateEvent& WXUNUSED(evt))
{
    SetWindowShape();
}

void lmSplashFrame::OnNotify(wxTimerEvent& WXUNUSED(event))
{
    m_fTimedOut = true;
    if (m_fDestroyable) Close(true);
}

void lmSplashFrame::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
    m_timer.Stop();
    this->Destroy();
}

void lmSplashFrame::AllowDestroy() {
    m_fDestroyable = true;
    if (m_fTimedOut) Close(true);
}

//! Force inmediate termination
void lmSplashFrame::TerminateSplash()
{
    m_timer.Stop();
    Close(true);

}


