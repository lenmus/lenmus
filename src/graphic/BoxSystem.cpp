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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "BoxSystem.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "BoxPage.h"
#include "BoxSystem.h"
#include "BoxSlice.h"
#include "BoxInstrSlice.h"

//access to colors
#include "../globals/Colors.h"
extern lmColors* g_pColors;


//-----------------------------------------------------------------------------------------
// Implementation of class lmBoxSystem: a system in the printed score. 
//-----------------------------------------------------------------------------------------

lmBoxSystem::lmBoxSystem(lmBoxPage* pParent, int nNumPage)
    : lmBox(eGMO_BoxSystem)
{
    m_nNumMeasures = 0;
    m_nNumPage = nNumPage;
    m_pBPage = pParent;
}


lmBoxSystem::~lmBoxSystem()
{
    //delete  InstrSlices collection
    for (int i=0; i < (int)m_InstrSlices.size(); i++)
    {
        delete m_InstrSlices[i];
    }
    m_InstrSlices.clear();

    //delete BoxSlices collection
    for (int i=0; i < (int)m_Slices.size(); i++)
    {
        delete m_Slices[i];
    }
    m_Slices.clear();
}

void lmBoxSystem::Render(int nSystem, lmScore* pScore, lmPaper* pPaper)
{
    //At this point paper position is not in the right place. Therefore, we move
    //to the start of system position.
    pPaper->SetCursorY( m_yPos );

#if 1	//OLD_RENDER
    //for each lmInstrument
    for (int i=0; i < (int)m_InstrSlices.size(); i++)
    {
        m_uBoundsBottom.y = m_InstrSlices[i]->Render(pPaper, m_xPos, m_nNumPage, nSystem);
    }

    //Draw the initial barline that joins all staffs in a system
    if (pScore->GetOptionBool(_T("Staff.DrawLeftBarline")) )
    {
        lmLUnits uLineThickness = lmToLogicalUnits(0.2, lmMILLIMETERS);        // thin line width will be 0.2 mm @todo user options
        pPaper->SolidLine(m_uBoundsTop.x, m_uBoundsTop.y,
                          m_uBoundsTop.x, m_uBoundsBottom.y,
                          uLineThickness, eEdgeNormal, *wxBLACK);
    }
#else
	//for each lmBoxSlice
    for (int i=0; i < (int)m_Slices.size(); i++)
    {
        m_Slices[i]->Render(pPaper, lmUPoint(m_xPos, m_yPos));
    }
#endif
}

void lmBoxSystem::SetNumMeasures(int nMeasures, lmScore* pScore)
{
    //Now we have all the information about the system. Let's create the collection
    //of BoxSlices

    m_nNumMeasures = nMeasures;

    //Build the slices
    int iInstr = 1;
	int nLastMeasure = m_nFirstMeasure + m_nNumMeasures - 1;
    for (lmInstrument* pInstr = pScore->GetFirstInstrument();
         pInstr;
         pInstr = pScore->GetNextInstrument(), iInstr++)
    {
            m_InstrSlices.push_back(
                    new lmBoxInstrSlice(this, m_nFirstMeasure, nLastMeasure,
                                        pInstr, iInstr) );

    }

}

//bool lmBoxSystem::ContainsPoint(lmUPoint& pointL)
//{
//    //returns true if point received is within the limits of this System
//
//    lmURect bounds(m_uBoundsTop.x, m_uBoundsTop.y,
//                   m_uBoundsBottom.x - m_uBoundsTop.x, m_uBoundsBottom.y - m_uBoundsTop.y);
//    return bounds.Contains(pointL);
//
//}
//
lmBoxSlice* lmBoxSystem::FindSliceAtPosition(lmUPoint& pointL)
{
    if (ContainsPoint(pointL))
    {
        //identify the measure
        for (int iS=0; iS < (int)m_Slices.size(); iS++)
        {
            if (m_Slices[iS]->FindMeasureAt(pointL))
            {
                return m_Slices[iS];
            }
        }
        wxMessageBox( wxString::Format( _T("Page %d, measure not identified!!! Between measure %d and %d"),
            m_nNumPage, m_nFirstMeasure, m_nFirstMeasure+m_nNumMeasures-1) );
        return (lmBoxSlice*)NULL;
    }
    return (lmBoxSlice*)NULL;
}

lmBoxInstrSlice* lmBoxSystem::FindInstrSliceAtPosition(lmUPoint& pointL)
{
    if (ContainsPoint(pointL))
    {
        //identify the InstrSlice
        for (int iIS=0; iIS < (int)m_InstrSlices.size(); iIS++)
        {
            if (m_InstrSlices[iIS]->ContainsPoint(pointL))
            {
                return m_InstrSlices[iIS];
            }
        }
        wxMessageBox( wxString::Format( _T("Page %d, InstrSlice not identified!!!"),
            m_nNumPage ));
        return (lmBoxInstrSlice*)NULL;
    }
    return (lmBoxInstrSlice*)NULL;
}

lmBoxSlice* lmBoxSystem::AddSlice(int nAbsMeasure, lmLUnits xStart, lmLUnits xEnd)
{
    lmBoxSlice* pBSlice = new lmBoxSlice(this, nAbsMeasure, xStart, xEnd);
    m_Slices.push_back(pBSlice);
    return pBSlice;
}

void lmBoxSystem::SetFinalX(lmLUnits xPos)
{ 
	SetXRight(xPos);

	//propagate change
    for (int i=0; i < (int)m_Slices.size(); i++)
    {
        m_Slices[i]->SetFinalX(xPos);
    }
}

wxString lmBoxSystem::Dump()
{
	wxString sDump = wxString::Format(_T("lmBoxSystem. %d measures starting at %d\n"),
						m_nNumMeasures, m_nFirstMeasure);

    //loop to dump the systems in this page
    for (int i=0; i < (int)m_Slices.size(); i++)
    {
        sDump += m_Slices[i]->Dump();
    }

	return sDump;
}

