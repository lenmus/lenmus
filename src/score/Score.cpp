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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
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
#include <wx/debug.h>
#include <wx/datetime.h>

#include <algorithm>

#include "Score.h"
#include "VStaff.h"
#include "UndoRedo.h"
#include "InstrGroup.h"
#include "../app/global.h"
#include "../app/Page.h"
#include "../sound/SoundEvents.h"
#include "../graphic/Formatter4.h"
#include "../graphic/GMObject.h"
#include "../graphic/Shapes.h"
#include "../app/ScoreView.h"
#include "../graphic/BoxSystem.h"
#include "../graphic/BoxScore.h"


// global unique variables used during score building
lmNoteRest*    g_pLastNoteRest;
lmBeam*        g_pCurBeam;            // lmBeam object that is being built ot NULL if none in process. See lmNote constructor

// access to colors
#include "../globals/Colors.h"
extern lmColors* g_pColors;

//to give a unique ID to each score
static long m_nCounterID = 0;

//=======================================================================================
// lmScoreCursor implementation
//=======================================================================================

lmScoreCursor::lmScoreCursor(lmScore* pOwnerScore)
{
    m_pScore = pOwnerScore;
    m_pView = (lmScoreView*)NULL;
	m_pVCursor = (lmVStaffCursor*)NULL;
	m_nCursorInstr = 0;		//no cursor
}

void lmScoreCursor::AttachCursor(lmScoreView* pView)
{
    m_pView = pView;
}

void lmScoreCursor::DetachCursor()
{
    m_pView = (lmScoreView*)NULL;
}

void lmScoreCursor::ResetCursor()
{
	if (!(m_pScore->m_cInstruments).empty())
	{
        std::vector<lmInstrument*>::iterator it = (m_pScore->m_cInstruments).begin();
        for (; it != (m_pScore->m_cInstruments).end(); ++it)
        {
            (*it)->ResetCursor();
        }
        SelectCursorFromInstr(1);
	}
	else
	{
		m_pVCursor = (lmVStaffCursor*)NULL;
		m_nCursorInstr = 0;		//no cursor
	}
}

lmStaffObj* lmScoreCursor::GetCursorSO()
{
	if (m_pVCursor)
		return m_pVCursor->GetStaffObj();
	else
		return (lmStaffObj*)NULL;
}

int lmScoreCursor::GetPageNumber()
{
	if (m_pVCursor)
		return m_pVCursor->GetPageNumber();
	else
		return 0;
}

void lmScoreCursor::SelectCursorFromInstr(int nInstr)
{
    lmInstrument* pInstr;

    //inform previous VCursor that it is not longer used
    if (m_nCursorInstr > 0)
    {
        pInstr = m_pScore->m_cInstruments[m_nCursorInstr-1];
	    pInstr->DetachCursor();
    }

    //inform new VCursor that it is now in use
    m_nCursorInstr = nInstr;
    pInstr = m_pScore->m_cInstruments[m_nCursorInstr-1];
	m_pVCursor = pInstr->AttachCursor(this);
    m_pVCursor->ResetCursor();
}

void lmScoreCursor::MoveRight(bool fNextObject)
{
    if (!m_pVCursor) return;

    if (!m_pVCursor->IsAtEnd())
        m_pVCursor->MoveRight(fNextObject);
    else
    {
		//if current instrument has more staves, advance to next staff.
		//else to first staff of next instrument
		lmInstrument* pInstr = m_pScore->m_cInstruments[m_nCursorInstr-1];
		int nStaff = m_pVCursor->GetNumStaff();
		if (nStaff < pInstr->GetNumStaves())
		{
			//advance to next staff of current instrument
			m_pVCursor->MoveToFirst(++nStaff);
		}
		else if ((int)((m_pScore->m_cInstruments).size()) > m_nCursorInstr)
		{
			//advance to first staff of next instrument
            SelectCursorFromInstr(m_nCursorInstr+1);
		}
        else
        {
            //no more instruments. Remain at the end
        }
    }
}

void lmScoreCursor::MoveLeft(bool fPrevObject)
{
    if (!m_pVCursor) return;

    if (!m_pVCursor->IsAtBeginning())
        m_pVCursor->MoveLeft(fPrevObject);
    else
    {
        //go back to last staff of prev instrument
        //TODO
    }
}

void lmScoreCursor::MoveFirst()
{
    if (!m_pVCursor) return;
    m_pVCursor->MoveToFirst(1);		//move to first object in staff 1
}

void lmScoreCursor::MoveUp()
{
	//Move up to previous staff in current system (or to last staff in previous system if
	//we are in first staff of current system), at aproximately the same horizontal paper
	//position

    if (!m_pVCursor) return;

	//get current paper position and current staff
    lmUPoint uPos = m_pVCursor->GetCursorPoint();
	int nStaff = m_pVCursor->GetNumStaff();
	int nMeasure = m_pVCursor->GetSegment();

	//if current instrument has previous staves, keep instrument and decrement staff
	if (nStaff > 1)
	{
		m_pVCursor->MoveToSegment(nMeasure, --nStaff, uPos);
		return;
	}

	//else if this is not the first instrument, take last staff of prev. instrument
	else if (m_nCursorInstr > 1)
	{
		int nInstr = m_nCursorInstr - 1;
		nStaff = m_pScore->GetInstrument(nInstr)->GetNumStaves();
		SelectCursorFromInstr(nInstr);
		m_pVCursor->MoveToSegment(nMeasure, nStaff, uPos);
		return;
	}

	//else if this is not the first system move up to last staff on previous system
	lmStaffObj* pCursorSO = m_pVCursor->GetStaffObj();
	if (pCursorSO)
	{
		lmShape* pShape2 = pCursorSO->GetShape();
		if (pShape2)
		{
			lmBoxSystem* pSystem = pShape2->GetOwnerSystem();
			int nSystem = pSystem->GetSystemNumber();
			lmBoxScore* pBScore = pSystem->GetBoxScore();
			if (nSystem > 1)
			{
				pSystem = pBScore->GetSystem(--nSystem);
				nMeasure = pSystem->GetNumMeasureAt(uPos.x);
				if (nMeasure > 0)
				{
					int nInstr = m_pScore->GetNumInstruments();
					nStaff = m_pScore->GetInstrument(nInstr)->GetNumStaves();
					SelectCursorFromInstr(nInstr);
					m_pVCursor->MoveToSegment(nMeasure-1, nStaff, uPos);
					return;
				}
			}
		}
	}

	//else, remain at current position
}

void lmScoreCursor::MoveDown()
{
	//Move to next staff in current system (or to first staff in next system if we are in
	//last staff of current system), at aproximately the same horizontal paper position

    if (!m_pVCursor) return;

	//get current paper position and current staff
    lmUPoint uPos = m_pVCursor->GetCursorPoint();
	int nStaff = m_pVCursor->GetNumStaff();
	int nMeasure = m_pVCursor->GetSegment();

	//if current instrument has more staves, keep instrument and increment staff
	lmInstrument* pInstr = m_pScore->GetInstrument(m_nCursorInstr);
	if (pInstr->GetNumStaves() > nStaff)
	{
		m_pVCursor->MoveToSegment(nMeasure, ++nStaff, uPos);
		return;
	}

	//else if there are more instrument, take first staff of next instrument
	else if (m_nCursorInstr < m_pScore->GetNumInstruments())
	{
		SelectCursorFromInstr(m_nCursorInstr + 1);
		m_pVCursor->MoveToSegment(nMeasure, 1, uPos);
		return;
	}

	//else if there are more systems move to first staff on next system
	lmStaffObj* pCursorSO = m_pVCursor->GetStaffObj();
	if (pCursorSO)
	{
		lmShape* pShape2 = pCursorSO->GetShape();
		if (pShape2)
		{
			lmBoxSystem* pSystem = pShape2->GetOwnerSystem();
			int nSystem = pSystem->GetSystemNumber();
			lmBoxScore* pBScore = pSystem->GetBoxScore();
			if (nSystem < pBScore->GetNumSystems())
			{
				pSystem = pBScore->GetSystem(++nSystem);
				nMeasure = pSystem->GetNumMeasureAt(uPos.x);
				if (nMeasure > 0)
				{
					SelectCursorFromInstr(1);
					m_pVCursor->MoveToSegment(nMeasure, 1, uPos);
					return;
				}
			}
		}
	}

	//else, remain at current position
}

void lmScoreCursor::MoveNearTo(lmUPoint uPos, lmVStaff* pVStaff, int nStaff, int nMeasure)
{
	if ((m_pScore->m_cInstruments).empty()) return;

	//get instrument
	lmInstrument* pInstr = pVStaff->GetOwnerInstrument();

	//Find instrument number
    std::vector<lmInstrument*>::iterator it = (m_pScore->m_cInstruments).begin();
    int i;
	for (i=1; it != (m_pScore->m_cInstruments).end(); ++it, i++)
    {
        if ((*it) == pInstr) break;
    }

	//get cursor and position it at required segment and position
    SelectCursorFromInstr(i);
	m_pVCursor->MoveToSegment(nMeasure - 1, nStaff, uPos);
}

void lmScoreCursor::MoveCursorToObject(lmStaffObj* pSO)
{
    if (!m_pVCursor) return;

    //get instrument
	lmInstrument* pInstr = pSO->GetVStaff()->GetOwnerInstrument();   //get instrument

	//Find instrument number
    std::vector<lmInstrument*>::iterator it = (m_pScore->m_cInstruments).begin();
    int i;
	for (i=1; it != (m_pScore->m_cInstruments).end(); ++it, i++)
    {
        if ((*it) == pInstr) break;
    }

	//get cursor for that instrument
    SelectCursorFromInstr(i);

	//position it at required staffobj
    m_pVCursor->MoveCursorToObject(pSO);
}

lmUPoint lmScoreCursor::GetCursorPoint()
{
    if (m_pVCursor)
        return m_pVCursor->GetCursorPoint();
    else
        return lmUPoint(0.0f, 0.0f);
}

lmStaff* lmScoreCursor::GetCursorStaff()
{
    if (m_pVCursor)
        return m_pVCursor->GetCursorStaff();
    else
        return (lmStaff*)NULL;
}

int lmScoreCursor::GetCursorNumStaff()
{
    if (m_pVCursor)
        return m_pVCursor->GetNumStaff();
    else
        return 1;
}

float lmScoreCursor::GetCursorTime()
{
    if (m_pVCursor)
        return m_pVCursor->GetTimepos();
    else
        return 0.0f;;
}

lmVStaff* lmScoreCursor::GetVStaff() 
{ 
    if (m_pVCursor)
        return m_pScore->GetInstrument(m_nCursorInstr)->GetVStaff();
    else
        return (lmVStaff*)NULL;
}

void lmScoreCursor::SelectCursor(lmVStaffCursor* pVCursor)
{
    //Replace current cursor by the one received as parameter.
    //PRECONDITION: The received cursor must be one of the active cursors in current 
    //              instruments
    //It is assumed that the Score and View don't change.

    m_pVCursor = pVCursor;

	//Find instrument
    std::vector<lmInstrument*>::iterator it = (m_pScore->m_cInstruments).begin();
    int nInstr;
	for (nInstr=1; it != (m_pScore->m_cInstruments).end(); ++it, nInstr++)
    {
        if ((*it)->GetVCursor() == m_pVCursor) break;
    }
    wxASSERT(it != (m_pScore->m_cInstruments).end());

    //get instrument number
	m_nCursorInstr = nInstr;
}

void lmScoreCursor::SetNewCursorState(lmVCursorState* pState)
{
    m_pVCursor->SetNewCursorState(this, pState);
}



//=======================================================================================
// lmScore implementation
//=======================================================================================

lmScore::lmScore() : lmScoreObj((lmScoreObj*)NULL), m_SCursor(this)
{
    //Set up an empty score, that is, without any lmInstrument.

    m_nID = ++m_nCounterID;

    //initializations
    m_pSoundMngr = (lmSoundManager*)NULL;
    m_sScoreName = _T("New score");
    m_pTenthsConverter = (lmVStaff*)NULL;

    //paper size and margins
    m_pPageInfo = (lmPageInfo*)NULL;

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
    if (m_pSoundMngr && m_pSoundMngr->IsPlaying())
    {
        this->Stop();
        while (m_pSoundMngr->IsPlaying())
            ::wxMilliSleep(100);    
    }
	for(int i=0; i < (int)m_cInstruments.size(); i++)
	{
		delete m_cInstruments[i];
	}
    m_cInstruments.clear();

    if (m_pSoundMngr) {
        m_pSoundMngr->DeleteEventsTable();
        delete m_pSoundMngr;
    }

    //Staffobjs must not be deleted, only the list
	m_cHighlighted.clear();

    //delete list of title indexes
    m_nTitles.clear();

    //delete other objects
    if (m_pPageInfo) delete m_pPageInfo;
}

void lmScore::SetPageInfo(lmPageInfo* pPageInfo)
{
    //Set default page information

    if (m_pPageInfo) delete m_pPageInfo;
    m_pPageInfo = pPageInfo;
}

lmLUnits lmScore::TenthsToLogical(lmTenths nTenths)
{
    //For all AuxObjs attached to the score 'tenths' will be 'tenths of millimeter'. This
    //is equivalent to using a reference staff whose lines are spaced 1 mm.
    lmLUnits uSpacing = lmToLogicalUnits(1.0f, lmMILLIMETERS);   //staff lines spacing: 1mm
    return (uSpacing * nTenths)/10.0f;
}

lmTenths lmScore::LogicalToTenths(lmLUnits uUnits)
{
    //For all AuxObjs attached to the score 'tenths' will be 'tenths of millimeter'. This
    //is equivalent to using a reference staff whose lines are spaced 1 mm.
    lmLUnits uSpacing = lmToLogicalUnits(1.0f, lmMILLIMETERS);   //staff lines spacing: 1mm
    return (10.0f * uUnits)/uSpacing;
}

lmScoreText* lmScore::AddTitle(wxString sTitle, lmEAlignment nAlign, lmLocation tPos,
                       wxString sFontName, int nFontSize, lmETextStyle nStyle)
{
    lmFontInfo tFont;
    tFont.nFontSize = nFontSize;
    tFont.nStyle = nStyle;
    tFont.sFontName = sFontName;

    lmScoreText* pTitle = new lmScoreText(sTitle, nAlign, tPos, tFont, true);  //true -> is title
    m_nTitles.push_back( AttachAuxObj(pTitle) );
    return pTitle;
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
    //LIMIT: it is being assumed that all instruments have the same number of bars
    //InstrumentsList::Node *node = m_cInstruments.GetFirst();
    //lmInstrument *pInstr = node->GetData();
    lmVStaff *pStaff = m_cInstruments[0]->GetVStaff();
    return(pStaff->GetNumMeasures());
}

lmInstrument* lmScore::AddInstrument(int nMIDIChannel, int nMIDIInstr,
                                     wxString sName, wxString sAbbrev,
                                     lmInstrGroup* pGroup)
{
    //add an lmInstrument.
    //nMIDIChannel is the MIDI channel to use for playing this instrument

    lmInstrument* pInstr = new lmInstrument(this, nMIDIChannel, nMIDIInstr, sName, sAbbrev);

	DoAddInstrument(pInstr, pGroup);
    return pInstr;
}

lmInstrument* lmScore::AddInstrument(int nMIDIChannel, int nMIDIInstr,
									 lmScoreText* pName, lmScoreText* pAbbrev,
                                     lmInstrGroup* pGroup)
{
    //add an lmInstrument.
    //nMIDIChannel is the MIDI channel to use for playing this instrument

    lmInstrument* pInstr = new lmInstrument(this, nMIDIChannel, nMIDIInstr, pName, pAbbrev);

	DoAddInstrument(pInstr, pGroup);
	return pInstr;
}

void lmScore::DoAddInstrument(lmInstrument* pInstr, lmInstrGroup* pGroup)
{
	//if this is the first intrument, initialize edit cursor
	if (m_cInstruments.empty())
        m_SCursor.ResetCursor();

    //add instrument
	m_cInstruments.push_back(pInstr);

    //include instrument in group
    if (pGroup)
        pGroup->Include(pInstr);
}

lmInstrument* lmScore::XML_FindInstrument(wxString sId)
{
    // iterate over instrument list to find the one with id == sId

	lmInstrument* pInstr = (lmInstrument*)NULL;
	for (int i=0; i < (int)m_cInstruments.size(); i++)
	{
		pInstr = m_cInstruments[i];
        if (pInstr->XML_GetId() == sId) break;
	}
    return pInstr;
}

//void lmScore::LayoutTitles(lmBox* pBox, lmPaper *pPaper)
//{
//    lmLUnits uyStartPos = pPaper->GetCursorY();		//save, to measure height
//
//    lmScoreText* pTitle;
//    lmLUnits nPrevTitleHeight = 0;
//    for (int i=0; i < (int)m_nTitles.size(); i++)
//    {
//        pTitle = (lmScoreText*)(*m_pAuxObjs)[m_nTitles[i]];
//		nPrevTitleHeight = CreateTitleShape(pBox, pPaper, pTitle, nPrevTitleHeight);
//    }
//
//	m_nHeadersHeight = pPaper->GetCursorY() - uyStartPos;
//
//}

void lmScore::LayoutTitles(lmBox* pBox, lmPaper *pPaper)
{
    //TODO: review these fixed values
    wxColour colorC = *wxBLACK;
    bool fHighlight = false;

    lmLUnits uyStartPos = pPaper->GetCursorY();		//save, to measure height
	m_uComputedPos.x = pPaper->GetCursorX();
	m_uComputedPos.y = pPaper->GetCursorY();

	// layout AuxObjs attached to directly to the score
    if (m_pAuxObjs)
    {
	    for (int i=0; i < (int)m_pAuxObjs->size(); i++)
	    {
		    //assign m_uComputedPos as paper pos. for this AuxObj
		    pPaper->SetCursorX(m_uComputedPos.x);
		    pPaper->SetCursorY(m_uComputedPos.y);

		    (*m_pAuxObjs)[i]->Layout(pBox, pPaper, colorC, fHighlight);

            //force auxObjs to take user position into account
            (*m_pAuxObjs)[i]->OnParentComputedPositionShifted(0.0f, 0.0f);
	    }
    }

    // update paper cursor position
    pPaper->SetCursorX(m_uComputedPos.x);
    m_nHeadersHeight = pPaper->GetCursorY() - uyStartPos;

}


lmLUnits lmScore::CreateTitleShape(lmBox* pBox, lmPaper *pPaper, lmScoreText* pTitle,
								   lmLUnits nPrevTitleHeight)
{
    // Creates the shape for the title and adds it to the box.
	// Returns height of title

    lmLUnits nWidth, nHeight;
	lmShapeText* pShape = (lmShapeText*)NULL;

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
        lmUPoint uPos(pPaper->GetCursorX(), pPaper->GetCursorY());
        pShape = pTitle->CreateShape(pPaper, uPos);
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
	pShape = pTitle->CreateShape(pPaper, lmUPoint(pPaper->GetCursorX(), pPaper->GetCursorY()) );

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

lmInstrument* lmScore::GetInstrument(int nInstr)
{
	m_nCurNode = nInstr - 2;
	return GetNextInstrument();
}

lmInstrument* lmScore::GetFirstInstrument()
{
	m_nCurNode = -1;
	return GetNextInstrument();
}

lmInstrument* lmScore::GetNextInstrument()
{
	m_nCurNode++;
	if (m_nCurNode < (int)m_cInstruments.size())
		return m_cInstruments[m_nCurNode];
	else
		return (lmInstrument*)NULL;

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

    //dump titles and other attached auxobjs
    if (m_pAuxObjs)
    {
        sDump += _T("\nAttached aux objects:\n");
	    for (int i=0; i < (int)m_pAuxObjs->size(); i++)
	    {
		    sDump += (*m_pAuxObjs)[i]->Dump();
	    }
        sDump += _T("\n");
    }

    //loop to dump all instruments
    sDump += _T("\nLocal objects:\n");
    for (int i=0; i < (int)m_cInstruments.size(); i++)
    {
        sDump += wxString::Format(_T("\nInstrument %d\n"), i+1 );
        sDump += m_cInstruments[i]->Dump();
    }

    //write to file, if requested
    WriteToFile(sFilename, sDump);

    return sDump;
}

wxString lmScore::SourceLDP(wxString sFilename)
{
    wxString sSource = _T("(score\n   (vers 1.5)(language en utf-8)\n");

    //add comment with info about generating program
    sSource += _T("   //LDP file generated by LenMus, version ");
    sSource += wxGetApp().GetVersionNumber();
	sSource += _T(". Date: ");
	sSource += (wxDateTime::Now()).Format(_T("%Y-%m-%d"));
    sSource += _T("\n");

    //titles and other attached auxobjs
    if (m_pAuxObjs)
    {
	    for (int i=0; i < (int)m_pAuxObjs->size(); i++)
	    {
		    sSource += (*m_pAuxObjs)[i]->SourceLDP(1);
	    }
    }

    //loop for each instrument
    for (int i=0; i < (int)m_cInstruments.size(); i++)
    {
        sSource += m_cInstruments[i]->SourceLDP(1);
    }
    sSource += _T(")");

    //write to file, if requested
    WriteToFile(sFilename, sSource);

    return sSource;
}

wxString lmScore::SourceXML(wxString sFilename)
{
	int nIndent = 1;
	wxString sIndent = _T("");
    sIndent.append(nIndent * lmXML_INDENT_STEP, _T(' '));

    wxString sSource =
			   _T("<?xml version='1.0' encoding='UTF-8' standalone='no'?>\n");
	sSource += _T("<!--MusicXML file created by GPL free program LenMus, version ");
    sSource += wxGetApp().GetVersionNumber();
    sSource += _T(" 'http://www.lenmus.org' -->\n");
	sSource += _T("<!DOCTYPE score-partwise PUBLIC '-//Recordare//DTD MusicXML 1.1 Partwise//EN' 'http://www.musicxml.org/dtds/partwise.dtd'>\n");
	sSource += _T("<score-partwise version='1.1'>\n");

	//identification
	sSource += sIndent;
	sSource += _T("<identification>\n");
	sSource += sIndent;
	sSource += sIndent;
	sSource += _T("<encoding>\n");
	sSource += sIndent;
	sSource += sIndent;
	sSource += sIndent;
	sSource += _T("<encoding-date>");
	sSource += (wxDateTime::Now()).Format(_T("%Y-%m-%d"));
	sSource += _T("</encoding-date>\n");
	sSource += sIndent;
	sSource += sIndent;
	sSource += sIndent;
	sSource += _T("<software>LenMus, version ");
    sSource += wxGetApp().GetVersionNumber();
	sSource += _T("</software>\n");
	sSource += sIndent;
	sSource += sIndent;
	sSource += sIndent;
	sSource += _T("<encoding-description>Options: FinaleDolet33=1, ChordCaseMatters=1, ExportToSibelius=0</encoding-description>\n");
	sSource += sIndent;
	sSource += sIndent;
	sSource += _T("</encoding>\n");
	sSource += sIndent;
	sSource += _T("</identification>\n\n");

	//defaults
	sSource += sIndent;
	sSource += _T("<defaults>\n");
	sSource += sIndent;
	sSource += sIndent;
	sSource += _T("<scaling>\n");
	sSource += sIndent;
	sSource += sIndent;
	sSource += sIndent;
	sSource += _T("<millimeters>1.8</millimeters>\n");
	sSource += sIndent;
	sSource += sIndent;
	sSource += sIndent;
	sSource += _T("<tenths>10.0</tenths>\n");
	sSource += sIndent;
	sSource += sIndent;
	sSource += _T("</scaling>\n");

	sSource += sIndent;
	sSource += sIndent;
	sSource += _T("<page-layout>\n");
	sSource += sIndent;
	sSource += sIndent;
	sSource += sIndent;
	sSource += _T("<page-height>1650</page-height>\n");
	sSource += sIndent;
	sSource += sIndent;
	sSource += sIndent;
	sSource += _T("<page-width>1166</page-width>\n");
	sSource += sIndent;
	sSource += sIndent;
	sSource += sIndent;
	sSource += _T("<page-margins type='both'>\n");
	sSource += sIndent;
	sSource += sIndent;
	sSource += sIndent;
	sSource += sIndent;
	sSource += _T("<left-margin>111</left-margin>\n");
	sSource += sIndent;
	sSource += sIndent;
	sSource += sIndent;
	sSource += sIndent;
	sSource += _T("<right-margin>111</right-margin>\n");
	sSource += sIndent;
	sSource += sIndent;
	sSource += sIndent;
	sSource += sIndent;
	sSource += _T("<top-margin>111</top-margin>\n");
	sSource += sIndent;
	sSource += sIndent;
	sSource += sIndent;
	sSource += sIndent;
	sSource += _T("<bottom-margin>111</bottom-margin>\n");
	sSource += sIndent;
	sSource += sIndent;
	sSource += sIndent;
	sSource += _T("</page-margins>\n");
	sSource += sIndent;
	sSource += sIndent;
	sSource += _T("</page-layout>\n");

	sSource += sIndent;
	sSource += sIndent;
	sSource += _T("<system-layout>\n");
	sSource += sIndent;
	sSource += sIndent;
	sSource += sIndent;
	sSource += _T("<system-margins>\n");
	sSource += sIndent;
	sSource += sIndent;
	sSource += sIndent;
	sSource += sIndent;
	sSource += _T("<left-margin>137</left-margin>\n");
	sSource += sIndent;
	sSource += sIndent;
	sSource += sIndent;
	sSource += sIndent;
	sSource += _T("<right-margin>0</right-margin>\n");
	sSource += sIndent;
	sSource += sIndent;
	sSource += sIndent;
	sSource += _T("</system-margins>\n");
	sSource += sIndent;
	sSource += sIndent;
	sSource += sIndent;
	sSource += _T("<system-distance>0</system-distance>\n");
	sSource += sIndent;
	sSource += sIndent;
	sSource += _T("	</system-layout>\n");
	sSource += sIndent;
	sSource += _T("</defaults>\n\n");

	//credits
	sSource += sIndent;
	sSource += _T("<credit>\n");
	sSource += sIndent;
	sSource += sIndent;
	sSource += _T("<credit-words></credit-words>\n");
	sSource += sIndent;
	sSource += _T("</credit>\n\n");

	//part-list
	sSource += sIndent;
	sSource += _T("<part-list>\n");

	nIndent++;
    for (int i=0; i < (int)m_cInstruments.size(); i++)
    {
		sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
		sSource += wxString::Format(_T("<score-part id='P%d'>\n"), i);
		sSource.append((nIndent+1) * lmXML_INDENT_STEP, _T(' '));
		sSource += _T("<part-name>"); 
        sSource += m_cInstruments[i]->GetInstrName();
		sSource += _T("</part-name>\n"); 
		sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
		sSource += _T("</score-part>\n");
    }
	nIndent--;
	sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
	sSource += _T("</part-list>\n\n");


	//Loop to create each instrument xml content
    for (int i=0; i < (int)m_cInstruments.size(); i++)
    {
		sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
		sSource += wxString::Format(_T("<part id='P%d'>\n"), i);
        sSource += m_cInstruments[i]->SourceXML(nIndent+1);
		sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
		sSource += _T("</part>\n\n");
    }
	sSource += _T("</score-partwise>\n");

    //write to file, if requested
    WriteToFile(sFilename, sSource);

    return sSource;

}

void lmScore::Play(bool fVisualTracking, bool fMarcarCompasPrevio, lmEPlayMode nPlayMode,
                 long nMM, wxWindow* pWindow)
{
    if (!m_pSoundMngr) {
        m_pSoundMngr = new lmSoundManager(this);
        ComputeMidiEvents();
    }

    m_pSoundMngr->Play(fVisualTracking, fMarcarCompasPrevio, nPlayMode, nMM, pWindow);

}

void lmScore::PlayMeasure(int nMeasure, bool fVisualTracking, lmEPlayMode nPlayMode,
                          long nMM, wxWindow* pWindow)
{
    if (!m_pSoundMngr) {
        m_pSoundMngr = new lmSoundManager(this);
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

void lmScore::ScoreHighlight(lmStaffObj* pSO, lmPaper* pPaper, lmEHighlightType nHighlightType)
{
    switch (nHighlightType) {
        case eVisualOn:
            m_cHighlighted.push_back(pSO);
            pSO->PlaybackHighlight(pPaper, g_pColors->ScoreHighlight());
            break;

        case eVisualOff:
			m_cHighlighted.erase( std::find(m_cHighlighted.begin(), m_cHighlighted.end(), pSO) );
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

	std::list<lmStaffObj*>::iterator pItem;
	for (pItem = m_cHighlighted.begin(); pItem != m_cHighlighted.end(); pItem++)
	{
        lmScoreHighlightEvent event(this->GetID(), *pItem, eVisualOff);
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

    pSO->PlaybackHighlight(pPaper, *wxWHITE);
    pSO->PlaybackHighlight(pPaper, g_pColors->ScoreNormal());
}

void lmScore::CursorHighlight(lmStaffObj* pSO, int nStaff, lmPaper* pPaper, bool fHighlight)
{
    pSO->CursorHighlight(pPaper, nStaff, fHighlight);
}

void lmScore::ComputeMidiEvents()
{
    int nChannel, nInstr;        //MIDI info. for instrument in process
    lmSoundManager* pSM;

    if (m_pSoundMngr)
        m_pSoundMngr->DeleteEventsTable();
    else
        m_pSoundMngr = new lmSoundManager(this);

    //Loop to generate Midi events for each instrument
    for (int i=0; i < (int)m_cInstruments.size(); i++)
    {
		lmInstrument* pInstr = m_cInstruments[i];
        nChannel = pInstr->GetMIDIChannel();
        nInstr = pInstr->GetMIDIInstrument();

        lmVStaff* pVStaff = pInstr->GetVStaff();
        pSM = pVStaff->ComputeMidiEvents(nChannel);
        m_pSoundMngr->Append(pSM);
        delete pSM;

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

void lmScore::SetMeasureModified(int nMeasure, bool fModified)
{
	if (fModified)
	{
		//Add the element to the list, if it not yet included
		std::list<int>::iterator itWhere =
			std::find(m_aMeasureModified.begin(), m_aMeasureModified.end(), nMeasure);
		if (itWhere == m_aMeasureModified.end() ) 
		{
			//not found. Insert it
			m_aMeasureModified.push_back(nMeasure);
		}
		else
			return;		//already included
	}
	else
	{
		//Remove the element from the list
		std::list<int>::iterator itWhere =
			std::find(m_aMeasureModified.begin(), m_aMeasureModified.end(), nMeasure);
		if (itWhere != m_aMeasureModified.end() ) 
			m_aMeasureModified.erase(itWhere);
	}
}

bool lmScore::IsMeasureModified(int nMeasure)
{
	std::list<int>::iterator itWhere =
		std::find(m_aMeasureModified.begin(), m_aMeasureModified.end(), nMeasure);
	return (itWhere != m_aMeasureModified.end() ); 
}

void lmScore::ResetMeasuresModified()
{
	m_aMeasureModified.clear();
}

lmScoreCursor* lmScore::AttachCursor(lmScoreView* pView) 
{ 
    m_SCursor.AttachCursor(pView);
	m_SCursor.ResetCursor();
    return &m_SCursor;
}

void lmScore::DetachCursor() 
{ 
    m_SCursor.DetachCursor();
}

lmScoreCursor* lmScore::SetCursor(lmVStaffCursor* pVCursor)
{
    //Replace current cursor by the one received as parameter

    m_SCursor.SelectCursor(pVCursor);
    return &m_SCursor;
}

lmScoreCursor* lmScore::SetNewCursorState(lmVCursorState* pState)
{
    m_SCursor.SetNewCursorState(pState);
    return &m_SCursor;
}

//-------------------------------------------------------------------------------------

lmBoxScore* lmScore::Layout(lmPaper* pPaper)
{
    lmFormatter4 oFormatter;
    m_pGMObj = oFormatter.Layout(this, pPaper);
    return (lmBoxScore*)m_pGMObj;
}

