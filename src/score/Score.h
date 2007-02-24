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
/*! @file Score.h
    @brief Header file for class lmScore
    @ingroup score_kernel
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __SCORE_H__        //to avoid nested includes
#define __SCORE_H__

#include "wx/debug.h"
#include "defs.h"
#include "../app/global.h"

enum EClefType
{
    eclvUndefined = 0,
    eclvSol,
    eclvFa4,
    eclvFa3,
    eclvDo1,
    eclvDo2,
    eclvDo3,
    eclvDo4,
    eclvPercussion,
    // other clefs not available for exercises
    eclvDo5,
    eclvFa5,
    eclvSol1,
    eclv8Sol,       //8 above
    eclvSol8,       //8 below
    eclv8Fa,        //8 above
    eclvFa8,        //8 below

};
// @attention enum constats EClefType are going to be ramdomly generated in object
// Generators. The next constants defines de range.
#define lmMIN_CLEF        eclvSol
#define lmMAX_CLEF        eclvPercussion
// @attention enum constats EClefType are going to be used as indexes in ClefConstrains


enum EStemType
{
    eDefaultStem = 0,   //default: as decided by program
    eStemUp,            //up: force stem up
    eStemDown,          //down: force stem down
    eStemNone,          //none: force no stem
    eStemDouble         //double: force doble line. Direction as decided by program
};

// Beaming: type of beaming
enum EBeamType {
    eBeamNone = 0, 
    eBeamBegin, 
    eBeamContinue, 
    eBeamEnd, 
    eBeamForward, 
    eBeamBackward
};



//-----------------------------------------------------------------------------------------------
/*!
This definition of note type has the following properties:
@verbatim
   a)  2 ^ (NoteType - 2)   is the number used as divider in the American English name
                            of the note. Examples:
                                crochet (quarter) = 2^(4-2) = 4
                                quaver (eighth) = 2^(5-2) = 8
   b)  2 ^ (10 - NoteType)  is the note's duration, relative to the shortest note (256th).
                            So the longest one (longa) last 1024 units and
                            the shortest one (256th, semigarrapatea) last 1 unit.
@endverbatim
*/
enum ENoteType
{
    eLonga = 0,     // es: longa            en-UK: longa                    en-USA: long
    eBreve,         // es: breve, cuadrada  en-UK: breve                    en-USA: double whole
    eWhole,         // es: redonda,         en-UK: semibreve                en_USA: whole
    eHalf,          // es: blanca,          en-UK: minim                    en_USA: half
    eQuarter,       // es: negra,           en-UK: crochet                  en_USA: quarter
    eEighth,        // es: corchea,         en-UK: quaver                   en_USA: eighth
    e16th,          // es: semicorchea,     en-UK: semiquaver               en_USA: 16th
    e32th,          // es: fusa,            en-UK: demisemiquaver           en_USA: 32nd
    e64th,          // es: semifusa,        en-UK: hemidemisemiquaver       en_USA: 64th
    e128th,         // es: garrapatea       en-UK: semihemidemisemiquaver   en_USA: 128th
    e256th          // es: semigarrapatea   en-UK: ?                        en_USA: 256th
};

// to facilitate access to standard notes' duration. 
enum ENoteDuration
{
    eLongaDuration = 1024,
    eBreveDottedDuration = 768,
    eBreveDuration = 512,
    eWholeDottedDuration = 384,
    eWholeDuration = 256,
    eHalfDottedDuration = 192,
    eHalfDuration = 128,
    eQuarterDottedDuration = 96,
    eQuarterDuration = 64,
    eEighthDottedDuration = 48,
    eEighthDuration = 32,
    e16hDottedDuration = 24,
    e16thDuration = 16,
    e32thDottedDuration = 12,
    e32thDuration = 8,
    e64thDottedDuration = 6,
    e64thDuration = 4,
    e128thDottedDuration = 3,
    e128thDuration = 2,
    e256thDuration = 1
};



//! accidental signs
enum EAccidentals
{
    eNoAccidentals = 0,        
    eNatural,               // es: becuadro
    eFlat,                  // es: bemol
    eSharp,                 // es: sostenido
    eFlatFlat,              // es: doble bemol
    eDoubleSharp,           // es: doble sostenido (single sign)
    eSharpSharp,            // es: doble sostenido (two sharp signs)
    eNaturalFlat,           // es: becuadro bemol
    eNaturalSharp,          // es: becuadro sostenido
    eQuarterFlat,
    eQuarterSharp,
    eThreeQuartersFlat,
    eThreeQuartersSharp
};

//! key signatures
enum EKeySignatures
{
    earmDo = 0,
    earmSol,
    earmRe,
    earmLa,
    earmMi,
    earmSi,
    earmFas,
    earmDos,
    earmDob,
    earmSolb,
    earmReb,
    earmLab,
    earmMib,
    earmSib,
    earmFa,
    //@attention minor keys must go after major keys. This is exploited in IsMajor() global
    //function
    earmLam,
    earmMim,
    earmSim,
    earmFasm,
    earmDosm,
    earmSolsm,
    earmResm,
    earmLasm,
    earmLabm,
    earmMibm,
    earmSibm,
    earmFam,
    earmDom,
    earmSolm,
    earmRem
};
// @attention enum constats EKeySignatures are going to be ramdomly generated in object
// Generators. The next constant defines de maximum and minimum values.
#define lmMIN_KEY  earmDo
#define lmMAX_KEY  earmRem
#define lmNUM_KEYS lmMAX_KEY - lmMIN_KEY + 1
// @attention names for key signatures are defined in object lmKeySignature. There is an
// array, named sKeySignatureName, and the key signatures names are defined 
// assuming a predefined order in the enum EKeySignatures
// @attention items 'earmDo' to 'earmFa' are used as indexes in DlgCfgEarIntervals. 
//  'earmDo' must be 0 and consecutive orden must be kept.


//! TimeSignatureType indicates the signature encoding being used
enum ETimeSignatureType
{
    eTS_Normal = 1,        // it is a single fraction
    eTS_Common,            // it is 4/4 but represented by a C symbol
    eTS_Cut,            // it is 2/4 but represented by a C/ simbol
    eTS_SingleNumber,    // it is a single number with an implied denominator
    eTS_Multiple,        // multiple fractions, i.e.: 2/4 + 3/8
    eTS_Composite,        // composite fraction, i.e.: 3+2/8
    eTS_SenzaMisura        // no time signature is present
};

enum ETimeSignature
{
    emtr24 = 1,  //  2/4
    emtr34,      //  3/4
    emtr44,      //  4/4
    emtr68,      //  6/8
    emtr98,      //  9/8
    emtr128,     // 12/8
    emtr28,      //  2/8
    emtr38,      //  3/8
    emtr22,      //  2/2
    emtr32,      //  3/2
};
// @attention enum constats ETimeSignature are going to be ramdomly generated in object
// Generators. The next constant defines de maximum and minimum values.
#define lmMIN_TIME_SIGN  emtr24
#define lmMAX_TIME_SIGN  emtr32


enum ETies
{
    eL_NotTied = 0,
    eL_Tied
};

enum ECalderon
{
    eC_SinCalderon = 0,
    eC_ConCalderon
};

//noteheads
enum ENoteHeads
{
    enh_Longa = 1,
    enh_Breve,
    enh_Whole,              //Whole note (redonda)
    enh_Half,               //Half note (blanca)
    enh_Quarter,            //Quarter note (negra)
    enh_Cross               //Cross (for percussion) (aspa)
};

// barlines
enum EBarline
{
    etb_SimpleBarline = 1,          //thin line
    etb_DoubleBarline,              //two thin lines
    etb_EndBarline,                 //thin-thick lines
    etb_StartRepetitionBarline,     //thick-thin-two dots
    etb_EndRepetitionBarline,       //two dots-thin-thick
    etb_StartBarline,               //thick-thin
    etb_DoubleRepetitionBarline     //two dots-thin-thin-two dots
};

// aligments
enum lmEAlignment
{
    lmALIGN_DEFAULT = 0,
    lmALIGN_LEFT,
    lmALIGN_RIGHT,
    lmALIGN_CENTER
};


//Play modes: instrument to use to play a score
enum EPlayMode
{
    ePM_NormalInstrument = 1,        //play using normal instrument
    ePM_RhythmInstrument,
    ePM_RhythmPercussion,
    ePM_RhythmHumanVoice
};

//Highlight when playing a score
enum EHighlightType
{
    eVisualOff = 0,
    eVisualOn,
    eRemoveAllHighlight,
    ePrepareForHighlight,
};

/*  Renderization options
    ---------------------
    eRenderJustified
        Render a score justifying measures so that they fit exactly in the width of the 
        staff

    eRenderSimple
        Render a score without bar justification and without breaking it into systems.
        That is, it draws all the score in a single system without taking into consideration
        paper length limitations.
        This very simple renderer is usefull for simple scores and in some rare occations
*/
enum ERenderizationType
{
    eRenderJustified = 1,
    eRenderSimple
};

///*  Spacing methods for rendering scores  (obsolete?)
//    -------------------------------------
//    Two basic methods:
//    1. Fixed: the spacing between notes is constant, independently of note duration.
//    2. Proportional: the spacing is adjusted so that note position is proportional to time.
//
//    In the proportional method several alternatives are posible:
//    1. ProportionalConstant: the proportion factor between time and space is fixed. Two alternative
//        methods for fixing this factor:
//        a) Fixed: is given by the vaule of a parameter
//        b) ShortNote: is computed for the shorter note in the score
//    2. ProportionalVariable: the proportion factor is computed for each bar. Two alternatives:
//        a) ShortNote: is computed for the shorter note in the bar
//        b) NumBars: Computed so taht the number of bars in the system is a predefined number
//*/
//enum ESpacingMethod
//{
//    esm_Fixed = 1,                    //used in RenderSimple
//    esm_PropConstantFixed,            //Used in FTeoria
//    esm_PropConstantShortNote,        //Default method in lmFormatter.RenderScore(). Not used
//    esm_PropVariableShortNote,        //Not used/implemented
//    esm_PropVariableNumBars           //Not used/implemented
//};


// font specification
enum lmETextStyle           // text styles
{
    lmTEXT_DEFAULT = 0,
    lmTEXT_NORMAL,
    lmTEXT_BOLD,
    lmTEXT_ITALIC,
    lmTEXT_ITALIC_BOLD,
};

typedef struct lmFontInfoStruct {
    wxString sFontName;
    int nFontSize;
    lmETextStyle nStyle;
} lmFontInfo;

    //global variables used as default initializators
extern lmFontInfo tLyricDefaultFont;            // defined in NoteRestObj.cpp
extern lmFontInfo g_tInstrumentDefaultFont;       // defined in Instrument.cpp
extern lmFontInfo tBasicTextDefaultFont;        // defined in NoteRestObj.cpp


//Location source data
enum lmELocationType
{
    lmLOCATION_RELATIVE = 0,
    lmLOCATION_ABSOLUTE,
    lmLOCATION_DEFAULT
};

typedef struct lmLocationStruct {
    int x;
    int y;
    lmELocationType xType;
    lmELocationType yType;
    lmEUnits xUnits;
    lmEUnits yUnits;
} lmLocation;

    //global variables used as default initializators
extern lmLocation g_tDefaultPos;          // defined in NoteRestObj.cpp


//Constants
#define XML_DURATION_TO_LDP  64        //factor to convert between LDP duration and MusicXML duration

//options for Play() method
#define lmVISUAL_TRACKING           true        //highligth notes on the score as they are played
#define lmNO_VISUAL_TRACKING        false
#define NO_MARCAR_COMPAS_PREVIO     false


// forward declarations
class lmPaper;
class lmScore;
class lmVStaff;
class lmInstrument;
class lmStaffObj;
class lmColStaffObjs;
class lmStaffObjIterator;
class InstrumentsList;
class VStavesList;
class StaffObjsList;

class lmBasicText;
class lmScoreText;
class lmWordsDirection;
class lmSOControl;
class lmClef;
class lmTimeSignature;
class lmKeySignature;
class lmBarline;
class lmNoteRest;
class lmBeam;
class lmTupletBracket;
class NotesList;
class lmChord;
struct lmTBeamInfo;
class lmNote;
class lmRest;

class lmStaff;
class StaffList;
class lmContext;
class ContextList;
class lmSoundManager;
class lmObjOptions;


#include "Context.h"
#include "StaffObj.h"
#include "StaffObjIterator.h"
#include "ColStaffObjs.h"
#include "SOControl.h"
#include "Text.h"
#include "NoteRestObj.h"
#include "Accidental.h"
#include "Staff.h"
#include "VStaff.h"
#include "Instrument.h"
#include "Slur.h"            // Slurs and Ties
#include "Direction.h"
#include "Barline.h"
#include "Clef.h"
#include "TimeSignature.h"
#include "KeySignature.h"
#include "NoteRest.h"
#include "Beam.h"
#include "TupletBracket.h"
#include "Note.h"
#include "Rest.h"
#include "Chord.h"
#include "../app/global.h"
#include "../app/Paper.h"
#include "../sound/SoundManager.h"



// global unique variables used during score building
// TODO: Replace for lmScore/lmNote member funtions 
extern lmNoteRest* g_pLastNoteRest;
extern lmBeam* g_pCurBeam;

class lmScore : public lmObject
{
public:
    //ctor and dtor
    lmScore();
    ~lmScore();

    lmInstrument* AddInstrument(int nVStaves, int nMIDIChannel, int nMIDIInstr,
                                wxString sName, wxString sAbbrev=_T(""));
    lmInstrument* AddInstrument(int nVStaves, int nMIDIChannel, int nMIDIInstr,
                                lmScoreText* pName, lmScoreText* pAbbrev);

    lmVStaff* GetVStaff(int nInstr, int nVStaff=1);
    int GetNumMeasures();

    // play methods
    void Play(bool fVisualTracking = lmNO_VISUAL_TRACKING, 
              bool fMarcarCompasPrevio = NO_MARCAR_COMPAS_PREVIO,
              EPlayMode nPlayMode = ePM_NormalInstrument,
              long nMM = 0, 
              wxWindow* pWindow = (wxWindow*)NULL );
    void PlayMeasure(int nMeasure,
                     bool fVisualTracking = lmNO_VISUAL_TRACKING, 
                     EPlayMode nPlayMode = ePM_NormalInstrument,
                     long nMM = 0,
                     wxWindow* pWindow = (wxWindow*)NULL );
    void Pause();
    void Stop();
    void WaitForTermination();


    // serving highlight events
    void ScoreHighlight(lmStaffObj* pSO, lmPaper* pPaper, EHighlightType nHighlightType);

    lmScoreObj* FindSelectableObject(lmUPoint& pt);

    // Debug methods. If filename provided writes also to file
    wxString Dump(wxString sFilename = _T(""));
    wxString SourceLDP(wxString sFilename = _T(""));
    wxString SourceXML(wxString sFilename = _T(""));
    wxString DumpMidiEvents(wxString sFilename = _T(""));

    int GetNumInstruments() { return (int)m_cInstruments.GetCount(); }

    //// Friend methods for lmFormatter object

    //iterator over instruments list
    lmInstrument* GetFirstInstrument();
    lmInstrument* GetNextInstrument();
    lmInstrument* GetLastInstrument();

    // titles related methods
    void AddTitle(wxString sTitle, lmEAlignment nAlign, lmLocation pos,
                  wxString sFontName, int nFontSize, lmETextStyle nStyle);
    void WriteTitles(bool fMeasuring, lmPaper *pPaper);

    // identification
    wxString GetScoreName();
    void SetScoreName(wxString sName);

    // methods related to MusicXML import/export
    lmInstrument* XML_FindInstrument(wxString sId);

    //layout related methods
    lmLUnits TopSystemDistance() { return m_nTopSystemDistance + m_nHeadersHeight; }
    void SetTopSystemDistance(lmLUnits nDistance) { m_nTopSystemDistance = nDistance; }

    //global lmStaffObj list related methods
    void IncludeInGlobalList(lmStaffObj* pSO);
    void RemoveFromGlobalList(lmStaffObj* pSO);

    //renderization options
    void SetRenderizationType(ERenderizationType nType) { m_nRenderizationType = nType; }
    //SetSpacingMethod(ESpacingMethod nMethod) { m_nSpacingMethod = nMethod; }
        // accessors for lmFormatter only
    ERenderizationType GetRenderizationType() const { return m_nRenderizationType; }
    //ESpacingMethod GetSpacingMethod() const { return m_nSpacingMethod; }


    //other methods
    long GetID() { return m_nID; }


private:
    void WriteToFile(wxString sFilename, wxString sContent);
    void ComputeMidiEvents();
    void RemoveHighlight(lmStaffObj* pSO, lmPaper* pPaper);
    lmLUnits MeasureTitle(lmPaper *pPaper, lmScoreText* pTitle, lmLUnits nPrevTitleHeight);


        //
        // member variables
        //

    // a lmScore is, mainly, a collection of Instruments plus some data (composer, title, ...)
    InstrumentsList     m_cInstruments;     //list of instruments that form this score
    StaffObjsList       m_cTitles;          //list of score titles

    //Variables related to polyphonic interpretation
    lmSoundManager*     m_pSoundMngr;       //Sound events table & manager
    StaffObjsList       m_cHighlighted;     //list of highlighted staffobjs

    //Layout related variables
    lmLUnits        m_nTopSystemDistance;
    lmLUnits        m_nHeadersHeight;

    //renderization options
    ERenderizationType  m_nRenderizationType;

    //other variables
    wxInstrumentsListNode*  m_pNode;        //last returned instrument node
    StaffObjsList   m_cGlobalStaffobjs;     //list of other StaffObjs not included in an lmVStaff
    long            m_nID;                  //unique ID for this score
    wxString        m_sScoreName;           //for user identification

};



#endif    // __SCORE_H__
