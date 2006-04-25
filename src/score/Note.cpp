// RCS-ID: $Id: Note.cpp,v 1.8 2006/02/23 19:23:54 cecilios Exp $
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
/*! @file Note.cpp
    @brief Implementation file for class lmNote
    @ingroup score_kernel
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

#include "wx/image.h"
#include "Score.h"
#include "wx/debug.h"
#include "../ldp_parser/AuxString.h"

#include "Glyph.h"

//implementation of the Notes List
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(NotesList);


lmNote::lmNote(lmVStaff* pVStaff, bool fAbsolutePitch,
        wxString sStep, wxString sOctave, wxString sAlter,
        EAccidentals nAccidentals,
        ENoteType nNoteType, float rDuration,
        bool fDotted, bool fDoubleDotted,
        wxInt32 nStaff,
        lmContext* pContext, 
        bool fBeamed, lmTBeamInfo BeamInfo[],
        bool fInChord,
        bool fTie,
        EStemType nStem)  : 
    lmNoteRest(pVStaff, DEFINE_NOTE, nNoteType, rDuration, fDotted, fDoubleDotted, nStaff)
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

//        nCalderon As ECalderon, _
//        cAnotaciones As Collection, _
//        ByRef anContext() As Long, _
//        fCabezaX As Boolean, _
//        nVoz As Long, _

    
    // stem information
    m_xStem = 0;            // will be updated in lmNote::DrawObject, in measurement phase
    m_yStem = 0;            // will be updated in lmNote::DrawObject, in measurement phase
    m_nStemType = nStem;    
    m_nStemLength = GetDefaultStemLength();        //default. If beamed note, this value will
                                                //be updated in lmBeam::ComputeStems


    // accidentals
    if (nAccidentals == eNoAccidentals) {
        m_pAccidentals = (lmAccidental*)NULL;
    } else {
        m_pAccidentals = new lmAccidental(this, nAccidentals);
    }

    //context information
    m_pContext = pContext;
    m_pContext->StartUsing();
    m_nClef = (m_pContext->GetClef())->GetType();
//    m_nVoz = nVoz;        //! @todo parameter nVoz
    //for (int i=0; i < 7; i++) {
    //    m_anContext[i] = 0;    //! @todo parameter anContext[i]
    //}
    
    //get context accidentals for this note step
    m_nStep = LetterToStep(sStep);
    int nNewAlter = m_pContext->GetAccidentals(m_nStep);

    //update context with displayed accidentals or with alterations
    if (fAbsolutePitch) {
        //update context with alterations
    }
    else {
        //update context with accidentals
        switch (nAccidentals) {
            case eNoAccidentals:
                //do not modify context
                break;
            case eNatural:
                nNewAlter = 0;    //! @todo review: shouldn't be the accidentals of the key signature?
                break;
            case eFlat:
                nNewAlter = -1;
                break;
            case eSharp:
                nNewAlter = 1;
                break;
            case eFlatFlat:
                nNewAlter = -2;
                break;
            case eSharpSharp: 
            case eDoubleSharp:
                nNewAlter = 2;
                break;
            case eNaturalFlat:
                nNewAlter = 1;
                break;
            case eNaturalSharp:
                nNewAlter = -1;
                break;
            default:
                ;
        }
    }

    // if needed, update context for following notes
    if (nNewAlter != m_pContext->GetAccidentals(m_nStep)) {
        m_pVStaff->UpdateContext(this, nStaff, m_nStep, nNewAlter, m_pContext);
    }

    //store all pitch related information
    long nAux;
    sOctave.ToLong(&nAux);
    m_nOctave = (int)nAux;
    sAlter.ToLong(&nAux);
    m_nAlter = (fAbsolutePitch ? (int)nAux : nNewAlter);
    m_nPitch = StepAndOctaveToPitch(m_nStep, m_nOctave);
    m_nMidiPitch = PitchToMidi(m_nPitch, m_nAlter);

    SetUpStemDirection();

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
    
    //if the note is part of a chord find the base note and take some values from it
    m_fIsNoteBase = true;        //by default all notes are base notes
    m_pChord = (lmChord*)NULL;    //by defaul note is not in chord
    if (fInChord) {
        if (!g_pLastNoteRest || g_pLastNoteRest->IsRest()) {
            ; //! @todo
            //MsgBox "[lmNoteRest.ConstructorNota] Se pide nota en acorde pero no hay ninguna nota" & _
            //    " base definida. Se ignora acorde"
        }
        else {
            lmNote* pLastNote = (lmNote*)g_pLastNoteRest;
            if (pLastNote->IsInChord()) {
                m_pChord = pLastNote->GetChord();
            } else {
                m_pChord = pLastNote->StartChord();
            }
            m_pChord->AddNote(this);
            m_fIsNoteBase = false;
        }
    }
    
    
    //lmTie information ----------------------------------------------------
    m_fNeedToBeTied = fTie;
    m_pTiePrev = (lmTie*)NULL;        //assume no tied
    m_pTieNext = (lmTie*)NULL;        //assume no tied

    // verify if a previous note is tied to this one and if so, build the tie
    lmNote* pNtPrev = m_pVStaff->FindPossibleStartOfTie(m_nMidiPitch, m_nStep);
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
    
    
    
    g_pLastNoteRest = this;

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

}

bool lmNote::UpdateContext(int nStep, int nNewAccidentals, lmContext* pNewContext)
{
    /*
    A previous note has updated the contex. It is necessary to verify if this note is
    affected and, if it is affected, to update context dependent information.
    Returns true if no context modification was needed.
    */

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
        m_nAlter = nNewAccidentals;
        m_nMidiPitch = PitchToMidi(m_nPitch, m_nAlter);
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
        case eWhole:
            nGlyph = GLYPH_NOTEHEAD_WHOLE;
            break;
        case eHalf:
            nGlyph = GLYPH_NOTEHEAD_HALF;
            break;
        case eQuarter:
            nGlyph = GLYPH_NOTEHEAD_QUARTER;
            break;
        //case ecn_Aspa:                    /// @todo ecn_Aspa is a notehead type not a note type
        //    nGlyph = GLYPH_NOTEHEAD_CROSS;
        //    break;
        default:
            wxASSERT(false);
    }

    wxString sGlyph( aGlyphsInfo[nGlyph].GlyphChar );
    return PrepareBitMap(rScale, sGlyph);

}

void lmNote::DrawObject(bool fMeasuring, lmPaper* pPaper, wxColour colorC)
{
    /*
    This method is invoked by the base class (lmStaffObj). When reaching this point 
    paper cursor variable (m_paperPos) has been updated. This value must be used
    as the base for any measurement / drawing operation.

    DrawObject() method is responsible for:
    1. In DO_MEASURE phase (fMeasuring == true):
        - Compute the surrounding rectangle, the glyph position and other measurements
    2. In DO_DRAW phase (fMeasuring == false):
        - Render the object

    */


    lmLUnits yStaffBaseLine=0;            // position of staff (top line)
    lmLUnits nxLeft=0, nyTop=0;    // current pos. as positioning computation takes place
    bool fDrawStem = true;            // assume stem down
    bool fInChord = IsInChord();

    // move to right staff
    //if (fMeasuring) {
        int nPosOnStaff = GetPosOnStaff();
        yStaffBaseLine = m_paperPos.y + GetStaffOffset();
        lmLUnits yPitchShift = GetPitchShift();
        nyTop = yStaffBaseLine - yPitchShift;
        nxLeft = m_paperPos.x;
    //} else {
        wxDC* pDC = pPaper->GetDC();
        wxASSERT(pDC);
        wxASSERT(pDC->Ok());
        int nStemWidth = m_pVStaff->TenthsToLogical(1, m_nStaffNum);
        wxPen pen(colorC, nStemWidth, wxSOLID);
        wxBrush brush(colorC, wxSOLID);
        pDC->SetPen(pen);
        pDC->SetBrush(brush);
        pDC->SetFont(*m_pFont);
    //}

    //If drawing phase do first MakeUp phase
    if (!fMeasuring) MakeUpPhase(pPaper);

    //if this is the first note/rest of a beam, measure beam
    //@attention This must be done before using stem information, as the beam could
    //change stem direction if it is not determined for some/all the notes in the beam
    //During measurement phase all computations, except final trimming of stem' lengths, is
    //done. Final trimming of stems' length is delayed to MakeUp phase because it is not
    //posible to adjust lentghts until the x position of the notes is finally established, and
    //this takes place AFTER the measurement phase, once the lines are justified.
    if (fMeasuring && m_fBeamed && m_BeamInfo[0].Type == eBeamBegin) {
        m_pBeam->ComputeStems();
    }

    //render accidental signs if exist
    if (m_pAccidentals) {
        if (fMeasuring) {
            // set position (relative to paperPos)
             lmLUnits xPos = nxLeft - m_paperPos.x;
            lmLUnits yPos = nyTop - m_paperPos.y;
            m_pAccidentals->SetSizePosition(pPaper, m_pVStaff, m_nStaffNum, xPos, yPos);
            m_pAccidentals->UpdateMeasurements();
        }
        else {
            m_pAccidentals->Draw(DO_DRAW, pPaper, colorC);
        }
        nxLeft += m_pAccidentals->GetSelRect().width +
            m_pVStaff->TenthsToLogical(20, m_nStaffNum)/3;        // after space = 1.5 tenths (0.3 mm)
    }
    
    //render the note
    if (fMeasuring) { m_xAnchor = nxLeft - m_paperPos.x; }

    if (!m_fBeamed && m_nNoteType > eQuarter && !fInChord) {
        //-------------------------------------------------------------------
        // It is a single note with corchete: draw it in one step with a glyph
        //-------------------------------------------------------------------
        DrawSingleNote(pDC, fMeasuring, m_nNoteType, m_fStemDown, 
                       nxLeft, nyTop, (m_fSelected ? g_pColors->ScoreSelected() : colorC) );
        fDrawStem = false;

    } else {
        //------------------------------------------------------------------------------
        // either the note is part of a group of beamed notes, is in a chord, or doesn't
        // have stem: it must be drawn in parts
        //------------------------------------------------------------------------------
    
        //--- draw the notehead -------------------------------------
        ECabezaNotas nNotehead;
        //if (! m_fCabezaX) {
            if (m_nNoteType == eHalf) {
                nNotehead = ecn_Blanca;
            } else if (m_nNoteType == eWhole) {
                nNotehead = ecn_Redonda;
                fDrawStem = false;
            } else {
                nNotehead = ecn_Negra;
            }
        //} else {
        //    nNotehead = ecn_Aspa;
        //}
        
        DrawNoteHead(pDC, fMeasuring, nNotehead, nxLeft, nyTop, 
                     (m_fSelected ? g_pColors->ScoreSelected() : colorC) );
    }
    nxLeft += m_noteheadRect.width;

 //   nxCalderon = nxLeft + m_noteheadRect.width / 2 ;
    
    //draw dots
    //------------------------------------------------------------
    if (m_fDotted || m_fDoubleDotted) {
        nxLeft += m_pVStaff->TenthsToLogical(5, m_nStaffNum);
        if (!fMeasuring) {
            lmLUnits halfLine = m_pVStaff->TenthsToLogical(5, m_nStaffNum);
            lmLUnits nDotRadius = m_pVStaff->TenthsToLogical(2, m_nStaffNum);
            lmLUnits yPos = nyTop + m_pVStaff->TenthsToLogical(50, m_nStaffNum);
            if (nPosOnStaff % 2 == 0) {
                // notehead is over a line. Shift up the dot
                pDC->DrawCircle(nxLeft, yPos - halfLine, nDotRadius);
            } else {
                pDC->DrawCircle(nxLeft, yPos, nDotRadius);
            }
        }
    }

    if (m_fDoubleDotted) {
        nxLeft += m_pVStaff->TenthsToLogical(5, m_nStaffNum);
        if (!fMeasuring) {
            lmLUnits halfLine = m_pVStaff->TenthsToLogical(5, m_nStaffNum);
            lmLUnits nDotRadius = m_pVStaff->TenthsToLogical(2, m_nStaffNum);
            lmLUnits yPos = nyTop + m_pVStaff->TenthsToLogical(50, m_nStaffNum);
            if (nPosOnStaff % 2 == 0) {
                // notehead is over a line. Shift up the dot
                pDC->DrawCircle(nxLeft, yPos - halfLine, nDotRadius);
            } else {
                pDC->DrawCircle(nxLeft, yPos, nDotRadius);
            }
        }
   }
    
    //render lmTie to previous note
    //--------------------------------------------------------------------------------------
    if (!fMeasuring && m_pTiePrev) {
        m_pTiePrev->Draw(DO_DRAW, pPaper, colorC);
    }
    
    // draw the stem
    // In measurement phase the stem is measured even if it must not be drawn. This is
    // necessary to have information for tuplets' bracket positioning
    //-----------------------------------------------------------------------------------
    if (m_nStemType != eStemNone) {
        if (fMeasuring) {
            nyTop += m_pVStaff->TenthsToLogical(50, m_nStaffNum);
            // compute and store start position of stem
            if (m_fStemDown) {
                //stem down: line down on the left of the notehead
                m_xStem = m_noteheadRect.x + nStemWidth/2;
                m_yStem = nyTop - m_paperPos.y;
            } else {
                //stem up: line up on the right of the notehead
                m_xStem = m_noteheadRect.x + m_noteheadRect.width - nStemWidth/2;
                m_yStem = nyTop - m_paperPos.y;
            }

        } else {
            // not measuring. Do draw stem unless it is a note in chord. In this case
            // the stem will be drawn later, when drawing the last note of the chord
            if (fDrawStem && ! fInChord) {
                wxDC* pDC = pPaper->GetDC();
                wxASSERT(pDC);
                pDC->DrawLine(GetXStem(), GetYStem(), GetXStem(), GetFinalYStem());
            }
        }
    }

    // set total width
    lmLUnits afterSpace = m_pVStaff->TenthsToLogical(10, m_nStaffNum);    //one line space
    if (fMeasuring) m_nWidth = nxLeft + afterSpace - m_paperPos.x;

    // draw lineas adicionales if necessary
    //--------------------------------------------
    if (!fMeasuring) {
        lmLUnits xLine = m_noteheadRect.x - m_pVStaff->TenthsToLogical(4, m_nStaffNum);
        // in DO_DRAW phase dxNotehead is 0. In DO_MEASRUE phase it is the width of the
        // glyph and then it is not right, particularly with notes drawn in block
        // lmLUnits widthLine = dxNotehead + m_pVStaff->TenthsToLogical(8, m_nStaffNum);
        lmLUnits widthLine = m_pVStaff->TenthsToLogical(20, m_nStaffNum);
        DrawAdditionalLines(pDC, nPosOnStaff, yStaffBaseLine, xLine, widthLine);
    }
    
    // render associated notations ------------------------------------------------------
    if (m_pNotations) {
        lmNoteRestObj* pNRO;
        wxAuxObjsListNode* pNode = m_pNotations->GetFirst();
        for (; pNode; pNode = pNode->GetNext() ) {
            pNRO = (lmNoteRestObj*)pNode->GetData();
            if (fMeasuring) {
                 lmLUnits xPos = 0;
                lmLUnits yPos = 0;
                switch(pNRO->GetSymbolType()) {
                    case eST_Fermata:
                        // set position (relative to paperPos)
                         xPos = m_noteheadRect.x + m_noteheadRect.width / 2;
                        yPos = yStaffBaseLine - m_paperPos.y;
                        pNRO->SetSizePosition(pPaper, m_pVStaff, m_nStaffNum, xPos, yPos);
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
                 lmLUnits xPos = m_noteheadRect.x;
                lmLUnits yPos = yStaffBaseLine - m_paperPos.y;
                pLyric->SetSizePosition(pPaper, m_pVStaff, m_nStaffNum, xPos, yPos);
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
        m_pChord->DrawStem(pDC);
    }

    //if this is the last note of a beamed group draw the beam lines
    //--------------------------------------------------------------------------------------
    if (!fMeasuring && m_fBeamed && m_BeamInfo[0].Type == eBeamEnd) {
        lmLUnits nThickness = m_pVStaff->TenthsToLogical(4, m_nStaffNum);
        lmLUnits nBeamSpacing = m_pVStaff->TenthsToLogical(7, m_nStaffNum);
        m_pBeam->DrawBeamLines(pDC, nThickness, nBeamSpacing);
    }

    //if this is the last note of a tuplet draw the tuplet bracket
    //--------------------------------------------------------------------------------------
    if (m_pTupletBracket && (m_pTupletBracket->GetEndNote())->GetID() == m_nId) {
        m_pTupletBracket->Draw(fMeasuring, pPaper, colorC);
    }
    
}

void lmNote::MakeUpPhase(lmPaper* pPaper)
{

    //Beams. If this is the first note/rest of a beam, do final trimming of stems' length
    if (m_fBeamed && m_BeamInfo[0].Type == eBeamBegin) {
        m_pBeam->TrimStems();
    }

    //Ties. If this note is the end of a tie, store tie end point
    if (m_pTiePrev) {
        // compute end point, relative to end note paperPos (this note) 
        lmLUnits xPos = m_noteheadRect.x + m_noteheadRect.width / 2;
        lmLUnits yPos = m_pVStaff->TenthsToLogical(5, m_nStaffNum);
        yPos = (m_pTiePrev->IsUnderNote() ? 
                m_noteheadRect.y + m_noteheadRect.height + yPos :
                m_noteheadRect.y - yPos );
        m_pTiePrev->SetEndPoint(xPos, yPos, pPaper->GetLeftMarginXPos());
    }
    
    //Ties. If this note is start of tie, store tie start point. Tie position is fixed
    //by start note stem direction
    if (m_pTieNext) {
        // compute start point, relative to start note paperPos (this note) 
        lmLUnits xPos = m_noteheadRect.x + m_noteheadRect.width / 2;
        lmLUnits yPos = m_pVStaff->TenthsToLogical(5, m_nStaffNum);
        yPos = (m_fStemDown ? 
                m_noteheadRect.y - yPos : m_noteheadRect.y + m_noteheadRect.height + yPos);
        m_pTieNext->SetStartPoint(xPos, yPos, pPaper->GetRightMarginXPos(), !m_fStemDown);
    }

}

void lmNote::DrawAdditionalLines(wxDC* pDC, int nPosOnStaff, lmLUnits yStaffBaseLine, lmLUnits xPos, lmLUnits width, int nROP)
{
    if (nPosOnStaff > 0 && nPosOnStaff < 12) return;

    if (nROP != wxCOPY) pDC->SetLogicalFunction(nROP);

    xPos += m_paperPos.x;        // make it absolute

    wxInt32 i;
    lmLUnits yPos, nTenths;
    if (nPosOnStaff > 11) {
        // lines at top
        for (i=12; i <= nPosOnStaff; i++) {
            if (i % 2 == 0) {
                nTenths = 5 * (i - 10);
                yPos = yStaffBaseLine - m_pVStaff->TenthsToLogical(nTenths, m_nStaffNum);
                pDC->DrawLine(xPos, yPos, xPos + width, yPos);
            }
        }

    } else {
        // nPosOnStaff < 1: lines at bottom
        for (i=nPosOnStaff; i <= 0; i++) {
            if (i % 2 == 0) {
                nTenths = 5 * (10 - i);
                yPos = yStaffBaseLine + m_pVStaff->TenthsToLogical(nTenths, m_nStaffNum);
                pDC->DrawLine(xPos, yPos, xPos + width, yPos);
            }
        }
    }

    if (nROP != wxCOPY) pDC->SetLogicalFunction(wxCOPY);    // restore DC logical function

}

void lmNote::DrawSingleNote(wxDC* pDC, bool fMeasuring, ENoteType nNoteType,
        bool fStemAbajo, lmLUnits nxLeft, lmLUnits nyTop, wxColour colorC)
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
        //! @todo add case statements for remaining note types
        default:
            wxASSERT(false);
    }

    wxString sGlyph( aGlyphsInfo[nGlyph].GlyphChar );
  
    if (fMeasuring) {
        // store glyph position
        m_glyphPos.x = nxLeft - m_paperPos.x;
        //m_glyphPos.y = nyTop - m_paperPos.y + m_pVStaff->TenthsToLogical( (fStemAbajo ? 30 : -10), m_nStaffNum );
        m_glyphPos.y = nyTop - m_paperPos.y + m_pVStaff->TenthsToLogical( aGlyphsInfo[nGlyph].GlyphOffset , m_nStaffNum );


        // define selection rectangle (relative to m_paperPos)
        lmLUnits nWidth, nHeight;
        pDC->GetTextExtent(sGlyph, &nWidth, &nHeight);
        m_selRect.width = nWidth;
        m_selRect.height = m_pVStaff->TenthsToLogical( aGlyphsInfo[nGlyph].SelRectHeight, m_nStaffNum );
        m_selRect.x = m_glyphPos.x;
        m_selRect.y = m_glyphPos.y + m_pVStaff->TenthsToLogical( aGlyphsInfo[nGlyph].SelRectShift, m_nStaffNum );

        // store notehead position and size
        m_noteheadRect = m_selRect;        //! @todo compute notehead rectangle

    } else {
        // else (drawing phase) do the draw
        wxPoint pos = GetGlyphPosition();
        pDC->SetTextForeground(colorC);
        pDC->DrawText(sGlyph, pos.x, pos.y );
    }

}

void lmNote::DrawNoteHead(wxDC* pDC, bool fMeasuring, ECabezaNotas nNoteheadType,
    lmLUnits nxLeft, lmLUnits nyTop, wxColour colorC)
{
    // draws a notehead of type nNoteheadType on position (nxLeft, nyTop) with color colorC.
    // In DO_MEASURE mode also stores measurements.
    
    lmEGlyphIndex nGlyph = GLYPH_NOTEHEAD_QUARTER;
    switch (nNoteheadType) {
        case ecn_Redonda:
            nGlyph = GLYPH_NOTEHEAD_WHOLE;
            break;
        case ecn_Blanca:
            nGlyph = GLYPH_NOTEHEAD_HALF;
            break;
        case ecn_Negra:
            nGlyph = GLYPH_NOTEHEAD_QUARTER;
            break;
        case ecn_Aspa:
            nGlyph = GLYPH_NOTEHEAD_CROSS;
            break;
        default:
            wxASSERT(false);
    }

    wxString sGlyph( aGlyphsInfo[nGlyph].GlyphChar );

    if (fMeasuring) {
        // store positions
        m_glyphPos.x = nxLeft - m_paperPos.x;
        m_glyphPos.y = nyTop - m_paperPos.y - m_pVStaff->TenthsToLogical(aGlyphsInfo[nGlyph].GlyphOffset, m_nStaffNum);

        // store selection rectangle position and size
        lmLUnits nWidth, nHeight;
        pDC->GetTextExtent(sGlyph, &nWidth, &nHeight);
        m_selRect.height = m_pVStaff->TenthsToLogical(aGlyphsInfo[nGlyph].SelRectHeight, m_nStaffNum);
        m_selRect.width = nWidth;
        m_selRect.x = m_glyphPos.x;
        m_selRect.y = m_glyphPos.y + m_pVStaff->TenthsToLogical(aGlyphsInfo[nGlyph].SelRectShift, m_nStaffNum);

        // store notehead position and size. selRect bounds the notehead, so just copy it
        m_noteheadRect = m_selRect;

    } else {
        // else (drawing phase) do the draw
        wxPoint pos = GetGlyphPosition();
        pDC->SetTextForeground(colorC);
        pDC->DrawText(sGlyph, pos.x, pos.y );
        //lmLUnits nWidth, nHeight;
        //pDC->GetTextExtent(sGlyph, &nWidth, &nHeight);
        //pDC->DrawRectangle(pos.x, pos.y, nWidth, nHeight);
        //wxLogMessage(_T("[lmNote::DrawNoteHead] pos=(%d, %d)"), pos.x, pos.y);
    }

}

void lmNote::MoveDragImage(lmPaper* pPaper, wxDragImage* pDragImage, wxPoint& offsetD, 
                         const wxPoint& pagePosL, const wxPoint& dragStartPosL, const wxPoint& canvasPosD)
{

    /*
    A note must stay on staff lines or spaces
    */

    lmLUnits dyHalfLine = m_pVStaff->TenthsToLogical(5, m_nStaffNum );
    wxPoint nShiftVector = pagePosL - dragStartPosL;    // the displacement
    wxInt32 nSteps = (nShiftVector.y % dyHalfLine);        // trim the displacement to half line steps
    nShiftVector.y -= nSteps;
    wxPoint newPaperPos = m_paperPos + nShiftVector;
    // then the shape must be drawn at:
    wxPoint ptNewD;
    ptNewD.x = pPaper->LogicalToDeviceX(newPaperPos.x + m_glyphPos.x) + offsetD.x;
    ptNewD.y = pPaper->LogicalToDeviceY(newPaperPos.y + m_glyphPos.y) + offsetD.y;
    pDragImage->Move(ptNewD);

    /*
    // compute new pitch
    wxInt32 nNewPitch = PosOnStaffToPitch(nSteps);
    SetUpPitchRelatedVariables(nNewPitch);
    */
}

wxPoint lmNote::EndDrag(const wxPoint& pos)
{
    wxPoint oldPos(m_paperPos + m_glyphPos);

    /*
    Notes can not freely moved. They must stay on staff lines or spaces
    */
    lmLUnits dyHalfLine = m_pVStaff->TenthsToLogical(5, m_nStaffNum );
    lmLUnits nShift = - (pos.y - GetGlyphPosition().y);
    wxInt32 nSteps = nShift / dyHalfLine;        // trim the displacement to half line steps

    // compute new pitch
    wxInt32 nNewPitch = PosOnStaffToPitch(nSteps);
    SetUpPitchRelatedVariables(nNewPitch);

    wxLogMessage( wxString::Format(wxT("EndDrag: nShift=%d, nSteps=%d, nNewPitch=%d"), 
        nShift, nSteps, nNewPitch ) );

    //ojo: estas dos líneas son el comportamiento de la clase base. Hay que dejarlas
    //de momento porque el flag m_fFixedPos impide que se actualice la posición
    // ¿Llevarlo a SetUpPitchRelatedVariables() ?
    //m_paperPos.x = pos.x - m_glyphPos.x;
    //m_paperPos.y = pos.y - m_glyphPos.y;

    //SetFixed(false);

    return wxPoint(oldPos);

}

wxInt32 lmNote::PosOnStaffToPitch(wxInt32 nSteps)
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
            return nPos + lmC4PITCH;
        case eclvFa4 :
            return nPos + lmC4PITCH - 12;
        case eclvFa3 :
            return nPos + lmC4PITCH - 10;
        case eclvDo1 :
            return nPos + lmC4PITCH - 2;
        case eclvDo2 :
            return nPos + lmC4PITCH - 4;
        case eclvDo3 :
            return nPos + lmC4PITCH - 6;
        case eclvDo4 :
            return nPos + lmC4PITCH - 8;
        default:
            wxASSERT(false);
            return 0;    //to get the compiler happy
    } 

}

void lmNote::SetUpPitchRelatedVariables(lmPitch nNewPitch)
{
    m_nPitch = nNewPitch;
    //! @todo compute new m_nAlter 
    int nAlter = m_nAlter;
    m_nMidiPitch = PitchToMidi(nNewPitch, nAlter);
    SetUpStemDirection();
}

void lmNote::SetUpStemDirection()
{
    switch (m_nStemType) {
        case eDefaultStem:
            m_fStemDown = (GetPosOnStaff() >= 5);
            break;
        case eStemDouble:
            /*! @todo
                I understand that "eStemDouble" means two stems: one up and one down.
                This is not yet implemented and is treated as eDefaultStem
            */
            m_fStemDown = (GetPosOnStaff() >= 5);
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
    //wxLogMessage( wxString::Format(wxT("SetUpPitchRelatedVariables: tipoStem=%d, plica=%s, nPosOnStaff %d"), 
    //    m_nStemType, (m_fStemDown ? _T("Abajo") : _T("Arriba")), GetPosOnStaff() ) );

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

    switch (m_nClef) {
        case eclvSol :
            return m_nPitch - lmC4PITCH;
        case eclvFa4 :
            return m_nPitch - lmC4PITCH + 12;
        case eclvFa3 :
            return m_nPitch - lmC4PITCH + 10;
        case eclvDo1 :
            return m_nPitch - lmC4PITCH + 2;
        case eclvDo2 :
            return m_nPitch - lmC4PITCH + 4;
        case eclvDo3 :
            return m_nPitch - lmC4PITCH + 6;
        case eclvDo4 :
            return m_nPitch - lmC4PITCH + 8;
        default:
            // no key, assume eclvSol
            return m_nPitch - lmC4PITCH;
    } 
}

// Rectangle that bounds the image. Absolute position referred to page origin
lmLUnits lmNote::GetBoundsTop()
{
    return (m_fStemDown ? m_selRect.GetTop() + m_paperPos.y : GetFinalYStem() );
}

lmLUnits lmNote::GetBoundsBottom()
{
    return (m_fStemDown ? GetFinalYStem() : m_selRect.GetBottom() + m_paperPos.y );
}

lmLUnits lmNote::GetBoundsLeft()
{
    return (m_fStemDown ? GetXStem() : m_selRect.GetLeft() + m_paperPos.x );
}

lmLUnits lmNote::GetBoundsRight()
{
    return (m_fStemDown ? m_selRect.GetRight() + m_paperPos.x : GetXStem() );
}

void lmNote::SetLeft(lmLUnits nLeft)
{
    /*
    The SetLeft() method is overriden to take into account the possible existence of
    associated AuxObjs, such as Ties and Accidentals. These AuxObjs have to be moved
    when the note is moved
    */

    m_paperPos.x = nLeft;

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
        _T("%d\tNote\tType=%d, Pitch=%d, MidiPitch=%d, PosOnStaff=%d, Step=%d, Alter=%d, TimePos=%.2f, rDuration=%.2f, StemType=%d"),
        m_nId, m_nNoteType, m_nPitch, m_nMidiPitch, GetPosOnStaff(), m_nStep, m_nAlter, m_rTimePos, m_rDuration,
        m_nStemType);
    if (m_pTieNext) sDump += _T("TiedNext");
    if (m_pTiePrev) sDump += _T("TiedPrev");
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
        sDump += wxString::Format(_T("xStem=%d, yStem=%d, length=%d"), 
                    m_xStem, m_yStem,m_nStemLength );
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
                      
    return sDump;
    
}

wxString lmNote::SourceLDP()
{
    wxString sSource = _T("            (n ");    
    //(fInChord ? _T("            (NA "), _T("            (N "));
    sSource += MIDINoteToLDPPattern(PitchToMidi(m_nPitch,0), earmDo, (lmPitch*)NULL);
    sSource += _T(" ");
    sSource += GetLDPNoteType();

    //! @todo take tonal key into account

    //! @todo Finish lmNote LDP Source code generation method

//    if (nLigado == eL_Ligada) { m_sFuente = m_sFuente & _T(" L");
//    if (nBeamMode == etaInicioGrupo) {
//        if (nTupla == eTP_Tresillo) {
//            m_sFuente = m_sFuente & _T(" (G + T3)");
//        } else {
//            m_sFuente = m_sFuente & _T(" G+");
//        }
//    } else if (nBeamMode == etaFinGrupo) {
//        m_sFuente = m_sFuente & _T(" G-");
//    }
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
//    sPitch = GetNombreSajon(m_nPitch)
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
bool lmNote::CanBeTied(lmPitch nMidiPitch, int nStep)
{
    //wxLogMessage(wxString::Format(
    //    _T("[lmNote::CanBeTied(%d, %d): m_nMidiPitch=%d, m_nStep=%d"),
    //    nMidiPitch, nStep, m_nMidiPitch, m_nStep));
    if (nMidiPitch != m_nMidiPitch || nStep != m_nStep) return false;
    return true;    // can be tied
}


//======================================================================================
// lmCompositeObj virtual functions implementation
//======================================================================================

lmScoreObj* lmNote::FindSelectableObject(wxPoint& pt)
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

lmPitch StepAndOctaveToPitch(int nStep, int nOctave)
{
    return (nStep + 1) + (7 * nOctave);
}

// PitchToMidiPitch -> PitchToMidi
lmPitch PitchToMidi(lmPitch nPitch, int nAlter)
{
    int nOctave = ((nPitch - 1) / 7) + 1;    
    wxASSERT(lmC4PITCH == 29);    //@attention It's assumed that we start in C0
    lmPitch nMidi = (lmPitch)(nOctave * 12);

    switch(nPitch % 7)
    {
        case 0:  //si
            nMidi = nMidi + 11;
            break;
        case 1:  //do
            //nada. valor correcto
            break;
        case 2:  //re
            nMidi = nMidi + 2;
            break;
        case 3:  //mi
            nMidi = nMidi + 4;
            break;
        case 4:  //fa
            nMidi = nMidi + 5;
            break;
        case 5:  //sol
            nMidi = nMidi + 7;
            break;
        case 6:  //la
            nMidi = nMidi + 9;
            break;
    }
    
    return nMidi + nAlter;
    
}

wxString MIDINoteToLDPPattern(lmPitch nPitchMIDI, EKeySignatures nTonalidad, lmPitch* pPitch)
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
            sPattern = _T("b+ c  d  d+ e  e+ f  f++g  a  a+ b  ");
            sDisplcm = _T("-1 0  1  1  2  2  3  3  4  5  5  6  ");
            break;

        case earmMi:
        case earmDosm:
            //            "C  C# D  D# E  F  F# G  G# A  A# B  " #FCGD
            sPattern = _T("b+ c  c++d  e  e+ f  f++g  a  a+ b  ");
            sDisplcm = _T("-1 0  0  1  2  2  3  3  4  5  5  6  ");
            break;

        case earmSi:
        case earmSolsm:
            //            "C  C# D  D# E  F  F# G  G# A  A# B  " #FCGDA
            sPattern = _T("b+ c  c++d  e  e+ f  f++g  g++a  b  ");
            sDisplcm = _T("-1 0  0  1  2  2  3  3  4  4  5  6  ");
            break;

        case earmFas:
        case earmResm:
            //            "C  C# D  D# E  F  F# G  G# A  A# B  " #FCGDAE
            sPattern = _T("b+ c  c++d  d++e  f  f++g  g++a  b  ");
            sDisplcm = _T("-1 0  0  1  1  2  3  3  4  4  5  6  ");
            break;

        case earmDos:
        case earmLasm:
            //            "C  C# D  D# E  F  F# G  G# A  A# B  " #FCGDAEB
            sPattern = _T("b  c  c++d  d++e  f  f++g  g++a  a++");
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
    wxString sShift = sDisplcm.Mid(i-1, 1);
    bool fOK = sShift.ToLong(&nShift);
    wxASSERT(fOK);

    if (pPitch)    // if requested, return the diatonic pitch
        *pPitch = (lmPitch)(nOctave * 7 + 1) + (lmPitch)nShift;
    
    if (nShift == -1)
        nOctave--;
    else if (nShift == 7)
        nOctave++;
    wxString sOctave = wxString::Format(_T("%d"), nOctave);
    wxString sAnswer = sPattern.Mid(i, 2);        // alterations
    sAnswer.Trim();
    sAnswer += sPattern.Mid(i-1, 1);            // note name
    sAnswer += sOctave;                            // octave
    return sAnswer;
    
}

/*! Returns the note name and octave in the physics namespace
*/
wxString GetNoteNamePhysicists(lmPitch nPitch)
{
    static wxString sNoteName[7] = {
        _("c%d"), _("d%d"), _("e%d"), _("f%d"), _("g%d"), _("a%d"), _("b%d")
    };

    int nOctave = (int)(nPitch - 1) / 7;
    int iNote = (nPitch - 1) % 7;

    return wxString::Format(sNoteName[iNote], nOctave);

}

