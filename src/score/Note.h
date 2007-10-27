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

#ifndef __NOTE_H__        //to avoid nested includes
#define __NOTE_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Note.cpp"
#endif

#include "../graphic/Shapes.h"

#define lmREMOVE_TIES   true
#define lmCHANGE_TIED   false

class lmNote: public lmNoteRest
{
public:

    lmNote(lmVStaff* pVStaff, lmEPitchType nPitchType,
        wxString& sStep, wxString& sOctave, wxString& sAlter,
        EAccidentals nAccidentals,
        ENoteType nNoteType, float rDuration,
        bool fDotted, bool fDoubleDotted,
        int nStaff, bool fVisible,
        lmContext* pContext, 
        bool fBeamed, lmTBeamInfo BeamInfo[],
        bool fInChord,
        bool fTie,
        EStemType nStem);

    ~lmNote();

    //implementation of virtual methods of base classes
        // lmScoreObj
    void SetLeft(lmLUnits uLeft);
        // lmStaffObj
    wxBitmap*   GetBitmap(double rScale);
    void        DrawObject(bool fMeasuring, lmPaper* pPaper, wxColour colorC, bool fHighlight);
    void        MoveDragImage(lmPaper* pPaper, wxDragImage* pDragImage, lmDPoint& ptOffset, 
                            const lmUPoint& ptLog, const lmUPoint& uDragStartPos, const lmDPoint& ptPixels);
    lmUPoint    EndDrag(const lmUPoint& uPos);

    wxString    Dump();
    wxString    SourceLDP();
    wxString    SourceXML();

        //lmStaffObj
    lmScoreObj* FindSelectableObject(lmUPoint& uPt);


    // methods related to note positioning information
    lmLUnits GetPitchShift();
    lmLUnits GetAnchorPos() { return m_uxAnchor; }
    void SetAnchorPos(lmLUnits uxPos) { m_uxAnchor = uxPos; }
    int GetPosOnStaff();        //line/space on which note is rendered

    // bounds of image. Abolute position (->referred to page origin)
    lmLUnits GetBoundsTop();
    lmLUnits GetBoundsBottom();
    lmLUnits GetBoundsLeft();
    lmLUnits GetBoundsRight();

    //methods related to stems
    EStemType   GetStemType() { return m_nStemType; }
    lmLUnits    GetDefaultStemLength();
    lmLUnits    GetStandardStemLenght();
    void        SetStemLength(lmLUnits uLength) { m_uStemLength = uLength; };
    void        SetStemDirection(bool fStemDown);
    lmLUnits    GetXStemLeft() {return m_uxStem + m_uPaperPos.x; }
    lmLUnits    GetXStemRight() {return m_uxStem + m_uStemThickness + m_uPaperPos.x; }
    lmLUnits    GetStemThickness() {return m_uStemThickness; }
    lmLUnits    GetYStem() {return m_uyStem + m_uPaperPos.y; }
    lmLUnits    GetStemLength() { return m_uStemLength; }
    lmLUnits    GetFinalYStem() {
                    return GetYStem() + (m_fStemDown ? m_uStemLength : -m_uStemLength); }
    bool        StemGoesDown() { return m_fStemDown; }
    void        SetStemInfo(lmLUnits uxStem, lmLUnits uyStem, lmLUnits uLength) {
                    m_uxStem = uxStem;
                    m_uyStem = uyStem;
                    m_uStemLength = uLength;
                }

    // methods related to chords
    bool        IsInChord() { return (m_pChord != (lmChord*)NULL); }        
    bool        IsBaseOfChord() { return m_pChord && m_fIsNoteBase; }
    lmChord*    GetChord() { return m_pChord; }
    lmChord*    StartChord();
    void        ClearChordInformation();
    void        SetNoteheadReversed(bool fValue) { m_fNoteheadReversed = fValue; }
    bool        IsNoteheadReversed() { return m_fNoteheadReversed; }

    //methods related to accidentals
    bool HasAccidentals() { return (m_pAccidentals != (lmAccidental*)NULL); }
    lmAccidental* GetAccidentals() { return m_pAccidentals; }
    lmLUnits DrawAccidentals(lmPaper* pPaper, bool fMeasuring,
                        lmLUnits uxLeft, lmLUnits uyTop, wxColour colorC);


    //methods related to ties
    bool    CanBeTied(lmAPitch anPitch);
    bool    NeedToBeTied() { return m_fNeedToBeTied; }
    void    SetTie(lmTie* pTie) {
                    m_pTieNext = pTie;
                    m_fNeedToBeTied = false; 
                }
    void    RemoveTie(lmTie* pTie); 
    bool    IsTiedToNext() { return (m_pTieNext != (lmTie*)NULL); }
    bool    IsTiedToPrev() { return (m_pTiePrev != (lmTie*)NULL); } 

    // methods related to sound
    lmDPitch    GetDPitch();
    lmMPitch    GetMPitch();
    inline lmAPitch GetAPitch() { return m_anPitch; }
    bool    IsPitchDefined();
    void    ChangePitch(int nStep, int nOctave, int nAlter, bool fRemoveTies); 
    void    ChangePitch(lmAPitch nAPitch, bool fRemoveTies);
    void    PropagateNotePitchChange(int nStep, int nOctave, int nAlter, bool fForward);
    int     GetStep() { return m_anPitch.Step(); }      //0-C, 1-D, 2-E, 3-F, 4-G, 5-A, 6-B
    int     GetOctave() { return m_anPitch.Octave(); }
    int     GetVolume() { return m_nVolume; }
    void    SetVolume(int nVolume) { m_nVolume = nVolume; }
    void    ComputeVolume();

    // methods used during layout computation
    bool DrawNote(lmPaper* pPaper, bool fMeasuring,
                  lmLUnits uxOffset, lmLUnits uyOffset, wxColour colorC);
    lmShape* GetNoteheadShape() { return m_pNoteheadShape; }
    void ShiftNoteShape(lmLUnits uxShift);

    //other methods
    bool        UpdateContext(int nStep, int nNewAccidentals, lmContext* pNewContext);
    lmContext*  GetContext() { return m_pContext; }

    // methods oriented to score processing
    int GetPositionInBeat() const;

    // methods for harmonic analisis
    int GetChordPosition() const;





private:
    // rendering
    void MakeUpPhase(lmPaper* pPaper);
    void DrawSingleNote(lmPaper* pPaper, bool fMeasuring, ENoteType nTipoNota,
                        bool fStemAbajo, lmLUnits uxLeft, lmLUnits uyTop, wxColour colorC);
    void DrawNoteHead(lmPaper* pPaper, bool fMeasuring, ENoteHeads nNoteheadType,
                        lmLUnits uxLeft, lmLUnits uyTop, wxColour colorC);
    void DrawLegerLines(lmPaper* pPaper, int nPosOnStaff, lmLUnits uyTopLine, lmLUnits uxPos,
                        lmLUnits uWidth, int nStaff, int nROP = wxCOPY);
    lmEGlyphIndex DrawFlag(bool fMeasuring, lmPaper* pPaper, lmUPoint uPos, wxColour colorC);

    //auxiliary
    int PosOnStaffToPitch(int nSteps);
    void SetUpPitchRelatedVariables(lmDPitch nNewPitch);
    void SetUpStemDirection();
    const EAccidentals ComputeAccidentalsToDisplay(int nCurContextAcc, int nNewAcc) const;

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
    EClefType       m_nClef;            //clef to draw this note
    lmContext*      m_pContext;         //context for this note

    // constituent shapes
    lmShapeGlyph*   m_pNoteheadShape;
    lmShapeLine*    m_pStemLine;
    lmShapeGlyph*   m_pFlagShape;

    // additional positioning related variables
    lmLUnits        m_uSpacePrev;       // space (after accidental) before note
    lmLUnits        m_uxAnchor;          // x position of anchor line (relative to m_uPaperPos.x)
    lmURect         m_uNoteheadRect;    // notehead bounding rectangle (relative to paper)

    // flag information
    lmLUnits        m_uyFlag;            //y pos for flag

    // stem information
    lmLUnits        m_uxStem;           //pos and length of stem (relative to m_uPaperPos)
    lmLUnits        m_uyStem;           //yStem refers to the notehead nearest position
    lmLUnits        m_uStemThickness;
    lmLUnits        m_uStemLength;     //length of stem;
    bool            m_fStemDown;       //stem direccion. true if down
    EStemType       m_nStemType;       //type of stem

    // dealing with beams
    bool            m_fMakeUpDone;      //make up phase done, to avoid doing it
                                        //   several times
    // playback info
    int             m_nVolume;          // MIDI volume (0-127)

    // common for sound and look 
    //-----------------------------------------------------------------------

    //info for chords
    lmChord*    m_pChord;           //chord to which this note belongs or NULL if it is a single note
    bool        m_fIsNoteBase;      //in chords identifies the first note of the chord. For notes not in
                                    //  in chord is always true
    bool        m_fNoteheadReversed;      //this notehead is reversed to avoid collisions

    //tie related variables
    lmTie*      m_pTiePrev;         //Tie to previous note. Null in note not tied
    lmTie*      m_pTieNext;         //Tie to next note. Null in note not tied
    bool        m_fNeedToBeTied;    //for building tie to previous note as the score is being built




};

// declare a list of Notes class
#include "wx/list.h"
WX_DECLARE_LIST(lmNote, NotesList);


// Global functions related to notes

wxString MIDINoteToLDPPattern(lmMPitch nPitchMIDI, EKeySignatures nTonalidad, 
                              lmDPitch* pPitch = (lmDPitch*)NULL);
wxString GetNoteNamePhysicists(lmDPitch nPitch);


#endif    // __NOTE_H__
