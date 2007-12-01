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

//implementation of the Instruments List
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(InstrumentsList);


//Global variables used as default initializators
lmFontInfo g_tInstrumentDefaultFont = { _T("Times New Roman"), 14, lmTEXT_BOLD };


lmInstrument::lmInstrument(lmScore* pScore, int nNumStaves, int nMIDIChannel,
                           int nMIDIInstr, wxString sName, wxString sAbbrev)
    : lmObject(pScore)
{
    //create objects for name and abbreviation
    lmScoreText* pName = (lmScoreText*)NULL;
    lmScoreText* pAbbreviation = (lmScoreText*)NULL;
    if (sName != _T("")) {
        pName = new lmScoreText(pScore, sName, lmALIGN_LEFT,
                           g_tDefaultPos, g_tInstrumentDefaultFont);
    }
    if (sAbbrev != _T("")) {
        pAbbreviation = new lmScoreText(pScore, sAbbrev, lmALIGN_LEFT,
                                   g_tDefaultPos, g_tInstrumentDefaultFont);
    }

    //create the instrument
    Create(pScore, nNumStaves, nMIDIChannel, nMIDIInstr, pName, pAbbreviation);

}

lmInstrument::lmInstrument(lmScore* pScore, int nNumStaves, int nMIDIChannel, int nMIDIInstr,
                 lmScoreText* pName, lmScoreText* pAbbrev)
    : lmObject(pScore)
{
    Create(pScore, nNumStaves, nMIDIChannel, nMIDIInstr, pName, pAbbrev);
}

void lmInstrument::Create(lmScore* pScore, int nNumStaves, int nMIDIChannel, int nMIDIInstr,
                 lmScoreText* pName, lmScoreText* pAbbrev)
{
    m_pScore = pScore;
    m_nMidiInstr = nMIDIInstr;
    m_nMidiChannel = nMIDIChannel;
    m_nIndentFirst = 0;
    m_nIndentOther = 0;
    m_pName = pName;
    m_pAbbreviation = pAbbrev;


    //Normally, only one lmVStaff with one or two lmStaff
    //If more than one, they normally represent overlayered additional voices
    for (int i = 1; i <= nNumStaves; i++) {
        AddVStaff( (i!=1) );    //second and remaining overlayered
    }

    ////include name/abbrev. in global list so that they are selectable for edition
    ////the owner of them is now the score. Do not delete them.
    //if (m_pName) pScore->IncludeInGlobalList(m_pName);
    //if (m_pAbbreviation) pScore->IncludeInGlobalList(m_pAbbreviation);


}

lmInstrument::~lmInstrument()
{
    m_cStaves.DeleteContents(true);
}

void lmInstrument::SetIndent(lmLUnits* pIndent, lmLocation* pPos)
{
    if (pPos->xUnits == lmTENTHS) {
        lmVStaff *pVStaff = GetVStaff(1);
        *pIndent = pVStaff->TenthsToLogical(pPos->x, 1);
    }
    else {
        *pIndent = lmToLogicalUnits(pPos->x, pPos->xUnits);
    }

}

lmVStaff* lmInstrument::AddVStaff(bool fOverlayered)
{
    lmVStaff *pStaff = new lmVStaff(m_pScore, this, fOverlayered);
    m_cStaves.Append(pStaff);
    return pStaff;

}

//returns lmVStaff number nStaff (1..n)
lmVStaff* lmInstrument::GetVStaff(int nStaff)
{
    int i;
    VStavesList::Node *node;
    //iterate over the list to locate lmVStaff nStaff
    for (i=1, node = m_cStaves.GetFirst(); node && i < nStaff; node = node->GetNext(), i++ ) {}
//    wxASSERT_MSG{i != nStaff, _T("No existe el lmVStaff num. nStaff"));
    return node->GetData();
}

wxString lmInstrument::Dump()
{
    wxString sDump;
    wxVStavesListNode *pNode;
    lmVStaff* pVStaff;
    int i;
    //iterate over the list of lmVStaff to dump them
    for (i=1, pNode = m_cStaves.GetFirst(); pNode; pNode = pNode->GetNext(), i++)
    {
        pVStaff = (lmVStaff*) pNode->GetData();
        sDump += wxString::Format(_T("\nVStaff %d\n"), i );
        sDump += pVStaff->Dump();
    }
    return sDump;

}

wxString lmInstrument::SourceLDP(int nIndent)
{
	wxString sSource = _T("");
	sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    sSource += _T("(instrument");

    //num of staves
    wxVStavesListNode *pNode = m_cStaves.GetFirst();
    lmVStaff* pVStaff = (lmVStaff*) pNode->GetData();
    if (pVStaff->GetNumStaves() > 1) {
        sSource += wxString::Format(_T(" (staves %d)"), pVStaff->GetNumStaves());
    }
    sSource += _T("\n");

    //loop for each lmVStaff
    nIndent++;
    for (; pNode; pNode = pNode->GetNext())
    {
        pVStaff = (lmVStaff*) pNode->GetData();
        sSource += pVStaff->SourceLDP(nIndent);
    }

    //close instrument
    nIndent--;
    sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    sSource += _T(")\n");
    return sSource;

}

wxString lmInstrument::SourceXML(int nIndent)
{

	//THINK:
	//MusicXML can not deal with more than one VStaff so I will export only
	//the first VStaff
    wxVStavesListNode *pNode = m_cStaves.GetFirst();
    lmVStaff* pVStaff = (lmVStaff*) pNode->GetData();
	return pVStaff->SourceXML(nIndent);

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
        lmShapeTex2* pShape = m_pName->CreateShape(pPaper);
        // set indent =  text extend + after text space
        m_nIndentFirst = pShape->GetWidth() + 30;    //TODO user options
		delete pShape;
    }

    if (m_pAbbreviation) {
        // measure text extent
        lmShapeTex2* pShape = m_pAbbreviation->CreateShape(pPaper);
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
		//save paper pos
        lmUPoint uPos(pPaper->GetCursorX(), pPaper->GetCursorY());

		//create the shape
        pBox->AddShape( m_pName->CreateShape(pPaper) );

		//restore paper position
		pPaper->SetCursorX(uPos.x);
		pPaper->SetCursorY(uPos.y);
    }
}

void lmInstrument::AddAbbreviationShape(lmBox* pBox, lmPaper* pPaper)
{
    //when this method is invoked paper is positioned at top left corner of instrument
    //renderization point (x = left margin, y = top line of first staff)
    //after rendering, paper position is not advanced

    if (m_pAbbreviation)
	{
		//save paper pos
        lmUPoint uPos(pPaper->GetCursorX(), pPaper->GetCursorY());

		//create the shape
        pBox->AddShape( m_pAbbreviation->CreateShape(pPaper) );

		//restore paper position
		pPaper->SetCursorX(uPos.x);
		pPaper->SetCursorY(uPos.y);
    }
}

