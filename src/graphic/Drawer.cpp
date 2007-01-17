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
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "Drawer.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "Drawer.h"


lmDrawer::lmDrawer(wxDC* pDC)
{
    m_pDC = pDC;

}

void lmDrawer::SolidLine(lmLUnits x1, lmLUnits y1, lmLUnits x2, lmLUnits y2,
                           lmLUnits width, lmELineEdges nEdge, wxColor color)
{
    double alpha = atan((y2 - y1) / (x2 - x1));

    switch(nEdge) {
        case eEdgeNormal:
            // edge line is perpendicular to line
            {
            lmLUnits incrX = (lmLUnits)( (width * sin(alpha)) / 2.0 );
            lmLUnits incrY = (lmLUnits)( (width * cos(alpha)) / 2.0 );
            lmUPoint points[] = {
                lmUPoint(x1+incrX, y1-incrY),
                lmUPoint(x1-incrX, y1+incrY),
                lmUPoint(x2-incrX, y2+incrY),
                lmUPoint(x2+incrX, y2-incrY)
            };
            SolidPolygon(4, points, color);
            break;
            }

        case eEdgeVertical:
            // edge is always a vertical line
            {
            lmLUnits incrY = (lmLUnits)( (width / cos(alpha)) / 2.0 );
            lmUPoint points[] = {
                lmUPoint(x1, y1-incrY),
                lmUPoint(x1, y1+incrY),
                lmUPoint(x2, y2+incrY),
                lmUPoint(x2, y2-incrY)
            };
            SolidPolygon(4, points, color);
            break;
            }

        case eEdgeHorizontal:
            // edge is always a horizontal line
            {
            lmLUnits incrX = (lmLUnits)( (width / sin(alpha)) / 2.0 );
            lmUPoint points[] = {
                lmUPoint(x1+incrX, y1),
                lmUPoint(x1-incrX, y1),
                lmUPoint(x2-incrX, y2),
                lmUPoint(x2+incrX, y2)
            };
            SolidPolygon(4, points, color);
            break;
            }
    }

}



//------------------------------------------------------------------------------------
// lmDirectDrawer
//------------------------------------------------------------------------------------

void lmDirectDrawer::GetTextExtent(const wxString& string, lmLUnits* w, lmLUnits* h) 
{
    wxCoord width, height;
    m_pDC->GetTextExtent(string, &width, &height);
    *w = (lmLUnits)width;
    *h = (lmLUnits)height;
}

void lmDirectDrawer::SketchLine(lmLUnits x1, lmLUnits y1, lmLUnits x2, lmLUnits y2,
                                wxColour color)
{
    m_pDC->SetPen( wxPen(color, 1, wxSOLID) );
    m_pDC->SetBrush( *wxTRANSPARENT_BRUSH );
    m_pDC->DrawLine((int)x1, (int)y1, (int)x2, (int)y2);
}

void lmDirectDrawer::SketchRectangle(lmUPoint uPoint, wxSize size, wxColour color)
{
    wxPoint point = lmUPointToPoint(uPoint);
    m_pDC->SetPen( wxPen(color, 1, wxSOLID) );
    m_pDC->SetBrush( *wxTRANSPARENT_BRUSH );
    m_pDC->DrawRectangle(point, size);
}

void lmDirectDrawer::SolidCircle(lmLUnits x, lmLUnits y, lmLUnits radius)
{
    wxPoint point((int)x, (int)y);
    m_pDC->DrawCircle(point, (wxCoord)radius); 
}

void lmDirectDrawer::SolidPolygon(int n, lmUPoint points[], wxColour color)
{ 
    m_pDC->SetPen( wxPen(color, 1, wxSOLID) );
    m_pDC->SetBrush( wxBrush(color, wxSOLID) );

    wxPoint* pts = new wxPoint[n];
    int i;
    for (i= 0; i < n; i++)  pts[i] = lmUPointToPoint(points[i]);
    m_pDC->DrawPolygon(n, pts);
    delete[] pts;
}

wxColour lmDirectDrawer::GetFillColor()
{
    return m_fillColor;
}

void lmDirectDrawer::SetFillColor(wxColour color)
{
    m_fillColor = color;
    m_pDC->SetBrush( wxBrush(m_fillColor) );
}

wxColour lmDirectDrawer::GetLineColor()
{
    return m_lineColor;
}

void lmDirectDrawer::SetLineColor(wxColour color)
{
    m_lineColor = color;
    m_pDC->SetPen( wxPen(m_lineColor, m_uLineWidth, wxSOLID) );
}

void lmDirectDrawer::SetLineWidth(lmLUnits uWidth)
{
    m_uLineWidth = uWidth;
    m_pDC->SetPen( wxPen(m_lineColor, m_uLineWidth, wxSOLID) );
}

void lmDirectDrawer::SetPen(wxColour color, lmLUnits uWidth)
{
    m_lineColor = color;
    m_uLineWidth = uWidth;
    m_pDC->SetPen( wxPen(m_lineColor, m_uLineWidth, wxSOLID) );
}
