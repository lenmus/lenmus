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
#include "wx/debug.h"
#include "../graphic/GMObject.h"
#include "../graphic/Shapes.h"


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
    if (sName != _T("")) {
        pName = new lmScoreText(sName, lmALIGN_LEFT,
                           g_tDefaultPos, g_tInstrumentDefaultFont);
        pName->SetOwner(this);
    }
    if (sAbbrev != _T("")) {
        pAbbreviation = new lmScoreText(sAbbrev, lmALIGN_LEFT,
                                   g_tDefaultPos, g_tInstrumentDefaultFont);
        pAbbreviation->SetOwner(this);
    }

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
    m_nIndentFirst = 0;
    m_nIndentOther = 0;
    m_pName = pName;
    m_pAbbreviation = pAbbrev;
    m_pVStaff = new lmVStaff(m_pScore, this);
}

lmInstrument::~lmInstrument()
{
	delete m_pVStaff;

	//delete names
	if (m_pName) delete m_pName;
    if (m_pAbbreviation) delete m_pAbbreviation;

}

lmLUnits lmInstrument::TenthsToLogical(lmTenths nTenths)
{
	//TODO
	return 0.0;
}

lmTenths lmInstrument::LogicalToTenths(lmLUnits uUnits)
{
	//TODO
	return 0.0;
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
	// measure the indentation for each instrument. 
    // When this method is invoked paper is positioned at top left corner of instrument
    // renderization point (x = left margin, y = top line of first staff)
	// To measure the names we have to create the shapes but we are going to delete
	// them at the end

    //Save original position to restore it later
    lmLUnits xPaper = pPaper->GetCursorX();

    m_nIndentFirst = 0;
    m_nIndentOther = 0;

    if (m_pName) {
        // measure text extent
        lmShapeText* pShape = m_pName->CreateShape(pPaper, m_uPaperPos);
        // set indent =  text extend + after text space
        m_nIndentFirst = pShape->GetWidth() + 30;    //TODO user options
		delete pShape;
    }

    if (m_pAbbreviation) {
        // measure text extent
        lmShapeText* pShape = m_pAbbreviation->CreateShape(pPaper, m_uPaperPos);
        // set indent =  text extend + after text space
        m_nIndentOther = pShape->GetWidth() + 30;    //TODO user options
		delete pShape;
    }

    //restore original paper position
    pPaper->SetCursorX( xPaper );

}

void lmInstrument::AddNameShape(lmBox* pBox, lmPaper* pPaper)
{
    //when this method is invoked paper is positioned at top left corner of instrument
    //renderization point (x = left margin, y = top line of first staff)
    //after rendering, paper position is not advanced

    if (m_pName)
	{
		//create the shape
        lmUPoint uPos(pPaper->GetCursorX(), pPaper->GetCursorY());
        pBox->AddShape( m_pName->CreateShape(pPaper, uPos) );

		////restore paper position
		//pPaper->SetCursorX(uPos.x);
		//pPaper->SetCursorY(uPos.y);
    }
}

void lmInstrument::AddAbbreviationShape(lmBox* pBox, lmPaper* pPaper)
{
    //when this method is invoked paper is positioned at top left corner of instrument
    //renderization point (x = left margin, y = top line of first staff)
    //after rendering, paper position is not advanced

    if (m_pAbbreviation)
	{
		//create the shape
        lmUPoint uPos(pPaper->GetCursorX(), pPaper->GetCursorY());
        pBox->AddShape( m_pAbbreviation->CreateShape(pPaper, uPos) );

		////restore paper position
		//pPaper->SetCursorX(uPos.x);
		//pPaper->SetCursorY(uPos.y);
    }
}

