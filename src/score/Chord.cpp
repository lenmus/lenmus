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

//
//  class lmChord
//    Information and methods to group several notes into a chord.
//
//    Contains the information needed to group several notes into a chord.
//    For each chord there exist a lmChord object and a pointer to it is stored on each
//    note of the chord
//
//    The class is named lmChord instead of Chord because there is a function named "Chord"
//    defined in WinGDI.h (Microsoft), and it causes compilation problems (with MS VC)
//    as the names collide.
//

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "Chord.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <algorithm>

#include "Score.h"
#include "VStaff.h"
#include "Glyph.h"
#include "UndoRedo.h"
#include "../graphic/Shapes.h"
#include "../graphic/ShapeNote.h"
#include "../graphic/GMObject.h"

int GlobalPitchCompare(const void* pNote1, const void* pNote2)
{
    if ((*(lmNote**)pNote1)->StemGoesDown())
        return ((*(lmNote**)pNote1)->GetDPitch() < (*(lmNote**)pNote2)->GetDPitch());
    else
        return ((*(lmNote**)pNote1)->GetDPitch() > (*(lmNote**)pNote2)->GetDPitch());
}


lmChord::lmChord(lmNote* pBaseNote)
{
    // Creates the chord object, with only the base note.

    m_Notes.push_back(pBaseNote);
    m_nStemType = pBaseNote->GetStemType();

	//inform added note
	pBaseNote->OnIncludedInChord(this);
}

lmChord::~lmChord()
{
    //
    // Destructor. When invoked, only from lmNote destructor, there must be only one
    // note: the base note.
    //

    wxASSERT(m_Notes.size() == 1);

    lmNote* pNote = m_Notes.front();
    pNote->OnRemovedFromChord();

    //notes will not be deleted when deleting the chord list, as they are still part of the
    //score and will be deleted later
    m_Notes.pop_front();
    m_Notes.clear();
}

wxString lmChord::Dump()
{
    wxString sDump = wxString::Format(_T("Chord: num.notes=%d ("), NumNotes());

    std::list<lmNote*>::iterator it;
    for(it = m_Notes.begin(); it != m_Notes.end(); ++it)
    {
        sDump += wxString::Format(_T("%d,"), (*it)->GetDPitch() );
    }
    sDump += wxString::Format(_T("), max=%d, min=%d, base=%d, stem="),
                GetMaxNote()->GetDPitch(), GetMinNote()->GetDPitch(), GetBaseNote()->GetDPitch() );
    sDump += (m_fStemDown ? _T("down") : _T("up"));

    return sDump;
}

void lmChord::Include(lmNote* pNewNote, int nIndex)
{
    // Add a note to the chord. Index is the position taht the added note must occupy
	// (0..n). If -1, note will be added at the end.
    // Precondition: When this method is invoked at least one note in the
	// chord: the base note

    wxASSERT(NumNotes() > 0);

	//add the note
	if (nIndex == -1 || nIndex == NumNotes())
		m_Notes.push_back(pNewNote);
	else
	{
		int iN;
		std::list<lmNote*>::iterator it;
		for(iN=0, it=m_Notes.begin(); it != m_Notes.end(); ++it, iN++)
		{
			if (iN == nIndex)
			{
				//insert before current item
				m_Notes.insert(it, pNewNote);
				break;
			}
		}
	}

    //re-compute stem direction
    ComputeStemDirection();

}

void lmChord::Remove(lmNote* pNoteToRemove)
{
	// Removes a note from a chord.
	// When invoked there must be at least two notes as
	// otherwise it can not be a chord.

	wxASSERT(NumNotes() > 1);

    //remove note
    std::list<lmNote*>::iterator it;
    it = std::find(m_Notes.begin(), m_Notes.end(), pNoteToRemove);
    m_Notes.erase(it);
}

int lmChord::GetNoteIndex(lmNote* pNote)
{
	//returns the position in the notes list (0..n)

	wxASSERT(NumNotes() > 1);

	int iN;
    std::list<lmNote*>::iterator it;
    for(iN=0, it=m_Notes.begin(); it != m_Notes.end(); ++it, iN++)
	{
		if (pNote == *it) return iN;
	}
    wxASSERT(false);	//note not found
	return 0;			//compiler happy
}

void lmChord::AddStemShape(lmPaper* pPaper, wxColour colorC,
						   wxFont* pFont, lmVStaff* pVStaff, int nStaff)
{
	// Add the shape for the stem of the chord.
	// Once the last note of a chord has been layouted this method
	// is invoked (from the last lines of lmNote::LayoutObject() ) to add the
	// shape for the stem and the flag of the chord.
	// The stem position is stored in the base note.

    lmNote* pBaseNote = GetBaseNote();
    lmLUnits uxStem = pBaseNote->GetXStemCenter();
    lmLUnits uyStartStem=0.0f, uExtraLenght=0.0f, uyStemEnd=0.0f;

    if (!pBaseNote->IsBeamed()) {
        //compute y positions
        if (m_fStemDown) 
        {
            uyStartStem = GetMaxNote()->GetYStartStem();
            uyStemEnd = GetMinNote()->GetYEndStem();
        }
        else 
        {
            uyStartStem = GetMinNote()->GetYStartStem();
            uyStemEnd = GetMaxNote()->GetYEndStem();
        }

    }
    else 
    {
        //If the chord is beamed, the stem length must be increased with the distance from
        //min note to max note.
        if (pBaseNote->StemGoesDown())
        {
            //stem down: line at left of noteheads
            uyStartStem = GetMaxNote()->GetYStartStem();
            uyStemEnd = GetMinNote()->GetYStartStem() + pBaseNote->GetStemLength();
        }
        else 
        {
            //stem up: line at right of noteheads
            uyStartStem = GetMinNote()->GetYStartStem();
            uyStemEnd = GetMaxNote()->GetYStartStem() - pBaseNote->GetStemLength();
        }
    }
    uExtraLenght = GetMinNote()->GetYStartStem() - GetMaxNote()->GetYStartStem();

	//proceed to create the stem and the flag shapes. If the flag must be added we
	//need to create a compoite shape as container for flag and stem. Otherwise we
	//will just add the stem shape
	bool fFlagNeeded = !pBaseNote->IsBeamed() && pBaseNote->GetNoteType() > eQuarter;
	bool fStemDown = pBaseNote->StemGoesDown();

	//if flag to be drawn, compute flag position
	lmUPoint uFlag;
	lmEGlyphIndex nGlyph;
    if (fFlagNeeded)
	{
		nGlyph = pBaseNote->GetGlyphForFlag();
        //lmLUnits uStemLength = fabs(uyStartStem - uyStemEnd);

        // to measure flag and stem I am going to use some glyph data. These
        // data is in FUnits but as 512 FU are 1 line (10 tenths) it is simple
        // to convert these data into tenths: just divide FU by 51.2
        float rFlag;
        if (fStemDown)
		{
            rFlag = fabs((2048.0 - (float)aGlyphsInfo[nGlyph].Bottom) / 51.2 );
        }
        else
		{
            if (pBaseNote->GetNoteType() == eEighth)
                rFlag = ((float)aGlyphsInfo[nGlyph].Top) / 51.2 ;
            else if (pBaseNote->GetNoteType() == e16th)
                rFlag = ((float)aGlyphsInfo[nGlyph].Top + 128.0) / 51.2 ;
            else
                rFlag = ((float)aGlyphsInfo[nGlyph].Top + 512.0) / 51.2 ;
        }
        lmLUnits uSizeFlag = pVStaff->TenthsToLogical(rFlag, nStaff);
        if (fStemDown)
			uFlag.y = uyStemEnd - uSizeFlag - pVStaff->TenthsToLogical(20, nStaff);
		else
			uFlag.y = uyStemEnd - (pVStaff->TenthsToLogical(60, nStaff) - uSizeFlag);

		uFlag.x = (pBaseNote->StemGoesDown() ?
					pBaseNote->GetXStemLeft() : pBaseNote->GetXStemRight());
    }

	//create the stem shape
	lmShapeNote* pShapeNote = (lmShapeNote*)pBaseNote->GetShape();
    #define STEM_WIDTH   12     //stem line width (cents = tenths x10)
    lmLUnits uStemThickness = pVStaff->TenthsToLogical(STEM_WIDTH, nStaff) / 10;
	//wxLogMessage(_T("[lmChord::AddStemShape] Shape xPos=%.2f, yStart=%.2f, yEnd=%.2f, yFlag=%.2f, fDown=%s)"),
	//	uxStem, uyStartStem, uyStemEnd, uyFlag, (pBaseNote->StemGoesDown() ? _T("down") : _T("up")) );
    lmShapeStem* pStem =
        new lmShapeStem(pShapeNote->GetScoreOwner(), uxStem, uyStartStem, uExtraLenght,
                        uyStemEnd, pBaseNote->StemGoesDown(), uStemThickness, colorC);

	// if beamed, the stem shape will be owned by the beam; otherwise by the note
	if (pBaseNote->IsBeamed()) {
		lmBeam* pBeam = pBaseNote->GetBeam();
		pBeam->AddNoteAndStem(pStem, pShapeNote, pBaseNote->GetBeamInfo());
	}
	else
		pShapeNote->AddStem(pStem);

    //add the flag
	if (fFlagNeeded)
	{
		lmShapeGlyph* pShape = new lmShapeGlyph(pShapeNote->GetScoreOwner(), -1, nGlyph, pFont,
                                                pPaper, uFlag, _T("Flag"));
		pShapeNote->AddFlag(pShape);
	}

	//delete stem shapes created in the notes
    std::list<lmNote*>::iterator it;
    for(it = m_Notes.begin(); it != m_Notes.end(); ++it)
    {
        (*it)->DeleteStemShape();
    }
}

lmNote* lmChord::GetMaxNote() 
{
    std::list<lmNote*>::iterator it = m_Notes.begin();
	lmDPitch dMaxPitch = (*it)->GetDPitch();
    lmNote* pMaxNote = (*it);

    for(++it; it != m_Notes.end(); ++it)
    {
        if (dMaxPitch < (*it)->GetDPitch())
		{
            pMaxNote = (*it);
			dMaxPitch = (*it)->GetDPitch();
        }
    }
    return pMaxNote; 
}

lmNote* lmChord::GetMinNote() 
{ 
    std::list<lmNote*>::iterator it = m_Notes.begin();
	lmDPitch dMinPitch = (*it)->GetDPitch();
    lmNote* pMinNote = (*it);

    for(++it; it != m_Notes.end(); ++it)
    {
        if (dMinPitch > (*it)->GetDPitch())
		{
            pMinNote = (*it);
			dMinPitch = (*it)->GetDPitch();
        }
    }
    return pMinNote; 
}

lmLUnits lmChord::DrawFlag(bool fMeasuring, lmPaper* pPaper, lmNote* pBaseNote,
                                 lmUPoint uPos, wxColour colorC, wxFont* pFont,
                                 lmVStaff* pVStaff, int nStaff)
{
    //Draws the flag using a glyph. Returns the flag width

    lmENoteType nNoteType = pBaseNote->GetNoteType();
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
        pPaper->DrawText(sGlyph, uPos.x,
            uPos.y + pVStaff->TenthsToLogical( aGlyphsInfo[nGlyph].GlyphOffset, nStaff ) );
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


    if (NumNotes() < 2) return;

    lmNote* pBaseNote = GetBaseNote();

    #define TWO_NOTES_DEFAULT true          //TODO move to layout user options

    if (m_nStemType == lmSTEM_UP) {          //force stem up
        m_fStemDown = false;
    }
    else if (m_nStemType == lmSTEM_DOWN) {   //force stem down
        m_fStemDown = true;
    }
    else if (m_nStemType == lmSTEM_NONE) {   //force no stem
        m_fStemDown = false;
    }
    else if (m_nStemType == lmSTEM_DEFAULT) {    //as decided by program
        //Rules
        int nWeight = GetMinNote()->GetPosOnStaff() + GetMaxNote()->GetPosOnStaff();
        if (nWeight > 12)
            m_fStemDown = true;
        else if (nWeight < 12)
            m_fStemDown = false;
        else {
            //majority rule if more than two notes. Else default for two notes case
            m_fStemDown = TWO_NOTES_DEFAULT;
            if (NumNotes() > 2) {
                int iN;
                nWeight = 0;
				std::list<lmNote*>::iterator it;
				for(iN=0, it=m_Notes.begin(); it != m_Notes.end(); ++it, iN++)
				{
                    nWeight += (*it)->GetPosOnStaff();
                }
                m_fStemDown = (nWeight >= 6*iN);
            }
        }
    }

    //update max and min notes with conclusion about stem direction.
    //AWARE: for chords, setting the base note forces to call lmChord::SetStemDirection()
    //  to setup also max and min notes
    pBaseNote->SetStemDirection(m_fStemDown);

}

void lmChord::SetStemDirection(bool fStemDown)
{
    m_fStemDown = fStemDown;
    lmNote* pBaseNote = GetBaseNote();
    if (pBaseNote != GetMinNote())    //check to avoid infinite loops
        GetMinNote()->SetStemDirection(m_fStemDown);
    if (pBaseNote != GetMaxNote())    //check to avoid infinite loops
        GetMaxNote()->SetStemDirection(m_fStemDown);

}

void lmChord::ArrangeNoteheads()
{
    //arrange noteheads at left/right of stem to avoid collisions
    //This method asumes that the stem direction has been computed
    //This method sets flag  pNote->SetNoteheadReversed(true); to true or false,
    //depending on the requiered notehead position for the chord.

    if (NumNotes() < 2) return;

    //arrange notes by pitch
    std::list<lmNote*> cNotes = m_Notes; 
    cNotes.sort(GlobalPitchCompare);

    bool fSomeReversed = false;
    int nPosPrev = 1000;    // a very high number not posible in real world
    int nPos;
	std::list<lmNote*>::iterator it = 
		(m_fStemDown ? --m_Notes.end() : m_Notes.begin());
	for(; it != m_Notes.end(); (m_fStemDown ? --it : ++it))
	{
        nPos = (*it)->GetPosOnStaff();
        if (abs(nPosPrev - nPos) < 2)
		{
            //collision. Reverse position of this notehead
            fSomeReversed = true;
            (*it)->SetNoteheadReversed(true);
            nPosPrev = 1000;
        }
        else
		{
            (*it)->SetNoteheadReversed(false);
            nPosPrev = nPos;
        }
    }

    return;


}

void lmChord::LayoutNoteHeads(lmBox* pBox, lmPaper* pPaper, lmUPoint uPaperPos, wxColour colorC)
{
    //Step 1) arrange noteheads at left/right of stem to avoid collisions
    //As result, flag fNoteheadReversed is set for all notes in the chord. No
    //notehead placement takes place
	//-----------------------------------------------------------------------------------
    ArrangeNoteheads();


	//Step 2). Prepare the notes for layout.
	//AWARE: this method (LayoutNoteHeads) is invoked while layouting the chord base
	// note. Therefore, all chord notes, except the base note, are not prepared for
	// layout:
	// - paper position is not yet assigned
	// - container shape is not craeted
	// In following loop all these issues are fixed
	//-----------------------------------------------------------------------------------
    int iN;
	std::list<lmNote*>::iterator it = m_Notes.begin();
	for(iN=1; it != m_Notes.end(); ++it, iN++ )
	{
		if (iN > 1) //skip base note
		{
			//assign paper pos to this note.
            lmNote* pBase = GetBaseNote();
			(*it)->SetReferencePos( pBase->GetReferencePaperPos() );
            (*it)->SetLayoutRefPos( pBase->GetLayoutRefPos() );
		}
		//create the shape container
		(*it)->CreateContainerShape(pBox, uPaperPos.x, uPaperPos.y, colorC);
    }


	//Step 3) Layout accidentals
	//Algorithm:
	//  Loop to compute position for accidentals:
	//  - Set x pos to start of chord x pos
	//  - Render accidental. If collision with other accidental:
	//	     Do while collision:
	//		     set x pos after collisioning accidental
	//		     Render accidental
	//		     test if collision
	//	     end do
	//-----------------------------------------------------------------------------------

    //first loop: process notes not shitfted to right
	it = m_Notes.begin();
	for(iN=1; it != m_Notes.end(); ++it, iN++ )
	{
        if (!(*it)->IsNoteheadReversed() && (*it)->HasAccidentals())
            ComputeAccidentalLayout(true, *it, iN, pPaper, uPaperPos, colorC);
    }

    //second loop: process notes  shitfted to right
	it = m_Notes.begin();
	for(iN=1; it != m_Notes.end(); ++it, iN++ )
	{
        if ((*it)->IsNoteheadReversed() && (*it)->HasAccidentals())
            ComputeAccidentalLayout(false, *it, iN, pPaper, uPaperPos, colorC);
    }

    //Here all accidentals are positioned without collisions. Procceed to compute
    //noteheads positions (Use also this loop to add accidental shapes)


	//Step 4) Layout noteheads
    //Loop to compute noteheads' position:
	//- Set x pos to start of chord x pos + note's accidental width (if exists)
	//- Render notehead. If collision with an accidental:
	//	Do while collision:
	//		set x pos after collisioning accidental
	//		Render notehead
	//		test if collision
	//	end do
	//-----------------------------------------------------------------------------------

    lmLUnits xPos, yPos;
    lmLUnits yStaffTopLine;
    lmAccidental* pAccidental;
    lmShape* pNoteHead;
    lmNote* pCrashNote;
	it = m_Notes.begin();
	for(iN=1; it != m_Notes.end(); ++it, iN++ )
	{
        //get the note
		lmShapeNote* pNoteShape = (lmShapeNote*)(*it)->GetShape();

        //compute offset
        yStaffTopLine = (*it)->GetStaffOffset();   // staff y position (top line)
        yPos = yStaffTopLine - (*it)->GetPitchShift();
        xPos = 0;
        if ((*it)->HasAccidentals()) {
            pAccidental = (*it)->GetAccidentals();
            xPos += pAccidental->GetWidth();
            //add accidental shape
            pNoteShape->AddAccidental(pAccidental->GetShape());
        }

        //compute notehead's position
		///*dbg*/ wxLogMessage(_T("[lmChord::LayoutNoteHeads] adding note %d, xPos=%.2f, reversed=%s"),
		//      	iN, xPos + uPaperPos.x, ((*it)->IsNoteheadReversed() ? _T("yes") : _T("no")) );
		(*it)->AddNoteShape(pNoteShape, pPaper, xPos + uPaperPos.x, yPos + uPaperPos.y, colorC);
        pNoteHead = (*it)->GetNoteheadShape();
        //check if collision with any previous note accidentals
        pCrashNote = CheckIfNoteCollision(pNoteHead);
        while (pCrashNote)
		{
            //try to render at right of colliding accidental
			lmLUnits uShift = (pCrashNote->GetAccidentals())->GetWidth();
			(*it)->ShiftNoteHeadShape(uShift);
			///*dbg*/ wxLogMessage(_T("[lmChord::LayoutNoteHeads] shift note %d, uShift=%.2f"),
			//                     iN, uShift );
            //check again for collision
            pCrashNote = CheckIfNoteCollision(pNoteHead);
        }
    }

    //Here all noteheads are positioned without collisions. Proceed to shift
    //noteheads positions to have a common anchor line, so that accidentals get
    //not shifted in justification process
	//Proceed to align all noteheads along the common anchor line
	//-----------------------------------------------------------------------------------

    //a) Compute common anchor line as the max anchor line position

	///*dbg*/ wxLogMessage(_T("[lmChord::LayoutNoteHeads] Compute common anchor line:"));
    lmLUnits uMaxAnchor = -99999;
	it = m_Notes.begin();
	for(; it != m_Notes.end(); ++it)
	{
        uMaxAnchor = wxMax(uMaxAnchor, (*it)->GetAnchorPos());
		///*dbg*/ wxLogMessage(_T("[lmChord::LayoutNoteHeads] note %d, uMaxAnchor=%.2f, pNote->GetAnchorPos = %.2f"),
		//          	++iNote, uMaxAnchor, pNote->GetAnchorPos() );
    }

	//b) Add a shift to each note so that its anchor line become the max anchor line
	///*dbg*/ wxLogMessage(_T("[lmChord::LayoutNoteHeads] Adding a shift to each note:"));
    lmLUnits uShift;
	it = m_Notes.begin();
	for(; it != m_Notes.end(); ++it)
	{
        pNoteHead = (*it)->GetNoteheadShape();
        uShift = uMaxAnchor - (*it)->GetAnchorPos();
		///*dbg*/ wxLogMessage(_T("[lmChord::LayoutNoteHeads] uShift=%.2f"), uShift );
        (*it)->ShiftNoteHeadShape(uShift);
    }


}

lmLUnits lmChord::GetXRight()
{
	lmLUnits uxRight = 0.0;
	std::list<lmNote*>::iterator it = m_Notes.begin();
	for(; it != m_Notes.end(); ++it)
	{
		uxRight = wxMax(uxRight, (*it)->GetShape()->GetXRight());
    }
	return uxRight;
}

lmNote* lmChord::CheckIfCollisionWithAccidentals(bool fOnlyLeftNotes, int iCurNote, lmShape* pShape)
{
	//Check to see if the shape pShape overlaps any accidental of
    //the chord, from first note to note iCurNote (excluded, range: 1..NumNotes())
    //If no collision returns NULL, otherwse, returns the Note
    //owning the accidental that collides

    int iN;
    lmAccidental* pAccidental;
	std::list<lmNote*>::iterator it = m_Notes.begin();
    for(iN=1; it != m_Notes.end() && iN < iCurNote; ++it, iN++ )
	{
        if (fOnlyLeftNotes && !(*it)->IsNoteheadReversed() || !fOnlyLeftNotes)
		{
            if ((*it)->HasAccidentals())
			{
                pAccidental = (*it)->GetAccidentals();
                if ( pShape->Collision(pAccidental->GetShape()) )
				{
                    //collision
                    return (*it);
                }
            }
        }
    }

    //In second loop, when checking accidentals for right shifted notes, it is necessary
    //to verify collisions with all remaining notes at left
    if (!fOnlyLeftNotes)
	{
		it = m_Notes.begin();
        for(iN=iCurNote; it != m_Notes.end() && iN <= NumNotes(); ++it, iN++ )
		{
            if (!(*it)->IsNoteheadReversed() && (*it)->HasAccidentals())
			{
                pAccidental = (*it)->GetAccidentals();
                if ( pShape->Collision(pAccidental->GetShape()) )
				{
                    //collision
                    return (*it);
                }
            }
        }
    }

    //no collision
    return (lmNote*)NULL;

}

lmNote* lmChord::CheckIfNoteCollision(lmShape* pShape)
{
	//Check to see if the shape pShape overlaps any accidental of the chord
    //If no collision returns NULL, otherwse, returns the Note
    //owning the accidental that collides

    int iN;
    lmAccidental* pAccidental;
	std::list<lmNote*>::iterator it = m_Notes.begin();
    for(iN=1; it != m_Notes.end() && iN <= NumNotes(); ++it, iN++ )
	{
        if ((*it)->HasAccidentals()) {
            pAccidental = (*it)->GetAccidentals();
            if ( pShape->Collision(pAccidental->GetShape()) ) {
                //collision
                return *it;
            }
        }
    }

    //no collision
    return (lmNote*)NULL;

}

void lmChord::ComputeAccidentalLayout(bool fOnlyLeftNotes, lmNote* pNote, int iN, lmPaper* pPaper, lmUPoint uPaperPos, wxColour colorC)
{
    wxASSERT(pNote->HasAccidentals());

    //compute offset
    lmLUnits yStaffTopLine = pNote->GetStaffOffset();   // staff y position (top line)
    lmLUnits yPos = yStaffTopLine - pNote->GetPitchShift() + pPaper->GetCursorY();
    lmLUnits xPos = pPaper->GetCursorX();

    //compute accidentals layout
    lmAccidental* pAccidental = pNote->GetAccidentals();
    pAccidental->Layout(pPaper, xPos, yPos);
    lmShape* pAccShape = pAccidental->GetShape();

    //check if collision with any previous note accidentals
    lmNote* pCrashNote = CheckIfCollisionWithAccidentals(fOnlyLeftNotes, iN, pAccShape);
    int nWatchDog = 0;
    while (pCrashNote)
	{
        //Collision. try to render at right of colliding accidental

        xPos += ((pCrashNote->GetAccidentals())->GetShape())->GetWidth();
        pAccidental->MoveTo(xPos, yPos);
		//wxLogMessage(_T("[lmChord::ComputeAccidentalLayout] Accidental moved to (%.2f, %.2f)"), xPos, yPos);
        //check again for collision
        pCrashNote = CheckIfCollisionWithAccidentals(fOnlyLeftNotes, iN, pAccShape);
        nWatchDog++;
        if (nWatchDog > 1000) {
            wxLogMessage(_T("[lmChord::ComputeAccidentalLayout] Loop detected"));
            break;
        }
    }

}
