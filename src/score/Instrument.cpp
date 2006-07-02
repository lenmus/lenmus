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
/*! @file Instrument.cpp
    @brief Implementation file for class lmInstrument
    @ingroup score_kernel
*/
/*! class lmInstrument    A collection of Staves

    An lmInstrument is a collection of Staves. Usually only one staff or two staves
    (piano grand staff) but could be any other number.

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

#include "Score.h"
#include "wx/debug.h"

//implementation of the Instruments List
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(InstrumentsList);


//Global variables used as default initializators
lmFontInfo g_tInstrumentDefaultFont = { _T("Times New Roman"), 14, lmTEXT_BOLD };


lmInstrument::lmInstrument(lmScore* pScore, int nNumStaves, int nMIDIChannel,
                           int nMIDIInstr, wxString sName, wxString sAbbrev)
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

    //include name/abbrev. in global list so that they are selectable for edition
    //the owner of them is now the score. Do not delete them.
    if (m_pName) pScore->IncludeInGlobalList(m_pName);
    if (m_pAbbreviation) pScore->IncludeInGlobalList(m_pAbbreviation);


}

lmInstrument::~lmInstrument()
{
    m_cStaves.DeleteContents(true);

    //remove name/abbrev. from global list and delete them
    if (m_pName) {
        m_pScore->RemoveFromGlobalList(m_pName);
        delete m_pName;
    }
    if (m_pAbbreviation) {
        m_pScore->RemoveFromGlobalList(m_pAbbreviation);
        delete m_pAbbreviation;
    }

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

wxString lmInstrument::SourceLDP()
{
    wxString sSource = _T("   (instrument");

    //num of staves
    wxVStavesListNode *pNode = m_cStaves.GetFirst();
    lmVStaff* pVStaff = (lmVStaff*) pNode->GetData();
    if (pVStaff->GetNumStaves() > 1) {
        sSource += wxString::Format(_T(" (staves %d)"), pVStaff->GetNumStaves());
    }

    //loop for each lmVStaff
    for (; pNode; pNode = pNode->GetNext())
    {
        pVStaff = (lmVStaff*) pNode->GetData();
        sSource += _T("\n      (voice\n");
        sSource += pVStaff->SourceLDP();
        sSource += _T("      )\n");
    }

    sSource += _T("   )\n");
    return sSource;

}

wxString lmInstrument::SourceXML()
{
    wxString sSource = _T("TODO: lmInstrument XML Source code generation methods");
    return sSource;

}

void lmInstrument::MeasureNames(lmPaper* pPaper)
{
    //when this method is invoked paper is positioned at top left corner of instrument
    //renderization point (x = left margin, y = top line of first staff)

    //As name/abbreviation are StaffObjs, method Draw() advances paper to
    //end of name/abbreviation. Let's save original position to restore it
    lmLUnits xPaper = pPaper->GetCursorX();

    m_nIndentFirst = 0;
    m_nIndentOther = 0;

    if (m_pName) {
        // measure text extent
        m_pName->Draw(DO_MEASURE, pPaper);
        // set indent =  text extend + after text space
        m_nIndentFirst = m_pName->GetSelRect().width + 30;    //! @todo user options

        //lmLUnits xPaperPos = pPaper->GetCursorX();
        //lmLUnits yPaperPos = pPaper->GetCursorY();

        ////if need to reposition paper, convert units to tenths
        //lmLUnits xPos, yPos;
        //lmLocation tPos = m_pName->GetLocation();
        //if (tPos.xType != lmLOCATION_DEFAULT) {
        //    if (tPos.xUnits == lmTENTHS)
        //        xPos = tPos.x;
        //    else
        //        xPos = lmToLogicalUnits(tPos.x, tPos.xUnits);
        //}

        //if (tPos.yType != lmLOCATION_DEFAULT) {
        //    if (tPos.yUnits == lmTENTHS)
        //        yPos = tPos.y;
        //    else
        //        yPos = lmToLogicalUnits(tPos.y, tPos.yUnits);
        //}

        ////reposition paper according text required positioning info
        //if (tPos.xType == lmLOCATION_RELATIVE) {
        //    xPaperPos += xPos;
        //}
        //else if (tPos.xType == lmLOCATION_ABSOLUTE) {
        //    xPaperPos = xPos + pPaper->GetLeftMarginXPos();
        //}

        //if (tPos.yType == lmLOCATION_RELATIVE) {
        //    yPaperPos += yPos;
        //}
        //else if (tPos.yType == lmLOCATION_ABSOLUTE) {
        //    yPaperPos = yPos + pPaper->GetPageTopMargin();
        //}
        //pPaper->SetCursorY( yPaperPos );

        ////Ignore alignment. Always align left.
        //if (tPos.xType == lmLOCATION_DEFAULT)
        //    pPaper->SetCursorX(pPaper->GetLeftMarginXPos());
        //else
        //    pPaper->SetCursorX( xPaperPos );

        //m_pName->Draw(DO_MEASURE, pPaper);
        //m_nIndentFirst = m_pName->GetSelRect().width + 30;    //! @todo user options
    }

    if (m_pAbbreviation) {
        // measure text extent
        m_pAbbreviation->Draw(DO_MEASURE, pPaper);
        // set indent =  text extend + after text space
        m_nIndentOther = m_pAbbreviation->GetSelRect().width + 30;    //! @todo user options
    }

    //restore original paper position
    pPaper->SetCursorX( xPaper );

}

void lmInstrument::DrawName(lmPaper* pPaper, wxColour colorC)
{
    //when this method is invoked paper is positioned at top left corner of instrument
    //renderization point (x = left margin, y = top line of first staff)
    //after rendering, paper position is not advanced

    if (m_pName) {
        //As name/abbreviation are StaffObjs, method Draw() should be invoked but
        //it draws the object not at current paper pos but at stored m_paperPos.
        //It also performs other non necessary thigs.
        //So, I will invoke directly DarwObject and, previouly, set the text
        //position at current paper position
        m_pName->MoveTo(wxPoint(pPaper->GetCursorX(), pPaper->GetCursorY()));
        m_pName->DrawObject(DO_DRAW, pPaper, colorC);
    }
}

void lmInstrument::DrawAbbreviation(lmPaper* pPaper, wxColour colorC)
{
    //when this method is invoked paper is positioned at top left corner of instrument
    //renderization point (x = left margin, y = top line of first staff)
    //after rendering, paper position is not advanced

    if (m_pAbbreviation) {
        //As name/abbreviation are StaffObjs, method Draw() should be invoked but
        //it draws the object not at current paper pos but at stored m_paperPos.
        //It also performs other non necessary thigs.
        //More: Abbreviation is written on every system but Draw will always
        //draw it at the same position.
        //So, I will invoke directly DarwObject and, previouly, set the text
        //position at current paper position.
        m_pAbbreviation->MoveTo(wxPoint(pPaper->GetCursorX(), pPaper->GetCursorY()));
        m_pAbbreviation->DrawObject(DO_DRAW, pPaper, colorC);
    }
}

