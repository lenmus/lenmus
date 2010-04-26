//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2010 LenMus project
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

#ifndef __LM_THEOINTERVALSCONSTRAINS_H__        //to avoid nested includes
#define __LM_THEOINTERVALSCONSTRAINS_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "TheoIntervalsConstrains.cpp"
#endif

// For compilers that support precompilation, includes <wx/wx.h>.
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "../score/Score.h"
#include "Constrains.h"


enum EProblemTheoIntervals
{
    ePT_DeduceInterval = 0,         //WARNING: the enum values are used as indexes
    ePT_BuildInterval,              // in DlgCfgTheoIntervals. Do not alter
    ePT_Both                        // neither order nor values. Must start in 0
};


class lmTheoIntervalsConstrains: public lmExerciseOptions
{
public:
    lmTheoIntervalsConstrains(wxString sSection);
    ~lmTheoIntervalsConstrains() {}

    inline bool IsValidClef(lmEClefType nClef) { return m_oClefs.IsValid(nClef); }
    inline void SetClef(lmEClefType nClef, bool fValid) { m_oClefs.SetValid(nClef, fValid); }

    inline EProblemTheoIntervals GetProblemType() { return m_nProblemType; }
    inline void SetProblemType(EProblemTheoIntervals nType) { m_nProblemType = nType; }

    inline bool IsTypeAllowed(int nType) { return m_fTypeAllowed[nType]; }
    inline void SetTypeAllowed(int nType, bool fValue) { m_fTypeAllowed[nType] = fValue; }

    inline bool GetAccidentals() { return m_fAccidentals; }
    inline void SetAccidentals(bool fValue) { m_fAccidentals = fValue; }

    inline bool GetDoubleAccidentals() { return m_fDoubleAccidentals; }
    inline void SetDoubleAccidentals(bool fValue) { m_fDoubleAccidentals = fValue; }

    inline lmClefConstrain* GetClefConstrains() { return &m_oClefs; }
    inline lmKeyConstrains* GetKeyConstrains() { return &m_oValidKeys; }

    inline int GetLedgerLinesAbove() { return m_nLedgerAbove; }
    inline int GetLedgerLinesBelow() { return m_nLedgerBelow; }
    inline void SetLedgerLinesAbove(int nLines) { m_nLedgerAbove = nLines; }
    inline void SetLedgerLinesBelow(int nLines) { m_nLedgerBelow = nLines; }

    inline int GetProblemLevel() { return m_nProblemLevel; }
    inline void SetProblemLevel(int nLevel) { m_nProblemLevel = nLevel; }

    inline void SetSection(wxString sSection) {
                m_sSection = sSection;
                LoadSettings();
            }

    void SaveSettings();


private:
    void LoadSettings();

    lmClefConstrain         m_oClefs;               //allowed clefs
    lmKeyConstrains         m_oValidKeys;           //allowed key signatures
    EProblemTheoIntervals   m_nProblemType;
    int                     m_nLedgerAbove;         //ledger lines above
    int                     m_nLedgerBelow;         //ledger lines below
    int                     m_nProblemLevel;        //Problem level:        
                                                    //  0-Just name the interval number
                                                    //  1-Perfect, major and minor intervals
                                                    //  2-Also augmented and diminished
                                                    //  3-Also double augmented / diminished
    bool                    m_fAccidentals;         //allow accidentals
    bool                    m_fDoubleAccidentals;   //allow double accidentals
    bool                    m_fTypeAllowed[3];      //intervals. Allowed types:
                                                    //  0-harmonic
                                                    //  1-melodic ascending
                                                    //  2-melodic descending
};


#endif  // __LM_THEOINTERVALSCONSTRAINS_H__
