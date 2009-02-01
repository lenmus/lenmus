//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
//
//    This program is free software; you can redistribute it and/or modify it under the
//    terms of the GNU General Public License as published by the Free Software Foundation,
//    either version 3 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this
//    program. If not, see <http://www.gnu.org/licenses/>.
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

#include <vector>

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
    ~lmClef();

	wxString GetName() const { return _T("clef"); }

    //other methods
    lmEClefType GetClefType() {return m_nClefType;}

    //implementation of virtual methods defined in abstract base class lmStaffObj
    lmLUnits LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC);
    lmLUnits LayoutObjectAsInvisible(lmBox* pBox, lmUPoint uPos);
	lmUPoint ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper);

    //debugging
    wxString Dump();
    wxString SourceLDP(int nIndent);
    wxString SourceXML(int nIndent);

    //rendering related methods
    inline lmShape* GetShape(int nStaff=1) { return lmScoreObj::GetShape(1); }
    lmShape* CreateShape(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos,
					     wxColour colorC = *wxBLACK, bool fSmallClef = false);

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
	lmContext*		m_pContext;			//context created by this clef

};

//
// global functions related to clefs
//
wxString GetClefLDPNameFromType(lmEClefType nType);

#endif    // __LM_CLEF_H__

