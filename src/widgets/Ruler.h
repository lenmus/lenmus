//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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

#ifndef __LM_RULER_H__        //to avoid nested includes
#define __LM_RULER_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Ruler.cpp"
#endif


#include <wx/dc.h>
#include <wx/event.h>
#include <wx/font.h>
#include <wx/panel.h>
#include <wx/window.h>

class lmScoreView;

class lmRuler : public wxPanel
{
    DECLARE_DYNAMIC_CLASS(lmRuler)

public:

   enum lmERulerFormat {
      lm_eMetricFormat,
      lm_eEnglishFormat,
      lm_ePointsFormat,
   };

   lmRuler(wxWindow* parent, lmScoreView* view, wxWindowID id,
              int orientation,        // wxHORIZONTAL || wxVERTICAL
              const wxPoint& pos,
              const int length);

   ~lmRuler();
    void NewSize(int x, int y, int length);
    void SetLentgh(int length);
    int GetWidth() { return ( (m_Orientation == wxHORIZONTAL) ? m_height : m_width ); }
    void ShowPosition(wxPoint pt);

    void OnPaint(wxPaintEvent &evt);
    void ScrollWindow(int dx, int dy, const wxRect *rect);


    void SetBounds(int left, int top, int length);
    void SetScale(double rScale) { m_rScale = rScale; }
    void SetFormat(lmERulerFormat format) { m_format = format; }
    void SetOffset(int offset) { m_offset = offset; }

private:
    lmScoreView*	m_pView;		//owner view
    wxFont*			m_pFont;		//font for numbers
    int				m_left, m_top, m_width, m_height;
    double			m_rScale;
    int				m_Orientation;	//ruler orientation: wxHORIZONTAL || wxVERTICAL
    lmERulerFormat	m_format;
    wxPoint			m_oldPos;		//last known mouse position over score canvas
    int				m_offset;		//offset to add to the marker line

    DECLARE_EVENT_TABLE()
};

#endif    // __LM_RULER_H__
