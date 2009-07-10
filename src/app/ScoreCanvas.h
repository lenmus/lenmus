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

#ifndef _LM_SCORECANVAS_H        //to avoid nested includes
#define _LM_SCORECANVAS_H

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ScoreCanvas.cpp"
#endif

#include "wx/docview.h"
#include "global.h"
class lmScoreHighlightEvent;
#include "../sound/SoundEvents.h"
#include "toolbox/ToolsBox.h"

class lmScoreView;
class lmDocument;
class lmGMSelection;
class lmToolBoxToolSelectedEvent;
class lmToolBoxPageChangedEvent;
class lmToolPage;
class lmScoreProcessor;
class lmShapeStaff;
class lmBoxSliceInstr;

#define lmUNSELECT      false       //remove selection
#define lmSELECT        true        //select objects


//Abstract class. All controllers must derive from it
class lmController : public wxWindow
{
   DECLARE_ABSTRACT_CLASS(lmController)

public:
    lmController(wxWindow *pParent, lmScoreView *pView, lmDocument* pDoc,
				 wxColor colorBg, wxWindowID id = wxID_ANY,
				 const wxPoint& pos = wxDefaultPosition,
				 const wxSize& size = wxDefaultSize, long style = 0);
	virtual ~lmController();

	//commands without Do/Undo support
	virtual void PlayScore(bool fFromCursor=false) {}
    virtual void StopPlaying(bool fWait=false) {}
    virtual void PausePlaying() {}


	// commands with Do/Undo support

        //add/insert commands
	virtual void AddTitle() {}
    virtual void AttachNewText(lmComponentObj* pCO) {}
	virtual void InsertBarline(lmEBarline nType = lm_eBarlineSimple) {}
    virtual void InsertClef(lmEClefType nClefType) {}
	virtual void InsertNote(lmEPitchType nPitchType, int nStep, int nOctave,
					lmENoteType nNoteType, float rDuration, int nDots,
					lmENoteHeads nNotehead,
                    lmEAccidentals nAcc = lm_eNoAccidentals,
                    int nVoice = 0,
					lmNote* pBaseOfChord = (lmNote*)NULL,
					bool fTiedPrev = false) {}

    virtual void InsertRest(lmENoteType nNoteType, float rDuration, int nDots, int nVoice) {}
    virtual void InsertTimeSignature(int nBeats, int nBeatType, bool fVisible = true) {}    //for type eTS_Normal
    virtual void InsertKeySignature(int nFifths, bool fMajor, bool fVisible = true) {}

        //delete commands
	virtual void DeleteStaffObj() {}
    virtual void DeleteSelection() {}
    virtual void DeleteTie(lmNote* pEndNote) {}

        //change/move commands
    virtual void AddTie(lmNote* pStartNote, lmNote* pEndNote) {}
    virtual void ChangeMidiSettings(lmInstrument* pInstr, int nMidiChannel,
                                    int nMidiInstr) {}
	virtual void ChangeNotePitch(int nSteps) {}
	virtual void ChangeNoteAccidentals(int nAcc) {}
	virtual void ChangeNoteDots(int nDots) {}
    virtual void ChangeTie(lmNote* pStartNote, lmNote* pEndNote) {}
    virtual void AddTuplet() {}
    virtual void DeleteTuplet(lmNoteRest* pStartNote) {}
	virtual void MoveNote(lmGMObject* pGMO, const lmUPoint& uPos, int nSteps) {}
	virtual void MoveObject(lmGMObject* pGMO, const lmUPoint& uPos,
                            bool fUpdateViews = true) {}
    virtual void MoveObjectPoints(lmGMObject* pGMO, lmUPoint uShifts[], int nNumPoints,
                                  bool fUpdateViews = true) {}
    virtual void BreakBeam() {}
    virtual void JoinBeam() {}
    virtual void ChangeText(lmScoreText* pST, wxString sText, lmEHAlign nAlign,
                            lmLocation tPos, lmTextStyle* pStyle, int nHintOptions=0) {}
    virtual void ChangePageMargin(lmGMObject* pGMO, int nIdx, int nPage, lmLUnits uPos) {}
	virtual void ChangeBarline(lmBarline* pBL, lmEBarline nBarlineType, bool fVisible) {}



    // event handlers
    virtual void OnKeyPress(wxKeyEvent& event) { event.Skip(); }
    virtual void OnKeyDown(wxKeyEvent& event) { event.Skip(); }
	virtual void OnEraseBackground(wxEraseEvent& event);

	//contextual menus
	void ShowContextualMenu(lmScoreObj* pOwner, lmGMObject* pGMO, wxMenu* pMenu, int x, int y);
	virtual wxMenu* GetContextualMenu(bool fInitialize = true);

	//event handlers for contextual menus
	virtual void OnCut(wxCommandEvent& event) {}
    virtual void OnCopy(wxCommandEvent& event) {}
    virtual void OnPaste(wxCommandEvent& event) {}
    virtual void OnColor(wxCommandEvent& event) {}
    virtual void OnProperties(wxCommandEvent& event) {}
    virtual void OnDeleteTiePrev(wxCommandEvent& event) {}
    virtual void OnAttachText(wxCommandEvent& event) {}
	virtual void OnScoreTitles(wxCommandEvent& event) {}
	virtual void OnViewPageMargins(wxCommandEvent& event) {}
#ifdef __WXDEBUG__
	virtual void OnDumpShape(wxCommandEvent& event) {}
#endif

    //call backs
    virtual void SynchronizeToolBox() {}
    virtual void RestoreToolBoxSelections() {}



protected:
	wxMenu*			m_pMenu;			//contextual menu
	lmScoreObj*		m_pMenuOwner;		//contextual menu owner
	lmGMObject*		m_pMenuGMO;			//graphic object who displayed the contextual menu
	lmToolBox*      m_pToolBox;         //the associated toolbox, if any
    int             m_nOctave;          //current octave for note insertion
	int				m_nVoice;			//current voice for note insertion

private:

	DECLARE_EVENT_TABLE()
};



//-----------------------------------------------------------------------------------------------
// lmScoreCanvas: Controller for score edition
//-----------------------------------------------------------------------------------------------

//Valid areas for mouse interaction
#define lmMOUSE_OnStaff         0x0001      //pointing to an staff shape
#define lmMOUSE_OnAboveStaff    0x0002      //pointing to top margin of lmBoxSliceInstr
#define lmMOUSE_OnBelowStaff    0x0004      //pointing to bottom margin of lmBoxSliceInstr
#define lmMOUSE_OnOtherShape    0x0008      //pointing to a shape other than staff
#define lmMOUSE_OnOtherBox      0x0010      //pointing to a box, other cases
#define lmMOUSE_OnOther         0x0020      //pointing to other place (score paper)
#define lmMOUSE_OnAny           0xFFFF      //pointing to any place (all score is valid)



//enum to define function/tool assigned to the mouse
enum lmEMouseFunction
{
    lm_eMouse_Select = 0,           //pointer: select, drag
    lm_eMouse_Tool_Note,            //insert note
};



class lmScoreCanvas : public lmController
{
	DECLARE_DYNAMIC_CLASS(lmScoreCanvas)

public:

    // constructors and destructor
    lmScoreCanvas(lmScoreView *pView, wxWindow *pParent, lmDocument* pDoc,
                  const wxPoint& pos, const wxSize& size, long style, wxColor colorBg);
    ~lmScoreCanvas();

    // event handlers
    void OnPaint(wxPaintEvent &WXUNUSED(event));
    void OnMouseEvent(wxMouseEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    void OnVisualHighlight(lmScoreHighlightEvent& event);
	void OnKeyPress(wxKeyEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnToolBoxEvent(lmToolBoxToolSelectedEvent& event);
    void OnToolBoxPageChanged(lmToolBoxPageChangedEvent& event);
#ifdef __WXMSW__
    void OnMouseCaptureLost(wxMouseCaptureLostEvent& event);
#endif

	//commands without Do/Undo support
    void PlayScore(bool fFromCursor=false);
    void StopPlaying(bool fWait=false);
    void PausePlaying();

	// commands with Do/Undo support

        //add/insert commands
	void AddTitle();
    void AttachNewText(lmComponentObj* pCO);
	void InsertBarline(lmEBarline nType = lm_eBarlineSimple);
    void InsertClef(lmEClefType nClefType);
	void InsertNote(lmEPitchType nPitchType, int nStep, int nOctave,
					lmENoteType nNoteType, float rDuration, int nDots,
					lmENoteHeads nNotehead,
                    lmEAccidentals nAcc = lm_eNoAccidentals,
                    int nVoice = 0,
					lmNote* pBaseOfChord = (lmNote*)NULL,
					bool fTiedPrev = false);

    void InsertRest(lmENoteType nNoteType, float rDuration, int nDots, int nVoice);
    void InsertTimeSignature(int nBeats, int nBeatType, bool fVisible = true);    //for type eTS_Normal
    void InsertKeySignature(int nFifths, bool fMajor, bool fVisible = true);

        //delete commands
	void DeleteStaffObj();
    void DeleteSelection();
    void DeleteTie(lmNote* pEndNote);

        //change/move commands
    void AddTie(lmNote* pStartNote, lmNote* pEndNote);
    void ChangeMidiSettings(lmInstrument* pInstr, int nMidiChannel, int nMidiInstr);
	void ChangeNotePitch(int nSteps);
	void ChangeNoteAccidentals(int nAcc);
	void ChangeNoteDots(int nDots);
    void ChangeTie(lmNote* pStartNote, lmNote* pEndNote);
    void AddTuplet();
    void DeleteTuplet(lmNoteRest* pStartNR);
	void MoveNote(lmGMObject* pGMO, const lmUPoint& uPos, int nSteps);
	void MoveObject(lmGMObject* pGMO, const lmUPoint& uPos, bool fUpdateViews = true);
    void MoveObjectPoints(lmGMObject* pGMO, lmUPoint uShifts[], int nNumPoints,
                          bool fUpdateViews = true);
    void BreakBeam();
    void JoinBeam();
    void ChangeText(lmScoreText* pST, wxString sText, lmEHAlign nAlign,
                    lmLocation tPos, lmTextStyle* pStyle, int nHintOptions=0);
    void ChangePageMargin(lmGMObject* pGMO, int nIdx, int nPage, lmLUnits uPos);
	void ChangeBarline(lmBarline* pBL, lmEBarline nBarlineType, bool fVisible);

    //mouse processing
    void DoCaptureMouse();
    void DoReleaseMouse();
    void StartToolDrag(wxDC* pDC);
    void ContinueToolDrag(wxMouseEvent& event, wxDC* pDC);
    void TerminateToolDrag(wxDC* pDC);
    void StartToolDrag();
    void TerminateToolDrag();

    //call backs
    void SynchronizeToolBox();
    void RestoreToolBoxSelections();
    void OnViewUpdated();
    void OnNewGraphicalModel();

	//contextual menus
	wxMenu* GetContextualMenu(bool fInitialize = true);

	//event handlers for contextual menus
	void OnCut(wxCommandEvent& event);
    void OnCopy(wxCommandEvent& event);
    void OnPaste(wxCommandEvent& event);
    void OnColor(wxCommandEvent& event);
    void OnProperties(wxCommandEvent& event);
    void OnDeleteTiePrev(wxCommandEvent& event);
    void OnAttachText(wxCommandEvent& event);
    void OnScoreTitles(wxCommandEvent& event);
    void OnViewPageMargins(wxCommandEvent& event);
#ifdef __WXDEBUG__
	void OnDumpShape(wxCommandEvent& event);
#endif

    //call backs from lmScoreView to paint marks for mouse dragged tools
    lmUPoint OnDrawToolMarks(lmPaper* pPaper, const lmUPoint& uPos);
    lmUPoint OnRedrawToolMarks(lmPaper* pPaper, const lmUPoint& uPos);
    lmUPoint OnRemoveToolMarks(lmPaper* pPaper, const lmUPoint& uPos);


private:

    //mouse cursors
    enum lmEMouseCursor        //AWARE: Must match LoadMouseCursors() implementation
    {
        lm_eCursor_Pointer = 0,
        lm_eCursor_Note,
        lm_eCursor_Note_Forbidden,
        //
        lm_eCursor_Max          //the last item.
    };


    //dealing with mouse events
    void OnMouseEventToolMode(wxMouseEvent& event, wxDC* pDC);
    void OnMouseEventSelectMode(wxMouseEvent& event, wxDC* pDC);

    //mouse tools operations
    void OnToolClick(lmGMObject* pGMO, lmUPoint uPagePos, float rTime);
    void PrepareToolDragImages();
    void GetDataEntryMode();

    //mouse cursors
    wxCursor* LoadMouseCursor(wxString sFile, int nHotSpotX, int nHotSpotY);
    void LoadAllMouseCursors();
    wxCursor* GetMouseCursor(lmEMouseCursor nCursorID);
    void ChangeMouseIcon();

    void DeleteCaretOrSelected();
    void SynchronizeToolBoxWithSelection(bool fEnable = true);
    void SynchronizeToolBoxWithCaret(bool fEnable = true);

    bool TileBitmap(const wxRect& rect, wxDC& dc, wxBitmap& bitmap);
    void LogKeyEvent(wxString name, wxKeyEvent& event, int nTool=-1);
    wxString KeyCodeToName(int nKeyCode);
    void ProcessKey(wxKeyEvent& event);

    //tools' selection
    void SelectNoteDuration(int iButton);
    void SelectNoteAccidentals(bool fNext);
    void SelectNoteDots(bool fNext);
    void SelectOctave(bool fUp);
    void SelectOctave(int nOctave);
    void SelectVoice(bool fUp);
    void SelectVoice(int nVoice);

    //access to ToolBox
    void GetToolBoxValuesForPage(lmEToolPage nPage);


    //managing selections and cursor pointed objects
    bool IsSelectionValidForTies(lmNote** ppStartNote = NULL, lmNote** ppEndNote = NULL);
    lmNoteRest* IsSelectionValidForTuplet();
    bool IsCursorValidToCutBeam();
    bool IsSelectionValidToJoinBeam();




    lmScoreView*    m_pView;            //owner view
    wxWindow*       m_pOwner;           //parent window
    lmDocument*     m_pDoc;             //the document rendered by the view

    wxColour        m_colorBg;			//colour for background

    //data entry mode
    int             m_nEntryMode;

    //mouse cursors
    std::vector<wxCursor*>  m_MouseCursors;             //array. Indexes are enum lmEMouseCursor
    wxCursor*               m_pCursorOnSelectedObject;  //mouse cursors to use
    wxCursor*               m_pCursorOnValidArea;
    wxCursor*               m_pCursorElse;
    wxCursor*               m_pCursorCurrent;           //current displayed mouse cursor
    long                    m_nValidAreas;              //flags defining valid areas
    wxBitmap*               m_pToolBitmap;


	//to control octave when inserting several consecutive notes
	bool			m_fInsertionSequence;
	int				m_nLastOctave;

    //to sore information of key down event in order to deal properly with key codes
    int             m_nKeyDownCode;     //key code when KEY_DOWN event
    bool            m_fCmd;             //Ctrol pressed
    bool            m_fAlt;             //Alt pressed
    bool            m_fShift;           //Shift pressed

    //buffer for commands requiring several keystrokes
    wxString        m_sCmd;


    //to save options selected by user in ToolBox
    bool            m_fToolBoxSavedOptions;
    int             m_nTbAcc;
    int             m_nTbDots;
    int             m_nTbDuration;

    //current values selected in ToolBox
    //PageNotes
    lmENoteType     m_nSelNoteType;
	int             m_nSelDots;
	lmENoteHeads    m_nSelNotehead;
	lmEAccidentals  m_nSelAcc;
	int             m_nSelOctave;
	int             m_nSelVoice;

    //temporary data for OnMouseEvent method

    lmDPoint     m_vStartDrag;       //initial point (pixels) of dragging area
    lmDPoint     m_vEndDrag;         //last end point (pixels) of dragging area
    lmUPoint     m_uStartDrag;       //initial point (logical, page origin) of dragging area
    bool         m_fDraggingObject;  //dragging an object
    bool         m_fCheckTolerance;  //to control false dragging starts

    // As wxDragImage works with unscrolled device coordinates, we need current
    // maouse position in device units (pixels). All device coordinates are
    // referred to the lmScoreCanvas window.
    // We also need logical coordinates of point pointed by mouse

    lmUPoint     m_uMousePagePos;        //position (lmLUnits) referred to current page origin
    lmDPoint     m_vMousePagePos;        //position (pixels) referred to current page origin
    lmDPoint     m_vMouseCanvasPos;      //position referred to canvas org. (pixels)
    lmDPoint     m_vCanvasOffset;        //canvas: offset referred to view origin
    lmDPoint     m_vPageOrg;             //origin (pixels) of current page referred to view origin

    //other information related to mouse pointed point
    int          m_nNumPage;             //score page number (1..n) on which the mouse is placed

    //dragging a tool
    long            m_nMousePointedArea;    //type of area pointed by mouse
    lmShapeStaff*   m_pLastShapeStaff;      //last staff for point pointed by mouse
    lmShapeStaff*   m_pCurShapeStaff;       //current staff for point pointed by mouse
    lmBoxSliceInstr* m_pLastBSI;            //last BoxSliceInstr pointed by mouse
    lmBoxSliceInstr* m_pCurBSI;             //current BoxSliceInstr pointed by mouse
    float           m_rCurTime;             //time for current mouse position
    bool            m_fDraggingTool;        //dragging a tool



    //new--------------------------------------------------------------
	//dragging on canvas with left button: selection
	void OnCanvasBeginDragLeft(lmDPoint vCanvasPos, lmUPoint uPagePos, int nKeys);
	void OnCanvasContinueDragLeft(bool fDraw, lmDPoint vCanvasPos, lmUPoint uPagePos, int nKeys);
	void OnCanvasEndDragLeft(lmDPoint vCanvasPos, lmUPoint uPagePos, int nKeys);

	//dragging on canvas with right button
	void OnCanvasBeginDragRight(lmDPoint vCanvasPos, lmUPoint uPagePos, int nKeys);
	void OnCanvasContinueDragRight(bool fDraw, lmDPoint vCanvasPos, lmUPoint uPagePos, int nKeys);
	void OnCanvasEndDragRight(lmDPoint vCanvasPos, lmUPoint uPagePos, int nKeys);

	//dragging object with left button
	void OnObjectBeginDragLeft(wxMouseEvent& event, wxDC* pDC, lmDPoint vCanvasPos,
							   lmDPoint vCanvasOffset, lmUPoint uPagePos, int nKeys);
	void OnObjectContinueDragLeft(wxMouseEvent& event, wxDC* pDC, bool fDraw,
								  lmDPoint vCanvasPos, lmDPoint vCanvasOffset,
								  lmUPoint uPagePos, int nKeys);
	void OnObjectEndDragLeft(wxMouseEvent& event, wxDC* pDC, lmDPoint vCanvasPos,
							 lmDPoint vCanvasOffset, lmUPoint uPagePos, int nKeys);

	//dragging object with right button
	void OnObjectBeginDragRight(wxMouseEvent& event, wxDC* pDC, lmDPoint vCanvasPos,
							   lmDPoint vCanvasOffset, lmUPoint uPagePos, int nKeys);
	void OnObjectContinueDragRight(wxMouseEvent& event, wxDC* pDC, bool fDraw,
								  lmDPoint vCanvasPos, lmDPoint vCanvasOffset,
								  lmUPoint uPagePos, int nKeys);
	void OnObjectEndDragRight(wxMouseEvent& event, wxDC* pDC, lmDPoint vCanvasPos,
							 lmDPoint vCanvasOffset, lmUPoint uPagePos, int nKeys);

	//non-dragging events: click on an object
	void OnLeftClickOnObject(lmGMObject* pGMO, lmDPoint vCanvasPos, lmUPoint uPagePos, int nKeys);
	void OnLeftDoubleClickOnObject(lmGMObject* pGMO, lmDPoint vCanvasPos, lmUPoint uPagePos, int nKeys);
	void OnRightClickOnObject(lmGMObject* pGMO, lmDPoint vCanvasPos, lmUPoint uPagePos, int nKeys);
	void OnRightDoubleClickOnObject(lmGMObject* pGMO, lmDPoint vCanvasPos, lmUPoint uPagePos, int nKeys);

	//non-dragging events: click on canvas
	void OnRightClickOnCanvas(lmDPoint vCanvasPos, lmUPoint uPagePos, int nKeys);
	void OnLeftClickOnCanvas(lmDPoint vCanvasPos, lmUPoint uPagePos, int nKeys);

    // dragging control variables
    int             m_nDragState;
    lmUPoint        m_uDragStartPos;
    lmDPoint        m_vDragHotSpot;			//absolute point (pixels)
    lmUPoint        m_uHotSpotShift;		//distance from shape origin
	lmGMObject*		m_pDraggedGMO;			//GMObject being dragged

    // mouse over objects
	lmGMObject*		m_pMouseOverGMO;        //GMObject on which mouse is flying over

    DECLARE_EVENT_TABLE()
};


//---------------------------------------------------------------------------
// Helper class to define edition modes
//---------------------------------------------------------------------------

class lmEditorMode
{
public:
    lmEditorMode(wxClassInfo* pControllerInfo, wxClassInfo* pScoreProcInfo);
    lmEditorMode(wxString& sCreationMode, wxString& sCreationVers);
    ~lmEditorMode();

    void CustomizeToolBoxPages(lmToolBox* pToolBox);
    lmScoreProcessor* CreateScoreProcessor();

    //ToolBox reated
    void ChangeToolPage(int nPageID, wxClassInfo* pToolPageInfo);

    //Creation mode information
    inline void SetModeName(const wxString& sName) { m_sCreationModeName = sName; }
    inline void SetModeVers(const wxString& sVers) { m_sCreationModeVers = sVers; }
    inline wxString& GetModeName() { return m_sCreationModeName; }
    inline wxString& GetModeVers() { return m_sCreationModeVers; }

protected:

    wxString                m_sCreationModeName;
    wxString                m_sCreationModeVers;
    wxClassInfo*            m_pControllerInfo;
    wxClassInfo*            m_pScoreProcInfo;
    wxClassInfo*            m_ToolPagesInfo[lmPAGE_MAX];
    //lmToolBoxConfiguration  m_config;

};




#endif  // _LM_SCORECANVAS_H
