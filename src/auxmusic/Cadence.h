//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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

// For selecting the less bad chrd it is necessary to know all errors and defects that a chord
// has. To this end , each defect will be stored as a bit mark
#define lmChordError    long

// Values for defects must be assigned by severity: higher numbers for higher severity
enum lmBadChordReason {
    lm_eChordValid = 0,
    lm_eNotDoubledThird     = 0x0001,   //Cuando el bajo enlaza el V grado con el VI (cadencia rota), en el acorde de VI grado se duplica la tercera.
    lm_eNotContraryMotion   = 0x0002,   //bass moves by step and not all other voices moves in opposite direction to bass
    lm_eGreaterThanSixth    = 0x0004,   //No es conveniente exceder el intervalo de sexta, exceptuando la octava justa
    lm_eChromaticAlter      = 0x0008,   //Chromatic alteration not resolved the same direction than the alteration
    lm_eVoiceOverlap        = 0x0010,   //voice overlap
    lm_eVoiceCrossing       = 0x0020,   //notes not in ascending sequence or duplicated
    lm_eGreaterThanOctave   = 0x0040,   //notes interval greater than one octave (other than bass-tenor)
    lm_eSeventhResolution   = 0x0080,   //the seventh of a chord should always resolve down by second.
    lm_eLeadingResolution   = 0x0100,   //Scale degree seven (the leading tone) doesn't resolve to tonic
    lm_eLeadingToneDoubled  = 0x0200,   //the leading tone is doubled
    lm_eFifthDoubled        = 0x0400,   //the fifth is doubled
    lm_eResultantFifthOctves = 0x0800,  //3. No hacer 5ªs ni 8ªs resultantes, excepto:
                                            //> a) la soprano se ha movido por segundas
                                            //> b) (para 5ªs) uno de los sonidos ya estaba
    lm_eFifthOctaveMotion   = 0x1000,   //parallel motion of perfect octaves or perfect fifths
    lm_eFifthMissing        = 0x2000,   // Acorde completo. Contiene todas las notas (en todo caso, elidir la 5ª)
    lm_eNotAllNotes         = 0x4000,   //not all chord steps in the chord
    lm_eChordDiscarded      = 0x8000,   //Chord discarded: not possible to generate a valid next chord

    lm_eMaxSeverity         = 0x8888,
};

class lmHChord
{
public:

    lmHChord() { 
        nNote[0] = nNote[1] = nNote[2] = nNote[3] = -1;
        nAcc[0] = nAcc[1] = nAcc[2] = nAcc[3] = 0;
        nReason=lm_eChordValid;
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
        sAnswer += DPitch_GetEnglishNoteName(nNote[iNote]);
        return sAnswer;
    }


    lmDPitch            nNote[4];       //diatonic pitch so 1:1 mapping to staff lines/spaces
    int                 nAcc[4];        //accidentals: -2 ... +2
    lmBadChordReason    nReason;        //used when filtering out invalid chords
    lmChordError        nSeverity;      //broken rules
    int                 nImpact;        //0-internal voices, 1-external voices
    int                 nNumNotes;      //normally 4
};

typedef struct lmChordAuxDataStruct {
	int nSteps[4];	// the steps of the chord
    int nStep5;     // the fifth of the chord
	int nNumSteps;	// how many steps this chord has (3 or 4)
} lmChordAuxData;

//declare global functions defined in this module
extern wxString CadenceTypeToName(lmECadenceType nType);
extern wxString GetChordErrorDescription(lmChordError nError);

//A cadence is a sequence of up to 2 chords
//Change this for more chords in a cadence
#define lmCHORDS_IN_CADENCE  2

class lmCadence
{
public:
    lmCadence();
    ~lmCadence();

    bool Create(lmECadenceType nCadenceType, lmEKeySignatures nKey, bool fUseGrandStaff);
    bool IsCreated() { return m_fCreated; }

    lmECadenceType GetCadenceType() { return m_nType; }
    int GetNumChords() { return m_nNumChords; }
    lmChordManager* GetChord(int iC);
	wxString GetName();
    wxString GetNotePattern(int iChord, int iNote);
    lmChordManager* GetTonicChord();


private:
    wxString SelectChord(wxString sFunction, lmEKeySignatures nKey, int* pInversion);
    wxString GetRootNote(wxString sFunct, lmEKeySignatures nKey, lmEClefType nClef,
                         bool fUseGrandStaff);

    int GenerateFirstChord(std::vector<lmHChord>& aChords, lmChordAuxData& tChordData,
						   lmChordManager* pChord, int nInversion);
    int GenerateNextChord(std::vector<lmHChord>& aChords, lmChordAuxData& tChordData,
						  lmChordManager* pChord, int nInversion, int iPrevHChord);
    int FilterChords(std::vector<lmHChord>& aChords, int nNumChords, lmChordAuxData& tChordData,
                     lmHChord* pPrevChord, bool fExhaustive=false);
    void SelectLessBad(std::vector<lmHChord>& aChords, lmChordAuxData& tChordData,
					   int iHChord);
    int GenerateSopranoNote(lmNoteBits oChordNotes[4], int iBass, int nNumNotes);


	// Debug methods
	void Debug_DumpAllChords(std::vector<lmHChord>& aChords);
	void Debug_DumpChord(lmHChord& oChord, int iChord=0);



    //member variables

    bool            m_fCreated;
    lmECadenceType  m_nType;
	int				m_nImperfectCad;		
    lmEKeySignatures  m_nKey;
    bool            m_fTonicCreated;
    lmChordManager  m_oTonicChord;
    lmChordManager  m_aChord[lmCHORDS_IN_CADENCE];
    int             m_nInversions[lmCHORDS_IN_CADENCE];
    int             m_nNumChords;       //num of chords in this cadence
    lmHChord        m_Chord[lmCHORDS_IN_CADENCE];

};

#endif  // __LM_CADENCE_H__

