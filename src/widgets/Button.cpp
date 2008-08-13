//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
//
//    This file is based on file awx/button.cpp from AWX 0.2 (Additional wxWindow Classes),
//	  downloaded from http://www.iftools.com/awx.en.html
//
//    Author:       Joachim Buermann
//    Copyright:	(c) 2003 Joachim Buermann
//	  Licence:      wxWindows
//	  More info		$Id: button.cpp,v 1.2 2004/11/05 10:48:46 jb Exp $
//
//
//    Modified by:
//        Cecilio Salmeron
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

#include <wx/dc.h>
#include <wx/dcclient.h>
#include <wx/image.h>
#include <wx/settings.h>

#include "Button.h"
#include "../app/ArtProvider.h"

IMPLEMENT_DYNAMIC_CLASS(lmBitmapButton, wxWindow)

BEGIN_EVENT_TABLE(lmBitmapButton, wxWindow)
    EVT_MOUSE_EVENTS(lmBitmapButton::OnMouseEvent)
    EVT_PAINT(lmBitmapButton::OnPaint)
    EVT_SIZE(lmBitmapButton::OnSizeEvent)
    EVT_ERASE_BACKGROUND(lmBitmapButton::OnEraseBackground)
END_EVENT_TABLE()

#define wxMB_COLOR_OVER wxColour(0xff,0xff,0xff)	//0xE8,0xE8,0xE8)
#define wxMB_COLOR_DOWN wxColour(0xff,0xff,0xff)
#define wxMB_COLOR_BG wxColour(0xD7,0xD7,0xD7)
#define wxMB_TEXT_MARGIN 8


//lmBitmapButton
// Do not specify bitmap. It must later be set with specific methods

lmBitmapButton::lmBitmapButton(wxWindow* parent, wxWindowID id, const wxBitmap& bitmap, 
							 const wxPoint& pos, const wxSize& size) 
	: wxWindow(parent, id, pos, size)
{
    //if(size == wxDefaultSize)
    //    m_btSize = wxSize(32, 32);
    //else
    //    m_btSize = size;

    m_bitmaps[0] = bitmap;
    // without a disable image, the up image will be used
	m_bitmaps[lm_eButtonDis] = bitmap;

	//If the default size is specified then the button is sized appropriately
	//for the bitmap.
	wxSize nSize = size;
	if (size == wxDefaultSize)
		nSize = wxSize(bitmap.GetWidth(), bitmap.GetHeight());
	Create(pos, nSize);
	if (size == wxDefaultSize)
		SetSize(nSize);
}


void lmBitmapButton::Create(const wxPoint& pos, const wxSize& size)
{
    //if(size == wxDefaultSize) {
	   //m_btSize.x = 32;
	   //m_btSize.y = 32;
    //}
    //else {
	   //m_btSize.x = size.x;
	   //m_btSize.y = size.y;
    //}

    m_state = lm_eButtonUp;
    m_enabled = true;
	m_nBorderDown = lm_eBorderSunken;
	m_nBorderOver = lm_eBorderHigh;

    // button font text
#ifdef __WIN32__
    m_font = new wxFont(8,wxDEFAULT,wxNORMAL,wxBOLD);
#else
    m_font = new wxFont(10,wxDEFAULT,wxNORMAL,wxBOLD);
#endif    
    m_laststate = lm_eButtonNew;

    // use the up icon dimensions for the image
    int x = 0;
    if(m_bitmaps[0].IsOk()) x = m_bitmaps[0].GetWidth();
    if(x <= m_btSize.x) m_dx = (m_btSize.x - x) >> 1;
    else m_dx = 0;
    m_dy = 0;

    SetSize(m_btSize.x, m_btSize.y);
    m_bitmap = new wxBitmap(m_btSize.x, m_btSize.y);

}

void lmBitmapButton::SetText(const wxChar* text)
{
    int w,h,x = 0;
    wxClientDC dc(this);
    dc.SetFont(*m_font);
    m_text = text;
    dc.GetTextExtent(text,&w,&h);
    if(w > m_btSize.x) m_btSize.x = w + wxMB_TEXT_MARGIN;
    m_btSize.y += m_font->GetPointSize()+2;
    SetSize(m_btSize.x, m_btSize.y);

    if(m_bitmaps[0].IsOk()) x = m_bitmaps[0].GetWidth();
    if(x <= m_btSize.x) m_dx = (m_btSize.x - x) >> 1;
    else m_dx = 0;

    m_bitmap->Create(m_btSize.x, m_btSize.y);
    Redraw();
}

lmBitmapButton::~lmBitmapButton()
{
    delete m_bitmap;
    delete m_font;
}

//void lmBitmapButton::DrawOnBitmap()
//{
//    wxCoord dx = 0;
//    wxCoord dy = 0;
//    wxCoord w;
//    wxCoord h;
//
//    wxMemoryDC dc;
//    dc.SelectObject(*m_bitmap);
//    // use the system background colour for buttons (wxSYS_COLOUR_3DFACE)
//	wxBrush brush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE),
//				  wxSOLID);
//	wxColour bg = m_parent->GetBackgroundColour();
//	dc.SetBackground(brush);
//	dc.Clear();
//
//	dc.SetFont(*m_font);
//
//    switch(m_state)
//	{
//		case lm_eButtonUp:
//			if (m_bitmaps[m_state].IsOk())
//				dc.DrawBitmap(m_bitmaps[m_state], m_dx, m_dy, true); 
//			break;
//
//		case lm_eButtonOver:
//			if (m_bitmaps[m_state].IsOk())
//				dc.DrawBitmap(m_bitmaps[m_state], m_dx, m_dy, true); 
//			else {
//				DrawBorder(dc, m_nBorderOver);
//				if(m_bitmaps[lm_eButtonUp].IsOk()) {
//					dc.DrawBitmap(m_bitmaps[lm_eButtonUp], m_dx, m_dy, true);
//				}
//			}
//			break;
//
//		case lm_eButtonDown:
//			if (m_bitmaps[m_state].IsOk())
//				dc.DrawBitmap(m_bitmaps[m_state], m_dx, m_dy, true); 
//			else {
//				DrawBorder(dc, m_nBorderDown);
//				if(m_bitmaps[lm_eButtonUp].IsOk()) {
//					dc.DrawBitmap(m_bitmaps[lm_eButtonUp], m_dx+1, m_dy+1, true);
//				}
//			}
//			dx = dy = 1;
//			break;
//
//		case lm_eButtonDis:
//			if (m_bitmaps[m_state].IsOk())
//				dc.DrawBitmap(m_bitmaps[m_state], m_dx, m_dy, true); 
//			else {
//			}
//			break;
//
//		default:
//			dc.SelectObject(wxNullBitmap);
//			return;
//    }
//
//    dc.GetTextExtent(m_text,&w,&h);
//    if(w > m_btSize.x) w = m_btSize.x;
//    if(h > m_btSize.y) h = m_btSize.y;
//
//    if(!m_text.IsEmpty()) {
//	   if(m_enabled) {
//		  dc.SetTextForeground(wxColour(0,0,0));
//		  dc.DrawText(m_text,((m_btSize.x-w)>>1)+1+dx, m_btSize.y-h+dy);
//	   }
//	   else {
//		  dc.SetTextForeground(wxColour(255,255,255));
//		  dc.DrawText(m_text,((m_btSize.x-w)>>1)+1+dx, m_btSize.y-h+dy);
//		  dc.SetTextForeground(wxColour(128,128,128));
//		  dc.DrawText(m_text,((m_btSize.x-w)>>1)+dy, m_btSize.y-h-1+dy);
//	   }
//    }
//
//    dc.SelectObject(wxNullBitmap);
//}

void lmBitmapButton::OnPaint(wxPaintEvent & event)
{
	if (m_bitmaps[m_state].IsOk())
    {
        wxPaintDC dc(this);
	    dc.DrawBitmap(m_bitmaps[m_state], 0, 0, false);
    }
}

void lmBitmapButton::OnEraseBackground(wxEraseEvent &event)
{
#ifdef __WIN32__
    Redraw();
    event.Skip();
#else
    // at this time, the background image isn't repaint by the toolbar,
    // so defer the redraw for later
    wxSizeEvent ev(GetSize(),GetId());
    AddPendingEvent(ev);
    event.Skip();
#endif
}

void lmBitmapButton::OnMouseEvent(wxMouseEvent & event)
{
    if(!m_enabled) return;

    wxCommandEvent ev(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
    ev.SetEventObject(GetParent());

    if(event.ButtonDown()) {
	   m_state = lm_eButtonDown;
	   Redraw();
    }
    else if(event.ButtonUp()) {
	   m_state = lm_eButtonOver;
	   wxPoint pos = event.GetPosition();
	   if((pos.x < GetSize().GetWidth()) && 
		 (pos.y < GetSize().GetHeight())) {
		  GetParent()->ProcessEvent(ev);
	   }
	   Redraw();
    }
    else if (event.Entering()) {
	   m_state = lm_eButtonOver;
	   Redraw();
	   return;
    }
    else if(event.Leaving()) {
	   if((m_state == lm_eButtonOver) || (m_state == lm_eButtonDown)) {
		  m_state = lm_eButtonUp;
		  Redraw();
		  return;
	   }
    }
}

void lmBitmapButton::OnSizeEvent(wxSizeEvent &event)
{
    Redraw();
    event.Skip();
}

void lmBitmapButton::Enable()
{
    if(!m_enabled) {
	   m_state = m_laststate;
	   m_enabled = true;
	   Redraw();
    }
}

void lmBitmapButton::Disable()
{
    if(m_enabled) {
	   m_laststate = m_state;
	   m_enabled = false;
	   m_state = lm_eButtonDis;
	   Redraw();
    }
}

bool lmBitmapButton::Press()
{
    if(m_state != lm_eButtonDown) {
	   m_state = lm_eButtonDown;
	   Redraw();
	   return true;
    }
    return false;
}

void lmBitmapButton::Redraw()
{
	if (m_bitmaps[m_state].IsOk())
    {
        wxClientDC dc(this);
	    dc.DrawBitmap(m_bitmaps[m_state], 0, 0, false);
    }
}

bool lmBitmapButton::Release()
{
    if(m_state == lm_eButtonDown) {
	   m_state = lm_eButtonUp;
	   Redraw();
	   return true;
    }
    return false;
}

wxBitmap lmBitmapButton::CreateBitmap(wxString sBmpName, wxString sBg, wxSize size)
{
    //create bitmap and fill it with background colour
    wxMemoryDC dcFinal;
	wxBitmap bmpFinal(size.x, size.y);
    dcFinal.SelectObject(bmpFinal);
	wxBrush brush( GetParent()->GetBackgroundColour() );
	dcFinal.SetBackground(brush);
	dcFinal.Clear();

    //overlay button decoration
    if (sBg != _T(""))
    {
        wxMemoryDC dcBt;
	    wxBitmap bmpBt = wxArtProvider::GetBitmap(sBg, wxART_TOOLBAR, size);
        dcBt.SelectObject(bmpBt);

        //blend with background bitmap
        dcFinal.Blit(0, 0, size.x, size.y, &dcBt, 0, 0, wxCOPY, true);
        dcBt.SelectObject(wxNullBitmap);
    }

    //overlay with image bitmap
	wxBitmap bmpImg = wxArtProvider::GetBitmap(sBmpName, wxART_TOOLBAR, size);
    wxMemoryDC dcImg;
    dcImg.SelectObject(bmpImg);
    dcFinal.Blit(0, 0, size.x, size.y, &dcImg, 0, 0, wxCOPY, true);
    dcImg.SelectObject(wxNullBitmap);

    //clean up and return new bitmap
    dcFinal.SelectObject(wxNullBitmap);

    return bmpFinal;
} 

void lmBitmapButton::SetBitmapDown(const wxBitmap& bitmap)
{
	m_bitmaps[lm_eButtonDown] = bitmap;
}

void lmBitmapButton::SetBitmapOver(const wxBitmap& bitmap)
{
	m_bitmaps[lm_eButtonOver] = bitmap;
}

void lmBitmapButton::SetBitmapDown(wxString sBmpName, wxString sBg, wxSize size)
{
	m_bitmaps[lm_eButtonDown] = CreateBitmap(sBmpName, sBg, size);
}

void lmBitmapButton::SetBitmapOver(wxString sBmpName, wxString sBg, wxSize size)
{
	m_bitmaps[lm_eButtonOver] = CreateBitmap(sBmpName, sBg, size);
}

void lmBitmapButton::SetBitmapUp(wxString sBmpName, wxString sBg, wxSize size)
{
	m_bitmaps[lm_eButtonUp] = CreateBitmap(sBmpName, sBg, size);
}

void lmBitmapButton::SetBitmapDisabled(wxString sBmpName, wxString sBg, wxSize size)
{
	m_bitmaps[lm_eButtonDis] = CreateBitmap(sBmpName, sBg, size);
}





BEGIN_EVENT_TABLE(lmCheckButton, lmBitmapButton)
    EVT_MOUSE_EVENTS(lmCheckButton::OnMouseEvent)
END_EVENT_TABLE()

lmCheckButton::lmCheckButton(wxWindow* parent, wxWindowID id, const wxBitmap& bitmap, 
							 const wxPoint& pos, const wxSize& size) 
	: lmBitmapButton(parent, id, bitmap, pos, size)
{
	m_fButtonDown = false;
}

void lmCheckButton::OnMouseEvent(wxMouseEvent & event)
{
    if(!m_enabled) return;

    wxCommandEvent ev(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
    ev.SetEventObject(GetParent());

    if(event.ButtonDown())
    {
	   m_state = lm_eButtonDown;
	   Redraw();
    }
    else if(event.ButtonUp())
    {
	   wxPoint pos = event.GetPosition();
	   if((pos.x < GetSize().GetWidth()) && 
		 (pos.y < GetSize().GetHeight()))
       {
		  // mouse up must inside button range, toggle internal state
		  m_fButtonDown ^= true;
		  if(m_fButtonDown)
              m_state = lm_eButtonDown;
		  else
              m_state = lm_eButtonOver;
		  // set it in the event
		  ev.SetInt(m_fButtonDown);
		  // and send it to the parent
		  GetParent()->ProcessEvent(ev);
		  Redraw();
	   }
    }
    else if (event.Entering())
    {
	   // do not redraw ButtonOver state if button is pressed
	   if(m_fButtonDown)
           return;
	   m_state = lm_eButtonOver;
	   Redraw();
	   return;
    }
    else if(event.Leaving())
    {
	   // do not redraw ButtonOver state if button is pressed
	   if(m_fButtonDown) return;
	   if((m_state == lm_eButtonOver) || (m_state == lm_eButtonDown))
       {
		  m_state = lm_eButtonUp;
		  Redraw();
		  return;
	   }
    }
}

bool lmCheckButton::Press()
{
    bool result = false;
    if((result = lmBitmapButton::Press()) == true)
    {
	   m_fButtonDown = true;
    }
    return result;
}

bool lmCheckButton::Release()
{
    bool result = false;
    if((result = lmBitmapButton::Release()) == true)
    {
	   m_fButtonDown = false;
    }
    return result;
}




