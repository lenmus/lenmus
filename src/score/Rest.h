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

#ifndef __LM_REST_H__        //to avoid nested includes
#define __LM_REST_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Rest.cpp"
#endif

#include "Glyph.h"


class lmRest: public lmNoteRest
{
public:
    lmRest(lmVStaff* pVStaff, 
        lmENoteType nNoteType, float rDuration,
        int nNumDots, int nStaff, int nVoice, bool fVisible,
        bool fBeamed, lmTBeamInfo BeamInfo[]);

    ~lmRest();

    //implementation of virtual methods of base classes
        // lmStaffObj
    lmLUnits LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC);
	lmUPoint ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper);
	wxString GetName() const { return _T("rest"); }


	wxString    Dump();
    wxString    SourceLDP(int nIndent);
    wxString    SourceXML(int nIndent);


private:
    // access to glyph data to define character to use and selection rectangle 
    lmEGlyphIndex GetGlyphIndex();
    lmLUnits GetDotShift();

};


#endif    // __LM_REST_H__
