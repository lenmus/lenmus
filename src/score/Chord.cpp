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

#ifdef __GNUG__
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

#include "Score.h"
#include "Glyph.h"
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
    //
    // Creates the chord object, with only the base note.
    //

    m_cNotes.Append(pBaseNote);

    // as this is the only note it is the max and the min one
    m_pBaseNote = pBaseNote;
    m_pMinNote = pBaseNote;
    m_pMaxNote = pBaseNote;
	m_pLastNote = pBaseNote;

    m_nStemType = pBaseNote->GetStemType();
}

lmChord::~lmChord()
{
    //
    // Destructor. When invoked, only from lmNote destructor, there must be only one
    // note: the base note.
    //

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
        sDump += wxString::Format(_T("%d,") ,pNote->GetDPitch() );
    }
    sDump += wxString::Format(_T("), max=%d, min=%d, base=%d, stem="),
                m_pMaxNote->GetDPitch(), m_pMinNote->GetDPitch(), m_pBaseNote->GetDPitch() );
    sDump += (m_fStemDown ? _T("down") : _T("up"));

    return sDump;
}

void lmChord::AddNote(lmNote* pNewNote)
{
    // Add a note to the chord.
    // Notes will be kept ordered by pitch. First item lowest pitch
    // When this method is invoked at least the base note is in the collection

    wxASSERT(m_cNotes.GetCount() > 0);

    int iPos;
    wxNotesListNode *pNode = m_cNotes.GetFirst();
    lmNote* pNote;
    for(iPos=0; pNode; pNode=pNode->GetNext(), iPos++) {
        pNote = (lmNote*)pNode->GetData();
        if (pNote->GetDPitch() > pNewNote->GetDPitch()) break;
    }
    m_cNotes.Insert((size_t)iPos, pNewNote);

    //Update Max and Min note
    if (m_pMinNote->GetDPitch() > pNewNote->GetDPitch()) {
        m_pMinNote = pNewNote;
    } else if (m_pMaxNote->GetDPitch() < pNewNote->GetDPitch()) {
        m_pMaxNote = pNewNote;
    }

	// update last note
	m_pLastNote = pNewNote;

    //compute stem direction
    ComputeStemDirection();

}

void lmChord::RemoveNote(lmNote* pNote)
{
	// Removes a note from a chord.
	// When invoked there must be at least two notes as
	// otherwise it can not be a chord.

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
        if (m_pMinNote->GetDPitch() > pNote->GetDPitch()) {
            m_pMinNote = pNote;
        } else if (m_pMaxNote->GetDPitch() < pNote->GetDPitch()) {
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
    lmLUnits uyStemStart=0, uyStemEnd=0;

    if (!pBaseNote->IsBeamed()) {
        //compute y positions
        if (m_fStemDown) {
            uyStemStart = m_pMaxNote->GetYStartStem();
            uyStemEnd = m_pMinNote->GetYEndStem();
        }
        else {
            uyStemStart = m_pMinNote->GetYStartStem();
            uyStemEnd = m_pMaxNote->GetYEndStem();
        }

    }
    else {
        // If the chord is beamed, the stem length was computed during beam computation and
        // stored in the base note
        if (pBaseNote->StemGoesDown()) {
            //stem down: line at left of noteheads
            uyStemStart = m_pMaxNote->GetYStartStem();
            uyStemEnd = m_pMinNote->GetYStartStem() + pBaseNote->GetStemLength();
        } else {
            //stem up: line at right of noteheads
            uyStemStart = m_pMinNote->GetYStartStem();
            uyStemEnd = m_pMaxNote->GetYStartStem() - pBaseNote->GetStemLength();
        }
    }

	//proceed to create the stem and the flag shapes. If the flag must be added we
	//need to create a compoite shape as container for flag and stem. Otherwise we
	//will just add the stem shape
	bool fFlagNeeded = !pBaseNote->IsBeamed() && pBaseNote->GetNoteType() > eQuarter;

//_______________________________
//

	//if flag to be drawn, adjust stem size and compute flag position
	lmLUnits uxFlag, uyFlag;
	lmEGlyphIndex nGlyph;
    if (fFlagNeeded) {
		nGlyph = pBaseNote->GetGlyphForFlag();
        lmLUnits uStemLength = fabs(uyStemStart - uyStemEnd);
        // to measure flag and stem I am going to use some glyph data. These
        // data is in FUnits but as 512 FU are 1 line (10 tenths) it is simple
        // to convert these data into tenths: just divide FU by 51.2
        float rFlag, rMinStem;
        if (pBaseNote->StemGoesDown()) {
            rFlag = fabs((2048.0 - (float)aGlyphsInfo[nGlyph].Bottom) / 51.2 );
            //rMinStem = ((float)aGlyphsInfo[nGlyph].Top - 2048.0 + 128.0) / 51.2 ;
        }
        else {
            if (pBaseNote->GetNoteType() == eEighth)
                rFlag = ((float)aGlyphsInfo[nGlyph].Top) / 51.2 ;
            else if (pBaseNote->GetNoteType() == e16th)
                rFlag = ((float)aGlyphsInfo[nGlyph].Top + 128.0) / 51.2 ;
            else
                rFlag = ((float)aGlyphsInfo[nGlyph].Top + 512.0) / 51.2 ;

            //rMinStem = fabs( (float)aGlyphsInfo[nGlyph].Bottom / 51.2 );
        }
        lmLUnits uFlag = pVStaff->TenthsToLogical(rFlag, nStaff);
        //lmLUnits uMinStem = pVStaff->TenthsToLogical(rMinStem, nStaff);
        //uStemLength = wxMax((uStemLength > uFlag ? uStemLength-uFlag : 0), uMinStem);
        if (pBaseNote->StemGoesDown()) {
			//uyStemEnd = m_pMinNote->GetYStartStem() + uStemLength;
			uyFlag = uyStemEnd - uFlag - pVStaff->TenthsToLogical(20);
		}
		else {
            //uyStemEnd = m_pMaxNote->GetYStartStem() - uStemLength;
			uyFlag = uyStemEnd - (pVStaff->TenthsToLogical(60) -uFlag);	//uyStemEnd - uFlag ;
		}

  //      uyFlag = uyStemStart + (pBaseNote->StemGoesDown() ? uStemLength : -uStemLength);
		//uyStemEnd = uyFlag + (pBaseNote->StemGoesDown() ? uFlag : -uFlag);;
  //      //SetStemLength(uStemLength + uFlag);
		//uyFlag = uyStemEnd - (pBaseNote->StemGoesDown() ? -uFlag : uFlag);
    }

//________________________________
//

	//create the stem shape
	lmShapeNote* pShapeNote = (lmShapeNote*)pBaseNote->GetShap2(); 
    #define STEM_WIDTH   12     //stem line width (cents = tenths x10)
    lmLUnits uStemThickness = pVStaff->TenthsToLogical(STEM_WIDTH, nStaff) / 10;
	wxLogMessage(_T("[lmChord::AddStemShape] Shape xPos=%.2f, yStart=%.2f, yEnd=%.2f, yFlag=%.2f, fDown=%s)"),
		uxStem, uyStemStart, uyStemEnd, uyFlag, (pBaseNote->StemGoesDown() ? _T("down") : _T("up")) );
    lmShapeStem* pStem = 
        new lmShapeStem(pShapeNote->GetScoreOwner(), uxStem, uyStemStart, uyStemEnd,
						pBaseNote->StemGoesDown(), uStemThickness, colorC);

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
		uxFlag = (pBaseNote->StemGoesDown() ?
					pBaseNote->GetXStemLeft() : pBaseNote->GetXStemRight());
		lmEGlyphIndex nGlyph = pBaseNote->GetGlyphForFlag();
		//lmLUnits yPos = uyStemEnd + pVStaff->TenthsToLogical(aGlyphsInfo[nGlyph].GlyphOffset, nStaff);
		wxLogMessage(_T("[lmChord::AddStemShape] yFlag=%.2f)"), uyFlag);
		lmShapeGlyph* pShape = new lmShapeGlyph(pShapeNote->GetScoreOwner(), nGlyph, pFont,
                                                pPaper, lmUPoint(uxFlag, uyFlag), _T("Flag"));
		pShapeNote->AddFlag(pShape);
	}

	//delete stem shapes created in the notes
    for(wxNotesListNode *pNode = m_cNotes.GetFirst(); pNode; pNode=pNode->GetNext() )
	{
        ((lmNote*)pNode->GetData())->DeleteStemShape();
    }

}


bool lmChord::IsLastNoteOfChord(lmNote* pNote)
{
    //wxNotesListNode *pNode = m_cNotes.GetLast();
    //lmNote* pLastNote = (lmNote*)pNode->GetData();
    return (m_pLastNote->GetID() == pNote->GetID());

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


    if (m_cNotes.GetCount() < 2) return;

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
    //This method sets flag  pNote->SetNoteheadReversed(true); to true or false,
    //depending on the requiered notehead position for the chord.

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
	// - font is nor set
	// - container shape is not craeted
	// In following loop all these issues are fixed
	//-----------------------------------------------------------------------------------
    int iN;
    lmNote* pNote;
    wxNotesListNode* pNode = m_cNotes.GetFirst();
    for(iN=1; pNode; pNode=pNode->GetNext(), iN++ )
	{
		pNote = (lmNote*)pNode->GetData();
		if (iN > 1) //skip base note
		{
			//assign paper pos to this note.
			pNote->SetReferencePos(m_pBaseNote->GetReferencePaperPos());
		}
		//set the font
		//pNote->SetFont(pPaper);
		//create the shape container
		pNote->CreateContainerShape(pBox, uPaperPos.x, uPaperPos.y, colorC);
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
    pNode = m_cNotes.GetFirst();
    for(iN=1; pNode; pNode=pNode->GetNext(), iN++ )
	{
        pNote = (lmNote*)pNode->GetData();
        if (!pNote->IsNoteheadReversed() && pNote->HasAccidentals())
            ComputeAccidentalLayout(true, pNote, iN, pPaper, uPaperPos, colorC);
    }
    //second loop: process notes  shitfted to right
    pNode = m_cNotes.GetFirst();
    for(iN=1; pNode; pNode=pNode->GetNext(), iN++ )
	{
        pNote = (lmNote*)pNode->GetData();
        if (pNote->IsNoteheadReversed() && pNote->HasAccidentals())
            ComputeAccidentalLayout(false, pNote, iN, pPaper, uPaperPos, colorC);
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
    pNode = m_cNotes.GetFirst();
    for(iN=1; pNode; pNode=pNode->GetNext(), iN++ )
    {
        //get the note
        pNote = (lmNote*)pNode->GetData();
		lmShapeNote* pNoteShape = (lmShapeNote*)pNote->GetShap2();

        //compute offset
        yStaffTopLine = pNote->GetStaffOffset();   // staff y position (top line)
        yPos = yStaffTopLine - pNote->GetPitchShift();
        xPos = 0;
        if (pNote->HasAccidentals()) {
            pAccidental = pNote->GetAccidentals();
            xPos += pAccidental->GetWidth();
            //add accidental shape
            pNoteShape->AddAccidental(pAccidental->GetShape());
        }

        //compute notehead's position
		///*dbg*/ wxLogMessage(_T("[lmChord::LayoutNoteHeads] adding note %d, xPos=%.2f, reversed=%s"),
		//      	iN, xPos + uPaperPos.x, (pNote->IsNoteheadReversed() ? _T("yes") : _T("no")) );
		pNote->AddNoteShape(pNoteShape, pPaper, xPos + uPaperPos.x, yPos + uPaperPos.y, colorC);
        pNoteHead = pNote->GetNoteheadShape();
        //check if collision with any previous note accidentals
        pCrashNote = CheckIfNoteCollision(pNoteHead);
        while (pCrashNote) {
            //try to render at right of colliding accidental
			lmLUnits uShift = (pCrashNote->GetAccidentals())->GetWidth();
			pNote->ShiftNoteHeadShape(uShift);
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
    pNode = m_cNotes.GetFirst();
	int iNote=0;
    for(; pNode; pNode=pNode->GetNext())
	{
        pNote = (lmNote*)pNode->GetData();
        uMaxAnchor = wxMax(uMaxAnchor, pNote->GetAnchorPos());
		///*dbg*/ wxLogMessage(_T("[lmChord::LayoutNoteHeads] note %d, uMaxAnchor=%.2f, pNote->GetAnchorPos = %.2f"),
		//          	++iNote, uMaxAnchor, pNote->GetAnchorPos() );
    }

	//b) Add a shift to each note so that its anchor line become the max anchor line
	///*dbg*/ wxLogMessage(_T("[lmChord::LayoutNoteHeads] Adding a shift to each note:"));
    lmLUnits uShift;
    pNode = m_cNotes.GetFirst();
    for(; pNode; pNode=pNode->GetNext())
	{
        pNote = (lmNote*)pNode->GetData();
        pNoteHead = pNote->GetNoteheadShape();
        uShift = uMaxAnchor - pNote->GetAnchorPos();
		///*dbg*/ wxLogMessage(_T("[lmChord::LayoutNoteHeads] uShift=%.2f"), uShift );
        pNote->ShiftNoteHeadShape(uShift);
    }


}

lmNote* lmChord::CheckIfCollisionWithAccidentals(bool fOnlyLeftNotes, int iCurNote, lmShape* pShape)
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

lmNote* lmChord::CheckIfNoteCollision(lmShape* pShape)
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
