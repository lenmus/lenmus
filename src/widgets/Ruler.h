// RCS-ID: $Id: Ruler.h,v 1.3 2006/02/23 19:26:12 cecilios Exp $
//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
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
/*! @file Ruler.h
    @brief Header file for class lmRuler
    @ingroup widgets
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __RULERH__        //to avoid nested includes
#define __RULERH__

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

   enum RulerFormat {
      MetricFormat,
      EnglishFormat,
      PointsFormat
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
    void SetFormat(RulerFormat format) { m_format = format; }
    void SetOffset(int offset) { m_offset = offset; }

private:
    int            m_left, m_top, m_width, m_height;
    double        m_rScale;
    int            m_Orientation;        // ruler orientation: wxHORIZONTAL || wxVERTICAL
    wxFont*        m_pFont;            // font for numbers
    RulerFormat m_format;
    wxPoint        m_oldPos;            // last known mouse position over score canvas
    lmScoreView*    m_pView;            // view who controls the scrolling
    int        m_offset;            // offset to add to the marker line

    DECLARE_EVENT_TABLE()
};

#endif    // __RULERH__
