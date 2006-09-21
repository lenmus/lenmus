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

void lmDrawer::DrawLine(lmLUnits x1, lmLUnits y1, lmLUnits x2, lmLUnits y2,
                           lmLUnits width)
{
    double alpha = atan((y2 - y1) / (x2 - x1));
    double incrX = (width * sin(alpha)) / 2.0;
    double incrY = (width * cos(alpha)) / 2.0;

    wxPoint points[] = {
        wxPoint(x1+incrX, y1-incrY),
        wxPoint(x1-incrX, y1+incrY),
        wxPoint(x2-incrX, y2+incrY),
        wxPoint(x2+incrX, y2-incrY)
    };
    DrawPolygon(4, points);

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

//void lmDirectDrawer::DrawRectangle(wxCoord left, wxCoord top, wxCoord width, wxCoord height)
//{
//    //is height is 0 
//    m_pDC->DrawRectangle(left, top, width, height);
//}
