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
#pragma implementation "Staff.h"
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
#include "../app/global.h"

//implementation of the lmStaff List
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(StaffList);

// A lmStaff is a collection of consecutive lines and spaces.

//constructor
lmStaff::lmStaff(lmScore* pScore, int nNumLines, lmLUnits uUnits)
    : lmScoreObj(pScore)
{
    m_nNumLines = nNumLines;
    m_uLineThickness = lmToLogicalUnits(0.15, lmMILLIMETERS);     //TODO user option
    if (uUnits == 0)
        m_uSpacing = lmToLogicalUnits(1.8, lmMILLIMETERS);   //Default 1.8 mm -> lmStaff height = 7.2 mm
    else
        m_uSpacing = uUnits;        // in logical units

    // margins
    m_uAfterSpace = lmToLogicalUnits(10, lmMILLIMETERS);    // 10 mm
    m_uLeftMargin = 0;
    m_uRightMargin = 0;
}

lmStaff::~lmStaff()
{
}

lmLUnits lmStaff::GetHeight()
{
    // returns the height (in logical units) of the staff without margins, that is, the
    // distance between first and last line
    return (m_nNumLines - 1) * m_uSpacing;

}

lmContext* lmStaff::NewContext(lmContext* pCurrentContext, int nNewAccidentals, int nStep)
{
	return (lmContext*)NULL;
    ////locate context received
    //int nNodeIndex = m_cContext.IndexOf(pCurrentContext);
    //wxASSERT(nNodeIndex != wxNOT_FOUND);

    ////create a new context and store it
    //lmContext* pNewContext = new lmContext(pCurrentContext->GetClef(),
    //                                pCurrentContext->GeyKey(),
    //                                pCurrentContext->GetTime() );
    //pNewContext->CopyAccidentals(pCurrentContext);
    //pNewContext->SetAccidental(nStep, nNewAccidentals);

    ////insert new context after the received one
    //m_cContext.Insert(++nNodeIndex, pNewContext);
    //return pNewContext;

    /*AWARE
      propagation of changes to following contexts until a start of measure found.

      Two problems had to be solved:

        Problem 1: How to propagate changes to Notes using the context ?
            When an accidental is inserted in/ deleted from a note it is the note who must
            invoke for a change in context. And context propagates changes to the start of
            a new measure. To update pitch I devise two alternatives:
            1. If notes does not have precomputed pitch but it is computed when needed, nothing
                else must be done: context is updated and pitch will be updated when required.
            2. If notes must have pitch updated, then context must invoke a process to update
                the pitch of all notes in the measure.

        Problem 2: How to know when the start of measure is reached ?
            The context must keep information about the measure in which it is created.
            If, for example, the next context was created three measures later, how do we know
            were to create a new context and what are the affected staffobj? ->
            There MUST exist a context for each measure. Too expensive?

        A third alternative could solve both problems and keep picth updated: When an accidental
        is inserted in or deleted from a note this note invokes a method (in lmVStaff) to update
        contexts. lmVStaff, through the lmColStaffObjs, has information
        about where each measure starts and which StaffObjs belong to each staff. So lmVStaff
        can locate
        the starting note and ask all remaining notes (from that one until the end of that measure)
        to update their contexts.

        Solution:
        In current implementation I use the third alternative, so in this method it is
        not necessary to do anything about, as te lmVStaff method OnContextUpdated() takes care
        of everything
    */

}

wxString lmStaff::Dump()
{
    wxString sDump = _T("Staff\n");
    return sDump;
}
