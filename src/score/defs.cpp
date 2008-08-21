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
//    This file contains some code copied (and modified) from file src/common/gdicmn.cpp
//    from wxWidgets 2.8.0 project:
//      - class lmURect is based in wxRect code.
//      - class lmUSize is based in wxSize code.
//    Author:       Julian Smart
//    Copyright (c) Julian Smart
//
//-------------------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "defs.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <math.h>
#include "wx/utils.h"
#include "defs.h"

// DOC:
// prefixes for variables:
//
//  a) standard types:
//      p - pointer
//      n - integer, long
//      s - string
//      r - real (float, double)
//
//  b) program specific types:
//      u - logical units (lmLUnits)
//      v - device units (lmPixels)
//      t - tenths (lmTenths)
//      ux, uy - logical units. coordinates
//      udx, udy - logical units. distances
//      vx, vy - device units. coordinates
//      vdx, vdy - device units. distances
//
//      Pitch
//      ap - lmAPitch
//      dp - lmDPitch
//      mp - lmMPitch
//      fp - lmFPitch
//
//


//helper functions to compare times (two floating point numbers)
bool IsEqualTime(float t1, float t2)
{
    return (fabs(t1 - t2) < 0.1f);
}

bool IsLowerTime(float t1, float t2)
{
    return (t1 < t2) && ((t2 - t1) >= 0.1f);
}

bool IsHigherTime(float t1, float t2)
{
    return (t1 > t2) && ((t1 - t2) >= 0.1f);
}


//Global variables used for default initialization
lmLocation g_tDefaultPos = {0.0, 0.0, lmLOCATION_DEFAULT,lmLOCATION_DEFAULT,lmTENTHS,lmTENTHS};

//-------------------------------------------------------------------------------------
// implementation of class lmURect code. A real rectangle (lmLUnits). Based on wxRect
//-------------------------------------------------------------------------------------

lmURect::lmURect(const lmUPoint& point1, const lmUPoint& point2)
{
    x = point1.x;
    y = point1.y;
    width = point2.x - point1.x;
    height = point2.y - point1.y;

    if (width < 0)
    {
        width = -width;
        x = point2.x;
    }
    width++;

    if (height < 0)
    {
        height = -height;
        y = point2.y;
    }
    height++;
}

//bool lmURect::operator==(const lmURect& rect) const
//{
//    return ((x == rect.x) &&
//            (y == rect.y) &&
//            (width == rect.width) &&
//            (height == rect.height));
//}
//
//lmURect lmURect::operator+(const lmURect& rect) const
//{
//    float x1 = wxMin(this->x, rect.x);
//    float y1 = wxMin(this->y, rect.y);
//    float y2 = wxMax(y+height, rect.height+rect.y);
//    float x2 = wxMax(x+width, rect.width+rect.x);
//    return lmURect(x1, y1, x2-x1, y2-y1);
//}

lmURect& lmURect::Union(const lmURect& rect)
{
    // ignore empty rectangles: union with an empty rectangle shouldn't extend
    // this one to (0, 0)
    if ( !width || !height )
    {
        *this = rect;
    }
    else if ( rect.width && rect.height )
    {
        float x1 = wxMin(x, rect.x);
        float y1 = wxMin(y, rect.y);
        float y2 = wxMax(y + height, rect.height + rect.y);
        float x2 = wxMax(x + width, rect.width + rect.x);

        x = x1;
        y = y1;
        width = x2 - x1;
        height = y2 - y1;
    }
    //else: we're not empty and rect is empty

    return *this;
}

//lmURect& lmURect::Inflate(wxCoord dx, wxCoord dy)
//{
//     if (-2*dx>width)
//     {
//         // Don't allow deflate to eat more width than we have,
//         // a well-defined rectangle cannot have negative width.
//         x+=width/2;
//         width=0;
//     }
//     else
//     {
//         // The inflate is valid.
//         x-=dx;
//         width+=2*dx;
//     }
//
//     if (-2*dy>height)
//     {
//         // Don't allow deflate to eat more height than we have,
//         // a well-defined rectangle cannot have negative height.
//         y+=height/2;
//         height=0;
//     }
//     else
//     {
//         // The inflate is valid.
//         y-=dy;
//         height+=2*dy;
//     }
//
//    return *this;
//}

bool lmURect::Contains(float cx, float cy) const
{
    return ( (cx >= x) && (cy >= y)
          && ((cy - y) < height)
          && ((cx - x) < width)
          );
}

bool lmURect::Contains(const lmURect& rect) const
{
    return Contains(rect.GetTopLeft()) && Contains(rect.GetBottomRight());
}

lmURect& lmURect::Intersect(const lmURect& rect)
{
    float x2 = GetRight(),
        y2 = GetBottom();

    if ( x < rect.x )
        x = rect.x;
    if ( y < rect.y )
        y = rect.y;
    if ( x2 > rect.GetRight() )
        x2 = rect.GetRight();
    if ( y2 > rect.GetBottom() )
        y2 = rect.GetBottom();

    width = x2 - x + 1;
    height = y2 - y + 1;

    if ( width <= 0 || height <= 0 )
    {
        width =
        height = 0;
    }

    return *this;
}

bool lmURect::Intersects(const lmURect& rect) const
{
    lmURect r = Intersect(rect);

    // if there is no floatersection, both width and height are 0
    return r.width != 0;
}


//---------------------------------------------------------------------------------------
//Helper functions for common operations
//---------------------------------------------------------------------------------------

//float round(float val)
//{
//	if (val > 0) {
//		return ceil(val+0.5);
//	} else {
//		return floor(val-0.5);
//	}
//}
//
//double round(double val)
//{
//	if (val > 0) {
//		return ceil(val+0.5);
//	} else {
//		return floor(val-0.5);
//	}
//}

bool StrToDouble(wxString sValue, double* pNumber)
{
    // There is a problem with wxString::ToDouble(). The issue is that apparently
    // the expected number format varies with locale settings. For example,
    // in my computer (Spanish locale) instead of decimal point (dot) it only
    // accepts comma. In LenMus, the real number format is always with dot. Therefore,
    // I cannot use ToDouble(). So I will look for the dot, extract the integer
    // part and the fraction part, convert both to double, and combine the results
	// Returns true if error.

	bool fError = false;
	int i = sValue.Find(_T("."));
	if (i != wxNOT_FOUND)
	{
		if (i > 0)
		{
			wxString sLeft = sValue.Left(i);
			fError |= !sLeft.ToDouble(pNumber);
		}
		else
			*pNumber = 0.0;

		double rRight;
		wxString sRight = sValue.substr(i+1);
		fError |= !sRight.ToDouble(&rRight);
		if (!fError)
		{
			*pNumber += rRight / pow(10.0, (int)sRight.length());
			return false;
		}
	}
    else if (sValue.ToDouble(pNumber))
	{
        return false;
    }
	return true;

}

wxString DoubleToStr(double rNumber, int nDecimalDigits)
{
    // In LenMus, the real number format is always with dot. Therefore,
    // I cannot use standard formatting sprintf %f, as decimal point could be
	// replaced by comma, depending on locale.
	// So this method edit the number using always a dot and the requested
	// decimal digits  %.Nf

	//I will use standard C funcion
	//	double modf(double x, double* intpart);
	//Breaks x into two parts: the integer part (stored in the object pointed by intpart)
	//and the fractional part (returned by the function). Each part has the same sign
	//as x.

	double rIntegerPart;
	double rDecimalPart = modf(rNumber, &rIntegerPart);
	double rDecimals = fabs(rDecimalPart) * pow(10.0, nDecimalDigits);
	//wxLogMessage(_T("[DoubleToStr] number=%.5f, integer=%.2f, decimal part=%.5f, num.decimal=%d, decimals=%.2f"),
	//	rNumber, rIntegerPart, rDecimalPart, nDecimalDigits, rDecimals);
	return wxString::Format(_T("%0.f.%0.f"), rIntegerPart, rDecimals );

}


lmLUnits lmToLogicalUnits(int nValue, lmEUnits nUnits)
{
    return lmToLogicalUnits((double)nValue, nUnits);
}

lmLUnits lmToLogicalUnits(double rValue, lmEUnits nUnits)
{
    // first convert to tenths of millimeter (mode MM_LOMETRIC), then divide by SCALE factor
    switch(nUnits) {
		case lmLUNITS:			return (lmLUnits)rValue;
        case lmMICRONS:         return (lmLUnits)((rValue / 100.) / lmSCALE);      break;
        case lmMILLIMETERS:     return (lmLUnits)((rValue * 10.) / lmSCALE);       break;
        case lmCENTIMETERS:     return (lmLUnits)((rValue * 100.) / lmSCALE);      break;
        case lmINCHES:          return (lmLUnits)((rValue * 254.) / lmSCALE);      break;
        default:
            wxASSERT(false);
            return 10.0f;
    }

}

double lmLogicalToUserUnits(int nValue, lmEUnits nUnits)
{
    return lmLogicalToUserUnits((double)nValue, nUnits);
}

double lmLogicalToUserUnits(double rValue, lmEUnits nUnits)
{
    // first multiply by SCALE factor, then convert from tenths of millimeter (mode MM_LOMETRIC)
    switch(nUnits) {
		case lmLUNITS:			return rValue;
        case lmMICRONS:         return ((lmSCALE * rValue) * 100.);      break;
        case lmMILLIMETERS:     return ((lmSCALE * rValue) / 10.);       break;
        case lmCENTIMETERS:     return ((lmSCALE * rValue) / 100.);      break;
        case lmINCHES:          return ((lmSCALE * rValue) / 254.);      break;
        default:
            wxASSERT(false);
            return 10.0;
    }

}
