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
#pragma implementation "BoxSliceVStaff.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "../score/Score.h"
#include "BoxScore.h"
#include "BoxPage.h"
#include "BoxSystem.h"
#include "BoxSlice.h"
#include "BoxSliceInstr.h"
#include "BoxSliceVStaff.h"
#include "ShapeStaff.h"

//access to colors
#include "../globals/Colors.h"
extern lmColors* g_pColors;

//-----------------------------------------------------------------------------------------
// Implementation of class lmBoxSliceVStaff: a part (measure) of a VStaff of an instrument.
//

lmBoxSliceVStaff::lmBoxSliceVStaff(lmBoxSliceInstr* pParent, lmVStaff* pVStaff)
    : lmBox(eGMO_BoxSliceVStaff)
{
    m_pSliceInstr = pParent;
    m_pVStaff = pVStaff;
}


lmBoxSliceVStaff::~lmBoxSliceVStaff()
{
    //delete staff shapes
    for (int i=0; i < (int)m_ShapeStaff.size(); i++)
    {
        delete m_ShapeStaff[i];
    }
    m_ShapeStaff.clear();
}

void lmBoxSliceVStaff::Render(lmPaper* pPaper, lmUPoint uPos)
{
    //render staff lines
    for (int i=0; i < (int)m_ShapeStaff.size(); i++)
    {
        m_ShapeStaff[i]->Render(pPaper);
    }

	//render shapes
    for (int i=0; i < (int)m_Shapes.size(); i++)
    {
        m_Shapes[i]->Render(pPaper);
    }

#if 0   //1 = render also staffObjs. 
	int nMeasure = m_pSliceInstr->GetNumMeasure();
	int nNumPage = 1;
	RenderMeasure(nMeasure, pPaper, nNumPage);
#endif
}

void lmBoxSliceVStaff::AddShape(lmShape* pShape)
{
	//override to avoid adding the staff to the shapes list
	if (pShape->GetType() == eGMO_ShapeStaff)
	{
		m_ShapeStaff.push_back( (lmShapeStaff*)pShape );
	}
	else
		lmBox::AddShape(pShape);

    //for visual highlight we need to know the page in wich the StaffObj to highlight
    //is located. To this end we are going to store the page number in each
    //StaffObj
    //lmObject* pSO = pShape->Owner();
    //if (pSO->G
    //pSO->SetPageNumber(nNumPage);

}

void lmBoxSliceVStaff::RenderMeasure(int nMeasure, lmPaper* pPaper, int nNumPage)
{
    ////
    //// Draw all StaffObjs in measure nMeasure, including the barline.
    //// It is assumed that all positioning information is already computed
    ////

    //wxASSERT(nMeasure <= m_pVStaff->GetNumMeasures());


    ///*TODO
    //    Review this commented code. Implies to review also comented
    //    code in lmFormatter4::SizeMeasure
    //*/
    ////el posicionamiento relativo de objetos (en LDP) requiere conocer la
    ////posición de inicio del compas. Para ello, se guarda aquí, de forma
    ////que el método GetXInicioCompas pueda devolver este valor
    ////m_pVStaff->SetXInicioCompas = pPaper->GetCursorX()

    ////TODO Review this
    //////si no es el primer compas de la partitura avanza separación con la barra de compas
    //////o con prólogo, si es comienzo de línea.
    ////if (nMeasure != 1) {
    ////    m_oCanvas.Avanzar        //separación con la barra de compas
    ////}

    ////space occupied by clefs is computed only when all clefs has been drawn, so that we
    ////can properly align notes and othe StaffObjs. The next flag is used to signal that
    ////it is pending to compute clefs space.
    //bool fSpacePending = false;        //initialy not clefs ==> no space pending
    //lmLUnits xClefs=0;                //x position of first clef. To align all clefs
    //lmLUnits nMaxClefWidth=0;        //to save the width of the wider clef

    ////loop to process all StaffObjs in this measure
    //lmStaffObj* pSO = (lmStaffObj*)NULL;
    //lmStaffObjIterator* pIT = m_pVStaff->CreateIterator(eTR_AsStored);
    //pIT->AdvanceToMeasure(nMeasure);
    //while(!pIT->EndOfList())
    //{
    //    pSO = pIT->GetCurrent();

    //    if (pSO->GetClass() == eSFOT_Clef) {
    //        //clefs don't consume space until a lmStaffObj of other type is found
    //        if (!fSpacePending) {
    //            //This is the first cleft. Save paper position
    //            xClefs = pPaper->GetCursorX();
    //            fSpacePending = true;
    //            nMaxClefWidth = 0;
    //        } else {
    //            /*TODO
    //                Review this. I thing that now, with lmTimeposTable mechanism,
    //                it is useless.
    //            */
    //            pPaper->SetCursorX(xClefs);        //force position to align all clefs
    //        }
    //        pSO->Draw(DO_DRAW, pPaper);
    //        if (fSpacePending) {
    //            nMaxClefWidth = wxMax(nMaxClefWidth, pPaper->GetCursorX() - xClefs);
    //        }

    //    } else {
    //        //It is not a clef. Just draw it
    //        if (fSpacePending) {
    //            pPaper->SetCursorX(xClefs + nMaxClefWidth);
    //            fSpacePending = false;
    //        }
    //        pSO->Draw(DO_DRAW, pPaper);

    //    }

    //    //for visual highlight we need to know the page in wich the StaffObj to highlight
    //    //is located. To this end we are going to store the page number in each
    //    //StaffObj
    //    pSO->SetPageNumber(nNumPage);

    //    // if barline, exit loop: end of measure reached
    //    if (pSO->GetClass() == eSFOT_Barline) break;

    //    pIT->MoveNext();
    //}
    //delete pIT;

}

void lmBoxSliceVStaff::UpdateXLeft(lmLUnits xLeft)
{
	// During layout there is a need to update initial computations about this
	// box slice position. This update must be propagated to all contained boxes

	SetXLeft(xLeft);

}

void lmBoxSliceVStaff::UpdateXRight(lmLUnits xRight)
{
	// During layout there is a need to update initial computations about this
	// box slice position. This update must be propagated to all contained boxes

	SetXRight(xRight);

}

void lmBoxSliceVStaff::SystemXRightUpdated(lmLUnits xRight)
{
	// During layout there is a need to update initial computations about this
	// box slice position. This method is invoked when the right x position of
	// the parent system has been updated. It is only invoked for the first
	// slice of the system in order to update the ShapeStaff final position

    for (int i=0; i < (int)m_ShapeStaff.size(); i++)
    {
        m_ShapeStaff[i]->SetXRight(xRight);
    }


}

void lmBoxSliceVStaff::CopyYBounds(lmBoxSliceVStaff* pBSV)
{
	//This method is only invoked during layout phase, when the number of measures in the
	//system has been finally decided. There is a need to copy 'y' coordinates from first
	//slice to all others. This method receives the first vstaff slice and must copy 'y'
	//coordinates from there

	SetYTop(pBSV->GetYTop());
	SetYBottom(pBSV->GetYBottom());

}

wxString lmBoxSliceVStaff::Dump(int nIndent)
{
	wxString sDump = _T("");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
	sDump.append(_T("lmBoxSliceVStaff "));
    sDump += DumpBounds();
    sDump += _T("\n");

	nIndent++;

	// dump the staff
    for (int i=0; i < (int)m_ShapeStaff.size(); i++)
    {
        sDump += m_ShapeStaff[i]->Dump(nIndent);
    }

    //dump the other shapes
    for (int i=0; i < (int)m_Shapes.size(); i++)
    {
        sDump += m_Shapes[i]->Dump(nIndent);
    }

	return sDump;
}

lmGMObject* lmBoxSliceVStaff::FindGMObjectAtPosition(lmUPoint& pointL)
{
    //if not in this object return
    if (!ContainsPoint(pointL)) 
        return (lmGMObject*)NULL;

    //look in shapes collection
    lmShape* pShape = FindShapeAtPosition(pointL);
    if (pShape) return pShape;

	//is it any staff?
    for (int i=0; i < (int)m_ShapeStaff.size(); i++)
    {
        if (m_ShapeStaff[i]->ContainsPoint(pointL))
			return m_ShapeStaff[i];
    }

    // no shape found. So the point is in this object
    return this;

}
