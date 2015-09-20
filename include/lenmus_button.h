//---------------------------------------------------------------------------------------
//    This file is based on file awx/button.h from AWX 0.2 (Additional wxWindow Classes),
//	  downloaded from http://www.iftools.com/awx.en.html
//
//    Author:				Joachim Buermann
//    Copyright:			(c) 2003,2004 Joachim Buermann
//	  Original licence:		wxWindows
//	  More info				$Id: button.h,v 1.3 2004/08/30 10:20:19 jb Exp $
//
//    Modified by:
//        Cecilio Salmeron
//
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2015 LenMus project
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
#ifndef __LENMUS_BUTTON_H__
#define __LENMUS_BUTTON_H__

//wxWidgets
#include <wx/wx.h>
#include <wx/bitmap.h>

class wxFont;

namespace lenmus
{



enum EButtonBorder {
	k_border_Sunken,
	k_border_Flat,
	k_border_High,
	k_border_Over,
};


class BitmapButton : public wxWindow
{

wxDECLARE_DYNAMIC_CLASS(BitmapButton);

public:
	BitmapButton(wxWindow* parent, wxWindowID id, const wxBitmap& bitmap,
			const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
    BitmapButton() {};
    virtual ~ BitmapButton();

    virtual void Disable();
    virtual void Enable();
    virtual bool Enable(bool enable) {
	   if(enable) Enable();
	   else Disable();
	   return true;
    };
    virtual bool IsPressed() {return m_state == k_button_Down;};
    bool IsEnabled() {return m_enabled;};
    virtual void OnPaint(wxPaintEvent &event);
    virtual void OnEraseBackground(wxEraseEvent &event);
    virtual void OnMouseEvent(wxMouseEvent& event);
    virtual void OnSizeEvent(wxSizeEvent& event);
    virtual bool Press();
    virtual bool Release();
    virtual void SetText(const wxChar* text);
	virtual void SetBorderDown(EButtonBorder nBorderType) { m_nBorderDown = nBorderType; }
	virtual void SetBorderOver(EButtonBorder nBorderType) { m_nBorderOver = nBorderType; }

	virtual void SetBitmapDown(const wxBitmap& bitmap);
	virtual void SetBitmapOver(const wxBitmap& bitmap);

    virtual void SetBitmapUp(wxString sBmpName, wxString sBg, wxSize size);
    virtual void SetBitmapDown(wxString sBmpName, wxString sBg, wxSize size);
    virtual void SetBitmapOver(wxString sBmpName, wxString sBg, wxSize size);
    virtual void SetBitmapDisabled(wxString sBmpName, wxString sBg, wxSize size);


protected:
    enum EButtonState
    {
	   k_button_Up = 0,
	   k_button_Over,
	   k_button_Down,
	   k_button_Dis,
	   k_button_New
    };

    wxBitmap CreateBitmap(wxString sBmpName, wxString sBg, wxSize size);

	void Create(const wxPoint& pos, const wxSize& size);
    void Redraw();

    wxString    m_sNameUp;
    wxString    m_sNameDis;
    wxSize      m_btSize;
    int		    m_dx;
    int		    m_dy;

    bool	m_enabled;

    EButtonState		m_state;
    EButtonState		m_laststate;
    wxBitmap*		    m_bitmap;

    wxString	m_text;
    wxFont*		m_font;
	wxBitmap	m_bitmaps[4];

	EButtonBorder		m_nBorderDown;
	EButtonBorder		m_nBorderOver;


    wxDECLARE_EVENT_TABLE();
};

class CheckButton : public BitmapButton
{
public:
	CheckButton(wxWindow* parent, wxWindowID id, const wxBitmap& bitmap,
				  const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

	virtual void OnMouseEvent(wxMouseEvent& event);
    virtual bool Press();
    virtual bool Release();

protected:
    bool m_fButtonDown;


	wxDECLARE_EVENT_TABLE();
};



}   // namespace lenmus

#endif		// __LENMUS_BUTTON_H__
