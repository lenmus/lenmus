// RCS-ID: $Id: Formatter.h,v 1.3 2006/02/23 19:22:56 cecilios Exp $
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
/*! @file Formatter.h
    @brief Header file for class lmFormatter
    @ingroup score_kernel
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __FORMATTER_H__        //to avoid nested includes
#define __FORMATTER_H__

//'Spacing methods for rendering scores
//'-------------------------------------
//'Two basic methods:
//'   1. Fixed: the spacing between notes is constant, independently of note duration.
//'   2. Proportional: the spacing is adjusted so that note position is proportional to time.
//'
//'In the proportional method several alternatives are posible:
//'   1. ProportionalConstant: the proportion factor between time and space is fixed. Two alternative
//'       methods for fixing this factor:
//'       a) Fixed: is given by the vaule of a parameter
//'       b) ShortNote: is computed for the shorter note in the score
//'   2. ProportionalVariable: the proportion factor is computed for each bar. Two alternatives:
//'       a) ShortNote: is computed for the shorter note in the bar
//'       b) NumBars: Computed so taht the number of bars in the system is a predefined number
//-------------------------------------------------------------------------------------------------
enum ESpacingMethod
{
    esm_Fixed = 0,
    esm_PropConstantFixed,            //Used in FTeoria
    esm_PropConstantShortNote,        //Default method in lmFormatter.RenderScore(). Not used
    esm_PropVariableShortNote,        //Not used/implemented
    esm_PropVariableNumBars            //Not used/implemented
};


class lmFormatter
{
public:
    lmFormatter();
    virtual ~lmFormatter() {};

    //identify the score to be rendered
    void SetScore(lmScore* pScore) { m_pScore = pScore; };

    //commnad to render the score
    virtual void RenderScore(lmPaper* pPaper) = 0; 
                        //bool fMetodoJustificado = true,
                        //ESpacingMethod nTipoEspaciado = esm_PropConstantShortNote,
                        //bool fJustificada = true,
                        //bool fTruncarUltimoSistema = false, 
                        //float rFactorAjuste = 1.0) = 0;

protected:
    lmScore*        m_pScore;        //the score to be rendered

};


#endif    // __FORMATTER_H__



