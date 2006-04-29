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
/*! @file GraphicManager.cpp
    @brief Implementation file for class lmGraphicManager
    @ingroup graphic_management
*/
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "GraphicManager.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "GraphicManager.h"
#include "Formatter4.h"

//access to colors
#include "../globals/Colors.h"
extern lmColors* g_pColors;

//-----------------------------------------------------------------------------------------

lmGraphicManager::lmGraphicManager(lmScore* pScore, lmPaper* pPaper)
{
    m_pScore = pScore;
    m_pPaper = pPaper;

    //initializations
    m_pBoxScore = (lmBoxScore*) NULL;

}


lmGraphicManager::~lmGraphicManager()
{
    if (m_pBoxScore) {
        delete m_pBoxScore;
        m_pBoxScore = (lmBoxScore*) NULL;
    }

}

void lmGraphicManager::Layout()
{
    /*
        The goal of this method is to parse the IIR representation of a score and
        generate the graphical representation so that it can be displayed or printed. 
        The result is a lmBoxScore object.
    */
    if (m_pBoxScore) {
        delete m_pBoxScore;
        m_pBoxScore = (lmBoxScore*) NULL;
    }
    lmFormatter4 oFormatter;   //the formatter object
    m_pBoxScore = oFormatter.Layout(m_pScore, m_pPaper);

}

void lmGraphicManager::Render(lmPaper* pPaper)
{
    m_pBoxScore->Render(pPaper);

}



