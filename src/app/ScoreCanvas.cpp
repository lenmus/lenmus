//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
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

/*! @class lmScoreCanvas
    @ingroup app_gui
    @brief The lmScoreCanvas is the window on which the lmPaper object is placed.

    It is just a wxWindow and it is asumed that the view will scroll and
    modify its size according to the needs.
*/

#ifdef __GNUG__
#pragma implementation "ScoreCanvas.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif


#include "global.h"
#include "TheApp.h"
#include "MainFrame.h"
#include "ScoreDoc.h"
#include "scoreView.h"

#include "global.h"

// access to global external variables (to disable mouse interaction with the score)
extern bool g_fReleaseVersion;            // in TheApp.cpp
extern bool g_fReleaseBehaviour;        // in TheApp.cpp


BEGIN_EVENT_TABLE(lmScoreCanvas, wxWindow)
    EVT_ERASE_BACKGROUND(lmScoreCanvas::OnEraseBackground)
    EVT_MOUSE_EVENTS(lmScoreCanvas::OnMouseEvent)
    EVT_PAINT(lmScoreCanvas::OnPaint)
    LM_EVT_SCORE_HIGHLIGHT(lmScoreCanvas::OnVisualHighlight)
END_EVENT_TABLE()

// Define a constructor for my canvas
lmScoreCanvas::lmScoreCanvas(lmScoreView *v, wxWindow *parent, const wxPoint& pos,
        const wxSize& size, long style, wxColor colorBg) :
    wxWindow(parent, -1, pos, size, style)
{
    m_pView = v;
    wxASSERT(parent);
    m_pOwner = parent;
    m_colorBg = colorBg;

}

lmScoreCanvas::~lmScoreCanvas()
{
}


// Repainting behaviour
//
// We are going to use the technique of the virtual window. A bitmap, to act as the virtual
// window is mateined in the lmScoreView. All score display output is drawn onto this bitmap.
// Thus, to respond to a paint event we only have to copy the bitmap to the physical window.
// For optimization, only the damaged window rectangles will be repainted.
//
void lmScoreCanvas::OnPaint(wxPaintEvent &WXUNUSED(event))
{
    // In a paint event handler, the application must always create a wxPaintDC object,
    // even if it is not used. Otherwise, under MS Windows, refreshing for this and
    // other windows will go wrong.
    wxPaintDC dc(this);
    if (!m_pView) return;

    // get the updated rectangles list
    wxRegionIterator upd(GetUpdateRegion());

    // iterate to redraw each damaged rectangle
    // The rectangles are in pixels, referred to the client area, and are unscrolled
    while (upd) {
        wxRect rect = upd.GetRect();
        m_pView->RepaintScoreRectangle(&dc, rect);
        upd++;
    }

}

void lmScoreCanvas::OnMouseEvent(wxMouseEvent& event)
{
    if (!m_pView) return;
    wxClientDC dc(this);

    //Disable interaction with the score. Only mouse wheel allowed
    //Only for release version
    if (g_fReleaseVersion || g_fReleaseBehaviour) {
        if (event.GetEventType() == wxEVT_MOUSEWHEEL) {
            m_pView->OnMouseEvent(event, &dc);
        }
    }
    else
        m_pView->OnMouseEvent(event, &dc);

}

void lmScoreCanvas::OnEraseBackground(wxEraseEvent& event)
{
#if 1
    // Code for flooding the background with a color
    wxDC* pDC = event.GetDC();
    wxBrush brush(m_colorBg, wxSOLID);
    pDC->SetBackground(brush);
    pDC->Clear();

#else
    //    Code for tiling the background with a bitmap

    wxBitmap& bitmap = wxGetApp().GetBackgroundBitmap();
    if (bitmap.Ok() ) {
        wxSize sz = GetClientSize();    // size (pixels) of the visible area
        int xOrg, yOrg;
        m_pView->GetViewStart(&xOrg, &yOrg);        // start point (scroll units) of the visible portion of the window
        int pixelsPerStepX, pixelsPerStepY;
        m_pView->GetScrollPixelsPerUnit(&pixelsPerStepX, &pixelsPerStepY);
        xOrg *= pixelsPerStepX;
        yOrg *= pixelsPerStepY;

        int w = bitmap.GetWidth();
        int h = bitmap.GetHeight();
        int xShift = xOrg % w;
        int yShift = yOrg % h;
        wxRect rect(-xShift, -yShift, sz.x+xShift, sz.y+yShift);

        if (event.GetDC() ) {
            TileBitmap(rect, *(event.GetDC()), bitmap);
        } else {
            wxClientDC dc(this);
            TileBitmap(rect, dc, bitmap);
        }
    }
    else
        event.Skip();        // The official way of doing it
#endif
}

bool lmScoreCanvas::TileBitmap(const wxRect& rect, wxDC& dc, wxBitmap& bitmap)
{
    int w = bitmap.GetWidth();
    int h = bitmap.GetHeight();

    int i, j;
    for (i = rect.x; i < rect.x + rect.width; i += w) {
        for (j = rect.y; j < rect.y + rect.height; j+= h)
            dc.DrawBitmap(bitmap, i, j);
    }
    return true;
}

void lmScoreCanvas::OnVisualHighlight(lmScoreHighlightEvent& event)
{
    m_pView->OnVisualHighlight(event);
}

