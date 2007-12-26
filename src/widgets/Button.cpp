//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
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

IMPLEMENT_DYNAMIC_CLASS(lmBitmapButton, wxWindow)

BEGIN_EVENT_TABLE(lmBitmapButton, wxWindow)
    EVT_MOUSE_EVENTS(lmBitmapButton::OnMouseEvent)
    EVT_PAINT(lmBitmapButton::OnPaint)
    EVT_SIZE(lmBitmapButton::OnSizeEvent)
    EVT_ERASE_BACKGROUND(lmBitmapButton::OnEraseBackground)
END_EVENT_TABLE()

#define wxMB_COLOR_OVER wxColour(255, 255, 255)	//0xE8,0xE8,0xE8)
#define wxMB_COLOR_BG wxColour(0xD7,0xD7,0xD7)
#define wxMB_TEXT_MARGIN 8

lmBitmapButton::lmBitmapButton(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
				   char** upXPM, char** overXPM, char** downXPM, char** disXPM)
	: wxWindow(parent, id, pos, size)
{
    wxIcon *pIcons[4];
    memset(pIcons,0,sizeof(pIcons));

    if(upXPM) pIcons[0] = new wxIcon((const char **)upXPM);
    // if the over image is a NULL pointer, use a upper frame instead
    if(overXPM) pIcons[1] = new wxIcon((const char **)overXPM);
    // if the down image is a NULL pointer, use a lower frame instead
    if(downXPM) pIcons[2] = new wxIcon((const char **)downXPM);
    // without a disable image, the up image will be used
    if(disXPM) pIcons[3] = new wxIcon((const char **)disXPM);
    else {
	   if(upXPM) pIcons[3] = new wxIcon((const char **)upXPM);
    }


	if(upXPM) m_bitmaps[0].CopyFromIcon(*pIcons[0]);
	if(overXPM) m_bitmaps[1].CopyFromIcon(*pIcons[1]);
	if(downXPM) m_bitmaps[2].CopyFromIcon(*pIcons[2]);
	if(disXPM || upXPM) m_bitmaps[3].CopyFromIcon(*pIcons[3]);


	Create(pos, size);
}

lmBitmapButton::lmBitmapButton(wxWindow* parent, wxWindowID id, const wxBitmap& bitmap, 
							 const wxPoint& pos, const wxSize& size) 
	: wxWindow(parent, id, pos, size)
{
    m_bitmaps[0] = bitmap;
    // without a disable image, the up image will be used
	m_bitmaps[3] = bitmap;

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
    if(size == wxDefaultSize) {
	   m_width = 32;
	   m_height = 32;
    }
    else {
	   m_width = size.x;
	   m_height = size.y;
    }

    m_state = State_ButtonUp;
    m_enabled = true;
    m_painted = false;
	m_nBorderDown = lm_eBorderSunken;
	m_nBorderOver = lm_eBorderHigh;

    // button font text
#ifdef __WIN32__
    m_font = new wxFont(8,wxDEFAULT,wxNORMAL,wxBOLD);
#else
    m_font = new wxFont(10,wxDEFAULT,wxNORMAL,wxBOLD);
#endif    
    m_laststate = State_ButtonNew;

    // use the up icon dimensions for the image
    int x = 0;
    if(m_bitmaps[0].IsOk()) x = m_bitmaps[0].GetWidth();
    if(x <= m_width) m_dx = (m_width - x) >> 1;
    else m_dx = 0;
    m_dy = 0;

    SetSize(m_width, m_height);
    m_bitmap = new wxBitmap(m_width, m_height);

};

void lmBitmapButton::SetText(const wxChar* text)
{
    int w,h,x = 0;
    wxClientDC dc(this);
    dc.SetFont(*m_font);
    m_text = text;
    dc.GetTextExtent(text,&w,&h);
    if(w > m_width) m_width = w + wxMB_TEXT_MARGIN;
    m_height += m_font->GetPointSize()+2;
    SetSize(m_width, m_height);

    if(m_bitmaps[0].IsOk()) x = m_bitmaps[0].GetWidth();
    if(x <= m_width) m_dx = (m_width - x) >> 1;
    else m_dx = 0;

    m_bitmap->Create(m_width, m_height);
    Redraw();
};

lmBitmapButton::~lmBitmapButton()
{
    delete m_bitmap;
    delete m_font;
};

void lmBitmapButton::DrawBorder(wxDC& dc, lmEButtonBorder border)
{
	wxColour bg = GetParent()->GetBackgroundColour();
	wxBrush brush_over(wxMB_COLOR_OVER,wxSOLID);
	dc.SetBrush(brush_over);
	dc.SetPen(*wxTRANSPARENT_PEN);
	dc.DrawRectangle(0,0, m_width, m_height);

	//wxPen light(wxColour(0xFF,0xFF,0xFF),1,wxSOLID);
    //wxPen dark(wxColour(0x80,0x80,0x80),1,wxSOLID);
    //wxPen corner(wxMB_COLOR_BG,1,wxSOLID);

    ////wxPen light(wxSystemSettings::GetColour(wxSYS_COLOUR_3DHILIGHT), 1, wxSOLID);
    wxPen light(wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT), 1, wxSOLID);
    wxPen corner(wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW), 1, wxSOLID);
    wxPen dark(wxSystemSettings::GetColour(wxSYS_COLOUR_3DDKSHADOW), 1, wxSOLID);
    wxPen over(wxColour(251, 202, 106), 1, wxSOLID);
    wxPen flat(wxColour(42, 85, 255), 1, wxSOLID);

    //wxPen light(wxColour(220, 220, 0), 1, wxSOLID);
    //wxPen corner(wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW), 1, wxSOLID);
    //wxPen dark(wxColour(80, 0, 255), 1, wxSOLID);

    switch(border)
	{
		case lm_eBorderHigh:
			dc.SetPen(light);
			dc.DrawLine(1,0, m_width-2,0);
			dc.DrawLine(0,1,0, m_height-2);
			dc.SetPen(dark);
			dc.DrawLine(0, m_height-1, m_width-1, m_height-1);
			dc.DrawLine(m_width-1,0, m_width-1, m_height-1);
			break;

		case lm_eBorderSunken:
			dc.SetPen(dark);
			dc.DrawLine(1,0, m_width-2,0);
			dc.DrawLine(0,1,0, m_height-2);
			dc.SetPen(light);
			dc.DrawLine(1, m_height-1, m_width-1, m_height-1);
			dc.DrawLine(m_width-1,1, m_width-1, m_height-1);
			break;

		case lm_eBorderFlat:
			dc.SetPen(flat);
			dc.DrawLine(1, 0, m_width-2,0);
			dc.DrawLine(0, 1, 0, m_height-2);
			dc.DrawLine(1, m_height-1, m_width-1, m_height-1);
			dc.DrawLine(m_width-1, 1, m_width-1, m_height-1);
			break;

		case lm_eBorderOver:
			dc.SetPen(over);
			dc.DrawLine(1, 0, m_width-2,0);
			dc.DrawLine(0, 1, 0, m_height-2);
			dc.DrawLine(1, m_height-1, m_width-1, m_height-1);
			dc.DrawLine(m_width-1, 1, m_width-1, m_height-1);
			break;

		default:
			break;
    }
};

void lmBitmapButton::DrawOnBitmap()
{
    wxCoord dx = 0;
    wxCoord dy = 0;
    wxCoord w;
    wxCoord h;

    wxMemoryDC dc;
    dc.SelectObject(*m_bitmap);
    // use the system background colour for buttons (wxSYS_COLOUR_3DFACE)
	wxBrush brush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE),
				  wxSOLID);
	wxColour bg = m_parent->GetBackgroundColour();
	dc.SetBackground(brush);
	dc.Clear();

	dc.SetFont(*m_font);

    switch(m_state)
	{
		case State_ButtonUp:
			if (m_bitmaps[m_state].IsOk())
				dc.DrawBitmap(m_bitmaps[m_state], m_dx, m_dy, true); 
			break;

		case State_ButtonOver:
			if (m_bitmaps[m_state].IsOk())
				dc.DrawBitmap(m_bitmaps[m_state], m_dx, m_dy, true); 
			else {
				DrawBorder(dc, m_nBorderOver);
				if(m_bitmaps[State_ButtonUp].IsOk()) {
					dc.DrawBitmap(m_bitmaps[State_ButtonUp], m_dx, m_dy, true);
				}
			}
			break;

		case State_ButtonDown:
			if (m_bitmaps[m_state].IsOk())
				dc.DrawBitmap(m_bitmaps[m_state], m_dx, m_dy, true); 
			else {
				DrawBorder(dc, m_nBorderDown);
				if(m_bitmaps[State_ButtonUp].IsOk()) {
					dc.DrawBitmap(m_bitmaps[State_ButtonUp], m_dx+1, m_dy+1, true);
				}
			}
			dx = dy = 1;
			break;

		case State_ButtonDis:
			if (m_bitmaps[m_state].IsOk())
				dc.DrawBitmap(m_bitmaps[m_state], m_dx, m_dy, true); 
			else {
			}
			break;

		default:
			dc.SelectObject(wxNullBitmap);
			return;
    }

    dc.GetTextExtent(m_text,&w,&h);
    if(w > m_width) w = m_width;
    if(h > m_height) h = m_height;

    if(!m_text.IsEmpty()) {
	   if(m_enabled) {
		  dc.SetTextForeground(wxColour(0,0,0));
		  dc.DrawText(m_text,((m_width-w)>>1)+1+dx, m_height-h+dy);
	   }
	   else {
		  dc.SetTextForeground(wxColour(255,255,255));
		  dc.DrawText(m_text,((m_width-w)>>1)+1+dx, m_height-h+dy);
		  dc.SetTextForeground(wxColour(128,128,128));
		  dc.DrawText(m_text,((m_width-w)>>1)+dy, m_height-h-1+dy);
	   }
    }

    dc.SelectObject(wxNullBitmap);
};

void lmBitmapButton::OnPaint(wxPaintEvent & event)
{
    wxPaintDC dc(this);
    if(!m_painted) {
	   DrawOnBitmap();
	   m_painted = true;
    }
    dc.DrawBitmap(*m_bitmap,0,0,false);
};

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
};

void lmBitmapButton::OnMouseEvent(wxMouseEvent & event)
{
    if(!m_enabled) return;

    wxCommandEvent ev(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
    ev.SetEventObject(GetParent());

    if(event.ButtonDown()) {
	   m_state = State_ButtonDown;
	   Redraw();
    }
    else if(event.ButtonUp()) {
	   m_state = State_ButtonOver;
	   wxPoint pos = event.GetPosition();
	   if((pos.x < GetSize().GetWidth()) && 
		 (pos.y < GetSize().GetHeight())) {
		  GetParent()->ProcessEvent(ev);
	   }
	   Redraw();
    }
    else if (event.Entering()) {
	   m_state = State_ButtonOver;
	   Redraw();
	   return;
    }
    else if(event.Leaving()) {
	   if((m_state == State_ButtonOver) || (m_state == State_ButtonDown)) {
		  m_state = State_ButtonUp;
		  Redraw();
		  return;
	   }
    }
};

void lmBitmapButton::OnSizeEvent(wxSizeEvent &event)
{
    Redraw();
    event.Skip();
};

void lmBitmapButton::Enable()
{
    if(!m_enabled) {
	   m_state = m_laststate;
	   m_enabled = true;
	   Redraw();
    }
};

void lmBitmapButton::Disable()
{
    if(m_enabled) {
	   m_laststate = m_state;
	   m_enabled = false;
	   m_state = State_ButtonDis;
	   Redraw();
    }
};

bool lmBitmapButton::Press()
{
    if(m_state != State_ButtonDown) {
	   m_state = State_ButtonDown;
	   Redraw();
	   return true;
    }
    return false;
};

void lmBitmapButton::Redraw()
{
    if(m_painted) {    
	   DrawOnBitmap();
	   wxClientDC dc(this);
	   dc.DrawBitmap(*m_bitmap,0,0,false);
    }
};

bool lmBitmapButton::Release()
{
    if(m_state == State_ButtonDown) {
	   m_state = State_ButtonUp;
	   Redraw();
	   return true;
    }
    return false;
};





BEGIN_EVENT_TABLE(lmCheckButton, lmBitmapButton)
    EVT_MOUSE_EVENTS(lmCheckButton::OnMouseEvent)
END_EVENT_TABLE()

lmCheckButton::lmCheckButton(wxWindow* parent, wxWindowID id, const wxBitmap& bitmap, 
							 const wxPoint& pos, const wxSize& size) 
	: lmBitmapButton(parent, id, bitmap, pos, size)
{
	m_fButtonDown = false;
};

lmCheckButton::lmCheckButton(wxWindow* parent, wxWindowID id, char** upXPM, 
							 const wxPoint& pos, const wxSize& size,
							 char** overXPM, char** downXPM, char** disXPM)
	: lmBitmapButton(parent, id, pos, size, upXPM, overXPM, downXPM, disXPM)
{
	m_fButtonDown = false;
};

void lmCheckButton::OnMouseEvent(wxMouseEvent & event)
{
    if(!m_enabled) return;

    wxCommandEvent ev(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
    ev.SetEventObject(GetParent());

    if(event.ButtonDown()) {
	   m_state = State_ButtonDown;
	   Redraw();
    }
    else if(event.ButtonUp()) {
	   wxPoint pos = event.GetPosition();
	   if((pos.x < GetSize().GetWidth()) && 
		 (pos.y < GetSize().GetHeight())) {
		  // mouse up must inside button range, toggle internal state
		  m_fButtonDown ^= true;
		  if(m_fButtonDown) m_state = State_ButtonDown;
		  else m_state = State_ButtonOver;
		  // set it in the event
		  ev.SetInt(m_fButtonDown);
		  // and send it to the parent
		  GetParent()->ProcessEvent(ev);
		  Redraw();
	   }
    }
    else if (event.Entering()) {
	   // do not redraw ButtonOver state if button is pressed
	   if(m_fButtonDown) return;
	   m_state = State_ButtonOver;
	   Redraw();
	   return;
    }
    else if(event.Leaving()) {
	   // do not redraw ButtonOver state if button is pressed
	   if(m_fButtonDown) return;
	   if((m_state == State_ButtonOver) || (m_state == State_ButtonDown)) {
		  m_state = State_ButtonUp;
		  Redraw();
		  return;
	   }
    }
};

bool lmCheckButton::Press()
{
    bool result = false;
    if((result = lmBitmapButton::Press()) == true) {
	   m_fButtonDown = true;
    }
    return result;
};

bool lmCheckButton::Release()
{
    bool result = false;
    if((result = lmBitmapButton::Release()) == true) {
	   m_fButtonDown = false;
    }
    return result;
};




