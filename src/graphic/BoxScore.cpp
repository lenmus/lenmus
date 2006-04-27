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
/*! @file BoxScore.cpp
    @brief Implementation file for class lmBoxScore
    @ingroup graphic_management
*/
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "BoxScore.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "BoxScore.h"

//access to colors
#include "../globals/Colors.h"
extern lmColors* g_pColors;

//-----------------------------------------------------------------------------------------

lmBoxScore::lmBoxScore(lmScore* pScore, lmPaper* m_pPaper)
{
    m_pScore = pScore;
    m_pPaper = m_pPaper;

    //Initially the BoxScore will have one page.
    AddPage();

}


lmBoxScore::~lmBoxScore()
{
    //delete all pages
    WX_CLEAR_ARRAY(m_aPages);
}

void lmBoxScore::Render(lmPaper* pPaper)
{
    // write score titles
    m_pScore->WriteTitles(DO_DRAW, pPaper);
    pPaper->RestartPageCursors();                //restore page cursors are at top-left corner

    int nSystem;                    //number of system in process
    int iIni = 1;                   //number of measure in process

    //loop to render pages
    int iPage;
    lmBoxPage* pBoxPage;
    for(iPage=0; iPage < (int)m_aPages.GetCount(); iPage++) {
        pBoxPage = m_aPages.Item(iPage);
        if (iPage != 0) pPaper->NewPage();
        wxLogMessage(_T("[lmBoxScore::Render] Rendering page %d. Fisrt system=%d, last system=%d, iIni=%d"),
                iPage, pBoxPage->GetFirstSystem(), pBoxPage->GetLastSystem(), iIni);
        for (nSystem = pBoxPage->GetFirstSystem(); nSystem <= pBoxPage->GetLastSystem(); nSystem++) {
            iIni += RenderSystem(nSystem, iIni, pPaper);
        }
    }

}

int lmBoxScore::RenderSystem(int nSystem, int iIni, lmPaper* pPaper)
{
    //Get its positioning information
    int nMeasuresInSystem = m_nNumMeasures[nSystem];    //number of measures that fit in current system

    //At this point paper position is not in the right place as it has been advanced
    //during the measurement operations.
    //This is not a problem for StaffObjs as they have stored its positioning information.
    //But for other objects such as staff lines and the prolog, that are drawn at current
    //paper position, this caouses incorrect positioning. Therefore, we saved the start
    //system position in m_ySystemPos[] and must restore it here.
    //! @todo include positioning information in the lmStaff objects
    pPaper->SetCursorY( m_ySystemPos[nSystem] );

    wxInt32 iVStaff;
    lmInstrument *pInstr;
    lmVStaff *pVStaff;

    //for each lmInstrument
    for (pInstr = m_pScore->GetFirstInstrument(); pInstr; pInstr=m_pScore->GetNextInstrument())
    {
        //for each lmVStaff
        for (iVStaff=1; iVStaff <= pInstr->GetNumStaves(); iVStaff++) {
            pVStaff = pInstr->GetVStaff(iVStaff);
            /*! @todo
                Finish next code for fTruncarUltimoSistema
                to properly draw barlines it is necessary that staff lines are already drawn.
                so, start the drawing by the staff lines
            */
//                if (nSystem == m_nNumSystems And fTruncarUltimoSistema) {
//                    //es el último sistema y se pide truncar: dibuja las líneas del pentagrama
//                    //para que lleguen sólo hasta la barra de fin de partitura
//                    pVStaff->PintarPentagrama DO_DRAW, , pVStaff->GetXPosBarraFinal - 1
//                } else {
                //staff lines to the right margin
                //! @todo Length of staff lines
                pVStaff->DrawStaffLines(DO_DRAW, pPaper, 0, 50);
//                }

            //draw the prolog, except if this is the first system
            if (nSystem != 1)
                pVStaff->DrawProlog(DO_DRAW, (nSystem == 1), pPaper);

            //draw the measures in this system
            for (int i = iIni; i < iIni + nMeasuresInSystem; i++) {
                RenderMeasure(pVStaff, i, pPaper);
            }

            // advance paper: height off this lmVStaff
            pVStaff->NewLine(pPaper);
            //! @todo advance inter-staff distance

        } // next lmVStaff
    } // next lmInstrument


    //Draw the initial barline that joins all staffs in a system
    pInstr = m_pScore->GetFirstInstrument();
    pVStaff = pInstr->GetVStaff(1);                    // first lmVStaff of this system
    lmLUnits xPos = pVStaff->GetXStartOfStaff();
    lmLUnits yTop = pVStaff->GetYTop();
    pInstr = m_pScore->GetLastInstrument();
    pVStaff = pInstr->GetVStaff(pInstr->GetNumStaves());    //last staff of this system
    lmLUnits yBottom = pVStaff->GetYBottom();

    lmLUnits THIN_LINE_WIDTH = lmToLogicalUnits(0.2, lmMILLIMETERS);        // thin line width will be 0.2 mm
    wxDC* pDC = pPaper->GetDC();
    wxPen pen(*wxBLACK, THIN_LINE_WIDTH, wxSOLID);
    wxBrush brush(*wxBLACK, wxSOLID);
    pDC->SetPen(pen);
    pDC->SetBrush(brush);
    pDC->DrawLine((wxCoord)xPos, (wxCoord)yTop, (wxCoord)xPos, (wxCoord)yBottom);


    return nMeasuresInSystem;

}


void lmBoxScore::RenderMeasure(lmVStaff* pVStaff, int nMeasure, lmPaper* pPaper)
{
    /*
    Draw all StaffObjs in measure nMeasure, including the barline.
    It is assumed that all positioning information is already computed
    */

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

            //! @todo Is next code up to date? usefull for anything?
            //Set oClave = pSO
            //pVStaff->SetCurrentKey(oClave.Pentagrama) = pSO

        } else {
            //It is not a clef. Just draw it

            //! @todo Is next code up to date? usefull for anything?
            //if (pSO->GetType() = eTPO_KeySignature) {
            //    pVStaff->SetCurrentTonalKey = pSO
            //ElseIf pSO->GetType() = eTPO_TimeSignature) {
            //    pVStaff->SetCurrentTimeKey = pSO
            //}

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

void lmBoxScore::CopyData(int nNumMeasures[], lmLUnits ySystemPos[], 
                          bool fNewPage[], int nNumSystems)
{
    int i;
    for (i=0; i < MAX_SYSTEMS+1; i++) {
        m_nNumMeasures[i] = nNumMeasures[i];
        m_ySystemPos[i] = ySystemPos[i];
        m_fNewPage[i] = fNewPage[i];
    }
    m_nNumSystems = nNumSystems;

}

lmBoxPage* lmBoxScore::AddPage()
{
    lmBoxPage* pPage = new lmBoxPage();
    m_aPages.Add(pPage);
    return pPage;

}
