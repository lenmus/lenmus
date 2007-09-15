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
#pragma implementation "TheoKeySignConstrains.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "TheoKeySignConstrains.h"

lmTheoKeySignConstrains::lmTheoKeySignConstrains(wxString sSection)
    : lmExerciseOptions(sSection)
{
    //
    // default values
    //

    // all key signatures allowed
    int i;
    for (i=lmMIN_KEY; i <= lmMAX_KEY; i++) {
        m_oValidKeys.SetValid((EKeySignatures)i, true);
    }

    // only G clef allowed
    for (i = lmMIN_CLEF; i <= lmMAX_CLEF; i++) {
        m_oClefs.SetValid((EClefType) i, false);
    }
    m_oClefs.SetValid(eclvSol, true);

    // other settings
    m_nProblemType = eBothKeySignProblems;      // both problems allowed
    m_nMaxAccidentals = 5;                      // max.: 5 accidentals
    m_nMode = eMayorAndMinorModes;              // both, major and minor, allowed

}

