//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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

#ifndef __LM_BARLINE_H__        //to avoid nested includes
#define __LM_BARLINE_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
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

	wxString GetName() const { return _T("barline"); }

    //other methods
    lmEBarline GetBarlineType() {return m_nBarlineType;}

    //implementation of virtual methods defined in abstract base class lmStaffObj
    lmLUnits LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC);
	lmUPoint ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper);

        //specific methods for barline

    //deal with contexts array
	inline void SetContext(lmContext* pContext) { m_pContext = pContext; }
    inline lmContext* GetContext() { return m_pContext; }

    //positioning
    //void SetLocation(lmLUnits uxPos, lmELocationType nType);
    //lmLUnits GetLocationPos() { return m_uxUserPos; }
    lmELocationType GetLocationType() { return m_xUserPosType; }


    //    debugging
    wxString Dump();
    wxString SourceLDP(int nIndent);
    wxString SourceXML(int nIndent);


private:
    lmEBarline          m_nBarlineType;     //type of barline
	lmContext*			m_pContext;			//ptr to current context

    lmLUnits            m_uxUserPos;
    lmELocationType     m_xUserPosType;

};

//
// global functions related to barlines
//
wxString GetBarlineLDPNameFromType(lmEBarline nBarlineType);


#endif    // __LM_BARLINE_H__

