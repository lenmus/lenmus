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
#pragma implementation "Slur.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Score.h"
#include "../graphic/GMObject.h"
#include "../graphic/ShapeNote.h"
#include "../graphic/ShapeArch.h"



//===================================================================================
// lmBezier implementation
//===================================================================================

lmBezier::lmBezier()
{
}

lmBezier::~lmBezier()
{
}

void lmBezier::SetPoint(int nPointID, lmTPoint& uPoint)
{
    wxASSERT(nPointID >= 0 && nPointID < lmBEZIER_MAX);
    m_tPoints[nPointID] = uPoint;
}

lmTPoint& lmBezier::GetPoint(int nPointID)
{
    wxASSERT(nPointID >= 0 && nPointID < lmBEZIER_MAX);
    return m_tPoints[nPointID];
}

wxString lmBezier::SourceLDP(int nIndent)
{
    WXUNUSED(nIndent);

	wxString sSource = _T(" (bezier");
    bool fDefault = true;       //default bezier values
    static wxString sPointNames[4] = { _T("start"), _T("end"), _T("ctrol1"), _T("ctrol2") };

    for (int i=0; i < 4; i++)
    {
        if (m_tPoints[i].x != 0.0f || m_tPoints[i].y != 0.0f)
        {
            fDefault = false;
            if (m_tPoints[i].x != 0.0f)
		        sSource += wxString::Format(_T(" % s-x:%s"), sPointNames[i].c_str(),
                                    DoubleToStr((double)m_tPoints[i].x, 4).c_str() );
            if (m_tPoints[i].y != 0.0f)
		        sSource += wxString::Format(_T(" % s-y:%s"), sPointNames[i].c_str(),
                                    DoubleToStr((double)m_tPoints[i].y, 4).c_str() );
        }
    }
    sSource += _T(")");

    //if default values return empty string
    if (fDefault)
        return wxEmptyString;
    else
    	return sSource;
}

wxString lmBezier::SourceXML(int nIndent)
{
    return wxEmptyString;
}



//===================================================================================
// lmTie implementation
//===================================================================================

lmTie::lmTie(lmNote* pStartNote, lmNote* pEndNote)
    : lmBinaryRelObj(eAXOT_Tie, pStartNote, pEndNote, lmDRAGGABLE)
{
    DefineAsMultiShaped();      //define a tie as a multi-shaped ScoreObj
}

lmTie::~lmTie()
{
}

void lmTie::SetBezierPoints(int nBezier, lmTPoint* ptPoints)
{
    wxASSERT(nBezier == 0 || nBezier == 1);

    for (int i=0; i < 4; i++)
    {
        m_Bezier[nBezier].SetPoint(i, *(ptPoints+i));
    }
}

lmLUnits lmTie::LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour color)
{
	// Create two arch shapes
	// Both notes' shapes will have attached both tie shapes.
	// One of the tie shapes will be invisible

    WXUNUSED(uPos);

    //prepare information
    lmShapeNote* pShapeStart = (lmShapeNote*)m_pStartNR->GetShape();
    lmShapeNote* pShapeEnd = (lmShapeNote*)m_pEndNR->GetShape();
    bool fTieUnderNote = !((lmNote*)m_pStartNR)->StemGoesDown();

	//create the first tie shape

    //convert bezier displacements to logical units
    lmUPoint uPoints[4];
    for (int i=0; i < 4; i++)
    {
        uPoints[i].x = m_pParent->TenthsToLogical(m_Bezier[0].GetPoint(i).x);
        uPoints[i].y = m_pParent->TenthsToLogical(m_Bezier[0].GetPoint(i).y);
    }

    //creat the shape
    lmShapeTie* pShape1 = new lmShapeTie(this, 0, (lmNote*)m_pEndNR, &uPoints[0], pShapeStart,
                                         pShapeEnd, fTieUnderNote, color, lmVISIBLE);
    StoreShape(pShape1);
	pBox->AddShape(pShape1);
	pShapeStart->Attach(pShape1, eGMA_StartNote);
	pShapeEnd->Attach(pShape1, eGMA_EndNote);
    pShapeEnd->OnTieAttached(0, pShape1);     //inform end note shape of this attachment



	//create the second tie shape

    //convert bezier displacements to logical units
    for (int i=0; i < 4; i++)
    {
        uPoints[i].x = m_pParent->TenthsToLogical(m_Bezier[1].GetPoint(i).x);
        uPoints[i].y = m_pParent->TenthsToLogical(m_Bezier[1].GetPoint(i).y);
    }

    //creat the shape
    lmShapeTie* pShape2 = new lmShapeTie(this, 1, (lmNote*)m_pStartNR, &uPoints[0], pShapeStart,
                                         pShapeEnd, fTieUnderNote, color, lmNO_VISIBLE);
    StoreShape(pShape2);
	pBox->AddShape(pShape2);
	pShapeStart->Attach(pShape2, eGMA_StartNote);
	pShapeEnd->Attach(pShape2, eGMA_EndNote);
    pShapeEnd->OnTieAttached(1, pShape2);     //inform end note shape of this attachment

	//link both ties
	pShape1->SetBrotherTie(pShape2);
	pShape2->SetBrotherTie(pShape1);

	//return the shape width
    return pShape1->GetWidth();
}

void lmTie::PropagateNotePitchChange(lmNote* pNote, int nStep, int nOctave, int nAlter, bool fForward)
{
    if (pNote == m_pStartNR && fForward)
    {
        // propagate forwards
        ((lmNote*)m_pEndNR)->PropagateNotePitchChange(nStep, nOctave, nAlter, lmFORWARDS);
    }
    else if (pNote == m_pEndNR && !fForward)
    {
        // propagate backwards
        ((lmNote*)m_pStartNR)->PropagateNotePitchChange(nStep, nOctave, nAlter, lmBACKWARDS);
    }
    //other cases are for notes whose pitch is already changed
}

void lmTie::MoveObjectPoints(int nNumPoints, int nShapeIdx, lmUPoint* pShifts, bool fAddShifts)
{
    //This method is only used during interactive edition.
    //It receives a vector with the shifts for object points and a flag to signal
    //whether to add or to substract shifts.

    wxASSERT(nNumPoints == GetNumPoints());
    wxASSERT(nShapeIdx == 0 || nShapeIdx == 1);

    for (int i=0; i < nNumPoints; i++)
    {
        if (fAddShifts)
        {
            lmTPoint tNew = m_Bezier[nShapeIdx].GetPoint(i);
            tNew.x += m_pParent->LogicalToTenths((*(pShifts+i)).x);
            tNew.y += m_pParent->LogicalToTenths((*(pShifts+i)).y);
            m_Bezier[nShapeIdx].SetPoint(i, tNew);
        }
        else
        {
            lmTPoint tNew = m_Bezier[nShapeIdx].GetPoint(i);
            tNew.x -= m_pParent->LogicalToTenths((*(pShifts+i)).x);
            tNew.y -= m_pParent->LogicalToTenths((*(pShifts+i)).y);
            m_Bezier[nShapeIdx].SetPoint(i, tNew);
        }
    }

    //inform the shape
    lmShapeTie* pShape = (lmShapeTie*)GetGraphicObject(nShapeIdx);
    wxASSERT(pShape);
    pShape->MovePoints(nNumPoints, nShapeIdx, pShifts, fAddShifts);
}

lmUPoint lmTie::ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper)
{
    return uOrg;
}

wxString lmTie::SourceLDP_First(int nIndent)
{
    WXUNUSED(nIndent);

    //wxString sSource = _T(" l");
    wxString sSource = wxString::Format(_T(" (tie %d start"), GetID());
    sSource += m_Bezier[0].SourceLDP(nIndent);
    sSource += _T(")");
    return sSource;
}

wxString lmTie::SourceLDP_Last(int nIndent)
{
    WXUNUSED(nIndent);

    wxString sSource = wxString::Format(_T(" (tie %d stop"), GetID());
    sSource += m_Bezier[1].SourceLDP(nIndent);
    sSource += _T(")");
    return sSource;
}

wxString lmTie::SourceXML(int nIndent)
{
    return _T("");
}

wxString lmTie::Dump()
{
    return _T("");
}



////===================================================================================
//// lmSlur implementation
////===================================================================================
