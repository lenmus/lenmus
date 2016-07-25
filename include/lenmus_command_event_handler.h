//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2015 LenMus project
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

#ifndef __LENMUS_COMMAND_EVENT_HANDLER_H__        //to avoid nested includes
#define __LENMUS_COMMAND_EVENT_HANDLER_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_tool_box.h"
#include "lenmus_tool_group.h"
#include "lenmus_tool_page_barlines.h"

//lomse
#include <lomse_internal_model.h>
#include <lomse_im_note.h>
#include <lomse_pitch.h>        //EAccidentals
#include <lomse_events.h>
#include <lomse_interactor.h>
#include <lomse_selections.h>
#include <lomse_document_cursor.h>
using namespace lomse;


//#include <wx/docview.h>
//class wxTipWindow;

namespace lenmus
{

//forward declarations
class DocumentWindow;
class ClickHandler;

//#define lmUNSELECT      false       //remove selection
//#define lmSELECT        true        //select objects
//





//---------------------------------------------------------------------------------------
// Helper class for generating and executing Lomse commands
class CommandGenerator
{
protected:
    DocumentWindow* m_pController;
    SelectionSet* m_selection;
    DocCursor* m_cursor;

public:
    CommandGenerator(DocumentWindow* pController, SelectionSet* selection,
                     DocCursor* cursor);
    ~CommandGenerator() {}


    //add/insert commands
    void add_chord_note(const string& stepLetter, int octave);
    void add_tie();
//	void AddTitle();
//    void AttachNewText(lmComponentObj* pCO);
    void add_tuplet();
    void insert_barline(int barlineType);
    void insert_clef(int clefType, int staff=0);
//	void InsertFiguredBass();
//	void InsertFiguredBassLine();
    void insert_note(string stepLetter, int octave, EAccidentals acc,
                     ENoteType noteType, int dots,
                     int voice, int staff);
    void insert_rest(ENoteType noteType, int dots, int voice, int staff);
    void insert_staffobj(string ldpSrc, string name="");
//    void InsertTimeSignature(int nBeats, int nBeatType, bool fVisible = true);    //for type eTS_Normal
//    void InsertKeySignature(int nFifths, bool fMajor, bool fVisible = true);

    //delete commands
	void delete_staffobj();
    void delete_tie();
    void delete_tuplet();

    //change/move commands
    void change_attribute(ImoObj* pImo, int attrb, int newValue);
    void change_attribute(ImoObj* pImo, int attrb, Color newValue);
    void change_attribute(ImoObj* pImo, int attrb, double newValue);
    void change_attribute(ImoObj* pImo, int attrb, const string& newValue);
//	void ChangeBarline(lmBarline* pBL, lmEBarline nBarlineType, bool fVisible);
	void change_dots(int dots);
//    void ChangeFiguredBass(lmFiguredBass* pFB, wxString& sFigBass);
//    void ChangeMidiSettings(lmInstrument* pInstr, int nMidiChannel, int nMidiInstr);
	void change_note_accidentals(EAccidentals nAcc);
//	void ChangeNotePitch(int nSteps);
//    void ChangePageMargin(lmGMObject* pGMO, int nIdx, int nPage, lmLUnits uPos);
//    void ChangeText(lmScoreText* pST, wxString sText, lmEHAlign nAlign,
//                    lmLocation tPos, lmTextStyle* pStyle, int nHintOptions=0);
//	void MoveNote(lmGMObject* pGMO, const lmUPoint& uPos, int nSteps);
//	void MoveObject(lmGMObject* pGMO, const lmUPoint& uPos);
    void move_object_point(int iPoint, UPoint newPos);
    void move_caret_to_click_point(SpEventMouse event);
    void break_beam();
    void join_beam();
    void toggle_stem();

    //selection commands
    void delete_selection();
    void select_object(ImoId id, bool fClearSelection=true);

};


//---------------------------------------------------------------------------------------
// CommandEventHandler
//    Responsible for handling toolbox events, keyboard events, mouse clicks events and
//    lomse cmmand events (control point moved events), generating commands for doing
//    the implicated actions, such as:
//        * changing view options (zooming, scrolling)
//        * editing the document
//        * modifiying toolbox selected options
//
class CommandEventHandler
{
private:
    ApplicationScope& m_appScope;
    DocumentWindow* m_pController;
    ToolsInfo& m_toolsInfo;
    SelectionSet* m_selection;
    DocCursor* m_cursor;
    bool m_fEventProcessed;
    CommandGenerator m_executer;

public:
    CommandEventHandler(ApplicationScope& appScope, DocumentWindow* pController,
                        ToolsInfo& toolsInfo, SelectionSet* selection, DocCursor* cursor);
    ~CommandEventHandler();

    void process_tool_event(EToolID toolID, ToolBox* pToolBox);
    void process_page_changed_in_toolbox_event(ToolBox* pToolBox);
    void process_key_event(wxKeyEvent& event);
    void process_on_click_event(SpEventMouse event);
    void process_command_event(SpEventCommand event);

    inline bool event_processed() { return m_fEventProcessed; }

protected:
    int m_key;
    int m_keyFlags;
    int m_keyCmd;

    void command_on_selection(EToolID toolID);
    void command_on_caret_pointed_object(EToolID toolID);
    void check_commands_for_current_toolbox_context();
    void check_always_valid_edition_commands();
//    ClickHandler* new_click_handler_for_current_context();
    void delete_selection_or_pointed_object();
    void move_caret_to_click_point(SpEventMouse event);
    void switch_interactor_mode_for_current_mouse_mode();
    void set_drag_image_for_tool(EToolID toolID);
    void common_tasks_for_toolbox_event(ToolBox* pToolBox);
    void enter_top_level_and_edit();
    void move_cursor_up_down();
    void add_note(string step);
    void add_chord_note(string step);
    void ask_and_add_clef();

};


//=======================================================================================
// Handler for mouse clicks
//=======================================================================================
class ClickHandler
{
protected:
    DocumentWindow* m_pController;
    ToolsInfo& m_toolsInfo;
    SelectionSet* m_selection;
    DocCursor* m_cursor;
    bool m_fEventProcessed;
    CommandGenerator m_executer;

public:
    ClickHandler(DocumentWindow* pController, ToolsInfo& toolsInfo,
                 SelectionSet* selection, DocCursor* cursor);
    virtual ~ClickHandler() {}

    void process_click(SpEventMouse event);
    inline bool event_processed() { return m_fEventProcessed; }

protected:
    void add_barline(SpEventMouse event);
    void add_clef(SpEventMouse event);
    void add_note_rest(SpEventMouse event);

};


////Abstract class. All controllers must derive from it
//class DocumentWindow : public wxWindow
//{
//   wxDECLARE_ABSTRACT_CLASS(DocumentWindow);
//
//public:
//    DocumentWindow(wxWindow *pParent, lmScoreView *pView, lmDocument* pDoc,
//				 wxColor colorBg, wxWindowID id = wxID_ANY,
//				 const wxPoint& pos = wxDefaultPosition,
//				 const wxSize& size = wxDefaultSize, long style = 0);
//	virtual ~DocumentWindow();
//
//	//commands without Do/Undo support
//	virtual void PlayScore(bool fFromCursor=false, bool fCountOff=false) {}
//    virtual void StopPlaying(bool fWait=false) {}
//    virtual void PausePlaying() {}
//
//
//
//    // event handlers
//    virtual void OnKeyPress(wxKeyEvent& event) { event.Skip(); }
//    virtual void OnKeyDown(wxKeyEvent& event) { event.Skip(); }
//	virtual void OnEraseBackground(wxEraseEvent& event);
//
//	//contextual menus
//	void ShowContextualMenu(lmScoreObj* pOwner, lmGMObject* pGMO, wxMenu* pMenu, int x, int y);
//	virtual wxMenu* GetContextualMenu(bool fInitialize = true);
//
//	//event handlers for contextual menus
//	virtual void OnCut(wxCommandEvent& event) {}
//    virtual void OnCopy(wxCommandEvent& event) {}
//    virtual void OnPaste(wxCommandEvent& event) {}
//    virtual void OnColor(wxCommandEvent& event) {}
//    virtual void OnProperties(wxCommandEvent& event) {}
//    virtual void OnDeleteTiePrev(wxCommandEvent& event) {}
//    virtual void OnAttachText(wxCommandEvent& event) {}
//	virtual void OnScoreTitles(wxCommandEvent& event) {}
//	virtual void OnViewPageMargins(wxCommandEvent& event) {}
//    virtual void OnToggleStem(wxCommandEvent& event) {}
////#ifdef _LM_DEBUG_
//	virtual void OnDumpShape(wxCommandEvent& event) {}
////#endif
//
//	//event handlers for ToolBox contextual menus
//    virtual void OnToolPopUpMenuEvent(wxCommandEvent& event) {}
//
//    //call backs
//    virtual void SynchronizeToolBox() {}
//    virtual void RestoreToolBoxSelections() {}
//
//
//
//protected:
//	wxMenu*			m_pMenu;			//contextual menu
//	lmScoreObj*		m_pMenuOwner;		//contextual menu owner
//	lmGMObject*		m_pMenuGMO;			//graphic object who displayed the contextual menu
//	ToolBox*      m_pToolBox;         //the associated toolbox, if any
//    int             m_nOctave;          //current octave for note insertion
//	int				m_nVoice;			//current voice for note insertion
//
//private:
//
//	wxDECLARE_EVENT_TABLE();
//};
//
//
//
////-----------------------------------------------------------------------------------------------
//// lmScoreCanvas: Controller for score edition
////-----------------------------------------------------------------------------------------------
//
////Valid areas for mouse interaction
//#define lmMOUSE_OnStaff         0x0001      //pointing to an staff shape
//#define lmMOUSE_OnAboveStaff    0x0002      //pointing to top margin of lmBoxSliceInstr
//#define lmMOUSE_OnBelowStaff    0x0004      //pointing to bottom margin of lmBoxSliceInstr
//#define lmMOUSE_OnOtherShape    0x0008      //pointing to a shape other than staff
//#define lmMOUSE_OnOtherBox      0x0010      //pointing to a box, other cases
//#define lmMOUSE_OnOther         0x0020      //pointing to other place (score paper)
//#define lmMOUSE_OnNotesRests    0x0040      //pointing to a note or a rest
//#define lmMOUSE_OnAny           0xFFFF      //pointing to any place (all score is valid)
//
//
//
////enum to define function/tool assigned to the mouse
//enum lmEMouseFunction
//{
//    lm_eMouse_Select = 0,           //pointer: select, drag
//    lm_eMouse_Tool_Note,            //insert note
//};
//
//
//
//class lmScoreCanvas : public DocumentWindow
//{
//	wxDECLARE_DYNAMIC_CLASS(lmScoreCanvas);
//
//public:
//
//    // constructors and destructor
//    lmScoreCanvas(lmScoreView *pView, wxWindow *pParent, lmDocument* pDoc,
//                  const wxPoint& pos, const wxSize& size, long style, wxColor colorBg);
//    ~lmScoreCanvas();
//
//    // event handlers
//    void OnPaint(wxPaintEvent &WXUNUSED(event));
//    void OnMouseEvent(wxMouseEvent& event);
//    void OnEraseBackground(wxEraseEvent& event);
//    void OnVisualHighlight(lmScoreHighlightEvent& event);
//	void OnKeyPress(wxKeyEvent& event);
//    void OnKeyDown(wxKeyEvent& event);
//    void on_tool_selected_in_toolbox(ToolBoxToolSelectedEvent& event);
//    void on_page_changed_in_toolbox(ToolBoxPageChangedEvent& event);
////#ifdef _LM_WINDOWS_
//    void OnMouseCaptureLost(wxMouseCaptureLostEvent& event);
////#endif
//
//	//commands without Do/Undo support
//    void PlayScore(bool fFromCursor=false, bool fCountOff=false);
//    void StopPlaying(bool fWait=false);
//    void PausePlaying();
//
//    //mouse processing
//    void CaptureTheMouse();
//    void ReleaseTheMouse();
//    void StartToolDrag(wxDC* pDC);
//    void ContinueToolDrag(wxMouseEvent& event, wxDC* pDC);
//    void TerminateToolDrag(wxDC* pDC);
//    void StartToolDrag();
//    void TerminateToolDrag();
//
//    //call backs
//    void SynchronizeToolBox();
//    void RestoreToolBoxSelections();
//    void OnViewUpdated();
//    void OnNewGraphicalModel();
//
//	//contextual menus
//	wxMenu* GetContextualMenu(bool fInitialize = true);
//
//	//event handlers for contextual menus
//	void OnCut(wxCommandEvent& event);
//    void OnCopy(wxCommandEvent& event);
//    void OnPaste(wxCommandEvent& event);
//    void OnColor(wxCommandEvent& event);
//    void OnProperties(wxCommandEvent& event);
//    void OnDeleteTiePrev(wxCommandEvent& event);
//    void OnAttachText(wxCommandEvent& event);
//    void OnScoreTitles(wxCommandEvent& event);
//    void OnViewPageMargins(wxCommandEvent& event);
//    void OnToggleStem(wxCommandEvent& event);
//#ifdef _LM_DEBUG_
//	void OnDumpShape(wxCommandEvent& event);
//#endif
//
//	//event handlers for ToolBox contextual menus
//    void OnToolPopUpMenuEvent(wxCommandEvent& event);
//
//    //call backs from lmScoreView to paint marks for mouse dragged tools
//    lmUPoint OnDrawToolMarks(lmPaper* pPaper, const lmUPoint& uPos);
//    lmUPoint OnRedrawToolMarks(lmPaper* pPaper, const lmUPoint& uPos);
//    lmUPoint OnRemoveToolMarks(lmPaper* pPaper, const lmUPoint& uPos);
//
//private:
//
//    //mouse cursors
//    enum lmEMouseCursor
//    {
//        lm_eCursor_Pointer = 0,     //Must start at 0. See LoadAllMouseCursors()
//        lm_eCursor_Cross,
//        lm_eCursor_BullsEye,
//        lm_eCursor_Hand,
//        lm_eCursor_Note,
//        lm_eCursor_Note_Forbidden,
//        //
//        lm_eCursor_Max              //the last item.
//    };
//
//    //flags to control tool marks while dragging
//    #define lmMARK_NONE             0L
//    #define lmMARK_TIME_GRID        1L
//    #define lmMARK_LEDGER_LINES     2L
//    #define lmMARK_MEASURE          4L
//
//    //values that depend on selected tool
//    wxMenu* GetContextualMenuForTool();
//    void UpdateValidAreasAndMouseIcons();
//
//    //dealing with mouse events
//    void OnMouseEventToolMode(wxMouseEvent& event, wxDC* pDC);
//    void OnMouseEventSelectMode(wxMouseEvent& event, wxDC* pDC);
//    void GetPointedAreaInfo();
//
//    //mouse tools operations
//    void PrepareToolDragImages();
//    void UpdateToolInfoString();
//    void UpdateStatusBarToolBox(wxString sMoreInfo = "");
//    void OnToolClick(lmGMObject* pGMO, lmUPoint uPagePos, float rGridTime);
//    void OnToolNotesClick(lmGMObject* pGMO, lmUPoint uPagePos, float rGridTime);
//    void OnToolClefsClick(lmGMObject* pGMO, lmUPoint uPagePos, float rGridTime);
//    void OnToolSymbolsClick(lmGMObject* pGMO, lmUPoint uPagePos, float rGridTime);
//    void OnToolHarmonyClick(lmGMObject* pGMO, lmUPoint uPagePos, float rGridTime);
//    void OnToolTimeSignatureClick(lmGMObject* pGMO, lmUPoint uPagePos, float rGridTime);
//    void OnToolKeySignatureClick(lmGMObject* pGMO, lmUPoint uPagePos, float rGridTime);
//    void OnToolBarlinesClick(lmGMObject* pGMO, lmUPoint uPagePos, float rGridTime);
//
//    //mouse cursors
//    wxCursor* LoadMouseCursor(wxString sFile, int nHotSpotX, int nHotSpotY);
//    void LoadAllMouseCursors();
//    wxCursor* GetMouseCursor(lmEMouseCursor nCursorID);
//    void ChangeMouseIcon();
//
//    void SynchronizeToolBoxWithSelection(bool fEnable = true);
//    void SynchronizeToolBoxWithCaret(bool fEnable = true);
//
//    bool TileBitmap(const wxRect& rect, wxDC& dc, wxBitmap& bitmap);
//    void LogKeyEvent(wxString name, wxKeyEvent& event, int nTool=-1);
//    wxString KeyCodeToName(int nKeyCode);
//
//    //tools' selection
//    void SelectNoteDuration(int iButton);
//    void SelectNoteAccidentals(bool fNext);
//    void SelectNoteDots(bool fNext);
//    void SelectOctave(bool fUp);
//    void SelectOctave(int nOctave);
//    void SelectVoice(bool fUp);
//    void SelectVoice(int nVoice);

//    //caret positioning
//    void MoveCursorTo(lmBoxSliceInstr* pBSI, int nStaff, float rTime, bool fEndOfTime);
//    void MoveCursorNearTo(lmBoxSliceInstr* pBSI, lmUPoint uPagePos, int nStaff);
//
//    //dragging
//    void SetDraggingObject(bool fValue);
//
//    //helper methods to determine which drag marks to render
//    inline bool RequiresTimeGrid() { return (m_nToolMarks & lmMARK_TIME_GRID) != 0L; }
//    inline bool RequiresLedgerLines() { return (m_nToolMarks & lmMARK_LEDGER_LINES) != 0L; }
//    inline bool RequiresMeasureFrame() { return (m_nToolMarks & lmMARK_MEASURE) != 0L; }
//
//    //other helper methods
//    lmDPitch GetNotePitchFromPosition(lmShapeStaff* pShapeStaff, lmUPoint uPagePos);
//
//
//    lmScoreView*    m_pView;            //owner view
//    wxWindow*       m_pOwner;           //parent window
//    lmDocument*     m_pDoc;             //the document rendered by the view
//
//    wxColour        m_colorBg;			//colour for background
//
//    //mouse mode
//    int    m_nMouseMode;
//
//    //mouse cursors
//    std::vector<wxCursor*>  m_MouseCursors;             //array. Indexes are enum lmEMouseCursor
//    wxCursor*               m_pCursorOnSelectedObject;  //mouse cursors to use
//    wxCursor*               m_pCursorOnValidArea;
//    wxCursor*               m_pCursorElse;
//    wxCursor*               m_pCursorCurrent;           //current displayed mouse cursor
//    long                    m_nValidAreas;              //flags defining valid areas
//    wxBitmap*               m_pToolBitmap;
//    lmDPoint                m_vToolHotSpot;             //hot spot for m_pToolBitmap
//
//    //flags and data to control tool marks while dragging
//    unsigned long                   m_nToolMarks;        //flags defining marks
//
//	//to control octave when inserting several consecutive notes
//	bool			m_fInsertionSequence;
//	int				m_nLastOctave;
//
//    //to sore information of key down event in order to deal properly with key codes
//    int             m_nKeyDownCode;     //key code when KEY_DOWN event
//    bool            m_fCmd;             //Ctrol pressed
//    bool            m_fAlt;             //Alt pressed
//    bool            m_fShift;           //Shift pressed
//
//    //buffer for commands requiring several keystrokes
//    wxString        m_sCmd;
//
//
//    //to save options selected by user in ToolBox
//    bool            m_fToolBoxSavedOptions;
//    int             m_nTbAcc;
//    int             m_nTbDots;
//    int             m_nTbDuration;
//
//    //current values selected in ToolBox
//	ToolBox*      m_pToolBox;
//    lmEToolPageID   pageID;
//    lmEToolGroupID  m_nGroupID;
//    lmEToolID       m_nToolID;
//        //PageNotes
//    lmENoteType     m_nSelNoteType;
//	int             m_nSelDots;
//	lmENoteHeads    m_nSelNotehead;
//	lmEAccidentals  m_nSelAcc;
//	int             m_nSelOctave;
//	int             m_nSelVoice;
//    bool            m_fSelIsNote;
//        //Page Clefs
//    lmEClefType     m_nClefType;
//
//    //temporary data for OnMouseEvent method
//
//    lmDPoint     m_vStartDrag;       //initial point (pixels) of dragging area
//    lmDPoint     m_vEndDrag;         //last end point (pixels) of dragging area
//    lmUPoint     m_uStartDrag;       //initial point (logical, page origin) of dragging area
//    bool         m_fDraggingObject;  //dragging an object
//    bool         m_fCheckTolerance;  //to control false dragging starts
//
//    // As wxDragImage works with unscrolled device coordinates, we need current
//    // maouse position in device units (pixels). All device coordinates are
//    // referred to the lmScoreCanvas window.
//    // We also need logical coordinates of point pointed by mouse
//
//    lmUPoint     m_uMousePagePos;        //position (lmLUnits) referred to current page origin
//    lmDPoint     m_vMousePagePos;        //position (pixels) referred to current page origin
//    lmDPoint     m_vMouseCanvasPos;      //position referred to canvas org. (pixels)
//    lmDPoint     m_vCanvasOffset;        //canvas: offset referred to view origin
//    lmDPoint     m_vPageOrg;             //origin (pixels) of current page referred to view origin
//
//    //other information related to mouse pointed point
//    int          m_nNumPage;             //score page number (1..n) on which the mouse is placed
//
//    //dragging a tool
//    lmShapeStaff*   m_pLastShapeStaff;      //last staff for point pointed by mouse
//    lmBoxSliceInstr* m_pLastBSI;            //last BoxSliceInstr pointed by mouse
//    bool            m_fDraggingTool;        //dragging a tool
//    lmInfoWindow*   m_pInfoWindow;
//
//    //mouse position related info ( updated in GetPointedAreaInfo() )
//    lmShapeStaff*   m_pCurShapeStaff;       //current staff for point pointed by mouse
//    lmBoxSliceInstr* m_pCurBSI;             //current BoxSliceInstr pointed by mouse
//    lmGMObject*     m_pCurGMO;              //current GMObjcet pointed by mouse
//    long            m_nMousePointedArea;    //type of area pointed by mouse
//    float           m_rCurGridTime;         //time for current mouse position
//
//
//
//    //new--------------------------------------------------------------
//	//dragging on canvas with left button: selection
//	void OnCanvasBeginDragLeft(lmDPoint vCanvasPos, lmUPoint uPagePos, int nKeys);
//	void OnCanvasContinueDragLeft(bool fDraw, lmDPoint vCanvasPos, lmUPoint uPagePos, int nKeys);
//	void OnCanvasEndDragLeft(lmDPoint vCanvasPos, lmUPoint uPagePos, int nKeys);
//
//	//dragging on canvas with right button
//	void OnCanvasBeginDragRight(lmDPoint vCanvasPos, lmUPoint uPagePos, int nKeys);
//	void OnCanvasContinueDragRight(bool fDraw, lmDPoint vCanvasPos, lmUPoint uPagePos, int nKeys);
//	void OnCanvasEndDragRight(lmDPoint vCanvasPos, lmUPoint uPagePos, int nKeys);
//
//	//dragging object with left button
//	void OnObjectBeginDragLeft(wxMouseEvent& event, wxDC* pDC, lmDPoint vCanvasPos,
//							   lmDPoint vCanvasOffset, lmUPoint uPagePos, int nKeys);
//	void OnObjectContinueDragLeft(wxMouseEvent& event, wxDC* pDC, bool fDraw,
//								  lmDPoint vCanvasPos, lmDPoint vCanvasOffset,
//								  lmUPoint uPagePos, int nKeys);
//	void OnObjectEndDragLeft(wxMouseEvent& event, wxDC* pDC, lmDPoint vCanvasPos,
//							 lmDPoint vCanvasOffset, lmUPoint uPagePos, int nKeys);
//
//	//dragging object with right button
//	void OnObjectBeginDragRight(wxMouseEvent& event, wxDC* pDC, lmDPoint vCanvasPos,
//							   lmDPoint vCanvasOffset, lmUPoint uPagePos, int nKeys);
//	void OnObjectContinueDragRight(wxMouseEvent& event, wxDC* pDC, bool fDraw,
//								  lmDPoint vCanvasPos, lmDPoint vCanvasOffset,
//								  lmUPoint uPagePos, int nKeys);
//	void OnObjectEndDragRight(wxMouseEvent& event, wxDC* pDC, lmDPoint vCanvasPos,
//							 lmDPoint vCanvasOffset, lmUPoint uPagePos, int nKeys);
//
//	//non-dragging events: click on an object
//	void OnLeftClickOnObject(lmGMObject* pGMO, lmDPoint vCanvasPos, lmUPoint uPagePos, int nKeys);
//	void OnLeftDoubleClickOnObject(lmGMObject* pGMO, lmDPoint vCanvasPos, lmUPoint uPagePos, int nKeys);
//	void OnRightClickOnObject(lmGMObject* pGMO, lmDPoint vCanvasPos, lmUPoint uPagePos, int nKeys);
//	void OnRightDoubleClickOnObject(lmGMObject* pGMO, lmDPoint vCanvasPos, lmUPoint uPagePos, int nKeys);
//
//	//non-dragging events: click on canvas
//	void OnRightClickOnCanvas(lmDPoint vCanvasPos, lmUPoint uPagePos, int nKeys);
//	void OnLeftClickOnCanvas(lmDPoint vCanvasPos, lmUPoint uPagePos, int nKeys);
//
//    // dragging control variables
//    int             m_nDragState;
//    lmUPoint        m_uDragStartPos;
//    lmDPoint        m_vDragHotSpot;			//absolute point (pixels)
//    lmUPoint        m_uHotSpotShift;		//distance from shape origin
//	lmGMObject*		m_pDraggedGMO;			//GMObject being dragged
//
//    // mouse over objects
//	lmGMObject*		m_pMouseOverGMO;        //GMObject on which mouse is flying over
//
//    wxDECLARE_EVENT_TABLE();
//};
//
//
////---------------------------------------------------------------------------
//// Helper class to define edition modes
////---------------------------------------------------------------------------
//
//class lmEditorMode
//{
//public:
//    lmEditorMode(wxClassInfo* pControllerInfo, wxClassInfo* pScoreProcInfo);
//    lmEditorMode(wxString& sCreationMode, wxString& sCreationVers);
//    ~lmEditorMode();
//
//    void CustomizeToolBoxPages(ToolBox* pToolBox);
//    lmScoreProcessor* CreateScoreProcessor();
//
//    //ToolBox reated
//    void ChangeToolPage(int nPageID, wxClassInfo* pToolPageInfo);
//
//    //Creation mode information
//    inline void SetModeName(const wxString& sName) { m_sCreationModeName = sName; }
//    inline void SetModeVers(const wxString& sVers) { m_sCreationModeVers = sVers; }
//    inline wxString& GetModeName() { return m_sCreationModeName; }
//    inline wxString& GetModeVers() { return m_sCreationModeVers; }
//
//protected:
//
//    wxString                m_sCreationModeName;
//    wxString                m_sCreationModeVers;
//    wxClassInfo*            m_pControllerInfo;
//    wxClassInfo*            m_pScoreProcInfo;
//    wxClassInfo*            m_ToolPagesInfo[lmPAGE_MAX];
//    lmScoreProcessor*       m_pScoreProc;
//};
//
//

}   // namespace lenmus


#endif  // __LENMUS_COMMAND_EVENT_HANDLER_H__
