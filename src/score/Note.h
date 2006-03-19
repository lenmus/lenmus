// RCS-ID: $Id: Note.h,v 1.5 2006/02/23 19:23:54 cecilios Exp $
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
/*! @file Note.h
    @brief Header file for class lmNote
    @ingroup score_kernel
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __NOTE_H__        //to avoid nested includes
#define __NOTE_H__


class lmNote: public lmNoteRest
{
public:
    lmNote(lmVStaff* pVStaff, bool fAbsolutePitch,
        wxString sStep, wxString sOctave, wxString sAlter,
        EAccidentals nAccidentals,
        ENoteType nNoteType, float rDuration,
        bool fDotted, bool fDoubleDotted,
        wxInt32 nStaff,
        lmContext* pContext, 
        bool fBeamed, lmTBeamInfo BeamInfo[],
        bool fInChord,
        bool fTie,
        EStemType nStem);

    ~lmNote();

    //implementation of virtual methods of base classes
        // lmScoreObj
    void SetLeft(lmMicrons nLeft);
        // lmStaffObj
    wxBitmap*   GetBitmap(double rScale);
    void        DrawObject(bool fMeasuring, lmPaper* pPaper, wxColour colorC);
    void        MoveDragImage(lmPaper* pPaper, wxDragImage* pDragImage, wxPoint& ptOffset, 
                            const wxPoint& ptLog, const wxPoint& dragStartPosL, const wxPoint& ptPixels);
    wxPoint     EndDrag(const wxPoint& pos);

    wxString    Dump();
    wxString    SourceLDP();
    wxString    SourceXML();

        //lmCompositeObj
    lmScoreObj* FindSelectableObject(wxPoint& pt);


    // methods related to note positioning information
    lmMicrons GetPitchShift() { return (m_pVStaff->TenthsToLogical(GetPosOnStaff() * 10, m_nStaffNum )) / 2 ; }
    lmMicrons GetAnchorPos() { return m_xAnchor; }
    int GetPosOnStaff();        //line/space on which note is rendered

    // bounds of image. Abolute position (->referred to page origin)
    lmMicrons GetBoundsTop();
    lmMicrons GetBoundsBottom();
    lmMicrons GetBoundsLeft();
    lmMicrons GetBoundsRight();

    //methos related to stems
    EStemType   GetStemType() { return m_nStemType; }
    lmMicrons   GetDefaultStemLength() { return m_pVStaff->TenthsToLogical(35, m_nStaffNum); }
    void        SetStemLength(lmMicrons length) { m_nStemLength = length; };
    void        SetStemDirection(bool fStemDown) { m_fStemDown = fStemDown; }
    lmMicrons   GetXStem() {return m_xStem + m_paperPos.x; }
    lmMicrons   GetYStem() {return m_yStem + m_paperPos.y; }
    lmMicrons   GetStemLength() { return m_nStemLength; }
    lmMicrons   GetFinalYStem() {
                    return GetYStem() + (m_fStemDown ? m_nStemLength : -m_nStemLength); }
    bool        StemGoesDown() { return m_fStemDown; }
    void        SetStemInfo(lmMicrons xStem, lmMicrons yStem, lmMicrons length) {
                    m_xStem = xStem;
                    m_yStem = yStem;
                    m_nStemLength = length;
                }

    //additional methods related to chords
    bool        IsInChord() { return (m_pChord != (lmChord*)NULL); }        
    bool        IsBaseOfChord() { return m_pChord && m_fIsNoteBase; }
    lmChord*    GetChord() { return m_pChord; }
    lmChord*    StartChord();
    void        ClearChordInformation();

    //methods related to ties
    bool    CanBeTied(lmPitch nMidiPitch, int nStep);
    bool    NeedToBeTied() { return m_fNeedToBeTied; }
    void    SetTie(lmTie* pTie) {
                    m_pTieNext = pTie;
                    m_fNeedToBeTied = false; 
                }
    void    RemoveTie(lmTie* pTie); 
    bool    IsTiedToNext() { return (m_pTieNext != (lmTie*)NULL); }
    bool    IsTiedToPrev() { return (m_pTiePrev != (lmTie*)NULL); } 

    // methods related to sound
    lmPitch GetPitch() { return m_nPitch; }
    lmPitch GetMidiPitch() { return m_nMidiPitch; }
    int     GetStep() { return m_nStep; }        //0-C, 1-D, 2-E, 3-F, 4-G, 5-A, 6-B

    //other methods
    bool    UpdateContext(int nStep, int nNewAccidentals, lmContext* pNewContext);



private:
    // rendering
    void MakeUpPhase(lmPaper* pPaper);
    void DrawSingleNote(wxDC* pDC, bool fMeasuring, ENoteType nTipoNota,
                        bool fStemAbajo, lmMicrons nxLeft, lmMicrons nyTop, wxColour colorC);
    void DrawNoteHead(wxDC* pDC, bool fMeasuring, ECabezaNotas nNoteheadType,
                        lmMicrons nxLeft, lmMicrons nyTop, wxColour colorC);
    void DrawAdditionalLines(wxDC* pDC, int nPosOnStaff, lmMicrons yTopLine, lmMicrons xPos,
                        lmMicrons width, int nROP = wxCOPY);

    //auxiliary
    wxInt32 PosOnStaffToPitch(wxInt32 nSteps);
    void SetUpPitchRelatedVariables(lmPitch nNewPitch);
    void SetUpStemDirection();

        //============================================================
        // member variables 
        //============================================================

    /*
    Pitch information is stored in:
    a) variables related to sound:
        m_nMidiPitch        real pitch. accidentals and context already included
        m_nAlter            accidentals added to diatonic pitch implied by the combination of
                            m_nStep, m_nOctave and context accidentals. If displayed music 
                            coincides with played music then the combination of m_nPitch and
                            m_nAlter should match the real pitch m_nMidiPitch.

    b) variables related to how the note should look:
        m_nStep                note name: 0-C, 1-D, 2-E, 3-F, 4-G, 5-A, 6-B
        m_nOctave            octave: 0 .. 9.  4 = the octave started by middle C.
        m_nPitch            diatonic pitch: the combination of m_nStep and m_nOctave
        m_pAccidentals        to render in this note. Other than the implied by key signature

    */

    // variables related to how the note should sound when playing the score
    //-----------------------------------------------------------------------
    lmPitch        m_nMidiPitch;    //real pitch: combination of all three previous vars.
    int            m_nAlter;        //chromatic alteration in number of semitones
                                //(e.g., -1 for flat, 1 for sharp).
                                //! @todo accept decimal values like 0.5 (quarter tone sharp)


    // variables related to how the note should look when rendering the score
    //-----------------------------------------------------------------------

    // pitch and accidentals
    int             m_nStep;            //0-C, 1-D, 2-E, 3-F, 4-G, 5-A, 6-B
    int             m_nOctave;          //0 .. 9.  4 = the octave started by middle C.
    lmPitch         m_nPitch;           //absolute pitch, without accidentals (diatonic pitch)
    lmAccidental*   m_pAccidentals;     //accidentals
    EClefType       m_nClef;            //clef to draw this note
    lmContext*      m_pContext;         //context for this note

    // additional positioning related variables
    lmMicrons       m_xAnchor;          // x position of anchor line (relative to m_paperPos.x)
    wxRect          m_noteheadRect;     // notehead bounding rectangle (relative to paper)

    // stem information
    lmMicrons       m_xStem;           //pos and length of stem (relative to m_paperPos)
    lmMicrons       m_yStem;           //yStem refers to the notehead nearest position
    lmMicrons       m_nStemLength;     //length of stem;
    bool            m_fStemDown;       //stem direccion. true if down
    EStemType       m_nStemType;       //type of stem

    //accidentals positioning
    wxPoint        m_alterPos;

    // common for sound and look 
    //-----------------------------------------------------------------------

    //info for chords
    lmChord*    m_pChord;           //chord to which this note belongs or NULL if it is a single note
    bool        m_fIsNoteBase;      //in chords identifies the first note of the chord. For notes not in
                                    //  in chord is always true
    //tie related variables
    lmTie*      m_pTiePrev;         //Tie to previous note. Null in note not tied
    lmTie*      m_pTieNext;         //Tie to next note. Null in note not tied
    bool        m_fNeedToBeTied;    //for building tie to previous note as the score is being built




};

// declare a list of Notes class
#include "wx/list.h"
WX_DECLARE_LIST(lmNote, NotesList);


// Global functions related to notes

lmPitch StepAndOctaveToPitch(int nStep, int nOctave);
lmPitch PitchToMidi(lmPitch nPitch, int nAlter);
wxString MIDINoteToLDPPattern(lmPitch nPitchMIDI, EKeySignatures nTonalidad, 
                              lmPitch* pPitch = (lmPitch*)NULL);
wxString GetNoteNamePhysicists(lmPitch nPitch);


#endif    // __NOTE_H__