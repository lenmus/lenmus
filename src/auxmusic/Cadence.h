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

#ifndef __LM_CADENCE_H__        //to avoid nested includes
#define __LM_CADENCE_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Cadence.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "vector"

#include "../score/Score.h"
#include "Interval.h"
#include "../exercises/CadencesConstrains.h"
#include "Conversion.h"
#include "ChordManager.h"


// auxiliary object to comfortably manage chords
class lmHChord
{
public:

    lmHChord() { 
        nNote[0] = nNote[1] = nNote[2] = nNote[3] = -1;
        nAcc[0] = nAcc[1] = nAcc[2] = nAcc[3] = 0;
        fValid=false;
        nNumNotes=0;
    };

    wxString GetPrintName(int iNote) {
        wxString sAnswer;
        switch(nAcc[iNote]) {
            case -2: sAnswer =_T("--"); break;
            case -1: sAnswer =_T("-"); break;
            case 0:  sAnswer =_T(" "); break;
            case 1:  sAnswer =_T("+"); break;
            case 2:  sAnswer =_T("++"); break;
            default:
                sAnswer = _T("");
        }
        sAnswer += lmConverter::GetEnglishNoteName(nNote[iNote]).c_str();
        return sAnswer;
    };

    lmDPitch nNote[4];      //diatonic pitch so 1:1 mapping to staff lines/spaces
    int      nAcc[4];       //accidentals: -2 ... +2
    bool     fValid;        //useful to filter out invalid chords
    int      nNumNotes;     //normally 4
};



//declare global functions defined in this module
extern wxString CadenceTypeToName(lmECadenceType nType);
//extern int NumNotesInScale(lmECadenceType nType);

//A cadence is a sequence of up to 2 chords
//Change this for more chords in a cadence
#define lmCHORDS_IN_CADENCE  2

class lmCadence
{
public:
    lmCadence();
    ~lmCadence();

    bool Create(lmECadenceType nCadenceType, EKeySignatures nKey, bool fUseGrandStaff);
    bool IsCreated() { return m_fCreated; }

    lmECadenceType GetCadenceType() { return m_nType; }
    int GetNumChords() { return m_nNumChords; }
    lmChordManager* GetChord(int iC);
	wxString GetName();
    wxString GetNotePattern(int iChord, int iNote);

private:
    wxString SelectChord(wxString sFunction, EKeySignatures nKey, int* pInversion);
    wxString GetRootNote(wxString sFunct, EKeySignatures nKey, EClefType nClef,
                         bool fUseGrandStaff);
    //--------
    void GenerateFirstChord(lmChordManager* pChord, int nInversion);
    int GenerateSopranoNote(lmNoteBits oChordNotes[4], int iBass, int nNumNotes);
    int FilterChords(std::vector<lmHChord>& aChords, int nSteps[4], int nNumSteps,
                     int nStep5);
    void GenerateNextChord(lmChordManager* pChord, int nInversion, int iPrevHChord);


    //member variables

    bool            m_fCreated;
    lmECadenceType  m_nType;
    EKeySignatures  m_nKey;
    lmChordManager  m_aChord[lmCHORDS_IN_CADENCE];
    int             m_nInversions[lmCHORDS_IN_CADENCE];
    int             m_nNumChords;       //num of chords in this cadence
    lmHChord        m_Chord[lmCHORDS_IN_CADENCE];

};

#endif  // __LM_CADENCE_H__

