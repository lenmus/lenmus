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
//---------------------------------------------------------------------------------
// A ruler around the score page
//
//    At a minimum, the user must specify the dimensions of the
//    ruler, its orientation (horizontal or vertical), and the
//    values displayed at the two ends of the ruler (min and max).
//    By default, this class will display tick marks at reasonable
//    round numbers and fractions, for example, 100, 50, 10, 5, 1,
//    0.5, 0.1, etc.
//
//    The class is designed to display a small handful of
//    labeled Major ticks, and a few Minor ticks between each of
//    these.  Minor ticks are labeled if there is enough space.
//    Labels will never run into each other.
//
//    lmRuler currently supports three formats for its display:
//        Metrics - shows tick marks for millimeters.
//        English - shows tick marks for thenths of inch.
//        Point   - show tick marks for thents of point
//
//*/
//---------------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "Ruler.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Ruler.h"
#include "../app/ScoreView.h"


#define lmRULER_WIDTH  15        // ruler width in pixels

BEGIN_EVENT_TABLE(lmRuler, wxPanel)
    EVT_PAINT(lmRuler::OnPaint)
END_EVENT_TABLE()

IMPLEMENT_CLASS(lmRuler, wxPanel)

lmRuler::lmRuler(wxWindow* parent,
                    lmScoreView* view,
                    wxWindowID id,
                    int orientation,
                    const wxPoint& pos,
                    const int length) :
   wxPanel(parent, id, pos, wxSize(10,10))
{

    // default settings
    m_format = lm_eMetricFormat;
    m_rScale = 1.0;
    wxASSERT(orientation == wxHORIZONTAL || orientation == wxVERTICAL);
    m_Orientation = orientation;
    m_oldPos = wxPoint(0,0);
    m_offset = 0;


    //create the font
    int fontSize = 10;
    #ifdef __WXMSW__
    fontSize = 8;
    #endif
    m_pFont = new wxFont(fontSize, wxSWISS, wxNORMAL, wxNORMAL);
    #ifdef __WXMAC__
    m_pFont->SetNoAntiAliasing(true);
    #endif

    // store received values
    wxASSERT(view);
    m_pView = view;
    SetBounds(pos.x, pos.y, length);
    SetSize(m_left, m_top, m_width, m_height);        //resize the window

}

lmRuler::~lmRuler()
{
    delete m_pFont;
}

void lmRuler::SetBounds(int left, int top, int length)
{
    m_left = left;
    m_top = top;
    if (m_Orientation == wxHORIZONTAL) {
        m_width = length;
        m_height = lmRULER_WIDTH;
    } else {
        m_width = lmRULER_WIDTH;
        m_height = length;
    }

}

void lmRuler::NewSize(int x, int y, int length)
{
    SetBounds(x, y, length);
    SetSize(m_left, m_top, m_width, m_height);
    //wxLogStatus(_T("NewSize =(%d, %d, %d, %d)"), m_left, m_top, m_width, m_height);
    Refresh();
}

void lmRuler::SetLentgh(int length)
{
    if (m_Orientation == wxHORIZONTAL) {
        m_width = length;
        m_height = lmRULER_WIDTH;
    } else {
        m_width = lmRULER_WIDTH;
        m_height = length;
    }
    SetSize(m_left, m_top, m_width, m_height);
    Refresh();

}

void lmRuler::OnPaint(wxPaintEvent &evt)
{
    wxPaintDC dc(this);

    // position de DC to take into account the scrolling
    int xScrollUnits, yScrollUnits, xOrigin, yOrigin;
    m_pView->GetViewStart(&xOrigin, &yOrigin);
    m_pView->GetScrollPixelsPerUnit(&xScrollUnits, &yScrollUnits);
     if (m_Orientation == wxHORIZONTAL) {
        dc.SetDeviceOrigin(-xOrigin * xScrollUnits, 0);
    } else {
        dc.SetDeviceOrigin(0, -yOrigin * yScrollUnits);
    }

    dc.SetPen(*wxBLACK_PEN);
    dc.SetBrush(*wxLIGHT_GREY_BRUSH);

    // draw ruler body rectangle
    dc.SetPen(*wxGREY_PEN);
    dc.SetBrush(*wxLIGHT_GREY_BRUSH);
    if (m_Orientation == wxHORIZONTAL) {
        dc.DrawRectangle(0, 0, m_width, m_height-1);
    } else {
        dc.DrawRectangle(0, 0, m_width-1, m_height);
    }

    // draw high lights lines
    dc.SetPen(*wxWHITE_PEN);
    if (m_Orientation == wxHORIZONTAL) {
        dc.DrawLine(0, 1, m_width, 1);
        dc.DrawLine(0, m_height-1, m_width, m_height-1);
    } else {
        dc.DrawLine(1, 0, 1, m_height);
        dc.DrawLine(m_width-1, 0, m_width-1, m_height);
    }

    //draw minor ticks every millimeter, major ticks every five millimeters and
    //numbers every centimeter
    dc.SetPen(*wxBLACK_PEN);
    dc.SetFont(*m_pFont);
    int x=0, y=0;
    wxString sBuffer;
    int fW, fH;            // font width and height
    int fX, fY;            // font position
    if (m_Orientation == wxHORIZONTAL) {
        for (x=0; x <= m_width; x+=10) {
            if ((x % 100) == 0) {
                // one centimeter: draw number
                sBuffer = wxString::Format(_T("%d"), x/100);
                dc.GetTextExtent(sBuffer, &fW, &fH);
                fX = x + 1 - fW / 2;
                fY = ((m_height - fH) ? (m_height - fH)/2 : 0);
                dc.DrawText(sBuffer, fX, fY);
            } else if ((x % 50) == 0) {
                // half centimeter: major tick
                dc.DrawLine(x, y+5, x, y+9);
            } else {
                // one millimeter: minor tick
                dc.DrawLine(x, y+6, x, y+8);
            }
        }

    } else {
        // Vertical ruler ticks and numbers
        for (y=0; y <= m_height; y+=10) {
            if ((y % 100) == 0) {
                // one centimeter: draw number
                sBuffer = wxString::Format(_T("%d"), y/100);
                //dc.DrawText(sBuffer, x+4, y);
                dc.GetTextExtent(sBuffer, &fW, &fH);
                fY = y - fH / 2;
                fX = ((m_width - fW) ? (m_width + 1 - fW)/2 : 0);
                dc.DrawText(sBuffer, fX, fY);
            } else if ((y % 50) == 0) {
                // half centimeter: major tick
                dc.DrawLine(x+5, y, x+9, y);
            } else {
                // one millimeter: minor tick
                dc.DrawLine(x+6, y, x+8, y);
            }
        }
    }

    // Draw marker line
    dc.SetLogicalFunction(wxEQUIV);
    if (m_Orientation == wxHORIZONTAL) {
        dc.DrawLine(m_oldPos.x, 0, m_oldPos.x, m_height);
    } else {
        dc.DrawLine(0, m_oldPos.y, m_width, m_oldPos.y);
    }

}


// Draw a marker on the ruler to show position
void lmRuler::ShowPosition(wxPoint mousePos)
{
    wxClientDC dc(this);

    dc.SetPen(*wxBLACK_PEN);
    dc.SetLogicalFunction(wxEQUIV);

    // Remove the old marker
    if (m_Orientation == wxHORIZONTAL) {
        dc.DrawLine(m_oldPos.x, 0, m_oldPos.x, m_height);
    } else {
        dc.DrawLine(0, m_oldPos.y, m_width, m_oldPos.y);
    }

    // Now draw in the new marker
    wxPoint pt = mousePos;
    pt.x += m_offset;
    pt.y += m_offset;
    if (m_Orientation == wxHORIZONTAL) {
        dc.DrawLine(pt.x, 0, pt.x, m_height);
    } else {
        dc.DrawLine(0, pt.y, m_width, pt.y);
    }

    // Store where the marker is currently drawn
    m_oldPos = pt;

}

// This override is needed to erase the marker line before scrolling and redrawing it
// after scrolling, to avoid the marker line being unproperly scrolled
void lmRuler::ScrollWindow(int dx, int dy, const wxRect *rect)
{
    wxClientDC dc(this);
    dc.SetPen(*wxBLACK_PEN);
    dc.SetLogicalFunction(wxEQUIV);

    // Remove the old marker
    if (m_Orientation == wxHORIZONTAL) {
        dc.DrawLine(m_oldPos.x, 0, m_oldPos.x, m_height);
    } else {
        dc.DrawLine(0, m_oldPos.y, m_width, m_oldPos.y);
    }

    // now scroll the window
    wxPanel::ScrollWindow(dx, dy, rect);        // scroll me

    // Repaint the marker
    if (m_Orientation == wxHORIZONTAL) {
        dc.DrawLine(m_oldPos.x, 0, m_oldPos.x, m_height);
    } else {
        dc.DrawLine(0, m_oldPos.y, m_width, m_oldPos.y);
    }

}
