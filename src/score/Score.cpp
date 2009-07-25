//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
//
//    This program is free software; you can redistribute it and/or modify it under the
//    terms of the GNU General Public License as published by the Free Software Foundation,
//    either version 3 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this
//    program. If not, see <http://www.gnu.org/licenses/>.
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
#else
#include <wx/choicdlg.h>
#include <wx/file.h>
#include <wx/debug.h>
#include <wx/datetime.h>
#endif

#include <algorithm>

#include "Score.h"
#include "VStaff.h"
#include "InstrGroup.h"
#include "properties/DlgProperties.h"
#include "../app/global.h"
#include "../app/ScoreCanvas.h"
#include "../app/ArtProvider.h"
#include "../sound/SoundEvents.h"
#include "../sound/SoundManager.h"
#include "../graphic/Formatter4.h"
#include "../graphic/GMObject.h"
#include "../graphic/Shapes.h"
#include "../graphic/ShapeText.h"
#include "../app/ScoreView.h"
#include "../graphic/BoxSystem.h"
#include "../graphic/BoxScore.h"
#include "../graphic/Handlers.h"

// access to colors
#include "../globals/Colors.h"
extern lmColors* g_pColors;

//access to logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;


//to give a unique ID to each score
static long m_nScoreCounterID = 0;

//=======================================================================================
// helper class lmPageInfo implementation
//=======================================================================================

lmPageInfo::lmPageInfo(int nLeftMargin, int nRightMargin, int nTopMargin,
                       int nBottomMargin, int nBindingMargin, wxSize nPageSize,
                       bool fPortrait)
{
    //constructor: all data in milimeters
    //default paper size: DIN A4 (210.0 x 297.0 mm)

    m_uLeftMargin = lmToLogicalUnits(nLeftMargin, lmMILLIMETERS);
    m_uRightMargin = lmToLogicalUnits(nRightMargin, lmMILLIMETERS);
    m_uTopMargin = lmToLogicalUnits(nTopMargin, lmMILLIMETERS);
    m_uBottomMargin = lmToLogicalUnits(nBottomMargin, lmMILLIMETERS);
    m_uBindingMargin = lmToLogicalUnits(nBindingMargin, lmMILLIMETERS);

    SetPageSizeMillimeters(nPageSize);

    m_fPortrait = fPortrait;
    m_fNewSection = true;
}

lmPageInfo::lmPageInfo(lmPageInfo* pPageInfo)
{
    m_uLeftMargin = pPageInfo->m_uLeftMargin;
    m_uRightMargin = pPageInfo->m_uRightMargin;
    m_uTopMargin = pPageInfo->m_uTopMargin;
    m_uBottomMargin = pPageInfo->m_uBottomMargin;
    m_uBindingMargin = pPageInfo->m_uBindingMargin;

    m_uPageSize.SetHeight(pPageInfo->PageHeight());
	m_uPageSize.SetWidth(pPageInfo->PageWidth());

    m_fPortrait = pPageInfo->m_fPortrait;
    m_fNewSection = false;
}

void lmPageInfo::SetPageSizeMillimeters(wxSize nSize)
{
    m_uPageSize.SetHeight(lmToLogicalUnits(nSize.GetHeight(), lmMILLIMETERS));
    m_uPageSize.SetWidth(lmToLogicalUnits(nSize.GetWidth(), lmMILLIMETERS));
}

wxString lmPageInfo::SourceLDP(int nIndent, bool fUndoData)
{
    wxString sSource = _T("");
    sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    sSource += _T("(pageLayout ");

    //size: width, height
	sSource += wxString::Format(_T("(pageSize %.0f %.0f)"), m_uPageSize.x, m_uPageSize.y);

	//margins: left,top,right,bottom, binding
	sSource += wxString::Format(_T("(pageMargins %.0f %.0f %.0f %.0f %.0f)"),
							    m_uLeftMargin, m_uTopMargin, m_uRightMargin,
								m_uBottomMargin, m_uBindingMargin);

	//orientation
	sSource += (m_fPortrait ? _T(" portrait") : _T(" landscape"));

    //close element
    sSource += _T(")\n");
	return sSource;
}



//=======================================================================================
// helper class lmSystemInfo implementation
//=======================================================================================

lmSystemInfo::lmSystemInfo(lmLUnits uLeftMargin, lmLUnits uRightMargin,
                           lmLUnits uSystemDistance, lmLUnits uTopSystemDistance)
    : m_uLeftMargin(uLeftMargin)
    , m_uRightMargin(uRightMargin)
    , m_uSystemDistance(uSystemDistance)
    , m_uTopSystemDistance(uTopSystemDistance)
{
}

lmSystemInfo::lmSystemInfo(lmSystemInfo* pSysInfo)
{
    m_uLeftMargin = pSysInfo->m_uLeftMargin;
    m_uRightMargin = pSysInfo->m_uRightMargin;
    m_uTopSystemDistance = pSysInfo->m_uTopSystemDistance;
    m_uSystemDistance = pSysInfo->m_uSystemDistance;
}

wxString lmSystemInfo::SourceLDP(int nIndent, bool fUndoData)
{
    wxString sSource = _T("");
    sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    sSource += _T("(systemLayout ");

	//margins: left,top,right,bottom, binding
	sSource += wxString::Format(_T("(systemMargins %.0f %.0f %.0f %.0f)"),
							    m_uLeftMargin, m_uRightMargin,
								m_uSystemDistance, m_uTopSystemDistance);

    //close element
    sSource += _T(")\n");
	return sSource;
}



//=======================================================================================
// lmScore implementation
//=======================================================================================

//global variable for exporting to LDP ScoreCursor data
lmStaffObj* g_pCursorSO = (lmStaffObj*)NULL;        //not NULL for exporting cursor data


lmScore::lmScore()
    : lmScoreObj((lmScoreObj*)NULL, lmNEW_ID)
    , m_SCursor(this)
    , m_sCreationModeName(wxEmptyString)
    , m_sCreationModeVers(wxEmptyString)
    , m_nCounterID(0L)
    , m_fUndoMode(false)
{
    //Set up an empty score, that is, without any lmInstrument.

    m_nScoreID = ++m_nScoreCounterID;

    //initializations
    m_fReadOnly = false;
    m_pSoundMngr = (lmSoundManager*)NULL;
    m_sScoreName = _T("New score");
    m_pTenthsConverter = (lmVStaff*)NULL;

    //paper size and margins
    m_pPageInfo = new lmPageInfo();     //default page size and margins
    m_PagesInfo.push_back( m_pPageInfo );
    m_nNumPage = 1;

    //Default systems information (Left,Right,Top,Bottom margins)
    lmLUnits uTopSystemDistance = lmToLogicalUnits(2, lmCENTIMETERS);    //TODO user options
    lmSystemInfo* pSysInfo = new lmSystemInfo(0.0f, 0.0f, 0.0f, uTopSystemDistance);    //First system
    m_SystemsInfo.push_back(pSysInfo);
    lmLUnits uSystemDistance = lmToLogicalUnits(3, lmCENTIMETERS);
    pSysInfo = new lmSystemInfo(0.0f, 0.0f, uSystemDistance, uTopSystemDistance);    //Other systems
    m_SystemsInfo.push_back(pSysInfo);

    //default ObjOptions
    SetOption(_T("Score.FillPageWithEmptyStaves"), false);
    SetOption(_T("StaffLines.StopAtFinalBarline"), true);
    SetOption(_T("Score.JustifyFinalBarline"), false);
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

    //delete pages info
    std::list<lmPageInfo*>::iterator it;
    for (it = m_PagesInfo.begin(); it != m_PagesInfo.end(); ++it)
        delete *it;

    //delete systems information
    std::list<lmSystemInfo*>::iterator itSys;
    for (itSys = m_SystemsInfo.begin(); itSys != m_SystemsInfo.end(); ++itSys)
        delete *itSys;
}

lmLUnits lmScore::GetSystemLeftSpace(int iSystem)
{ 
    // iSystem (0..n-1)

    if (iSystem == 0)
        return m_SystemsInfo.front()->LeftMargin(); 
    else
        return m_SystemsInfo.back()->LeftMargin(); 
}

lmLUnits lmScore::GetSystemRightSpace(int iSystem)
{ 
    // iSystem (0..n-1)

    if (iSystem == 0)
        return m_SystemsInfo.front()->RightMargin(); 
    else
        return m_SystemsInfo.back()->RightMargin(); 
}

lmLUnits lmScore::GetSystemDistance(int iSystem, bool fStartOfPage)
{
    // iSystem (0..n-1)

    if (iSystem == 0)
        return m_SystemsInfo.front()->SystemDistance(fStartOfPage); 
    else
        return m_SystemsInfo.back()->SystemDistance(fStartOfPage); 
}

void lmScore::ClearPages()
{
    std::list<lmPageInfo*>::iterator it;
    for (it = m_PagesInfo.begin(); it != m_PagesInfo.end(); ++it)
        delete *it;

	//create default page size and margins for first page
    m_pPageInfo = new lmPageInfo();
    m_PagesInfo.push_back( m_pPageInfo );
    m_nNumPage = 1;
}

void lmScore::SetPageInfo(int nPage)
{
	//wxLogMessage(_T("[lmScore::SetPageInfo] nPage=%d, m_nNumPage=%d, num items=%d"),
	//			 nPage, m_nNumPage, m_PagesInfo.size() );
	if (nPage != m_nNumPage)
	{
		if (nPage > (int)m_PagesInfo.size())
		{
			//create new page, copying info from previous one
			m_pPageInfo = new lmPageInfo( m_pPageInfo );
			m_PagesInfo.push_back( m_pPageInfo );
			m_nNumPage = nPage;
			wxASSERT_MSG(nPage == (int)m_PagesInfo.size(), _T("[lmScore::SetPageInfo] Pages not in sequence!"));
		}
		else
		{
			//find page info
			int iP = 1;
			std::list<lmPageInfo*>::iterator it;
			for (it = m_PagesInfo.begin(); it != m_PagesInfo.end(); ++it, ++iP)
				if (iP == nPage) break;
			wxASSERT_MSG(it != m_PagesInfo.end(), _T("[lmScore::SetPageInfo] Page not found!"));
			m_pPageInfo = *it;
			m_nNumPage = nPage;
		}
	}
}

lmLUnits lmScore::GetPageTopMargin(int nPage)
{
	SetPageInfo(nPage);
	return m_pPageInfo->TopMargin();
}

lmLUnits lmScore::GetPageLeftMargin(int nPage)
{
	SetPageInfo(nPage);
	return m_pPageInfo->LeftMargin(m_nNumPage);
}

lmLUnits lmScore::GetPageRightMargin(int nPage)
{
	SetPageInfo(nPage);
	return m_pPageInfo->RightMargin(m_nNumPage);
}

lmUSize lmScore::GetPaperSize(int nPage)
{
	SetPageInfo(nPage);
	return lmUSize(m_pPageInfo->PageWidth(), m_pPageInfo->PageHeight());
}

lmLUnits lmScore::GetMaximumY(int nPage)
{
	SetPageInfo(nPage);
	return m_pPageInfo->GetUsableHeight() + m_pPageInfo->TopMargin();
}

lmLUnits lmScore::GetLeftMarginXPos(int nPage)
{
	SetPageInfo(nPage);
    return GetPageLeftMargin();
}

lmLUnits lmScore::GetRightMarginXPos(int nPage)
{
	SetPageInfo(nPage);
    return GetPaperSize().GetWidth() - GetPageRightMargin();
}

void lmScore::SetPageTopMargin(lmLUnits uValue, int nPage)
{
	SetPageInfo(nPage);
	m_pPageInfo->SetTopMargin(uValue);
}

void lmScore::SetPageLeftMargin(lmLUnits uValue, int nPage)
{
	SetPageInfo(nPage);
	m_pPageInfo->SetLeftMargin(uValue);
}

void lmScore::SetPageRightMargin(lmLUnits uValue, int nPage)
{
	SetPageInfo(nPage);
	m_pPageInfo->SetRightMargin(uValue);
}

void lmScore::SetPageBottomMargin(lmLUnits uValue, int nPage)
{
	SetPageInfo(nPage);
	m_pPageInfo->SetBottomMargin(uValue);
}

void lmScore::SetPageBindingMargin(lmLUnits uValue, int nPage)
{
	SetPageInfo(nPage);
	m_pPageInfo->SetBindingMargin(uValue);
}

void lmScore::SetPageSize(lmLUnits uWidth, lmLUnits uHeight, int nPage)
{
	SetPageInfo(nPage);
	m_pPageInfo->SetPageSize(uWidth, uHeight);
}

void lmScore::SetPageOrientation(bool fPortrait, int nPage)
{
	SetPageInfo(nPage);
	m_pPageInfo->SetOrientation(fPortrait);
}

void lmScore::SetPageNewSection(bool fNewSection, int nPage)
{
	SetPageInfo(nPage);
	m_pPageInfo->SetNewSection(fNewSection);
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

lmScoreTitle* lmScore::AddTitle(wxString sTitle, lmEHAlign nHAlign, lmTextStyle* pStyle,
                                long nID)
{
    lmScoreTitle* pTitle =
        new lmScoreTitle(this, nID, sTitle, lmBLOCK_ALIGN_BOTH, nHAlign, lmVALIGN_TOP, pStyle);

    m_nTitles.push_back( AttachAuxObj(pTitle) );
    return pTitle;
}

long lmScore::AssignID(lmScoreObj* pSO)
{ 
    if (m_fUndoMode)
    {
        if (pSO->GetID() == 0)
        {
            wxLogMessage(_T("[lmScore::AssignID] UndoMode: an object does not have a saved ID"));
            pSO->SetID(++m_nCounterID);
        }
        else
            m_nCounterID = pSO->GetID();
    }
    else
    {
        if (pSO->GetID() != 0)
        {
            wxLogMessage(_T("[lmScore::AssignID] Normal mode: an object has already an ID"));
            m_nCounterID = pSO->GetID();
        }
        else
            pSO->SetID(++m_nCounterID);
    }
    m_ScoreObjs[m_nCounterID] = pSO;
    return m_nCounterID;
}

lmScoreObj* lmScore::GetScoreObj(long nID)
{
    if (nID == lmNULL_ID)
        return (lmScoreObj*)NULL;
    else
        return m_ScoreObjs[nID];
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
                                     wxString sName, wxString sAbbrev, long nID,
                                     long nVStaffID, lmInstrGroup* pGroup)
{
    //add an lmInstrument.
    //nMIDIChannel is the MIDI channel to use for playing this instrument

    lmInstrument* pInstr = new lmInstrument(this, nID, nVStaffID, nMIDIChannel, nMIDIInstr,
                                            sName, sAbbrev);

	DoAddInstrument(pInstr, pGroup);
    return pInstr;
}

lmInstrument* lmScore::AddInstrument(int nMIDIChannel, int nMIDIInstr,
									 lmInstrNameAbbrev* pName, lmInstrNameAbbrev* pAbbrev,
                                     long nID, long nVStaffID, lmInstrGroup* pGroup)
{
    //add an lmInstrument.
    //nMIDIChannel is the MIDI channel to use for playing this instrument

    lmInstrument* pInstr = new lmInstrument(this, nID, nVStaffID, nMIDIChannel, nMIDIInstr,
                                            pName, pAbbrev);

	DoAddInstrument(pInstr, pGroup);
	return pInstr;
}

void lmScore::DoAddInstrument(lmInstrument* pInstr, lmInstrGroup* pGroup)
{
    //add instrument
	m_cInstruments.push_back(pInstr);

    //include instrument in group
    if (pGroup)
        pGroup->Include(pInstr);

    m_SCursor.MoveToStartOfInstrument( (int)m_cInstruments.size() );
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

void lmScore::LayoutAttachedObjects(lmBox* pBox, lmPaper *pPaper)
{
    //TODO: review these fixed values
    wxColour colorC = *wxBLACK;
    bool fHighlight = false;

    lmLUnits uyStartPos = pPaper->GetCursorY();		//save, to measure height
	m_uComputedPos.x = pPaper->GetCursorX();
	m_uComputedPos.y = pPaper->GetCursorY();

	//layout other AuxObjs attached directly to the score
    if (m_pAuxObjs)
    {
	    for (int i=0; i < (int)m_pAuxObjs->size(); i++)
	    {
		    //assign m_uComputedPos as paper pos. for this AuxObj
		    pPaper->SetCursorX(m_uComputedPos.x);
		    pPaper->SetCursorY(m_uComputedPos.y);

			//layout object
	        //std::vector<int>::iterator it = find(m_nTitles.begin(), m_nTitles.end(), i);
         //   if (it == m_nTitles.end())
			(*m_pAuxObjs)[i]->Layout(pBox, pPaper, fHighlight);

            //force auxObjs to take user position into account
            (*m_pAuxObjs)[i]->OnParentComputedPositionShifted(0.0f, 0.0f);

			//if block shape, add vertical space to paper cursor
            if ((*m_pAuxObjs)[i]->IsTextBlock())
            {
				lmGMObject* pGMO = (*m_pAuxObjs)[i]->GetGraphicObject();
				m_uComputedPos.y += pGMO->GetHeight();
            }
	    }
    }

    // update paper cursor position
    pPaper->SetCursor(m_uComputedPos.x, m_uComputedPos.y);
    m_nHeadersHeight = pPaper->GetCursorY() - uyStartPos;
}


lmLUnits lmScore::CreateTitleShape(lmBox* pBox, lmPaper *pPaper, lmScoreTitle* pTitle,
								   lmLUnits nPrevTitleHeight)
{
 //   // Creates the shape for the title and adds it to the box.
	//// Returns height of title

 //   lmLUnits nWidth, nHeight;
	//lmShape* pShape = (lmShape*)NULL;

 //   //// if not yet measured and positioned do it
 //   //if (!pTitle->IsFixed())
 //   //{
 //       lmEHAlign nAlign = pTitle->GetAlignment();
 //       lmLUnits xInitPaperPos = pPaper->GetCursorX();
 //       lmLUnits xPaperPos = xInitPaperPos;
 //       lmLUnits yPaperPos = pPaper->GetCursorY();

 //       //if need to reposition paper, convert units to tenths
 //       lmLUnits xPos, yPos;
 //       lmLocation tPos = pTitle->GetLocation();

 //       if (tPos.xUnits == lmTENTHS)
 //           xPos = tPos.x;
 //       else
 //           xPos = lmToLogicalUnits(tPos.x, tPos.xUnits);

 //       if (tPos.yUnits == lmTENTHS)
 //           yPos = tPos.y;
 //       else
 //           yPos = lmToLogicalUnits(tPos.y, tPos.yUnits);

 //       xPaperPos += xPos;
 //       yPaperPos += yPos;

 //       pPaper->SetCursorY( yPaperPos );

 //       //measure the text so that it can be properly positioned
 //       lmUPoint uPos(pPaper->GetCursorX(), pPaper->GetCursorY());
 //       pShape = pTitle->CreateShape(pPaper, uPos);
 //       pPaper->SetCursorX(xInitPaperPos);      //restore values altered by CreateShape
 //       pPaper->SetCursorY(yPaperPos);
 //       nWidth = pShape->GetWidth();
 //       nHeight = pShape->GetHeight();

 //       //Force new line if no space in current line
 //       lmLUnits xSpace = GetRightMarginXPos() - xInitPaperPos;
 //       if (xSpace < nWidth) {
 //           pPaper->SetCursorX(GetLeftMarginXPos());
 //           pPaper->SetCursorY(pPaper->GetCursorY() + nPrevTitleHeight);
 //       }

 //       if (nAlign == lmHALIGN_CENTER)
 //       {
 //           // 'center' alignment forces to center the string in current line,
 //           // without taking into account the space consumed by any posible existing
 //           // left title. That is, 'center' always means 'centered in the line'

 //           //if (tPos.xType == lmLOCATION_DEFAULT)
 //           //{
 //           //    xPos = (GetRightMarginXPos() - GetLeftMarginXPos() - nWidth)/2;
 //           //    //force new line if not enough space
 //           //    if (pPaper->GetCursorX() > xPos)
 //           //        pPaper->SetCursorY(pPaper->GetCursorY() + nPrevTitleHeight);
 //           //    pPaper->SetCursorX(GetLeftMarginXPos() + xPos);
 //           //}
 //           //else
 //           {
 //               pPaper->SetCursorX( xPaperPos );
 //           }
 //       }

 //       else if (nAlign == lmHALIGN_LEFT)
 //       {
 //           //align left.
 //           //if (tPos.xType == lmLOCATION_DEFAULT)
 //           //    pPaper->SetCursorX( GetLeftMarginXPos() );
 //           //else
 //               pPaper->SetCursorX( xPaperPos );
 //       }

 //       else
 //       {
 //           //align right
 //           //if (tPos.xType == lmLOCATION_DEFAULT)
 //           //    pPaper->SetCursorX(GetRightMarginXPos() - nWidth);
 //           //else
 //               pPaper->SetCursorX(xPaperPos - nWidth);
 //       }
 //   //}

	////the position has been computed. Create the shape if not yet created or
	////update it, if its was created during measurements
	//if (pShape) delete pShape;
	//pShape = pTitle->CreateShape(pPaper, lmUPoint(pPaper->GetCursorX(), pPaper->GetCursorY()) );

	////add shape to the box
	//pBox->AddShape(pShape);

 //   nHeight = pShape->GetHeight();

 //   //if rigth aligned, advance new line
 //   if (pTitle->GetAlignment() == lmHALIGN_RIGHT) {
 //       pPaper->SetCursorX( GetLeftMarginXPos() );
 //       pPaper->IncrementCursorY( nHeight );
 //   }

 //   return nHeight;

return 0.0f;

}

int lmScore::GetNumberOfInstrument(lmInstrument* pInstr)
{
    //returns the number (1..n) of instrument pInstr in the instruments collection

    std::vector<lmInstrument*>::iterator it;
    int i;
    for(i=1, it=m_cInstruments.begin(); it != m_cInstruments.end(); ++it, ++i)
    {
        if (*it == pInstr)
            return i;
    }
    wxASSERT(false);
    return 1;           //compiler happy
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
    wxString sDump = wxString::Format(_T("Score ID: %d\nGlobal objects:\n"), GetScoreID());

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

wxString lmScore::SourceLDP(bool fUndoData, wxString sFilename)
{
    if(fUndoData)
        g_pCursorSO = m_SCursor.GetStaffObj();
    else
        g_pCursorSO = (lmStaffObj*)NULL;

    wxString sSource = _T("(score\n   (vers 1.6)(language en utf-8)\n");

    //add comment with info about generator program and version
    sSource += _T("   //LDP file generated by LenMus, version ");
    sSource += wxGetApp().GetVersionNumber();
	sSource += _T(". Date: ");
	sSource += (wxDateTime::Now()).Format(_T("%Y-%m-%d"));
    sSource += _T("\n");

    //comment with ID counter value
    sSource += wxString::Format(_T("   (undoData (idCounter  %d))\n"), m_nCounterID);

    //creation mode
    if (!m_sCreationModeName.empty())
    {
        sSource += _T("   (creationMode ");
        sSource += m_sCreationModeName;
        sSource += _T(" ");
        sSource += m_sCreationModeVers;
        sSource += _T(")\n");
    }

    //styles
    sSource += m_TextStyles.SourceLDP(1, fUndoData);

    //titles and other attached auxobjs
    if (m_pAuxObjs)
    {
	    for (int i=0; i < (int)m_pAuxObjs->size(); i++)
	    {
		    sSource += (*m_pAuxObjs)[i]->SourceLDP(1, fUndoData);
	    }
    }

    //first page layout info
	lmPageInfo* pPageInfo = m_PagesInfo.front();
    sSource += pPageInfo->SourceLDP(1, fUndoData);

    //first system and other systems layout info
    //sSource += m_SystemsInfo.front()->SourceLDP(1, fUndoData);
    //sSource += m_SystemsInfo.back()->SourceLDP(1, fUndoData);

    //score cursor information
    if (fUndoData)
    {
        sSource += wxString::Format(_T("   (cursor %d %d %s)\n"),
                        m_SCursor.GetCursorInstrumentNumber(),
                        m_SCursor.GetCursorNumStaff(),
		                DoubleToStr((double)m_SCursor.GetCursorTime(), 2).c_str() );
    }

    //loop for each instrument
    for (int i=0; i < (int)m_cInstruments.size(); i++)
    {
        sSource += m_cInstruments[i]->SourceLDP(1, fUndoData);
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

void lmScore::Play(bool fVisualTracking, bool fCountOff, lmEPlayMode nPlayMode,
                 long nMM, wxWindow* pWindow)
{
    if (!m_pSoundMngr) {
        m_pSoundMngr = new lmSoundManager(this);
        ComputeMidiEvents();
    }

    m_pSoundMngr->Play(fVisualTracking, fCountOff, nPlayMode, nMM, pWindow);

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

void lmScore::PlayFromMeasure(int nMeasure, bool fVisualTracking, lmEPlayMode nPlayMode,
							  long nMM, wxWindow* pWindow)
{
    if (!m_pSoundMngr) {
        m_pSoundMngr = new lmSoundManager(this);
        ComputeMidiEvents();
    }

    m_pSoundMngr->PlayFromMeasure(nMeasure, fVisualTracking, nPlayMode, nMM, pWindow);
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

void lmScore::ScoreHighlight(lmStaffObj* pSO, wxDC* pDC, lmEHighlightType nHighlightType)
{
    switch (nHighlightType) {
        case eVisualOn:
            m_cHighlighted.push_back(pSO);
            pSO->PlaybackHighlight(pDC, g_pColors->ScoreHighlight());
            break;

        case eVisualOff:
			m_cHighlighted.erase( std::find(m_cHighlighted.begin(), m_cHighlighted.end(), pSO) );
            RemoveHighlight(pSO, pDC);
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
        lmScoreHighlightEvent event(this->GetScoreID(), *pItem, eVisualOff);
        ::wxPostEvent(pCanvas, event);
    }
}

void lmScore::RemoveHighlight(lmStaffObj* pSO, wxDC* pDC)
{
    // If we paint in black it remains a red aureole around
    // the note.
    // A posible solution is to paint the highlight using XOR draw mode in RED and
    // to remove it using again XOR draw mode in RED

    pSO->PlaybackHighlight(pDC, g_pColors->ScoreHighlight());   //g_pColors->ScoreNormal());
}

void lmScore::DeleteMidiEvents()
{
    if (m_pSoundMngr)
    {
        m_pSoundMngr->DeleteEventsTable();
        delete m_pSoundMngr;
        m_pSoundMngr = (lmSoundManager*)NULL;
    }
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

lmScoreCursor* lmScore::SetState(lmCursorState* pState)
{
    m_SCursor.SetState(pState);
    return &m_SCursor;
}

lmScoreCursor* lmScore::SetState(int nInstr, int nStaff, float rTimepos, lmStaffObj* pSO)
{
    lmCursorState oState(nInstr, nStaff, rTimepos, pSO);
    return SetState(&oState);
}

lmUPoint lmScore::CheckHandlerNewPosition(lmHandler* pHandler, int nIdx, int nPage, lmUPoint& uPos)
{
    //A handler owned by the score is being dragged. This method is invoked to
    //apply any desired movement constrain on the handler.
    //Returns the received point, modified to satisfy the constrains


    //limit margins movement to have at least 30% of page size for rendering the score

    lmUSize size = GetPaperSize(nPage);

    lmLUnits uTopPos = GetPageTopMargin(nPage);
    lmLUnits uBottomPos = GetMaximumY(nPage);
    lmLUnits uLeftPos = GetPageLeftMargin(nPage);
    lmLUnits uRightPos = GetRightMarginXPos(nPage);

    if (nIdx == lmMARGIN_TOP)
        uTopPos = uPos.y;

    else if (nIdx == lmMARGIN_BOTTOM)
        uBottomPos = uPos.y;

    else if (nIdx == lmMARGIN_LEFT)
        uLeftPos = uPos.x;

    else    //right
        uRightPos = uPos.x;

    //check 30% of size
    lmLUnits uMinimum;
    bool fLimit = false;
    if (nIdx == lmMARGIN_LEFT || nIdx == lmMARGIN_RIGHT)     //vertical margin
    {
        uMinimum = 0.3f * size.Width();
        fLimit = (uRightPos - uLeftPos < uMinimum);
    }
    else    //horizontal margin
    {
        uMinimum = 0.3f * size.Height();
        fLimit = (uBottomPos - uTopPos < uMinimum);
    }


    //proceed to clip
    lmUPoint pos = uPos;
    if (fLimit)
    {
        if (nIdx == lmMARGIN_TOP)
            pos. y = uBottomPos - uMinimum;

        else if (nIdx == lmMARGIN_BOTTOM)
            pos.y = uTopPos + uMinimum;

        else if (nIdx == lmMARGIN_LEFT)
            pos.x = uRightPos - uMinimum;

        else    //right
            pos.x = uLeftPos + uMinimum;
    }

    if (pos.x < 0.0f)
        pos.x = 0.0f;

    if (pos.x > size.Width())
        pos.x = size.Width();

    if (pos.y < 0.0f)
        pos.y = 0.0f;

    if (pos.y > size.Height())
        pos.y = size.Height();

    return pos;
}

void lmScore::PopupMenu(lmController* pCanvas, lmGMObject* pGMO, const lmDPoint& vPos)
{
	wxMenu* pMenu = pCanvas->GetContextualMenu(false);		//false->do not add any menu item
;

#if defined(__WXMSW__) || defined(__WXGTK__)
    //bitmaps on menus are supported only on Windows and GTK+
    wxMenuItem* pItem;
    wxSize nIconSize(16, 16);

    pItem = new wxMenuItem(pMenu, lmPOPUP_Score_Titles, _("Add title"),
						   _("Add a title to the score"), wxITEM_NORMAL);
    pItem->SetBitmap( wxArtProvider::GetBitmap(_T("tool_add_text"),
										       wxART_TOOLBAR, nIconSize) );
    pMenu->Append(pItem);

    pItem = new wxMenuItem(pMenu, lmPOPUP_View_Page_Margins, _("Margins and spacers"));
    pItem->SetBitmap( wxArtProvider::GetBitmap(_T("tool_page_margins"), wxART_TOOLBAR, nIconSize) );
    pMenu->Append(pItem);

	//pMenu->AppendSeparator();


#else
    //No bitmaps on menus for other platforms different from Windows and GTK+
    pMenu->Append(lmPOPUP_Score_Titles, _("Add title"));
    pMenu->Append(lmPOPUP_View_Page_Margins, _("Margins and spacers"));
	//pMenu->AppendSeparator();

#endif

	pCanvas->ShowContextualMenu(this, pGMO, pMenu, vPos.x, vPos.y);
}

bool lmScore::OnInstrProperties(int nInstr, lmController* pController)
{
	//Shows the Instruments properties dialog for the selected instrumnet (0..n).
	//If nInst == -1 shows a selction dialog first
    //Returns true if dialog end by clicking on 'Accept' button

	if (nInstr == -1)
	{
		//load array with instruments names
		wxArrayString aChoices;
		int iN = 1;
		lmInstrument* pInstr = this->GetFirstInstrument();
		while(pInstr)
		{
			aChoices.Add( wxString::Format(_T("%d - %s"), iN, pInstr->GetInstrName().c_str()) );
			pInstr = this->GetNextInstrument();
			iN++;
		}
		//if only one instrument select it. Otherwise, show selection dialog
		if (iN==2)
			nInstr = 0;
		else
			nInstr = ::wxGetSingleChoiceIndex(_("Select instrument:"),
											  _("Instrument selection"), aChoices);
	}

	if (nInstr == -1) return false;		//cancel button

	//Instrument selected. Show properties dialog
	lmDlgProperties dlg(pController);
	lmInstrument* pInstr = GetInstrument(nInstr+1);
    pInstr->OnEditProperties(&dlg);		//add specific panels
	dlg.Layout();
	if (dlg.ShowModal() != wxID_OK)
        return false;

    pInstr->OnPropertiesChanged();
    return true;
}

//-------------------------------------------------------------------------------------

lmBoxScore* lmScore::Layout(lmPaper* pPaper)
{
    //wxLogMessage(this->Dump());
    #ifdef __WXDEBUG__
        wxStopWatch oTimer;
    #endif

    lmFormatter4 oFormatter;
    lmBoxScore* pGMObj = oFormatter.LayoutScore(this, pPaper);
    StoreShape(pGMObj);

    #ifdef __WXDEBUG__
        oTimer.Pause();
        g_pLogger->LogTrace(_T("Timing: Score renderization"), _T("[lmScore::Layout] %ld ms required for layouting score %d (%s)"),
                            oTimer.Time(), m_nScoreID, m_sScoreName.c_str() );
    #endif

    return pGMObj;
}



//-----------------------------------------------------------------------------------
// lmStylesCollection implementation
//-----------------------------------------------------------------------------------

lmStylesCollection::lmStylesCollection()
{
    //initialize iterator
    m_it = m_aTextStyles.begin();
}

lmStylesCollection::~lmStylesCollection()
{
    std::list<lmTextStyle*>::iterator it;
    for (it = m_aTextStyles.begin(); it != m_aTextStyles.end(); ++it)
        delete *it;
    m_aTextStyles.clear();
}

lmTextStyle* lmStylesCollection::GetStyleInfo(const wxString& sStyleName)
{
    //Returns the TextStyle info for the requested style name

    std::list<lmTextStyle*>::iterator it;
    for (it = m_aTextStyles.begin(); it != m_aTextStyles.end(); ++it)
    {
        if ((*it)->sName == sStyleName)
            return *it;
    }

    //Style not found
    //if the requested style is one of the default styles, create and register it
    if (sStyleName == _("Lyrics"))
    {
        lmFontInfo tLyricFont = { _T("Times New Roman"), 8, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL};
        lmTextStyle* pTS = new lmTextStyle;
        pTS->nColor = *wxBLACK;
        pTS->tFont = tLyricFont;
        pTS->sName = _("Lyrics");
        m_aTextStyles.push_back( pTS );
        return pTS;
    }

    if (sStyleName == _("Normal text"))
    {
        lmFontInfo tNormalFont = { _T("Times New Roman"), 10, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL};
        lmTextStyle* pTS = new lmTextStyle;
        pTS->nColor = *wxBLACK;
        pTS->tFont = tNormalFont;
        pTS->sName = _("Normal text");
        m_aTextStyles.push_back( pTS );
        return pTS;
    }

    if (sStyleName == _("Title"))
    {
        lmFontInfo tNormalFont = { _T("Times New Roman"), 21, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD};
        lmTextStyle* pTS = new lmTextStyle;
        pTS->nColor = *wxBLACK;
        pTS->tFont = tNormalFont;
        pTS->sName = _("Title");
        m_aTextStyles.push_back( pTS );
        return pTS;
    }

    return (lmTextStyle*)NULL;
}

lmTextStyle* lmStylesCollection::GetStyleName(lmFontInfo& tFontData, wxColour nColor)
{
    //This method tries to find a matching style for the received font and colour info.
    //If not found, creates a new one and returns it.

    std::list<lmTextStyle*>::iterator it;
    for (it = m_aTextStyles.begin(); it != m_aTextStyles.end(); ++it)
    {
        if (nColor == (*it)->nColor &&
            tFontData.nFontSize == (*it)->tFont.nFontSize &&
            tFontData.nFontStyle == (*it)->tFont.nFontStyle &&
            tFontData.nFontWeight == (*it)->tFont.nFontWeight &&
            tFontData.sFontName == (*it)->tFont.sFontName )

            return *it;
    }

    //not found. Create a new style
    return AddStyle( wxString::Format(_("Style%d"), (int)m_aTextStyles.size()),
                     tFontData, nColor );
}

lmTextStyle* lmStylesCollection::AddStyle(const wxString& sName, lmFontInfo& tFontData,
                                          wxColour nColor)
{
	//check that style name is not duplicated
    std::list<lmTextStyle*>::iterator it;
    for (it = m_aTextStyles.begin(); it != m_aTextStyles.end(); ++it)
    {
        if ((*it)->sName == sName)
			return *it;		//already exists
            break;
    }

    lmTextStyle* pNewTS = new lmTextStyle;
    pNewTS->nColor = nColor;
    pNewTS->tFont = tFontData;
    pNewTS->sName = sName;

    m_aTextStyles.push_back( pNewTS );
    return pNewTS;
}

void lmStylesCollection::RemoveStyle(lmTextStyle* pStyle)
{
    std::list<lmTextStyle*>::iterator it;
    it = std::find(m_aTextStyles.begin(), m_aTextStyles.end(), pStyle);
    m_aTextStyles.erase(it);
}

void lmStylesCollection::StyleInUse(bool fInUse)
{
}

lmTextStyle* lmStylesCollection::GetFirstStyle()
{
    m_it = m_aTextStyles.begin();
    if (m_it == m_aTextStyles.end())
        return (lmTextStyle*)NULL;

    return *m_it;
}

lmTextStyle* lmStylesCollection::GetNextStyle()
{
    //advance to next one
    ++m_it;
    if (m_it != m_aTextStyles.end())
        return *m_it;

    //no more items
    return (lmTextStyle*)NULL;
}

wxString lmStylesCollection::SourceLDP(int nIndent, bool fUndoData)
{
	wxString sSource = _T("");
    std::list<lmTextStyle*>::iterator it;
    for (it = m_aTextStyles.begin(); it != m_aTextStyles.end(); ++it)
        sSource += SourceLDP(nIndent, *it);

    return sSource;
}

wxString lmStylesCollection::SourceLDP(int nIndent, lmTextStyle* pStyle)
{
	wxString sSource = _T("");
	sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    sSource += _T("(defineStyle");

    //style name
    sSource += _T(" \"");
    sSource += pStyle->sName;
    sSource += _T("\"");

    //font info
    sSource += wxString::Format(_T(" (font \"%s\" %dpt"),
                                pStyle->tFont.sFontName.c_str(), pStyle->tFont.nFontSize );
    if (pStyle->tFont.nFontStyle == wxFONTSTYLE_ITALIC &&
        pStyle->tFont.nFontWeight == wxFONTWEIGHT_BOLD)
        sSource += _T(" bold-italic");
    else if (pStyle->tFont.nFontWeight == wxFONTWEIGHT_BOLD)
        sSource += _T(" bold");
    else if(pStyle->tFont.nFontStyle == wxFONTSTYLE_ITALIC)
        sSource += _T(" italic");
    else
        sSource += _T(" normal");
    sSource += _T(")");

    //color
    sSource += _T(" (color ");
    sSource += pStyle->nColor.GetAsString(wxC2S_HTML_SYNTAX);
    sSource += _T(")");

    //close element
    sSource += _T(")\n");
    return sSource;
}

