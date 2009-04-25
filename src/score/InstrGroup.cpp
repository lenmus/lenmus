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
#pragma implementation "InstrGroup.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "InstrGroup.h"
#include "Score.h"
#include "../graphic/GMObject.h"
#include "../graphic/Shapes.h"
#include "../graphic/ShapeText.h"
#include "../graphic/ShapeBracket.h"
#include "../app/Preferences.h"


lmInstrGroup::lmInstrGroup(lmEBracketSymbol nGrpSymbol, bool fJoinBarlines)
{
	m_pShape = (lmShape*)NULL;
    m_fJoinBarlines = fJoinBarlines;
    m_nBracket = nGrpSymbol;
    m_pName = (lmTextItem*)NULL;
    m_pAbbreviation = (lmTextItem*)NULL;
}

lmInstrGroup::~lmInstrGroup()
{
    //AWARE: instruments must not be deleted when deleting the list, as they are
    //part of a lmScore and will be deleted there.

	//the relationship is going to be removed. Release all instruments
    std::list<lmInstrument*>::iterator it;
    for(it = m_Instruments.begin(); it != m_Instruments.end(); ++it)
	{
        (*it)->OnRemovedFromGroup(this);
	}
    m_Instruments.clear();
}

void lmInstrGroup::Include(lmInstrument* pInstr, int nIndex)
{
    // Add a note to the relation. Index is the position that the added instrument 
    // must occupy (0..n).
    // If nIndex == -1, instrument will be added at the end.

	//add the instrument
	if (nIndex == -1 || nIndex == NumInstruments())
		m_Instruments.push_back(pInstr);
	else
	{
		int iN;
		std::list<lmInstrument*>::iterator it;
		for(iN=0, it=m_Instruments.begin(); it != m_Instruments.end(); ++it, iN++)
		{
			if (iN == nIndex)
			{
				//insert before current item
				m_Instruments.insert(it, pInstr);
				break;
			}
		}
	}
	//wxLogMessage(Dump());
	pInstr->OnIncludedInGroup(this);
    OnGroupModified();
}

void lmInstrGroup::Remove(lmInstrument* pInstr)
{
    //remove instrument.
	//AWARE: This method is always invoked from an Instrument. Therefore it will
	//not inform back the Instrument, as this is unnecessary and causes problems when
	//deleting the group object

    wxASSERT(NumInstruments() > 0);

    std::list<lmInstrument*>::iterator it;
    it = std::find(m_Instruments.begin(), m_Instruments.end(), pInstr);
    m_Instruments.erase(it);
    OnGroupModified();
}


void lmInstrGroup::MeasureNames(lmPaper* pPaper)
{
	// This method is invoked only from lmInstrument::MeasureNames(), in order to
	// measure the indentation for the group. 
    // When this method is invoked paper is positioned at top left corner of instrument
    // renderization point (x = left margin, y = top line of first staff)
	// To measure the names we have to create the shapes but we are going to delete
	// them at the end

    //Save original position to restore it later
    lmUPoint uPaperPos(pPaper->GetCursorX(), pPaper->GetCursorY());

    m_uIndentFirst = 0;
    m_uIndentOther = 0;

    lmPgmOptions* pPgmOpt = lmPgmOptions::GetInstance();
    lmTenths nOptValue = pPgmOpt->GetFloatValue(lm_EO_GRP_SPACE_AFTER_NAME);
    lmLUnits uSpaceAfterName = GetFirstInstrument()->TenthsToLogical(nOptValue);

    if (m_pName) {
        // measure text extent
        lmShape* pShape = m_pName->CreateShape(pPaper, uPaperPos);
        // set indent =  text extend + after text space
        m_uIndentFirst = pShape->GetWidth() + uSpaceAfterName;
		delete pShape;
    }

    if (m_pAbbreviation) {
        // measure text extent
        lmShape* pShape = m_pAbbreviation->CreateShape(pPaper, uPaperPos);
        // set indent =  text extend + after text space
        m_uIndentOther = pShape->GetWidth() + uSpaceAfterName;
		delete pShape;
    }

    if (RenderBraket())
    {
        if (m_nBracket == lm_eBracket)
        {
            //bracket
            nOptValue = pPgmOpt->GetFloatValue(lm_EO_GRP_BRACKET_WIDTH);
            m_uBracketWidth = GetFirstInstrument()->TenthsToLogical(nOptValue);
            nOptValue = pPgmOpt->GetFloatValue(lm_EO_GRP_BRACKET_GAP);
            m_uBracketGap = GetFirstInstrument()->TenthsToLogical(nOptValue);
        }
        else
        {
            //brace
            nOptValue = pPgmOpt->GetFloatValue(lm_EO_GRP_BRACE_WIDTH);
            m_uBracketWidth = GetFirstInstrument()->TenthsToLogical(nOptValue);
            nOptValue = pPgmOpt->GetFloatValue(lm_EO_GRP_BRACE_GAP);
            m_uBracketGap = GetFirstInstrument()->TenthsToLogical(nOptValue);
        }

        m_uIndentOther += m_uBracketWidth + m_uBracketGap;
        m_uIndentFirst += m_uBracketWidth + m_uBracketGap;
    }

    //restore original paper position
    pPaper->SetCursor( uPaperPos );

}


bool lmInstrGroup::RenderBraket()
{
    //returns true if a brace / bracket must be rendered

    return (m_nBracket == lm_eBracket || m_nBracket == lm_eBrace );
}

void lmInstrGroup::AddNameAndBracket(lmBox* pBox, lmPaper* pPaper, int nSystem,
                                     lmLUnits xLeft, lmLUnits yTop, lmLUnits yBottom)
{
    //Layout. This method is responsible for adding the shapes for the group name/abbreviation
    //and for the group brace/bracket. It receives xLeft, yTop & yBottom of the staves that
    //form the group of instruments, and has to coumpute positions for the shapes to add.
    //The received box is a BoxSystem and its xLeft bound is at right marging of paper

	if (nSystem == 1)
        AddNameAbbrevShape(pBox, pPaper, m_pName, xLeft, yTop, yBottom);
	else
        AddNameAbbrevShape(pBox, pPaper, m_pAbbreviation, xLeft, yTop, yBottom);
}

void lmInstrGroup::AddNameAbbrevShape(lmBox* pBox, lmPaper* pPaper, lmTextItem* pName,
                                      lmLUnits xStaff, lmLUnits yTop, lmLUnits yBottom)
{
    //add shape for the bracket
    if (RenderBraket())
    {
        lmLUnits xLeft = xStaff - m_uBracketWidth - m_uBracketGap;
        lmLUnits xRight = xStaff - m_uBracketGap;
        lmShape* pShape;
        if (m_nBracket == lm_eBracket)
            pShape = new lmShapeBracket(GetFirstInstrument(), xLeft, yTop, xRight, yBottom,
                                        *wxBLACK);
        else
        {
            lmLUnits dyHook = GetFirstInstrument()->TenthsToLogical(6.0f);
            pShape = new lmShapeBrace(GetFirstInstrument(), xLeft, yTop, xRight, yBottom,
                                      dyHook, *wxBLACK);
        }
        pBox->AddShape(pShape, lm_eLayerStaff);
    }

    //add shape for the name/abbreviation
    if (pName)
    {
        lmScore* pScore = pBox->GetScoreOwner()->GetScore();
        lmUPoint uPos(pScore->GetPageLeftMargin(), yTop);
        lmShape* pShape = pName->CreateShape(pPaper, uPos);
        pShape->Shift(0.0f, ((yBottom - yTop) - pShape->GetHeight())/2.0f );
        pBox->AddShape(pShape, lm_eLayerStaff);
    }
}

void lmInstrGroup::OnGroupModified()
{
    //it is not ncessary to do anything.
}

