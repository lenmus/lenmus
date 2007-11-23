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

#ifndef __METRONOMEMARK_H__        //to avoid nested includes
#define __METRONOMEMARK_H__

#if defined(__GNUG__) && !defined(__APPLE__)
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
                    ENoteType nNoteType,
                    int nDots,
                    int nTicksPerMinute,
                    bool fParentheses = false,
                    bool fVisible = true);
    lmMetronomeMark(lmVStaff* pVStaff,          // 'm.m. = 80'
                    int nTicksPerMinute,
                    bool fParentheses = false,
                    bool fVisible = true);
    lmMetronomeMark(lmVStaff* pVStaff,          // 'note_symbol = note_symbol'
                    ENoteType nLeftNoteType,
                    int nLeftDots,
                    ENoteType nRightNoteType,
                    int nRightDots,
                    bool fParentheses = false,
                    bool fVisible = true);
    ~lmMetronomeMark();

    // properties related to the clasification of this lmStaffObj
    float GetTimePosIncrement() { return 0; }

    // implementation of virtual methods defined in abstract base class lmStaffObj
    void LayoutObject(lmBox* pBox, lmPaper* pPaper, wxColour colorC);
    wxBitmap* GetBitmap(double rScale);

    // debugging
    wxString Dump();
    wxString SourceLDP(int nIndent);
    wxString SourceXML(int nIndent);

    //specific methods of this object


private:
    wxString GetLDPNote(ENoteType nNoteType, int nDots);
    lmLUnits DrawMetronomeMark(bool fMeasuring, lmPaper* pPaper,
                               lmLUnits uxPos, lmLUnits uyPos, wxColour colorC = *wxBLACK);
    lmLUnits DrawText(bool fMeasuring, lmPaper* pPaper,
                      lmLUnits uxPos, lmLUnits uyPos, wxColour colorC);
    lmLUnits DrawSymbol(bool fMeasuring, lmPaper* pPaper, lmShapeGlyph* pShape,
                        lmLUnits uxPos, lmLUnits uyPos, wxColour colorC);
    lmEGlyphIndex SelectGlyph(ENoteType nNoteType, int nDots);

    EMetronomeMarkType  m_nMarkType;
    ENoteType           m_nLeftNoteType;
    int                 m_nLeftDots;
    ENoteType           m_nRightNoteType;
    int                 m_nRightDots;
    int                 m_nTicksPerMinute;
    bool                m_fParentheses;         // enclose metronome mark between parentheses

    //shapes for rendering note symbols
    lmShapeGlyph*       m_pLeftNoteShape;
    lmShapeGlyph*       m_pRightNoteShape;
    lmShapeText*        m_pTextShape;


};

#endif    // __METRONOMEMARK_H__

