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

#ifdef __GNUG__
#pragma implementation "Score.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/file.h>
#include "wx/debug.h"
#include "wx/list.h"
#include "Score.h"
#include "VStaff.h"
#include "../app/global.h"
#include "../sound/SoundEvents.h"
#include "../graphic/GMObject.h"
#include "../graphic/Shapes.h"

// global unique variables used during score building
lmNoteRest*    g_pLastNoteRest;
lmBeam*        g_pCurBeam;            // lmBeam object that is being built ot NULL if none in process. See lmNote constructor

// access to colors
#include "../globals/Colors.h"
extern lmColors* g_pColors;

//to give a unique ID to each score
static long m_nCounterID = 0;

//---------------------------------------------------------------------------------------
// lmScore constructors and destructor
//---------------------------------------------------------------------------------------

lmScore::lmScore() : lmScoreObj((lmScoreObj*)NULL)
{
    //Set up an empty score, that is, without any lmInstrument.

    m_nID = ++m_nCounterID;

    //initializations
    m_pSoundMngr = (lmSoundManager*)NULL;
    m_sScoreName = _T("New score");

    //TODO user options, not a constant
    m_nTopSystemDistance = lmToLogicalUnits(2, lmCENTIMETERS);    // 2 cm

    //default renderization options
    m_nRenderizationType = eRenderJustified;

    //default ObjOptions
    SetOption(_T("Score.FillPageWithEmptyStaves"), false);
    SetOption(_T("StaffLines.StopAtFinalBarline"), true);
    SetOption(_T("StaffLines.Hide"), false);
    SetOption(_T("Staff.DrawLeftBarline"), true);
    SetOption(_T("Staff.UpperLegerLines.Displacement"), 0L);

    //default options for renderization algorithms
        // Note spacing is proportional to duration.
        // As the duration of quarter note is 64 (duration units), I am
        // going to map it to 35 tenths. This gives a conversion factor
        // of 35/64 = 0.547
    SetOption(_T("Render.SpacingFactor"), 0.547);
    SetOption(_T("Render.SpacingMethod"), (long)esm_PropConstantFixed);
    SetOption(_T("Render.SpacingValue"), 15L);       // 15 tenths (1.5 lines)
}

lmScore::~lmScore()
{
    m_cInstruments.DeleteContents(true);
    m_cInstruments.Clear();

    if (m_pSoundMngr) {
        m_pSoundMngr->DeleteEventsTable();
        delete m_pSoundMngr;
    }

    m_cHighlighted.DeleteContents(false);    //Staffobjs must not be deleted, only the list
    m_cHighlighted.Clear();

    //delete list of title indexes
    m_nTitles.clear();

}

lmUPoint lmScore::GetReferencePos(lmPaper* pPaper)
{
	//TODO
	return lmUPoint(0.0, 0.0);
}

lmLUnits lmScore::TenthsToLogical(lmTenths nTenths)
{
	//TODO
	return 0.0;
}

lmTenths lmScore::LogicalToTenths(lmLUnits uUnits)
{
	//TODO
	return 0.0;
}

void lmScore::AddTitle(wxString sTitle, lmEAlignment nAlign, lmLocation tPos,
                       wxString sFontName, int nFontSize, lmETextStyle nStyle)
{
    lmFontInfo tFont;
    tFont.nFontSize = nFontSize;
    tFont.nStyle = nStyle;
    tFont.sFontName = sFontName;

    lmScoreText* pTitle = new lmScoreText(sTitle, nAlign, tPos, tFont);
    m_nTitles.push_back( AttachAuxObj(pTitle) );

}

wxString lmScore::GetScoreName()
{
    // returns the name of this score (the file name)
    return m_sScoreName;

}

void lmScore::SetScoreName(wxString sName)
{
    m_sScoreName = sName;
}


int lmScore::GetNumMeasures()
{
    //LIMIT
	//it is being assumed that all instruments and staves have the same number of bars
    InstrumentsList::Node *node = m_cInstruments.GetFirst();
    lmInstrument *pInstr = node->GetData();
    lmVStaff *pStaff = pInstr->GetVStaff(1);
    return(pStaff->GetNumMeasures());
}

lmInstrument* lmScore::AddInstrument(int nVStaves,
                                     int nMIDIChannel, int nMIDIInstr,
                                     wxString sName, wxString sAbbrev)
{
    //add an lmInstrument with nVStaves (1..m) empty VStaves.
    //nMIDIChannel is the MIDI channel to use for playing this instrument

    lmInstrument* pInstr = new lmInstrument(this, nVStaves, nMIDIChannel, nMIDIInstr,
                                            sName, sAbbrev);
    m_cInstruments.Append(pInstr);
    return pInstr;

}

lmInstrument* lmScore::AddInstrument(int nVStaves, int nMIDIChannel, int nMIDIInstr,
                                lmScoreText* pName, lmScoreText* pAbbrev)
{
    //add an lmInstrument with nVStaves (1..m) empty VStaves.
    //nMIDIChannel is the MIDI channel to use for playing this instrument

    lmInstrument* pInstr = new lmInstrument(this, nVStaves, nMIDIChannel, nMIDIInstr,
                                            pName, pAbbrev);
    m_cInstruments.Append(pInstr);
    return pInstr;

}

lmVStaff* lmScore::GetVStaff(int nInstr, int nVStaff)
{
	//returns lmVStaff number nVStaff (1..n), of lmInstrument nInstr (1..m)

	int i;
    InstrumentsList::Node *node;
    lmInstrument *pInstr;
    //iterate over the list to locate instrument nInstr
    for (i=1, node = m_cInstruments.GetFirst();
        node && i < nInstr; node = node->GetNext(), i++ ) {}
    pInstr = node->GetData();
    return(pInstr->GetVStaff(nVStaff));
}

lmInstrument* lmScore::XML_FindInstrument(wxString sId)
{
    // iterate over instrument list to find the one with id == sId
    wxInstrumentsListNode *node;
    lmInstrument* pInstr = (lmInstrument*)NULL;
    for (node = m_cInstruments.GetFirst(); node; node = node->GetNext()) {
        pInstr = (lmInstrument*)node->GetData();
        if (pInstr->XML_GetId() == sId) break;
    }
    return pInstr;
}

void lmScore::LayoutTitles(lmBox* pBox, lmPaper *pPaper)
{
    lmLUnits uyStartPos = pPaper->GetCursorY();		//save, to measure height

    lmScoreText* pTitle;
    lmLUnits nPrevTitleHeight = 0;
    for (int i=0; i < (int)m_nTitles.size(); i++)
    {
        pTitle = (lmScoreText*)(*m_pAuxObjs)[m_nTitles[i]];
		nPrevTitleHeight = CreateTitleShape(pBox, pPaper, pTitle, nPrevTitleHeight);
    }

	m_nHeadersHeight = pPaper->GetCursorY() - uyStartPos;

}

lmLUnits lmScore::CreateTitleShape(lmBox* pBox, lmPaper *pPaper, lmScoreText* pTitle,
								   lmLUnits nPrevTitleHeight)
{
    // Creates the shape for the title and adds it to the box.
	// Returns height of title

    lmLUnits nWidth, nHeight;
	lmShapeTex2* pShape = (lmShapeTex2*)NULL;

    //// if not yet measured and positioned do it
    //if (!pTitle->IsFixed())
    //{
        lmEAlignment nAlign = pTitle->GetAlignment();
        lmLUnits xInitPaperPos = pPaper->GetCursorX();
        lmLUnits xPaperPos = xInitPaperPos;
        lmLUnits yPaperPos = pPaper->GetCursorY();

        //if need to reposition paper, convert units to tenths
        lmLUnits xPos, yPos;
        lmLocation tPos = pTitle->GetLocation();
        if (tPos.xType != lmLOCATION_DEFAULT) {
            if (tPos.xUnits == lmTENTHS)
                xPos = tPos.x;
            else
                xPos = lmToLogicalUnits(tPos.x, tPos.xUnits);
        }

        if (tPos.yType != lmLOCATION_DEFAULT) {
            if (tPos.yUnits == lmTENTHS)
                yPos = tPos.y;
            else
                yPos = lmToLogicalUnits(tPos.y, tPos.yUnits);
        }

        //reposition paper according text required positioning info
        if (tPos.xType == lmLOCATION_USER_RELATIVE) {
            xPaperPos += xPos;
        }
        else if (tPos.xType == lmLOCATION_USER_ABSOLUTE) {
            xPaperPos = xPos + pPaper->GetLeftMarginXPos();
        }

        if (tPos.yType == lmLOCATION_USER_RELATIVE) {
            yPaperPos += yPos;
        }
        else if (tPos.yType == lmLOCATION_USER_ABSOLUTE) {
            yPaperPos = yPos + pPaper->GetPageTopMargin();
        }
        pPaper->SetCursorY( yPaperPos );

        //measure the text so that it can be properly positioned
        pShape = pTitle->CreateShape(pPaper);
        pPaper->SetCursorX(xInitPaperPos);      //restore values altered by CreateShape
        pPaper->SetCursorY(yPaperPos);
        nWidth = pShape->GetWidth();
        nHeight = pShape->GetHeight();

        //Force new line if no space in current line
        lmLUnits xSpace = pPaper->GetRightMarginXPos() - xInitPaperPos;
        if (xSpace < nWidth) {
            pPaper->SetCursorX(pPaper->GetLeftMarginXPos());
            pPaper->SetCursorY(pPaper->GetCursorY() + nPrevTitleHeight);
        }

        if (nAlign == lmALIGN_CENTER)
        {
            // 'center' alignment forces to center the string in current line,
            // without taking into account the space consumed by any posible existing
            // left title. That is, 'center' always means 'centered in the line'

            if (tPos.xType == lmLOCATION_DEFAULT) {
                xPos = (pPaper->GetRightMarginXPos() - pPaper->GetLeftMarginXPos() - nWidth)/2;
                //force new line if not enough space
                if (pPaper->GetCursorX() > xPos)
                    pPaper->SetCursorY(pPaper->GetCursorY() + nPrevTitleHeight);
                pPaper->SetCursorX(pPaper->GetLeftMarginXPos() + xPos);
            }
            else {
                pPaper->SetCursorX( xPaperPos );
            }
        }

        else if (nAlign == lmALIGN_LEFT)
        {
            //align left.
            if (tPos.xType == lmLOCATION_DEFAULT)
                pPaper->SetCursorX(pPaper->GetLeftMarginXPos());
            else
                pPaper->SetCursorX( xPaperPos );
        }

        else
        {
            //align right
            if (tPos.xType == lmLOCATION_DEFAULT)
                pPaper->SetCursorX(pPaper->GetRightMarginXPos() - nWidth);
            else
                pPaper->SetCursorX(xPaperPos - nWidth);
        }
    //}

	//the position has been computed. Create the shape if not yet created or
	//update it, if its was created during measurements 
	if (pShape) delete pShape;
	pShape = pTitle->CreateShape(pPaper);

	//add shape to the box
	pBox->AddShape(pShape);

    nHeight = pShape->GetHeight();

    //if rigth aligned, advance new line
    if (pTitle->GetAlignment() == lmALIGN_RIGHT) {
        pPaper->SetCursorX(pPaper->GetLeftMarginXPos());
        pPaper->IncrementCursorY( nHeight );
    }

    return nHeight;

}

lmInstrument* lmScore::GetFirstInstrument()
{
    m_pNode = m_cInstruments.GetFirst();
    return (m_pNode ? (lmInstrument *)m_pNode->GetData() : (lmInstrument *)m_pNode);
}

lmInstrument* lmScore::GetNextInstrument()
{
    wxASSERT(m_pNode);
    m_pNode = m_pNode->GetNext();
    return (m_pNode ? (lmInstrument *)m_pNode->GetData() : (lmInstrument *)m_pNode);
}

lmInstrument* lmScore::GetLastInstrument()
{
    wxASSERT(m_pNode);
    m_pNode = m_cInstruments.GetLast();
    return (m_pNode ? (lmInstrument *)m_pNode->GetData() : (lmInstrument *)m_pNode);
}

void lmScore::WriteToFile(wxString sFilename, wxString sContent)
{
    if (sFilename == _T("")) return;

    wxFile oFile;     //open for writing, delete any previous content
    oFile.Create(sFilename, true);
    oFile.Open(sFilename, wxFile::write);
    if (!oFile.IsOpened()) {
        wxLogMessage(_T("[lmScore::WriteToFile] File '%s' could not be openned. Write to file cancelled"),
            sFilename.c_str());
    }
    else {
        oFile.Write(sContent);
        oFile.Close();
    }

}

wxString lmScore::Dump(wxString sFilename)
{
    //dump global VStaff
    wxString sDump = wxString::Format(_T("Score ID: %d\nGlobal objects:\n"), GetID());

    //loop to dump all instruments
    sDump += _T("\nLocal objects:\n");
    lmInstrument *pInstr = GetFirstInstrument();
    for (int i=1; i<= (int)m_cInstruments.GetCount(); i++, pInstr = GetNextInstrument())
    {
        sDump += wxString::Format(_T("\nInstrument %d\n"), i );
        sDump += pInstr->Dump();
    }

    //write to file, if requested
    WriteToFile(sFilename, sDump);

    return sDump;
}

wxString lmScore::SourceLDP(wxString sFilename)
{
    wxString sSource = wxString::Format(_T("//Score ID: %d\n\n"), m_nID);
    sSource += _T("(score\n   (vers 1.5)(language en iso-8859-1)\n");

    //loop for each instrument
    lmInstrument *pInstr = GetFirstInstrument();
    for (int i=1; i<= (int)m_cInstruments.GetCount(); i++, pInstr = GetNextInstrument())
    {
        sSource += pInstr->SourceLDP(1);
    }
    sSource += _T(")");

    //write to file, if requested
    WriteToFile(sFilename, sSource);

    return sSource;
}

wxString lmScore::SourceXML(wxString sFilename)
{
    wxString sSource =
		       _T("<?xml version='1.0' encoding='UTF-8' standalone='no'?>\n");
	sSource += _T("<!DOCTYPE score-partwise PUBLIC '-//Recordare//DTD MusicXML 2.0 Partwise//EN'\n");
	sSource += _T("   'http://www.musicxml.org/dtds/partwise.dtd'>\n");
	sSource += _T("<score-partwise version='2.0'>\n");

	//part-list
	int nIndent = 1;
    sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
	sSource += _T("<part-list>\n");

	nIndent++;
    lmInstrument* pInstr = GetFirstInstrument();
    for (int i=1; i<= (int)m_cInstruments.GetCount(); i++, pInstr = GetNextInstrument())
    {
		sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
		sSource += wxString::Format(_T("<score-part id='P%d'>\n"), i);
		sSource.append((nIndent+1) * lmXML_INDENT_STEP, _T(' '));
		sSource += _T("<part-name>"); 
        sSource += pInstr->GetInstrName();
		sSource += _T("</part-name>\n"); 
		sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
		sSource += _T("</score-part>\n");
    }
	nIndent--;
	sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
	sSource += _T("</part-list>\n\n");


	//Loop to create each instrument xml content
    pInstr = GetFirstInstrument();
    for (int i=1; i<= (int)m_cInstruments.GetCount(); i++, pInstr = GetNextInstrument())
    {
		sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
		sSource += wxString::Format(_T("<part id='P%d'>\n"), i);
        sSource += pInstr->SourceXML(nIndent+1);
		sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
		sSource += _T("</part>\n\n");
    }
	sSource += _T("</score-partwise>\n");

    //write to file, if requested
    WriteToFile(sFilename, sSource);

    return sSource;

}

void lmScore::Play(bool fVisualTracking, bool fMarcarCompasPrevio, EPlayMode nPlayMode,
                 long nMM, wxWindow* pWindow)
{
    if (!m_pSoundMngr) {
        m_pSoundMngr = new lmSoundManager();
        ComputeMidiEvents();
    }

    m_pSoundMngr->Play(fVisualTracking, fMarcarCompasPrevio, nPlayMode, nMM, pWindow);

}

void lmScore::PlayMeasure(int nMeasure, bool fVisualTracking, EPlayMode nPlayMode,
                          long nMM, wxWindow* pWindow)
{
    if (!m_pSoundMngr) {
        m_pSoundMngr = new lmSoundManager();
        ComputeMidiEvents();
    }

    m_pSoundMngr->PlayMeasure(nMeasure, fVisualTracking, nPlayMode, nMM, pWindow);
}

void lmScore::Pause()
{
    if (!m_pSoundMngr) return;
    m_pSoundMngr->Pause();

}

void lmScore::Stop()
{
    if (!m_pSoundMngr) return;
    m_pSoundMngr->Stop();

}

void lmScore::WaitForTermination()
{
    if (!m_pSoundMngr) return;
    m_pSoundMngr->WaitForTermination();

}

void lmScore::ScoreHighlight(lmStaffObj* pSO, lmPaper* pPaper, EHighlightType nHighlightType)
{
    switch (nHighlightType) {
        case eVisualOn:
            m_cHighlighted.Append(pSO);
            pSO->Highlight(pPaper, g_pColors->ScoreHighlight());
            break;

        case eVisualOff:
            m_cHighlighted.DeleteObject(pSO);
            RemoveHighlight(pSO, pPaper);
            break;

        case eRemoveAllHighlight:
			//This case value is impossible. It won't reach this method
            wxASSERT(false);

        default:
			//no more cases defined!
            wxASSERT(false);
    }

}

void lmScore::RemoveAllHighlight(wxWindow* pCanvas)
{
    //remove highlight from all staffobjs in m_cHighlighted list
    wxStaffObjsListNode* pNode;
    for(pNode = m_cHighlighted.GetFirst(); pNode; pNode = pNode->GetNext())
	{
		lmStaffObj* pSO = (lmStaffObj*)pNode->GetData();
        lmScoreHighlightEvent event(pSO, eVisualOff);
        ::wxPostEvent(pCanvas, event);
    }
}

void lmScore::RemoveHighlight(lmStaffObj* pSO, lmPaper* pPaper)
{
    //TODO
    // If we paint in black it remains a red aureole around
    // the note. By painting it first in white the size of the aureole
    // is smaller but still visible. A posible better solution is to
    // modify Render method to accept an additional parameter: a flag
    // to signal that XOR draw mode in RED followed by a normal
    // draw in BLACK must be done.

    pSO->Highlight(pPaper, *wxWHITE);
    pSO->Highlight(pPaper, g_pColors->ScoreNormal());
}

void lmScore::ComputeMidiEvents()
{
    int nChannel, nInstr;        //MIDI info. for instrument in process
    lmSoundManager* pSM;

    if (m_pSoundMngr)
        m_pSoundMngr->DeleteEventsTable();
    else
        m_pSoundMngr = new lmSoundManager();

    //Loop to generate Midi events for each instrument
    lmVStaff* pVStaff;
     lmInstrument *pInstr = GetFirstInstrument();
    for (int i=1; i<= (int)m_cInstruments.GetCount(); i++, pInstr = GetNextInstrument())
    {
        nChannel = pInstr->GetMIDIChannel();
        nInstr = pInstr->GetMIDIInstrument();

       //for each lmVStaff
        for (int iVStaff=1; iVStaff <= pInstr->GetNumStaves(); iVStaff++) {
            pVStaff = pInstr->GetVStaff(iVStaff);
            pSM = pVStaff->ComputeMidiEvents(nChannel);
            m_pSoundMngr->Append(pSM);
            delete pSM;
        }

        //Add an event to program sound for this instrument
        m_pSoundMngr->StoreEvent(0, eSET_ProgInstr, nChannel, nInstr, 0, 0, (lmStaffObj*)NULL, 0);

    }

    //End up Midi events table and sort it by time
    m_pSoundMngr->CloseTable();

}

wxString lmScore::DumpMidiEvents(wxString sFilename)
{
    if (!m_pSoundMngr) ComputeMidiEvents();
    wxString sDump = m_pSoundMngr->DumpMidiEvents();
    WriteToFile(sFilename, sDump);
    return sDump;
}

