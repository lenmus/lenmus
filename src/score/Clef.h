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

#ifndef __LM_CLEF_H__        //to avoid nested includes
#define __LM_CLEF_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Clef.cpp"
#endif

#include "wx/dc.h"

#include "Glyph.h"

//------------------------------------------------------------------------------------------------
// lmClef object
//------------------------------------------------------------------------------------------------

class lmClef: public lmStaffObj
{
public:
    //constructor and destructor
    lmClef(lmEClefType nClefType, lmVStaff* pStaff, int nNumStaff=1, bool fVisible=true,
		   wxColour colorC = *wxBLACK);
    ~lmClef() {}

	wxString GetName() const { return _T("clef"); }

    //other methods
    lmEClefType GetClefType() {return m_nClefType;}

    //implementation of virtual methods defined in abstract base class lmStaffObj
    lmLUnits LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC);
	lmUPoint ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper);


    //debugging
    wxString Dump();
    wxString SourceLDP(int nIndent);
    wxString SourceXML(int nIndent);

    //rendering related methods
	lmShape* AddShape(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos,
					  wxColour colorC = *wxBLACK);

    //methods for hiding the clef in prologs
    void Hide(bool fHide) { m_fHidden = fHide; }

	//context management
	inline void SetContext(lmContext* pContext) { m_pContext = pContext; }
    inline lmContext* GetContext() { return m_pContext; }
    void RemoveCreatedContexts();


private:

    // get fixed measures and values that depend on key type
    lmTenths GetGlyphOffset();
    lmEGlyphIndex GetGlyphIndex();

    //variables
    lmEClefType		m_nClefType;        //type of clef
    bool            m_fHidden;          //to hide it in system prolog
	wxColour		m_color;			//clef color
	lmContext*		m_pContext;			//context created by this clef

};

//
// global functions related to clefs
//
wxString GetClefLDPNameFromType(lmEClefType nType);

#endif    // __LM_CLEF_H__

