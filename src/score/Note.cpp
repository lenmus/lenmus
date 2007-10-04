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
#include "Score.h"
#include "wx/debug.h"
#include "../ldp_parser/AuxString.h"

#include "Glyph.h"

//implementation of the Notes List
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(NotesList);

const bool m_fDrawSmallNotesInBlock = false;    //! @todo option depending on used font

lmNote::lmNote(lmVStaff* pVStaff, lmEPitchType nPitchType,
        wxString& sStep, wxString& sOctave, wxString& sAlter,
        EAccidentals nAccidentals,
        ENoteType nNoteType, float rDuration,
        bool fDotted, bool fDoubleDotted,
        int nStaff, bool fVisible,
        lmContext* pContext,
        bool fBeamed, lmTBeamInfo BeamInfo[],
        bool fInChord,
        bool fTie,
        EStemType nStem)  :
    lmNoteRest(pVStaff, DEFINE_NOTE, nNoteType, rDuration, fDotted, fDoubleDotted, nStaff,
               fVisible)
{
    /*
    Diatonic Pitch is determined by Step
    This constructor is used for two kind of pitch information:
      -    MusicXML style: sound and look information totally independent; pitch is absolute, and
        is implied by combining Step, Octave and Alter. Accidentals are just for rendering.
      - LDP style: notes defined as in hand writing; pitch depends on context (key signature
        and accidentals introduced by a previous note in the same measure) and its own
        accidentals. Therefore pitch is computred from Step, Octave, Accidentals and Context.
        Alter is not used.
    To support both kinds of definitions, parameter fAbsolutePitch indicates that pitch
    information is absolute (MusicXML style) or relative (LDP style).
    */

    //shapes initialization
    m_pNoteheadShape = (lmShapeGlyph*)NULL;
    m_pStemLine = (lmShapeLine*)NULL;
    m_pFlagShape = (lmShapeGlyph*)NULL;

    // stem information
    m_uxStem = 0;            // will be updated in lmNote::DrawObject, in measurement phase
    m_uyStem = 0;            // will be updated in lmNote::DrawObject, in measurement phase
    m_nStemType = nStem;
    m_uStemLength = GetDefaultStemLength();     //default. If beamed note, this value will
                                                //be updated in lmBeam::ComputeStemsDirection
    ////DBG
    //if (fTie) {   //GetID() == 13) {
    //    // break here
    //    int nDbg = 0;
    //}

    //context information
    m_pContext = pContext;
    m_pContext->StartUsing();
    m_nClef = (m_pContext->GetClef())->GetClefType();
//    m_nVoz = nVoz;        //! @todo parameter nVoz
    //for (int i=0; i < 7; i++) {
    //    m_anContext[i] = 0;    //! @todo parameter anContext[i]
    //}

    //get octave, step and context accidentals for this note step
    long nAux;
    sOctave.ToLong(&nAux);
    int nOctave = (int)nAux;
    int nStep = LetterToStep(sStep);
    int nCurContextAcc = m_pContext->GetAccidentals(nStep);
    int nNewContextAcc = nCurContextAcc;
    EAccidentals nDisplayAcc = nAccidentals;

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
            case eNoAccidentals:
                //do not modify context
                break;
            case eNatural:
                //ignore context. Force 'natural' (=no accidentals)
                nNewContextAcc = 0;
                break;
            case eFlat:
            case eNaturalFlat:
                //ignore context. Put one flat
                nNewContextAcc = -1;
                break;
            case eSharp:
            case eNaturalSharp:
                //ignore context. Put one sharp
                nNewContextAcc = 1;
                break;
            case eFlatFlat:
                //ignore context. Put two flats
                nNewContextAcc = -2;
                break;
            case eSharpSharp:
            case eDoubleSharp:
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
        nDisplayAcc = eNoAccidentals;
    }

    else {
        wxLogMessage(_T("[lmNote] unknown nPitchType %d"), nPitchType);
        wxASSERT(false);
    }

    // if needed, update context for following notes
    if (nNewContextAcc != nCurContextAcc) {
        m_pVStaff->UpdateContext(this, nStaff, nStep, nNewContextAcc, m_pContext);
    }

    // create the accidentals
    if (nDisplayAcc == eNoAccidentals) {
        m_pAccidentals = (lmAccidental*)NULL;
    } else {
        m_pAccidentals = new lmAccidental(this, nDisplayAcc);
    }

    //initialize stem
    SetUpStemDirection();       // standard lenght

//    Set m_cPentobjs = new Collection
//
//    //Analiza los flags
//    m_fCalderon = (nCalderon = eC_ConCalderon)

//    //analiza las anotaciones
//    Dim sDato As String, oGrafObj As CPOGrafObj
//    for (i = 1 To cAnotaciones.Count
//        sDato = cAnotaciones.Item(i)
//        switch (sDato) {
//            case "AMR":     //marca de pausa
//                Set oGrafObj = new CPOGrafObj
//                oGrafObj.ConstructorGrafObj eGO_Respiracion, oStaff, true
//                m_cPentobjs.Add oGrafObj
//            default:
//                Debug.Assert false
//        }
//    }

    // MIDI volume information can not be computed until the note is created (constructor
    // method finished) and the note added to the VStaff collection. Otherwise
    // timing information is not valid.
    // So I am going to set a default initial value meaning 'no initialized'
    m_nVolume = -1;

    //if the note is part of a chord find the base note and take some values from it
    m_fIsNoteBase = true;        //by default all notes are base notes
    m_pChord = (lmChord*)NULL;    //by defaul note is not in chord
    m_fNoteheadReversed = false;
    if (fInChord) {
        if (!g_pLastNoteRest || g_pLastNoteRest->IsRest()) {
            ; //! @todo
            //MsgBox "[lmNoteRest.ConstructorNota] Se pide nota en acorde pero no hay ninguna nota" & _
            //    " base definida. Se ignora acorde"
        }
        else {
            m_fIsNoteBase = false;
            lmNote* pLastNote = (lmNote*)g_pLastNoteRest;
            if (pLastNote->IsInChord()) {
                m_pChord = pLastNote->GetChord();
            } else {
                m_pChord = pLastNote->StartChord();
            }
            m_pChord->AddNote(this);
        }
    }


    //lmTie information ----------------------------------------------------
    m_fNeedToBeTied = fTie;
    m_pTiePrev = (lmTie*)NULL;        //assume no tied
    m_pTieNext = (lmTie*)NULL;        //assume no tied

    // verify if a previous note is tied to this one and if so, build the tie
    lmNote* pNtPrev = m_pVStaff->FindPossibleStartOfTie(m_anPitch);
    if (pNtPrev && pNtPrev->NeedToBeTied()) {

        //do the tie between previous note and this one
        m_pTiePrev = new lmTie(pNtPrev, this);
        pNtPrev->SetTie(m_pTiePrev);

        //if stem direction is not forced, choose it to be as that of the start of tie note
        if (nStem == eDefaultStem)
            m_fStemDown = pNtPrev->StemGoesDown();
    }

//    //Parámetros gráficos independientes del papel.
////    m_rEspacioPost = 0#     //ya se calculará
//    m_fCabezaX = fCabezaX


    // Generate beaming information -----------------------------------------------------
    CreateBeam(fBeamed, BeamInfo);
    m_fMakeUpDone = false;

    if (!IsInChord() || IsBaseOfChord()) g_pLastNoteRest = this;

    //initializations for renderization
    m_uSpacePrev = 0;

}

lmNote::~lmNote()
{
    //remove the note from the beam and if beam is empty delete the beam
    if (m_pBeam) {
        m_pBeam->Remove(this);
        if (m_pBeam->NumNotes() == 0) {
            delete m_pBeam;
            m_pBeam = (lmBeam*)NULL;
        }
    }

    //if note in chord, remove it from the chord. Delete the chord when only one note left in it.
    if (IsInChord()) {
        m_pChord->RemoveNote(this);
        if (m_pChord->GetNumNotes() == 1) {
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

    //delete the shapes
    if (m_pNoteheadShape) {
        delete m_pNoteheadShape;
        m_pNoteheadShape = (lmShapeGlyph*)NULL;
    }
    if (m_pStemLine) {
        delete m_pStemLine;
        m_pStemLine = (lmShapeLine*)NULL;
    }
    if (m_pFlagShape) {
        delete m_pFlagShape;
        m_pFlagShape = (lmShapeGlyph*)NULL;
    }

}

bool lmNote::UpdateContext(int nStep, int nNewAccidentals, lmContext* pNewContext)
{
    // A previous note has updated the contex. It is necessary to verify if this note is
    // affected and, if it is affected, to update context dependent information.
    // Returns true if no context modification was needed.

    //get context accidentals for the modified step
    int nAccidentals = m_pContext->GetAccidentals(nStep);

    //compare context accidentals for modified step with new accidentals
    if (nAccidentals == nNewAccidentals) {
        //are equal. No need to update context for this note.
        //Verify if this note alters current context
        //! @todo
        return true;
    }
    else {
        //are different. Update contex information
        m_pContext->StopUsing();    //inform old context that is not longer used by this note
        m_pContext = pNewContext;    //update context for this note
        m_pContext->StartUsing();    //and inform new context that it is being used by this note
        //update pitch
        m_anPitch.SetAccidentals(nNewAccidentals);
        return false;
    }

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

void lmNote::ClearChordInformation()
{
    // invoked only from lmChord destructor
    m_fIsNoteBase = true;        //by default all notes are base notes
    m_pChord = (lmChord*)NULL;    //by defaul note is not in chord
}

void lmNote::ComputeVolume()
{
    // If volume is not set assign a value
    if (m_nVolume != -1) return;    //volume already set. Do nothing.

    // Volume should depend on several factors: beak (strong, medium, weak) on which
    // this note, phrase, on dynamics information, etc. For now we are going to
    // consider only beat information
    lmTimeSignature* pTS = m_pContext->GetTime();
    if (pTS)
        m_nVolume = AssignVolume(m_rTimePos, pTS->GetNumBeats(), pTS->GetBeatType());
    else
        m_nVolume = 64;
}

int lmNote::GetPositionInBeat() const
{
    lmTimeSignature* pTS = m_pContext->GetTime();
    if (pTS)
        return GetNoteBeatPosition(m_rTimePos, pTS->GetNumBeats(), pTS->GetBeatType());
    else
        return lmUNKNOWN_BEAT;
}

int lmNote::GetChordPosition() const
{
    lmTimeSignature* pTS = m_pContext->GetTime();
    if (pTS)
        return ::GetChordPosition(m_rTimePos, m_rDuration, pTS->GetNumBeats(), pTS->GetBeatType());
    else
        return lmNON_CHORD_NOTE;
}

//====================================================================================================
// implementation of virtual methods defined in base abstract class lmNoteRest
//====================================================================================================

// Create the drag image.
// Under wxGTK the DC logical function (ROP) is used by DrawText() but it is ignored by
// wxMSW. Thus, it is not posible to do dragging just by redrawing the lmStaffObj using ROPs.
// For portability it is necessary to implement dragging by means of bitmaps and wxDragImage
wxBitmap* lmNote::GetBitmap(double rScale)
{

    lmEGlyphIndex nGlyph = GLYPH_EIGHTH_NOTE_UP;
    switch (m_nNoteType) {
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
        case eLonga:
            nGlyph = GLYPH_LONGA_NOTE;
            break;
        case eBreve:
            nGlyph = GLYPH_BREVE_NOTE;
            break;
        case eWhole:
            nGlyph = GLYPH_WHOLE_NOTE;
            break;
        case eHalf:
            nGlyph = GLYPH_NOTEHEAD_HALF;
            break;
        case eQuarter:
            nGlyph = GLYPH_NOTEHEAD_QUARTER;
            break;
        //case enh_Cross:                    /// @todo enh_Cross is a notehead type not a note type
        //    nGlyph = GLYPH_NOTEHEAD_CROSS;
        //    break;
        default:
            wxASSERT(false);
    }

    wxString sGlyph( aGlyphsInfo[nGlyph].GlyphChar );
    return PrepareBitMap(rScale, sGlyph);

}

void lmNote::DrawObject(bool fMeasuring, lmPaper* pPaper, wxColour colorC, bool fHighlight)
{
    /*
    This method is invoked by the base class (lmStaffObj). When reaching this point
    paper cursor variable (m_uPaperPos) has been updated. This value must be used
    as the base for any measurement / drawing operation.

    DrawObject() method is responsible for:
    1. In DO_MEASURE phase (fMeasuring == true):
        - Compute the surrounding rectangle, the glyph position and other measurements
    2. In DO_DRAW phase (fMeasuring == false):
        - Render the object

    */


    bool fDrawStem = true;            // assume stem down
    bool fInChord = IsInChord();

    //prepare DC
    pPaper->SetFont(*m_pFont);

    // move to right staff
    lmLUnits uyStaffTopLine = m_uPaperPos.y + GetStaffOffset();   // staff y position (top line)
    lmLUnits uxLeft=0, uyTop=0;    // current pos. as positioning computation takes place
    int nPosOnStaff = GetPosOnStaff();
    lmLUnits uyPitchShift = GetPitchShift();
    uyTop = uyStaffTopLine - uyPitchShift;
    uxLeft = m_uPaperPos.x;


    //If drawing phase do first MakeUp phase
    if (fMeasuring)
        m_fMakeUpDone = false;
    else if (!m_fMakeUpDone)
        MakeUpPhase(pPaper);

    //In measuring phase, if this is the first note of a chord, give lmChord the
    //responsibility for computing chord layout (notes and rests' positions). If it is
    //any other note of a chord, do nothing and mark the note as 'already measured'.
    bool fMeasured = false;
    if (fMeasuring) {
        if (IsBaseOfChord()) {
            //wxLogMessage( m_pChord->Dump() );
            m_pChord->ComputeLayout(pPaper, m_uPaperPos, colorC);
            fMeasured = true;
        }
        else if (IsInChord()) {
            fMeasured = true;
        }
    }

    //if this is the first note/rest of a beam, measure beam
    //AWARE This must be done before using stem information, as the beam could
    //change stem direction if it is not determined for some/all the notes in the beam
    //During measurement phase all computations, except final trimming of stem' lengths, is
    //done. Final trimming of stems' length is delayed to MakeUp phase because it is not
    //posible to adjust lentghts until the x position of the notes is finally established, and
    //this takes place AFTER the measurement phase, once the lines are justified.
    if (fMeasuring && m_fBeamed && m_BeamInfo[0].Type == eBeamBegin) {
        m_pBeam->ComputeStemsDirection();
    }

    //render accidental signs if exist
    //@aware:
    //  Accidentals shape can not be part of note shape. This is required to allow
    //  independent positioning when user select the accidental and moves it.
    //  On the other hand, the note and its accidentals are not taken into account by the
    //  system justification process, as they are considered part of the note
    if (m_pAccidentals) {
        if ((!fMeasuring && !fHighlight) || !fMeasured && fMeasuring) {
            DrawAccidentals(pPaper, fMeasuring, uxLeft - m_uPaperPos.x, uyTop - m_uPaperPos.y, colorC);
        }
        uxLeft += m_pAccidentals->GetWidth();
    }

    //advance space before note
    uxLeft += m_uSpacePrev;

    //render the notehead (or the full note if single glyph)
    if (!fMeasuring || !fMeasured && fMeasuring) {
        fDrawStem = DrawNote(pPaper, fMeasuring, uxLeft - m_uPaperPos.x, uyTop - m_uPaperPos.y, colorC);
    }
    uxLeft += m_uNoteheadRect.width;
    //lmLUnits uxNote = uxLeft;

    if (!fMeasuring && fHighlight) return;  //highlight done. Finish


 //   nxCalderon = uxLeft + m_uNoteheadRect.width / 2 ;

    //draw dots
    //------------------------------------------------------------
    if (m_fDotted || m_fDoubleDotted) {
        //! @todo user selectable
        lmLUnits uSpaceBeforeDot = m_pVStaff->TenthsToLogical(5, m_nStaffNum);
        uxLeft += uSpaceBeforeDot;
        lmLUnits uyPos = uyTop;
        if (nPosOnStaff % 2 == 0) {
            // notehead is over a line. Shift up the dots by half line
            uyPos -= m_pVStaff->TenthsToLogical(5, m_nStaffNum);
        }
        uxLeft += DrawDot(fMeasuring, pPaper, uxLeft, uyPos, colorC, true);
        if (m_fDoubleDotted) {
            uxLeft += uSpaceBeforeDot;
            uxLeft += DrawDot(fMeasuring, pPaper, uxLeft, uyPos, colorC, true);
        }
    }


    //render lmTie to previous note
    //--------------------------------------------------------------------------------------
    if (!fMeasuring && m_pTiePrev) {
        m_pTiePrev->Draw(DO_DRAW, pPaper, colorC);
    }

    // draw the stem
    // In measurement phase the stem is measured even if it must not be drawn. This is
    // necessary to have information for positioning tuplets' bracket
    // @aware x position was set in DrawNote()
    //-----------------------------------------------------------------------------------
    if (m_nStemType != eStemNone) {
        if (fMeasuring)
        {
            //! @todo move thickness to user options
            #define STEM_WIDTH   12     //stem line width (cents = tenths x10)
            m_uStemThickness = m_pVStaff->TenthsToLogical(STEM_WIDTH, m_nStaffNum) / 10;
            // compute and store start position of stem
            if (m_fStemDown) {
                //stem down: line down on the left of the notehead
                uyTop += m_pVStaff->TenthsToLogical(51, m_nStaffNum);
                m_uyStem = uyTop - m_uPaperPos.y;
            } else {
                //stem up: line up on the right of the notehead
                m_uxStem -= m_uStemThickness;
                uyTop += m_pVStaff->TenthsToLogical(49, m_nStaffNum);
                m_uyStem = uyTop - m_uPaperPos.y;
            }
            //adjust stem size if flag to be drawn
            if (!m_fDrawSmallNotesInBlock && !m_fBeamed && m_nNoteType > eQuarter) {
                int nGlyph = DrawFlag(fMeasuring, pPaper, lmUPoint(0, 0), colorC);
                lmLUnits uStem = GetStandardStemLenght();
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
                uStem = wxMax((uStem > uFlag ? uStem-uFlag : 0), uMinStem);
                m_uyFlag = GetYStem() + (m_fStemDown ? uStem : -uStem);
                SetStemLength(uStem + uFlag);
            }

        } else {
            // not measuring. Do draw stem unless it is a note in chord. In this case
            // the stem will be drawn later, when drawing the last note of the chord
            if (fDrawStem && ! fInChord) {
                lmLUnits uxPos = GetXStemLeft() + m_uStemThickness/2;     //SolidLine centers line width on given coordinates
                pPaper->SolidLine(uxPos, GetYStem(), uxPos, GetFinalYStem()+1,
                                    m_uStemThickness, eEdgeNormal, colorC);
            }
        }
    }

    //Draw flag
    //-----------------------------------------------------------------------------
    if (!m_fDrawSmallNotesInBlock && !fMeasuring) {
        if (!m_fBeamed && m_nNoteType > eQuarter && !IsInChord()) {
            lmLUnits uxPos = (m_fStemDown ? GetXStemLeft() : GetXStemRight());
            DrawFlag(fMeasuring, pPaper, lmUPoint(uxPos, m_uyFlag), colorC);
        }
    }

    // set total width
    #define NOTE_AFTERSPACE     0      //one line space     @todo user options
    lmLUnits uAfterSpace = m_pVStaff->TenthsToLogical(NOTE_AFTERSPACE, m_nStaffNum);
    if (fMeasuring) m_uWidth = uxLeft + uAfterSpace - m_uPaperPos.x;

    // draw leger lines if necessary
    //--------------------------------------------
    if (!fMeasuring) {
        lmLUnits uxLine = m_uNoteheadRect.x - m_pVStaff->TenthsToLogical(4, m_nStaffNum);
        //lmLUnits uxLine = m_pNoteheadShape->GetBoundsRectangle().x - m_pVStaff->TenthsToLogical(4, m_nStaffNum);
        lmLUnits widthLine = m_uNoteheadRect.width +
                             m_pVStaff->TenthsToLogical(8, m_nStaffNum);
        //lmLUnits widthLine = m_pNoteheadShape->GetBoundsRectangle().width +
        //                     m_pVStaff->TenthsToLogical(8, m_nStaffNum);
        DrawLegerLines(pPaper, nPosOnStaff, uyStaffTopLine, uxLine, widthLine, m_nStaffNum);
    }

    // render associated notations ------------------------------------------------------
    if (m_pNotations) {
        lmNoteRestObj* pNRO;
        wxAuxObjsListNode* pNode = m_pNotations->GetFirst();
        for (; pNode; pNode = pNode->GetNext() ) {
            pNRO = (lmNoteRestObj*)pNode->GetData();
            if (fMeasuring) {
                lmLUnits uxPos = 0;
                lmLUnits uyPos = 0;
                switch(pNRO->GetSymbolType()) {
                    case eST_Fermata:
                        // set position (relative to paperPos)
                        uxPos = m_uNoteheadRect.x + m_uNoteheadRect.width / 2;
                        uyPos = uyStaffTopLine - m_uPaperPos.y;
                        pNRO->SetSizePosition(pPaper, m_pVStaff, m_nStaffNum, uxPos, uyPos);
                        pNRO->UpdateMeasurements();
                        break;
                    default:
                        wxASSERT(false);
                }
            }
            else
                pNRO->Draw(DO_DRAW, pPaper, colorC);
        }
    }

    // render associated lmLyric objects -------------------------------------------------
    if (m_pLyrics) {
        lmLyric* pLyric;
        wxAuxObjsListNode* pNode = m_pLyrics->GetFirst();
        for (; pNode; pNode = pNode->GetNext() ) {
            pLyric = (lmLyric*)pNode->GetData();
            if (fMeasuring) {
                // set position (relative to paperPos)
                lmLUnits uxPos = m_uNoteheadRect.x;
                lmLUnits uyPos = uyStaffTopLine - m_uPaperPos.y;
                pLyric->SetSizePosition(pPaper, m_pVStaff, m_nStaffNum, uxPos, uyPos);
                pLyric->UpdateMeasurements();
            }
            else
                pLyric->Draw(DO_DRAW, pPaper, colorC);
        }
    }

    //if this last note of a chord draw the stem of the chord
    //--------------------------------------------------------------------------------------
    if (!fMeasuring && IsInChord() && m_pChord->IsLastNoteOfChord(this)
        && m_nNoteType >= eHalf)
    {
        m_pChord->DrawStem(fMeasuring, pPaper, colorC, m_pFont, m_pVStaff, m_nStaffNum);
    }

    //if this is the last note of a beamed group draw the beam lines
    //--------------------------------------------------------------------------------------
    if (!fMeasuring && m_fBeamed && m_BeamInfo[0].Type == eBeamEnd) {
        lmLUnits uThickness = m_pVStaff->TenthsToLogical(5, m_nStaffNum);
        //DOC: Beam spacing
        // ----------------
        //according to http://www2.coloradocollege.edu/dept/mu/Musicpress/engraving.html
        //distance between primary and secondary beams should be 1/4 space (2.5 tenths)
        //But if I use 3 tenths (2.5 up rounding) beam spacing is practicaly
        //invisible. In pictures displayed in the above mentioned www page, spacing
        //is about 1/2 space, not 1/4 space. So I will use 5 tenths.
        //So the number to put in next statement is 9:
        //  4 for beam thikness + 5 for beams spacing
        lmLUnits uBeamSpacing = m_pVStaff->TenthsToLogical(9, m_nStaffNum);
        m_pBeam->DrawBeamLines(pPaper, uThickness, uBeamSpacing, colorC);
    }

    //if this is the last note of a tuplet draw the tuplet bracket
    //--------------------------------------------------------------------------------------
    if (m_pTupletBracket && (m_pTupletBracket->GetEndNote())->GetID() == m_nId) {
        m_pTupletBracket->Draw(fMeasuring, pPaper, colorC);
    }

    ////DBG
    //if (fMeasuring) wxLogMessage( m_pNoteheadShape->Dump() );

}

lmLUnits lmNote::DrawAccidentals(lmPaper* pPaper, bool fMeasuring,
                        lmLUnits uxOffset, lmLUnits uyOffset, wxColour colorC)
{
    if (fMeasuring) {
        m_pAccidentals->Measure(pPaper, m_pVStaff->GetStaff(m_nStaffNum),
                                lmUPoint(uxOffset, uyOffset));
    }
    else {
        m_pAccidentals->Render(pPaper, m_uPaperPos, colorC);
        m_pAccidentals->Draw(DO_DRAW, pPaper, colorC);  //compatibility: to draw selrect
    }

    return m_pAccidentals->GetWidth();

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

bool lmNote::DrawNote(lmPaper* pPaper, bool fMeasuring,
                      lmLUnits uxOffset, lmLUnits uyOffset, wxColour colorC)
{
    // draws the notehead (or the full note if single glyph) and returns flag to
    // draw or not the stem

    lmLUnits uxLeft = uxOffset + m_uPaperPos.x;
    lmLUnits uyTop = uyOffset + m_uPaperPos.y;
    bool fDrawStem = true;

    //initially set anchor point at left side of notehead
    if (fMeasuring) { m_uxAnchor = uxOffset; }

    if (m_fDrawSmallNotesInBlock && !m_fBeamed && m_nNoteType > eQuarter && !IsInChord()) {
        // It is a single note with flag: draw it in one step with a glyph
        DrawSingleNote(pPaper, fMeasuring, m_nNoteType, m_fStemDown,
                       uxLeft, uyTop, (m_fSelected ? g_pColors->ScoreSelected() : colorC) );
        fDrawStem = false;

    } else {
        // either the note is part of a group of beamed notes, is in a chord, or doesn't
        // have stem: it must be drawn in parts
        // Draw the notehead
        ENoteHeads nNotehead;
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
                wxLogMessage(_T("[lmNote::DrawNote] Unknown note type."));
                wxASSERT(false);
            }
        //} else {
        //    nNotehead = enh_Cross;
        //}

        DrawNoteHead(pPaper, fMeasuring, nNotehead, uxLeft, uyTop,
                     (m_fSelected ? g_pColors->ScoreSelected() : colorC) );
    }

    //set stem x position
    //x position is set even if stem must not be drawn. This is
    //necessary to have information for positioning tuplets' bracket
    if (fMeasuring) {
        if (m_fStemDown) {
            //stem down: line down on the left of the notehead unless notehead reversed
            m_uxStem = m_uNoteheadRect.x;
            if (m_fNoteheadReversed) m_uxStem += m_uNoteheadRect.width;
        } else {
            //stem up: line up on the right of the notehead unless notehead reversed
            m_uxStem = m_uNoteheadRect.x;
            if (!m_fNoteheadReversed) m_uxStem += m_uNoteheadRect.width;
        }
    }

    //in chords, if nothehead is reversed the anchor point have to be at the other side
    if (fMeasuring && m_fNoteheadReversed) {
        m_uxAnchor += (m_fStemDown ? m_uNoteheadRect.width : -m_uNoteheadRect.width);
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

void lmNote::ShiftNoteShape(lmLUnits uxShift)
{
    //reposition the note shape and all directly related measurements: stem x pos,
    //anchor pos,
    //This method does not change the accidentals position, only the note itself.

    m_pNoteheadShape->Shift(uxShift);    // shift notehead
    m_uxAnchor += uxShift;                // shift anchor line
    m_uNoteheadRect.x += uxShift;
    m_uSelRect.x += uxShift;
    m_uxStem += uxShift;

}

void lmNote::MakeUpPhase(lmPaper* pPaper)
{
    m_fMakeUpDone = true;

    //Beams. If this is the first note/rest of a beam, do final trimming of stems' length
    if (m_fBeamed && m_BeamInfo[0].Type == eBeamBegin) {
        m_pBeam->TrimStems();
    }

    //Ties. If this note is the end of a tie, store tie end point
    if (m_pTiePrev) {
        // compute end point, relative to end note paperPos (this note)
        lmLUnits uxPos = m_uNoteheadRect.x + m_uNoteheadRect.width / 2;
        lmLUnits uyPos = m_pVStaff->TenthsToLogical(5, m_nStaffNum);
        uyPos = (m_pTiePrev->IsUnderNote() ?
                m_uNoteheadRect.y + m_uNoteheadRect.height + uyPos :
                m_uNoteheadRect.y - uyPos );
        m_pTiePrev->SetEndPoint(uxPos, uyPos, pPaper->GetLeftMarginXPos());
    }

    //Ties. If this note is start of tie, store tie start point. Tie position is fixed
    //by start note stem direction
    if (m_pTieNext) {
        // compute start point, relative to start note paperPos (this note)
        lmLUnits uxPos = m_uNoteheadRect.x + m_uNoteheadRect.width / 2;
        lmLUnits uyPos = m_pVStaff->TenthsToLogical(5, m_nStaffNum);
        uyPos = (m_fStemDown ?
                m_uNoteheadRect.y - uyPos : m_uNoteheadRect.y + m_uNoteheadRect.height + uyPos);
        m_pTieNext->SetStartPoint(uxPos, uyPos, pPaper->GetRightMarginXPos(), !m_fStemDown);
    }

}

void lmNote::DrawLegerLines(lmPaper* pPaper, int nPosOnStaff, lmLUnits uyStaffTopLine,
                            lmLUnits uxPos, lmLUnits uWidth, int nStaff, int nROP)
{
    if (nPosOnStaff > 0 && nPosOnStaff < 12) return;

    if (nROP != wxCOPY) pPaper->SetLogicalFunction(nROP);

    uxPos += m_uPaperPos.x;        // make it absolute
    lmLUnits uThick = m_pVStaff->GetStaffLineThick(nStaff);

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
                pPaper->SolidLine(uxPos, uyPos, uxPos + uWidth, uyPos, uThick,
                                   eEdgeNormal, *wxBLACK);
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
            }
        }
    }

    if (nROP != wxCOPY) pPaper->SetLogicalFunction(wxCOPY);    // restore DC logical function

}

void lmNote::DrawSingleNote(lmPaper* pPaper, bool fMeasuring, ENoteType nNoteType,
        bool fStemAbajo, lmLUnits uxLeft, lmLUnits uyTop, wxColour colorC)
{
    /*
    Draws a note by using a glyph.
    In DO_MEASURE mode also stores measurements.
    */

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

    wxString sGlyph( aGlyphsInfo[nGlyph].GlyphChar );

    if (fMeasuring) {
        // store glyph position
        m_uGlyphPos.x = uxLeft - m_uPaperPos.x;
        //m_uGlyphPos.y = uyTop - m_uPaperPos.y + m_pVStaff->TenthsToLogical( (fStemAbajo ? 30 : -10), m_nStaffNum );
        m_uGlyphPos.y = uyTop - m_uPaperPos.y + m_pVStaff->TenthsToLogical( aGlyphsInfo[nGlyph].GlyphOffset , m_nStaffNum );


        // define selection rectangle (relative to m_uPaperPos)
        lmLUnits uWidth, uHeight;
        pPaper->GetTextExtent(sGlyph, &uWidth, &uHeight);
        m_uSelRect.width = uWidth;
        m_uSelRect.height = m_pVStaff->TenthsToLogical( aGlyphsInfo[nGlyph].SelRectHeight, m_nStaffNum );
        m_uSelRect.x = m_uGlyphPos.x;
        m_uSelRect.y = m_uGlyphPos.y + m_pVStaff->TenthsToLogical( aGlyphsInfo[nGlyph].SelRectShift, m_nStaffNum );

        // store notehead position and size
        m_uNoteheadRect = m_uSelRect;        //! @todo compute notehead rectangle

    } else {
        // else (drawing phase) do the draw
        lmUPoint uPos = GetGlyphPosition();
        pPaper->SetTextForeground(colorC);
        pPaper->DrawText(sGlyph, uPos.x, uPos.y );
    }

}

void lmNote::DrawNoteHead(lmPaper* pPaper, bool fMeasuring, ENoteHeads nNoteheadType,
    lmLUnits uxLeft, lmLUnits uyTop, wxColour colorC)
{
    // draws a notehead of type nNoteheadType on position (uxLeft, uyTop) with color colorC.
    // In DO_MEASURE mode also stores measurements.

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

    wxString sGlyph( aGlyphsInfo[nGlyph].GlyphChar );

    if (fMeasuring) {
        lmUPoint uOffset(uxLeft - m_uPaperPos.x, uyTop - m_uPaperPos.y);
        if (!m_pNoteheadShape) m_pNoteheadShape = new lmShapeGlyph(this, nGlyph, m_pFont);
        m_pNoteheadShape->Measure(pPaper, m_pVStaff->GetStaff(m_nStaffNum), uOffset);

        //COMPATIBILITY

        // store positions
        m_uGlyphPos.x = uxLeft - m_uPaperPos.x;
        m_uGlyphPos.y = uyTop - m_uPaperPos.y - m_pVStaff->TenthsToLogical(aGlyphsInfo[nGlyph].GlyphOffset, m_nStaffNum);

        // store selection rectangle position and size
        m_uSelRect = m_pNoteheadShape->GetSelRectangle();

        // store notehead position and size. selRect bounds the notehead, so just copy it
        m_uNoteheadRect = m_uSelRect;

        //fix for width. It is not correctly computed by DC->GetTextExtent
        //m_uNoteheadRect.width -= m_pVStaff->TenthsToLogical(10, m_nStaffNum)/10;


    } else {
        //wxLogMessage(_T("[lmNote::DrawNoteHead]"));
        m_pNoteheadShape->Render(pPaper, m_uPaperPos, colorC);
    }

}

void lmNote::MoveDragImage(lmPaper* pPaper, wxDragImage* pDragImage, lmDPoint& offsetD,
                         const lmUPoint& pagePosL, const lmUPoint& uDragStartPos, const lmDPoint& canvasPosD)
{

    /*
    A note must stay on staff lines or spaces
    */

    lmLUnits dyHalfLine = m_pVStaff->TenthsToLogical(5, m_nStaffNum );
    lmUPoint nShiftVector = pagePosL - uDragStartPos;    // the displacement
    int nSteps = (int)(nShiftVector.y / dyHalfLine);        // trim the displacement to half line steps
    nShiftVector.y -= nSteps;
    lmUPoint newPaperPos = m_uPaperPos + nShiftVector;
    // then the shape must be drawn at:
    lmDPoint ptNewD;
    ptNewD.x = pPaper->LogicalToDeviceX(newPaperPos.x + m_uGlyphPos.x) + offsetD.x;
    ptNewD.y = pPaper->LogicalToDeviceY(newPaperPos.y + m_uGlyphPos.y) + offsetD.y;
    pDragImage->Move(ptNewD);

    /*
    // compute new pitch
    int nNewPitch = PosOnStaffToPitch(nSteps);
    SetUpPitchRelatedVariables(nNewPitch);
    */
}

lmUPoint lmNote::EndDrag(const lmUPoint& uPos)
{
    lmUPoint uOldPos(m_uPaperPos + m_uGlyphPos);

    /*
    Notes can not freely moved. They must stay on staff lines or spaces
    */
    lmLUnits udyHalfLine = m_pVStaff->TenthsToLogical(5, m_nStaffNum );
    lmLUnits uShift = - (uPos.y - GetGlyphPosition().y);
    int nSteps = (int)(uShift / udyHalfLine);        // trim the displacement to half line steps

    // compute new pitch
    int nNewPitch = PosOnStaffToPitch(nSteps);
    SetUpPitchRelatedVariables(nNewPitch);

    //wxLogMessage( wxString::Format(wxT("EndDrag: nShift=%d, nSteps=%d, nNewPitch=%d"),
    //    nShift, nSteps, nNewPitch ) );

    //ojo: estas dos líneas son el comportamiento de la clase base. Hay que dejarlas
    //de momento porque el flag m_fFixedPos impide que se actualice la posición
    // ¿Llevarlo a SetUpPitchRelatedVariables() ?
    //m_uPaperPos.x = uPos.x - m_uGlyphPos.x;
    //m_uPaperPos.y = uPos.y - m_uGlyphPos.y;

    //SetFixed(false);

    return lmUPoint(uOldPos);

}

int lmNote::PosOnStaffToPitch(int nSteps)
{
    /*
    When the note is dragged it is necessary to compute the new pitch from the
    its new position on the paper. From the paper displacement it is computed how
    many half line steps the note has been moved. This method receives the steps
    and computes the new pitch
    */

    int nPos = GetPosOnStaff() + nSteps;
    switch (m_nClef) {
        case eclvSol :
            return nPos + lmC4_DPITCH;
        case eclvFa4 :
            return nPos + lmC4_DPITCH - 12;
        case eclvFa3 :
            return nPos + lmC4_DPITCH - 10;
        case eclvDo1 :
            return nPos + lmC4_DPITCH - 2;
        case eclvDo2 :
            return nPos + lmC4_DPITCH - 4;
        case eclvDo3 :
            return nPos + lmC4_DPITCH - 6;
        case eclvDo4 :
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
    int nAcc = m_pContext->GetAccidentals(nStep);
    m_anPitch.Set(nNewPitch, nAcc);
    SetUpStemDirection();
}

void lmNote::SetUpStemDirection()
{
    switch (m_nStemType) {
        case eDefaultStem:
            m_fStemDown = (GetPosOnStaff() >= 6);
            break;
        case eStemDouble:
            /*! @todo
                I understand that "eStemDouble" means two stems: one up and one down.
                This is not yet implemented and is treated as eDefaultStem
            */
            m_fStemDown = (GetPosOnStaff() >= 6);
            break;
        case eStemUp:
            m_fStemDown = false;
            break;
        case eStemDown:
            m_fStemDown = true;
            break;
        case eStemNone:
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
    #define DEFAULT_STEM_LEGHT      35      //! in tenths. @todo move to user options

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
    switch (m_nClef) {
        case eclvSol :
            return m_anPitch.ToDPitch() - lmC4_DPITCH;
        case eclvFa4 :
            return m_anPitch.ToDPitch() - lmC4_DPITCH + 12;
        case eclvFa3 :
            return m_anPitch.ToDPitch() - lmC4_DPITCH + 10;
        case eclvFa5 :
            return m_anPitch.ToDPitch() - lmC4_DPITCH + 14;
        case eclvDo1 :
            return m_anPitch.ToDPitch() - lmC4_DPITCH + 2;
        case eclvDo2 :
            return m_anPitch.ToDPitch() - lmC4_DPITCH + 4;
        case eclvDo3 :
            return m_anPitch.ToDPitch() - lmC4_DPITCH + 6;
        case eclvDo4 :
            return m_anPitch.ToDPitch() - lmC4_DPITCH + 8;
        case eclvDo5 :
            return m_anPitch.ToDPitch() - lmC4_DPITCH + 10;
        default:
            // no key, assume eclvSol
            return m_anPitch.ToDPitch() - lmC4_DPITCH;
    }
}

const EAccidentals  lmNote::ComputeAccidentalsToDisplay(int nCurContextAcc, int nNewAcc) const
{
    //Current context accidentals for considered step is nCurContextAcc.
    //Note has nNewAcc. This method computes the accidentals to display so that
    //sound has nNewAcc

    EAccidentals nDisplayAcc;
    if (nNewAcc == nCurContextAcc)
        nDisplayAcc = eNoAccidentals;
    else if (nNewAcc == 1 && nCurContextAcc == 2)
        nDisplayAcc = eNaturalSharp;
    else if (nNewAcc == -1 && nCurContextAcc == -2)
        nDisplayAcc = eNaturalFlat;
    else if (nNewAcc == 2)
        nDisplayAcc = eDoubleSharp;
    else if (nNewAcc == -2)
        nDisplayAcc = eFlatFlat;
    else if (nNewAcc == 1)
        nDisplayAcc = eSharp;
    else if (nNewAcc == -1)
        nDisplayAcc = eFlat;
    else {
        wxLogMessage(_T("[lmNote::ComputeAccidentalsToDisplay] Non prgrammed case: nNewAcc=%d, nCurContextAcc=%d"),
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

void lmNote::DoChangePitch(int nStep, int nOctave, int nAlter)
{
    // This method changes the note pitch. It does not take care of tied notes

    //Update context and displayed accidentals
    int nCurAcc = m_pContext->GetAccidentals(nStep);
    if (nCurAcc == nAlter) {
        //Accidentals already included. Remove any possible displayed accidental
        if (m_pAccidentals) {
            delete m_pAccidentals;
            m_pAccidentals = (lmAccidental*)NULL;
        }
    }
    else {
        // need to add/change displayed accidentals
        if (m_pAccidentals) delete m_pAccidentals;
        EAccidentals nNewAcc = ComputeAccidentalsToDisplay(nCurAcc, nAlter);
        m_pAccidentals = new lmAccidental(this, nNewAcc);
        // update context
        m_pVStaff->UpdateContext(this, GetStaffNum(), nStep, nAlter, m_pContext);
    }

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

// Rectangle that bounds the image. Absolute position referred to page origin
lmLUnits lmNote::GetBoundsTop()
{
    return (m_fStemDown ? m_uSelRect.GetTop() + m_uPaperPos.y : GetFinalYStem() );
}

lmLUnits lmNote::GetBoundsBottom()
{
    return (m_fStemDown ? GetFinalYStem() : m_uSelRect.GetBottom() + m_uPaperPos.y );
}

lmLUnits lmNote::GetBoundsLeft()
{
    return (m_fStemDown ? GetXStemLeft() : m_uSelRect.GetLeft() + m_uPaperPos.x );
}

lmLUnits lmNote::GetBoundsRight()
{
    return (m_fStemDown ? m_uSelRect.GetRight() + m_uPaperPos.x : GetXStemLeft() );
}

void lmNote::SetLeft(lmLUnits uLeft)
{
    //
    // The SetLeft() method in StaffObj is overriden to take into account the possible
    // existence of associated AuxObjs, such as Ties and Accidentals. These AuxObjs have
    // also to be moved when the note is moved
    //

    m_uPaperPos.x = uLeft;

    //move Ties
    if (m_pTieNext) m_pTieNext->UpdateMeasurements();
    if (m_pTiePrev) m_pTiePrev->UpdateMeasurements();

    //move associated AuxObjs
    if (m_pNotations) {
        lmNoteRestObj* pNRO;
        wxAuxObjsListNode* pNode = m_pNotations->GetFirst();
        for (; pNode; pNode = pNode->GetNext() ) {
            pNRO = (lmNoteRestObj*)pNode->GetData();
            pNRO->UpdateMeasurements();
        }
    }

    // move associated lyrics
    if (m_pLyrics) {
        lmLyric* pLyric;
        wxAuxObjsListNode* pNode = m_pLyrics->GetFirst();
        for (; pNode; pNode = pNode->GetNext() ) {
            pLyric = (lmLyric*)pNode->GetData();
            pLyric->UpdateMeasurements();
        }
    }

    //move associated accidentals
    if (m_pAccidentals) m_pAccidentals->UpdateMeasurements();

}

wxString lmNote::Dump()
{
    wxString sDump;
    sDump = wxString::Format(
        _T("%d\tNote\tType=%d, Pitch=%s, Midi=%d, Volume=%d, PosOnStaff=%d, TimePos=%.2f, rDuration=%.2f, StemType=%d"),
        m_nId, m_nNoteType, m_anPitch.LDPName().c_str(), m_anPitch.GetMPitch(), m_nVolume, GetPosOnStaff(), m_rTimePos, m_rDuration,
        m_nStemType);
    if (m_pTieNext) sDump += _T(", TiedNext");
    if (m_pTiePrev) sDump += _T(", TiedPrev");
    if (IsBaseOfChord())
        sDump += _T(", BaseOfChord");
    if (IsInChord()) {
        sDump += wxString::Format(_T(", InChord, Notehead shift = %s"),
            (m_fNoteheadReversed ? _T("yes") : _T("no")) );
    }
    if (m_fBeamed) {
        sDump += wxString::Format(_T(", Beamed: BeamTypes(%d"), m_BeamInfo[0].Type);
        for (int i=1; i < 6; i++) {
            sDump += wxString::Format(_T(",%d"), m_BeamInfo[i].Type);
        }
        sDump += _T(")");
    }
    if (m_pTupletBracket) {
        if ((m_pTupletBracket->GetEndNote())->GetID() == m_nId) {
            sDump += _T(", End of tuplet\n");
            sDump += m_pTupletBracket->Dump();
        }
        else if ((m_pTupletBracket->GetStartNote())->GetID() == m_nId)
            sDump += _T(", Start of tuplet");
        else
            sDump += _T(", In tuplet");
    }
    //stem info
    if (m_nStemType != eStemNone) {
        sDump += wxString::Format(_T(", xStem=%d, yStem=%d, length=%d"),
                    m_uxStem, m_uyStem,m_uStemLength );
    }
    sDump += _T("\n");

    // Dump associated lyrics
    if (m_pLyrics) {
        lmLyric* pLyric;
        wxAuxObjsListNode* pNode = m_pLyrics->GetFirst();
        for (; pNode; pNode = pNode->GetNext() ) {
            pLyric = (lmLyric*)pNode->GetData();
            sDump += pLyric->Dump();
        }
    }

	//positioning information
	lmURect rect = GetSelRect();
	sDump += wxString::Format(_T("\n                    SelRect=(%.2f, %.2f, %.2f, %.2f)"),
		rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom() );
    sDump += _T("\n");

    return sDump;

}

wxString lmNote::SourceLDP()
{
    wxString sSource = _T("         (n ");
    //(fInChord ? _T("            (NA "), _T("            (N "));
    if (IsPitchDefined())
        sSource += m_anPitch.LDPName();
    else
        sSource += _T("*");

    sSource += _T(" ");
    sSource += GetLDPNoteType();
    if (m_fDotted) sSource += _T(".");
    if (m_fDoubleDotted) sSource += _T(".");

    //! @todo take tonal key into account

    //! @todo Finish lmNote LDP Source code generation method

    //tied
    if (IsTiedToNext()) sSource += _T(" l");

    //start or end of group
    if (m_fBeamed) {
        if (m_BeamInfo[0].Type == eBeamBegin) {
            sSource += _T(" g+");
        }
        else if (m_BeamInfo[0].Type == eBeamEnd) {
            sSource += _T(" g-");
        }
    }

    //tuplets
    if (m_pTupletBracket) {
        if ((lmNoteRest*)this == m_pTupletBracket->GetStartNote()) {
            sSource += wxString::Format(_T(" t%d"), m_pTupletBracket->GetTupletNumber());
        }
        else if((lmNoteRest*)this == m_pTupletBracket->GetEndNote()) {
            sSource += _T(" t-");
        }
    }

    //staff num
    if (m_pVStaff->GetNumStaves() > 1) {
        sSource += wxString::Format(_T(" p%d"), m_nStaffNum);
    }


//    if (nCalderon == eC_ConCalderon) m_sFuente = m_sFuente & " C";
//    for (int i=1; i <= cAnotaciones.Count; i++) {
//        m_sFuente = m_sFuente & " " & cAnotaciones.Item(i);
//    }
    sSource += _T(")\n");
    return sSource;
}

wxString lmNote::SourceXML()
{
    wxString sSource = _T("TODO: lmNote XML Source code generation methods");
    return sSource;
//    sPitch = GetNombreSajon(m_anPitch.ToDPitch())
//
//    sFuente = "            <note>" & sCrLf
//    sFuente = sFuente & "                <pitch>" & sCrLf
//    sFuente = sFuente & "                    <step>" & Left$(sPitch, 1) & "</step>" & sCrLf
//    sFuente = sFuente & "                    <octave>" & Mid$(sPitch, 2) & "</octave>" & sCrLf
//    sFuente = sFuente & "                </pitch>" & sCrLf
//    sFuente = sFuente & "                <duration>2</duration>" & sCrLf
//    sFuente = sFuente & "                <voice>1</voice>" & sCrLf
//    sFuente = sFuente & "                <type>quarter</type>" & sCrLf
//    sFuente = sFuente & "                <stem>up</stem>" & sCrLf
//    sFuente = sFuente & "                <notations>" & sCrLf
//    sFuente = sFuente & "                    <slur type=""start"" number=""1""/>" & sCrLf
//    sFuente = sFuente & "                </notations>" & sCrLf
//    sFuente = sFuente & "            </note>" & sCrLf
//    FuenteXML = sFuente
}

lmChord* lmNote::StartChord()
{
    //Start a chord with this note as the base note of the chord
    m_pChord = new lmChord(this);
    m_fIsNoteBase = true;
    return m_pChord;
}

//devuelve true si esta nota puede estar ligada con otra cuyos valores se reciben como argumentos
bool lmNote::CanBeTied(lmAPitch anPitch)
{
    return (anPitch == m_anPitch);
}


//======================================================================================
// lmStaffObj virtual functions implementation
//======================================================================================

lmScoreObj* lmNote::FindSelectableObject(lmUPoint& pt)
{
    if (IsAtPoint(pt)) return this;

    // try with ties
    if (m_pTieNext && m_pTieNext->IsAtPoint(pt)) return m_pTieNext;
    if (m_pTiePrev && m_pTiePrev->IsAtPoint(pt)) return m_pTiePrev;

    // try with associated AuxObjs
    if (m_pNotations) {
        lmNoteRestObj* pNRO;
        wxAuxObjsListNode* pNode = m_pNotations->GetFirst();
        for (; pNode; pNode = pNode->GetNext() ) {
            pNRO = (lmNoteRestObj*)pNode->GetData();
            if (pNRO->IsAtPoint(pt)) return pNRO;
        }
    }

    // try with associated lyrics
    if (m_pLyrics) {
        lmLyric* pLyric;
        wxAuxObjsListNode* pNode = m_pLyrics->GetFirst();
        for (; pNode; pNode = pNode->GetNext() ) {
            pLyric = (lmLyric*)pNode->GetData();
            if (pLyric->IsAtPoint(pt)) return pLyric;
        }
    }

    // Not found
    return (lmScoreObj*)NULL;    //none found

}


//==========================================================================================
// Global functions related to notes
//    See AuxString.cpp for details about note pitch encoding
//==========================================================================================

wxString MIDINoteToLDPPattern(lmMPitch nPitchMIDI, EKeySignatures nTonalidad, lmDPitch* pPitch)
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

