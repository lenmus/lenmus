// RCS-ID: $Id: Formatter3.h,v 1.3 2006/02/23 19:23:54 cecilios Exp $
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
/*! @file Formatter3.h
    @brief Header file for class lmFormatter3
    @ingroup score_kernel
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __FORMATTER3_H__        //to avoid nested includes
#define __FORMATTER3_H__

//constants to define some tables' size
//! @limit a score can not have more than 30 systems (very strong: only a couple of pages)
//! @limit a system can not have more than 30 staves
//! @limit a system can not have more than 20 measures
//! @todo Review all code to avoid limits: dynamic tables
#define MAX_STAVES_PER_SYSTEM        30    //max number of staves in a system
#define MAX_SYSTEMS                    30    //max number of systems in a score
#define MAX_MEASURES_PER_SYSTEM        20    //max number of measures in a system

#include "TimeposTable.h"

class lmFormatter3 : public lmFormatter
{
public:
    lmFormatter3();
    ~lmFormatter3();

    //implementation of base class virtual functions
    void RenderScore(lmPaper* pPaper); 
                        //bool fMetodoJustificado = true,
                        //ESpacingMethod nTipoEspaciado = esm_PropConstantShortNote,
                        //bool fJustificada = true,
                        //bool fTruncarUltimoSistema = false, 
                        //float rFactorAjuste = 1.0) = 0;

private:
    void RenderMinimal(lmPaper *pPaper);
    void RenderJustified(lmPaper* pPaper);

    lmMicrons SizeMeasureColumn(int nAbsMeasure, int nRelMeasure, int nSystem, lmPaper* pPaper);
    void RedistributeFreeSpace(lmMicrons nAvailable);
    void DrawMeasure(lmVStaff* pVStaff, int iMeasure, lmPaper* pPaper);
    void SizeMeasure(lmVStaff* pVStaff, int nAbsMeasure, int nRelMeasure, lmPaper* pPaper);


        // member variables

    //auxiliary data for computing and justifying systems.
    lmTimeposTable    m_oTimepos[MAX_MEASURES_PER_SYSTEM+1];    //timepos table for current measure column
    lmMicrons    m_nFreeSpace;                            //free space available on current system
    lmMicrons    m_nMeasureSize[MAX_STAVES_PER_SYSTEM+1];    //size of all measure columns of current system
    int        m_nMeasuresInSystem;                        //the number of measures in current system

    ////variables de dibujo que se pasan como parámetros a diversas funciones y que defino aqui para evitarlo
    //m_rFactorAjuste As Single           //factor de ajuste para espaciado proporcional
    //m_nSpacingMethod As ESpacingMethod
    //
    ////to optimize re-paints if canvas has not changed
    //m_nIdLastCanvas As Long                        //Id of canvas used last time RenderScore was invoked
    //m_nLastCanvasWidth As Long
    int            m_nNumMeasures[MAX_SYSTEMS+1];    //num of measures in each system
    lmMicrons    m_ySystemPos[MAX_SYSTEMS+1];    //paper y position at which each system starts
    int            m_nNumSystems;                    //num of systems in which the score has been splitted

    // variables for debugging
    bool        m_fDebugMode;            //debug on/off

};


#endif    // __FORMATTER3_H__



