//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 Cecilio Salmeron
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

#ifndef __LM_SCORE_H__        //to avoid nested includes
#define __LM_SCORE_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Score.cpp"
#endif

#include <vector>
#include <list>

#include "wx/debug.h"

#include "defs.h"
#include "Pitch.h"
#include "../app/global.h"



//Play modes: instrument to use to play a score
enum lmEPlayMode
{
    ePM_NormalInstrument = 1,        //play using normal instrument
    ePM_RhythmInstrument,
    ePM_RhythmPercussion,
    ePM_RhythmHumanVoice
};

//Highlight when playing a score
enum lmEHighlightType
{
    eVisualOff = 0,
    eVisualOn,
    eRemoveAllHighlight,
    ePrepareForHighlight,
};

//for paper size and margins settings
enum lmEPageScope
{
    lmSCOPE_PAGE = 0,       //for current page
    lmSCOPE_SECTION,        //for current section
    lmSCOPE_SCORE,          //for the whole document
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


//Constants
#define XML_DURATION_TO_LDP  64        //factor to convert between LDP duration and MusicXML duration

//options for Play() method
#define lmVISUAL_TRACKING           true        //highligth notes on the score as they are played
#define lmNO_VISUAL_TRACKING        false
#define lmNO_COUNTOFF     false

#define lmLDP_INDENT_STEP   3       //indent step for Source LDP generation
#define lmXML_INDENT_STEP   3       //indent step for MusicXML generation

// forward declarations
class lmPaper;
class lmPageInfo;
class lmScore;
class lmVStaff;
class lmInstrument;
class lmStaffObj;
class lmSOIterator;

class lmBasicText;
class lmTextItem;
class lmSOControl;
class lmClef;
class lmTimeSignature;
class lmKeySignature;
class lmBarline;
class lmNoteRest;
class lmBeam;
class lmTupletBracket;
class lmChord;
struct lmTBeamInfo;
class lmNote;
class lmRest;
class lmInstrGroup;
class lmStaff;
class lmContext;
class lmSoundManager;
class lmObjOptions;

class lmBox;
class lmBoxScore;
class lmScoreView;
class lmHandler;

#include "StaffObj.h"

#include "SOControl.h"
#include "AuxObj.h"
#include "Accidental.h"
#include "Instrument.h"
#include "Slur.h"            // Slurs and Ties
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
#include "ColStaffObjs.h"   //for lmVCursorState;
#include "../app/Paper.h"



//=======================================================================================
// class lmScoreCursor
//=======================================================================================

class lmScoreCursor
{
public:
    lmScoreCursor(lmScore* pOwnerScore);
    ~lmScoreCursor() {}

    //attachment to a ScoreView
	void AttachCursor(lmScoreView* pView);
	void DetachCursor();

    //positioning
    void ResetCursor();
    void MoveRight(bool fAlsoChordNotes = true);
    void MoveLeft(bool fAlsoChordNotes = true);
    void MoveToInitialPosition();
    void MoveUp();
    void MoveDown();
	void MoveNearTo(lmUPoint uPos, lmVStaff* pVStaff, int iStaff, int nMeasure);
    void MoveCursorToObject(lmStaffObj* pSO);

    //current position info
    float GetCursorTime();
    lmStaffObj* GetCursorSO();
    lmUPoint GetCursorPoint(int* pNumPage = NULL);
    lmStaff* GetCursorStaff();
    int GetCursorNumStaff();
    lmVStaff* GetVStaff();
	inline int GetCursorInstrumentNumber() { return m_nCursorInstr; }
	inline lmScore* GetCursorScore() { return m_pScore; }
    inline lmVStaffCursor* GetVCursor() { return m_pVCursor; }
    void SetNewCursorState(lmVCursorState* pState);
    void SelectCursor(lmVStaffCursor* pVCursor);
    //int GetPageNumber();


private:
    void SelectCursorFromInstr(int nInstr);

    lmScore*            m_pScore;           //owner score
    lmScoreView*        m_pView;            //View using this cursor
	lmVStaffCursor*		m_pVCursor;		    //current cursor
	int					m_nCursorInstr;		//instrument number (1..n) of current cursor

};


//=======================================================================================
// lmStylesCollection: helper class with info about available styles
//=======================================================================================

class lmStylesCollection
{
public:
    lmStylesCollection();
    ~lmStylesCollection();
    
    //info
    inline int NumObjects() { return (int)m_aTextStyles.size(); }
    inline bool IsEmpty() const { return m_aTextStyles.size()==0; }

    //management
    lmTextStyle* AddStyle(const wxString& sName, lmFontInfo& tFontData, wxColour nColor);
    void RemoveStyle(lmTextStyle* pStyle);

    //access 
    lmTextStyle* GetStyleInfo(const wxString& sStyleName);
    lmTextStyle* GetStyleName(lmFontInfo& tFontData, wxColour nColor);
    void StyleInUse(bool fInUse);
    lmTextStyle* GetFirstStyle();
    lmTextStyle* GetNextStyle();

    //other
    wxString SourceLDP(int nIndent);


protected:
    wxString SourceLDP(int nIndent, lmTextStyle* pStyle);

	std::list<lmTextStyle*>     m_aTextStyles;		//list of defined styles
    std::list<lmTextStyle*>::iterator    m_it;      //for GetFirst(), GetNext() methods

};



//=======================================================================================
// helper class lmPageInfo: page size, margins, orientation, etc.
//=======================================================================================

class lmPageInfo
{
public:
    //constructor: all data in milimeters
    lmPageInfo(int nLeftMargin = 20, int nRightMargin = 15, int nTopMargin = 20,
               int nBottomMargin = 20, int nBindingMargin = 0,
               wxSize nPageSize = wxSize(210, 297), bool fPortrait = true );            
    lmPageInfo(lmPageInfo* pPageInfo);            
    ~lmPageInfo() {}

    //change settings
    inline void SetTopMargin(lmLUnits uValue) { m_uTopMargin = uValue; }
    inline void SetBottomMargin(lmLUnits uValue) { m_uBottomMargin = uValue; }
    inline void SetLeftMargin(lmLUnits uValue) { m_uLeftMargin = uValue; }
    inline void SetRightMargin(lmLUnits uValue) { m_uRightMargin = uValue; }
    inline void SetBindingMargin(lmLUnits uValue) { m_uBindingMargin = uValue; }
    inline void SetPageSize(lmLUnits uWidth, lmLUnits uHeight)
                    { m_uPageSize.SetWidth(uWidth); m_uPageSize.SetHeight(uHeight); }
    void SetPageSizeMillimeters(wxSize nSize);
    inline void SetOrientation(bool fPortrait) { m_fPortrait = fPortrait; }
    inline void SetNewSection(bool fNewSection) { m_fNewSection = fNewSection; }

    // Access

    lmLUnits TopMargin() { return m_uTopMargin; }
    lmLUnits BottomMargin() { return m_uBottomMargin; }
    lmLUnits LeftMargin(int nNumPage) {
        return (nNumPage % 2) ? m_uLeftMargin + m_uBindingMargin : m_uLeftMargin ;
    }
    lmLUnits RightMargin(int nNumPage) {
        return (nNumPage % 2) ? m_uRightMargin : m_uRightMargin + m_uBindingMargin ;
    }
    inline lmLUnits PageWidth() { return (m_fPortrait ? m_uPageSize.Width() : m_uPageSize.Height()); }
    inline lmLUnits PageHeight() { return (m_fPortrait ? m_uPageSize.Height() : m_uPageSize.Width()); }
    inline lmLUnits GetUsableHeight() { return m_uPageSize.GetHeight() - m_uTopMargin - m_uBottomMargin; }

	//source code
    wxString SourceLDP(int nIndent);


private:
    //margins, all in logical units
    lmLUnits        m_uLeftMargin;
    lmLUnits        m_uRightMargin;
    lmLUnits        m_uTopMargin;
    lmLUnits        m_uBottomMargin;
    lmLUnits        m_uBindingMargin;

    //paper size, in logical units
    lmUSize         m_uPageSize; 
    bool            m_fPortrait;
    bool            m_fNewSection;
};



//=======================================================================================
// class lmScore
//=======================================================================================

// to identify handlers
enum {
    lmMARGIN_TOP = 0,
    lmMARGIN_BOTTOM,
    lmMARGIN_LEFT,
    lmMARGIN_RIGHT,
};

class lmScore : public lmScoreObj
{
public:
    //ctor and dtor
    lmScore();
    ~lmScore();

	//---- virtual methods of base class -------------------------

    // units conversion
    lmLUnits TenthsToLogical(lmTenths nTenths);
    lmTenths LogicalToTenths(lmLUnits uUnits);
	inline lmEScoreObjType GetScoreObjType() { return lmSOT_Score; }
    inline lmScore* GetScore() { return this; }

	//---- overrides
	void PopupMenu(lmController* pCanvas, lmGMObject* pGMO, const lmDPoint& vPos);


	//---- specific methods of this class ------------------------

    int GetNumMeasures();

    // play methods
    void Play(bool fVisualTracking = lmNO_VISUAL_TRACKING,
              bool fCountOff = lmNO_COUNTOFF,
              lmEPlayMode nPlayMode = ePM_NormalInstrument,
              long nMM = 0,
              wxWindow* pWindow = (wxWindow*)NULL );
    void PlayMeasure(int nMeasure,
                     bool fVisualTracking = lmNO_VISUAL_TRACKING,
                     lmEPlayMode nPlayMode = ePM_NormalInstrument,
                     long nMM = 0,
                     wxWindow* pWindow = (wxWindow*)NULL );
    void PlayFromMeasure(int nMeasure,
						 bool fVisualTracking = lmNO_VISUAL_TRACKING,
						 lmEPlayMode nPlayMode = ePM_NormalInstrument,
						 long nMM = 0,
						 wxWindow* pWindow = (wxWindow*)NULL );
    void Pause();
    void Stop();
    void WaitForTermination();
    void DeleteMidiEvents();

    // serving highlight events
    void ScoreHighlight(lmStaffObj* pSO, wxDC* pDC, lmEHighlightType nHighlightType);
	void RemoveAllHighlight(wxWindow* pCanvas);


    // Debug methods. If filename provided writes also to file
    wxString Dump() { return Dump(_T("")); }
    wxString Dump(wxString sFilename);
    wxString SourceLDP(wxString sFilename = _T(""));
    wxString SourceXML(wxString sFilename = _T(""));
    wxString DumpMidiEvents(wxString sFilename = _T(""));

	// instrument related
    int GetNumInstruments() { return (int)m_cInstruments.size(); }
    lmInstrument* GetInstrument(int nInstr);
    lmInstrument* GetFirstInstrument();
    lmInstrument* GetNextInstrument();
    lmInstrument* AddInstrument(int nMIDIChannel, int nMIDIInstr,
                                wxString sName, wxString sAbbrev=_T(""),
                                lmInstrGroup* pGroup = (lmInstrGroup*)NULL );
    lmInstrument* AddInstrument(int nMIDIChannel, int nMIDIInstr,
                                lmInstrNameAbbrev* pName,
                                lmInstrNameAbbrev* pAbbrev,
                                lmInstrGroup* pGroup = (lmInstrGroup*)NULL );


    // titles related methods
    lmTextBlock* AddTitle(wxString sTitle, lmEHAlign nAlign, lmTextStyle* pStyle);
	void LayoutTitles(lmBox* pBox, lmPaper *pPaper);

    // identification
    wxString GetScoreName();
    void SetScoreName(wxString sName);
    inline long GetID() const { return m_nID; }

    // properties
    inline bool IsReadOnly() { return m_fReadOnly; }
    inline void SetReadOnly(bool fValue) { m_fReadOnly = fValue; }

    // methods related to MusicXML import/export
    lmInstrument* XML_FindInstrument(wxString sId);

    //layout related methods
    lmBoxScore* Layout(lmPaper* pPaper);
    lmLUnits TopSystemDistance() { return m_nTopSystemDistance + m_nHeadersHeight; }
    void SetTopSystemDistance(lmLUnits nDistance) { m_nTopSystemDistance = nDistance; }
    void LayoutAttachedObjects(lmBox* pBox, lmPaper *pPaper);

	inline void SetModified(bool fValue) { m_fModified = fValue; }
	inline bool IsModified() { return m_fModified; }

	//methods used for renderization
	void SetMeasureModified(int nMeasure, bool fModified = true);
	bool IsMeasureModified(int nMeasure);
	void ResetMeasuresModified();

	//cursor management
    lmScoreCursor* SetCursor(lmVStaffCursor* pVCursor);
    inline void ResetCursor() { m_SCursor.ResetCursor(); }
    inline lmScoreCursor* GetCursor() { return &m_SCursor; }
        //attachment to a ScoreView
	lmScoreCursor* AttachCursor(lmScoreView* pView);
	void DetachCursor();
    lmScoreCursor* SetNewCursorState(lmVCursorState* pState);

	//pages layout information
	void SetPageInfo(int nPage);
	void ClearPages();

    // paper size and margings
    lmLUnits GetPageTopMargin(int nPage = 1);
    lmLUnits GetPageLeftMargin(int nPage = 1);
    lmLUnits GetPageRightMargin(int nPage = 1);
    lmUSize GetPaperSize(int nPage = 1);
    lmLUnits GetMaximumY(int nPage = 1);
    lmLUnits GetRightMarginXPos(int nPage = 1);
    lmLUnits GetLeftMarginXPos(int nPage = 1);

        //change settings of current page
    void SetPageTopMargin(lmLUnits uValue, int nPage=1);
    void SetPageLeftMargin(lmLUnits uValue, int nPage=1);
    void SetPageRightMargin(lmLUnits uValue, int nPage=1);
    void SetPageBottomMargin(lmLUnits uValue, int nPage=1);
    void SetPageBindingMargin(lmLUnits uValue, int nPage=1);

    void SetPageSize(lmLUnits uWidth, lmLUnits uHeight, int nPage=1);
    inline void SetPageSizeMillimeters(wxSize nSize)
                            { m_pPageInfo->SetPageSizeMillimeters(nSize); }

    void SetPageOrientation(bool fPortrait, int nPage=1);
    void SetPageNewSection(bool fNewSection, int nPage=1);

    //text styles
    inline lmTextStyle* GetStyleInfo(const wxString& sStyleName)
                            { return m_TextStyles.GetStyleInfo(sStyleName); }
    inline lmTextStyle* GetStyleName(lmFontInfo& tFontData, wxColour nColor = *wxBLACK)
                            { return m_TextStyles.GetStyleName(tFontData, nColor); }
    inline lmTextStyle* GetFirstStyle() { return m_TextStyles.GetFirstStyle(); }
    inline lmTextStyle* GetNextStyle() { return m_TextStyles.GetNextStyle(); }
    inline lmTextStyle* AddStyle(const wxString& sName, lmFontInfo& tFont, wxColour nColor)
                            { return m_TextStyles.AddStyle(sName, tFont, nColor); }
    inline void RemoveStyle(lmTextStyle* pStyle)
                            { m_TextStyles.RemoveStyle(pStyle); }

    //handlers
    lmUPoint CheckHandlerNewPosition(lmHandler* pHandler, int nIdx, int nPage, lmUPoint& uPos);

	//call backs for edition
	bool OnInstrProperties(int nInstr, lmController* pController);

private:
    friend class lmScoreCursor;

    void WriteToFile(wxString sFilename, wxString sContent);
    void ComputeMidiEvents();
    void RemoveHighlight(lmStaffObj* pSO, wxDC* pDC);
	lmLUnits CreateTitleShape(lmBox* pBox, lmPaper *pPaper, lmTextBlock* pTitle,
							  lmLUnits nPrevTitleHeight);
	void DoAddInstrument(lmInstrument* pInstr, lmInstrGroup* pGroup);


        //
        // member variables
        //

    // a lmScore is, mainly, a collection of Instruments plus some data (composer, title, ...)
    std::vector<lmInstrument*>	m_cInstruments;     //list of instruments that form this score
    std::vector<int>			m_nTitles;          //indexes (over attached AuxObjs) to titles

    //Variables related to polyphonic interpretation
    lmSoundManager*			m_pSoundMngr;       //Sound events table & manager
    std::list<lmStaffObj*>	m_cHighlighted;     //list of highlighted staffobjs

    //Layout related variables
    lmLUnits			    m_nTopSystemDistance;
    lmLUnits			    m_nHeadersHeight;
    lmVStaff*               m_pTenthsConverter;     //for lmTenths <-> lmLUnits conversion

    //renderization options
	bool				    m_fModified;            //to force a repaint

    //page size and margins information
    lmPageInfo*             m_pPageInfo;    //for current page 
    int                     m_nNumPage;     //number of current page
	std::list<lmPageInfo*>  m_PagesInfo;    //info for each score page


    //other variables
    bool                    m_fReadOnly;    //the score is in read only mode
	int					    m_nCurNode;     //last returned instrument node
    long				    m_nID;          //unique ID for this score
    wxString			    m_sScoreName;   //for user identification
	lmStylesCollection      m_TextStyles;   //list of defined styles

	//temporary data used for edition/renderization
	std::list<int>          m_aMeasureModified;		//list of measures modified

	//for edition
	lmScoreCursor		m_SCursor;			//Active cursor pointing to current position

};



#endif    // __LM_SCORE_H__
