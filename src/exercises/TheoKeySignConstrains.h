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

#ifndef __THOEKEYSIGNCONSTRAINS_H__        //to avoid nested includes
#define __THOEKEYSIGNCONSTRAINS_H__

#ifdef __GNUG__
#pragma interface "TheoKeySignConstrains.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "../score/Score.h"
#include "Constrains.h"
#include "ExerciseCtrol.h"


enum EProblemTheoKeySig
{
    eIdentifyKeySignature = 0,
    eWriteKeySignature,
    eBothKeySignProblems
};

enum EScaleMode
{
    eMajorMode = 0,
    eMinorMode,
    eMayorAndMinorModes
};


/*! @class lmTheoKeySignConstrains
    @brief Options for lmTheoKeySignCtrol control
*/
class lmTheoKeySignConstrains : public lmExerciseConstrains
{
public:
    lmTheoKeySignConstrains(wxString sSection);
    ~lmTheoKeySignConstrains() {}

    EProblemTheoKeySig GetProblemType() { return m_nProblemType; }
    void SetProblemType(EProblemTheoKeySig nType) { m_nProblemType = nType; }

    int GetMaxAccidentals() { return m_nMaxAccidentals; }
    void SetMaxAccidentals(int nNum) { m_nMaxAccidentals = nNum; }

    EScaleMode GetScaleMode() { return m_nMode; }
    void SetScaleMode(EScaleMode nMode) { m_nMode = nMode; }

    bool IsValidClef(EClefType nClef) { return m_oClefs.IsValid(nClef); }
    void SetClef(EClefType nClef, bool fValid) { m_oClefs.SetValid(nClef, fValid); }
    lmClefConstrain* GetClefConstrains() { return &m_oClefs; }

    lmKeyConstrains* GetKeyConstrains() { return &m_oValidKeys; }


private:
    EProblemTheoKeySig      m_nProblemType;
    int                     m_nMaxAccidentals;
    EScaleMode              m_nMode;
    lmKeyConstrains         m_oValidKeys;           //allowed key signatures
    lmClefConstrain         m_oClefs;               //allowed clefs

};

#endif  // __THOEKEYSIGNCONSTRAINS_H__
