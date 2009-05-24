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

void lmDrawer::SolidLine(lmLUnits ux1, lmLUnits uy1, lmLUnits ux2, lmLUnits uy2,
                           lmLUnits uWidth, lmELineEdges nEdge, wxColor color)
{
    double alpha = atan((uy2 - uy1) / (ux2 - ux1));

    switch(nEdge) {
        case lm_eEdgeNormal:
            // edge line is perpendicular to line
            {
            lmLUnits uIncrX = (lmLUnits)( (uWidth * sin(alpha)) / 2.0 );
            lmLUnits uIncrY = (lmLUnits)( (uWidth * cos(alpha)) / 2.0 );
            lmUPoint uPoints[] = {
                lmUPoint(ux1+uIncrX, uy1-uIncrY),
                lmUPoint(ux1-uIncrX, uy1+uIncrY),
                lmUPoint(ux2-uIncrX, uy2+uIncrY),
                lmUPoint(ux2+uIncrX, uy2-uIncrY)
            };
            SolidPolygon(4, uPoints, color);
            break;
            }

        case lm_eEdgeVertical:
            // edge is always a vertical line
            {
            lmLUnits uIncrY = (lmLUnits)( (uWidth / cos(alpha)) / 2.0 );
            lmUPoint uPoints[] = {
                lmUPoint(ux1, uy1-uIncrY),
                lmUPoint(ux1, uy1+uIncrY),
                lmUPoint(ux2, uy2+uIncrY),
                lmUPoint(ux2, uy2-uIncrY)
            };
            SolidPolygon(4, uPoints, color);
            break;
            }

        case lm_eEdgeHorizontal:
            // edge is always a horizontal line
            {
            lmLUnits uIncrX = (lmLUnits)( (uWidth / sin(alpha)) / 2.0 );
            lmUPoint uPoints[] = {
                lmUPoint(ux1+uIncrX, uy1),
                lmUPoint(ux1-uIncrX, uy1),
                lmUPoint(ux2-uIncrX, uy2),
                lmUPoint(ux2+uIncrX, uy2)
            };
            SolidPolygon(4, uPoints, color);
            break;
            }
    }

}



//------------------------------------------------------------------------------------
// lmDirectDrawer
//------------------------------------------------------------------------------------

void lmDirectDrawer::GetTextExtent(const wxString& string, lmLUnits* uw, lmLUnits* uh)
{
    lmPixels vWidth, vHeight;
    m_pDC->GetTextExtent(string, &vWidth, &vHeight);
    *uw = (lmLUnits)vWidth;
    *uh = (lmLUnits)vHeight;
}

void lmDirectDrawer::SketchLine(lmLUnits ux1, lmLUnits uy1, lmLUnits ux2, lmLUnits uy2,
                                wxColour color, int style)
{
    m_pDC->SetPen( wxPen(color, 1, style) );
    m_pDC->SetBrush( *wxTRANSPARENT_BRUSH );
    m_pDC->DrawLine((int)ux1, (int)uy1, (int)ux2, (int)uy2);
}

void lmDirectDrawer::SketchRectangle(lmUPoint uPoint, lmUSize uSize, wxColour color)
{
    wxPoint point = lmUPointToPoint(uPoint);
    m_pDC->SetPen( wxPen(color, 1, wxSOLID) );
    m_pDC->SetBrush( *wxTRANSPARENT_BRUSH );
    m_pDC->DrawRectangle(point, lmUSizeToSize(uSize) );
}

void lmDirectDrawer::SolidCircle(lmLUnits ux, lmLUnits uy, lmLUnits uRadius)
{
    wxPoint point((int)ux, (int)uy);
    m_pDC->DrawCircle(point, (int)uRadius);
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
    m_pDC->SetPen( wxPen(m_lineColor, (int)m_uLineWidth, wxSOLID) );
}

void lmDirectDrawer::SetLineWidth(lmLUnits uWidth)
{
    m_uLineWidth = uWidth;
    m_pDC->SetPen( wxPen(m_lineColor, (int)m_uLineWidth, wxSOLID) );
}

void lmDirectDrawer::SetPen(wxColour color, lmLUnits uWidth)
{
    m_lineColor = color;
    m_uLineWidth = uWidth;
    m_pDC->SetPen( wxPen(m_lineColor, (int)m_uLineWidth, wxSOLID) );
}
