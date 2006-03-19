// RCS-ID: $Id: Clef.h,v 1.3 2006/02/23 19:22:56 cecilios Exp $
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
/*! @file Clef.h
    @brief Header file for class lmClef
    @ingroup score_kernel
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __CLEF_H__        //to avoid nested includes
#define __CLEF_H__

#include "wx/dc.h"

//------------------------------------------------------------------------------------------------
// lmClef object
//------------------------------------------------------------------------------------------------

class lmClef: public lmSimpleObj
{
    //// attributes
private:
    EClefType        m_nClefType;        //type of clef

public:

    //// methods

public:
    //constructor and destructor
    lmClef(EClefType nClefType, lmVStaff* pStaff, wxInt32 nNumStaff=1, bool fVisible=true);
    ~lmClef() {}

    //other methods
    EClefType GetType() {return m_nClefType;}

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

    //rendering related methods
    lmMicrons DrawAt(bool fMeasuring, wxDC* pDC, wxPoint pos, wxColour colorC = *wxBLACK);



private:
    // get fixed measures and values that depend on key type
    lmTenths GetSelRectHeight();
    lmTenths GetSelRectShift();
    lmTenths GetGlyphOffset();
    wxString GetLenMusChar();
    lmMicrons DrawClef(bool fMeasuring, lmPaper* pPaper, wxColour colorC = *wxBLACK);

};

//
// global functions related to clefs
//
wxString GetClefLDPNameFromType(EClefType nType);

#endif    // __CLEF_H__
