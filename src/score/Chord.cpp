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
/*! @file Chord.cpp
    @brief Implementation file for class lmChord
    @ingroup score_kernel
*/
/*! @class lmChord 
    @ingroup score_kernel
    @brief Information and methods to group several notes into a chord.

    Contains the information needed to group several notes into a chord.
    For each chord there exist a lmChord object and a pointer to it is stored on each
    note of the chord

    The class is named lmChord instead of Chord because there is a function named "Chord"
    defined in WinGDI.h (Microsoft), and it causes compilation problems (with MS VC) 
    as the names collide.

*/

#ifdef __GNUG__
// #pragma implementation
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Score.h"
#include "Glyph.h"


/*! Creates the chord object, with only the base note.
*/
lmChord::lmChord(lmNote* pBaseNote)
{
    m_cNotes.Append(pBaseNote);

    // as this is the only note it is the max and the min one
    m_pMinNote = pBaseNote;
    m_pMaxNote = pBaseNote;
}

/*! Destructor. When invoked, only from lmNote destructor, there must be only one 
    note: the base note.
*/
lmChord::~lmChord()
{
    wxASSERT(m_cNotes.GetCount() == 1);

    wxNotesListNode *pNode = m_cNotes.GetFirst();
    lmNote* pNote = (lmNote*)pNode->GetData();
    pNote->ClearChordInformation();

    m_cNotes.DeleteObject(pNote);

    //notes will not be deleted when deleting the list, as they are part of a lmScore
    // and will be deled there.
    m_cNotes.DeleteContents(false);
    m_cNotes.Clear();
}

/*!  @brief Add a note to the chord.
*/
void lmChord::AddNote(lmNote* pNote)
{
    m_cNotes.Append(pNote);
    
    //Update Max and Min note 
    if (m_pMinNote->GetPitch() > pNote->GetPitch()) {
        m_pMinNote = pNote;
    } else if (m_pMaxNote->GetPitch() < pNote->GetPitch()) {
        m_pMaxNote = pNote;
    }

    //compute stem direction
    ComputeStemDirection();

}

/*! @brief Removes a note from a chord.
    
    When invoked there must be at least two notes as 
    otherwise it can not be a chord. 
*/
void lmChord::RemoveNote(lmNote* pNote)
{
    wxASSERT(m_cNotes.GetCount() > 1);

    m_cNotes.DeleteObject(pNote);

    //Update Max and Min note 
    wxNotesListNode *pNode = m_cNotes.GetFirst();
    pNote = (lmNote*)pNode->GetData();
    m_pMinNote = pNote;
    m_pMaxNote = pNote;

    pNode=pNode->GetNext();
    for(; pNode; pNode=pNode->GetNext()) 
    {
        pNote = (lmNote*)pNode->GetData();
        if (m_pMinNote->GetPitch() > pNote->GetPitch()) {
            m_pMinNote = pNote;
        } else if (m_pMaxNote->GetPitch() < pNote->GetPitch()) {
            m_pMaxNote = pNote;
        }
    }

}

lmNote* lmChord::GetBaseNote()
{
    return (lmNote*)(m_cNotes.GetFirst())->GetData();
}

int lmChord::GetNumNotes()
{ 
    return (int)m_cNotes.GetCount(); 
}

/*!    @brief Draw the stem of the chord.

    Once the last notehead of a chord has been drawn this method is invoked (from the
    last lines of lmNote::DrawObject() ) to draw the stem of the chord.
    The stem position is stored in the base note.

*/
void lmChord::DrawStem(bool fMeasuring, wxDC* pDC, wxColour colorC, wxFont* pFont,
                       lmVStaff* pVStaff, int nStaff)
{
    wxASSERT(pDC);

    lmNote* pBaseNote = (lmNote*)(m_cNotes.GetFirst())->GetData();
    lmLUnits xStem = pBaseNote->GetXStem();
    lmLUnits yStemStart=0, yStemEnd=0;

    #define TWO_NOTES_DEFAULT true          //! @todo move to layout user options


    if (!pBaseNote->IsBeamed()) {
        //compute y positions
        if (m_fStemDown) {
            yStemStart = m_pMaxNote->GetYStem();
            yStemEnd = m_pMinNote->GetFinalYStem();
        }
        else {
            yStemStart = m_pMinNote->GetYStem();
            yStemEnd = m_pMaxNote->GetFinalYStem();
        }

    }
    else {
        if (pBaseNote->StemGoesDown()) {
            //stem down: line at left of noteheads
            if (pBaseNote->IsBeamed()) {
                // chord beamed: use base note information
                yStemStart = m_pMaxNote->GetYStem();
                yStemEnd = yStemStart + pBaseNote->GetStemLength();
            }
            else {
                // chord not beamed. Use max and min notes information
                yStemStart = m_pMaxNote->GetYStem();
                yStemEnd = m_pMinNote->GetFinalYStem();
            }
        } else {
            //stem up: line at right of noteheads
            if (pBaseNote->IsBeamed()) {
                // chord beamed: use base note information
                yStemStart = m_pMinNote->GetYStem();
                yStemEnd = yStemStart - pBaseNote->GetStemLength();
            }
            else {
                // chord not beamed. Use max and min notes information
                yStemStart = m_pMinNote->GetYStem();
                yStemEnd = m_pMaxNote->GetFinalYStem();
            }
        }
    }

    pDC->DrawLine(xStem, yStemStart, xStem, yStemEnd);

    //draw the flag for chords not beamed
    if (!pBaseNote->IsBeamed() && pBaseNote->GetNoteType() > eQuarter) {
        DrawFlag(fMeasuring, pDC, pBaseNote, wxPoint(xStem, yStemEnd), colorC, pFont,
                 pVStaff, nStaff);
    }
    
}

bool lmChord::IsLastNoteOfChord(lmNote* pNote)
{
    wxNotesListNode *pNode = m_cNotes.GetLast();
    lmNote* pLastNote = (lmNote*)pNode->GetData();
    return (pLastNote->GetID() == pNote->GetID());
    
}

lmLUnits lmChord::DrawFlag(bool fMeasuring, wxDC* pDC, lmNote* pBaseNote,
                                 wxPoint pos, wxColour colorC, wxFont* pFont,
                                 lmVStaff* pVStaff, int nStaff)
{
    //
    //Draws the flag using a glyph. Returns the flag width
    //

    ENoteType nNoteType = pBaseNote->GetNoteType();
    bool fStemDown = pBaseNote->StemGoesDown(); 
        
    lmEGlyphIndex nGlyph = GLYPH_EIGHTH_FLAG_DOWN;
    switch (nNoteType) {
        case eEighth :
            nGlyph = (fStemDown ? GLYPH_EIGHTH_FLAG_DOWN : GLYPH_EIGHTH_FLAG_UP);
            break;
        case e16th :
            nGlyph = (fStemDown ? GLYPH_16TH_FLAG_DOWN : GLYPH_16TH_FLAG_UP);
            break;
        case e32th :
            nGlyph = (fStemDown ? GLYPH_32ND_FLAG_DOWN : GLYPH_32ND_FLAG_UP);
            break;
        case e64th :
            nGlyph = (fStemDown ? GLYPH_64TH_FLAG_DOWN : GLYPH_64TH_FLAG_UP);
            break;
        case e128th :
            nGlyph = (fStemDown ? GLYPH_128TH_FLAG_DOWN : GLYPH_128TH_FLAG_UP);
            break;
        case e256th :
            nGlyph = (fStemDown ? GLYPH_256TH_FLAG_DOWN : GLYPH_256TH_FLAG_UP);
            break;
        default:
            wxLogMessage(_T("[lmChord::DrawFlag] Error: invalid note type %d."),
                        nNoteType);
        }

    wxString sGlyph( aGlyphsInfo[nGlyph].GlyphChar );
  
    pDC->SetFont(*pFont);
    if (!fMeasuring) {
        // drawing phase: do the draw
        pDC->SetTextForeground(colorC);
        pDC->DrawText(sGlyph, pos.x, 
            pos.y + pVStaff->TenthsToLogical( aGlyphsInfo[nGlyph].GlyphOffset, nStaff ) );
    }

    lmLUnits width, height;
    pDC->GetTextExtent(sGlyph, &width, &height);
    return width;
        
}

void lmChord::ComputeStemDirection()
{
    //  Rules (taken from www.coloradocollege.edu/dept/mu/mu2/musicpress/NotesStems.html
    //
    //  a) Two Notes on a stem:
    //    a1. If the interval above the middle line is greater than the interval below the
    //      middle line: downward stems. i.e. (a4,d5) (f4,f5) (a4,g5)
    //      ==>   (MaxNotePos + MinNotePos)/2 > MiddleLinePos
    //
    //    a2. If the interval below the middle line is greater than the interval above the
    //      middle line: upward stems. i.e. (e4,c5)(g4,c5)(d4,e5)
    //
    //    a3. If the two notes are the same distance from the middle line: stem can go in
    //      either direction, but most engravers prefer downward stems. i.e. (g4.d5)(a4,c5)
    //
    //
    //  b) More than two notes on a stem:
    //
    //    b1. If the interval of the highest note above the middle line is greater than the
    //      interval of the lowest note below the middle line: downward stems. 
    //      ==>   same than a1
    //
    //    b2. If the interval of the lowest note below the middle line is greater than the
    //      interval of the highest note above the middle line: upward stems. 
    //      ==>   same than a2
    //
    //    b3. If the highest and the lowest notes are the same distance from the middle line
    //      use the majority rule to determine stem direction: If the majority of the notes
    //      are above the middle: downward stems. Else: upward stems. 
    //      ==>   Mean(NotePos) > MiddleLinePos -> downward


    if (m_cNotes.GetCount() < 2) return;

    lmNote* pBaseNote = (lmNote*)(m_cNotes.GetFirst())->GetData();

    #define TWO_NOTES_DEFAULT true          //! @todo move to layout user options

    m_fStemDown = pBaseNote->StemGoesDown();     //defaul value

    //Rules
    int nWeight = m_pMinNote->GetPosOnStaff() + m_pMaxNote->GetPosOnStaff();
    if (nWeight > 12)
        m_fStemDown = true;
    else if (nWeight < 12)
        m_fStemDown = false;
    else {
        //majority rule if more than two notes. Else default for two notes case
        m_fStemDown = TWO_NOTES_DEFAULT;
        if (m_cNotes.GetCount() > 2) {
            int iN;
            nWeight = 0;
            lmNote* pNote;
            wxNotesListNode *pNode = m_cNotes.GetFirst();
            for(iN=0; pNode; pNode=pNode->GetNext(), iN++) {
                pNote = (lmNote*)pNode->GetData();
                nWeight += pNote->GetPosOnStaff();
            }
            m_fStemDown = (nWeight >= 6*iN);
        }
    }

    //update max and min notes with conclusion about stem direction
    m_pMinNote->SetStemDirection(m_fStemDown);
    m_pMaxNote->SetStemDirection(m_fStemDown);
    pBaseNote->SetStemDirection(m_fStemDown);

}
