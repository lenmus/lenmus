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
//---------------------------------------------------------------------------------
//    @brief A ruler around the score page
//
//    You can use a lmRuler like any other wxWindow.
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

#ifdef __GNUG__
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


//
// lmRuler
//

#define RULER_WIDTH  15        // ruler width in pixels

BEGIN_EVENT_TABLE(lmRuler, wxPanel)
    EVT_PAINT(lmRuler::OnPaint)
    //EVT_SIZE(lmRuler::OnSize)
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
    m_format = MetricFormat;
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
        m_height = RULER_WIDTH;
    } else {
        m_width = RULER_WIDTH;
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
        m_height = RULER_WIDTH;
    } else {
        m_width = RULER_WIDTH;
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



    //pDC->SetPen(*wxBLACK_PEN);
    //pDC->SetTextForeground(*wxBLACK);
//
//   if (m_Orientation == wxHORIZONTAL) {
//      if (mFlip)
//         pDC->DrawLine(m_left, m_top, m_right+1, m_top);
//      else
//         pDC->DrawLine(m_left, m_bottom, m_right+1, m_bottom);
//   }
//   else {
//      if (mFlip)
//         pDC->DrawLine(m_left, m_top, m_left, m_bottom+1);
//      else
//         pDC->DrawLine(m_right, m_top, m_right, m_bottom+1);
//   }
//
//   int i;
//
//   pDC->SetFont(*mMajorFont);
//
//   for(i=0; i<mNumMajor; i++) {
//      int pos = mMajorLabels[i].pos;
//
//      if (m_Orientation == wxHORIZONTAL) {
//         if (mFlip)
//            pDC->DrawLine(m_left + pos, m_top,
//                          m_left + pos, m_top + 4);
//         else
//            pDC->DrawLine(m_left + pos, m_bottom - 4,
//                          m_left + pos, m_bottom);
//      }
//      else {
//         if (mFlip)
//            pDC->DrawLine(m_left, m_top + pos,
//                          m_left + 4, m_top + pos);
//         else
//            pDC->DrawLine(m_right - 4, m_top + pos,
//                          m_right, m_top + pos);
//      }
//
//      if (mMajorLabels[i].text != "")
//         pDC->DrawText(mMajorLabels[i].text,
//                       mMajorLabels[i].lx,
//                       mMajorLabels[i].ly);
//   }
//
//   pDC->SetFont(*mMinorFont);
//
//   for(i=0; i<mNumMinor; i++) {
//      int pos = mMinorLabels[i].pos;
//
//      if (m_Orientation == wxHORIZONTAL) {
//         if (mFlip)
//            pDC->DrawLine(m_left + pos, m_top,
//                          m_left + pos, m_top + 2);
//         else
//            pDC->DrawLine(m_left + pos, m_bottom - 2,
//                          m_left + pos, m_bottom);
//      }
//      else {
//         if (mFlip)
//            pDC->DrawLine(m_left, m_top + pos,
//                          m_left + 2, m_top + pos);
//         else
//            pDC->DrawLine(m_right - 2, m_top + pos,
//                          m_right, m_top + pos);
//      }
//
//      if (mMinorLabels[i].text != "")
//         pDC->DrawText(mMinorLabels[i].text,
//                       mMinorLabels[i].lx,
//                       mMinorLabels[i].ly);
//   }



//lmRuler::lmRuler()
//{
//   mMin = 0.0;
//   mMax = 100.0;
//   m_Orientation = wxHORIZONTAL;
//   mSpacing = 6;
//   mHasSetSpacing = false;
//   mFormat = RealFormat;
//   mFlip = false;
//   mLog = false;
//   mLabelEdges = false;
//   mUnits = "";
//
//   m_left = -1;
//   m_top = -1;
//   m_right = -1;
//   m_bottom = -1;
//
//   int fontSize = 10;
//#ifdef __WXMSW__
//   fontSize = 8;
//#endif
//
//   mMinorFont = new wxFont(fontSize, wxSWISS, wxNORMAL, wxNORMAL);
//   mMajorFont = new wxFont(fontSize, wxSWISS, wxNORMAL, wxBOLD);
//
//   #ifdef __WXMAC__
//   mMinorFont->SetNoAntiAliasing(true);
//   mMajorFont->SetNoAntiAliasing(true);
//   #endif
//
//   mMajorLabels = 0;
//   mMinorLabels = 0;
//   mBits = NULL;
//   mUserBits = NULL;
//   mUserBitLen = 0;
//
//   mValid = false;
//}
//
//lmRuler::~lmRuler()
//{
//   Invalidate();  // frees up our arrays
//
//   delete mMinorFont;
//   delete mMajorFont;
//}
//
//void lmRuler::SetFormat(RulerFormat format)
//{
//   // IntFormat, RealFormat, TimeFormat, or LinearDBFormat
//
//   if (mFormat != format) {
//      mFormat = format;
//
//      Invalidate();
//   }
//}
//
//void lmRuler::SetLog(bool log)
//{
//   // Logarithmic
//
//   if (mLog != log) {
//      mLog = log;
//
//      Invalidate();
//   }
//}
//
//void lmRuler::SetUnits(wxString units)
//{
//   // Specify the name of the units (like "dB") if you
//   // want numbers like "1.6" formatted as "1.6 dB".
//
//   if (mUnits != units) {
//      mUnits = units;
//
//      Invalidate();
//   }
//}
//
//void lmRuler::SetOrientation(int orient)
//{
//   // wxHORIZONTAL || wxVERTICAL
//
//   if (m_Orientation != orient) {
//      m_Orientation = orient;
//
//      if (m_Orientation == wxVERTICAL && !mHasSetSpacing)
//         mSpacing = 2;
//
//      Invalidate();
//   }
//}
//
//void lmRuler::SetRange(double min, double max)
//{
//   // For a horizontal ruler,
//   // min is the value in the center of pixel "left",
//   // max is the value in the center of pixel "right".
//
//   if (mMin != min || mMax != max) {
//      mMin = min;
//      mMax = max;
//
//      Invalidate();
//   }
//}
//
//void lmRuler::SetSpacing(int spacing)
//{
//   mHasSetSpacing = true;
//
//   if (mSpacing != spacing) {
//      mSpacing = spacing;
//
//      Invalidate();
//   }
//}
//
//void lmRuler::SetLabelEdges(bool labelEdges)
//{
//   // If this is true, the edges of the ruler will always
//   // receive a label.  If not, the nearest round number is
//   // labeled (which may or may not be the edge).
//
//   if (mLabelEdges != labelEdges) {
//      mLabelEdges = labelEdges;
//
//      Invalidate();
//   }
//}
//
//void lmRuler::SetFlip(bool flip)
//{
//   // If this is true, the orientation of the tick marks
//   // is reversed from the default; eg. above the line
//   // instead of below
//
//   if (mFlip != flip) {
//      mFlip = flip;
//
//      Invalidate();
//   }
//}
//
//void lmRuler::SetFonts(const wxFont &minorFont, const wxFont &majorFont)
//{
//   *mMinorFont = minorFont;
//   *mMajorFont = majorFont;
//
//   #ifdef __WXMAC__
//   mMinorFont->SetNoAntiAliasing(true);
//   mMajorFont->SetNoAntiAliasing(true);
//   #endif
//
//   Invalidate();
//}
//
//void lmRuler::OfflimitsPixels(int start, int end)
//{
//   int i;
//
//   if (!mUserBits) {
//      if (m_Orientation == wxHORIZONTAL)
//         mLength = m_right-m_left;
//      else
//         mLength = m_bottom-m_top;
//      mUserBits = new int[mLength+1];
//      for(i=0; i<=mLength; i++)
//         mUserBits[i] = 0;
//      mUserBitLen  = mLength+1;
//   }
//
//   if (end < start) {
//      i = end;
//      end = start;
//      start = i;
//   }
//
//   if (start < 0)
//      start = 0;
//   if (end > mLength)
//      end = mLength;
//
//   for(i=start; i<=end; i++)
//      mUserBits[i] = 1;
//}
//
//void lmRuler::SetBounds(int left, int top, int right, int bottom)
//{
//   if (m_left != left || m_top != top ||
//       m_right != right || m_bottom != bottom) {
//      m_left = left;
//      m_top = top;
//      m_right = right;
//      m_bottom = bottom;
//
//      Invalidate();
//   }
//}
//
//void lmRuler::Invalidate()
//{
//   mValid = false;
//
//   if (m_Orientation == wxHORIZONTAL)
//      mLength = m_right-m_left;
//   else
//      mLength = m_bottom-m_top;
//
//   if (mMajorLabels) {
//      delete [] mMajorLabels;
//      mMajorLabels = NULL;
//   }
//   if (mMinorLabels) {
//      delete [] mMinorLabels;
//      mMinorLabels = NULL;
//   }
//   if (mBits) {
//      delete [] mBits;
//      mBits = NULL;
//   }
//   if (mUserBits && mLength+1 != mUserBitLen) {
//      delete[] mUserBits;
//      mUserBits = NULL;
//      mUserBitLen = 0;
//   }
//}
//
//void lmRuler::FindLinearTickSizes(double UPP)
//{
//   // Given the dimensions of the ruler, the range of values it
//   // has to display, and the format (i.e. Int, Real, Time),
//   // figure out how many units are in one Minor tick, and
//   // in one Major tick.
//   //
//   // The goal is to always put tick marks on nice round numbers
//   // that are easy for humans to grok.  This is the most tricky
//   // with time.
//
//   double d;
//
//   // As a heuristic, we want at least 16 pixels
//   // between each minor tick
//   double units = 16 * fabs(UPP);
//
//   mDigits = 0;
//
//   switch(mFormat) {
//   case LinearDBFormat:
//      if (units < 0.1) {
//         mMinor = 0.1;
//         mMajor = 0.5;
//         return;
//      }
//      if (units < 1.0) {
//         mMinor = 1.0;
//         mMajor = 6.0;
//         return;
//      }
//      mMinor = 3.0;
//      mMajor = 12.0;
//      return;
//
//   case IntFormat:
//      d = 1.0;
//      for(;;) {
//         if (units < d) {
//            mMinor = d;
//            mMajor = d*5.0;
//            return;
//         }
//         d *= 5.0;
//         if (units < d) {
//            mMinor = d;
//            mMajor = d*2.0;
//            return;
//         }
//         d *= 2.0;
//      }
//      break;
//
//   case TimeFormat:
//      if (units > 0.5) {
//         if (units < 1.0) { // 1 sec
//            mMinor = 1.0;
//            mMajor = 5.0;
//            return;
//         }
//         if (units < 5.0) { // 5 sec
//            mMinor = 5.0;
//            mMajor = 15.0;
//            return;
//         }
//         if (units < 10.0) {
//            mMinor = 10.0;
//            mMajor = 30.0;
//            return;
//         }
//         if (units < 15.0) {
//            mMinor = 15.0;
//            mMajor = 60.0;
//            return;
//         }
//         if (units < 30.0) {
//            mMinor = 30.0;
//            mMajor = 60.0;
//            return;
//         }
//         if (units < 60.0) { // 1 min
//            mMinor = 60.0;
//            mMajor = 300.0;
//            return;
//         }
//         if (units < 300.0) { // 5 min
//            mMinor = 300.0;
//            mMajor = 900.0;
//            return;
//         }
//         if (units < 600.0) { // 10 min
//            mMinor = 600.0;
//            mMajor = 1800.0;
//            return;
//         }
//         if (units < 900.0) { // 15 min
//            mMinor = 900.0;
//            mMajor = 3600.0;
//            return;
//         }
//         if (units < 1800.0) { // 30 min
//            mMinor = 1800.0;
//            mMajor = 3600.0;
//            return;
//         }
//         if (units < 3600.0) { // 1 hr
//            mMinor = 3600.0;
//            mMajor = 6*3600.0;
//            return;
//         }
//         if (units < 6*3600.0) { // 6 hrs
//            mMinor = 6*3600.0;
//            mMajor = 24*3600.0;
//            return;
//         }
//         if (units < 24*3600.0) { // 1 day
//            mMinor = 24*3600.0;
//            mMajor = 7*24*3600.0;
//            return;
//         }
//
//         mMinor = 24.0 * 7.0 * 3600.0; // 1 week
//         mMajor = 24.0 * 7.0 * 3600.0;
//      }
//
//      // Otherwise fall through to RealFormat
//      // (fractions of a second should be dealt with
//      // the same way as for RealFormat)
//
//   case RealFormat:
//      d = 0.000001;
//      mDigits = 6;
//      for(;;) {
//         if (units < d) {
//            mMinor = d;
//            mMajor = d*5.0;
//            return;
//         }
//         d *= 5.0;
//         if (units < d) {
//            mMinor = d;
//            mMajor = d*2.0;
//            return;
//         }
//         d *= 2.0;
//         mDigits--;
//      }
//      break;
//
//   }
//
//}
//
//wxString lmRuler::LabelString(double d, bool major)
//{
//   // Given a value, turn it into a string according
//   // to the current ruler format.  The number of digits of
//   // accuracy depends on the resolution of the ruler,
//   // i.e. how far zoomed in or out you are.
//
//   wxString s;
//
//   // Replace -0 with 0
//   if (d < 0.0 && d+mMinor > 0.0)
//      d = 0.0;
//
//   switch(mFormat) {
//   case IntFormat:
//      s.Printf("%d", (int)floor(d+0.5));
//      break;
//   case LinearDBFormat:
//      if (mMinor >= 1.0)
//         s.Printf("%d", (int)floor(d+0.5));
//      else {
//         s.Printf("%.1f", d);
//      }
//      break;
//   case RealFormat:
//      if (mMinor >= 1.0)
//         s.Printf("%d", (int)floor(d+0.5));
//      else {
//         s.Printf(wxString::Format("%%.%df", mDigits), d);
//      }
//      break;
//   case TimeFormat:
//      if (major) {
//         if (d < 0) {
//            s = "-";
//            d = -d;
//         }
//
//         #if ALWAYS_HH_MM_SS
//         int secs = (int)(d + 0.5);
//         if (mMinor >= 1.0) {
//            s.Printf("%d:%02d:%02d", secs/3600, (secs/60)%60, secs%60);
//         }
//         else {
//            wxString t1, t2, format;
//            t1.Printf("%d:%02d:", secs/3600, (secs/60)%60);
//            format.Printf("%%0%d.%dlf", mDigits+3, mDigits);
//            t2.Printf((const char *)format, fmod(d, 60.0));
//            s += t1 + t2;
//         }
//         break;
//         #endif
//
//         if (mMinor >= 3600.0) {
//            int hrs = (int)(d / 3600.0 + 0.5);
//            wxString h;
//            h.Printf("%d:00:00", hrs);
//            s += h;
//         }
//         else if (mMinor >= 60.0) {
//            int minutes = (int)(d / 60.0 + 0.5);
//            wxString m;
//            if (minutes >= 60)
//               m.Printf("%d:%02d:00", minutes/60, minutes%60);
//            else
//               m.Printf("%d:00", minutes);
//            s += m;
//         }
//         else if (mMinor >= 1.0) {
//            int secs = (int)(d + 0.5);
//            wxString t;
//            if (secs >= 3600)
//               t.Printf("%d:%02d:%02d", secs/3600, (secs/60)%60, secs%60);
//            else if (secs >= 60)
//               t.Printf("%d:%02d", secs/60, secs%60);
//            else
//               t.Printf("%d", secs);
//            s += t;
//         }
//         else {
//            int secs = (int)(d);
//            wxString t1, t2, format;
//
//            if (secs >= 3600)
//               t1.Printf("%d:%02d:", secs/3600, (secs/60)%60);
//            else if (secs >= 60)
//               t1.Printf("%d:", secs/60);
//
//            if (secs >= 60)
//               format.Printf("%%0%d.%dlf", mDigits+3, mDigits);
//            else
//               format.Printf("%%%d.%dlf", mDigits+3, mDigits);
//            t2.Printf((const char *)format, fmod(d, 60.0));
//
//            s += t1 + t2;
//         }
//      }
//      else {
//      }
//   }
//
//   if (mUnits != "")
//      s = (s + " " + mUnits);
//
//   return s;
//}
//
//void lmRuler::Tick(int pos, double d, bool major)
//{
//   wxString l;
//   wxCoord strW, strH;
//   int strPos, strLen, strLeft, strTop;
//
//   Label *label;
//   if (major)
//      label = &mMajorLabels[mNumMajor++];
//   else
//      label = &mMinorLabels[mNumMinor++];
//
//   label->pos = pos;
//   label->lx = m_left - 1000; // don't display
//   label->ly = m_top - 1000;  // don't display
//   label->text = "";
//
//   mDC->SetFont(major? *mMajorFont: *mMinorFont);
//
//   l = LabelString(d, major);
//   mDC->GetTextExtent(l, &strW, &strH);
//
//   if (m_Orientation == wxHORIZONTAL) {
//      strLen = strW;
//      strPos = pos - strW/2;
//      if (strPos < 0)
//         strPos = 0;
//      if (strPos + strW >= mLength)
//         strPos = mLength - strW;
//      strLeft = m_left + strPos;
//      if (mFlip)
//         strTop = m_top + 4;
//      else
//         strTop = m_bottom - strH - 6;
//   }
//   else {
//      strLen = strH;
//      strPos = pos - strH/2;
//      if (strPos < 0)
//         strPos = 0;
//      if (strPos + strH >= mLength)
//         strPos = mLength - strH;
//      strTop = m_top + strPos;
//      if (mFlip)
//         strLeft = m_left + 5;
//      else
//         strLeft = m_right - strW - 6;
//   }
//
//   // See if any of the pixels we need to draw this
//   // label is already covered
//
//   int i;
//   for(i=0; i<strLen; i++)
//      if (mBits[strPos+i])
//         return;
//
//   // If not, position the label and give it text
//
//   label->lx = strLeft;
//   label->ly = strTop;
//   label->text = l;
//
//   // And mark these pixels, plus some surrounding
//   // ones (the spacing between labels), as covered
//
//   int leftMargin = mSpacing;
//   if (strPos < leftMargin)
//      leftMargin = strPos;
//   strPos -= leftMargin;
//   strLen += leftMargin;
//
//   int rightMargin = mSpacing;
//   if (strPos + strLen > mLength - mSpacing)
//      rightMargin = mLength - strPos - strLen;
//   strLen += rightMargin;
//
//   for(i=0; i<strLen; i++)
//      mBits[strPos+i] = 1;
//}
//
//void lmRuler::Update()
//{
//  Update(NULL, 0, 0);
//}
//
//void lmRuler::Update( Envelope *speedEnv, long minSpeed, long maxSpeed )
//{
//   // This gets called when something has been changed
//   // (i.e. we've been invalidated).  Recompute all
//   // tick positions.
//
//   int i;
//   int j;
//
//   mNumMajor = 0;
//   mMajorLabels = new Label[mLength+1];
//   mNumMinor = 0;
//   mMinorLabels = new Label[mLength+1];
//
//   if (mBits)
//      delete[] mBits;
//   mBits = new int[mLength+1];
//   if (mUserBits)
//      for(i=0; i<=mLength; i++)
//         mBits[i] = mUserBits[i];
//   else
//      for(i=0; i<=mLength; i++)
//         mBits[i] = 0;
//
//   if(mLog==false) {
//
//      double UPP = (mMax-mMin)/mLength;  // Units per pixel
//
//      FindLinearTickSizes(UPP);
//
//      // Left and Right Edges
//      if (mLabelEdges) {
//         Tick(0, mMin, true);
//         Tick(mLength, mMax, true);
//      }
//
//      // Zero (if it's in the middle somewhere)
//      if (mMin * mMax < 0.0) {
//         int mid = (int)(mLength*(mMin/(mMin-mMax)) + 0.5);
//         Tick(mid, 0.0, true);
//      }
//
//      double sg = UPP > 0.0? 1.0: -1.0;
//
//      // Major ticks
//      double d = mMin - UPP/2;
//      double lastD = d;
//      int majorInt = (int)floor(sg * d / mMajor);
//      i = -1;
//      while(i <= mLength) {
//         double warpfactor;
//         if( d>0 && speedEnv != NULL ) {
//            warpfactor = speedEnv->Average( lastD, d );
//            // Now we re-scale so that 0.5 is normal speed and
//            // 0 and 1.0 are min% and max% of normal speed
//            warpfactor = (maxSpeed * (1 - warpfactor) +
//                          warpfactor * minSpeed) / 100.0;
//         }
//         else
//            warpfactor = 1.0;
//
//         i++;
//         lastD = d;
//         d += UPP*warpfactor;
//
//         if ((int)floor(sg * d / mMajor) > majorInt) {
//            majorInt = (int)floor(sg * d / mMajor);
//            Tick(i, sg * majorInt * mMajor, true);
//         }
//      }
//
//      // Minor ticks
//      d = mMin - UPP/2;
//      lastD = d;
//      int minorInt = (int)floor(sg * d / mMinor);
//      i = -1;
//      while(i <= mLength) {
//         double warpfactor;
//         if( d>0 && speedEnv != NULL ) {
//            warpfactor = speedEnv->Average( lastD, d );
//            // Now we re-scale so that 0.5 is normal speed and
//            // 0 and 1.0 are min% and max% of normal speed
//            warpfactor = (maxSpeed * (1 - warpfactor) +
//                          warpfactor * minSpeed) / 100.0;
//         }
//         else
//            warpfactor = 1.0;
//
//         i++;
//         lastD = d;
//         d += UPP*warpfactor;
//
//         if ((int)floor(sg * d / mMinor) > minorInt) {
//            minorInt = (int)floor(sg * d / mMinor);
//            Tick(i, sg * minorInt * mMinor, false);
//         }
//      }
//
//      // Left and Right Edges
//      if (mLabelEdges) {
//         Tick(0, mMin, true);
//         Tick(mLength, mMax, true);
//      }
//
//   }
//   else {
//      // log case
//
//      double loLog = log10(mMin);
//      double hiLog = log10(mMax);
//      double scale = mLength/(hiLog - loLog);
//      int loDecade = (int) floor(loLog);
//      int hiDecade = (int) ceil(hiLog);
//
//      int pos;
//      double val;
//      double startDecade = pow(10., (double)loDecade);
//
//      // Major ticks are the decades
//      double decade = startDecade;
//      for(i=loDecade; i<hiDecade; i++) {
//         if(i!=loDecade) {
//            val = decade;
//            if(val > mMin && val < mMax) {
//               pos = (int)(((log10(val) - loLog)*scale)+0.5);
//               Tick(pos, val, true);
//            }
//         }
//         decade *= 10.;
//      }
//
//      // Minor ticks are multiples of decades
//      decade = startDecade;
//      for(i=loDecade; i<hiDecade; i++) {
//         for(j=2; j<=9; j++) {
//            val = decade * j;
//            if(val > mMin && val < mMax) {
//               pos = (int)(((log10(val) - loLog)*scale)+0.5);
//               Tick(pos, val, false);
//            }
//         }
//         decade *= 10.;
//      }
//   }
//
//   mValid = true;
//}
//
//void lmRuler::Draw(wxDC& dc)
//{
//   Draw( dc, NULL, 0, 0 );
//}
//
//void lmRuler::Draw(wxDC& dc, Envelope *speedEnv, long minSpeed, long maxSpeed)
//{
//   mDC = &dc;
//
//   if (!mValid)
//      Update( speedEnv, minSpeed, maxSpeed );
//
//   mDC->SetPen(*wxBLACK_PEN);
//   mDC->SetTextForeground(*wxBLACK);
//
//   if (m_Orientation == wxHORIZONTAL) {
//      if (mFlip)
//         mDC->DrawLine(m_left, m_top, m_right+1, m_top);
//      else
//         mDC->DrawLine(m_left, m_bottom, m_right+1, m_bottom);
//   }
//   else {
//      if (mFlip)
//         mDC->DrawLine(m_left, m_top, m_left, m_bottom+1);
//      else
//         mDC->DrawLine(m_right, m_top, m_right, m_bottom+1);
//   }
//
//   int i;
//
//   mDC->SetFont(*mMajorFont);
//
//   for(i=0; i<mNumMajor; i++) {
//      int pos = mMajorLabels[i].pos;
//
//      if (m_Orientation == wxHORIZONTAL) {
//         if (mFlip)
//            mDC->DrawLine(m_left + pos, m_top,
//                          m_left + pos, m_top + 4);
//         else
//            mDC->DrawLine(m_left + pos, m_bottom - 4,
//                          m_left + pos, m_bottom);
//      }
//      else {
//         if (mFlip)
//            mDC->DrawLine(m_left, m_top + pos,
//                          m_left + 4, m_top + pos);
//         else
//            mDC->DrawLine(m_right - 4, m_top + pos,
//                          m_right, m_top + pos);
//      }
//
//      if (mMajorLabels[i].text != "")
//         mDC->DrawText(mMajorLabels[i].text,
//                       mMajorLabels[i].lx,
//                       mMajorLabels[i].ly);
//   }
//
//   mDC->SetFont(*mMinorFont);
//
//   for(i=0; i<mNumMinor; i++) {
//      int pos = mMinorLabels[i].pos;
//
//      if (m_Orientation == wxHORIZONTAL) {
//         if (mFlip)
//            mDC->DrawLine(m_left + pos, m_top,
//                          m_left + pos, m_top + 2);
//         else
//            mDC->DrawLine(m_left + pos, m_bottom - 2,
//                          m_left + pos, m_bottom);
//      }
//      else {
//         if (mFlip)
//            mDC->DrawLine(m_left, m_top + pos,
//                          m_left + 2, m_top + pos);
//         else
//            mDC->DrawLine(m_right - 2, m_top + pos,
//                          m_right, m_top + pos);
//      }
//
//      if (mMinorLabels[i].text != "")
//         mDC->DrawText(mMinorLabels[i].text,
//                       mMinorLabels[i].lx,
//                       mMinorLabels[i].ly);
//   }
//}




