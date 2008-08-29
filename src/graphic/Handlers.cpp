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
#pragma implementation "Handlers.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "GMObject.h"
#include "../score/Score.h"
#include "../app/ScoreCanvas.h"

#include "Handlers.h"


//-------------------------------------------------------------------------------------
// Implementation of lmHandler
//-------------------------------------------------------------------------------------


lmHandler::lmHandler(lmScoreObj* pOwner)
	: lmSimpleShape(eGMO_Handler, pOwner, 0, _T("Handler"), lmDRAGGABLE, lmNO_SELECTABLE)
{
}



//-------------------------------------------------------------------------------------
// Implementation of lmShapeMargin
//-------------------------------------------------------------------------------------


lmShapeMargin::lmShapeMargin(lmScore* pScore, int nIdx, bool fVertical, lmLUnits uPos,
                             lmLUnits uLenght, wxColour color)
	: lmHandler(pScore)
{
    m_nIdx = nIdx;

    m_fVertical = fVertical;
    m_uPos = uPos;
    m_uLenght = uLenght;
	m_color = color;

    //options?
    m_uThighness = pScore->TenthsToLogical(10.0f);   //handler thighness
    m_uWidth = pScore->TenthsToLogical(20.0f);       //handler width

	//set bounds
    lmLUnits uWidth = m_uThighness / 2.0f;
    if (fVertical)
    {
	    SetXLeft(m_uPos - uWidth);
	    SetYTop(0.0f);
	    SetXRight(m_uPos + uWidth);
	    SetYBottom(m_uLenght);
    }
    else
    {
	    SetXLeft(0.0f);
	    SetYTop(m_uPos - uWidth);
	    SetXRight(m_uLenght);
	    SetYBottom(m_uPos + uWidth);
    }

    // store selection rectangle position and size
	m_uSelRect = GetBounds();
}

lmShapeMargin::~lmShapeMargin()
{
}

void lmShapeMargin::Render(lmPaper* pPaper, wxColour color)
{
    // Render will draw nothing unless flag g_fShowMargins is true

    if (!g_fShowMargins) return;


    //render the margin
    wxColour colorC = color;

    pPaper->SetLogicalFunction(wxXOR);

    // as painting uses XOR we need the complementary color
    colorC = wxColour(255 - (int)color.Red(), 255 - (int)color.Green(), 255 - (int)color.Blue() );


    DrawLine(pPaper, colorC);
    DrawHandlers(pPaper, colorC);

    pPaper->SetLogicalFunction(wxCOPY);

    lmSimpleShape::Render(pPaper, color);
}

wxString lmShapeMargin::Dump(int nIndent)
{
	wxString sDump = _T("");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
	sDump += wxString::Format(_T("%04d %s: uPos=%.2f, uLenght=%.2f, fVertical=%s, "),
		m_nOwnerIdx, m_sGMOName.c_str(), m_uPos, m_uLenght, 
        (m_fVertical ? _T("yes") : _T("no")) );
    sDump += DumpBounds();
    sDump += _T("\n");
	return sDump;
}

void lmShapeMargin::Shift(lmLUnits xIncr, lmLUnits yIncr)
{
    if (m_fVertical)
    {
        m_uPos += xIncr;
        ShiftBoundsAndSelRec(xIncr, 0.0f);
    }
    else
    {
        m_uPos += yIncr;
        ShiftBoundsAndSelRec(0.0f, yIncr);
    }
}

void lmShapeMargin::DrawLine(lmPaper* pPaper, wxColour color)
{
    if (m_fVertical)
        pPaper->SketchLine(m_uPos, 0.0f, m_uPos, m_uLenght, color);
    else
        pPaper->SketchLine(0.0f, m_uPos, m_uLenght, m_uPos, color);
}

void lmShapeMargin::DrawHandlers(lmPaper* pPaper, wxColour color)
{
    if (m_fVertical)
    {
        lmUSize size(m_uThighness, m_uWidth);
        lmUPoint uLeft(m_uPos - m_uThighness / 2.0f, 0.0f);
        pPaper->SketchRectangle(uLeft, size, color);
        lmUPoint uRight(m_uPos - m_uThighness / 2.0f, m_uLenght - m_uWidth);
        pPaper->SketchRectangle(uRight, size, color);
    }
    else
    {
        lmUSize size(m_uWidth, m_uThighness);
        lmUPoint uTop(0.0f, m_uPos - m_uThighness / 2.0f);
        pPaper->SketchRectangle(uTop, size, color);
        lmUPoint uBottom(m_uLenght - m_uWidth, m_uPos - m_uThighness / 2.0f);
        pPaper->SketchRectangle(uBottom, size, color);
    }
}

void lmShapeMargin::OnMouseIn(wxWindow* pWindow, lmUPoint& pointL)
{
    if (m_fVertical)
	    pWindow->SetCursor( wxCursor(wxCURSOR_SIZEWE) );
    else
	    pWindow->SetCursor( wxCursor(wxCURSOR_SIZENS) );

    m_pMouseCursorWindow = pWindow;
}

lmUPoint lmShapeMargin::OnDrag(lmPaper* pPaper, const lmUPoint& uPos)
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

    // A margin line only can be moved in vertical or horizontal

    //limit margins movement to have at least 30% of page size for rendering the score
    lmScore* pScore = (lmScore*)m_pOwner;
    lmUPoint pos = uPos;
    pos = pScore->CheckHandlerNewPosition(this, m_nIdx, pos);


    //received paper is a DirectDrawer DC

    //erase previous draw
    Render(pPaper, *wxGREEN);

    //store new coordinates
    if (m_fVertical)
        m_uPos = pos.x;
    else
        m_uPos = pos.y;

    //draw at new position
    Render(pPaper, *wxGREEN);

    return pos;
}

void lmShapeMargin::OnEndDrag(lmController* pCanvas, const lmUPoint& uPos)
{
	// End drag. Receives the command processor associated to the view and the
	// final position of the object (logical units referred to page origin).
	// This method must validate/adjust final position and, if ok, it must 
	// send a move object command to the controller.


    lmScore* pScore = (lmScore*)m_pOwner;
    lmUPoint pos = uPos;
    pos = pScore->CheckHandlerNewPosition(this, m_nIdx, pos);

    //save new position and send the command to change margin position
    if (m_fVertical)
        m_uPos = pos.x;
    else
        m_uPos = pos.y;

    pCanvas->ChangePageMargin(this, m_nIdx, m_uPos);
}
