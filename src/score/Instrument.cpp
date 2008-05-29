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

// lmInstrument    A collection of Staves
//
// An lmInstrument is a collection of Staves. Usually only one staff or two staves
// (piano grand staff) but could be any other number.

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "Instrument.h"
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
#include "VStaff.h"
#include "InstrGroup.h"
#include "wx/debug.h"
#include "../graphic/GMObject.h"
#include "../graphic/Shapes.h"
#include "../graphic/ShapeBracket.h"


//Global variables used as default initializators
lmFontInfo g_tInstrumentDefaultFont = { _T("Times New Roman"), 14, lmTEXT_BOLD };


//=======================================================================================
// lmInstrument implementation
//=======================================================================================

lmInstrument::lmInstrument(lmScore* pScore, int nMIDIChannel,
                           int nMIDIInstr, wxString sName, wxString sAbbrev)
    : lmScoreObj(pScore)
{
    //create objects for name and abbreviation
    lmScoreText* pName = (lmScoreText*)NULL;
    lmScoreText* pAbbreviation = (lmScoreText*)NULL;

    if (sName != _T(""))
        pName = new lmScoreText(sName, lmALIGN_LEFT,
                           g_tDefaultPos, g_tInstrumentDefaultFont);

    if (sAbbrev != _T(""))
        pAbbreviation = new lmScoreText(sAbbrev, lmALIGN_LEFT,
                                   g_tDefaultPos, g_tInstrumentDefaultFont);

    //create the instrument
    Create(pScore, nMIDIChannel, nMIDIInstr, pName, pAbbreviation);
}

lmInstrument::lmInstrument(lmScore* pScore, int nMIDIChannel, int nMIDIInstr,
						   lmScoreText* pName, lmScoreText* pAbbrev)
    : lmScoreObj(pScore)
{
    Create(pScore, nMIDIChannel, nMIDIInstr, pName, pAbbrev);
}

void lmInstrument::Create(lmScore* pScore, int nMIDIChannel, int nMIDIInstr,
						  lmScoreText* pName, lmScoreText* pAbbrev)
{
    m_pScore = pScore;
    m_nMidiInstr = nMIDIInstr;
    m_nMidiChannel = nMIDIChannel;
    m_uIndentFirst = 0;
    m_uIndentOther = 0;
    m_pVStaff = new lmVStaff(m_pScore, this);
    m_pGroup = (lmInstrGroup*)NULL;
    m_nBracket = lm_eBracketDefault;
    m_uBracketWidth = 0.0f;
    m_uBracketGap = 0.0f;

    m_pName = pName;
    if (pName)
        pName->SetOwner(this);

    m_pAbbreviation = pAbbrev;
    if (pAbbrev)
        pAbbrev->SetOwner(this);
}

lmInstrument::~lmInstrument()
{
	delete m_pVStaff;

	//delete names
	if (m_pName) delete m_pName;
    if (m_pAbbreviation) delete m_pAbbreviation;

    //remove from group
    if (m_pGroup)
    {
        m_pGroup->Remove(this);
		if (m_pGroup->NumInstruments() <= 1)
			delete m_pGroup;
	}
}

lmLUnits lmInstrument::TenthsToLogical(lmTenths nTenths)
{
    return GetVStaff()->TenthsToLogical(nTenths, 1);
}

lmTenths lmInstrument::LogicalToTenths(lmLUnits uUnits)
{
    return GetVStaff()->LogicalToTenths(uUnits, 1);
}

void lmInstrument::SetIndent(lmLUnits* pIndent, lmLocation* pPos)
{
    if (pPos->xUnits == lmTENTHS) {
        lmVStaff *pVStaff = GetVStaff();
        *pIndent = pVStaff->TenthsToLogical(pPos->x, 1);
    }
    else {
        *pIndent = lmToLogicalUnits(pPos->x, pPos->xUnits);
    }

}

wxString lmInstrument::Dump()
{
    wxString sDump = _T("\nVStaff\n");
    sDump += m_pVStaff->Dump();
    return sDump;

}

wxString lmInstrument::SourceLDP(int nIndent)
{
	wxString sSource = _T("");
	sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    sSource += _T("(instrument");

    //num of staves
	sSource += wxString::Format(_T(" (staves %d)\n"), m_pVStaff->GetNumStaves());

    //the music data (lmVStaff)
	nIndent++;
	sSource += m_pVStaff->SourceLDP(nIndent);
	nIndent--;

    //close instrument
    sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    sSource += _T(")\n");
    return sSource;

}

wxString lmInstrument::SourceXML(int nIndent)
{
	wxString sSource = _T("");
	sSource += m_pVStaff->SourceXML(nIndent);
	return sSource;

}

wxString lmInstrument::GetInstrName()
{
    if (m_pName)
		return m_pName->GetText();

	wxString sName = _T("");
	return sName;
}

void lmInstrument::MeasureNames(lmPaper* pPaper)
{
	// This method is invoked only from lmFormatter4::RenderJustified(), in order to
	// measure the indentation for each instrument, so that a suitable common indentation
    // value for the whole system can be computed.
    //
    // When this method is invoked paper is positioned at top left corner of instrument
    // renderization point (x = left margin, y = top line of first staff)
	// To measure the names we need to create the shapes but they are deleted before
	// returning

    //Save original position to restore it later
    lmLUnits xPaper = pPaper->GetCursorX();

    //if this instrument is in a group get group names & brace widths
    if (m_pGroup)
    {
        if (m_pGroup->GetFirstInstrument() == this)
        {
            m_pGroup->MeasureNames(pPaper);
        }
        m_uIndentFirst = m_pGroup->GetIndentFirst();
        m_uIndentOther = m_pGroup->GetIndentOther();
    }
    else
    {
        m_uIndentFirst = 0;
        m_uIndentOther = 0;
    }

    //TODO: user options
    lmLUnits uSpaceAfterName = TenthsToLogical(10.0f);

    if (m_pName)
    {
        // measure text extent
        lmShapeText* pShape = m_pName->CreateShape(pPaper, m_uPaperPos);
        // set indent =  text extend + after text space
        m_uIndentFirst += pShape->GetWidth() + uSpaceAfterName;
		delete pShape;
    }

    if (m_pAbbreviation)
    {
        // measure text extent
        lmShapeText* pShape = m_pAbbreviation->CreateShape(pPaper, m_uPaperPos);
        // set indent =  text extend + after text space
        m_uIndentOther += pShape->GetWidth() + uSpaceAfterName;
		delete pShape;
    }

    if (RenderBraket())
    {
        //TODO: user options
        m_uBracketWidth = TenthsToLogical(20.0f);
        m_uBracketGap = TenthsToLogical(5.0f);

        m_uIndentOther += m_uBracketWidth + m_uBracketGap;
        m_uIndentFirst += m_uBracketWidth + m_uBracketGap;
    }

    //restore original paper position
    pPaper->SetCursorX( xPaper );

}

void lmInstrument::AddNameAndBracket(lmBox* pBSystem, lmBox* pBSliceInstr, lmPaper* pPaper,
                                     int nSystem)
{
    //Layout.
    // invoked when layouting first measure in system, to add instrument name and bracket/brace.
    // This method is also responsible for managing group name and bracket/brace layout
    // When reaching this point, BoxSystem and BoxSliceInstr have their bounds correctly
    // set (except xRight)

	if (nSystem == 1)
        AddNameAbbrevShape(pBSliceInstr, pPaper, m_pName);
	else
        AddNameAbbrevShape(pBSliceInstr, pPaper, m_pAbbreviation);

    // if first instrument in group, save yTop position for group
    static lmLUnits yTopGroup;
	if (IsFirstOfGroup())
		yTopGroup = pBSliceInstr->GetYTop();

    // if last instrument of a group, add group name and bracket/brace
	if (IsLastOfGroup())
        m_pGroup->AddNameAndBracket(pBSystem, pPaper, nSystem, pBSliceInstr->GetXLeft(),
                                    yTopGroup, pBSliceInstr->GetYBottom() );
}

void lmInstrument::AddNameAbbrevShape(lmBox* pBox, lmPaper* pPaper, lmScoreText* pName)
{
    //get box position
    lmUPoint uBox(pBox->GetXLeft(), pBox->GetYTop() );

    //add shape for the bracket, if necessary
    if (RenderBraket())
    {
        lmLUnits xLeft = uBox.x - m_uBracketWidth - m_uBracketGap;
        lmLUnits xRight = uBox.x - m_uBracketGap;
        lmLUnits yBottom = pBox->GetYBottom();
        lmEBracketSymbol nSymbol = (m_nBracket == lm_eBracketDefault ? lm_eBracket : m_nBracket);
        lmShape* pShape = new lmShapeBracket(this, nSymbol, xLeft, uBox.y,
                                    xRight, yBottom, *wxBLACK);
        pBox->AddShape( pShape );
    }

    //add shape for the name/abbreviation
    if (pName)
    {
        lmUPoint uPos(pPaper->GetPageLeftMargin(), uBox.y);
        lmShape* pShape = pName->CreateShape(pPaper, uPos);
        pShape->Shift(0.0f, (pBox->GetHeight() - pShape->GetHeight())/2.0f );
        pBox->AddShape( pShape );
    }
}

bool lmInstrument::RenderBraket()
{
    //returns true if a brace / bracket must be rendered

    return (m_nBracket == lm_eBracketDefault && m_pVStaff->GetNumStaves() > 1 ||
            m_nBracket == lm_eBracket ||
            m_nBracket == lm_eBrace );
}
void lmInstrument::OnRemovedFromGroup(lmInstrGroup* pGroup)
{
	//AWARE: this method is invoked only when the group is being deleted and
	//this deletion is not requested by this instrument. If this instrument would
    //like to delete the group it MUST invoke Remove(this) before deleting the 
	//group object

    m_pGroup = (lmInstrGroup*)NULL;
}

void lmInstrument::OnIncludedInGroup(lmInstrGroup* pGroup)
{
    m_pGroup = pGroup;
}

bool lmInstrument::IsLastOfGroup()
{
    return (m_pGroup && m_pGroup->GetLastInstrument() == this);
}

bool lmInstrument::IsFirstOfGroup()
{
    return (m_pGroup && m_pGroup->GetFirstInstrument() == this);
}

lmVStaffCursor* lmInstrument::GetVCursor() 
{ 
    return m_pVStaff->GetVCursor();
}

void lmInstrument::ResetCursor() 
{ 
    m_pVStaff->ResetCursor(); 
}

lmVStaffCursor* lmInstrument::AttachCursor(lmScoreCursor* pSCursor)
{ 
    return m_pVStaff->GetVCursor()->AttachCursor(pSCursor); 
}

void lmInstrument::DetachCursor() 
{ 
    m_pVStaff->GetVCursor()->DetachCursor(); 
}

int lmInstrument::GetNumStaves() 
{ 
    return m_pVStaff->GetNumStaves(); 
}

