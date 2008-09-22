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

#ifndef __LM_METRONOMEMARK_H__        //to avoid nested includes
#define __LM_METRONOMEMARK_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "MetronomeMark.cpp"
#endif

#include "Glyph.h"
class lmShapeGlyph;

// type of printed metronome mark
enum EMetronomeMarkType
{
    eMMT_MM_Value = 0,      //(metronome 80)   -->  'm.m. = 80'
    eMMT_Note_Note,         //(metronome n n.) -->  'note_symbol = note_symbol'
    eMMT_Note_Value,        //(metronome n 80) -->  'note_symbol = 80'
};

class lmMetronomeMark : public lmStaffObj
{
public:
    // constructors / destructor
    lmMetronomeMark(lmVStaff* pVStaff,          // 'note_symbol = 80'
                    lmENoteType nNoteType,
                    int nDots,
                    int nTicksPerMinute,
                    bool fParentheses = false,
                    bool fVisible = true);
    lmMetronomeMark(lmVStaff* pVStaff,          // 'm.m. = 80'
                    int nTicksPerMinute,
                    bool fParentheses = false,
                    bool fVisible = true);
    lmMetronomeMark(lmVStaff* pVStaff,          // 'note_symbol = note_symbol'
                    lmENoteType nLeftNoteType,
                    int nLeftDots,
                    lmENoteType nRightNoteType,
                    int nRightDots,
                    bool fParentheses = false,
                    bool fVisible = true);
    ~lmMetronomeMark();

	wxString GetName() const { return _T("metronome mark"); }

    // properties related to the clasification of this lmStaffObj
    float GetTimePosIncrement() { return 0; }

    // implementation of virtual methods defined in abstract base class lmStaffObj
    lmLUnits LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC);
	lmUPoint ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper);

    // debugging
    wxString Dump();
    wxString SourceLDP(int nIndent);
    wxString SourceXML(int nIndent);

    //specific methods of this object


private:
    wxString GetLDPNote(lmENoteType nNoteType, int nDots);
    lmEGlyphIndex SelectGlyph(lmENoteType nNoteType, int nDots);

	lmLUnits AddSymbolShape(lmCompositeShape* pShape, lmPaper* pPaper, lmEGlyphIndex nGlyph,
							wxFont* pFont, lmUPoint uPos, wxColour colorC = *wxBLACK);
	lmLUnits AddTextShape(lmCompositeShape* pShape, lmPaper* pPaper,
							wxString sText, lmUPoint uPos, wxColour colorC = *wxBLACK);


    EMetronomeMarkType  m_nMarkType;
    lmENoteType			m_nLeftNoteType;
    int                 m_nLeftDots;
    lmENoteType			m_nRightNoteType;
    int                 m_nRightDots;
    int                 m_nTicksPerMinute;
    bool                m_fParentheses;         // enclose metronome mark between parentheses

};

#endif    // __LM_METRONOMEMARK_H__

