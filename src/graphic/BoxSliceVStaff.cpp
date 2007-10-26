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
    //delete shapes collection
    for (int i=0; i < (int)m_Shapes.size(); i++)
    {
        delete m_Shapes[i];
    }
    m_Shapes.clear();
}

void lmBoxSliceVStaff::AddShape(lmShape* pShape)
{
    m_Shapes.push_back(pShape);
}

void lmBoxSliceVStaff::Render(lmPaper* pPaper, lmUPoint uPos, wxColour color)
{
    for (int i=0; i < (int)m_Shapes.size(); i++)
    {
        m_Shapes[i]->Render(pPaper, uPos, color);
    }

	int nMeasure = m_pSliceInstr->GetNumMeasure();
	int nNumPage = 1;
	RenderMeasure(nMeasure, pPaper, nNumPage);
}

void lmBoxSliceVStaff::RenderMeasure(int nMeasure, lmPaper* pPaper, int nNumPage)
{
    //
    // Draw all StaffObjs in measure nMeasure, including the barline.
    // It is assumed that all positioning information is already computed
    //

    wxASSERT(nMeasure <= m_pVStaff->GetNumMeasures());


    /*! @todo
        Review this commented code. Implies to review also comented
        code in lmFormatter4::SizeMeasure
    */
    //el posicionamiento relativo de objetos (en LDP) requiere conocer la
    //posición de inicio del compas. Para ello, se guarda aquí, de forma
    //que el método GetXInicioCompas pueda devolver este valor
    //m_pVStaff->SetXInicioCompas = pPaper->GetCursorX()

    //! @todo Review this
    ////si no es el primer compas de la partitura avanza separación con la barra de compas
    ////o con prólogo, si es comienzo de línea.
    //if (nMeasure != 1) {
    //    m_oCanvas.Avanzar        //separación con la barra de compas
    //}

    //space occupied by clefs is computed only when all clefs has been drawn, so that we
    //can properly align notes and othe StaffObjs. The next flag is used to signal that
    //it is pending to compute clefs space.
    bool fSpacePending = false;        //initialy not clefs ==> no space pending
    lmLUnits xClefs=0;                //x position of first clef. To align all clefs
    lmLUnits nMaxClefWidth=0;        //to save the width of the wider clef

    //loop to process all StaffObjs in this measure
    lmStaffObj* pSO = (lmStaffObj*)NULL;
    lmStaffObjIterator* pIT = m_pVStaff->CreateIterator(eTR_AsStored);
    pIT->AdvanceToMeasure(nMeasure);
    while(!pIT->EndOfList())
    {
        pSO = pIT->GetCurrent();

        if (pSO->GetClass() == eSFOT_Clef) {
            //clefs don't consume space until a lmStaffObj of other type is found
            if (!fSpacePending) {
                //This is the first cleft. Save paper position
                xClefs = pPaper->GetCursorX();
                fSpacePending = true;
                nMaxClefWidth = 0;
            } else {
                /*! @todo
                    Review this. I thing that now, with lmTimeposTable mechanism,
                    it is useless.
                */
                pPaper->SetCursorX(xClefs);        //force position to align all clefs
            }
            pSO->Draw(DO_DRAW, pPaper);
            if (fSpacePending) {
                nMaxClefWidth = wxMax(nMaxClefWidth, pPaper->GetCursorX() - xClefs);
            }

        } else {
            //It is not a clef. Just draw it
            if (fSpacePending) {
                pPaper->SetCursorX(xClefs + nMaxClefWidth);
                fSpacePending = false;
            }
            pSO->Draw(DO_DRAW, pPaper);

        }

        //for visual highlight we need to know the page in wich the StaffObj to highlight
        //is located. To this end we are going to store the page number in each
        //StaffObj
        pSO->SetPageNumber(nNumPage);

        // if barline, exit loop: end of measure reached
        if (pSO->GetClass() == eSFOT_Barline) break;

        pIT->MoveNext();
    }
    delete pIT;

}

void lmBoxSliceVStaff::SetFinalX(lmLUnits xPos)
{ 
	//locate ShapeStaff objects
    for (int i=0; i < (int)m_Shapes.size(); i++)
    {
		if (m_Shapes[i]->GetType() == eGMO_ShapeStaff)
			m_Shapes[i]->SetXRight(xPos);
    }
}
