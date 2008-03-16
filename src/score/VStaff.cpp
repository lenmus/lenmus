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

//--------------------------------------------------------------------------------------------------
// class lmVStaff: A generalization of the 'staff' concept.
//
//    The object lmVStaff (Virtual staff) is a generalization of the concept 'staff': it is a staff
//    with an great number of lines and spaces (almost infinite) so to represent all posible pitches,
//    but whose visual renderization can be controlled so
//    that only the specified (needed) lines are rendered i.e: the standard five-lines staff, the piano
//    grand staff, the single line staff used for rithm or unpitched music, or any other design you
//    would like.
//
//    An lmVStaff can contain many 'staffs' (groups of lines rendered toghether), depending on the layout
//    choosen. For example:
//    - the standard five-lines staff is an lmVStaff with one five-lines staff.
//    - the grand staff is an lmVStaff with two five-lines staves.
//    - the single line unpitched staff is an VSatff with one sigle line staff.
//
//    The concept of 'staff' is not modelled into the program, its is just a term to be used only
//    in documentation, algoritms explanations, etc.
//
//    An lmVStaff is composed by objects such as notes, rests, barlines, keys, and all other
//    musical signs traditionally used for writing music.
//    All this objects than can appear in a staff will be modeled by an abstract class 'lmStaffObj'.
//    Therefore an lmVStaff is, roughfly, a collection of StaffObjs and some attributes.
//
//    Bars are not modelled by objects. They are just the collection of lmStaffObj found between
//    two lmStaffObj of type 'barline' (and between the start of the score and the first barline).
//    Nevertheless, tha concept of bar is very important at least in two situations:
//        1. In interpretation (playing back the score). For example:  play from bar #7
//        2. When rendering the score, as all the objects in a bar must be rendered together in
//           the same paper line.
//    Due to this, althoug the bar is not modelled as an object, there exits methods in the
//    VStaff object to deal with bars.
//
//-------------------------------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "VStaff.h"
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
#include "UndoRedo.h"
#include "Notation.h"
#include "MetronomeMark.h"
#include "../app/global.h"
#include "../app/TheApp.h"		//to access g_rScreenDPI and g_rPixelsPerLU
#include "../graphic/GMObject.h"
#include "../graphic/ShapeStaff.h"
#include "../graphic/BoxSliceVStaff.h"
#include "../graphic/ShapeBarline.h"


//for AddShitTimeTag methods
#define lmGO_FWD   true
#define lmGO_BACK  false

//constructor
lmVStaff::lmVStaff(lmScore* pScore, lmInstrument* pInstr)
    : lmScoreObj(pScore), m_nNumStaves(1), m_cStaffObjs(this, 1)    // 1 = m_nNumStaves
{
    //pScore is the lmScore to which this vstaff belongs.
    //Initially the lmVStaff will have only one standard five-lines staff. This can be
    //later modified invoking the 'AddStaff' method (REVIEW)

    m_pScore = pScore;
    m_pInstrument = pInstr;

    // default lmVStaff margins (logical units = tenths of mm)
    m_nHeight = 0;          //a value of 0 means 'compute it'
    m_leftMargin = 0;
    m_topMargin = 0;
    m_rightMargin = 0;
    m_bottomMargin = lmToLogicalUnits(1, lmCENTIMETERS);    // 1 cm

    //initialize staves
    for (int i=0; i < lmMAX_STAFF; i++)
        m_cStaves[i] = (lmStaff*)NULL;

    //create one standard staff (five lines, 7.2 mm height)
    lmStaff* pStaff = new lmStaff(pScore);
    m_cStaves[0] = pStaff;

	//link cursor to collection
	m_VCursor.AttachToCollection(&m_cStaffObjs);

 //   //Add EOS control object to the StaffObjs collection
 //   //m_cStaffObjs.Add( new lmBarline(lm_eBarlineEOS, this, lmNO_VISIBLE) );
	//AddBarline(lm_eBarlineEOS, lmVISIBLE);

    //default value
    //TODO review this fixed space before the clef
    m_nSpaceBeforeClef = TenthsToLogical(10, 1);    // one line of first staff

    g_pLastNoteRest = (lmNoteRest*)NULL;
    m_sErrorMsg = _T("");


}

lmVStaff::~lmVStaff()
{
    for (int i=0; i < lmMAX_STAFF; i++)
        if (m_cStaves[i]) delete m_cStaves[i];
}

lmLUnits lmVStaff::TenthsToLogical(lmTenths nTenths)
{
	return TenthsToLogical(nTenths, 1);
}

lmTenths lmVStaff::LogicalToTenths(lmLUnits uUnits)
{
	return LogicalToTenths(uUnits, 1);
}

lmStaff* lmVStaff::AddStaff(int nNumLines, lmLUnits nMicrons)
{
    wxASSERT(m_nNumStaves < lmMAX_STAFF);
    lmStaff* pStaff = new lmStaff(m_pScore, nNumLines, nMicrons);
    m_cStaves[m_nNumStaves] = pStaff;
    m_nNumStaves++;
    m_cStaffObjs.AddStaff();

    return pStaff;

}

lmStaff* lmVStaff::GetStaff(int nStaff)
{
    return m_cStaves[nStaff-1];
}

lmStaff* lmVStaff::GetFirstStaff()
{
    m_nCurStaff = 0;
    return m_cStaves[0];
}

lmStaff* lmVStaff::GetNextStaff()
{
    if (++m_nCurStaff < m_nNumStaves)
        return m_cStaves[m_nCurStaff];
    else
        return (lmStaff*)NULL;
}

lmStaff* lmVStaff::GetLastStaff()
{
    m_nCurStaff = m_nNumStaves-1;
    return m_cStaves[m_nCurStaff];
}

lmLUnits lmVStaff::TenthsToLogical(lmTenths nTenths, int nStaff)
{
    lmStaff* pStaff = GetStaff(nStaff);
    wxASSERT(pStaff);
    return pStaff->TenthsToLogical(nTenths);
}

lmTenths lmVStaff::LogicalToTenths(lmLUnits uUnits, int nStaff)
{
    lmStaff* pStaff = GetStaff(nStaff);
    wxASSERT(pStaff);
    return pStaff->LogicalToTenths(uUnits);
}

lmLUnits lmVStaff::GetStaffLineThick(int nStaff)
{
    lmStaff* pStaff = GetStaff(nStaff);
    wxASSERT(pStaff);
    return pStaff->GetLineThick();

}

//----------------------------------------------------------------------------------------
// contexts related
//----------------------------------------------------------------------------------------

void lmVStaff::OnContextUpdated(lmNote* pStartNote, int nStaff, int nStep,
                           int nNewAccidentals, lmContext* pCurrentContext)
{
    // Note pStartNote (whose diatonic name is nStep) has accidentals that must be 
	// propagated to the context and to the following notes until the end of the measure
	// or until a new accidental for the same step is found

}

int lmVStaff::GetUpdatedContextAccidentals(lmStaffObj* pThisSO, int nStep)
{
	//returns the current context, updated with the accidentals for step nStep,
    //applicable to StaffObj pThisSO.

	lmContext* pContext = m_cStaffObjs.NewUpdatedContext(pThisSO);
	if (pContext)
	{
		int nAcc = pContext->GetAccidentals(nStep);
		delete pContext;
		return nAcc;
	}
	else
		return 0;
}

//---------------------------------------------------------------------------------------
// Methods for inserting StaffObjs
//---------------------------------------------------------------------------------------

lmClef* lmVStaff::InsertClef(lmEClefType nClefType)
{
    lmStaffObj* pCursorSO = m_VCursor.GetStaffObj();
    int nStaff = pCursorSO->GetStaffNum();
    lmClef* pClef = new lmClef(nClefType, this, nStaff, lmVISIBLE);
    lmStaff* pStaff = GetStaff(nStaff);
    lmContext* pContext = pStaff->NewContextAfter(pClef, GetCurrentContext(pCursorSO));
	pClef->SetContext(pContext);
    m_cStaffObjs.Add(pClef);
    return pClef;
}

lmBarline* lmVStaff::InsertBarline(lmEBarline nType)
{
    lmBarline* pBarline = new lmBarline(nType, this, lmVISIBLE);
    m_cStaffObjs.Add(pBarline);
    return pBarline;
}

lmNote* lmVStaff::Cmd_InsertNote(lmUndoData* pUndoData,
								 lmEPitchType nPitchType, wxString sStep,
								 wxString sOctave, lmENoteType nNoteType, float rDuration,
								 lmENoteHeads nNotehead, lmEAccidentals nAcc)
{
    lmStaffObj* pCursorSO = m_VCursor.GetStaffObj();
    int nStaff = pCursorSO->GetStaffNum();

	//get the applicable context
	lmContext* pContext = NewUpdatedContext(pCursorSO);

    lmTBeamInfo BeamInfo[6];
    for (int i=0; i < 6; i++) {
        BeamInfo[i].Repeat = false;
        BeamInfo[i].Type = eBeamNone;
    }
	wxString sAccidentals = _T("");

	//TODO: For now, only auto-voice. It is necessary to get info from GUI about
	//user selected voice. Need to change this command parameter list to include voice
	int nVoice = 0;     //auto-voice
    lmNote* pNt = new lmNote(this, nPitchType,
                        sStep, sOctave, sAccidentals, nAcc,
                        nNoteType, rDuration, false, false, nStaff, nVoice, lmVISIBLE,
                        pContext, false, BeamInfo, false, false, lmSTEM_DEFAULT);

    m_cStaffObjs.Add(pNt);

	delete pContext;
    return pNt;
}

void lmVStaff::DeleteObject()
{
    //delete object pointed by cursor

    m_cStaffObjs.Delete(m_VCursor.GetStaffObj());
}

void lmVStaff::DeleteObject(lmStaffObj* pSO)
{
    //delete requested object
    //Precondition: must be in this VStaff

    m_cStaffObjs.Delete(pSO);
}

void lmVStaff::Cmd_DeleteObject(lmUndoData* pUndoData, lmStaffObj* pSO)
{
    //delete the requested object, and log info to undo history

    //AWARE: Logged actions must be logged in the required order for re-construction. 
    //History works as a FIFO stack: first one logged will be the first one to be recovered

    //save positioning information
    m_cStaffObjs.LogPosition(pUndoData, pSO);

    //Save info to re-create the object
    ((lmNote*)pSO)->Freeze(pUndoData);

    //Delete the object
    m_cStaffObjs.Delete(pSO, false);    //false = do not delete object, only remove it from collection

}

void lmVStaff::Cmd_Undo_DeleteObject(lmUndoData* pUndoData, lmStaffObj* pSO)
{
    //un-delete the object, according to info in history

    //recover positioning info
    lmStaffObj* pBeforeSO = pUndoData->GetParam<lmStaffObj*>();

    //unfreeze restored object
    ((lmNote*)pSO)->UnFreeze(pUndoData);

	//re-insert the deleted object
    m_cStaffObjs.Insert(pSO, pBeforeSO);
	//wxLogMessage(m_cStaffObjs.Dump());
}


//---------------------------------------------------------------------------------------
// Methods for adding StaffObjs
//---------------------------------------------------------------------------------------

// adds a clef to the end of current StaffObjs collection
lmClef* lmVStaff::AddClef(lmEClefType nClefType, int nStaff, bool fVisible)
{
    wxASSERT(nStaff <= GetNumStaves());

    lmClef* pClef = new lmClef(nClefType, this, nStaff, fVisible);
    lmStaff* pStaff = GetStaff(nStaff);
    lmContext* pContext = pStaff->NewContextAfter(pClef);
	pClef->SetContext(pContext);
    m_cStaffObjs.Add(pClef);
    return pClef;
}

// adds a spacer to the end of current StaffObjs collection
lmSpacer* lmVStaff::AddSpacer(lmTenths nWidth)
{
    lmSpacer* pSpacer = new lmSpacer(this, nWidth);
    m_cStaffObjs.Add(pSpacer);
    return pSpacer;

}

lmStaffObj* lmVStaff::AddAnchorObj()
{
    // adds an anchor StaffObj to the end of current StaffObjs collection

    if (IsGlobalStaff())
    {
        lmScoreAnchor* pAnchor = new lmScoreAnchor(this);
        m_cStaffObjs.Add(pAnchor);
        return pAnchor;
    }
    else
    {
        lmAnchor* pAnchor = new lmAnchor(this);
        m_cStaffObjs.Add(pAnchor);
        return pAnchor;
    }

}

// returns a pointer to the lmNote object just created
lmNote* lmVStaff::AddNote(lmEPitchType nPitchType,
                    wxString sStep, wxString sOctave, wxString sAlter,
                    lmEAccidentals nAccidentals,
                    lmENoteType nNoteType, float rDuration,
                    bool fDotted, bool fDoubleDotted,
                    int nStaff, int nVoice, bool fVisible,
                    bool fBeamed, lmTBeamInfo BeamInfo[],
                    bool fInChord,
                    bool fTie,
                    lmEStemType nStem)
{

    wxASSERT(nStaff <= GetNumStaves() );
    lmContext* pContext = NewUpdatedLastContext(nStaff);

    lmNote* pNt = new lmNote(this, nPitchType,
                        sStep, sOctave, sAlter, nAccidentals,
                        nNoteType, rDuration, fDotted, fDoubleDotted, nStaff, nVoice,
						fVisible, pContext, fBeamed, BeamInfo, fInChord, fTie, nStem);

    m_cStaffObjs.Add(pNt);

	delete pContext;
    return pNt;
}

// returns a pointer to the lmRest object just created
lmRest* lmVStaff::AddRest(lmENoteType nNoteType, float rDuration,
                      bool fDotted, bool fDoubleDotted,
                      int nStaff, int nVoice, bool fVisible,
                      bool fBeamed, lmTBeamInfo BeamInfo[])
{
    wxASSERT(nStaff <= GetNumStaves() );

    lmRest* pR = new lmRest(this, nNoteType, rDuration, fDotted, fDoubleDotted, nStaff,
							nVoice, fVisible, fBeamed, BeamInfo);

    m_cStaffObjs.Add(pR);
    return pR;

}

lmStaffObj* lmVStaff::AddText(wxString sText, lmEAlignment nAlign,
                            lmLocation* pPos, lmFontInfo tFontData, bool fHasWidth)
{
    lmScoreText* pText = new lmScoreText(sText, nAlign, *pPos, tFontData);

    // create an anchor object
    lmStaffObj* pAnchor;
    if (fHasWidth)
    {
        //attach it to a spacer
        pAnchor = this->AddSpacer( pText->GetShap2()->GetWidth() );
    }
    else
    {
        //No width. Attach it to an anchor
        pAnchor = AddAnchorObj();
    }
    pAnchor->AttachAuxObj(pText);

    return pAnchor;

}

lmMetronomeMark* lmVStaff::AddMetronomeMark(int nTicksPerMinute,
                        bool fParentheses, bool fVisible)
{
    lmMetronomeMark* pMM = new lmMetronomeMark(this, nTicksPerMinute,
                                               fParentheses, fVisible);
    m_cStaffObjs.Add(pMM);
    return pMM;

}

lmMetronomeMark* lmVStaff::AddMetronomeMark(lmENoteType nLeftNoteType, int nLeftDots,
                        lmENoteType nRightNoteType, int nRightDots,
                        bool fParentheses, bool fVisible)
{
    lmMetronomeMark* pMM = new lmMetronomeMark(this, nLeftNoteType, nLeftDots,
                                               nRightNoteType, nRightDots,
                                               fParentheses, fVisible);
    m_cStaffObjs.Add(pMM);
    return pMM;

}

lmMetronomeMark* lmVStaff::AddMetronomeMark(lmENoteType nLeftNoteType, int nLeftDots,
                        int nTicksPerMinute, bool fParentheses, bool fVisible)
{
    lmMetronomeMark* pMM = new lmMetronomeMark(this, nLeftNoteType, nLeftDots,
                                               nTicksPerMinute,
                                               fParentheses, fVisible);
    m_cStaffObjs.Add(pMM);
    return pMM;

}


//for types eTS_Common, eTS_Cut and eTS_SenzaMisura
lmTimeSignature* lmVStaff::AddTimeSignature(lmETimeSignatureType nType, bool fVisible)
{
    lmTimeSignature* pTS = new lmTimeSignature(nType, this, fVisible);
    return AddTimeSignature(pTS);
}

//for type eTS_SingleNumber
lmTimeSignature* lmVStaff::AddTimeSignature(int nSingleNumber, bool fVisible)
{
    lmTimeSignature* pTS = new lmTimeSignature(nSingleNumber, this, fVisible);
    return AddTimeSignature(pTS);
}

//for type eTS_Composite
lmTimeSignature* lmVStaff::AddTimeSignature(int nNumBeats, int nBeats[], int nBeatType,
                                        bool fVisible)
{
    lmTimeSignature* pTS = new lmTimeSignature(nNumBeats, nBeats, nBeatType, this, fVisible);
    return AddTimeSignature(pTS);
}

//for type eTS_Multiple
lmTimeSignature* lmVStaff::AddTimeSignature(int nNumFractions, int nBeats[], int nBeatType[],
                                        bool fVisible)
{
    lmTimeSignature* pTS = new lmTimeSignature(nNumFractions, nBeats, nBeatType, this, fVisible);
    return AddTimeSignature(pTS);
}

//for type eTS_Normal
lmTimeSignature* lmVStaff::AddTimeSignature(int nBeats, int nBeatType, bool fVisible)
{
    lmTimeSignature* pTS = new lmTimeSignature(nBeats, nBeatType, this, fVisible);
    return AddTimeSignature(pTS);
}

lmTimeSignature* lmVStaff::AddTimeSignature(lmETimeSignature nTimeSign, bool fVisible)
{
    lmTimeSignature* pTS = new lmTimeSignature(nTimeSign, this, fVisible);
    return AddTimeSignature(pTS);
}


//common code for all time signatures types
lmTimeSignature* lmVStaff::AddTimeSignature(lmTimeSignature* pTS)
{
    //iterate over the collection of Staves to add a new context
    for (int nStaff=1; nStaff <= m_nNumStaves; nStaff++)
	{
        lmStaff* pStaff = GetStaff(nStaff);
	    lmContext* pContext = pStaff->NewContextAfter(pTS);
        pTS->SetContext(nStaff, pContext);
    }
    m_cStaffObjs.Add(pTS);
    return pTS;
}

lmKeySignature* lmVStaff::AddKeySignature(int nFifths, bool fMajor, bool fVisible)
{
    lmKeySignature* pKS = new lmKeySignature(nFifths, fMajor, this, fVisible);

    //iterate over the collection of Staves to add a new context
    for (int nStaff=1; nStaff <= m_nNumStaves; nStaff++)
	{
        lmStaff* pStaff = GetStaff(nStaff);
	    lmContext* pContext = pStaff->NewContextAfter(pKS);
        pKS->SetContext(nStaff, pContext);
    }
    m_cStaffObjs.Add(pKS);
    return pKS;
}

lmKeySignature* lmVStaff::AddKeySignature(lmEKeySignatures nKeySignature, bool fVisible)
{
    int nFifths = KeySignatureToNumFifths(nKeySignature);
    bool fMajor = IsMajor(nKeySignature);
    return AddKeySignature(nFifths, fMajor, fVisible);
}

int lmVStaff::GetNumMeasures()
{
    return m_cStaffObjs.GetNumMeasures();
}

lmLUnits lmVStaff::LayoutStaffLines(lmBox* pBox, lmLUnits xFrom, lmLUnits xTo, lmLUnits yPos)
{
    //Computes all staff lines of this lmVStaff and creates the necessary shapes
	//to render them. Add this shapes to the received lmBox object.
    //Returns the Y coord. of last line (line 1, last staff)

    if (HideStaffLines()) return 0.0;

    //Set left position and lenght of lines, and save these values
    lmLUnits yCur = yPos + m_topMargin;
    m_yLinTop = yCur;              //save y coord. for first line start point

    //iterate over the collection of Staves (lmStaff Objects)
    for (int nStaff=1; nStaff <= m_nNumStaves; nStaff++)
	{
        lmStaff* pStaff = GetStaff(nStaff);
        //draw one staff
		lmShapeStaff* pShape = 
				new lmShapeStaff(pStaff, nStaff, pStaff->GetNumLines(), 
								 pStaff->GetLineThick(), pStaff->GetLineSpacing(),
								 xFrom, yCur, xTo, *wxBLACK );
		pBox->AddShape(pShape);
        yCur = pShape->GetYBottom() + pStaff->GetAfterSpace();
		m_yLinBottom = pShape->GetYBottom() - pStaff->GetLineThick();  
    }
	return m_yLinBottom;
}

void lmVStaff::SetUpFonts(lmPaper* pPaper)
{
    //iterate over the collection of Staves (lmStaff Objects) to set up the fonts
    // to use on that staff
    for (int nStaff=1; nStaff <= m_nNumStaves; nStaff++)
	{
        SetFont(GetStaff(nStaff), pPaper);
    }
}

void lmVStaff::SetFont(lmStaff* pStaff, lmPaper* pPaper)
{
    // Font "LeMus Notas" has been designed to draw on a staff whose interline
    // space is of 512 FUnits. This gives an optimal rendering on VGA displays (96 pixels per inch)
    // as staff lines are drawn on exact pixels, according to the following relationships:
    //       Let dyLines be the distance between lines (pixels), then
    //       Font size = 3 * dyLines   (points)
    //       Scale = 100 * dyLines / 8     (%)
    //
    // Given a zooming factor (as a percentage, i.e. zoom=250.0%) fontsize can be computed as
    //       i = Round((zoom*8) / 100)
    //       dyLines = i        (pixels)
    //       FontSize = 3*i        (points)
    //
    // As all scaling takes place in the DC it is not necessary to allocate fonts of
    // different size as all scaling takes place in the DC. Then:
    //       Let dyLines be the distance between lines (logical units), then
    //       Font size = 3 * dyLines   (logical points)

    lmLUnits dyLinesL = pStaff->GetLineSpacing();

    // the font for drawing is scaled by the DC.
    pStaff->SetFontDraw( pPaper->GetFont((int)(3.0 * dyLinesL * 96.0/g_rScreenDPI), _T("LenMus Basic") ) );        //logical points

    //wxLogMessage(_T("[lmVStaff::SetFont] dyLinesL=%d"), dyLinesL);

    //// the font for dragging is not scaled by the DC as all dragging operations takes
    //// place dealing with device units
    //int dyLinesD = pPaper->LogicalToDeviceY(100 * dyLinesL);
    //pStaff->SetFontDrag( pPaper->GetFont((3 * dyLinesD) / 100) );

}

lmLUnits lmVStaff::GetStaffOffset(int nStaff)
{
    //returns the Y offset to staff nStaff (1..n)
    wxASSERT(nStaff <= m_nNumStaves );
    lmLUnits yOffset = m_topMargin;

    // iterate over the collection of Staves (lmStaff Objects) to add up the
    // height and after space of all previous staves to the requested one
    for (int iS=1; iS < nStaff; iS++)
	{
        lmStaff* pStaff = GetStaff(iS);
        yOffset += pStaff->GetHeight();
        yOffset += pStaff->GetAfterSpace();
    }
    return yOffset;

}

wxString lmVStaff::Dump()
{
    return m_cStaffObjs.Dump();
}

wxString lmVStaff::SourceLDP(int nIndent)
{
	wxString sSource = _T("");
    sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    sSource += _T("(musicData\n");
    nIndent++;

    //iterate over the collection of StaffObjs, ordered by voice.
    //Measures must be processed one by one
    for (int nMeasure=1; nMeasure <= m_cStaffObjs.GetNumMeasures(); nMeasure++)
    {
        //add comment to separate measures
        sSource += _T("\n");
        sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
        sSource += wxString::Format(_T("//Measure %d\n"), nMeasure);

        int nNumVoices = m_cStaffObjs.GetNumVoicesInMeasure(nMeasure);
        int nVoice = 1;
		lmBarline* pBL = (lmBarline*)NULL;
        bool fGoBack = false;
		float rTime = 0.0f;
        while (true)
        {
            lmSOIterator* pIT = m_cStaffObjs.CreateIterator(eTR_ByTime, nVoice);
            pIT->AdvanceToMeasure(nMeasure);
            while(!pIT->EndOfMeasure())
            {
                lmStaffObj* pSO = pIT->GetCurrent();
                //voice 0 staffobjs go with first voice if more than one voice
				if (!pSO->IsBarline())
				{
					if (nVoice == 1)
					{
						if (!pSO->IsNoteRest() || ((lmNoteRest*)pSO)->GetVoice() == nVoice)
						{
							LDP_AddShitTimeTagIfNeeded(sSource, nIndent, lmGO_FWD, rTime, pSO);
							sSource += pSO->SourceLDP(nIndent);
							rTime = LDP_AdvanceTimeCounter(pSO);
						}
					}
					else
						if (pSO->IsNoteRest() && ((lmNoteRest*)pSO)->GetVoice() == nVoice)
						{
							LDP_AddShitTimeTagIfNeeded(sSource, nIndent, lmGO_FWD, rTime, pSO);
							sSource += pSO->SourceLDP(nIndent);
							rTime = LDP_AdvanceTimeCounter(pSO);
						}
				}
				else
					pBL = (lmBarline*)pSO;

                pIT->MoveNext();
            }
            delete pIT;

            //check if more voices
            nVoice++;
            if (nVoice >= nNumVoices) break;

            //there are more voices. Add (goBak) tag
            fGoBack = true;
            sSource += _T("\n");
            sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
            sSource += _T("(goBack start)\n");
            rTime = 0.0f;
        }

        //if goBack added, add a goFwd to ensure that we are at end of measure
        if (fGoBack)
        {
            sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
            sSource += _T("(goFwd end)\n");
        }

		//add barline, if present
		if (pBL)
			sSource += pBL->SourceLDP(nIndent);

    }

    //close musicData
    nIndent--;
    sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    sSource += _T(")\n");

    return sSource;

}

void lmVStaff::LDP_AddShitTimeTagIfNeeded(wxString& sSource, int nIndent, bool fFwd,
									      float rTime, lmStaffObj* pSO)
{
	if (!IsEqualTime(rTime, pSO->GetTimePos()))
	{
		//wxLogMessage(_T("[lmVStaff::LDP_AddShitTimeTagIfNeeded] Different times: %.2f %.2f"), rTime, pSO->GetTimePos() );
		sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
		if (fFwd)
			sSource += _T("(goFwd ");
		else
			sSource += _T("(goBack ");
		sSource += wxString::Format(_T("%.0f)\n"), pSO->GetTimePos());
	}
}

float lmVStaff::LDP_AdvanceTimeCounter(lmStaffObj* pSO)
{
	float rTime = pSO->GetTimePos();
	
	//advance counter unless current object is a note in chord and is not the
	//last note in the chord
	bool fAdvance = true;
	if (pSO->IsNoteRest() && !((lmNote*)pSO)->IsRest())
	{
		lmNote* pNote = (lmNote*)pSO;
		if (pNote->IsInChord() && !pNote->GetChord()->IsLastNoteOfChord(pNote))
			fAdvance = false;
	}

	if (fAdvance)
		return rTime + pSO->GetTimePosIncrement();
	else
		return rTime;
}

void lmVStaff::XML_AddShitTimeTagIfNeeded(wxString& sSource, int nIndent, bool fFwd,
										  float rTime, lmStaffObj* pSO)
{
	if (!IsEqualTime(rTime, pSO->GetTimePos()))
        XML_AddShitTimeTag(sSource, nIndent, fFwd, pSO->GetTimePos());
}

void lmVStaff::XML_AddShitTimeTag(wxString& sSource, int nIndent, bool fFwd, float rTime)
{
	sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
	if (fFwd)
		sSource += _T("<forward>\n");
	else
		sSource += _T("<backup>\n");

	//duration
	nIndent++;
	sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
	sSource += wxString::Format(_T("<duration>%.0f"), rTime);
	sSource += _T("</duration>\n");

	//close tag
	nIndent--;
	sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
	if (fFwd)
		sSource += _T("</forward>\n");
	else
		sSource += _T("</backup>\n");
}

wxString lmVStaff::SourceXML(int nIndent)
{
	wxString sSource = _T("");

    //iterate over the collection of StaffObjs, ordered by voice.
    //Measures must be processed one by one
    for (int nMeasure=1; nMeasure <= m_cStaffObjs.GetNumMeasures(); nMeasure++)
    {
        //start measure
		sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
		sSource += wxString::Format(_T("<measure number='%d'>\n"), nMeasure);
		nIndent++;

		bool fStartAttributes = true;
        int nNumVoices = m_cStaffObjs.GetNumVoicesInMeasure(nMeasure);
        int nVoice = 1;
		lmBarline* pBL = (lmBarline*)NULL;
        bool fGoBack = false;
		float rTime = 0.0f;
        while (true)
        {
            lmSOIterator* pIT = m_cStaffObjs.CreateIterator(eTR_ByTime, nVoice);
            pIT->AdvanceToMeasure(nMeasure);
            while(!pIT->EndOfMeasure())
            {
                lmStaffObj* pSO = pIT->GetCurrent();
                //voice 0 staffobjs go with first voice if more than one voice
				if (!pSO->IsBarline())
				{
					if (nVoice == 1)
					{
						if (!pSO->IsNoteRest() || ((lmNoteRest*)pSO)->GetVoice() == nVoice)
						{
							//check if this is a clef, time signature or key signature
							if (pSO->IsClef() || pSO->IsKeySignature() ||
								pSO->IsTimeSignature())
							{
								if (fStartAttributes)
								{
									//start <attributes> tag
									sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
									sSource += _T("<attributes>\n");
									fStartAttributes = false;
									nIndent++;
								}
							}
							else
							{
								if (!fStartAttributes)
								{
									//close <attributes> tag
									nIndent--;
									sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
									sSource += _T("</attributes>\n");
									fStartAttributes = true;
								}
							}

							//add xml source for this staffobj
							XML_AddShitTimeTagIfNeeded(sSource, nIndent, lmGO_FWD, rTime, pSO);
							sSource += pSO->SourceXML(nIndent);
							rTime = pSO->GetTimePos() + pSO->GetTimePosIncrement();
						}
					}
					else
					{
						if (pSO->IsNoteRest() && ((lmNoteRest*)pSO)->GetVoice() == nVoice)
						{
							if (!fStartAttributes)
							{
								nIndent--;
								sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
								sSource += _T("</attributes>\n");
								fStartAttributes = true;
							}
							//add xml source for this staffobj
							XML_AddShitTimeTagIfNeeded(sSource, nIndent, lmGO_FWD, rTime, pSO);
							sSource += pSO->SourceXML(nIndent);
							rTime = pSO->GetTimePos() + pSO->GetTimePosIncrement();
						}
					}
				}
				else
					pBL = (lmBarline*)pSO;

                pIT->MoveNext();
            }
            delete pIT;

            //check if more voices
            nVoice++;
            if (nVoice >= nNumVoices) break;

            //there are more voices. Add <backup> tag
            fGoBack = true;
            XML_AddShitTimeTag(sSource, nIndent, lmGO_BACK, rTime);
            rTime = 0.0f;
        }

        //if goBack added, add a goFwd to ensure that we are at end of measure
        if (fGoBack)
        {
            rTime = m_cStaffObjs.GetMeasureDuration(nMeasure) - rTime;
            XML_AddShitTimeTag(sSource, nIndent, lmGO_FWD, rTime);
        }

		//add barline, if present
		if (pBL)
			sSource += pBL->SourceXML(nIndent);

		//close measure
		nIndent--;
		sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
		sSource += _T("</measure>\n");
    }

    return sSource;
}

// the next two methods are mainly used for drawing the barlines. For that purpose it is necessary
// to know the y coordinate of the top most upper line of first staff and the bottom most lower
// line of the last staff.

//TODO: These methods must be moved to lmBoxSystem / lmBoxSlice

lmLUnits lmVStaff::GetYTop()
{
    return m_yLinTop;
}

lmLUnits lmVStaff::GetYBottom()
{
    return m_yLinBottom;
}

lmLUnits lmVStaff::GetVStaffHeight()
{
    if (m_nHeight == 0) {
        m_nHeight = m_topMargin + m_bottomMargin;
        // iterate over the collection of Staves (lmStaff Objects) to add up its
        // height and its after space
        for (int nStaff=1; nStaff <= m_nNumStaves; nStaff++)
        {
            lmStaff* pStaff = GetStaff(nStaff);
            m_nHeight += pStaff->GetHeight();
            m_nHeight += pStaff->GetAfterSpace();
        }
    }

    return m_nHeight;

}

lmBarline* lmVStaff::AddBarline(lmEBarline nType, bool fVisible)
{
    //create and save the barline
    lmBarline* pBarline = new lmBarline(nType, this, fVisible);
    m_cStaffObjs.Add(pBarline);
    return pBarline;
}

bool lmVStaff::GetXPosFinalBarline(lmLUnits* pPos)
{
    // returns true if a barline is found and in this case updates content
    // of variable pointed by pPos with the right x position of last barline
    // This method is only used by Formatter, in order to not justify the last system
    lmStaffObj* pSO = (lmStaffObj*) NULL;
    lmSOIterator* pIter = m_cStaffObjs.CreateIterator(eTR_ByTime);
    pIter->MoveLast();
    while(true)
    {
        pSO = pIter->GetCurrent();
        if (pSO->GetClass() == eSFOT_Barline) break;
		if(pIter->StartOfList()) break;
        pIter->MovePrev();
    }
    delete pIter;

    //check that a barline is found. Otherwise no barlines in the score
    if (pSO->GetClass() == eSFOT_Barline) {
        lmShape* pShape = (lmShape*)pSO->GetShap2();
        if (!pShape) return false;
		*pPos = pShape->GetXRight();
        return true;
    }
    else
        return false;

}

void lmVStaff::NewLine(lmPaper* pPaper)
{
    //move x cursor to the left and advance y cursor the space
    //height of all stafves of this lmVStaff
    pPaper->NewLine(GetVStaffHeight());

}

//void lmVStaff::AddPrologShapes(lmBoxSliceVStaff* pBSV, int nMeasure, bool fDrawTimekey,
//							   lmPaper* pPaper)
//{
//    // The prolog (clef and key signature) must be rendered on each system,
//    // but the matching StaffObjs only exist in the first system. Therefore, in the
//    // normal staffobj rendering process, the prolog would be rendered only in
//    // the first system.
//    // So, for the other systems it is necessary to force the rendering
//    // of the prolog because there are no StaffObjs representing it.
//    // This method does it.
//    //
//    // To know what clef, key and time signature to draw we take this information from the
//    // context associated to first note of the measure on each staff. If there are no notes,
//    // the context is taken from the barline. If, finally, no context is found, no prolog
//    // is drawn.
//
//    lmLUnits nPrologWidth = 0;
//    lmClef* pClef = (lmClef*)NULL;
//    lmEClefType nClef = lmE_Undefined;
//    lmKeySignature* pKey = (lmKeySignature*)NULL;
//    lmTimeSignature* pTime = (lmTimeSignature*)NULL;
//
//    //AWARE when this method is invoked the paper position must be at the left marging,
//    //at the start of a new system.
//    lmLUnits xStartPos = pPaper->GetCursorX() + m_nSpaceBeforeClef;         //Save x to align all clefs
//    lmLUnits yStartPos = pPaper->GetCursorY();
//
//    //iterate over the collection of lmStaff objects to draw current clef and key signature
//
//    wxStaffListNode* pNode = m_cStaves.GetFirst();
//    lmStaff* pStaff = (lmStaff*)NULL;
//    lmLUnits yOffset = m_topMargin;
//    lmLUnits xPos=0;
//
//    lmContext* pContext = (lmContext*)NULL;
//    lmStaffObj* pSO = (lmStaffObj*) NULL;
//    lmNoteRest* pNR = (lmNoteRest*)NULL;
//    lmNote* pNote = (lmNote*)NULL;
//    for (int nStaff=1; pNode; pNode = pNode->GetNext(), nStaff++)
//    {
//        pStaff = (lmStaff *)pNode->GetData();
//        xPos = xStartPos;
//
//        //locate first context for this staff
//        pContext = (lmContext*)NULL;
//        lmSOIterator* pIter = m_cStaffObjs.CreateIterator(eTR_ByTime);
//        pIter->AdvanceToMeasure(nMeasure);
//        while(!pIter->EndOfList())
//		{
//            pSO = pIter->GetCurrent();
//            if (pSO->GetClass() == eSFOT_NoteRest) {
//                pNR = (lmNoteRest*)pSO;
//                if (!pNR->IsRest() && pNR->GetStaffNum() == nStaff)
//				{
//                    //OK. Note fount. Take context
//                    pNote = (lmNote*)pSO;
//                    pContext = pNote->GetCurrentContext();
//                    break;
//                }
//            }
//            else if (pSO->GetClass() == eSFOT_Barline)
//			{
//				//end of measure reached. Take content
//                lmBarline* pBar = (lmBarline*)pSO;
//                pContext = pBar->GetContext(nStaff);
//                break;
//            }
//            pIter->MoveNext();
//        }
//        delete pIter;
//
//        if (pContext) {
//            pClef = pContext->GetClef();
//            pKey = pContext->GeyKey();
//            pTime = pContext->GetTime();
//
//            //render clef
//            if (pClef) {
//                nClef = pClef->GetClefType();
//				if (pClef->IsVisible()) {
//					lmUPoint uPos = lmUPoint(xPos, yStartPos+yOffset);        //absolute position
//					lmShape* pShape = pClef->AddShape(pBSV, pPaper, uPos);
//					xPos += pShape->GetWidth();
//				}
//            }
//
//            //render key signature
//            if (pKey && pKey->IsVisible()) {
//                wxASSERT(nClef != lmE_Undefined);
//                lmUPoint uPos = lmUPoint(xPos, yStartPos+yOffset);        //absolute position
//                lmShape* pShape = pKey->AddShape(pBSV, pPaper, uPos, nClef, nStaff);
//                xPos += pShape->GetWidth();
//            }
//
//        }
//
//        //compute prolog width
//        nPrologWidth = wxMax(nPrologWidth, xPos - xStartPos);
//
//        //compute vertical displacement for next staff
//        yOffset += pStaff->GetHeight();
//        yOffset += pStaff->GetAfterSpace();
//
//    }
//
//    // update paper cursor position
//    pPaper->SetCursorX(xStartPos + nPrologWidth);
//
//}

lmSoundManager* lmVStaff::ComputeMidiEvents(int nChannel)
{
    /*
    nChannel is the MIDI channel to use for all events of this lmVStaff.
    Returns the lmSoundManager object. It is not retained by the lmVStaff, so it is caller
    responsibility to delete it when no longer needed.
    */

    //TODO review this commented code
//    Dim nMetrica As lmETimeSignature, nDurCompas As Long, nTiempoIni As Long
//
//    nMetrica = this.MetricaInicial
//    nDurCompas = GetDuracionMetrica(nMetrica)
//    nTiempoIni = nDurCompas - this.DuracionCompas(1)
//
//    wxASSERT(nTiempoIni >= 0        //El compas tiene más notas de las que caben
//
//    //Si el primer compas no es anacrusa, retrasa todo un compas para que
//    //se marque un compas completo antes de comenzar
//    if (nTiempoIni = 0 { nTiempoIni = nDurCompas

    //Create lmSoundManager and initialize MIDI events table
    lmSoundManager* pSM = new lmSoundManager();
    //TODO review next line
//    pSM->Inicializar GetStaffsCompas(nMetrica), nTiempoIni, nDurCompas, this.NumCompases

    //iterate over the collection to create the MIDI events
    float rMeasureStartTime = 0;
    int nMeasure = 1;        //to count measures (1 based, normal musicians way)
    pSM->StoreMeasureStartTime(nMeasure, rMeasureStartTime);

    //iterate over the collection to create the MIDI events
    lmStaffObj* pSO;
    lmNoteRest* pNR;
    lmTimeSignature* pTS;
    lmSOIterator* pIter = m_cStaffObjs.CreateIterator(eTR_ByTime);
    while(!pIter->EndOfList()) {
        pSO = pIter->GetCurrent();
        if (pSO->GetClass() == eSFOT_NoteRest) {
            pNR = (lmNoteRest*)pSO;
            pNR->AddMidiEvents(pSM, rMeasureStartTime, nChannel, nMeasure);
        }
        else if (pSO->GetClass() == eSFOT_Barline) {
            rMeasureStartTime += pSO->GetTimePos();        //add measure duration
            nMeasure++;
            pSM->StoreMeasureStartTime(nMeasure, rMeasureStartTime);
        }
        else if (pSO->GetClass() == eSFOT_TimeSignature) {
            //add a RhythmChange event to set up tempo (num beats, duration of a beat)
            pTS = (lmTimeSignature*)pSO;
            pTS->AddMidiEvent(pSM, rMeasureStartTime, nMeasure);
        }
        pIter->MoveNext();
    }
    delete pIter;

    return pSM;

}

lmNote* lmVStaff::FindPossibleStartOfTie(lmAPitch anPitch)
{
    //
    // This method is invoked from lmNote constructor to find if the note being created
    // (the "target note") is tied to a previous one ("the candidate" one).
    // This method explores backwards to try to find a note that can be tied with the received
    // as parameter (the "target note").
    //
    // Algorithm:
    // Find the first previous note of the same pitch, in this measure or
    // in the previous one
    //


    //define a backwards iterator
    bool fInPrevMeasure = false;
    lmStaffObj* pSO = (lmStaffObj*) NULL;
    lmNoteRest* pNR = (lmNoteRest*)NULL;
    lmNote* pNote = (lmNote*)NULL;
    lmSOIterator* pIter = m_cStaffObjs.CreateIterator(eTR_ByTime);
    pIter->MoveLast();
    while(!pIter->EndOfList() && ! pIter->StartOfList())
    {
        pSO = pIter->GetCurrent();
        switch (pSO->GetClass()) {
            case eSFOT_NoteRest:
                pNR = (lmNoteRest*)pSO;
                if (!pNR->IsRest()) {
                    pNote = (lmNote*)pSO;
                    if (pNote->CanBeTied(anPitch)) {
                        delete pIter;
                        return pNote;    // candidate found
                    }
                }
                break;

            case eSFOT_Barline:
                if (fInPrevMeasure) {
                    delete pIter;
                    return (lmNote*)NULL;        // no suitable note found
                }
                fInPrevMeasure = true;
                break;

            default:
                ;
        }
		if(pIter->StartOfList()) break;
        pIter->MovePrev();
    }
    delete pIter;
    return (lmNote*)NULL;        //no suitable note found

}

bool lmVStaff::ShiftTime(float rTimeShift)
{
    //Shifts the time counter.
    //Returns true if error

    return m_cStaffObjs.ShiftTime(rTimeShift);
}

lmSOControl* lmVStaff::AddNewSystem()
{
    /*
    Inserts a control lmStaffObj to signal a new system
    */

    //Insert the control object
    lmSOControl* pControl = new lmSOControl(lmNEW_SYSTEM, this);
    m_cStaffObjs.Add(pControl);
    return pControl;

}

lmSOIterator* lmVStaff::CreateIterator(ETraversingOrder nOrder)
{
    return m_cStaffObjs.CreateIterator(nOrder);
}

//void lmVStaff::AutoBeam(int nMeasure)
//{
//    //loop to process all StaffObjs in this measure
//    EStaffObjType nType;                    //type of score obj being processed
//    lmStaffObj* pSO = (lmStaffObj*)NULL;
//    lmSOIterator* pIT = pVStaff->CreateIterator(eTR_ByTime);
//    pIT->AdvanceToMeasure(nAbsMeasure);
//    while(!pIT->EndOfList())
//    {
//        pSO = pIT->GetCurrent();
//        nType = pSO->GetClass();
//
//        if (nType == eSFOT_NoteRest) {
//            pNoteRest = (lmNoteRest*)pSO;
//            pNote = (lmNote*)pSO;        //AWARE we do not know yet if it is a note or a rest,
//                                    //but I force the casting to simplify next if statement
//            if (!pNoteRest->IsRest() && pNote->IsInChord())
//            {
//            }
//			else
//            {
//                //Is a rest or is a lmNote not in chord. Store its x position
//            }
//
//        }
//
//        pIT->MoveNext();
//    }
//    delete pIT;
//}
//
