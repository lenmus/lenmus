// RCS-ID: $Id: Barline.h,v 1.3 2006/02/23 19:22:56 cecilios Exp $
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
    lmBarline(ETipoBarra nBarlineType, lmVStaff* pStaff, bool fVisible);
    ~lmBarline() {}

    //other methods
    ETipoBarra GetType() {return m_nBarlineType;}

    //implementation of virtual methods defined in abstract base class lmStaffObj
    void DrawObject(bool fMeasuring, lmPaper* pPaper, wxColour colorC);
    wxBitmap* GetBitmap(double rScale);
    void MoveDragImage(lmPaper* pPaper, wxDragImage* pDragImage, wxPoint& ptOffset, 
                         const wxPoint& ptLog, const wxPoint& dragStartPosL, const wxPoint& ptPixels);
    wxPoint EndDrag(const wxPoint& pos);


    //    debugging
    wxString Dump();
    wxString SourceLDP();
    wxString SourceXML();


private:
    lmMicrons DrawBarline(bool fMeasuring, wxDC* pDC, lmMicrons xPos, lmMicrons yTop,
                          lmMicrons yBottom, wxColour colorC = *wxBLACK);
    void DrawThinLine(wxDC* pDC, lmMicrons xPos, lmMicrons yTop, lmMicrons yBottom);
    void DrawThickLine(wxDC* pDC, lmMicrons xLeft, lmMicrons yTop, lmMicrons nWidth, lmMicrons nHeight);
    void DrawTwoDots(wxDC* pDC, lmMicrons xPos, lmMicrons yPos);

private:
    ETipoBarra        m_nBarlineType;        //type of barline

};

//
// global functions related to barlines
//
wxString GetBarlineLDPNameFromType(ETipoBarra nBarlineType);


#endif    // __BARLINE_H__

