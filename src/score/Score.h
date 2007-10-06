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

#ifndef __SCORE_H__        //to avoid nested includes
#define __SCORE_H__

#ifdef __GNUG__
#pragma interface "Score.cpp"
#endif

#include "wx/debug.h"
#include "defs.h"
#include "Pitch.h"
#include "../app/global.h"

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

// Spacing methods for rendering scores
// -------------------------------------
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

enum lmESpacingMethod
{
    esm_Fixed = 1,
    esm_PropConstantFixed,
    //esm_PropConstantShortNote,
    //esm_PropVariableShortNote,
    //esm_PropVariableNumBars,
};


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
	void RemoveAllHighlight(wxWindow* pCanvas);

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
	inline void SetModified(bool fValue) { m_fModified = fValue; }
	inline bool IsModified() { return m_fModified; }


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
	bool				m_fModified;			//to force a repaint

    //other variables
    wxInstrumentsListNode*  m_pNode;        //last returned instrument node
    StaffObjsList   m_cGlobalStaffobjs;     //list of other StaffObjs not included in an lmVStaff
    long            m_nID;                  //unique ID for this score
    wxString        m_sScoreName;           //for user identification

};



#endif    // __SCORE_H__
