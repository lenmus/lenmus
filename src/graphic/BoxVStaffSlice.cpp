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
#pragma implementation "BoxVStaffSlice.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "BoxScore.h"
#include "BoxPage.h"
#include "BoxSystem.h"
#include "BoxVStaffSlice.h"
#include "BoxInstrSlice.h"

//access to colors
#include "../globals/Colors.h"
extern lmColors* g_pColors;

//
// Class lmBoxVStaffSlice represents a part of a VStaff of an instrument.
//
// It is defined by the first and last measures contained in the slice.
// It is a container for positioning information
// It is used for:
//  - drawing the staff lines
//  - determine if a mouse click is on the staff
//

//-----------------------------------------------------------------------------------------

lmBoxVStaffSlice::lmBoxVStaffSlice(lmBoxInstrSlice* pParent, int nFirstMeasure, 
                                   int nLastMeasure, int nVStaff, lmVStaff* pVStaff)
    : lmBox(eGMO_BoxVStaffSlice)
{
    m_pInstrSlice = pParent;
    m_nFirstMeasure = nFirstMeasure;
    m_nLastMeasure = nLastMeasure;
    m_nVStaff = nVStaff;
    m_pVStaff = pVStaff;
}


lmBoxVStaffSlice::~lmBoxVStaffSlice()
{
}


lmLUnits lmBoxVStaffSlice::Render(lmPaper* pPaper, int nNumPage, int nSystem)
{
    lmLUnits xFrom = (m_pInstrSlice->GetBounds()).x;
    lmBoxSystem* pSystem = m_pInstrSlice->GetBoxSystem();
    lmLUnits xFinal = pSystem->GetSystemFinalX();

    //Now I define two auxiliary variables to save the start paper pos of 
    //VStaffs. This is necessary in case there is an overlayered VStaff, to
    //restore previous VStaff position
    lmLUnits xStartPos=0, yStartPos=0;      

    lmLUnits yBottomLeft;   
    bool fFirstStaffInSystem = (m_pInstrSlice->GetNumInstrument() == 1 && 
                                m_nVStaff == 1);


    //if it is not first VStaff and is overlayered, set paper position for this VStaff 
    if (m_nVStaff != 1) {
        if (m_pVStaff->IsOverlayered()) {
            //overlayered: restore paper position to previous VStaff position
            pPaper->SetCursorX( xStartPos );
            pPaper->SetCursorY( yStartPos );
        }
    }

    //save this VStaff start paper position, in case we need to restore it if
    //next VStaff is overlayered with this one
    xStartPos = pPaper->GetCursorX();
    yStartPos = pPaper->GetCursorY();
    SetXLeft(xFrom);
    SetYTop(yStartPos);

    //to properly draw barlines it is necessary that staff lines are already drawn.
    //so, lets draw the staff lines
    lmLUnits yTopLeftLine;
    //m_pVStaff->DrawStaffLines(pPaper, xFrom, xFinal, &yTopLeftLine, &yBottomLeft);

    //save start position of instrument
    if (m_nVStaff == 1)
        m_pInstrSlice->SetYTop(yTopLeftLine);

    //m_posStartStaff.push_back( lmUPoint(xFrom, yStartPos) );
    //m_posEndStaff.push_back( lmUPoint(xFinal, pPaper->GetCursorY()) );

    //if this is first staff of a system, save system start position
    if (fFirstStaffInSystem) {
        pSystem->SetXLeft(xFrom);
        pSystem->SetYTop(yTopLeftLine);
        fFirstStaffInSystem = false;
    }
    

    //now draw the prolog, except if this is the first system
    int nMaxMeasure = m_pVStaff->GetNumMeasures();
    if (nSystem != 1)
    {
        if (m_nFirstMeasure <= nMaxMeasure) {
            m_pVStaff->DrawProlog(DO_DRAW, m_nFirstMeasure, false, pPaper);
        }
        else
        {
            // we are drawing an empty system after final barline. Use
            // for prolog the last measure if it exists!
            if (nMaxMeasure != 0)
                m_pVStaff->DrawProlog(DO_DRAW, nMaxMeasure, false, pPaper);
        }
    }

    //draw the measures in this system
    for (int i = m_nFirstMeasure; i <= m_nLastMeasure; i++) {
        RenderMeasure(i, pPaper, nNumPage);
    }

    //advance paper in height off this lmVStaff
    m_pVStaff->NewLine(pPaper);
    //TODO advance inter-staff distance

    //save bounds
    SetXRight(xFinal);
    SetYBottom(yBottomLeft);

    return yBottomLeft;

}

void lmBoxVStaffSlice::RenderMeasure(int nMeasure, lmPaper* pPaper, int nNumPage)
{
    //
    // Draw all StaffObjs in measure nMeasure, including the barline.
    // It is assumed that all positioning information is already computed
    //

    wxASSERT(nMeasure <= m_pVStaff->GetNumMeasures());

        //Experimental code to render cursor
    lmBoxSystem* pBSystem = m_pInstrSlice->GetBoxSystem();  //parent system
    lmBoxPage* pBPage = pBSystem->GetBoxPage();             //parent page
    lmBoxScore* pBScore = pBPage->GetBoxScore();            //parent score
    lmStaffObj* pCursorObj = pBScore->GetCursorPointedObject();

    /*TODO
        Review this commented code. Implies to review also comented
        code in lmFormatter4::SizeMeasure
    */
    //el posicionamiento relativo de objetos (en LDP) requiere conocer la
    //posición de inicio del compas. Para ello, se guarda aquí, de forma
    //que el método GetXInicioCompas pueda devolver este valor
    //m_pVStaff->SetXInicioCompas = pPaper->GetCursorX()

    //TODO Review this
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
                /*TODO
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

            //Experimental code to render cursor
        if (false && pCursorObj && pCursorObj->GetID() == pSO->GetID())
        {
            //Cursor is pointing to this StaffObj. Render it
            pSO->Draw(DO_DRAW, pPaper, *wxBLUE);

	        ////cursor geometry
	        //lmURect SOPos = pSO->GetSelRect();
	        //lmLUnits uxLine = SOPos.GetLeft();
	        //lmLUnits uyTop = SOPos.GetY();
	        //lmLUnits uyBottom = uyTop + SOPos.GetHeight();
         //   lmLUnits uWidth = m_pVStaff->TenthsToLogical(2, 1);

	        ////draw vertical line
	        ////dc.DrawLine(vxLine, vyTop, vxLine, vyBottom);
         //   pPaper->SolidLine(uxLine, uyTop, uxLine, uyBottom, uWidth, eEdgeNormal, *wxBLUE);

	        //////draw horizontal segments
	        ////dc.DrawLine(vxLine-vdxSegment, vyTop-1, vxLine+vdxSegment+1, vyTop-1);
	        ////dc.DrawLine(vxLine-vdxSegment, vyBottom, vxLine+vdxSegment+1, vyBottom);

        }

        // if barline, exit loop: end of measure reached
        if (pSO->GetClass() == eSFOT_Barline) break;

        pIT->MoveNext();
    }
    delete pIT;

}

wxString lmBoxVStaffSlice::Dump(int nIndent)
{
	//TODO
	wxString sDump = _T("");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
	sDump.append(_T("lmBoxVStaffSlice\n"));
	return sDump;
}
