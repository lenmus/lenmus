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
/*! @file Barline.h
    @brief Header file for class lmBarline
    @ingroup score_kernel
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __BARLINE_H__        //to avoid nested includes
#define __BARLINE_H__

#include "wx/dc.h"

//------------------------------------------------------------------------------------------------
// lmBarline object
//------------------------------------------------------------------------------------------------

class lmBarline:  public lmSimpleObj
{
public:
    //constructor and destructor
    lmBarline(EBarline nBarlineType, lmVStaff* pStaff, bool fVisible);
    ~lmBarline() {}

    //other methods
    EBarline GetType() {return m_nBarlineType;}

    //implementation of virtual methods defined in abstract base class lmStaffObj
    void DrawObject(bool fMeasuring, lmPaper* pPaper, wxColour colorC, bool fHighlight);
    wxBitmap* GetBitmap(double rScale);
    void MoveDragImage(lmPaper* pPaper, wxDragImage* pDragImage, lmDPoint& ptOffset, 
                         const lmUPoint& ptLog, const lmUPoint& dragStartPosL, const lmDPoint& ptPixels);
    lmUPoint EndDrag(const lmUPoint& pos);

        //specific methods for barline

    //deal with contexts array
    void AddContext(lmContext* pContext, int nStaff);
    lmContext* GetContext(int nStaff);


    //    debugging
    wxString Dump();
    wxString SourceLDP();
    wxString SourceXML();


private:
    lmLUnits DrawBarline(bool fMeasuring, lmPaper* pPaper, lmLUnits xPos, lmLUnits yTop,
                         lmLUnits yBottom, wxColour colorC = *wxBLACK);
    void DrawThinLine(lmPaper* pPaper, lmLUnits xPos, lmLUnits yTop, lmLUnits yBottom,
                      wxColour color);
    void DrawThickLine(lmPaper* pPaper, lmLUnits xLeft, lmLUnits yTop, lmLUnits nWidth,
                       lmLUnits nHeight, wxColour color);
    void DrawTwoDots(lmPaper* pPaper, lmLUnits xPos, lmLUnits yPos);

private:
    EBarline            m_nBarlineType;     //type of barline
    ArrayOfContexts     m_aContexts;        //pointers to contexts at barline position
};

//
// global functions related to barlines
//
wxString GetBarlineLDPNameFromType(EBarline nBarlineType);


#endif    // __BARLINE_H__

