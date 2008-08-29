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
#pragma implementation "ShapeText.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "ShapeText.h"
#include "../score/Glyph.h"      //access to glyphs table
#include "../score/Score.h"
#include "../app/ScoreCanvas.h"


//========================================================================================
// lmShapeTextOld object implementation
//========================================================================================

lmShapeTextOld::lmShapeTextOld(lmScoreObj* pOwner, wxString sText, wxFont* pFont, lmPaper* pPaper,
						 lmUPoint offset, wxString sName, bool fDraggable, wxColour color)
    : lmSimpleShape(eGMO_ShapeText, pOwner, 0, sName, fDraggable, lmSELECTABLE, color)
{
    m_sText = sText;
    m_pFont = pFont;

    // compute and store position
    m_uPos.x = offset.x;
    m_uPos.y = offset.y;

    // store boundling rectangle position and size
    lmLUnits uWidth, uHeight;
    pPaper->SetFont(*m_pFont);
    pPaper->GetTextExtent(m_sText, &uWidth, &uHeight);

    m_uBoundsTop.x = offset.x;
    m_uBoundsTop.y = offset.y;
    m_uBoundsBottom.x = m_uBoundsTop.x + uWidth;
    m_uBoundsBottom.y = m_uBoundsTop.y + uHeight;

    // store selection rectangle position and size
	m_uSelRect = GetBounds();

}

void lmShapeTextOld::Render(lmPaper* pPaper, wxColour color)
{
    pPaper->SetFont(*m_pFont);
    pPaper->SetTextForeground(color);
    pPaper->DrawText(m_sText, m_uPos.x, m_uPos.y);

    lmSimpleShape::Render(pPaper, color);
}

void lmShapeTextOld::SetFont(wxFont *pFont)
{
    m_pFont = pFont;
}

wxString lmShapeTextOld::Dump(int nIndent)
{
	wxString sDump = _T("");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
    sDump += wxString::Format(_T("TextShape: pos=(%.2f,%.2f), text=%s, "),
        m_uPos.x, m_uPos.y, m_sText.c_str() );
    sDump += DumpBounds();
    sDump += _T("\n");

	return sDump;
}

void lmShapeTextOld::Shift(lmLUnits xIncr, lmLUnits yIncr)
{
    m_uPos.x += xIncr;
    m_uPos.y += yIncr;

    ShiftBoundsAndSelRec(xIncr, yIncr);

	//if included in a composite shape update parent bounding and selection rectangles
	if (this->IsChildShape())
		((lmCompositeShape*)GetParentShape())->RecomputeBounds();
}

wxBitmap* lmShapeTextOld::OnBeginDrag(double rScale, wxDC* pDC)
{
	// A dragging operation is started. The view invokes this method to request the
	// bitmap to be used as drag image. No other action is required.
	// If no bitmap is returned drag is cancelled.
	//
	// So this method returns the bitmap to use with the drag image.


	// Get size of text, in logical units
    wxCoord wText, hText;
//    wxScreenDC dc;
//    dc.SetMapMode(lmDC_MODE);
//    dc.SetUserScale(rScale, rScale);
    pDC->SetFont(*m_pFont);
    pDC->GetTextExtent(m_sText, &wText, &hText);
//    dc.SetFont(wxNullFont);

    // allocate the bitmap
    // convert size to pixels
    int wD = (int)pDC->LogicalToDeviceXRel(wText);
    int hD = (int)pDC->LogicalToDeviceYRel(hText);
    wxBitmap bitmap(wD+2, hD+2);

    // allocate a memory DC for drawing into a bitmap
    wxMemoryDC dc2;
    dc2.SelectObject(bitmap);
    dc2.SetMapMode(lmDC_MODE);
    dc2.SetUserScale(rScale, rScale);
    dc2.SetFont(*m_pFont);

    // draw onto the bitmap
    dc2.SetBackground(* wxWHITE_BRUSH);
    dc2.Clear();
    dc2.SetBackgroundMode(wxTRANSPARENT);
    dc2.SetTextForeground(g_pColors->ScoreSelected());
    dc2.DrawText(m_sText, 0, 0);


    dc2.SelectObject(wxNullBitmap);

 //   //cut out the image, to discard the outside out of the bounding box
 //   lmPixels vxLeft = dc2.LogicalToDeviceYRel(GetXLeft() - m_uGlyphPos.x);
 //   lmPixels vyTop = dc2.LogicalToDeviceYRel(GetYTop() - m_uGlyphPos.y);
 //   lmPixels vWidth = wxMin(bitmap.GetWidth() - vxLeft,
 //                           dc2.LogicalToDeviceXRel(GetWidth()) );
 //   lmPixels vHeight = wxMin(bitmap.GetHeight() - vyTop,
 //                            dc2.LogicalToDeviceYRel(GetHeight()) );
 //   const wxRect rect(vxLeft, vyTop, vWidth, vHeight);
 //   //wxLogMessage(_T("[lmShapeGlyph::OnBeginDrag] bitmap size w=%d, h=%d. Cut x=%d, y=%d, w=%d, h=%d"),
 //   //    bitmap.GetWidth(), bitmap.GetHeight(), vxLeft, vyTop, vWidth, vHeight);
 //   wxBitmap oShapeBitmap = bitmap.GetSubBitmap(rect);
 //   wxASSERT(oShapeBitmap.IsOk());

    // Make the bitmap masked
    //wxImage image = oShapeBitmap.ConvertToImage();
    wxImage image = bitmap.ConvertToImage();
    image.SetMaskColour(255, 255, 255);
    wxBitmap* pBitmap = new wxBitmap(image);

 //   ////DBG -----------
 //   //wxString sFileName = _T("ShapeGlyp2.bmp");
 //   //pBitmap->SaveFile(sFileName, wxBITMAP_TYPE_BMP);
 //   ////END DBG -------

    return pBitmap;

}



//========================================================================================
// lmShapeText object implementation
//
// lmShapeText represents a block of text (one or several paragraphs).
//  - Normally adjusted to page width, but can be floating
//
//
//  (xLeft, yTop)
//     (*)-----------------------------------+
//      |                                    |
//      |                                    |
//      |                                    |
//      |                                    |
//      +-----------------------------------(*) (xRight, yBottom)
//
//  block alignment:
//      - Left, Right, Both: The x position is no fixed. It will be always assigned to
//          paper left/right margin.
//      - None: xLeft and xRight can be set by user
//
//  text h-alignment:
//      - Left, Right, Center, Justified
//
//  text v-alignment:
//      - Top, Bottom, middle
//
//
//
//
//========================================================================================


//Compatibility constructor
lmShapeText::lmShapeText(lmScoreObj* pOwner, const wxString& sText, wxFont* pFont,
                         lmPaper* pPaper, lmUPoint offset, wxString sName,
				         bool fDraggable, wxColour color)
    : lmShapeRectangle(pOwner, offset.x, offset.y, 0.0f, 0.0f, 0.0f, 
                       sName, fDraggable, true, color)
{
    Create(sText, pFont, pPaper, lmBLOCK_ALIGN_LEFT, lmHALIGN_LEFT, lmVALIGN_MIDDLE,
           offset.x, offset.y, 0.0f, 0.0f);
}

lmShapeText::lmShapeText(lmScoreObj* pOwner, const wxString& sText, wxFont* pFont,
                         lmPaper* pPaper, lmEBlockAlign nBlockAlign,
                         lmEHAlign nHAlign, lmEVAlign nVAlign,
                         lmLUnits xLeft, lmLUnits yTop,
                         lmLUnits xRight, lmLUnits yBottom,
                         wxColour nColor, wxString sName,
						 bool fDraggable)
    : lmShapeRectangle(pOwner, xLeft, yTop, xRight, yBottom, 0.0f,
                       sName, fDraggable, true, nColor)
{
    Create(sText, pFont, pPaper, nBlockAlign, nHAlign, nVAlign, xLeft, yTop, xRight,
           yBottom);
}

void lmShapeText::Create(const wxString& sText, wxFont* pFont, lmPaper* pPaper,
                         lmEBlockAlign nBlockAlign, lmEHAlign nHAlign, lmEVAlign nVAlign,
                         lmLUnits xLeft, lmLUnits yTop, lmLUnits xRight, lmLUnits yBottom)
{                         
    //The simplest constructor: just a text using a single font. No break lines

    m_sText = sText;
    m_pFont = pFont;
    m_nBlockAlign = nBlockAlign;
    m_nHAlign = nHAlign;
    m_nVAlign = nVAlign;

    //For now, behaviour wiil be that of a SimpleText object. Rectangle will be 
    //adjusted to text

    // compute and store position
    m_uTextPos.x = xLeft;
    m_uTextPos.y = yTop;

    // store boundling rectangle position and size
    lmLUnits uWidth, uHeight;
    pPaper->SetFont(*m_pFont);
    pPaper->GetTextExtent(m_sText, &uWidth, &uHeight);

    //compute rectangle bounds
    lmLUnits uxLeft, uyTop, uxRight, uyBottom;
    lmScore* pScore = m_pOwner->GetScore();
    switch (nBlockAlign)
    {
        case lmBLOCK_ALIGN_DEFAULT:
        case lmBLOCK_ALIGN_BOTH:
            {
                //xLeft and xRight on respective page margins
                uxLeft = pScore->GetLeftMarginXPos();
                uxRight = pScore->GetRightMarginXPos();
            }
            break;

        case lmBLOCK_ALIGN_LEFT:
            {
                //xLeft on left page margin
                uxLeft = pScore->GetLeftMarginXPos();
                uxRight = xRight;
            }
            break;

        case lmBLOCK_ALIGN_RIGHT:
            {
                //xRight on right page margin
                uxLeft = xLeft;
                uxRight = pScore->GetRightMarginXPos();
            }
            break;

        case lmBLOCK_ALIGN_NONE:
            {
                //Floating block: xLeft and xRight set by user
                uxLeft = xLeft;
                uxRight = xRight;
            }
            break;

        default:
            wxASSERT(false);
    }

    m_uBoundsTop.x = uxLeft;
    m_uBoundsTop.y = yTop;
    m_uBoundsBottom.x = uxRight;
    m_uBoundsBottom.y = m_uBoundsTop.y + uHeight;

    // store selection rectangle position and size
	m_uSelRect = GetBounds();


    //switch (m_nHAlign)
    //{
    //    case lmHALIGN_DEFAULT:
    //    case lmHALIGN_LEFT:
    //        {
    //        }
    //        break;

    //    case lmHALIGN_RIGHT:
    //        {
    //        }
    //        break;

    //    case lmHALIGN_JUSTIFY:
    //        {
    //        }
    //        break;

    //    case lmHALIGN_CENTER:
    //        {
    //        }
    //        break;

    //    default:
    //        wxASSERT(false);
    //}

}

void lmShapeText::Render(lmPaper* pPaper, wxColour color)
{
    pPaper->SetFont(*m_pFont);
    pPaper->SetTextForeground(color);
    pPaper->DrawText(m_sText, m_uTextPos.x, m_uTextPos.y);

    lmSimpleShape::Render(pPaper, color);
}

void lmShapeText::SetFont(wxFont *pFont)
{
    m_pFont = pFont;
}

wxString lmShapeText::Dump(int nIndent)
{
	wxString sDump = _T("");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
    sDump += wxString::Format(_T("TextBlock: pos=(%.2f,%.2f), text=%s, "),
        m_uTextPos.x, m_uTextPos.y, m_sText.c_str() );
    sDump += DumpBounds();
    sDump += _T("\n");

	return sDump;
}

void lmShapeText::Shift(lmLUnits xIncr, lmLUnits yIncr)
{
    m_uTextPos.x += xIncr;
    m_uTextPos.y += yIncr;

    ShiftBoundsAndSelRec(xIncr, yIncr);

	//if included in a composite shape update parent bounding and selection rectangles
	if (this->IsChildShape())
		((lmCompositeShape*)GetParentShape())->RecomputeBounds();
}

wxBitmap* lmShapeText::OnBeginDrag(double rScale, wxDC* pDC)
{
	// A dragging operation is started. The view invokes this method to request the
	// bitmap to be used as drag image. No other action is required.
	// If no bitmap is returned drag is cancelled.
	//
	// So this method returns the bitmap to use with the drag image.


	// Get size of text, in logical units
    wxCoord wText, hText;
//    wxScreenDC dc;
//    dc.SetMapMode(lmDC_MODE);
//    dc.SetUserScale(rScale, rScale);
    pDC->SetFont(*m_pFont);
    pDC->GetTextExtent(m_sText, &wText, &hText);
//    dc.SetFont(wxNullFont);

    // allocate the bitmap
    // convert size to pixels
    int wD = (int)pDC->LogicalToDeviceXRel(wText);
    int hD = (int)pDC->LogicalToDeviceYRel(hText);
    wxBitmap bitmap(wD+2, hD+2);

    // allocate a memory DC for drawing into a bitmap
    wxMemoryDC dc2;
    dc2.SelectObject(bitmap);
    dc2.SetMapMode(lmDC_MODE);
    dc2.SetUserScale(rScale, rScale);
    dc2.SetFont(*m_pFont);

    // draw onto the bitmap
    dc2.SetBackground(* wxWHITE_BRUSH);
    dc2.Clear();
    dc2.SetBackgroundMode(wxTRANSPARENT);
    dc2.SetTextForeground(g_pColors->ScoreSelected());
    dc2.DrawText(m_sText, 0, 0);


    dc2.SelectObject(wxNullBitmap);

 //   //cut out the image, to discard the outside out of the bounding box
 //   lmPixels vxLeft = dc2.LogicalToDeviceYRel(GetXLeft() - m_uGlyphPos.x);
 //   lmPixels vyTop = dc2.LogicalToDeviceYRel(GetYTop() - m_uGlyphPos.y);
 //   lmPixels vWidth = wxMin(bitmap.GetWidth() - vxLeft,
 //                           dc2.LogicalToDeviceXRel(GetWidth()) );
 //   lmPixels vHeight = wxMin(bitmap.GetHeight() - vyTop,
 //                            dc2.LogicalToDeviceYRel(GetHeight()) );
 //   const wxRect rect(vxLeft, vyTop, vWidth, vHeight);
 //   //wxLogMessage(_T("[lmShapeGlyph::OnBeginDrag] bitmap size w=%d, h=%d. Cut x=%d, y=%d, w=%d, h=%d"),
 //   //    bitmap.GetWidth(), bitmap.GetHeight(), vxLeft, vyTop, vWidth, vHeight);
 //   wxBitmap oShapeBitmap = bitmap.GetSubBitmap(rect);
 //   wxASSERT(oShapeBitmap.IsOk());

    // Make the bitmap masked
    //wxImage image = oShapeBitmap.ConvertToImage();
    wxImage image = bitmap.ConvertToImage();
    image.SetMaskColour(255, 255, 255);
    wxBitmap* pBitmap = new wxBitmap(image);

 //   ////DBG -----------
 //   //wxString sFileName = _T("ShapeGlyp2.bmp");
 //   //pBitmap->SaveFile(sFileName, wxBITMAP_TYPE_BMP);
 //   ////END DBG -------

    return pBitmap;

}

