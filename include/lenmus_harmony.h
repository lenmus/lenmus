//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2010-2015 LenMus project
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
//---------------------------------------------------------------------------------------

//#ifndef __LENMUS_THEOHARMONYTOOLSET_H__        //to avoid nested includes
//#define __LENMUS_THEOHARMONYTOOLSET_H__
//
//#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
//#pragma interface "Harmony.cpp"
//#endif
//
//#include "lenmus_standard_header.h"
//
//#include "../auxmusic/Chord.h"
//
////
//// Chord harmony types and classes
////
//
////
//// GLOBAL AUX FUNCTIONS
////
//
//
//namespace lenmus
//{
//
//// return
////  -1: negative, 0, 1: positive
//extern int GetHarmonicDirection(int nInterval);
//extern void DrawArrow(ImoNote* pNote1, ImoNote* pNote2, wxColour color);
////returns interval number ignoring octaves: 1=unison, 2=2nd, ..., 8=8ve
//extern int GetIntervalNumberFromFPitchDistance(FPitch n2, FPitch n1);
//extern void SortChordNotes( int numNotes, ImoNote** inpChordNotes);
//extern void SortChordNotes(int nNumNotes, FPitch fInpChordNotes[]);
//extern FIntval FPitchInterval(int nRootStep, EKeySignature nKey, int nIncrementSteps);
//// todo: move this to "Pitch" file o  merge this with FPitch_ToAbsLDPName
//// This is just FPitch_ToAbsLDPName but WITHOUT OCTAVE
//extern wxString NormalizedFPitch_ToAbsLDPName(FPitch fp);
//extern wxString GetChordDegreeString(StepType nStep);
//extern wxString Get4VoiceName(int nVoice);
//
//
//
//
//enum lmHarmonicMovementType {
//    lm_eDirectMovement ,    // 2 voices with the same delta sign (cero included)
//    lm_eObliqueMovement ,   // one delta sign is 0, the other not
//    lm_eContraryMovement    // 2 voices with contrary delta sign (cero not included)
//};
//extern int GetHarmonicMovementType( FPitch fVoice10, FPitch fVoice11, FPitch fVoice20, FPitch fVoice21);
//
//
////--------------------------------------------------------------------------
//// A list of notes
////   with individual absolute end time
////   with global absolute current time
////--------------------------------------------------------------------------
//typedef struct lmActiveNoteInfoStruct {
//    ImoNote*  pNote;
//    TimeUnits    rEndTime;
//    lmActiveNoteInfoStruct(ImoNote* pNoteS, TimeUnits rEndTimeS)
//    {
//        pNote = pNoteS;
//        rEndTime = rEndTimeS;
//    }
//} lmActiveNoteInfo;
//
//class lmActiveNotes
//{
//public:
//    lmActiveNotes();
//    ~lmActiveNotes();
//
//    void SetTime(TimeUnits rNewCurrentTime);
//    inline TimeUnits GetTime() { return m_rCurrentTime; };
//    int GetNotes(ImoNote** pNotes);
//    void AddNote(ImoNote* pNote, TimeUnits rEndTime);
//    void RecalculateActiveNotes();
//    int  GetNumActiveNotes();
//
//    // For debugging
//    wxString ToString();
//
//protected:
//    void ResetNotes();
//
//    TimeUnits                           m_rCurrentTime;
//    std::list<lmActiveNoteInfo*>    m_ActiveNotesInfo;
//};
//
//
//// To store all the possible errors resulting from chord analysis
//// Error <--> broken rule
//// Basically it is a compressed list of rule identifiers. Each rule is a bit
//typedef struct lmChordErrorStruct
//{
//    unsigned int nErrList;
//    bool IncludesError(int nBrokenRule);  // arg: lmChordValidationRules
//    void SetError(int nBrokenRule, bool fVal); // arg1: lmChordValidationRules
//} lmChordError;
//
//
//#define lmMAX_NUM_CHORDS 50
//
////
//// lmFPitchChord is a Chord with notes in FPitch
//// lmScoreChord is a Chord with notes in FPitch and in ImoNote
////
//// we implement a very simple RTTI (run-time type information)
////
//// lmScoreChord inherits from lmFPitchChord
////
//// lmRule::Evaluate must be able to work with chord descriptors of both kinds
////   lmScoreChord: when the results draw thing areound the notes (message box, arrows, etc)
////   lmFPitchChord: whne the results do not draw anything; just want to know the result of the rule over the chord
////
//// lmScoreChord must be the type used to access notes from lmRule::Evaluate
////
////
//
//// Chord is an "abstract" chord: defined by intervals.
////   Chord: Number of notes = number of intervals +1
//// lmFPitchChord is a "real" chord: it contains a set of actual notes in FPitch
////   lmNChord: Number of notes can be ANY; IT ALLOWS DUPLICATED NOTES.
//// lmScoreChord: lmFPitchChord with notes of in ImoNote
//// TODO: move this class to Chord.cpp?
//class lmFPitchChord: public Chord
//{
//public:
//    //  Constructors from notes
//    //     (the notes can not be added afterwards)
//    //build a chord from a list of ordered notes
//    lmFPitchChord(int nNumNotes, FPitch fNotes[], EKeySignature nKey = k_key_C);
//    lmFPitchChord(int nNumNotes, ImoNote** pNotes, EKeySignature nKey = k_key_C);
//    //  Constructors without notes
//    //      (the notes can be added afterwards)
//    //     build a chord from "essential" information
//    lmFPitchChord(int nDegree, EKeySignature nKey, int nNumIntervals, int nNumInversions, int octave);
//
// /* TODO:  possibly helpful // Creates a chord from an unordered list of ordered score notes
//    lmFPitchChord(EKeySignature nKey, lmActiveNotes* pActiveNotesList); --*/
//
//    virtual ~lmFPitchChord(){};
//
//    int get_num_notes() {return m_nNumChordNotes;}
//
//    wxString ToString();
//
//    FPitch GetNoteFpitch(int nIndex) {return m_fpChordNotes[nIndex];} ;
//    // GetVoice should not be used for this class since it has no lmNotes (just FPitch)
//    //   but just in case, it can be emulated, since the notes are ordered
//    //   note 0 -> voice 4
//    //.. note 3 -> voice 1
//    int GetNoteVoice(int nNoteIndex)
//    {
//        assert(nNoteIndex<m_nNumChordNotes);
//        return m_nNumChordNotes-nNoteIndex;
//    };
//
//    // aware: to be used only after using constructor without notes
//    // return the number of notes
//    int AddNoteLmFPitch(FPitch fNote);
//
//    void RemoveAllNotes(); // todo: not necessary, remove?
//
//    bool IsBassDuplicated();
// // todo: consider to implement:   int CreateRandomNotes(int nNumNotes);
//
//protected:
//    int m_nNumChordNotes;
//    FPitch m_fpChordNotes[k_notes_in_chord];
//    bool m_fCreatedWithNotes;
//};
//
//class lmScoreChord: public lmFPitchChord
//{
//public:
//    //build a chord from a list of score note pointers
//    lmScoreChord(int nNumNotes, ImoNote** pNotes, EKeySignature nKey = k_key_C);
//    //  Constructors without notes
//    //      (the notes can be added afterwards)
//    //     build a chord from "essential" information
//    lmScoreChord(int nDegree, EKeySignature nKey, int nNumIntervals, int nNumInversions, int octave);
//
//    virtual ~lmScoreChord() {};  // TODO: review virtual destructors (needed if ancestors destructors must be called)
//
//    void RemoveAllNotes(); // todo: no necessary. Remove?
//
//    bool  HasLmNotes() {return m_nNumLmNotes > 0 && m_nNumLmNotes == m_nNumChordNotes;}
//
//    // aware: this is only to associate the score note (ImoNote) to a note in FPitch that already exists
//    //   it is not to add a note!
//    bool SetLmNote(ImoNote* pNote);
//
//    ImoNote* GetNoteLmNote(int nIndex);
//    int GetNoteVoice(int nIndex);
//    int GetNumLmNotes();
//    wxString ToString();
//
//    lmChordError  tChordErrors;
//private:
//    int m_nNumLmNotes;
//
//    ImoNote* m_pChordNotes[k_notes_in_chord];
//};
//
//
//
//
//
////
//// Message box to display the results if the chord analysis
////
//
//// Remember:
////      x: relative to object; positive: right
////      y: relative to top line; positive: down
//class ChordInfoBox
//{
//public:
//    ChordInfoBox(wxSize* pSize, lmFontInfo* pFontInfo
//        , int nBoxX, int nBoxY, int nLineX, int nLineY, int nBoxYIncrement);
//    ~ChordInfoBox() {};
//
//    void Settings(wxSize* pSize, lmFontInfo* pFontInfo
//        , int nBoxX, int nBoxY, int nLineX, int nLineY, int nBoxYIncrement);
//    void DisplayChordInfo(lmScore* pScore, lmScoreChord* pChordDsct, wxColour colour, wxString &sText);
//    void ResetPosition();
//    void SetYPosition(int nYpos);
//
//protected:
//
// // Const values
// int m_ntConstBoxXstart;
// int m_ntConstLineXstart;
// int m_ntConstLineYStart;
//
// // Variable values: only Box Y position: incremented in each use
// int m_ntConstInitialBoxYStart;
// int m_ntConstBoxYIncrement;
// int m_ntCurrentBoxYStart;
//
// lmFontInfo* m_pFontInfo;
// wxSize* m_pSize;
//};
//
//
//
//
//
//
//enum  lmChordValidationRules
//{
//    lmCVR_ChordHasAllSteps,  // The chord is complete (has all note steps)
//    lmCVR_FirstChordValidationRule = lmCVR_ChordHasAllSteps,
//    lmCVR_ChordHasAllNotes,  // lm_eFifthMissing - Acorde completo. Contiene todas las notas (en todo caso, elidir la 5ª)
//    lmCVR_NoParallelMotion, //  No parallel motion of perfect octaves, perfect fifths, and unisons
//    lmCVR_NoResultingFifthOctaves,
//    // lm_eResultantFifthOctves - No hacer 5ªs ni 8ªs resultantes, excepto:
//                                            //> a) la soprano se ha movido por segundas
//                                            //> b) (para 5ªs) uno de los sonidos ya estaba
//    lmCVR_NoFifthDoubled, // The fifth is not doubled
//	lmCVR_NoLeadingToneDoubled, // The leading tone is never doubled
//    lmCVR_LeadingToneResolveToTonic, // Scale degree seven (the leading tone) should resolve to tonic.
//    lmCVR_SeventhResolution, // m_eSeventhResolution - the seventh of a chord should always resolve down by second.
//    lmCVR_NoIntervalHigherThanOctave, // voices interval not greater than one octave (except bass-tenor)
//    lmCVR_NoVoicesCrossing, // Do not allow voices crossing. No duplicates
//    lmCVR_NoVoicesOverlap, // Voice overlap: when a voice moves above or below a pitch previously sounded by another voice.
//    lmCVR_ChromaticAlterationsDirection, // Resolve chromatic alterations by step in the same direction than the alteration.
//    lmCVR_NoIntervalHigherThanSixth, // lm_eGreaterThanSixth - No es conveniente exceder el intervalo de sexta, exceptuando la octava justa
//    lmCVR_BassMovementByStep, // If bass moves by step all other voices moves in opposite direction to bass
//    lmCVR_ThirdDoubledInBrokenCadence, // lm_eNotDoubledThird - Cuando el bajo enlaza el V grado con el VI (cadencia rota), en el acorde de VI grado se duplica la tercera.
//    lmCVR_LastChordValidationRule = lmCVR_ThirdDoubledInBrokenCadence,
//
//};
//
//
////
//// Base virtual class of rules
////
//class lmRule
//{
//public:
//  ///  wxDECLARE_ABSTRACT_CLASS(lmRule); //@@ TODO: aclarar ¿necesario?
//
//    lmRule(int nRuleID);
//    virtual ~lmRule(){};
//    virtual int Evaluate(wxString& sResultDetails, int pNumFailuresInChord[], ChordInfoBox* pBox )=0;
//    void SetChordDescriptor(lmScoreChord** pChD, int nNumChords)
//    {
//        m_pChordDescriptor = pChD;
//        m_nNumChords = nNumChords;
//    };
//    bool IsEnabled(){ return m_fEnabled; };
//    void Enable( bool fVal ){ m_fEnabled = fVal; };
//    wxString GetDescription() { return m_sDescription;};
//    void SetDescription(const wxString& sDescription) { m_sDescription = sDescription;};
//    int GetRuleId() { return m_nRuleId;};
//protected:
//    int m_nNumChords;
//    lmScoreChord** m_pChordDescriptor; // array of chord pointers
//    bool m_fEnabled;
//    wxString m_sDescription;
//    wxString m_sDetails;
//    int m_nRuleId;
//};
//
////
//// The list of rules
////
//// possibly useful: typedef std::map<int, lmRule*> lmRuleMapType;
//class lmRuleList
//{
//public:
//    lmRuleList(lmScoreChord** pChD, int nNumChords);
//    ~lmRuleList();
//
//    bool AddRule(lmRule* pNewRule, const wxString& sDescription );  // return: ok
//    bool DeleteRule(int nRuleId);  // arg: lmChordValidationRules; return: ok
//    lmRule* GetRule(int nRuleId);  // arg: lmChordValidationRules;
//    void SetChordDescriptor(lmScoreChord** pChD, int nNumChords);
//
//protected:
//    void CreateRules();
//    std::map<int, lmRule*> m_Rules;
//};
//
//
////
//// Derived classes of rules
////
//// TODO: improve this with a template...
//// Aware: text describing the rule must be set dynamically, since it
////  hast to be translated and therefore it requieres _("") instead of the static ""
//#define LM_CREATE_CHORD_RULE(classname, id) \
//class classname : public lmRule  \
//{ \
//public: \
//    classname() :lmRule(id) {}; \
//    int Evaluate(wxString& sResultDetails, int pNumFailuresInChord[], ChordInfoBox* pBox); \
//};
//
////
//// GLOBAL AUX FUNCTIONS
////
//
//extern void  HDisplayChordInfo(lmScore* pScore, lmScoreChord*  pChordDsct
//                                           , wxColour colour, wxString &sText, bool reset);
//// Analyze a progress (link) errors in a sequence o chords
//extern int AnalyzeHarmonicProgression(lmScoreChord** pChordDescriptor, int nNCH, ChordInfoBox* pChordErrorBox = 0);
//
//
//}   //namespace lenmus
//
//#endif
