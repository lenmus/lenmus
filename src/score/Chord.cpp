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

int GlobalPitchCompare(const void* pNote1, const void* pNote2)
{
    if ((*(lmNote**)pNote1)->StemGoesDown())
        return ((*(lmNote**)pNote1)->GetPitch() < (*(lmNote**)pNote2)->GetPitch());
    else
        return ((*(lmNote**)pNote1)->GetPitch() > (*(lmNote**)pNote2)->GetPitch());
}


/*! Creates the chord object, with only the base note.
*/
lmChord::lmChord(lmNote* pBaseNote)
{
    m_cNotes.Append(pBaseNote);

    // as this is the only note it is the max and the min one
    m_pBaseNote = pBaseNote;
    m_pMinNote = pBaseNote;
    m_pMaxNote = pBaseNote;

    m_nStemType = pBaseNote->GetStemType();
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

wxString lmChord::Dump()
{
    wxString sDump = wxString::Format(_T("Chord: num.notes=%d ("), m_cNotes.GetCount());

    int iPos;
    wxNotesListNode *pNode = m_cNotes.GetFirst();
    lmNote* pNote;
    for(iPos=0; pNode; pNode=pNode->GetNext(), iPos++) {
        pNote = (lmNote*)pNode->GetData();
        sDump += wxString::Format(_T("%d,") ,pNote->GetPitch() );
    }
    sDump += wxString::Format(_T("), max=%d, min=%d, base=%d, stem="),
                m_pMaxNote->GetPitch(), m_pMinNote->GetPitch(), m_pBaseNote->GetPitch() );
    sDump += (m_fStemDown ? _T("down") : _T("up"));

    return sDump;
}

/*!  @brief Add a note to the chord.
*/
void lmChord::AddNote(lmNote* pNewNote)
{
    //Notes will be kept ordered by pitch. First item lowest pitch
    //When this method is invoked at least the base note is in the collection
    wxASSERT(m_cNotes.GetCount() > 0);

    int iPos;
    wxNotesListNode *pNode = m_cNotes.GetFirst();
    lmNote* pNote;
    for(iPos=0; pNode; pNode=pNode->GetNext(), iPos++) {
        pNote = (lmNote*)pNode->GetData();
        if (pNote->GetPitch() > pNewNote->GetPitch()) break;
    }
    m_cNotes.Insert((size_t)iPos, pNewNote);
    
    //Update Max and Min note 
    if (m_pMinNote->GetPitch() > pNewNote->GetPitch()) {
        m_pMinNote = pNewNote;
    } else if (m_pMaxNote->GetPitch() < pNewNote->GetPitch()) {
        m_pMaxNote = pNewNote;
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
    return m_pBaseNote;
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
void lmChord::DrawStem(bool fMeasuring, lmPaper* pPaper, wxColour colorC, wxFont* pFont,
                       lmVStaff* pVStaff, int nStaff)
{
    lmNote* pBaseNote = GetBaseNote();
    lmLUnits xStem = pBaseNote->GetXStemLeft();
    lmLUnits yStemStart=0, yStemEnd=0;

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
        // If the chord is beamed, the stem length was computed during beam computation and
        // stored in the base note
        if (pBaseNote->StemGoesDown()) {
            //stem down: line at left of noteheads
            yStemStart = m_pMaxNote->GetYStem();
            yStemEnd = m_pMinNote->GetYStem() + pBaseNote->GetStemLength();
        } else {
            //stem up: line at right of noteheads
            yStemStart = m_pMinNote->GetYStem();
            yStemEnd = m_pMaxNote->GetYStem() - pBaseNote->GetStemLength();
        }
    }

    if (!fMeasuring) {
        #define STEM_WIDTH   12     //stem line width (cents = tenths x10)
        lmLUnits uStemThickness = pVStaff->TenthsToLogical(STEM_WIDTH, nStaff) / 10;
        pPaper->SolidLine(xStem, yStemStart, xStem, yStemEnd, uStemThickness,
                           eEdgeNormal, colorC);
    }

    //draw the flag for chords not beamed
    if (!pBaseNote->IsBeamed() && pBaseNote->GetNoteType() > eQuarter) {
        DrawFlag(fMeasuring, pPaper, pBaseNote, lmUPoint(xStem, yStemEnd), colorC, pFont,
                 pVStaff, nStaff);
    }
    
}

bool lmChord::IsLastNoteOfChord(lmNote* pNote)
{
    wxNotesListNode *pNode = m_cNotes.GetLast();
    lmNote* pLastNote = (lmNote*)pNode->GetData();
    return (pLastNote->GetID() == pNote->GetID());
    
}

lmLUnits lmChord::DrawFlag(bool fMeasuring, lmPaper* pPaper, lmNote* pBaseNote,
                                 lmUPoint pos, wxColour colorC, wxFont* pFont,
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
            //wxLogMessage(_T("[lmChord::DrawFlag] Error: invalid note type %d."),
            //            nNoteType);
            wxASSERT(false);
        }

    wxString sGlyph( aGlyphsInfo[nGlyph].GlyphChar );
  
    pPaper->SetFont(*pFont);
    if (!fMeasuring) {
        // drawing phase: do the draw
        pPaper->SetTextForeground(colorC);
        pPaper->DrawText(sGlyph, pos.x, 
            pos.y + pVStaff->TenthsToLogical( aGlyphsInfo[nGlyph].GlyphOffset, nStaff ) );
    }

    lmLUnits width, height;
    pPaper->GetTextExtent(sGlyph, &width, &height);
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

    lmNote* pBaseNote = GetBaseNote();

    #define TWO_NOTES_DEFAULT true          //! @todo move to layout user options
    
    if (m_nStemType == eStemUp) {          //force stem up
        m_fStemDown = false;
    }
    else if (m_nStemType == eStemDown) {   //force stem down
        m_fStemDown = true;
    }
    else if (m_nStemType == eStemNone) {   //force no stem
        m_fStemDown = false;
    }
    else if (m_nStemType == eDefaultStem) {    //as decided by program
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
    }

    //update max and min notes with conclusion about stem direction.
    //@aware for chords, setting the base note forces to call lmChord::SetStemDirection()
    //  to setup also max and min notes
    pBaseNote->SetStemDirection(m_fStemDown);

}

void lmChord::SetStemDirection(bool fStemDown)
{
    m_fStemDown = fStemDown;
    lmNote* pBaseNote = GetBaseNote();
    if (pBaseNote != m_pMinNote)    //check to avoid infinite loops
        m_pMinNote->SetStemDirection(m_fStemDown);
    if (pBaseNote != m_pMaxNote)    //check to avoid infinite loops
        m_pMaxNote->SetStemDirection(m_fStemDown);

}

void lmChord::ArrangeNoteheads()
{
    //arrange noteheads at left/right of stem to avoid collisions
    //This method asumes that the stem direction has been computed

    if (m_cNotes.GetCount() < 2) return;

    //arrange notes by pitch
    //NotesList cNotes = m_cNotes;
    //cNotes.Sort(GlobalPitchCompare);

    bool fSomeReversed = false;
    int nPosPrev = 1000;    // a very high number not posible in real world
    int nPos;
    lmNote* pNote;
    wxNotesListNode *pNode = (m_fStemDown ? m_cNotes.GetLast() : m_cNotes.GetFirst());
    for(; pNode; pNode=(m_fStemDown ? pNode->GetPrevious() : pNode->GetNext()) ) {
        pNote = (lmNote*)pNode->GetData();
        nPos = pNote->GetPosOnStaff();
        if (abs(nPosPrev - nPos) < 2) {
            //collision. Reverse position of this notehead
            fSomeReversed = true;
            pNote->SetNoteheadReversed(true);
            nPosPrev = 1000;
        }
        else {
            pNote->SetNoteheadReversed(false);
            nPosPrev = nPos;
        }
    }

    return;


}

void lmChord::ComputeLayout(lmPaper* pPaper, lmUPoint paperPos, wxColour colorC)
{
    //arrange noteheads at left/right of stem to avoid collisions
    ArrangeNoteheads();

	//Loop to compute position for accidentals:
	//- Set x pos to start of chord x pos
	//- Render accidental. If collision with other accidental:
	//	    Do while collision:
	//		    set x pos after collisioning accidental
	//		    Render accidental
	//		    test if collision
	//	    end do

    int iN;
    lmLUnits xPos, yPos;
    lmLUnits yStaffTopLine;
    lmAccidental* pAccidental;
    lmNote* pNote;
    //first loop to process notes not shitfted to right
    wxNotesListNode* pNode = m_cNotes.GetFirst();
    //wxLogMessage(_T("[lmChord::ComputeLayout] First loop to process notes not shitfted to right"));
    for(iN=1; pNode; pNode=pNode->GetNext(), iN++ ) {
        pNote = (lmNote*)pNode->GetData();
        ///@aware The font to render the note and its accidentals is set in method
        ///     lmStaffObj::Draw() before invoking lmNote->DrawObject(). To compute
        ///     the chord layout it is necessary to have the font set, so I force
        ///     to set it in next sentence.
        pNote->SetFont(pPaper);

        if (!pNote->IsNoteheadReversed() && pNote->HasAccidentals())
            ComputeAccidentalLayout(true, pNote, iN, pPaper, paperPos, colorC);
    }
    //second loop to process notes  shitfted to right
    //wxLogMessage(_T("[lmChord::ComputeLayout] Second loop to process notes  shitfted to right"));
    pNode = m_cNotes.GetFirst();
    for(iN=1; pNode; pNode=pNode->GetNext(), iN++ ) {
        pNote = (lmNote*)pNode->GetData();
        if (pNote->IsNoteheadReversed() && pNote->HasAccidentals())
            ComputeAccidentalLayout(false, pNote, iN, pPaper, paperPos, colorC);
    }

    //Here all accidentals are positioned without collisions. Procceed to compute
    //noteheads positions

    //Loop to compute noteheads' position:
	//- Set x pos to start of chord x pos + note's accidental width (if exists)
	//- Render notehead. If collision with an accidental:
	//	Do while collision:
	//		set x pos after collisioning accidental
	//		Render notehead
	//		test if collision
	//	end do

    lmShapeObj* pNoteHead;
    lmNote* pCrashNote;
    pNode = m_cNotes.GetFirst();
    for(iN=1; pNode; pNode=pNode->GetNext(), iN++ )
    {
        //get the note
        pNote = (lmNote*)pNode->GetData();

        //compute offset
        yStaffTopLine = pNote->GetStaffOffset();   // staff y position (top line)
        yPos = yStaffTopLine - pNote->GetPitchShift();
        xPos = 0;
        if (pNote->HasAccidentals()) {
            pAccidental = pNote->GetAccidentals();
            //! @todo instead of width it must be position+width
            xPos += pAccidental->GetWidth();
        }

        //compute notehead's position
        pNote->DrawNote(pPaper, DO_MEASURE, xPos, yPos, colorC);
        pNoteHead = pNote->GetNoteheadShape();
        //check if collision with any previous note accidentals
        pCrashNote = CheckIfNoteCollision(pNoteHead);
        while (pCrashNote) {
            //try to render at right of colliding accidental
            xPos += (pCrashNote->GetAccidentals())->GetWidth();
            pNote->DrawNote(pPaper, DO_MEASURE, xPos, yPos, colorC);
            //check again for collision
            pCrashNote = CheckIfNoteCollision(pNoteHead);
        }
    }

    //Here all noteheads are positioned without collisions. Proceed to shift
    //noteheads positions to have a common anchor line, so that accidentals get
    //not shifted in justification process

    //a) Compute common anchor line as the max anchor line position

    lmLUnits nMaxAnchor = -99999;
    pNode = m_cNotes.GetFirst();
    for(; pNode; pNode=pNode->GetNext()) {
        pNote = (lmNote*)pNode->GetData();
        nMaxAnchor = wxMax(nMaxAnchor, pNote->GetAnchorPos());
    }

	//b) Add a shift to each note so that its anchor line become the max anchor line
    lmLUnits nShift;
    pNode = m_cNotes.GetFirst();
    for(; pNode; pNode=pNode->GetNext()) {
        pNote = (lmNote*)pNode->GetData();
        pNoteHead = pNote->GetNoteheadShape();
        nShift = nMaxAnchor - pNote->GetAnchorPos();
        pNote->ShiftNoteShape(nShift);
    }

}

lmNote* lmChord::CheckIfCollisionWithAccidentals(bool fOnlyLeftNotes, int iCurNote, lmShapeObj* pShape)
{
	//Check to see if the shape pShape overlaps any accidental of
    //the chord, from first note to note iCurNote (excluded, range: 1..m_cNotes.GetCount())
    //If no collision returns NULL, otherwse, returns the Note
    //owning the accidental that collides
    int iN;
    lmNote* pNote;
    lmAccidental* pAccidental;
    wxNotesListNode *pNode = m_cNotes.GetFirst();
    for(iN=1; pNode && iN < iCurNote; pNode=pNode->GetNext(), iN++ ) {
        pNote = (lmNote*)pNode->GetData();
        if (fOnlyLeftNotes && !pNote->IsNoteheadReversed() || !fOnlyLeftNotes) {
            if (pNote->HasAccidentals()) {
                pAccidental = pNote->GetAccidentals();
                if ( pShape->Collision(pAccidental->GetShape()) ) {
                    //collision
                    return pNote;
                }
            }
        }
    }

    //In second loop, when checking accidentals for right shifted notes, it is necessary
    //to verify collisions with all remaining notes at left
    if (!fOnlyLeftNotes) {
        for(iN=iCurNote; pNode && iN <= (int)m_cNotes.GetCount(); pNode=pNode->GetNext(), iN++ ) {
            pNote = (lmNote*)pNode->GetData();
            if (!pNote->IsNoteheadReversed()) {
                if (pNote->HasAccidentals()) {
                    pAccidental = pNote->GetAccidentals();
                    if ( pShape->Collision(pAccidental->GetShape()) ) {
                        //collision
                        return pNote;
                    }
                }
            }
        }
    }

    //no collision
    return (lmNote*)NULL;

}

lmNote* lmChord::CheckIfNoteCollision(lmShapeObj* pShape)
{
	//Check to see if the shape pShape overlaps any accidental of the chord
    //If no collision returns NULL, otherwse, returns the Note
    //owning the accidental that collides

    int iN;
    lmNote* pNote;
    lmAccidental* pAccidental;
    wxNotesListNode *pNode = m_cNotes.GetFirst();
    for(iN=1; pNode && iN <= (int)m_cNotes.GetCount(); pNode=pNode->GetNext(), iN++ ) {
        pNote = (lmNote*)pNode->GetData();
        if (pNote->HasAccidentals()) {
            pAccidental = pNote->GetAccidentals();
            if ( pShape->Collision(pAccidental->GetShape()) ) {
                //collision
                return pNote;
            }
        }
    }

    //no collision
    return (lmNote*)NULL;

}

void lmChord::ComputeAccidentalLayout(bool fOnlyLeftNotes, lmNote* pNote, int iN, lmPaper* pPaper, lmUPoint paperPos, wxColour colorC)
{
    wxASSERT(pNote->HasAccidentals());

    //compute offset
    lmLUnits yStaffTopLine = pNote->GetStaffOffset();   // staff y position (top line)
    lmLUnits yPos = yStaffTopLine - pNote->GetPitchShift();
    lmLUnits xPos = 0;

    //compute accidentals layout
    pNote->DrawAccidentals(pPaper, DO_MEASURE, xPos, yPos, colorC);
    lmAccidental* pAccidental = pNote->GetAccidentals();
    lmShapeObj* pAccShape = pAccidental->GetShape();

    //check if collision with any previous note accidentals
    lmNote* pCrashNote = CheckIfCollisionWithAccidentals(fOnlyLeftNotes, iN, pAccShape);
    int nWatchDog = 0;
    while (pCrashNote) {
        //try to render at right of colliding accidental

        xPos += ((pCrashNote->GetAccidentals())->GetShape())->GetWidth();
        pNote->DrawAccidentals(pPaper, DO_MEASURE, xPos, yPos, colorC);
        //check again for collision
        pCrashNote = CheckIfCollisionWithAccidentals(fOnlyLeftNotes, iN, pAccShape);
        nWatchDog++;
        if (nWatchDog > 1000) {
            wxLogMessage(_T("[lmChord::ComputeAccidentalLayout] Loop detected"));
            break;
        }
    }

}
