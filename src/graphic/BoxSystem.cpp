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
/*! @file BoxSystem.cpp
    @brief Implementation file for class lmBoxSystem
    @ingroup graphic_management
*/
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "BoxSystem.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "BoxSystem.h"

//access to colors
#include "../globals/Colors.h"
extern lmColors* g_pColors;

//-----------------------------------------------------------------------------------------

lmBoxSystem::lmBoxSystem()
{
    m_nNumMeasures = 0;

}


lmBoxSystem::~lmBoxSystem()
{
}

void lmBoxSystem::Render(int nSystem, lmScore* pScore, lmPaper* pPaper)
{
    //At this point paper position is not in the right place as it has been advanced
    //during the measurement operations.
    //This is not a problem for StaffObjs as they have stored its positioning information.
    //But for other objects such as staff lines and the prolog, that are drawn at current
    //paper position, this caouses incorrect positioning. Therefore, we restore here the
    //start of system position.
    pPaper->SetCursorY( m_yPos );

    wxInt32 iVStaff;
    lmInstrument *pInstr;
    lmVStaff *pVStaff;

    //for each lmInstrument
    for (pInstr = pScore->GetFirstInstrument(); pInstr; pInstr=pScore->GetNextInstrument())
    {
        //for each lmVStaff
        lmLUnits yPaperPos = pPaper->GetCursorY();
        for (iVStaff=1; iVStaff <= pInstr->GetNumStaves(); iVStaff++)
        {
            pVStaff = pInstr->GetVStaff(iVStaff);

            //to properly draw barlines it is necessary that staff lines are already drawn.
            //so, start the drawing by the staff lines
            pVStaff->DrawStaffLines(DO_DRAW, pPaper, 0, 50, 0, m_xFinal);

            //now draw the prolog, except if this is the first system
            if (nSystem != 1)
                pVStaff->DrawProlog(DO_DRAW, m_nFirstMeasure, false, pPaper);

            //draw the measures in this system
            for (int i = m_nFirstMeasure; i < m_nFirstMeasure + m_nNumMeasures; i++) {
                RenderMeasure(pVStaff, i, pPaper);
            }

            //// advance paper: height off this lmVStaff
            //pVStaff->NewLine(pPaper);
            ////! @todo advance inter-staff distance
            pPaper->SetCursorY( yPaperPos );

        } // next lmVStaff
        // advance paper: height off this lmVStaff
        pVStaff->NewLine(pPaper);
        //! @todo advance inter-staff distance

    } // next lmInstrument


    //Draw the initial barline that joins all staffs in a system
    pInstr = pScore->GetFirstInstrument();
    pVStaff = pInstr->GetVStaff(1);                    // first lmVStaff of this system
    lmLUnits xPos = pVStaff->GetXStartOfStaff();
    lmLUnits yTop = pVStaff->GetYTop();
    pInstr = pScore->GetLastInstrument();
    pVStaff = pInstr->GetVStaff(pInstr->GetNumStaves());    //last staff of this system
    lmLUnits yBottom = pVStaff->GetYBottom();

    lmLUnits THIN_LINE_WIDTH = lmToLogicalUnits(0.2, lmMILLIMETERS);        // thin line width will be 0.2 mm
    wxDC* pDC = pPaper->GetDC();
    wxPen pen(*wxBLACK, THIN_LINE_WIDTH, wxSOLID);
    wxBrush brush(*wxBLACK, wxSOLID);
    pDC->SetPen(pen);
    pDC->SetBrush(brush);
    pDC->DrawLine((wxCoord)xPos, (wxCoord)yTop, (wxCoord)xPos, (wxCoord)yBottom);

}


void lmBoxSystem::RenderMeasure(lmVStaff* pVStaff, int nMeasure, lmPaper* pPaper)
{
    //
    // Draw all StaffObjs in measure nMeasure, including the barline.
    // It is assumed that all positioning information is already computed
    //

    wxASSERT(nMeasure <= pVStaff->GetNumMeasures());

    /*! @todo
        Review this commented code. Implies to review also comented
        code in lmFormatter4::SizeMeasure
    */
    //el posicionamiento relativo de objetos (en LDP) requiere conocer la
    //posición de inicio del compas. Para ello, se guarda aquí, de forma
    //que el método GetXInicioCompas pueda devolver este valor
    //pVStaff->SetXInicioCompas = pPaper->GetCursorX()

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
    lmStaffObjIterator* pIT = pVStaff->CreateIterator(eTR_AsStored);
    pIT->AdvanceToMeasure(nMeasure);
    while(!pIT->EndOfList())
    {
        pSO = pIT->GetCurrent();

        if (pSO->GetType() == eTPO_Clef) {
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

        // if barline, exit loop: end of measure reached
        if (pSO->GetType() == eTPO_Barline) break;

        pIT->MoveNext();
    }
    delete pIT;

}

