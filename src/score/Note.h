//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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

#ifndef __LM_NOTE_H__        //to avoid nested includes
#define __LM_NOTE_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Note.cpp"
#endif

#include "defs.h"

class lmVStaff;
class lmContext;
class lmPaper;
class lmBox;
class lmComponentObj;
class lmChordLayout;
class lmShape;
class lmCompositeShape;
class lmShapeNote;

#define lmREMOVE_TIES   true
#define lmCHANGE_TIED   false

class lmNote: public lmNoteRest
{
public:

    lmNote(lmVStaff* pVStaff, long nID, lmEPitchType nPitchType,
        int nStep, int nOctave, int nAlter, lmEAccidentals nAccidentals,
        lmENoteType nNoteType, float rDuration,
        int nNumDots, int nStaff, int nVoice, bool fVisible,
        lmContext* pContext,
        bool fBeamed, lmTBeamInfo BeamInfo[],
        lmNote* pBaseOfChord,
        bool fTie,
        lmEStemType nStem);

    ~lmNote();

    //implementation of virtual methods of base classes
    lmLUnits LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC);
	void PlaybackHighlight(wxDC* pDC, wxColour colorC);
	inline wxString GetName() const { return _T("note"); }

    wxString    Dump();
    wxString    SourceLDP(int nIndent, bool fUndoData);
    wxString    SourceXML(int nIndent);

	void CustomizeContextualMenu(wxMenu* pMenu, lmGMObject* pGMO);


    // methods related to note positioning information
    lmLUnits GetPitchShift();
    lmLUnits GetAnchorPos();
    int GetPosOnStaff();        //line/space on which note is rendered
    lmLUnits GetShiftToNotehead();

    //methods related to stems
    inline lmEStemType GetStemType() { return m_nStemType; }
    lmLUnits GetDefaultStemLength();
    lmLUnits GetStandardStemLenght();
    inline void SetStemLength(lmLUnits uLength) { m_uStemLength = uLength; };
    void SetStemDirection(bool fStemDown);
    inline lmLUnits GetStemThickness() {return m_uStemThickness; }
    inline lmLUnits GetStemLength() { return m_uStemLength; }
    inline bool StemGoesDown() { return m_fStemDown; }
    void ToggleStem();

	//stems: methods related to layout phase
    lmLUnits GetXStemLeft();
	lmLUnits GetXStemCenter();
    lmLUnits GetXStemRight();
    lmLUnits GetYStartStem();
    lmLUnits GetYEndStem();
	void DeleteStemShape();


    // methods related to chords
    inline bool IsInChord() { return (m_pChord != (lmChordLayout*)NULL); }
    bool IsLastOfChord();
    bool IsBaseOfChord();
    inline lmChordLayout* GetChord() { return m_pChord; }
    inline void SetNoteheadReversed(bool fValue) { m_fNoteheadReversed = fValue; }
    inline bool IsNoteheadReversed() { return m_fNoteheadReversed; }
	inline void OnIncludedInChord(lmChordLayout* pChord) { m_pChord = pChord; }
	inline void OnRemovedFromChord() { m_pChord = (lmChordLayout*)NULL; }

    //methods related to accidentals
    inline bool HasAccidentals() { return (m_pAccidentals != (lmAccidental*)NULL); }
    inline lmAccidental* GetAccidentals() { return m_pAccidentals; }
    void ComputeAccidentalsToKeepPitch(int nNewAcc);

    //methods related to ties
    bool CanBeTied(lmAPitch anPitch);
    inline bool NeedToBeTied() { return m_fNeedToBeTied; }
    inline void SetTiePrev(lmTie* pTie) { m_pTiePrev = pTie; }
    inline void SetTieNext(lmTie* pTie) {
                    m_pTieNext = pTie;
                    m_fNeedToBeTied = false;
                }
    inline lmTie* GetTiePrev() { return m_pTiePrev; }
    inline lmTie* GetTieNext() { return m_pTieNext; }
    void CreateTie(lmNote* pNtPrev, lmNote* pNtNext, long nID = lmNEW_ID);
    void CreateTie(lmNote* pNtNext, long nID, lmTPoint* pStartBezier,
                   lmTPoint* pEndBezier);
    void RemoveTie(lmTie* pTie);
    inline bool IsTiedToNext() { return (m_pTieNext != (lmTie*)NULL); }
    inline bool IsTiedToPrev() { return (m_pTiePrev != (lmTie*)NULL); }
    inline lmNote* GetTiedNotePrev() { return (m_pTiePrev ? (lmNote*)m_pTiePrev->GetStartNoteRest() : (lmNote*)NULL); }
    inline lmNote* GetTiedNoteNext() { return (m_pTieNext ? (lmNote*)m_pTieNext->GetEndNoteRest() : (lmNote*)NULL); }
    void DeleteTiePrev();

    //methods for relationships
    void OnRemovedFromRelationship(lmRelObj* pRel);

    // methods related to sound
    lmDPitch GetDPitch();
    lmMPitch GetMPitch();
    inline lmAPitch GetAPitch() { return m_anPitch; }
    inline lmFPitch GetFPitch() { return FPitch(m_anPitch); }
    bool IsPitchDefined();
    void ChangePitch(int nStep, int nOctave, int nAlter, bool fRemoveTies);
    void ChangePitch(lmAPitch nAPitch, bool fRemoveTies);
    void PropagateNotePitchChange(int nStep, int nOctave, int nAlter, bool fForward);
    void ModifyPitch(lmEClefType nNewClefType, lmEClefType nOldClefType);
    void ModifyPitch(int nAlterIncr);
    inline int GetStep() { return m_anPitch.Step(); }      //0-C, 1-D, 2-E, 3-F, 4-G, 5-A, 6-B
    inline int GetOctave() { return m_anPitch.Octave(); }
    inline int GetVolume() { return m_nVolume; }
    inline void SetVolume(int nVolume) { m_nVolume = nVolume; }
    void ComputeVolume();

    // methods used during layout phase
    bool AddNoteShape(lmShapeNote* pNoteShape, lmPaper* pPaper, lmLUnits uxLeft,
                      lmLUnits uyTop, wxColour colorC);
    inline lmShape* GetNoteheadShape() { return m_pNoteheadShape; }
    void ShiftNoteHeadShape(lmLUnits uxShift);
	lmEGlyphIndex GetGlyphForFlag();
	void CreateContainerShape(lmBox* pBox, lmLUnits uxLeft, lmLUnits uyTop, wxColour colorC);

    //other methods
    bool OnAccidentalsUpdated(int nNewAccidentals);

    // methods oriented to score processing
    int GetPositionInBeat();

    // methods for harmonic analisis
    int GetBeatPosition();

	//context related information
	lmTimeSignature* GetTimeSignature();
	lmEClefType GetCtxApplicableClefType();
	int GetContextAccidentals(int nStep);

	//methods for edition
	void ChangePitch(int nSteps);
	void ChangeAccidentals(int nSteps);

    //Other info
    wxString GetPrintName();


private:
    //creation related
    lmAccidental* CreateAccidentals(lmEAccidentals nAcc);

    // rendering
    lmEGlyphIndex DrawFlag(bool fMeasuring, lmPaper* pPaper, lmUPoint uPos, wxColour colorC);

    //layouting
    void AddSingleNoteShape(lmShapeNote* pNoteShape, lmPaper* pPaper, lmENoteType nNoteType,
                            bool fStemAbajo, lmLUnits uxLeft, lmLUnits uyTop,
							wxColour colorC);
    void AddNoteHeadShape(lmShapeNote* pNoteShape, lmPaper* pPaper, lmENoteHeads nNoteheadType,
                          lmLUnits uxLeft, lmLUnits uyTop, wxColour colorC);
    //void AddLegerLineShape(lmShapeNote* pNoteShape, lmPaper* pPaper, int nPosOnStaff,
    //                       lmLUnits uyStaffTopLine, lmLUnits uxPos, lmLUnits uWidth, int nStaff);
    lmEGlyphIndex AddFlagShape(lmShapeNote* pNoteShape, lmPaper* pPaper, lmUPoint uPos, wxColour colorC);

    //auxiliary
    void SetUpPitchRelatedVariables(lmDPitch nNewPitch);
    void SetUpStemDirection();
    lmEAccidentals ComputeAccidentalsToDisplay(int nCurContextAcc, int nNewAcc) const;
	void OnAccidentalsChanged(int nStep, int nNewAcc);


    //pitch
    void DoChangePitch(int nStep, int nOctave, int nAlter);


        //============================================================
        // member variables
        //============================================================

    // Absolute pitch information (that is, the real sound) is stored in m_anPitch, so
    // the real accidentals are in m_anPitch.Accidentals(). As the displayed accidentals
    // are usually different (some accidentals such as key signature accidentals and alterations
    // in previous notes are not displayed) the displayed accidentals are stored in
    // graphic m_pAccidentals

    lmAPitch        m_anPitch;          //real absolute pitch. Accidentals and context already included
    lmAccidental*   m_pAccidentals;     //accidentals to be drawn

    // additional positioning related variables
    lmLUnits        m_uSpacePrev;       // space (after accidental) before note

    // stem information
    lmLUnits        m_uStemThickness;
    lmLUnits        m_uStemLength;     //length of stem;
    bool            m_fStemDown;       //stem direccion. true if down
    lmEStemType		m_nStemType;       //type of stem

    // playback info
    int             m_nVolume;          // MIDI volume (0-127)

    //info for chords
    lmChordLayout*    m_pChord;           //chord to which this note belongs or NULL if it is a single note
    bool        m_fNoteheadReversed;      //this notehead is reversed to avoid collisions

    //tie related variables
    lmTie*      m_pTiePrev;         //Tie to previous note. Null if note not tied
    lmTie*      m_pTieNext;         //Tie to next note. Null if note not tied
    bool        m_fNeedToBeTied;    //for building tie to previous note as the score is being built


    //temporary information. Only valid during layout phase

    // constituent shapes
    lmShapeGlyph*   m_pNoteheadShape;
    lmShapeStem*    m_pStemShape;

};


// Global functions related to notes

extern wxString MIDINoteToLDPPattern(lmMPitch nPitchMIDI, lmEKeySignatures nTonalidad,
                                     lmDPitch* pPitch = (lmDPitch*)NULL);
extern wxString GetNoteNamePhysicists(lmDPitch nPitch);
extern int lmPitchToPosOnStaff(lmEClefType nClef, lmAPitch aPitch);
extern lmDPitch PosOnStaffToPitch(lmEClefType nClef, int nPos);
extern lmLUnits lmCheckNoteNewPosition(lmStaff* pStaff, lmLUnits uyOldPos, lmLUnits uyNewPos,
                                       int* pnSteps);


#endif    // __LM_NOTE_H__
