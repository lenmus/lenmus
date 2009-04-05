//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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
// lmShapeText object implementation
//========================================================================================

lmShapeText::lmShapeText(lmScoreObj* pOwner, wxString sText, wxFont* pFont, lmPaper* pPaper,
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

void lmShapeText::Render(lmPaper* pPaper, wxColour color)
{
    pPaper->SetFont(*m_pFont);
    pPaper->SetTextForeground(color);
    pPaper->DrawText(m_sText, m_uPos.x, m_uPos.y);

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
    sDump += wxString::Format(_T("TextShape: pos=(%.2f,%.2f), text=%s, "),
        m_uPos.x, m_uPos.y, m_sText.c_str() );
    sDump += DumpBounds();
    sDump += _T("\n");

	return sDump;
}

void lmShapeText::Shift(lmLUnits xIncr, lmLUnits yIncr)
{
    m_uPos.x += xIncr;
    m_uPos.y += yIncr;

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



//========================================================================================
// lmShapeTitle object implementation
//========================================================================================


////Simple text constructor
//lmShapeTitle::lmShapeTitle(lmScoreObj* pOwner, const wxString& sText, wxFont* pFont,
//                         lmPaper* pPaper, lmUPoint offset, wxString sName,
//				         bool fDraggable, wxColour color)
//    : lmShapeRectangle(pOwner, offset.x, offset.y, 0.0f, 0.0f, 0.0f, color, 
//                       sName, fDraggable, true)
//{
//    //measure text size
//    lmLUnits uWidth, uHeight;
//    pPaper->SetFont(*pFont);
//    pPaper->GetTextExtent(sText, &uWidth, &uHeight);
//
//    Create(sText, pFont, pPaper, lmBLOCK_ALIGN_LEFT, lmHALIGN_LEFT, lmVALIGN_MIDDLE,
//           offset.x, offset.y, offset.x + uWidth, offset.y + uHeight);
//}

//block text constructor
lmShapeTitle::lmShapeTitle(lmScoreObj* pOwner, const wxString& sText, wxFont* pFont,
                         lmPaper* pPaper, lmEBlockAlign nBlockAlign,
                         lmEHAlign nHAlign, lmEVAlign nVAlign,
                         lmLUnits xLeft, lmLUnits yTop,
                         lmLUnits xRight, lmLUnits yBottom,
                         wxColour nColor, wxString sName,
						 bool fDraggable)
    : lmShapeRectangle(pOwner, xLeft, yTop, xRight, yBottom, 0.0f, nColor,
                       sName, fDraggable, true)
{
    Create(sText, pFont, pPaper, nBlockAlign, nHAlign, nVAlign, xLeft, yTop, xRight,
           yBottom);
}

void lmShapeTitle::Create(const wxString& sText, wxFont* pFont, lmPaper* pPaper,
                         lmEBlockAlign nBlockAlign, lmEHAlign nHAlign, lmEVAlign nVAlign,
                         lmLUnits xLeft, lmLUnits yTop, lmLUnits xRight, lmLUnits yBottom)
{                         
    //The simplest constructor: just a text using a single font. No break lines

	m_nType = eGMO_ShapeTextBlock;

    m_sText = sText;
    m_pFont = pFont;
    m_nBlockAlign = nBlockAlign;
    m_nHAlign = nHAlign;
    m_nVAlign = nVAlign;

    //For now, behaviour wiil be that of a SimpleText object. Rectangle will be 
    //adjusted to text

    // store boundling rectangle position and size
    pPaper->SetFont(*m_pFont);
    pPaper->GetTextExtent(m_sText, &m_uTextWidth, &m_uTextHeight);

    //compute rectangle bounds
    ComputeBlockBounds(xLeft, yTop, xRight, yBottom);

    //Position the text within the box
    ComputeTextPosition(pPaper);

}

void lmShapeTitle::ComputeBlockBounds(lmLUnits xLeft, lmLUnits yTop, lmLUnits xRight, lmLUnits yBottom)
{
    //compute rectangle bounds
    lmLUnits uxLeft, uxRight;
    lmScore* pScore = m_pOwner->GetScore();
    switch (m_nBlockAlign)
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
    m_uBoundsBottom.y = m_uBoundsTop.y + m_uTextHeight;

    // store selection rectangle position and size
	m_uSelRect = GetBounds();
}

void lmShapeTitle::ComputeTextPosition(lmPaper* pPaper)
{
    //Position the text within the box
    lmLUnits uxLeft, uyTop;
    lmLUnits uBoxAreaWidth = m_uBoundsBottom.x - m_uBoundsTop.x;

	//clip text if longer than available space
	m_sClippedText = m_sText;
    m_uClippedTextWidth = m_uTextWidth;
    m_uClippedTextHeight = m_uTextHeight;
	if (uBoxAreaWidth < m_uTextWidth)
	{
		//we have to cut the text. Loop to add chars until line full
		pPaper->SetFont(*m_pFont);
		lmLUnits uWidth, uHeight;
		m_sClippedText = _T("");
		int iC = 0;
		lmLUnits uAvailable = uBoxAreaWidth;
		while(iC < (int)m_sText.Length())
		{
			const wxString ch = m_sText.Mid(iC, 1);
			pPaper->GetTextExtent(ch, &uWidth, &uHeight);
			if (uAvailable < uWidth)
				break;

			//add char to clipped text
			uAvailable -= uWidth;
			m_sClippedText += ch;
			iC++;
		}
		pPaper->GetTextExtent(m_sClippedText, &m_uClippedTextWidth, &m_uClippedTextHeight);
	}


    switch (m_nHAlign)
    {
        case lmHALIGN_DEFAULT:
        case lmHALIGN_LEFT:
            {
                uxLeft = m_uBoundsTop.x;
                uyTop = m_uBoundsTop.y;
            }
            break;

        case lmHALIGN_RIGHT:
            {
                uxLeft = m_uBoundsBottom.x - m_uClippedTextWidth;
                uyTop = m_uBoundsTop.y;
            }
            break;

        case lmHALIGN_JUSTIFY:
            {
                //TODO
                uxLeft = m_uBoundsTop.x;
                uyTop = m_uBoundsTop.y;
            }
            break;

        case lmHALIGN_CENTER:
            {
                uxLeft = m_uBoundsTop.x + (uBoxAreaWidth - m_uClippedTextWidth) / 2.0f;
                uyTop = m_uBoundsTop.y;
            }
            break;

        default:
            wxASSERT(false);
    }
    m_uTextPos.x = uxLeft;
    m_uTextPos.y = uyTop;

    // store selection rectangle position and size
	m_uSelRect.SetLeftTop(m_uTextPos);
    m_uSelRect.SetWidth(m_uClippedTextWidth);
    m_uSelRect.SetHeight(m_uClippedTextHeight);

}

void lmShapeTitle::Render(lmPaper* pPaper, wxColour color)
{
    //ensure measures are ok
    ComputeTextPosition(pPaper);

    pPaper->SetFont(*m_pFont);
    pPaper->SetTextForeground(color);
    pPaper->DrawText(m_sClippedText, m_uTextPos.x, m_uTextPos.y);

    lmSimpleShape::Render(pPaper, color);
}

void lmShapeTitle::SetFont(wxFont *pFont)
{
    m_pFont = pFont;
}

wxString lmShapeTitle::Dump(int nIndent)
{
	wxString sDump = _T("");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
    sDump += wxString::Format(_T("TextBlock: pos=(%.2f,%.2f), text=%s, "),
        m_uTextPos.x, m_uTextPos.y, m_sText.c_str() );
    sDump += DumpBounds();
    sDump += _T("\n");

	return sDump;
}

void lmShapeTitle::Shift(lmLUnits xIncr, lmLUnits yIncr)
{
    m_uTextPos.x += xIncr;
    m_uTextPos.y += yIncr;

    ShiftBoundsAndSelRec(xIncr, yIncr);

	//if included in a composite shape update parent bounding and selection rectangles
	if (this->IsChildShape())
		((lmCompositeShape*)GetParentShape())->RecomputeBounds();
}

wxBitmap* lmShapeTitle::OnBeginDrag(double rScale, wxDC* pDC)
{
	// A dragging operation is started. The view invokes this method to request the
	// bitmap to be used as drag image. No other action is required.
	// If no bitmap is returned drag is cancelled.
	//
	// So this method returns the bitmap to use with the drag image.


    // allocate a bitmap whose size is that of the box area
    // convert size to pixels
    int wD = (int)pDC->LogicalToDeviceXRel( m_uBoundsBottom.x - m_uBoundsTop.x );
    int hD = (int)pDC->LogicalToDeviceYRel( m_uBoundsBottom.y - m_uBoundsTop.y );
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
    dc2.DrawText(m_sClippedText, m_uTextPos.x - m_uBoundsTop.x, m_uTextPos.y - m_uBoundsTop.y);
    dc2.SelectObject(wxNullBitmap);

    // Make the bitmap masked
    wxImage image = bitmap.ConvertToImage();
    image.SetMaskColour(255, 255, 255);
    wxBitmap* pBitmap = new wxBitmap(image);

    ////DBG -----------
    //wxString sFileName = _T("lmShapeTitle.bmp");
    //pBitmap->SaveFile(sFileName, wxBITMAP_TYPE_BMP);
    ////END DBG -------

    return pBitmap;

}

lmUPoint lmShapeTitle::OnDrag(lmPaper* pPaper, const lmUPoint& uPos)
{
	// The view informs that the user continues dragging. We receive the new desired
	// shape position and we must return the new allowed shape position.
	//
	// The default behaviour is to return the received position, so the view redraws 
	// the drag image at that position. No action must be performed by the shape on 
	// the score and score objects.
	//
	// The received new desired shape position is in logical units and referred to page
	// origin. The returned new allowed shape position must also be in in logical units
	// and referred to page origin.


    // A TextBlock can only be vertically moved, unless it is a floating block
    if (m_nBlockAlign == lmBLOCK_ALIGN_NONE)
    {
        //floating block
        //TODO
        return lmUPoint(uPos.x, uPos.y);
    }
    else
    {
        //move only in vertical
        return lmUPoint(m_uBoundsTop.x, uPos.y);
    }
}

void lmShapeTitle::OnEndDrag(lmPaper* pPaper, lmController* pCanvas, const lmUPoint& uPos)
{
	// End drag. Receives the command processor associated to the view and the
	// final position of the object (logical units referred to page origin).
	// This method must validate/adjust final position and, if ok, it must 
	// send a move object command to the controller.

	lmUPoint uFinalPos(uPos.x, uPos.y);
    if (m_nBlockAlign == lmBLOCK_ALIGN_NONE)
    {
        //floating block
        //TODO
    }
    else
    {
        //move only in vertical
        uFinalPos.x = m_uBoundsTop.x;
    }

	//send a move object command to the controller
	pCanvas->MoveObject(this, uFinalPos);
}



//========================================================================================
// lmShapeTextbox object implementation
//========================================================================================

lmShapeTextbox::lmShapeTextbox(lmScoreObj* pOwner, const wxString& sText, wxFont* pFont,
                         lmPaper* pPaper, lmEBlockAlign nBlockAlign,
                         lmEHAlign nHAlign, lmEVAlign nVAlign,
                         lmLUnits xLeft, lmLUnits yTop,
                         lmLUnits xRight, lmLUnits yBottom,
                         wxColour nColor, wxString sName,
						 bool fDraggable)
    : lmShapeRectangle(pOwner, xLeft, yTop, xRight, yBottom, 0.0f, nColor,
                       sName, fDraggable, true)
{
    Create(sText, pFont, pPaper, nBlockAlign, nHAlign, nVAlign, xLeft, yTop, xRight,
           yBottom);
}

void lmShapeTextbox::Create(const wxString& sText, wxFont* pFont, lmPaper* pPaper,
                         lmEBlockAlign nBlockAlign, lmEHAlign nHAlign, lmEVAlign nVAlign,
                         lmLUnits xLeft, lmLUnits yTop, lmLUnits xRight, lmLUnits yBottom)
{                         
    //The simplest constructor: just a text using a single font. No break lines

	m_nType = eGMO_ShapeTextBlock;

    m_sText = sText;
    m_pFont = pFont;
    m_nBlockAlign = nBlockAlign;
    m_nHAlign = nHAlign;
    m_nVAlign = nVAlign;

    //For now, behaviour wiil be that of a SimpleText object. Rectangle will be 
    //adjusted to text

    // store boundling rectangle position and size
    pPaper->SetFont(*m_pFont);
    pPaper->GetTextExtent(m_sText, &m_uTextWidth, &m_uTextHeight);

    //compute rectangle bounds
    ComputeBlockBounds(xLeft, yTop, xRight, yBottom);

    //Position the text within the box
    ComputeTextPosition(pPaper);

}

void lmShapeTextbox::ComputeBlockBounds(lmLUnits xLeft, lmLUnits yTop, lmLUnits xRight, lmLUnits yBottom)
{
    //compute rectangle bounds
    lmLUnits uxLeft, uxRight;
    lmScore* pScore = m_pOwner->GetScore();
    switch (m_nBlockAlign)
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
    m_uBoundsBottom.y = m_uBoundsTop.y + m_uTextHeight;

    // store selection rectangle position and size
	m_uSelRect = GetBounds();
}

void lmShapeTextbox::ComputeTextPosition(lmPaper* pPaper)
{
    //Position the text within the box
    lmLUnits uxLeft, uyTop;
    lmLUnits uBoxAreaWidth = m_uBoundsBottom.x - m_uBoundsTop.x;

	//clip text if longer than available space
	m_sClippedText = m_sText;
    m_uClippedTextWidth = m_uTextWidth;
    m_uClippedTextHeight = m_uTextHeight;
	if (uBoxAreaWidth < m_uTextWidth)
	{
		//we have to cut the text. Loop to add chars until line full
		pPaper->SetFont(*m_pFont);
		lmLUnits uWidth, uHeight;
		m_sClippedText = _T("");
		int iC = 0;
		lmLUnits uAvailable = uBoxAreaWidth;
		while(iC < (int)m_sText.Length())
		{
			const wxString ch = m_sText.Mid(iC, 1);
			pPaper->GetTextExtent(ch, &uWidth, &uHeight);
			if (uAvailable < uWidth)
				break;

			//add char to clipped text
			uAvailable -= uWidth;
			m_sClippedText += ch;
			iC++;
		}
		pPaper->GetTextExtent(m_sClippedText, &m_uClippedTextWidth, &m_uClippedTextHeight);
	}


    switch (m_nHAlign)
    {
        case lmHALIGN_DEFAULT:
        case lmHALIGN_LEFT:
            {
                uxLeft = m_uBoundsTop.x;
                uyTop = m_uBoundsTop.y;
            }
            break;

        case lmHALIGN_RIGHT:
            {
                uxLeft = m_uBoundsBottom.x - m_uClippedTextWidth;
                uyTop = m_uBoundsTop.y;
            }
            break;

        case lmHALIGN_JUSTIFY:
            {
                //TODO
                uxLeft = m_uBoundsTop.x;
                uyTop = m_uBoundsTop.y;
            }
            break;

        case lmHALIGN_CENTER:
            {
                uxLeft = m_uBoundsTop.x + (uBoxAreaWidth - m_uClippedTextWidth) / 2.0f;
                uyTop = m_uBoundsTop.y;
            }
            break;

        default:
            wxASSERT(false);
    }
    m_uTextPos.x = uxLeft;
    m_uTextPos.y = uyTop;

    // store selection rectangle position and size
	m_uSelRect.SetLeftTop(m_uTextPos);
    m_uSelRect.SetWidth(m_uClippedTextWidth);
    m_uSelRect.SetHeight(m_uClippedTextHeight);

}

void lmShapeTextbox::Render(lmPaper* pPaper, wxColour color)
{
    //ensure measures are ok
    ComputeTextPosition(pPaper);

    pPaper->SetFont(*m_pFont);
    pPaper->SetTextForeground(color);
    pPaper->DrawText(m_sClippedText, m_uTextPos.x, m_uTextPos.y);

    lmSimpleShape::Render(pPaper, color);
}

void lmShapeTextbox::SetFont(wxFont *pFont)
{
    m_pFont = pFont;
}

wxString lmShapeTextbox::Dump(int nIndent)
{
	wxString sDump = _T("");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
    sDump += wxString::Format(_T("Textbox: pos=(%.2f,%.2f), text=%s, "),
        m_uTextPos.x, m_uTextPos.y, m_sText.c_str() );
    sDump += DumpBounds();
    sDump += _T("\n");

	return sDump;
}

void lmShapeTextbox::Shift(lmLUnits xIncr, lmLUnits yIncr)
{
    m_uTextPos.x += xIncr;
    m_uTextPos.y += yIncr;

    ShiftBoundsAndSelRec(xIncr, yIncr);

	//if included in a composite shape update parent bounding and selection rectangles
	if (this->IsChildShape())
		((lmCompositeShape*)GetParentShape())->RecomputeBounds();
}

wxBitmap* lmShapeTextbox::OnBeginDrag(double rScale, wxDC* pDC)
{
	// A dragging operation is started. The view invokes this method to request the
	// bitmap to be used as drag image. No other action is required.
	// If no bitmap is returned drag is cancelled.
	//
	// So this method returns the bitmap to use with the drag image.


    // allocate a bitmap whose size is that of the box area
    // convert size to pixels
    int wD = (int)pDC->LogicalToDeviceXRel( m_uBoundsBottom.x - m_uBoundsTop.x );
    int hD = (int)pDC->LogicalToDeviceYRel( m_uBoundsBottom.y - m_uBoundsTop.y );
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
    dc2.DrawText(m_sClippedText, m_uTextPos.x - m_uBoundsTop.x, m_uTextPos.y - m_uBoundsTop.y);
    dc2.SelectObject(wxNullBitmap);

    // Make the bitmap masked
    wxImage image = bitmap.ConvertToImage();
    image.SetMaskColour(255, 255, 255);
    wxBitmap* pBitmap = new wxBitmap(image);

    ////DBG -----------
    //wxString sFileName = _T("lmShapeTextbox.bmp");
    //pBitmap->SaveFile(sFileName, wxBITMAP_TYPE_BMP);
    ////END DBG -------

    return pBitmap;

}

lmUPoint lmShapeTextbox::OnDrag(lmPaper* pPaper, const lmUPoint& uPos)
{
	// The view informs that the user continues dragging. We receive the new desired
	// shape position and we must return the new allowed shape position.
	//
	// The default behaviour is to return the received position, so the view redraws 
	// the drag image at that position. No action must be performed by the shape on 
	// the score and score objects.
	//
	// The received new desired shape position is in logical units and referred to page
	// origin. The returned new allowed shape position must also be in in logical units
	// and referred to page origin.


    // A Textbox can only be vertically moved, unless it is a floating block
    if (m_nBlockAlign == lmBLOCK_ALIGN_NONE)
    {
        //floating block
        //TODO
        return lmUPoint(uPos.x, uPos.y);
    }
    else
    {
        //move only in vertical
        return lmUPoint(m_uBoundsTop.x, uPos.y);
    }
}

void lmShapeTextbox::OnEndDrag(lmPaper* pPaper, lmController* pCanvas, const lmUPoint& uPos)
{
	// End drag. Receives the command processor associated to the view and the
	// final position of the object (logical units referred to page origin).
	// This method must validate/adjust final position and, if ok, it must 
	// send a move object command to the controller.

	lmUPoint uFinalPos(uPos.x, uPos.y);
    if (m_nBlockAlign == lmBLOCK_ALIGN_NONE)
    {
        //floating block
        //TODO
    }
    else
    {
        //move only in vertical
        uFinalPos.x = m_uBoundsTop.x;
    }

	//send a move object command to the controller
	pCanvas->MoveObject(this, uFinalPos);
}
