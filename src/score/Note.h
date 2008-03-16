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
class lmChord;
class lmShape;
class lmCompositeShape;
class lmShapeNote;
class lmUndoData;

#define lmREMOVE_TIES   true
#define lmCHANGE_TIED   false

class lmNote: public lmNoteRest
{
public:

    lmNote(lmVStaff* pVStaff, lmEPitchType nPitchType,
        wxString& sStep, wxString& sOctave, wxString& sAlter,
        lmEAccidentals nAccidentals,
        lmENoteType nNoteType, float rDuration,
        bool fDotted, bool fDoubleDotted,
        int nStaff, int nVoice, bool fVisible,
        lmContext* pContext, 
        bool fBeamed, lmTBeamInfo BeamInfo[],
        bool fInChord,
        bool fTie,
        lmEStemType nStem);

    ~lmNote();

    //implementation of virtual methods of base classes
        // lmStaffObj
    lmLUnits LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC);
	lmUPoint ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper);
	void PlaybackHighlight(lmPaper* pPaper, wxColour colorC);
	void CursorHighlight(lmPaper* pPaper, int nStaff, bool fHighlight);
	inline wxString GetName() const { return _T("note"); }

    wxString    Dump();
    wxString    SourceLDP(int nIndent);
    wxString    SourceXML(int nIndent);


    // methods related to note positioning information
    lmLUnits GetPitchShift();
    lmLUnits GetAnchorPos();
    int GetPosOnStaff();        //line/space on which note is rendered


    //methods related to stems
    inline lmEStemType GetStemType() { return m_nStemType; }
    lmLUnits GetDefaultStemLength();
    lmLUnits GetStandardStemLenght();
    inline void SetStemLength(lmLUnits uLength) { m_uStemLength = uLength; };
    void SetStemDirection(bool fStemDown);
    inline lmLUnits GetStemThickness() {return m_uStemThickness; }
    inline lmLUnits GetStemLength() { return m_uStemLength; }
    inline bool StemGoesDown() { return m_fStemDown; }

	//stems: methods related to layout phase
    lmLUnits    GetXStemLeft();
	lmLUnits	GetXStemCenter();
    lmLUnits    GetXStemRight();
    lmLUnits    GetYStartStem();
    lmLUnits    GetYEndStem();
	void		DeleteStemShape();


    // methods related to chords
    inline bool IsInChord() { return (m_pChord != (lmChord*)NULL); }        
    bool IsBaseOfChord();
    inline lmChord* GetChord() { return m_pChord; }
    inline void SetNoteheadReversed(bool fValue) { m_fNoteheadReversed = fValue; }
    inline bool IsNoteheadReversed() { return m_fNoteheadReversed; }
	inline void OnIncludedInChord(lmChord* pChord) { m_pChord = pChord; }
	inline void OnRemovedFromChord() { m_pChord = (lmChord*)NULL; }

    //methods related to accidentals
    inline bool HasAccidentals() { return (m_pAccidentals != (lmAccidental*)NULL); }
    inline lmAccidental* GetAccidentals() { return m_pAccidentals; }

    //methods related to ties
    bool CanBeTied(lmAPitch anPitch);
    inline bool NeedToBeTied() { return m_fNeedToBeTied; }
    inline void SetTiePrev(lmTie* pTie) { m_pTiePrev = pTie; }
    inline void SetTieNext(lmTie* pTie) {
                    m_pTieNext = pTie;
                    m_fNeedToBeTied = false; 
                }
    void RemoveTie(lmTie* pTie); 
    inline bool IsTiedToNext() { return (m_pTieNext != (lmTie*)NULL); }
    inline bool IsTiedToPrev() { return (m_pTiePrev != (lmTie*)NULL); } 

    // methods related to sound
    lmDPitch GetDPitch();
    lmMPitch GetMPitch();
    inline lmAPitch GetAPitch() { return m_anPitch; }
    bool    IsPitchDefined();
    void    ChangePitch(int nStep, int nOctave, int nAlter, bool fRemoveTies); 
    void    ChangePitch(lmAPitch nAPitch, bool fRemoveTies);
    void    PropagateNotePitchChange(int nStep, int nOctave, int nAlter, bool fForward);
    inline int GetStep() { return m_anPitch.Step(); }      //0-C, 1-D, 2-E, 3-F, 4-G, 5-A, 6-B
    inline int GetOctave() { return m_anPitch.Octave(); }
    inline int GetVolume() { return m_nVolume; }
    inline void SetVolume(int nVolume) { m_nVolume = nVolume; }
    void    ComputeVolume();

    // methods used during layout phase
    bool AddNoteShape(lmShapeNote* pNoteShape, lmPaper* pPaper, lmLUnits uxLeft,
                      lmLUnits uyTop, wxColour colorC);
    inline lmShape* GetNoteheadShape() { return m_pNoteheadShape; }
    void ShiftNoteHeadShape(lmLUnits uxShift);
	lmEGlyphIndex GetGlyphForFlag();
	void CreateContainerShape(lmBox* pBox, lmLUnits uxLeft, lmLUnits uyTop, wxColour colorC);

    //other methods
    bool OnContextUpdated(int nStep, int nNewAccidentals, lmContext* pNewContext);

    // methods oriented to score processing
    int GetPositionInBeat();

    // methods for harmonic analisis
    int GetChordPosition();

	//context related information
	lmTimeSignature* GetTimeSignature();
	lmEClefType GetClefType();
	int GetContextAccidentals(int nStep);

	//methods for edition
	void ChangePitch(int nSteps);
	void ChangeAccidentals(int nSteps);

    //undo/redo
    virtual void Freeze(lmUndoData* pUndoData);
    virtual void UnFreeze(lmUndoData* pUndoData);


private:
    //creation related
    lmAccidental* CreateAccidentals(lmEAccidentals nAcc);
    void CreateTie(lmNote* pNtPrev, lmNote* pNtNext);

    // rendering
    lmEGlyphIndex DrawFlag(bool fMeasuring, lmPaper* pPaper, lmUPoint uPos, wxColour colorC);

    //layouting
    void AddSingleNoteShape(lmShapeNote* pNoteShape, lmPaper* pPaper, lmENoteType nNoteType,
                            bool fStemAbajo, lmLUnits uxLeft, lmLUnits uyTop,
							wxColour colorC);
    void AddNoteHeadShape(lmShapeNote* pNoteShape, lmPaper* pPaper, lmENoteHeads nNoteheadType,
                          lmLUnits uxLeft, lmLUnits uyTop, wxColour colorC);
    void AddLegerLineShape(lmShapeNote* pNoteShape, lmPaper* pPaper, int nPosOnStaff, 
                           lmLUnits uyStaffTopLine, lmLUnits uxPos, lmLUnits uWidth, int nStaff);
    lmEGlyphIndex AddFlagShape(lmShapeNote* pNoteShape, lmPaper* pPaper, lmUPoint uPos, wxColour colorC);

    //auxiliary
    int PosOnStaffToPitch(int nSteps);
    void SetUpPitchRelatedVariables(lmDPitch nNewPitch);
    void SetUpStemDirection();
    const lmEAccidentals ComputeAccidentalsToDisplay(int nCurContextAcc, int nNewAcc) const;
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
    lmChord*    m_pChord;           //chord to which this note belongs or NULL if it is a single note
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

// declare a list of Notes class
#include "wx/list.h"
WX_DECLARE_LIST(lmNote, NotesList);


// Global functions related to notes

wxString MIDINoteToLDPPattern(lmMPitch nPitchMIDI, lmEKeySignatures nTonalidad, 
                              lmDPitch* pPitch = (lmDPitch*)NULL);
wxString GetNoteNamePhysicists(lmDPitch nPitch);


#endif    // __LM_NOTE_H__
