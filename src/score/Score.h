// RCS-ID: $Id: Score.h,v 1.6 2006/02/23 19:23:54 cecilios Exp $
//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
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

enum EScoreObjType
{
    eTPO_Clef = 1,            // clef                    (ESP: clave)
    eTPO_KeySignature,        // key signature        (ESP: armadura, tonalidad)
    eTPO_TimeSignature,        // time signature        (ESP: métrica)
        eTPO_GrafObj,
        eTPO_Symbol,            // graphical objects
    eTPO_Barline,            // barlines
        eTPO_Indicacion,
    eTPO_NoteRest,            // notes and rests
    eTPO_Text,
        eTPO_Repeticion,        //directivas de repetición: D.C., Segno, Al Segno, Fine, ...
    eTPO_Control,            //control element (backup, forward)
    eTPO_Tie,                //lmAuxObj: tie            (ESP: ligadura (de duración))
    eTPO_TupletBracket,        //lmAuxObj: tuplet bracket
    eTPO_WordsDirection
};

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
    eclvSinClave        //For percusion scores
};
// @attention enum constats EClefType are going to be ramdomly generated in object
// Generators. The next constant defines de range.
#define lmMIN_CLEF        eclvSol
#define lmMAX_CLEF        eclvSinClave
// @attention enum constats EClefType are going to be used as indexes in ClefConstrains


enum EStemType
{
    eDefaultStem = 0,    //default: as decided by program
    eStemUp,            //up: force stem up
    eStemDown,            //down: force stem down
    eStemNone,            //none: force no stem
    eStemDouble            //double: force doble line. Direction as decided by program
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
   a)  2 ^ (NoteType - 1)    is the number used as divider in the American English name
                            of the note. Examples:
                               crochet (quarter) = 2^(3-1) = 4
                               quaver (eighth) = 2^(4-1) = 8
   b)  2 ^ (9 - NoteType)    is the note's duration, relative to the shortest note (256th).
                            So the longest one (long, breve, cuadrada) last 512 units and
                            the shortest one (256th, semigarrapatea) last 1 unit.
@endverbatim
*/
enum ENoteType
{
    eLong = 0,      // es: cuadrada,        en-UK: breve                    en-USA: double whole
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
    eLongDottedDuration = 768,
    eLongDuration = 512,
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


enum ELigados
{
    eL_NoLigada = 0,
    eL_Ligada
};

enum ECalderon
{
    eC_SinCalderon = 0,
    eC_ConCalderon
};

//noteheads
enum ECabezaNotas
{
    ecn_Redonda = 1,
    ecn_Blanca,
    ecn_Negra,
    ecn_Aspa
};

// barlines
enum ETipoBarra
{
    etbBarraNormal = 1,             //linea fina
    etbBarraDoble,                  //dos lineas finas
    etbBarraFinal,                  //fina-gruesa
    etbBarraInicioRepeticion,       //Gruesa-fina-dos puntos
    etbBarraFinRepeticion,          //dos puntos-fina-gruesa
    etbBarraInicial,                //Gruesa-fina
    etbDobleRepeticion              //dos puntos-fina-fina-dos puntos
};

//Play modes: instrument to use to play a score
enum EPlayMode
{
    ePM_NormalInstrument = 1,        //play using normal instrument
    ePM_RitmoInstrument,
    ePM_RitmoPercusion,
    ePM_SolfeoVoz
};

//Highlight when playing a score
enum EHighlightType
{
    eVisualOff = 0,
    eVisualOn,
    eRemoveAllHighlight
};


// XML position data
typedef struct lmTXMLPositionData {
    lmTenths xRel;
    lmTenths yRel;
    lmTenths xDef;
    lmTenths yDef;
    bool fOverrideDefaultX;
    bool fOverrideDefaultY;
} RXMLPositionData;

typedef struct lmTFontData {
    wxString sFontName;
    int nFontSize;
    bool fBold;
    bool fItalic;
} RFontData;

//Global variables used as default initializators
extern RFontData goLyricDefaultFont;        // defined in NoteRestObj.cpp
extern RXMLPositionData goDefaultPos;        // defined in Text.cpp
extern RFontData goBasicTextDefaultFont;


//Constants
#define XML_DURATION_TO_LDP  64        //factor to convert between LDP duration and MusicXML duration

//options for Play() method
#define lmVISUAL_TRACKING            true        //highligth notes on the score as they are played
#define lmNO_VISUAL_TRACKING        false
#define NO_MARCAR_COMPAS_PREVIO    false

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
class lmText;
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

class lmFormatter;
class lmFormatter3;
class lmStaff;
class StaffList;
class lmContext;
class ContextList;
class lmSoundManager;



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
#include "Formatter.h"
#include "Formatter3.h"
#include "../app/global.h"
#include "../app/Paper.h"
#include "../sound/SoundManager.h"



// global unique variables used during score building
// TODO: Replace for lmScore/lmNote member funtions 
extern lmNoteRest* g_pLastNoteRest;
extern lmBeam* g_pCurBeam;

class lmScore
{
public:
    //ctor and dtor
    lmScore();
    ~lmScore();

    lmInstrument* AddInstrument(wxInt32 nVStaves, wxInt32 nMIDIChannel, wxInt32 nMIDIInstr);
    lmVStaff* GetVStaff(wxInt32 nInstr, wxInt32 nVStaff=1);
    wxInt32 GetNumMeasures();
    void Draw(lmPaper* pPaper);

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

    lmScoreObj* FindSelectableObject(wxPoint& pt);

    // Debug methods
    wxString Dump();
    wxString SourceLDP();
    wxString SourceXML();
    wxString DumpMidiEvents();

    int GetNumInstruments() { return (int)m_cInstruments.GetCount(); }

    //// Friend methods for lmFormatter object

    //iterator over instruments list
    lmInstrument* GetFirstInstrument();
    lmInstrument* GetNextInstrument();
    lmInstrument* GetLastInstrument();

    // titles related methods
    void SetTitle(wxString title);
    void SetSubtitle(wxString subtitle);
    void WriteTitles(bool fMeasuring, lmPaper *pPaper);

    // methods related to MusicXML import/export
    lmInstrument* XML_FindInstrument(wxString sId);

    //layout related methods
    lmLUnits SystemsDistance() { return m_nSystemsDistance; }
    lmLUnits TopSystemDistance() { return m_nTopSystemDistance + m_nHeadersHeight; }
    lmLUnits SystemsLeftMargin() { return m_nSystemsLeftMargin; }
    lmLUnits SystemsRightMargin() { return m_nSystemsRightMargin; }

    void SetSystemsDistance(lmLUnits nDistance) { m_nSystemsDistance = nDistance; }
    void SetTopSystemDistance(lmLUnits nDistance) { m_nTopSystemDistance = nDistance; }
    void SetSystemsLeftMargin(lmLUnits nDistance) { m_nSystemsLeftMargin = nDistance; }
    void SetSystemsRightMargin(lmLUnits nDistance) { m_nSystemsRightMargin = nDistance; }

    //global lmStaffObj list related methods
    void IncludeInGlobalList(lmStaffObj* pSO);
    void RemoveFromGlobalList(lmStaffObj* pSO);


private:
    void ComputeMidiEvents();
    void RemoveHighlight(lmStaffObj* pSO, lmPaper* pPaper);


        //
        // member variables
        //

    // a lmScore is, mainly, a collection of Instruments plus some data (composer, title, ...)
    InstrumentsList        m_cInstruments;    //wxList of instruments that compose this score

    //identification information
    lmText*        m_pTitle;
    lmText*        m_pSubtitle;

    //Variables related to polyphonic interpretation
    lmSoundManager*    m_pSoundMngr;        //Sound events table & manager
    StaffObjsList    m_cHighlighted;        //list of highlighted staffobjs

    //Layout related variables
    lmLUnits        m_nSystemsDistance;
    lmLUnits        m_nTopSystemDistance;
    lmLUnits        m_nSystemsLeftMargin;
    lmLUnits        m_nSystemsRightMargin;
    lmLUnits        m_nHeadersHeight;

    //other variables
    lmFormatter*    m_pFormatter;            //rendering algorithm
    wxInstrumentsListNode *m_pNode;        //last returned instrument node
    StaffObjsList    m_cGlobalStaffobjs;    //list of other StaffObjs not included in an lmVStaff

};



#endif    // __SCORE_H__
