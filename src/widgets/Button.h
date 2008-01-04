//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
//
//    This file is based on file awx/button.h from AWX 0.2 (Additional wxWindow Classes),
//	  downloaded from http://www.iftools.com/awx.en.html
//
//    Author:				Joachim Buermann
//    Copyright:			(c) 2003,2004 Joachim Buermann
//	  Original licence:		wxWindows
//	  More info				$Id: button.h,v 1.3 2004/08/30 10:20:19 jb Exp $
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
#ifndef __LM_BUTTON_H__
#define __LM_BUTTON_H__

//#include <wx/dcmemory.h>
#include <wx/wx.h>

enum lmEButtonBorder {
	lm_eBorderSunken,
	lm_eBorderFlat,
	lm_eBorderHigh,
	lm_eBorderOver,
};

class wxBitmap;

class lmBitmapButton : public wxWindow
{

DECLARE_DYNAMIC_CLASS (lmBitmapButton)

public:
	lmBitmapButton(wxWindow* parent, wxWindowID id, const wxBitmap& bitmap, 
			const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize); 
    lmBitmapButton(wxWindow * parent, wxWindowID id,
		    const wxPoint & pos,
		    const wxSize & size,
		    char **upXPM, char **overXPM, char **downXPM, char **disXPM);
    lmBitmapButton() {};
    virtual ~ lmBitmapButton();

    virtual void Disable();
    virtual void Enable();
    virtual bool Enable(bool enable) {
	   if(enable) Enable();
	   else Disable();
	   return true;
    };
    virtual bool IsPressed() {return m_state == State_ButtonDown;};
    bool IsEnabled() {return m_enabled;};
    virtual void OnPaint(wxPaintEvent &event);
    virtual void OnEraseBackground(wxEraseEvent &event);
    virtual void OnMouseEvent(wxMouseEvent& event);
    virtual void OnSizeEvent(wxSizeEvent& event);
    virtual bool Press();
    virtual bool Release();
    virtual void SetText(const wxChar* text);
	virtual void SetBorderDown(lmEButtonBorder nBorderType) { m_nBorderDown = nBorderType; }
	virtual void SetBorderOver(lmEButtonBorder nBorderType) { m_nBorderOver = nBorderType; }
	virtual void SetBitmapSelected(const wxBitmap& bitmap);


protected:
    enum ButtonState {
	   State_ButtonUp,
	   State_ButtonOver,
	   State_ButtonDown,
	   State_ButtonDis,
	   State_ButtonNew
    };


	void Create(const wxPoint& pos, const wxSize& size);
    void DrawBorder(wxDC& dc, lmEButtonBorder border);
    virtual void DrawOnBitmap();
    void Redraw();


    int		m_width;
    int		m_height;
    int		m_dx;
    int		m_dy;

    bool	m_enabled;

    ButtonState		m_state;
    ButtonState		m_laststate;
    wxBitmap*		m_bitmap;

    wxString	m_text;
    wxFont*		m_font;
	wxBitmap	m_bitmaps[4];
    bool		m_painted;

	lmEButtonBorder		m_nBorderDown;
	lmEButtonBorder		m_nBorderOver;


    DECLARE_EVENT_TABLE()
};

class lmCheckButton : public lmBitmapButton
{
public:
	lmCheckButton(wxWindow* parent, wxWindowID id, const wxBitmap& bitmap, 
				  const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
    lmCheckButton(wxWindow* parent, wxWindowID id, char** upXPM, 
				  const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
				  char** overXPM = NULL, char** downXPM = NULL, char** disXPM = NULL);

	virtual void OnMouseEvent(wxMouseEvent& event);
    virtual bool Press();
    virtual bool Release();

protected:
    bool m_fButtonDown;

	
	DECLARE_EVENT_TABLE()
};


#endif		// __LM_BUTTON_H__
