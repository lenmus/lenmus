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

#ifndef __BARLINE_H__        //to avoid nested includes
#define __BARLINE_H__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "Barline.cpp"
#endif


#include "wx/dc.h"

//------------------------------------------------------------------------------------------------
// lmBarline object
//------------------------------------------------------------------------------------------------

class lmBarline:  public lmStaffObj
{
public:
    //constructor and destructor
    lmBarline(lmEBarline nBarlineType, lmVStaff* pStaff, bool fVisible);
    ~lmBarline() {}

    //other methods
    lmEBarline GetBarlineType() {return m_nBarlineType;}

    //implementation of virtual methods defined in abstract base class lmStaffObj
    lmLUnits LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC);
	lmLUnits ComputeXLocation(lmPaper* pPaper);
	lmLUnits ComputeYLocation(lmPaper* pPaper);

        //specific methods for barline

    //deal with contexts array
    void AddContext(lmContext* pContext, int nStaff);
    lmContext* GetContext(int nStaff);

    //positioning
    void SetLocation(lmLUnits uxPos, lmELocationType nType);
    lmLUnits GetLocationPos() { return m_uxUserPos; }
    lmELocationType GetLocationType() { return m_xUserPosType; }


    //    debugging
    wxString Dump();
    wxString SourceLDP(int nIndent);
    wxString SourceXML(int nIndent);


private:
    lmLUnits DrawBarline(bool fMeasuring, lmPaper* pPaper, lmLUnits uxPos, lmLUnits uyTop,
                         lmLUnits uyBottom, wxColour colorC = *wxBLACK);
    void DrawThinLine(lmPaper* pPaper, lmLUnits uxPos, lmLUnits uyTop, lmLUnits uyBottom,
                      wxColour color);
    void DrawThickLine(lmPaper* pPaper, lmLUnits uxLeft, lmLUnits uyTop, lmLUnits uWidth,
                       lmLUnits uHeight, wxColour color);
    void DrawTwoDots(lmPaper* pPaper, lmLUnits uxPos, lmLUnits uyPos);

private:
    lmEBarline            m_nBarlineType;     //type of barline
    ArrayOfContexts     m_aContexts;        //pointers to contexts at barline position

    lmLUnits            m_uxUserPos;
    lmELocationType     m_xUserPosType;
};

//
// global functions related to barlines
//
wxString GetBarlineLDPNameFromType(lmEBarline nBarlineType);


#endif    // __BARLINE_H__

