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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "Note.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/image.h"
#include "wx/debug.h"

#include "Score.h"
#include "Glyph.h"
#include "UndoRedo.h"
#include "../ldp_parser/AuxString.h"
#include "../graphic/Shapes.h"
#include "../graphic/ShapeNote.h"
#include "../graphic/ShapeBeam.h"
#include "../graphic/GMObject.h"


//global static variables common to all notes

static bool m_fBeingBuilt = false;		//true when constructor not yet finished
static lmContext* m_pContext = NULL;	//contains updated context when previous flag is true

//AWARE: Rationale for these two previous variables
//		 -------------------------------------------
//		 Methods (in VStaff) to get a context need a reference to the StaffObj. During
//note creation (lmNote constructor), as the note is not yet included
//in the StaffObjs collection, these get context methods will fail.
//		During note creation it is not necessary to access this methods, as the
//note constructor receives as parameter, an updated context. But invocation
//of other lmNote methods in the constructor could (in fact, does) cause
//problems because they will invoke get context methods.
//		As solution, I will have a temporary flag to signal that the note is being
//created and that context is stored in a temporary variable. All lmNote
//methods requiring a context will check this flag and, if raised, will use
//the stored context instead of asking the VStaff for the context.


const bool m_fDrawSmallNotesInBlock = false;    //TODO option depending on used font


lmNote::lmNote(lmVStaff* pVStaff, lmEPitchType nPitchType,
        wxString& sStep, wxString& sOctave, wxString& sAlter,
        lmEAccidentals nAccidentals,
        lmENoteType nNoteType, float rDuration,
        bool fDotted, bool fDoubleDotted,
        int nStaff, int nVoice, bool fVisible,
        lmContext* pContext,
        bool fBeamed, lmTBeamInfo BeamInfo[],
        bool fInChord,
        bool fTie,
        lmEStemType nStem)  :
    lmNoteRest(pVStaff, lmDEFINE_NOTE, nNoteType, rDuration, fDotted, fDoubleDotted, nStaff,
               nVoice, fVisible)
{
    // Diatonic Pitch is determined by Step
    // This constructor is used for two kind of pitch information:
    //  -    MusicXML style: sound and look information totally independent; pitch is absolute, and
    //    is implied by combining Step, Octave and Alter. Accidentals are just for rendering.
    //  - LDP style: notes defined as in hand writing; pitch depends on context (key signature
    //    and accidentals introduced by a previous note in the same measure) and its own
    //    accidentals. Therefore pitch is computred from Step, Octave, Accidentals and Context.
    //    Alter is not used.
    // To support both kinds of definitions, parameter fAbsolutePitch indicates that pitch
    // information is absolute (MusicXML style) or relative (LDP style).

	//AWARE: During lmNote construction is not possible to use methods to get contexts.
	//They will fail because the note is not yet included in the ColStaffObjs.
	//Therefore, all inititialization MUST use the received context pContext. VERIFY THIS
	//WHEN INVOKING OTHER METHODS FROM CONSTRUCTOR


	//Constructor starts
	m_fBeingBuilt = true;
	m_pContext = pContext;

    //shape initialization
	//AWARE: Althoug shape pointer is initialized to NULL never assume that there is
	//a shape if not NULL, as the shape is deleted in the graphic model.
    m_pNoteheadShape = (lmShapeGlyph*)NULL;
    m_pStemShape = (lmShapeStem*)NULL;

    // stem information
    m_nStemType = nStem;
    m_uStemLength = GetDefaultStemLength();     //default. If beamed note, this value will
                                                //be updated in lmBeam::CreateShape
    ////DBG
    //if (fTie) {   //GetID() == 13) {
    //    // break here
    //    int nDbg = 0;
    //}

    //get octave, step and context accidentals for this note step
    long nAux;
    sOctave.ToLong(&nAux);
    int nOctave = (int)nAux;
    int nStep = LetterToStep(sStep);
    int nCurContextAcc = pContext->GetAccidentals(nStep);
    int nNewContextAcc = nCurContextAcc;
    lmEAccidentals nDisplayAcc = nAccidentals;

    //update context with displayed accidentals or with alterations,
    //and store all pitch related info
    if (nPitchType == lm_ePitchAbsolute)
    {
        // Pitch is absolute and is defined by combining Step, Octave and Alter.
        // Accidentals are just for rendering.
        // Sound and look information totally independent

        //set pitch and accidentals to display
        sAlter.ToLong(&nAux);
        nNewContextAcc = (int)nAux;
        m_anPitch.Set(nStep, nOctave, nNewContextAcc);
        nDisplayAcc = ComputeAccidentalsToDisplay(nCurContextAcc, nNewContextAcc);
    }

    else if (nPitchType == lm_ePitchRelative)
    {
        // Relative pitch: pitch depends on context (key signature
        // and accidentals introduced by a previous note in the same measure) and its own
        // accidentals. Therefore pitch is computred from Step, Octave, Accidentals and Context.
        // Alter is not used.

        //update context with accidentals
        switch (nAccidentals) {
            case lm_eNoAccidentals:
                //do not modify context
                break;
            case eNatural:
                //ignore context. Force 'natural' (=no accidentals)
                nNewContextAcc = 0;
                break;
            case lm_eFlat:
            case lm_eNaturalFlat:
                //ignore context. Put one flat
                nNewContextAcc = -1;
                break;
            case lm_eSharp:
            case lm_eNaturalSharp:
                //ignore context. Put one sharp
                nNewContextAcc = 1;
                break;
            case lm_eFlatFlat:
                //ignore context. Put two flats
                nNewContextAcc = -2;
                break;
            case lm_eSharpSharp:
            case lm_eDoubleSharp:
                //ignore context. Put two sharps
                nNewContextAcc = 2;
                break;
            default:
                ;
        }

        //set pitch and displayed accidentals
        m_anPitch.Set(nStep, nOctave, nNewContextAcc);
        nDisplayAcc = nAccidentals;
    }

    else if (nPitchType == lm_ePitchNotDefined)
    {
        m_anPitch.Set(-1, 0);
        nDisplayAcc = lm_eNoAccidentals;
    }

    else {
        wxLogMessage(_T("[lmNote] unknown nPitchType %d"), nPitchType);
        wxASSERT(false);
    }

    // create the accidentals
    m_pAccidentals = CreateAccidentals(nDisplayAcc);

    //initialize stem
    SetUpStemDirection();       // standard lenght

    // MIDI volume information can not be computed until the note is created (constructor
    // method finished) and the note added to the VStaff collection. Otherwise
    // timing information is not valid.
    // So I am going to set a default initial value meaning 'no initialized'
    m_nVolume = -1;

    //if the note is part of a chord find the base note and take some values from it
    m_pChord = (lmChord*)NULL;		//by defaul note is not in chord
    m_fNoteheadReversed = false;
    if (fInChord) {
        if (!g_pLastNoteRest || g_pLastNoteRest->IsRest()) {
            ; //TODO
            wxLogMessage(_T("[lmNote::lmNote] Specified 'note in chord' but no base note yet defined. Chord ignored"));
        }
        else
		{
            lmNote* pLastNote = (lmNote*)g_pLastNoteRest;
            if (pLastNote->IsInChord())
                m_pChord = pLastNote->GetChord();
            else
				m_pChord = new lmChord(pLastNote);

            m_pChord->Include(this);
        }
    }


    //lmTie information ----------------------------------------------------
    m_fNeedToBeTied = fTie;
    m_pTiePrev = (lmTie*)NULL;        //assume no tied
    m_pTieNext = (lmTie*)NULL;        //assume no tied

    // verify if a previous note is tied to this one and if so, build the tie
    lmNote* pNtPrev = m_pVStaff->FindPossibleStartOfTie(m_anPitch);
    if (pNtPrev && pNtPrev->NeedToBeTied())
	{
        //do the tie between previous note and this one
        CreateTie(pNtPrev, this);

        //if stem direction is not forced, choose it to be as that of the start of tie note
        if (nStem == lmSTEM_DEFAULT)
            m_fStemDown = pNtPrev->StemGoesDown();
    }

    // Generate beaming information -----------------------------------------------------
    CreateBeam(fBeamed, BeamInfo);

    if (!IsInChord() || IsBaseOfChord()) g_pLastNoteRest = this;

    //initializations for renderization
    m_uSpacePrev = 0;

	//Constructor ends
	m_fBeingBuilt = false;
	m_pContext = (lmContext*)NULL;

}

lmNote::~lmNote()
{
    //if note in chord, remove it from the chord. Delete the chord when only one note left in it.
    if (IsInChord()) {
        m_pChord->Remove(this);
        if (m_pChord->NumNotes() == 1) {
            delete m_pChord;
            m_pChord = (lmChord*)NULL;
        }
    }

    //delete the Ties.
    if (m_pTiePrev) {
        m_pTiePrev->Remove(this);
        delete m_pTiePrev;
    }
    m_pTiePrev = (lmTie*)NULL;
    if (m_pTieNext) {
        m_pTieNext->Remove(this);
        delete m_pTieNext;
    }
    m_pTieNext = (lmTie*)NULL;

    //delete accidentals
    if (m_pAccidentals) {
        delete m_pAccidentals;
        m_pAccidentals = (lmAccidental*)NULL;
    }

}

lmAccidental* lmNote::CreateAccidentals(lmEAccidentals nAcc)
{
    // create the accidentals object

    if (nAcc != lm_eNoAccidentals)
        return new lmAccidental(this, nAcc);
    else
        return (lmAccidental*)NULL;
}

void lmNote::CreateTie(lmNote* pNtPrev, lmNote* pNtNext)
{
    //create a tie between the two notes. One of them is this one. The other one can
    //be NULL and in this case the tie is not created.

    if (pNtPrev && pNtNext)
    {
        lmTie* pTie = new lmTie(pNtPrev, pNtNext);
        pNtNext->SetTiePrev(pTie);
        pNtPrev->SetTieNext(pTie);
    }
    else if (!pNtPrev)
        SetTiePrev((lmTie*)NULL);
    else if (!pNtNext)
        SetTieNext((lmTie*)NULL);
}

lmEClefType lmNote::GetClefType()
{
	//returns the applicable clef for this note

	//during note construction we have the context. use it
	if (m_fBeingBuilt)
		return m_pContext->GetClef()->GetClefType();

	//in other cases, get the context
	lmContext* pContext = GetCurrentContext();
    if (pContext)
		return pContext->GetClef()->GetClefType();
	else
		return lmE_Undefined;
}

lmTimeSignature* lmNote::GetTimeSignature()
{
	//returns the applicable time signauture for this note

	//during note construction we have the context. use it
	if (m_fBeingBuilt)
		return m_pContext->GetTime();

	//in other cases, get the context
	lmContext* pContext = GetCurrentContext();
    if (pContext)
		return pContext->GetTime();
	else
		return (lmTimeSignature*)NULL;
}

int lmNote::GetContextAccidentals(int nStep)
{
	//returns the context number of accidentals for this note and step

	//during note construction we have the context. use it
	if (m_fBeingBuilt)
		return m_pContext->GetAccidentals(nStep);

	//in other cases, get the context
	return m_pVStaff->GetUpdatedContextAccidentals(this, nStep);
}

bool lmNote::OnContextUpdated(int nStep, int nNewAccidentals, lmContext* pNewContext)
{
    // A previous note has updated the context. It is necessary to verify if this note is
    // affected and, if it is affected, to update context dependent information.
    // Returns true if no modification was needed.

    ////get context accidentals for the modified step
    //int nAccidentals = pNewContext->GetAccidentals(nStep);

    ////compare context accidentals for modified step with new accidentals
    //if (nAccidentals == nNewAccidentals) return true;

    ////update pitch
    //m_anPitch.SetAccidentals(nNewAccidentals);
    //return false;
    return true;	//TODO
}

void lmNote::RemoveTie(lmTie* pTie)
{
    if (m_pTiePrev == pTie) {
        m_pTiePrev = (lmTie*)NULL;
    }
    else if (m_pTieNext == pTie) {
        m_pTieNext = (lmTie*)NULL;
    }
}

bool lmNote::IsBaseOfChord() 
{ 
	return m_pChord && m_pChord->IsBaseNote(this);
}

void lmNote::ComputeVolume()
{
    // If volume is not set assign a value
    if (m_nVolume != -1) return;    //volume already set. Do nothing.

    // Volume should depend on several factors: beak (strong, medium, weak) on which
    // this note, phrase, on dynamics information, etc. For now we are going to
    // consider only beat information
    lmTimeSignature* pTS = GetTimeSignature();
    if (pTS)
        m_nVolume = AssignVolume(m_rTimePos, pTS->GetNumBeats(), pTS->GetBeatType());
    else
        m_nVolume = 64;
}

int lmNote::GetPositionInBeat()
{
    lmTimeSignature* pTS = GetTimeSignature();
    if (pTS)
        return GetNoteBeatPosition(m_rTimePos, pTS->GetNumBeats(), pTS->GetBeatType());
    else
        return lmUNKNOWN_BEAT;
}

int lmNote::GetChordPosition()
{
    lmTimeSignature* pTS = GetTimeSignature();
    if (pTS)
        return ::GetChordPosition(m_rTimePos, m_rDuration, pTS->GetNumBeats(), pTS->GetBeatType());
    else
        return lmNON_CHORD_NOTE;
}

void lmNote::CreateContainerShape(lmBox* pBox, lmLUnits uxLeft, lmLUnits uyTop, wxColour colorC)
{
    //create the container shape and add it to the box
    lmShapeNote* pNoteShape = new lmShapeNote(this, uxLeft, uyTop, colorC);
	pBox->AddShape(pNoteShape);
    m_pShape = pNoteShape;
}


//====================================================================================================
// implementation of virtual methods defined in base abstract class lmNoteRest
//====================================================================================================

lmUPoint lmNote::ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper)
{
	// if no location is specified in LDP source file, this method is invoked from
	// base class to ask derived object to compute a suitable position to
	// place itself.
	// uOrg is the assigned paper position for this object.

	lmUPoint uPos = uOrg;
	//TODO
	return uPos;
}

lmLUnits lmNote::LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC)
{
    // This method is invoked by the base class (lmStaffObj). It is responsible for
    // creating the shape object and adding it to the graphical model.

 //   //DBG
 //   if (GetID() == 25)
	//{
 //       int nDbg = 0;	// set break point here
 //   }

    bool fDrawStem = true;            // assume stem
    bool fInChord = IsInChord();

    //prepare paper for measurements
    pPaper->SetFont(*GetSuitableFont(pPaper));

    // move to right staff
    lmLUnits uyStaffTopLine = uPos.y + GetStaffOffset();   // staff y position (top line)
    lmLUnits uxLeft=0, uyTop=0;    // current pos. as positioning computation takes place
    int nPosOnStaff = GetPosOnStaff();
    lmLUnits uyPitchShift = GetPitchShift();
    uyTop = uyStaffTopLine - uyPitchShift;
    uxLeft = uPos.x;

	// create the shape for the stem because it must be measured even if it must not be
	// drawn. This is necessary, for example, to have information for positioning
	// tuplets' bracket. Or to align notes in a chord.
	// If the shape is finally not needed (i.e. the note doesn't have stem)
	// the shape will be deleted. Otherwise, it will be added to the note shape.
    //-----------------------------------------------------------------------------------
    //TODO move thickness to user options
    #define STEM_WIDTH   12     //stem line width (cents = tenths x10)
    m_uStemThickness = m_pVStaff->TenthsToLogical(STEM_WIDTH, m_nStaffNum) / 10;
	bool fStemAdded = false;
	m_pStemShape = new lmShapeStem(this, 0.0, 0.0, 0.0,
                                   m_fStemDown, m_uStemThickness, colorC);


    //if this is the first note of a chord, give lmChord the responsibility for
	//layouting the chord (only note heads, accidentals and rests). If it is
    //any other note of a chord, do nothing and mark the note as 'already layouted'.
    bool fNoteLayouted = false;
    if (IsBaseOfChord())
	{
        m_pChord->LayoutNoteHeads(pBox, pPaper, uPos, colorC);
        fNoteLayouted = true;
    }
    else if (IsInChord())
	{
        fNoteLayouted = true;
    }
	else
	{
		//Isolated note. create the container shape and add it to the box
		CreateContainerShape(pBox, uxLeft, uyTop, colorC);
	}
	lmShapeNote* pNoteShape = (lmShapeNote*)GetShap2();



    //if this is the first note/rest of a beam, create the beam
    //AWARE This must be done before using stem information, as the beam could
    //change stem direction if it is not determined for some/all the notes in the beam
    if (m_pBeam && m_BeamInfo[0].Type == eBeamBegin) {
        m_pBeam->AutoSetUp();
        m_pBeam->CreateShape();
    }


    //create shapes for accidental signs if exist and note not in chord
    //------------------------------------------------------------------
    if (m_pAccidentals) {
        if (!fNoteLayouted) {
            m_pAccidentals->Layout(pPaper, uxLeft, uyTop);
			pNoteShape->AddAccidental(m_pAccidentals->GetShape());
        }
        uxLeft += m_pAccidentals->GetWidth();
    }

    //advance space before note
    uxLeft += m_uSpacePrev;

    //render the notehead (or the full note if single glyph)
    if (!fNoteLayouted)
	{
        fDrawStem = AddNoteShape(pNoteShape, pPaper, uxLeft, uyTop, colorC);
    }
    uxLeft += m_pNoteheadShape->GetWidth();
    //lmLUnits uxNote = uxLeft;

    //create shapes for dots if necessary
    //------------------------------------------------------------
    if (m_fDotted || m_fDoubleDotted) {
        //TODO user selectable
        lmLUnits uSpaceBeforeDot = m_pVStaff->TenthsToLogical(5, m_nStaffNum);
        uxLeft += uSpaceBeforeDot;
        lmLUnits uyPos = uyTop;
        if (nPosOnStaff % 2 == 0) {
            // notehead is over a line. Shift up the dots by half line
            uyPos -= m_pVStaff->TenthsToLogical(5, m_nStaffNum);
        }
        uxLeft += AddDotShape(pNoteShape, pPaper, uxLeft, uyPos, colorC);
        if (m_fDoubleDotted) {
            uxLeft += uSpaceBeforeDot;
            uxLeft += AddDotShape(pNoteShape, pPaper, uxLeft, uyPos, colorC);
        }
    }

    // measure the stem and add the shape for the stem, if necessary
    //-----------------------------------------------------------------------------------
    //set stem x position
	//line is centered on stem x position, so we must add/substract half stem thickness
	lmLUnits uxStem;
    if (m_fStemDown) {
        //stem down: line down on the left of the notehead unless notehead reversed
		uxStem = m_pNoteheadShape->GetXLeft() + m_uStemThickness / 2;
		if (m_fNoteheadReversed) uxStem += m_pNoteheadShape->GetWidth();
    } else {
        //stem up: line up on the right of the notehead unless notehead reversed
		uxStem = m_pNoteheadShape->GetXRight() - m_uStemThickness / 2;
        if (m_fNoteheadReversed) uxStem -= m_pNoteheadShape->GetWidth();
    }

	//now proceed to create the shape, if necessary
    lmLUnits uyFlag = 0.0;          //y pos for flag
	lmLUnits uyStemStart = 0.0;		//the nearest point to notehead
	if (m_nStemType != lmSTEM_NONE)
    {
        // compute and store start position of stem
        if (m_fStemDown) {
            //stem down: line down on the left of the notehead
            uyTop += m_pVStaff->TenthsToLogical(51, m_nStaffNum);
            uyStemStart = uyTop;
        } else {
            //stem up: line up on the right of the notehead
            uyTop += m_pVStaff->TenthsToLogical(49, m_nStaffNum);
            uyStemStart = uyTop;
        }

		//if flag to be drawn, adjust stem size and compute flag position
        if (!m_fDrawSmallNotesInBlock && !m_pBeam && m_nNoteType > eQuarter) {
            int nGlyph = GetGlyphForFlag();
            lmLUnits uStemLength = GetStandardStemLenght();
            // to measure flag and stem I am going to use some glyph data. These
            // data is in FUnits but as 512 FU are 1 line (10 tenths) it is simple
            // to convert these data into tenths: just divide FU by 51.2
            float rFlag, rMinStem;
            if (m_fStemDown) {
                rFlag = fabs((2048.0 - (float)aGlyphsInfo[nGlyph].Bottom) / 51.2 );
                rMinStem = ((float)aGlyphsInfo[nGlyph].Top - 2048.0 + 128.0) / 51.2 ;
            }
            else {
                if (m_nNoteType == eEighth)
                    rFlag = ((float)aGlyphsInfo[nGlyph].Top) / 51.2 ;
                else if (m_nNoteType == e16th)
                    rFlag = ((float)aGlyphsInfo[nGlyph].Top + 128.0) / 51.2 ;
                else
                    rFlag = ((float)aGlyphsInfo[nGlyph].Top + 512.0) / 51.2 ;

                rMinStem = fabs( (float)aGlyphsInfo[nGlyph].Bottom / 51.2 );
            }
            lmLUnits uFlag = m_pVStaff->TenthsToLogical(rFlag, m_nStaffNum);
            lmLUnits uMinStem = m_pVStaff->TenthsToLogical(rMinStem, m_nStaffNum);
            uStemLength = wxMax((uStemLength > uFlag ? uStemLength-uFlag : 0), uMinStem);
            uyFlag = uyStemStart + (m_fStemDown ? uStemLength : -uStemLength);
            SetStemLength(uStemLength + uFlag);
        }

		//adjust the position and size of the stem shape
		lmLUnits uyStemEnd = uyStemStart + (m_fStemDown? m_uStemLength : -m_uStemLength);
		m_pStemShape->Adjust(uxStem, uyStemStart, uyStemEnd, m_fStemDown);
		//wxLogMessage(_T("[lmNote::LayoutObject] Adjust xPos=%.2f, yTop=%.2f, yBottom=%.2f)"),
		//	uxStem, uyStemStart, uyStemEnd);

        //if not in a chord add the shape for the stem. When the note is in a chord
		//the stem will be created later, when layouting the last note of the chord
        if (fDrawStem && ! fInChord)
		{
			// if beamed, the stem shape will be owned by the beam; otherwise by the note
			if (m_pBeam)
				m_pBeam->AddNoteAndStem(m_pStemShape, pNoteShape, &m_BeamInfo[0]);
			else
				pNoteShape->AddStem(m_pStemShape);
			fStemAdded = true;
        }
    }

    //if this is the last note of a chord draw the stem of the chord
    //-----------------------------------------------------------------------------
    if (IsInChord() && m_pChord->IsLastNoteOfChord(this) && m_nNoteType >= eHalf)
    {
        m_pChord->AddStemShape(pPaper, colorC, GetSuitableFont(pPaper), m_pVStaff,
							   m_nStaffNum);
    }


    //Add shape for flag if necessary
    //-----------------------------------------------------------------------------
    if (!m_fDrawSmallNotesInBlock) {
        if (!m_pBeam && m_nNoteType > eQuarter && !IsInChord()) {
            lmLUnits uxPos = (m_fStemDown ? GetXStemLeft() : GetXStemRight());
            AddFlagShape(pNoteShape, pPaper, lmUPoint(uxPos, uyFlag), colorC);
        }
    }

    // add shapes for leger lines if necessary
    //--------------------------------------------
    lmLUnits uxLine = m_pNoteheadShape->GetXLeft() - m_pVStaff->TenthsToLogical(4, m_nStaffNum);
    lmLUnits widthLine = m_pNoteheadShape->GetWidth() +
                            m_pVStaff->TenthsToLogical(8, m_nStaffNum);
    AddLegerLineShape(pNoteShape, pPaper, nPosOnStaff, uyStaffTopLine, uxLine,
                        widthLine, m_nStaffNum);


	//if this is the last note of a multi-attached AuxObj add the shape for the aux obj
    //----------------------------------------------------------------------------------

    // beam lines
    if (IsInChord() && m_pChord->IsLastNoteOfChord(this))
	{
		lmNote* pBaseNote = m_pChord->GetBaseNote();
		if (pBaseNote->IsBeamed() && pBaseNote->GetBeamType(0) == eBeamEnd)
			pBaseNote->GetBeam()->LayoutObject(pBox, pPaper, colorC);
    }
    else if (!IsInChord() && m_pBeam && m_BeamInfo[0].Type == eBeamEnd)
	{
        m_pBeam->LayoutObject(pBox, pPaper, colorC);
    }

    // tuplet bracket
    if (m_pTuplet && (m_pTuplet->GetEndNoteRest())->GetID() == m_nId)
		m_pTuplet->LayoutObject(pBox, pPaper, colorC);

    //ties
    if (m_pTiePrev)
		m_pTiePrev->LayoutObject(pBox, pPaper, colorC);

	//clear temporary info
	if (!fStemAdded && (!IsInChord() || m_nNoteType < eHalf))
		DeleteStemShape();

	//for chords it must return the maximum width
    #define NOTE_AFTERSPACE     0      //TODO user options
    lmLUnits uAfterSpace = m_pVStaff->TenthsToLogical(NOTE_AFTERSPACE, m_nStaffNum);

	if (IsInChord())
		return m_pChord->GetXRight() - uPos.x + uAfterSpace;
	else
		return m_pShape->GetXRight() - uPos.x + uAfterSpace;

}

void lmNote::PlaybackHighlight(lmPaper* pPaper, wxColour colorC)
{
	//FIX_ME: there can be many views. Should only the active view be higlighted?
	//FIX_ME: m_pNoteheadShape is only valid during layout. And there can be many views!!

	m_pNoteheadShape->Render(pPaper, colorC);
}

void lmNote::CursorHighlight(lmPaper* pPaper, int nStaff, bool fHighlight)
{
	//FIX_ME: there can be many views. Should only the active view be higlighted?
	//FIX_ME: m_pNoteheadShape is only valid during layout. And there can be many views!!

    if (fHighlight)
    {
        m_pNoteheadShape->Render(pPaper, g_pColors->CursorColor());
    }
    else
    {
        //IMPROVE
        // If we paint in black it remains a coloured aureole around
        // the note. By painting it first in white the size of the aureole
        // is smaller but still visible. A posible better solution is to
        // modify Render method to accept an additional parameter: a flag
        // to signal that XOR draw mode in colour followed by a normal
        // draw in BLACK must be done.

        m_pNoteheadShape->Render(pPaper, *wxWHITE);
        m_pNoteheadShape->Render(pPaper, g_pColors->ScoreNormal());
    }
}

lmLUnits lmNote::GetPitchShift()
{
    int nPosOnStaff = GetPosOnStaff();
    lmLUnits uShift = m_pVStaff->TenthsToLogical(nPosOnStaff * 10, m_nStaffNum ) / 2 ;
    if (nPosOnStaff > 0 && nPosOnStaff < 12)
        return uShift;
    else {
        if (nPosOnStaff > 11) {
            // lines at top
            lmTenths nDsplz = (lmTenths) m_pVStaff->GetOptionLong(_T("Staff.UpperLegerLines.Displacement"));
            lmLUnits uyDsplz = m_pVStaff->TenthsToLogical(nDsplz, m_nStaffNum);
            return uShift + uyDsplz;
        }
        else
            return uShift;
    }

}

bool lmNote::AddNoteShape(lmShapeNote* pNoteShape, lmPaper* pPaper, lmLUnits uxLeft,
                        lmLUnits uyTop, wxColour colorC)
{
    // creates the shape for the notehead (or for the full note if single glyph) and
    // returns flag to draw or not the stem
	// Updates note positioning variables: m_uxStem

    bool fDrawStem = true;

    if (m_fDrawSmallNotesInBlock && !m_pBeam && m_nNoteType > eQuarter && !IsInChord())
    {
        // It is a single note with flag: draw it in one step with a glyph
        AddSingleNoteShape(pNoteShape, pPaper, m_nNoteType, m_fStemDown, uxLeft, uyTop, colorC);
        fDrawStem = false;
    }
    else
    {
        // either the note is part of a group of beamed notes, is in a chord, or doesn't
        // have stem: it must be drawn in parts
        // Create the notehead shape
        lmENoteHeads nNotehead;
        //if (! m_fCabezaX) {
            if (m_nNoteType > eHalf) {
                nNotehead = enh_Quarter;
            } else if (m_nNoteType == eHalf) {
                nNotehead = enh_Half;
            } else if (m_nNoteType == eWhole) {
                nNotehead = enh_Whole;
                fDrawStem = false;
            } else if (m_nNoteType == eBreve) {
                nNotehead = enh_Breve;
                fDrawStem = false;
            } else if (m_nNoteType == eLonga) {
                nNotehead = enh_Longa;
                fDrawStem = false;
            } else {
                wxLogMessage(_T("[lmNote::AddNoteShape] Unknown note type."));
                wxASSERT(false);
            }
        //} else {
        //    nNotehead = enh_Cross;
        //}

        AddNoteHeadShape(pNoteShape, pPaper, nNotehead, uxLeft, uyTop, colorC);
    }

    return fDrawStem;

}

lmEGlyphIndex lmNote::DrawFlag(bool fMeasuring, lmPaper* pPaper, lmUPoint uPos, wxColour colorC)
{
    //
    //Draws the flag using a glyph. Returns the glyph index
    //

    lmEGlyphIndex nGlyph = GLYPH_EIGHTH_FLAG_DOWN;
    switch (m_nNoteType) {
        case eEighth :
            nGlyph = (m_fStemDown ? GLYPH_EIGHTH_FLAG_DOWN : GLYPH_EIGHTH_FLAG_UP);
            break;
        case e16th :
            nGlyph = (m_fStemDown ? GLYPH_16TH_FLAG_DOWN : GLYPH_16TH_FLAG_UP);
            break;
        case e32th :
            nGlyph = (m_fStemDown ? GLYPH_32ND_FLAG_DOWN : GLYPH_32ND_FLAG_UP);
            break;
        case e64th :
            nGlyph = (m_fStemDown ? GLYPH_64TH_FLAG_DOWN : GLYPH_64TH_FLAG_UP);
            break;
        case e128th :
            nGlyph = (m_fStemDown ? GLYPH_128TH_FLAG_DOWN : GLYPH_128TH_FLAG_UP);
            break;
        case e256th :
            nGlyph = (m_fStemDown ? GLYPH_256TH_FLAG_DOWN : GLYPH_256TH_FLAG_UP);
            break;
        default:
            wxLogMessage(_T("[lmNote::DrawFlag] Error: invalid note type %d."),
                        m_nNoteType);
        }

    wxString sGlyph( aGlyphsInfo[nGlyph].GlyphChar );

    if (!fMeasuring) {
        // drawing phase: do the draw
        pPaper->SetTextForeground(colorC);
        //wxLogMessage(_T("[lmNote::DrawFlag] x=%d, y=%d"), uPos.x, uPos.y + m_pVStaff->TenthsToLogical( aGlyphsInfo[nGlyph].GlyphOffset, m_nStaffNum ));
        pPaper->DrawText(sGlyph, uPos.x, uPos.y + m_pVStaff->TenthsToLogical( aGlyphsInfo[nGlyph].GlyphOffset, m_nStaffNum ) );
    }

    return nGlyph;

}

lmEGlyphIndex lmNote::GetGlyphForFlag()
{
    //Returns the flag to use with this note

    lmEGlyphIndex nGlyph = GLYPH_EIGHTH_FLAG_DOWN;
    switch (m_nNoteType)
	{
        case eEighth :
            nGlyph = (m_fStemDown ? GLYPH_EIGHTH_FLAG_DOWN : GLYPH_EIGHTH_FLAG_UP);
            break;
        case e16th :
            nGlyph = (m_fStemDown ? GLYPH_16TH_FLAG_DOWN : GLYPH_16TH_FLAG_UP);
            break;
        case e32th :
            nGlyph = (m_fStemDown ? GLYPH_32ND_FLAG_DOWN : GLYPH_32ND_FLAG_UP);
            break;
        case e64th :
            nGlyph = (m_fStemDown ? GLYPH_64TH_FLAG_DOWN : GLYPH_64TH_FLAG_UP);
            break;
        case e128th :
            nGlyph = (m_fStemDown ? GLYPH_128TH_FLAG_DOWN : GLYPH_128TH_FLAG_UP);
            break;
        case e256th :
            nGlyph = (m_fStemDown ? GLYPH_256TH_FLAG_DOWN : GLYPH_256TH_FLAG_UP);
            break;
        default:
            wxLogMessage(_T("[lmNote::GetGlyphForFlag] Error: invalid note type %d."),
                        m_nNoteType);
    }

    return nGlyph;

}

lmEGlyphIndex lmNote::AddFlagShape(lmShapeNote* pNoteShape, lmPaper* pPaper, lmUPoint uPos,
                                   wxColour colorC)
{
    //Adds the flag shape. Returns the glyph index

    lmEGlyphIndex nGlyph = GetGlyphForFlag();

    lmLUnits yPos = uPos.y + m_pVStaff->TenthsToLogical( aGlyphsInfo[nGlyph].GlyphOffset, m_nStaffNum );
    lmShapeGlyph* pShape = new lmShapeGlyph(this, nGlyph, GetSuitableFont(pPaper), pPaper,
                                            lmUPoint(uPos.x, yPos), _T("Flag"));
	pNoteShape->AddFlag(pShape);
    return nGlyph;

}

void lmNote::ShiftNoteHeadShape(lmLUnits uxShift)
{
    //reposition the note shape and all directly related measurements: stem x pos
    //This method does not change the accidentals position, only the note head itself.

    m_pNoteheadShape->Shift(uxShift, 0.0);	// shift notehead horizontally
    //m_uxStem += uxShift;

}

lmLUnits lmNote::GetAnchorPos()
{
    ////wxLogMessage(_T("[lmNote::GetAnchorPos()] notehead reversed=%s, xLeft=%.2f, xRight=%.2f, width=%.2f"),
    ////    (m_fNoteheadReversed ? _T("yes") : _T("no")),
    ////    m_pNoteheadShape->GetXLeft(), m_pNoteheadShape->GetXRight(),
    ////    m_pNoteheadShape->GetWidth() );

    if (m_fNoteheadReversed) {
        lmLUnits uSize = m_pNoteheadShape->GetWidth();
		lmLUnits uxPos = m_pNoteheadShape->GetXLeft();
        return (StemGoesDown() ? uxPos+uSize : uxPos-uSize);
    }
    else
		return m_pNoteheadShape->GetXLeft();

}

void lmNote::AddLegerLineShape(lmShapeNote* pNoteShape, lmPaper* pPaper, int nPosOnStaff,
                               lmLUnits uyStaffTopLine, lmLUnits uxPos, lmLUnits uWidth,
                               int nStaff)
{
    if (nPosOnStaff > 0 && nPosOnStaff < 12) return;

    lmLUnits uThick = m_pVStaff->GetStaffLineThick(nStaff);
	lmLUnits uBoundsThick = m_pVStaff->TenthsToLogical(2, nStaff);

    int i;
    lmLUnits uyPos;
    int nTenths;
    if (nPosOnStaff > 11) {
        // lines at top
        lmLUnits uDsplz = m_pVStaff->GetOptionLong(_T("Staff.UpperLegerLines.Displacement"));
        lmLUnits uyStart = uyStaffTopLine - m_pVStaff->TenthsToLogical(uDsplz, m_nStaffNum);
        for (i=12; i <= nPosOnStaff; i++) {
            if (i % 2 == 0) {
                nTenths = 5 * (i - 10);
                uyPos = uyStart - m_pVStaff->TenthsToLogical(nTenths, m_nStaffNum);
                //pPaper->SolidLine(uxPos, uyPos, uxPos + uWidth, uyPos, uThick,
                //                   eEdgeNormal, *wxBLACK);
                lmShapeLine* pLine =
                    new lmShapeLine(this, uxPos, uyPos, uxPos + uWidth, uyPos, uThick,
                                    uBoundsThick, *wxBLACK, _T("Leger line"), eEdgeVertical);
	            pNoteShape->Add(pLine);
           }
        }

    } else {
        // nPosOnStaff < 1: lines at bottom
        for (i=nPosOnStaff; i <= 0; i++) {
            if (i % 2 == 0) {
                nTenths = 5 * (10 - i);
                uyPos = uyStaffTopLine + m_pVStaff->TenthsToLogical(nTenths, m_nStaffNum);
                pPaper->SolidLine(uxPos, uyPos, uxPos + uWidth, uyPos, uThick,
                                   eEdgeNormal, *wxBLACK);
                lmShapeLine* pLine =
                    new lmShapeLine(this, uxPos, uyPos, uxPos + uWidth, uyPos, uThick,
                                    uBoundsThick, *wxBLACK, _T("Leger line"), eEdgeVertical);
	            pNoteShape->Add(pLine);
            }
        }
    }

}

void lmNote::AddSingleNoteShape(lmShapeNote* pNoteShape, lmPaper* pPaper, lmENoteType nNoteType,
        bool fStemDown, lmLUnits uxLeft, lmLUnits uyTop, wxColour colorC)
{
    // Creates the shape for a note (including stem) using a single glyph.
    // Adds it to the composite shape pCS.

    lmEGlyphIndex nGlyph = GLYPH_EIGHTH_NOTE_UP;
    switch (nNoteType) {
        case eEighth :
            nGlyph = (m_fStemDown ? GLYPH_EIGHTH_NOTE_DOWN : GLYPH_EIGHTH_NOTE_UP);
            break;
        case e16th :
            nGlyph = (m_fStemDown ? GLYPH_16TH_NOTE_DOWN : GLYPH_16TH_NOTE_UP);
            break;
        case e32th :
            nGlyph = (m_fStemDown ? GLYPH_32ND_NOTE_DOWN : GLYPH_32ND_NOTE_UP);
            break;
        case e64th :
            nGlyph = (m_fStemDown ? GLYPH_64TH_NOTE_DOWN : GLYPH_64TH_NOTE_UP);
            break;
        case e128th :
            nGlyph = (m_fStemDown ? GLYPH_128TH_NOTE_DOWN : GLYPH_128TH_NOTE_UP);
            break;
        case e256th :
            nGlyph = (m_fStemDown ? GLYPH_256TH_NOTE_DOWN : GLYPH_256TH_NOTE_UP);
            break;
        default:
            wxASSERT(false);
    }

    // adjust 'y' glyph position by adding the glyph offset
    lmLUnits yPos = uyTop + m_pVStaff->TenthsToLogical( aGlyphsInfo[nGlyph].GlyphOffset , m_nStaffNum );

    //create the shape object
    lmShapeGlyph* pShape = new lmShapeGlyph(this, nGlyph, GetSuitableFont(pPaper), pPaper,
                                            lmUPoint(uxLeft, yPos), _T("NoteSingle"));
	pNoteShape->AddNoteInBlock(pShape);

}

void lmNote::AddNoteHeadShape(lmShapeNote* pNoteShape, lmPaper* pPaper, lmENoteHeads nNoteheadType,
                              lmLUnits uxLeft, lmLUnits uyTop, wxColour colorC)
{
    // creates the shape for a notehead of type nNoteheadType on position (uxLeft, uyTop)
    // with color colorC. Adds it to the composite shape pCS.

    lmEGlyphIndex nGlyph = GLYPH_NOTEHEAD_QUARTER;
    switch (nNoteheadType) {
        case enh_Longa:
            nGlyph = GLYPH_LONGA_NOTE;
            break;
        case enh_Breve:
            nGlyph = GLYPH_BREVE_NOTE;
            break;
        case enh_Whole:
            nGlyph = GLYPH_WHOLE_NOTE;
            break;
        case enh_Half:
            nGlyph = GLYPH_NOTEHEAD_HALF;
            break;
        case enh_Quarter:
            nGlyph = GLYPH_NOTEHEAD_QUARTER;
            break;
        case enh_Cross:
            nGlyph = GLYPH_NOTEHEAD_CROSS;
            break;
        default:
            wxASSERT(false);
    }

    // adjust 'y' glyph position by adding the glyph offset
    lmLUnits yPos = uyTop - m_pVStaff->TenthsToLogical( aGlyphsInfo[nGlyph].GlyphOffset , m_nStaffNum );

    //create the shape object
    m_pNoteheadShape = new lmShapeGlyph(this, nGlyph, GetSuitableFont(pPaper), pPaper,
                                        lmUPoint(uxLeft, yPos), _T("Notehead"),
										lmDRAGGABLE, colorC);
	pNoteShape->AddNoteHead(m_pNoteheadShape);

}

//void lmNote::OnDrag(lmPaper* pPaper, wxDragImage* pDragImage, lmDPoint& offsetD,
//                         const lmUPoint& pagePosL, const lmUPoint& uDragStartPos, const lmDPoint& canvasPosD)
//{
//    //Constraint: A note must stay on staff lines or spaces
//
//    lmUPoint uPaperPos(pPaper->GetCursorX(), pPaper->GetCursorY());
//
//    lmLUnits dyHalfLine = m_pVStaff->TenthsToLogical(5, m_nStaffNum );
//    lmUPoint nShiftVector = pagePosL - uDragStartPos;    // the displacement
//    int nSteps = (int)(nShiftVector.y / dyHalfLine);        // trim the displacement to half line steps
//    nShiftVector.y -= nSteps;
//    lmUPoint newPaperPos = uPaperPos + nShiftVector;
//    // then the shape must be drawn at:
//    lmDPoint ptNewD;
//    ptNewD.x = pPaper->LogicalToDeviceX(newPaperPos.x + m_uGlyphPos.x) + offsetD.x;
//    ptNewD.y = pPaper->LogicalToDeviceY(newPaperPos.y + m_uGlyphPos.y) + offsetD.y;
//    pDragImage->Move(ptNewD);
//
//    //// compute new pitch
//    //int nNewPitch = PosOnStaffToPitch(nSteps);
//    //SetUpPitchRelatedVariables(nNewPitch);
//}
//
//lmUPoint lmNote::EndDrag(const lmUPoint& uPos)
//{
//    lmUPoint uOldPos(m_uPaperPos + m_uGlyphPos);
//
//    /*
//    Notes can not freely moved. They must stay on staff lines or spaces
//    */
//    lmLUnits udyHalfLine = m_pVStaff->TenthsToLogical(5, m_nStaffNum );
//    lmLUnits uShift = - (uPos.y - GetGlyphPosition().y);
//    int nSteps = (int)(uShift / udyHalfLine);        // trim the displacement to half line steps
//
//    // compute new pitch
//    int nNewPitch = PosOnStaffToPitch(nSteps);
//    SetUpPitchRelatedVariables(nNewPitch);
//
//    //wxLogMessage( wxString::Format(wxT("EndDrag: nShift=%d, nSteps=%d, nNewPitch=%d"),
//    //    nShift, nSteps, nNewPitch ) );
//
//    //ojo: estas dos líneas son el comportamiento de la clase base. Hay que dejarlas
//    //de momento porque el flag m_fFixedPos impide que se actualice la posición
//    // ¿Llevarlo a SetUpPitchRelatedVariables() ?
//    m_uPaperPos.x = uPos.x - m_uGlyphPos.x;
//    m_uPaperPos.y = uPos.y - m_uGlyphPos.y;
//
//
//    return lmUPoint(uOldPos);
//
//}

int lmNote::PosOnStaffToPitch(int nSteps)
{
    // When the note is dragged it is necessary to compute the new pitch from the
    // its new position on the paper. From the paper displacement it is computed how
    // many half line steps the note has been moved. This method receives the steps
    // and computes the new pitch

    int nPos = GetPosOnStaff() + nSteps;
    switch (GetClefType()) {
        case lmE_Sol :
            return nPos + lmC4_DPITCH;
        case lmE_Fa4 :
            return nPos + lmC4_DPITCH - 12;
        case lmE_Fa3 :
            return nPos + lmC4_DPITCH - 10;
        case lmE_Do1 :
            return nPos + lmC4_DPITCH - 2;
        case lmE_Do2 :
            return nPos + lmC4_DPITCH - 4;
        case lmE_Do3 :
            return nPos + lmC4_DPITCH - 6;
        case lmE_Do4 :
            return nPos + lmC4_DPITCH - 8;
        default:
            wxASSERT(false);
            return 0;    //to get the compiler happy
    }

}

void lmNote::SetUpPitchRelatedVariables(lmDPitch nNewPitch)
{
    // A note has been moved. Put pitch to new pitch and current context for
    // new step

    int nStep = m_anPitch.Step();
    int nAcc = GetContextAccidentals(nStep);
    m_anPitch.Set(nNewPitch, nAcc);
    SetUpStemDirection();
}

void lmNote::SetUpStemDirection()
{
	switch (m_nStemType) {
        case lmSTEM_DEFAULT:
            m_fStemDown = (GetPosOnStaff() >= 6);
            break;
        case lmSTEM_DOUBLE:
            /*TODO
                I understand that "lmSTEM_DOUBLE" means two stems: one up and one down.
                This is not yet implemented and is treated as lmSTEM_DEFAULT
            */
            m_fStemDown = (GetPosOnStaff() >= 6);
            break;
        case lmSTEM_UP:
            m_fStemDown = false;
            break;
        case lmSTEM_DOWN:
            m_fStemDown = true;
            break;
        case lmSTEM_NONE:
            m_fStemDown = false;       //false or true. The value doesn't matter.
            break;
        default:
            wxASSERT(false);
    }

    m_uStemLength = GetStandardStemLenght();

    //wxLogMessage( wxString::Format(wxT("SetUpPitchRelatedVariables: tipoStem=%d, plica=%s, nPosOnStaff %d"),
    //    m_nStemType, (m_fStemDown ? _T("Abajo") : _T("Arriba")), GetPosOnStaff() ) );

}

lmLUnits lmNote::GetDefaultStemLength()
{
    // Returns the standard default value for stem lengths. This value is used as
    // starting point for stem computations, for example, to compute the stem in beams.

    // According to engraving rules, normal length is one octave (3.5 spaces)
    #define DEFAULT_STEM_LEGHT      35      //! in tenths. TODO move to user options

    return m_pVStaff->TenthsToLogical(DEFAULT_STEM_LEGHT, m_nStaffNum);
}

lmLUnits lmNote::GetStandardStemLenght()
{
    // Returns the stem lenght that this note should have, according to engraving
    // rules. It takes into account the `posiotion of the note on the staff.
    //
    // a1 - Normal length is one octave (3.5 spaces), but only for notes between the spaces
    //      previous to first leger lines (b3 and b5, in Sol key, both inclusive).
    //
    // a2 - Notes with stems upwards from c5 inclusive, or with stems downwards from
    //      g4 inclusive have a legth of 2.5 spaces.
    //
    // a3 - If a note is on or above the second leger line above the staff, or
    //      on or below the second leger line below the staff: the end of stem
    //      have to touch the middle staff line.


    int nPos = GetPosOnStaff();     //0 = first leger line below staff
    int nTenths;

    // rule a3
    if (nPos >= 14 && m_fStemDown) {
        nTenths = 5 * (nPos-6);     // touch middle line
    }
    else if (nPos <= -2 && !m_fStemDown) {
        nTenths = 5 *(6-nPos);     // touch middle line
    }

    // rule a2
    else if ((nPos >= 7 && !m_fStemDown) || (nPos <= 4 && m_fStemDown)) {
        nTenths = 25;     // 2.5 spaces
    }

    // rule a1 and any other case not covered (I did not analyze completness)
    else {
        nTenths = 35;     // 3.5 spaces
    }

    return m_pVStaff->TenthsToLogical(nTenths, m_nStaffNum);

}

void lmNote::SetStemDirection(bool fStemDown)
{
    m_fStemDown = fStemDown;
    m_uStemLength = GetStandardStemLenght();

    if (IsBaseOfChord()) {
        //propagate change to max and min notes of chord
        m_pChord->SetStemDirection(fStemDown);
    }

}

int lmNote::GetPosOnStaff()
{
    // Returns the position on the staff (line/space) referred to the first ledger line of
    // the staff. Depends on clef:
    //        0 - on first ledger line (C note in G clef)
    //        1 - on next space (D in G clef)
    //        2 - on first line (E not in G clef)
    //        3 - on first space
    //        4 - on second line
    //        5 - on second space
    //        etc.

    // if pitch is not yet defined, return line 0 (first bottom ledger line
    if (!IsPitchDefined()) return 0;

	// pitch is defined. Position will depend on key
    switch (GetClefType()) {
        case lmE_Sol :
            return m_anPitch.ToDPitch() - lmC4_DPITCH;
        case lmE_Fa4 :
            return m_anPitch.ToDPitch() - lmC4_DPITCH + 12;
        case lmE_Fa3 :
            return m_anPitch.ToDPitch() - lmC4_DPITCH + 10;
        case lmE_Fa5 :
            return m_anPitch.ToDPitch() - lmC4_DPITCH + 14;
        case lmE_Do1 :
            return m_anPitch.ToDPitch() - lmC4_DPITCH + 2;
        case lmE_Do2 :
            return m_anPitch.ToDPitch() - lmC4_DPITCH + 4;
        case lmE_Do3 :
            return m_anPitch.ToDPitch() - lmC4_DPITCH + 6;
        case lmE_Do4 :
            return m_anPitch.ToDPitch() - lmC4_DPITCH + 8;
        case lmE_Do5 :
            return m_anPitch.ToDPitch() - lmC4_DPITCH + 10;
        default:
            // no key, assume lmE_Sol
            return m_anPitch.ToDPitch() - lmC4_DPITCH;
    }
}

const lmEAccidentals lmNote::ComputeAccidentalsToDisplay(int nCurContextAcc, int nNewAcc) const
{
    //Current context accidentals for considered step is nCurContextAcc.
    //Note has nNewAcc. This method computes the accidentals to display so that
    //sound has nNewAcc

    lmEAccidentals nDisplayAcc;
    if (nNewAcc == nCurContextAcc)
        nDisplayAcc = lm_eNoAccidentals;
    else if (nNewAcc == 1 && nCurContextAcc == 2)
        nDisplayAcc = lm_eNaturalSharp;
    else if (nNewAcc == -1 && nCurContextAcc == -2)
        nDisplayAcc = lm_eNaturalFlat;
    else if (nNewAcc == 2)
        nDisplayAcc = lm_eDoubleSharp;
    else if (nNewAcc == -2)
        nDisplayAcc = lm_eFlatFlat;
    else if (nNewAcc == 1)
        nDisplayAcc = lm_eSharp;
    else if (nNewAcc == -1)
        nDisplayAcc = lm_eFlat;
    else if (nNewAcc == 0)
        nDisplayAcc = eNatural;
    else {
        wxLogMessage(_T("[lmNote::ComputeAccidentalsToDisplay] Non programmed case: nNewAcc=%d, nCurContextAcc=%d"),
            nNewAcc, nCurContextAcc );
        wxASSERT(false);
    }
    return nDisplayAcc;

}

lmDPitch lmNote::GetDPitch()
{
    if (IsPitchDefined())
        return m_anPitch.ToDPitch();
    else
        return lmC4_DPITCH;
}

lmMPitch lmNote::GetMPitch()
{
    if (IsPitchDefined())
        return m_anPitch.GetMPitch();
    else
        return 60;      // C4 midi key
}

bool lmNote::IsPitchDefined()
{
    return (m_anPitch.ToDPitch() != -1);
}

void lmNote::ChangePitch(lmAPitch nPitch, bool fRemoveTies)
{
    // This method changes the note pitch. If the note is tied, changing its pitch
    // will create inconsistencies. To avoid this flag 'fRemoveTies' is used.
    // If the note is tied and flag is true, ties will be removed. If flag is false
    // pitch change will be propagated to all tied notes.

    ChangePitch(nPitch.Step(), nPitch.Octave(), nPitch.Accidentals(), fRemoveTies);
}

void lmNote::ChangePitch(int nStep, int nOctave, int nAlter, bool fRemoveTies)
{
    // This method changes the note pitch. If the note is tied, changing its pitch
    // will create inconsistencies. To avoid this flag 'fRemoveTies' is used.
    // If the note is tied and flag is true, ties will be removed. If flag is false
    // pitch change will be propagated to all tied notes.

    DoChangePitch(nStep, nOctave, nAlter);

    // If the note is tied either remove ties or propagate pitch change
    if (m_pTiePrev) {
        if (fRemoveTies) {
            m_pTiePrev->Remove(this);
            delete m_pTiePrev;
            m_pTiePrev = (lmTie*)NULL;
        }
        else {
            m_pTiePrev->PropagateNotePitchChange(this, nStep, nOctave, nAlter, lmBACKWARDS);
        }
    }
    if (m_pTieNext) {
        if (fRemoveTies) {
            m_pTieNext->Remove(this);
            delete m_pTieNext;
            m_pTieNext = (lmTie*)NULL;
        }
        else {
            m_pTieNext->PropagateNotePitchChange(this, nStep, nOctave, nAlter, lmFORWARDS);
        }
    }

}

void lmNote::ChangePitch(int nSteps)
{
    // This method is used during edition. User has requested to raise/lower
	// this note pitch by nSteps (half lines)

    //update all pitch related information
	if (nSteps > 0)
	{
		while (nSteps > 0) {
			m_anPitch.IncrStep();
			nSteps--;
		}
	}
	else
	{
		while (nSteps < 0) {
			m_anPitch.DecrStep();
			nSteps++;
		}
	}
    SetUpStemDirection();
}

void lmNote::DoChangePitch(int nStep, int nOctave, int nAlter)
{
    // This method changes the note pitch. It does not take care of tied notes

	//update accidentals
	OnAccidentalsChanged(nStep, nAlter);

    //update all pitch related information
    m_anPitch.Set(nStep, nOctave, nAlter);
    SetUpStemDirection();
}

void lmNote::PropagateNotePitchChange(int nStep, int nOctave, int nAlter, bool fForward)
{
    //change note pitch
    DoChangePitch(nStep, nOctave, nAlter);

    // If the note is tied propagate pitch change
    if (m_pTiePrev && !fForward) {
        m_pTiePrev->PropagateNotePitchChange(this, nStep, nOctave, nAlter, lmBACKWARDS);
    }
    if (m_pTieNext && fForward) {
        m_pTieNext->PropagateNotePitchChange(this, nStep, nOctave, nAlter, lmFORWARDS);
    }

}

void lmNote::ChangeAccidentals(int nAccSteps)
{
	int nAcc = m_anPitch.Accidentals();
	int nStep = m_anPitch.Step();
	if (nAccSteps == 0)
		nAcc = 0;		//remove accidentals
	else
		nAcc += nAccSteps;		//increment/decrement accidentals
	m_anPitch.SetAccidentals(nAcc);
	OnAccidentalsChanged(nStep, nAcc);
}

void lmNote::OnAccidentalsChanged(int nStep, int nNewAcc)
{
	//accidentals have been changed to nNewAcc. This method recompute
	//accidentals to display

	lmContext* pContext = NewUpdatedContext();
	int nCurAcc = pContext->GetAccidentals(nStep);
    if (nCurAcc == nNewAcc)
	{
        //Accidentals already included. Remove any possible displayed accidental
        if (m_pAccidentals) {
            delete m_pAccidentals;
            m_pAccidentals = (lmAccidental*)NULL;
        }
    }
    else
	{
        // need to add/change displayed accidentals
        if (m_pAccidentals) delete m_pAccidentals;
        lmEAccidentals nAlter = ComputeAccidentalsToDisplay(nCurAcc, nNewAcc);
        m_pAccidentals = new lmAccidental(this, nAlter);

        // propagate accidentals to other notes in this measure
        m_pVStaff->OnContextUpdated(this, GetStaffNum(), nStep, nAlter, pContext);
    }
	delete pContext;
}


wxString lmNote::Dump()
{
    //get pitch relative to key signature
    //TODO: Uncomment following code. Commented in to avoid a chrash while debugging colstaffobjs
    //lmFPitch fp = FPitch(m_anPitch);
    //lmKeySignature* pKey = GetCurrentContext()->GeyKey();
    //lmEKeySignatures nKey = (pKey ? pKey->GetKeyType() : earmDo);
    wxString sPitch = _T("");   //FPitch_ToRelLDPName(fp, nKey);

    wxString sDump;
    sDump = wxString::Format(
        _T("%d\tNote\tType=%d, Pitch=%s, Midi=%d, Volume=%d, Voice=%d, TimePos=%.2f, ")
        _T("org=(%.2f, %.2f), rDuration=%.2f, StemType=%d"),
        m_nId, m_nNoteType, sPitch.c_str(), m_anPitch.GetMPitch(), m_nVolume, m_nVoice,
		m_rTimePos, m_uOrg.x, m_uOrg.y, m_rDuration, m_nStemType);

    if (m_pTieNext) sDump += _T(", TiedNext");
    if (m_pTiePrev) sDump += _T(", TiedPrev");
    if (IsBaseOfChord())
        sDump += _T(", BaseOfChord");
    if (IsInChord()) {
        sDump += wxString::Format(_T(", InChord, Notehead shift = %s"),
            (m_fNoteheadReversed ? _T("yes") : _T("no")) );
    }

    ////stem info
    //if (m_nStemType != lmSTEM_NONE) {
    //    sDump += wxString::Format(_T(", xStem=%d, yStem=%d, length=%d"),
    //                m_uxStem, m_uyStem,m_uStemLength );
    //}

	//base class info
	sDump += lmNoteRest::Dump();
    sDump += _T("\n");

    return sDump;
}

wxString lmNote::SourceLDP(int nIndent)
{
    wxString sSource = _T("");
    if (IsInChord() && IsBaseOfChord()) {
        sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
        sSource += _T("(chord\n");
    }

    if (IsInChord()) nIndent++;
    sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    sSource += _T("(n ");

    //pitch
    if (IsPitchDefined()) {
        //get pitch relative to context
		lmDPitch dp = m_anPitch.ToDPitch();
		if (m_pAccidentals)
			sSource += m_pAccidentals->GetLDPEncoding();
		sSource += DPitch_ToLDPName(dp);
    }
    else
        sSource += _T("*");
    sSource += _T(" ");

    //duration
    sSource += GetLDPNoteType();
    if (m_fDotted) sSource += _T(".");
    if (m_fDoubleDotted) sSource += _T(".");

    //tied ?
    if (IsTiedToNext()) sSource += _T(" l");

	//base class
	sSource += lmNoteRest::SourceLDP(nIndent);

	//close chord element
    if (IsInChord() && m_pChord->IsLastNoteOfChord(this))
	{
		nIndent--;
		sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
        sSource += _T(")\n");
    }

    return sSource;
}

wxString lmNote::SourceXML(int nIndent)
{
	wxString sSource = _T("");
	sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
    sSource += _T("<note>\n");
	nIndent++;

    //pitch
    if (IsPitchDefined())
	{
		sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
		sSource += _T("<pitch>\n");
		nIndent++;
		sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
		sSource += wxString::Format(_T("<step>%d</step>\n"), m_anPitch.Step() );
		sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
		sSource += wxString::Format(_T("<octave>%d</octave>\n"), m_anPitch.Octave() );
		nIndent--;
		sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
		sSource += _T("</pitch>\n");
    }

	//TODO
    //duration
	//  <duration>2</duration>
    //sSource += GetLDPNoteType();
    //if (m_fDotted) sSource += _T(".");
    //if (m_fDoubleDotted) sSource += _T(".");

	//note type
	sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
	sSource += _T("<type>");
    switch(m_nNoteType)
	{
        case eLonga:		sSource += _T("longa"); break;
        case eBreve:		sSource += _T("breve"); break;
        case eWhole:		sSource += _T("whole"); break;
        case eHalf:			sSource += _T("half"); break;
        case eQuarter:		sSource += _T("quarter"); break;
        case eEighth:		sSource += _T("eighth"); break;
        case e16th:			sSource += _T("16th"); break;
        case e32th:			sSource += _T("32th"); break;
        case e64th:			sSource += _T("64th"); break;
        case e128th:		sSource += _T("128th"); break;
        case e256th:		sSource += _T("256th"); break;
        default:
            wxASSERT(false);
            return _T("");        //compiler happy
    }
	sSource += _T("</type>\n");

	//voice
	sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
	sSource += wxString::Format(_T("<voice>%d</voice>\n"), m_nVoice);

	//stem
    if (m_nStemType != lmSTEM_NONE)
	{
		sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
		sSource += _T("<stem>");
		sSource += (m_fStemDown ? _T("down") : _T("up"));
		sSource += _T("</stem>\n");
    }

	//TODO
//    sFuente = sFuente & "                <notations>" & sCrLf
//    sFuente = sFuente & "                    <slur type=""start"" number=""1""/>" & sCrLf
//    sFuente = sFuente & "                </notations>" & sCrLf

	//base class
	sSource += lmNoteRest::SourceXML(nIndent);

	//close note
	nIndent--;
	sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
    sSource += _T("</note>\n");

    return sSource;
}

void lmNote::Freeze(lmUndoData* pUndoData)
{
    //save info about relations and invalidate ptrs.

    //if note in chord, remove it from the chord
    pUndoData->AddParam<bool>(IsInChord());
    if (IsInChord())
    {
		//save position in chord of note to be removed
		pUndoData->AddParam<int>(m_pChord->GetNoteIndex(this));

		//m_pChord->SaveChord(pUndoData);
        m_pChord->Remove(this);

		//save num notes, and first note in chord, for recovering chord pointer
        pUndoData->AddParam<int>( m_pChord->NumNotes() );
		pUndoData->AddParam<lmNote*>( m_pChord->GetBaseNote() );

        //if only one note remaining, delete chord
        if (m_pChord->NumNotes() == 1)
			delete m_pChord;	//this notifies all notes

        m_pChord = (lmChord*)NULL;
    }

    //save and remove tie with previous note
    if (m_pTiePrev)
    {
        pUndoData->AddParam<lmNote*>( m_pTiePrev->GetStartNote() );
        m_pTiePrev->Remove(this);
        delete m_pTiePrev;
        m_pTiePrev = (lmTie*)NULL;
    }
    else
        pUndoData->AddParam<lmNote*>( (lmNote*)NULL );

    //save and remove tie with next note
    if (m_pTieNext)
    {
        pUndoData->AddParam<lmNote*>( m_pTieNext->GetEndNote() );
        m_pTieNext->Remove(this);
        delete m_pTieNext;
        m_pTieNext = (lmTie*)NULL;
    }
    else
        pUndoData->AddParam<lmNote*>( (lmNote*)NULL );

    //save accidentals
    pUndoData->AddParam<lmEAccidentals>( 
        (m_pAccidentals ? m_pAccidentals->GetType() : lm_eNoAccidentals ) );
    m_pAccidentals = (lmAccidental*)NULL;

    //invalidate other pointers
    m_pNoteheadShape = (lmShapeGlyph*)NULL;
    m_pStemShape = (lmShapeStem*)NULL;

    lmNoteRest::Freeze(pUndoData);
}

void lmNote::UnFreeze(lmUndoData* pUndoData)
{
    //restore pointers

    //restore chord
    bool fInChord = pUndoData->GetParam<bool>();
    if (fInChord)
    {
		int nIndex = pUndoData->GetParam<int>();
		int nNumNotes = pUndoData->GetParam<int>();
		lmNote* pFirstNote = pUndoData->GetParam<lmNote*>();
		if (nNumNotes == 1)
		{
			//build chord
			m_pChord = new lmChord(pFirstNote);
		}
		else
		{
			//use existing chord
			m_pChord = pFirstNote->GetChord();
		}
		m_pChord->Include(this, nIndex);
    }

    //restore tie with previous note
    CreateTie(pUndoData->GetParam<lmNote*>(), this);

    //restore tie with next note
    CreateTie(this, pUndoData->GetParam<lmNote*>() );

    //restore accidentals
    m_pAccidentals = CreateAccidentals( pUndoData->GetParam<lmEAccidentals>() );

    lmNoteRest::UnFreeze(pUndoData);
}

//devuelve true si esta nota puede estar ligada con otra cuyos valores se reciben como argumentos
bool lmNote::CanBeTied(lmAPitch anPitch)
{
    return (anPitch == m_anPitch);
}


//======================================================================================
// lmStaffObj virtual functions implementation
//======================================================================================

//stems: methods related to layout phase
lmLUnits lmNote::GetXStemLeft()
{
	if (!m_pStemShape) return 0.0;

	return m_pStemShape->GetXLeft();
}

lmLUnits lmNote::GetXStemRight()
{
	if (!m_pStemShape) return 0.0;

	return m_pStemShape->GetXRight();
}

lmLUnits lmNote::GetXStemCenter()
{
	if (!m_pStemShape) return 0.0;
	return m_pStemShape->GetXCenterStem();
}

lmLUnits lmNote::GetYStartStem()
{
	//Start of stem is the nearest position to the notehead
	if (!m_pStemShape) return 0.0;

	return m_pStemShape->GetYStartStem();
	//return m_uyStem + m_uPaperPos.y;
}

lmLUnits lmNote::GetYEndStem()
{
	//End of stem is the farthest position from the notehead
	if (!m_pStemShape) return 0.0;

	return m_pStemShape->GetYEndStem();
	//return GetYStartStem() + (m_fStemDown ? m_uStemLength : -m_uStemLength);
}

void lmNote::DeleteStemShape()
{
	if (m_pStemShape)
	{
		delete m_pStemShape;
		m_pStemShape = (lmShapeStem*)NULL;
	}
}

//==========================================================================================
// Global functions related to notes
//    See AuxString.cpp for details about note pitch encoding
//==========================================================================================

wxString MIDINoteToLDPPattern(lmMPitch nPitchMIDI, lmEKeySignatures nTonalidad, lmDPitch* pPitch)
{
    /*
    Returns the LDP pattern (accidentals, note name and octave) representing the MIDI pitch
    received as parameter. It takes into account the tonal key so, for example, for MIDI 70
    (flat B4) in "F major" it returns B (pattern="B4", ntDiat=35 B), but in tonal key "A major"
    it returns La# (pattern="+A4", ntDiat=34 A).

    If pointer pPitch is not NULL stores in the variable pointed by this pointer the
    diatonic pitch.

    */

    int nOctave = (nPitchMIDI - 12) / 12;
    int nResto = nPitchMIDI % 12;
    //
    wxString sPattern, sDisplcm;
    switch(nTonalidad)
    {
        case earmDo:
        case earmLam:
            //            "C  C# D  D# E  F  F# G  G# A  A# B  "
            sPattern = _T("c  c+ d  d+ e  f  f+ g  g+ a  a+ b  ");
            sDisplcm = _T("0  0  1  1  2  3  3  4  4  5  5  6  ");
            break;

    //    //Sostenidos ---------------------------------------
        case earmSol:
        case earmMim:
            //            "C  C# D  D# E  F  F# G  G# A  A# B  " #F
            sPattern = _T("c  c+ d  d+ e  e+ f  g  g+ a  a+ b  ");
            sDisplcm = _T("0  0  1  1  2  2  3  4  4  5  5  6  ");
            break;

        case earmRe:
        case earmSim:
            //            "C  C# D  D# E  F  F# G  G# A  A# B  " #FC
            sPattern = _T("b+ c  d  d+ e  e+ f  g  g+ a  a+ b  ");
            sDisplcm = _T("-1 0  1  1  2  2  3  4  4  5  5  6  ");
            break;

        case earmLa:
        case earmFasm:
            //            "C  C# D  D# E  F  F# G  G# A  A# B  " #FCG
            sPattern = _T("b+ c  d  d+ e  e+ f  fx g  a  a+ b  ");
            sDisplcm = _T("-1 0  1  1  2  2  3  3  4  5  5  6  ");
            break;

        case earmMi:
        case earmDosm:
            //            "C  C# D  D# E  F  F# G  G# A  A# B  " #FCGD
            sPattern = _T("b+ c  cx d  e  e+ f  fx g  a  a+ b  ");
            sDisplcm = _T("-1 0  0  1  2  2  3  3  4  5  5  6  ");
            break;

        case earmSi:
        case earmSolsm:
            //            "C  C# D  D# E  F  F# G  G# A  A# B  " #FCGDA
            sPattern = _T("b+ c  cx d  e  e+ f  fx g  gx a  b  ");
            sDisplcm = _T("-1 0  0  1  2  2  3  3  4  4  5  6  ");
            break;

        case earmFas:
        case earmResm:
            //            "C  C# D  D# E  F  F# G  G# A  A# B  " #FCGDAE
            sPattern = _T("b+ c  cx d  dx e  f  fx g  gx a  b  ");
            sDisplcm = _T("-1 0  0  1  1  2  3  3  4  4  5  6  ");
            break;

        case earmDos:
        case earmLasm:
            //            "C  C# D  D# E  F  F# G  G# A  A# B  " #FCGDAEB
            sPattern = _T("b  c  cx d  dx e  f  fx g  gx a  ax ");
            sDisplcm = _T("-1 0  0  1  1  2  3  3  4  4  5  5  ");
            break;

    //    //Bemoles -------------------------------------------
        case earmFa:
        case earmRem:
            //            "C  C# D  D# E  F  F# G  G# A  A# B  "
            sPattern = _T("c  c+ d  d+ e  f  f+ g  g+ a  b  b= ");
            sDisplcm = _T("0  0  1  1  2  3  3  4  4  5  6  6  ");
            break;

        case earmSib:
        case earmSolm:
            //            "C  C# D  D# E  F  F# G  G# A  A# B  "
            sPattern = _T("c  c+ d  e  e= f  f+ g  g+ a  b  b= ");
            sDisplcm = _T("0  0  1  2  2  3  3  4  4  5  6  6  ");
            break;

        case earmMib:
        case earmDom:
            //            "C  C# D  D# E  F  F# G  G# A  A# B  "
            sPattern = _T("c  c+ d  e  e= f  f+ g  a  a= b  b= ");
            sDisplcm = _T("0  0  1  2  2  3  3  4  5  5  6  6  ");
            break;

        case earmLab:
        case earmFam:
            //            "C  C# D  D# E  F  F# G  G# A  A# B  "
            sPattern = _T("c  d  d= e  e= f  f+ g  a  a= b  b= ");
            sDisplcm = _T("0  1  1  2  2  3  3  4  5  5  6  6  ");
            break;

        case earmReb:
        case earmSibm:
             //         "C  C# D  D# E  F  F# G  G# A  A# B  "
            sPattern = _T("c  d  d= e  e= f  g  g= a  a= b  b= ");
            sDisplcm = _T("0  1  1  2  2  3  4  4  5  5  6  6  ");
            break;

        case earmSolb:
        case earmMibm:
            //            "C  C# D  D# E  F  F# G  G# A  A# B  "
            sPattern = _T("c= d  d= e  e= f  g  g= a  a= b  c  ");
            sDisplcm = _T("0  1  1  2  2  3  4  4  5  5  6  7  ");
            break;

        case earmDob:
        case earmLabm:
            //            "C  C# D  D# E  F  F# G  G# A  A# B  "
            sPattern = _T("c= d  d= e  f  f= g  g= a  a= b  c  ");
            sDisplcm = _T("0  1  1  2  2  3  4  4  5  5  6  7  ");
            break;

        default:
            wxASSERT(false);    // Tonal key error
    }

    int i = 3 * nResto + 1;
    long nShift;
    wxString sShift = sDisplcm.substr(i-1, 1);
    if (sShift == _T("-"))
        sShift = sDisplcm.substr(i-1, 2);
    bool fOK = sShift.ToLong(&nShift);
    wxASSERT(fOK);

    if (pPitch)    // if requested, return the diatonic pitch
        *pPitch = (lmDPitch)(nOctave * 7 + 1) + (lmDPitch)nShift;

    if (nShift == -1)
        nOctave--;
    else if (nShift == 7)
        nOctave++;
    wxString sOctave = wxString::Format(_T("%d"), nOctave);
    wxString sAnswer = sPattern.substr(i, 2);        // alterations
    sAnswer.Trim();
    sAnswer += sPattern.substr(i-1, 1);            // note name
    sAnswer += sOctave;                            // octave
    return sAnswer;

}

wxString GetNoteNamePhysicists(lmDPitch nPitch)
{
    // Returns the note name and octave in the physics namespace

    static wxString sNoteName[7] = {
        _("c%d"), _("d%d"), _("e%d"), _("f%d"), _("g%d"), _("a%d"), _("b%d")
    };

    int nOctave = (int)(nPitch - 1) / 7;
    int iNote = (nPitch - 1) % 7;

    return wxString::Format(sNoteName[iNote], nOctave);

}

