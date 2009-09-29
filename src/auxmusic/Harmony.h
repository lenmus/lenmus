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

#ifndef __LM_THEOHARMONYTOOLSET_H__        //to avoid nested includes
#define __LM_THEOHARMONYTOOLSET_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Harmony.cpp"
#endif

// aware: if included <list> before <map>: syntax error !  (MS bug?)
#include <map>  
#include <list>

#include "../auxmusic/Chord.h"

//
// Chord harmony types and classes
//


// To store all the possible errors resulting from chord analysis
// Error <--> broken rule
// Basically it is a compressed list of rule identifiers. Each rule is a bit
typedef struct lmChordErrorStruct
{
    unsigned int nErrList;
    bool IncludesError(int nBrokenRule);  // arg: lmChordValidationRules
    void SetError(int nBrokenRule, bool fVal); // arg1: lmChordValidationRules
} lmChordError;



typedef struct lmChordDescriptorStruct {
    lmChord*  pChord;
    lmNote* pChordNotes[lmNOTES_IN_CHORD-1];
    int nNumChordNotes;
    lmChordError  tChordErrors;

    // methods
    lmChordDescriptorStruct()
    {
        nNumChordNotes = 0;
        pChord = NULL;
        for (int i = 0; i<lmNOTES_IN_CHORD-1; i++)
        {
            pChordNotes[i] = NULL;
        }
        tChordErrors.nErrList = 0;
    }
    wxString ToString();
} lmChordDescriptor;
#define lmMAX_NUM_CHORDS 50






//--------------------------------------------------------------------------
// A list of notes 
//   with individual absolute end time
//   with global absolute current time
//--------------------------------------------------------------------------
typedef struct lmActiveNoteInfoStruct {
    lmNote*  pNote;
    float    rEndTime;
    lmActiveNoteInfoStruct(lmNote* pNoteS, float rEndTimeS)
    {
        pNote = pNoteS;
        rEndTime = rEndTimeS;
    }
} lmActiveNoteInfo;

class lmActiveNotes 
{
public:
    lmActiveNotes();
    ~lmActiveNotes();
    
    void SetTime(float rNewCurrentTime);
    inline float GetTime() { return m_rCurrentTime; };
    void GetChordDescriptor(lmChordDescriptor* ptChordDescriptor);
    void AddNote(lmNote* pNote, float rEndTime);
    void RecalculateActiveNotes();
    int  GetNumActiveNotes();

    // For debugging
    wxString ToString();

protected:
    void ResetNotes();

    float                           m_rCurrentTime;
    std::list<lmActiveNoteInfo*>    m_ActiveNotesInfo; 
};



//
// Message box to display the results if the chord analysis
// 
#include "../app/MainFrame.h"
extern lmMainFrame* GetMainFrame();
#include "../app/ScoreDoc.h"
//Error markup: the marked staffobj and its markup attachment
typedef std::pair<lmStaffObj*, lmAuxObj*> lmMarkup;
// Remember:
//      x: relative to object; positive: right
//      y: relative to top line; positive: down
class ChordInfoBox 
{
public:
    ChordInfoBox(wxSize* pSize, lmFontInfo* pFontInfo
        , int nBoxX, int nBoxY, int nLineX, int nLineY, int nBoxYIncrement);
    ~ChordInfoBox() {};
    
    void Settings(wxSize* pSize, lmFontInfo* pFontInfo
        , int nBoxX, int nBoxY, int nLineX, int nLineY, int nBoxYIncrement);
    void DisplayChordInfo(lmScore* pScore, lmChordDescriptor* pChordDsct, wxColour colour, wxString &sText);
    void ResetPosition();
    void SetYPosition(int nYpos);

protected:

 // Const values
 int m_ntConstBoxXstart;
 int m_ntConstLineXstart;
 int m_ntConstLineYStart;

 // Variable values: only Box Y position: incremented in each use
 int m_ntConstInitialBoxYStart;
 int m_ntConstBoxYIncrement;
 int m_ntCurrentBoxYStart;

 lmFontInfo* m_pFontInfo;
 wxSize* m_pSize;
};






enum  lmChordValidationRules 
{
    lmCVR_ChordHasAllSteps,  // The chord is complete (has all note steps)
    lmCVR_FirstChordValidationRule = lmCVR_ChordHasAllSteps,
    lmCVR_ChordHasAllNotes,  // lm_eFifthMissing - Acorde completo. Contiene todas las notas (en todo caso, elidir la 5ª)
    lmCVR_NoParallelMotion, //  No parallel motion of perfect octaves, perfect fifths, and unisons
    lmCVR_NoResultingFifthOctaves,
    // lm_eResultantFifthOctves - No hacer 5ªs ni 8ªs resultantes, excepto:
                                            //> a) la soprano se ha movido por segundas
                                            //> b) (para 5ªs) uno de los sonidos ya estaba
    lmCVR_NoFifthDoubled, // The fifth is not doubled
	lmCVR_NoLeadingToneDoubled, // The leading tone is never doubled
    lmCVR_LeadingToneResolveToTonic, // Scale degree seven (the leading tone) should resolve to tonic.
    lmCVR_SeventhResolution, // m_eSeventhResolution - the seventh of a chord should always resolve down by second.
    lmCVR_NoIntervalHigherThanOctave, // voices interval not greater than one octave (except bass-tenor)
    lmCVR_NoVoicesCrossing, // Do not allow voices crossing. No duplicates
    lmCVR_NoVoicesOverlap, // Voice overlap: when a voice moves above or below a pitch previously sounded by another voice.
    lmCVR_ChromaticAlterationsDirection, // Resolve chromatic alterations by step in the same direction than the alteration.
    lmCVR_NoIntervalHigherThanSixth, // lm_eGreaterThanSixth - No es conveniente exceder el intervalo de sexta, exceptuando la octava justa
    lmCVR_BassMovementByStep, // If bass moves by step all other voices moves in opposite direction to bass
    lmCVR_ThirdDoubledInBrokenCadence, // lm_eNotDoubledThird - Cuando el bajo enlaza el V grado con el VI (cadencia rota), en el acorde de VI grado se duplica la tercera.
    lmCVR_LastChordValidationRule = lmCVR_ThirdDoubledInBrokenCadence,

};


//
// Base virtual class of rules
// 
class lmRule 
{
public:
  ///  DECLARE_ABSTRACT_CLASS(lmRule) //@@ TODO: aclarar ¿necesario?

    lmRule(int nRuleID);
    virtual ~lmRule(){};
    virtual int Evaluate(wxString& sResultDetails, int pNumFailuresInChord[], ChordInfoBox* pBox )=0;
    void SetChordDescriptor(lmChordDescriptor* pChD, int nNumChords)
    {
        m_pChordDescriptor = pChD; 
        m_nNumChords = nNumChords;
    };
    bool IsEnabled(){ return m_fEnabled; };
    void Enable( bool fVal ){ m_fEnabled = fVal; };
    wxString GetDescription() { return m_sDescription;};
    void SetDescription(const wxString& sDescription) { m_sDescription = sDescription;};
    int GetRuleId() { return m_nRuleId;};
protected:
    int m_nNumChords;
    lmChordDescriptor* m_pChordDescriptor; // array of chords
    bool m_fEnabled;
    wxString m_sDescription;
    wxString m_sDetails;
    int m_nRuleId;
};

//
// The list of rules
// 
// possibly useful: typedef std::map<int, lmRule*> lmRuleMapType;
class lmRuleList 
{
public:
    lmRuleList(lmChordDescriptor* pChD, int nNumChords);
    ~lmRuleList();
    
    bool AddRule(lmRule* pNewRule, const wxString& sDescription );  // return: ok
    bool DeleteRule(int nRuleId);  // arg: lmChordValidationRules; return: ok
    lmRule* GetRule(int nRuleId);  // arg: lmChordValidationRules;
    void SetChordDescriptor(lmChordDescriptor* pChD, int nNumChords);

protected:
    void CreateRules();
    std::map<int, lmRule*> m_Rules; 
};


//
// Derived classes of rules
// 
// TODO: improve this with a template...
// Aware: text describing the rule must be set dynamically, since it 
//  hast to be translated and therefore it requieres _("") instead of the static _T("")
#define LM_CREATE_CHORD_RULE(classname, id) \
class classname : public lmRule  \
{ \
public: \
    classname() :lmRule(id) {}; \
    int Evaluate(wxString& sResultDetails, int pNumFailuresInChord[], ChordInfoBox* pBox); \
};



//
// GLOBAL AUX FUNCTIONS
//

// return
//  -1: negative, 0, 1: positive
extern int GetHarmonicDirection(int nInterval);
extern void  HDisplayChordInfo(lmScore* pScore, lmChordDescriptor*  pChordDsct
                                           , wxColour colour, wxString &sText, bool reset);
extern void DrawArrow(lmNote* pNote1, lmNote* pNote2, wxColour color); 
//returns interval number ignoring octaves: 1=unison, 2=2nd, ..., 8=8ve
extern int GetIntervalNumberFromFPitchDistance(lmFPitch n2, lmFPitch n1);

// TODO: global methods. They could probably be placed inside a class...
extern void SortChordNotes( int numNotes, lmNote** inpChordNotes);
extern int DoInversionsToChord( lmChordInfo* pInOutChordInfo, int nNumTotalInv);
extern lmFIntval FPitchInterval(int nRootStep, lmEKeySignatures nKey, int nIncrementSteps);

/* TODO: REMOVE THESE FUNCTIONS
extern void GetIntervalsFromNotes(int numNotes, lmNote** inpChordNotes, lmChordInfo* outChordInfo);
extern lmEChordType GetChordTypeFromIntervals(lmChordInfo& chordInfo, bool fAllowFifthElided=false );
extern bool TryChordCreation(int numNotes, lmNote** inpChordNotes, lmChordInfo* outChordInfo, wxString &outStatusStr);
*/

enum lmHarmonicMovementType {
    lm_eDirectMovement ,    // 2 voices with the same delta sign (cero included)
    lm_eObliqueMovement ,   // one delta sign is 0, the other not
    lm_eContraryMovement    // 2 voices with contrary delta sign (cero not included)
};
extern int GetHarmonicMovementType(  lmNote* pVoice10, lmNote* pVoice11, lmNote* pVoice20, lmNote* pVoice21);

#endif
