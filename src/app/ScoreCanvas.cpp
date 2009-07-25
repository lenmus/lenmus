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
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "ScoreCanvas.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif


#include "global.h"
#include "TheApp.h"
#include "MainFrame.h"
#include "ScoreDoc.h"
#include "ScoreView.h"
#include "ScoreDoc.h"
#include "ScoreCommand.h"
#include "Processor.h"
#include "ArtProvider.h"        // to use ArtProvider for managing icons
#include "toolbox/ToolNotes.h"
#include "toolbox/ToolBoxEvents.h"
#include "global.h"
#include "KbdCmdParser.h"
#include "../ldp_parser/LDPParser.h"
#include "../ldp_parser/AuxString.h"
#include "../graphic/GMObject.h"
#include "../graphic/BoxSystem.h"
#include "../graphic/ShapeStaff.h"
#include "../graphic/BoxSliceInstr.h"
#include "../score/VStaff.h"
#include "../score/Staff.h"
#include "../score/Context.h"
#include "../score/Clef.h"
#include "../score/properties/DlgProperties.h"
#include "DlgDebug.h"

//access to logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;

// access to paths
#include "../globals/Paths.h"
extern lmPaths* g_pPaths;

// access to global external variables (to disable mouse interaction with the score)
extern bool g_fReleaseVersion;            // in TheApp.cpp
extern bool g_fReleaseBehaviour;        // in TheApp.cpp

//do not re-draw the score after executing the command
#define lmNO_REDRAW    false


//-------------------------------------------------------------------------------------
// implementation of lmController
//-------------------------------------------------------------------------------------



BEGIN_EVENT_TABLE(lmController, wxEvtHandler)
	EVT_CHAR(lmController::OnKeyPress)
	EVT_KEY_DOWN(lmController::OnKeyDown)
    EVT_ERASE_BACKGROUND(lmController::OnEraseBackground)

	//contextual menus
	EVT_MENU	(lmPOPUP_Cut, lmController::OnCut)
    EVT_MENU	(lmPOPUP_Copy, lmController::OnCopy)
    EVT_MENU	(lmPOPUP_Paste, lmController::OnPaste)
    EVT_MENU	(lmPOPUP_Color, lmController::OnColor)
    EVT_MENU	(lmPOPUP_Properties, lmController::OnProperties)
    EVT_MENU	(lmPOPUP_DeleteTiePrev, lmController::OnDeleteTiePrev)
    EVT_MENU	(lmPOPUP_AttachText, lmController::OnAttachText)
    EVT_MENU	(lmPOPUP_Score_Titles, lmController::OnScoreTitles)
    EVT_MENU	(lmPOPUP_View_Page_Margins, lmController::OnViewPageMargins)
#ifdef __WXDEBUG__
    EVT_MENU	(lmPOPUP_DumpShape, lmController::OnDumpShape)
#endif

END_EVENT_TABLE()

IMPLEMENT_ABSTRACT_CLASS(lmController, wxWindow)


lmController::lmController(wxWindow *pParent, lmScoreView *pView, lmDocument* pDoc,
				 wxColor colorBg, wxWindowID id, const wxPoint& pos,
				 const wxSize& size, long style)
        : wxWindow(pParent, -1, pos, size, style)
{
	m_pMenu = (wxMenu*)NULL;		//no contextual menu
}

lmController::~lmController()
{
	if (m_pMenu) delete m_pMenu;
}

void lmController::OnEraseBackground(wxEraseEvent& event)
{
	// When wxWidgets wants to update the display it emits two events: an erase
	// background event and a paint event.
	// We are going to intercept the Erase Background event in order to prevent
	// that the default implementation in wxWindow erases the background, as this
	// will cause flickering
}


void lmController::ShowContextualMenu(lmScoreObj* pOwner, lmGMObject* pGMO, wxMenu* pMenu,
									  int x, int y)
{
	m_pMenuOwner = pOwner;
	m_pMenuGMO = pGMO;
	PopupMenu(pMenu, x, y);
}

wxMenu* lmController::GetContextualMenu(bool fInitialize)
{
	return (wxMenu*)NULL;
}

//-------------------------------------------------------------------------------------
// implementation of lmScoreCanvas
//-------------------------------------------------------------------------------------

IMPLEMENT_CLASS(lmScoreCanvas, lmController)

// keys pressed when a mouse event
enum {
    lmKEY_NONE = 0x0000,
    lmKEY_ALT = 0x0001,
    lmKEY_CTRL = 0x0002,
    lmKEY_SHIFT = 0x0004,
};

// Dragging states
enum
{
	lmDRAG_NONE = 0,
	lmDRAG_START_LEFT,
	lmDRAG_CONTINUE_LEFT,
	lmDRAG_START_RIGHT,
	lmDRAG_CONTINUE_RIGHT,
};




BEGIN_EVENT_TABLE(lmScoreCanvas, lmController)
    EVT_ERASE_BACKGROUND(lmScoreCanvas::OnEraseBackground)
    EVT_MOUSE_EVENTS(lmScoreCanvas::OnMouseEvent)
    EVT_PAINT(lmScoreCanvas::OnPaint)
    LM_EVT_SCORE_HIGHLIGHT(lmScoreCanvas::OnVisualHighlight)
    LM_EVT_TOOLBOX_TOOL_SELECTED(lmScoreCanvas::OnToolBoxEvent)
    LM_EVT_TOOLBOX_PAGE_CHANGED(lmScoreCanvas::OnToolBoxPageChanged)

#ifdef __WXMSW__
    //This event is currently emitted under Windows only
    EVT_MOUSE_CAPTURE_LOST(lmScoreCanvas::OnMouseCaptureLost)
#endif

END_EVENT_TABLE()

// Define a constructor for my canvas
lmScoreCanvas::lmScoreCanvas(lmScoreView *pView, wxWindow *pParent, lmDocument* pDoc,
                             const wxPoint& pos, const wxSize& size, long style, wxColor colorBg)
    : lmController(pParent, pView, pDoc, colorBg, wxID_ANY, pos, size, style)
    , m_nEntryMode(lm_DATA_ENTRY_KEYBOARD)
    , m_pView(pView)
    , m_pOwner(pParent)
    , m_pDoc(pDoc)
    , m_colorBg(colorBg)
    , m_pCursorCurrent((wxCursor*)NULL)
    , m_pToolBitmap((wxBitmap*)NULL)
    , m_pLastShapeStaff((lmShapeStaff*)NULL)
    , m_pLastBSI((lmBoxSliceInstr*)NULL)
{
	//attach the edit menu to the command processor
	m_pDoc->GetCommandProcessor()->SetEditMenu( GetMainFrame()->GetEditMenu() );

    //initializations
    m_nOctave = 4;      //start in octave 4
    m_sCmd = _T("");
    m_fCmd = false;
    m_fAlt = false;
    m_fShift = false;
    m_fToolBoxSavedOptions = false;

    // drag state control initializations
    m_fDraggingTool = false;
    m_nDragState = lmDRAG_NONE;
	m_vEndDrag = lmDPoint(0, 0);
	m_vStartDrag.x = 0;
	m_vStartDrag.y = 0;
	m_fCheckTolerance = true;

    //mouse cursor images
    LoadAllMouseCursors();

    //mouse over
    m_pMouseOverGMO = (lmGMObject*)NULL;
    m_nMousePointedArea = 0;
}

lmScoreCanvas::~lmScoreCanvas()
{
    //delete mouse cursors
    std::vector<wxCursor*>::iterator it;
    for (it = m_MouseCursors.begin(); it != m_MouseCursors.end(); ++it)
        delete *it;

    //delete cursor drag images
    if (m_pToolBitmap)
        delete m_pToolBitmap;
}

void lmScoreCanvas::DoCaptureMouse()
{
    wxLogMessage(_T("[lmScoreCanvas::DoCaptureMouse]"));
    //CaptureMouse();
}

void lmScoreCanvas::DoReleaseMouse()
{ 
    wxLogMessage(_T("[lmScoreCanvas::DoReleaseMouse] HasCapture=%s"),
                 (HasCapture() ? _T("yes") : _T("no")) );
    //if (HasCapture()) 
    //    ReleaseMouse();
}

#ifdef __WXMSW__
void lmScoreCanvas::OnMouseCaptureLost(wxMouseCaptureLostEvent& event)
{
}
#endif

void lmScoreCanvas::OnPaint(wxPaintEvent &WXUNUSED(event))
{
    // In a paint event handler, the application must always create a wxPaintDC object,
    // even if it is not used. Otherwise, under MS Windows, refreshing for this and
    // other windows will go wrong.
    wxPaintDC dc(this);
    if (!m_pView) return;

    // get the updated rectangles list
    wxRegionIterator upd(GetUpdateRegion());

    // iterate to redraw each damaged rectangle
    // The rectangles are in pixels, referred to the client area, and are unscrolled
    m_pView->PrepareForRepaint(&dc);
    while (upd)
    {
        wxRect rect = upd.GetRect();
        m_pView->RepaintScoreRectangle(&dc, rect);
        upd++;
    }
    m_pView->TerminateRepaint(&dc);
}

void lmScoreCanvas::OnMouseEvent(wxMouseEvent& event)
{
    //transfer mouse event to the view

    if (!m_pView)
        return;

    wxClientDC dc(this);

        //First, for better performance, filter out non-used events

    //filter out non-treated events
    wxEventType nEventType = event.GetEventType();
    if (nEventType==wxEVT_MIDDLE_DOWN || nEventType==wxEVT_MIDDLE_UP ||
        nEventType==wxEVT_MIDDLE_DCLICK)
    {
        return;
    }


        //Now deal with events that do not require to compute mouse position and/or which graphical
        //object is under the mouse


    // check for mouse entering/leaving the window events
	if (event.Entering())    //wxEVT_ENTER_WINDOW
	{
		//the mouse is entering the window. Change mouse icon as appropriate
		//TODO
		return;
	}
	if (event.Leaving())    //wxEVT_LEAVE_WINDOW
	{
		//the mouse is leaving the window. Change mouse icon as appropriate
		//TODO
		return;
	}

    //deal with mouse wheel events
	if (nEventType == wxEVT_MOUSEWHEEL)
    {
        m_pView->OnMouseWheel(event);
		return;
    }


        //From this point we need information about mouse position. Let's compute it an
        //update GUI (rules markers, status bar, etc.). Get also information about any possible
        //key being pressed whil mouse is moving, and about dragging


	// get mouse point (pixels, referred to lmScoreCanvas origin)
    m_vMouseCanvasPos = event.GetPosition();

    // Set DC in logical units and scaled, so that
    // transformations logical/device and viceversa can be computed
    m_pView->ScaleDC(&dc);

    //compute mouse point in logical units. Get also different origins and values
    bool fInInterpageGap;           //mouse click out of page
	m_pView->DeviceToLogical(m_vMouseCanvasPos, m_uMousePagePos, &m_vMousePagePos,
                             &m_vPageOrg, &m_vCanvasOffset, &m_nNumPage,
                             &fInInterpageGap);

	#ifdef __WXDEBUG__
	bool fDebugMode = g_pLogger->IsAllowedTraceMask(_T("OnMouseEvent"));
	#endif

    //update status bar: mouse position, and num page
    GetMainFrame()->SetStatusBarMousePos((float)m_uMousePagePos.x, (float)m_uMousePagePos.y);
    m_pView->UpdateNumPage(m_nNumPage);

	////for testing and debugging methods DeviceToLogical [ok] and LogicalToDevice [ok]
	//lmDPoint tempPagePosD;
	//LogicalToDevice(tempPagePosL, tempPagePosD);

    // draw markers on the rulers
    m_pView->UpdateRulerMarkers(m_vMousePagePos);

    // check if dragging (moving with a button pressed), and filter out mouse movements small
    //than tolerance
	bool fDragging = event.Dragging();
	if (fDragging && m_fCheckTolerance)
	{
		// Check if we're within the tolerance for mouse movements.
		// If we're very close to the position we started dragging
		// from, this may not be an intentional drag at all.
		lmLUnits uAx = abs(dc.DeviceToLogicalXRel((long)(m_vMouseCanvasPos.x - m_vStartDrag.x)));
		lmLUnits uAy = abs(dc.DeviceToLogicalYRel((long)(m_vMouseCanvasPos.y - m_vStartDrag.y)));
        lmLUnits uTolerance = m_pView->GetMouseTolerance();
		if (uAx <= uTolerance && uAy <= uTolerance)
			return;
		else
            //I will not allow for a second involuntary small movement. Therefore
			//if we have ignored a drag, smaller than tolerance, then do not check for
            //tolerance the next time in this drag.
			m_fCheckTolerance = false;
	}


        //At this point it has been determined all mouse position information. Now we start
        //dealing with mouse moving, mouse clicks and dragging events. Behaviour from this 
        //point is different, depending on data entry mode (using keyboard or using mouse). 
        //Therefore, processing is splitted at this point

    if (m_nEntryMode == lm_DATA_ENTRY_MOUSE)
        OnMouseEventToolMode(event, &dc);
    else
        OnMouseEventSelectMode(event, &dc);

}

void lmScoreCanvas::OnMouseEventSelectMode(wxMouseEvent& event, wxDC* pDC)
{
    //If we reach this point is because it is a mouse dragging or a mouse click event.
    //Let's deal with them.

	#ifdef __WXDEBUG__
	bool fDebugMode = g_pLogger->IsAllowedTraceMask(_T("OnMouseEvent"));
	#endif

	bool fDragging = event.Dragging();

    //determine if mouse is pointing to a shape
    lmGMObject* pGMO = m_pView->FindShapeAt(m_nNumPage, m_uMousePagePos, true);

    //check moving events
    if (event.GetEventType() == wxEVT_MOTION && !fDragging)
    {
	    if (pGMO)
        {
            //Mouse is currently pointing to an object (shape or box)
            if (m_pMouseOverGMO)
            {
                //mouse was previously over an object. If it is the same than current one there is
                //nothing to do
                if (m_pMouseOverGMO == pGMO)
                    return;     //nothing to do. Mouse continues over object

                //It is a new object. Inform previous object that it is left
                m_pMouseOverGMO->OnMouseOut(this, m_uMousePagePos);
            }
            m_pMouseOverGMO = pGMO;
            pGMO->OnMouseIn(this, m_uMousePagePos);
        }

        //mouse is not pointing neither to a shape nor to a box. If mouse was poining to an object
        //inform it that it has been left
        else
        {
            if (m_pMouseOverGMO)
            {
                //mouse was previously over an object. Inform it that it is left
                m_pMouseOverGMO->OnMouseOut(this, m_uMousePagePos);
                m_pMouseOverGMO = (lmGMObject*)NULL;
            }
        }
        return;
    }

    // check if a key is pressed
    int nKeysPressed = lmKEY_NONE;
    if (event.ShiftDown())
        nKeysPressed |= lmKEY_SHIFT;
    if (event.CmdDown())
        nKeysPressed |= lmKEY_CTRL;
    if (event.AltDown())
        nKeysPressed |= lmKEY_ALT;

	if (!fDragging)
	{
		// Non-dragging events.
        // In MS Windows the 'end of drag' event is a non-dragging event

		m_fCheckTolerance = true;

		#ifdef __WXDEBUG__
		if(fDebugMode) g_pLogger->LogDebug(_T("Non-dragging event"));
		#endif

		if (event.IsButton())
		{
			#ifdef __WXDEBUG__
			if(fDebugMode) g_pLogger->LogDebug(_T("button event"));
			#endif

			//find the object pointed with the mouse
			lmGMObject* pGMO = m_pView->FindShapeAt(m_nNumPage, m_uMousePagePos, false);
			if (pGMO) // Object event
			{
				#ifdef __WXDEBUG__
				if(fDebugMode) g_pLogger->LogDebug(_T("button on object event"));
				#endif

				if (event.LeftDown())
				{
					#ifdef __WXDEBUG__
					if(fDebugMode) g_pLogger->LogDebug(_T("button on object: event.LeftDown()"));
					#endif

					//Save data for a possible start of dragging
					m_pDraggedGMO = pGMO;
					m_nDragState = lmDRAG_START_LEFT;
					m_vStartDrag.x = m_vMouseCanvasPos.x;
					m_vStartDrag.y = m_vMouseCanvasPos.y;
                    m_uStartDrag = m_uMousePagePos;

					m_uDragStartPos = m_uMousePagePos;	// save mouse position (page logical coordinates)
					// compute the location of the drag position relative to the upper-left
					// corner of the image (pixels)
					m_uHotSpotShift = m_uMousePagePos - m_pDraggedGMO->GetObjectOrigin();
					m_vDragHotSpot.x = pDC->LogicalToDeviceXRel((int)m_uHotSpotShift.x);
					m_vDragHotSpot.y = pDC->LogicalToDeviceYRel((int)m_uHotSpotShift.y);
				}
				else if (event.LeftUp())
				{
					#ifdef __WXDEBUG__
					if(fDebugMode) g_pLogger->LogDebug(_T("button on object: event.LeftUp()"));
					#endif

			        if (m_nDragState == lmDRAG_CONTINUE_LEFT)
			        {
                        if (m_fDraggingObject)
                        {
                            //draggin. Finish left object dragging
				            OnObjectContinueDragLeft(event, pDC, false, m_vEndDrag, m_vCanvasOffset, m_uMousePagePos, nKeysPressed);
				            OnObjectEndDragLeft(event, pDC, m_vMouseCanvasPos, m_vCanvasOffset, m_uMousePagePos, nKeysPressed);
                        }
                        else
                        {
                            //draggin. Finish left canvas dragging
				            OnCanvasContinueDragLeft(false, m_vEndDrag, m_uMousePagePos, nKeysPressed);
				            OnCanvasEndDragLeft(m_vMouseCanvasPos, m_uMousePagePos, nKeysPressed);
                       }
			        }
                    else
                    {
					    //click on object. Only send a click event if the same object
                        //was involved in 'down' and 'up' events
					    if (pGMO == m_pDraggedGMO)
						    OnLeftClickOnObject(pGMO, m_vMouseCanvasPos, m_uMousePagePos, nKeysPressed);
                    }
					m_pDraggedGMO = (lmGMObject*)NULL;
					m_nDragState = lmDRAG_NONE;
                    m_fCheckTolerance = true;
				}
				else if (event.LeftDClick())
				{
					#ifdef __WXDEBUG__
					if(fDebugMode) g_pLogger->LogDebug(_T("button on object: event.LeftDClick()"));
					#endif

					OnLeftDoubleClickOnObject(pGMO, m_vMouseCanvasPos, m_uMousePagePos, nKeysPressed);
					m_pDraggedGMO = (lmGMObject*)NULL;
					m_nDragState = lmDRAG_NONE;
				}
				else if (event.RightDown())
				{
					#ifdef __WXDEBUG__
					if(fDebugMode) g_pLogger->LogDebug(_T("button on object: event.RightDown()"));
					#endif

					//Save data for a possible start of dragging
					m_pDraggedGMO = pGMO;
					m_nDragState = lmDRAG_START_RIGHT;
					m_vStartDrag.x = m_vMouseCanvasPos.x;
					m_vStartDrag.y = m_vMouseCanvasPos.y;
                    m_uStartDrag = m_uMousePagePos;

					m_uDragStartPos = m_uMousePagePos;	// save mouse position (page logical coordinates)
					// compute the location of the drag position relative to the upper-left
					// corner of the image (pixels)
					m_uHotSpotShift = m_uMousePagePos - m_pDraggedGMO->GetObjectOrigin();
					m_vDragHotSpot.x = pDC->LogicalToDeviceXRel((int)m_uHotSpotShift.x);
					m_vDragHotSpot.y = pDC->LogicalToDeviceYRel((int)m_uHotSpotShift.y);
				}
				else if (event.RightUp())
				{
					#ifdef __WXDEBUG__
					if(fDebugMode) g_pLogger->LogDebug(_T("button on object: event.RightUp()"));
					#endif

			        if (m_nDragState == lmDRAG_CONTINUE_RIGHT)
			        {
                        if (m_fDraggingObject)
                        {
                            //draggin. Finish right object dragging
				            OnObjectContinueDragRight(event, pDC, false, m_vEndDrag, m_vCanvasOffset, m_uMousePagePos, nKeysPressed);
				            OnObjectEndDragRight(event, pDC, m_vMouseCanvasPos, m_vCanvasOffset, m_uMousePagePos, nKeysPressed);
                        }
                        else
                        {
                            //draggin. Finish right canvas dragging
				            OnCanvasContinueDragRight(false, m_vEndDrag, m_uMousePagePos, nKeysPressed);
				            OnCanvasEndDragRight(m_vMouseCanvasPos, m_uMousePagePos, nKeysPressed);
                       }
			        }
                    else
                    {
					    //click on object. Only send a click event if the same object
                        //was involved in 'down' and 'up' events
					    if (pGMO == m_pDraggedGMO)
						    OnRightClickOnObject(pGMO, m_vMouseCanvasPos, m_uMousePagePos, nKeysPressed);
                    }
					m_pDraggedGMO = (lmGMObject*)NULL;
					m_nDragState = lmDRAG_NONE;
                    m_fCheckTolerance = true;
				}
				else if (event.RightDClick())
				{
					#ifdef __WXDEBUG__
					if(fDebugMode) g_pLogger->LogDebug(_T("button on object: event.RightDClick()"));
					#endif

					OnRightDoubleClickOnObject(pGMO, m_vMouseCanvasPos, m_uMousePagePos, nKeysPressed);
					m_pDraggedGMO = (lmGMObject*)NULL;
					m_nDragState = lmDRAG_NONE;
				}
				else
				{
					#ifdef __WXDEBUG__
					if(fDebugMode) g_pLogger->LogDebug(_T("button on object: no identified event"));
					#endif
				}

			}
			else // Canvas event (no pointed object)
			{
				#ifdef __WXDEBUG__
				if(fDebugMode) g_pLogger->LogDebug(_T("button on canvas event"));
				#endif

				if (event.LeftDown())
				{
					#ifdef __WXDEBUG__
					if(fDebugMode) g_pLogger->LogDebug(_T("button on canvas: event.LeftDown()"));
					#endif

					m_pDraggedGMO = (lmGMObject*)NULL;
					m_nDragState = lmDRAG_START_LEFT;
					m_vStartDrag.x = m_vMouseCanvasPos.x;
					m_vStartDrag.y = m_vMouseCanvasPos.y;
                    m_uStartDrag = m_uMousePagePos;
				}
				else if (event.LeftUp())
				{
					#ifdef __WXDEBUG__
					if(fDebugMode) g_pLogger->LogDebug(_T("button on canvas: event.LeftUp()"));
					#endif

			        if (m_nDragState == lmDRAG_CONTINUE_LEFT)
			        {
                        if (m_pDraggedGMO)
                        {
							#ifdef __WXDEBUG__
							if(fDebugMode) g_pLogger->LogDebug(_T("dragging object: Finish left dragging"));
							#endif

	                            //draggin. Finish left dragging
				            OnObjectContinueDragLeft(event, pDC, false, m_vEndDrag, m_vCanvasOffset, m_uMousePagePos, nKeysPressed);
				            OnObjectEndDragLeft(event, pDC, m_vMouseCanvasPos, m_vCanvasOffset, m_uMousePagePos, nKeysPressed);
                        }
                        else
                        {
							#ifdef __WXDEBUG__
							if(fDebugMode) g_pLogger->LogDebug(_T("dragging on canvas: Finish left dragging"));
							#endif

                            //draggin. Finish left dragging
				            OnCanvasContinueDragLeft(false, m_vEndDrag, m_uMousePagePos, nKeysPressed);
				            OnCanvasEndDragLeft(m_vMouseCanvasPos, m_uMousePagePos, nKeysPressed);
                        }
			        }
                    else
                    {
						#ifdef __WXDEBUG__
						if(fDebugMode) g_pLogger->LogDebug(_T("button on canvas: non-dragging. Left click on object"));
						#endif

                        //non-dragging. Left click on object
					    OnLeftClickOnCanvas(m_vMouseCanvasPos, m_uMousePagePos, nKeysPressed);
                    }
					m_pDraggedGMO = (lmGMObject*)NULL;
					m_nDragState = lmDRAG_NONE;
                    m_fCheckTolerance = true;
				}
				else if (event.RightDown())
				{
					#ifdef __WXDEBUG__
					if(fDebugMode) g_pLogger->LogDebug(_T("button on canvas: event.RightDown()"));
					#endif

					m_pDraggedGMO = (lmGMObject*)NULL;
					m_nDragState = lmDRAG_START_RIGHT;
					m_vStartDrag.x = m_vMouseCanvasPos.x;
					m_vStartDrag.y = m_vMouseCanvasPos.y;
                    m_uStartDrag = m_uMousePagePos;
				}
				else if (event.RightUp())
				{
					#ifdef __WXDEBUG__
					if(fDebugMode) g_pLogger->LogDebug(_T("button on canvas: event.RightUp()"));
					#endif

			        if (m_nDragState == lmDRAG_CONTINUE_RIGHT)
			        {
                        if (m_pDraggedGMO)
                        {
							#ifdef __WXDEBUG__
							if(fDebugMode) g_pLogger->LogDebug(_T("dragging object: Finish right dragging"));
							#endif

	                            //draggin. Finish right dragging
				            OnObjectContinueDragRight(event, pDC, false, m_vEndDrag, m_vCanvasOffset, m_uMousePagePos, nKeysPressed);
				            OnObjectEndDragRight(event, pDC, m_vMouseCanvasPos, m_vCanvasOffset, m_uMousePagePos, nKeysPressed);
                        }
                        else
                        {
							#ifdef __WXDEBUG__
							if(fDebugMode) g_pLogger->LogDebug(_T("dragging on canvas: Finish right dragging"));
							#endif

                            //draggin. Finish right dragging
				            OnCanvasContinueDragRight(false, m_vEndDrag, m_uMousePagePos, nKeysPressed);
				            OnCanvasEndDragRight(m_vMouseCanvasPos, m_uMousePagePos, nKeysPressed);
                        }
			        }
                    else
                    {
						#ifdef __WXDEBUG__
						if(fDebugMode) g_pLogger->LogDebug(_T("button on canvas: non-dragging. Right click on object"));
						#endif

                        //non-dragging. Right click on object
					    OnRightClickOnCanvas(m_vMouseCanvasPos, m_uMousePagePos, nKeysPressed);
                    }
					m_pDraggedGMO = (lmGMObject*)NULL;
					m_nDragState = lmDRAG_NONE;
                    m_fCheckTolerance = true;
				}
                else
				{
					#ifdef __WXDEBUG__
					if(fDebugMode) g_pLogger->LogDebug(_T("button on canvas: no identified event"));
					#endif
				}
			}
		}
        else
		{
			#ifdef __WXDEBUG__
			if(fDebugMode) g_pLogger->LogDebug(_T("non-dragging: no button event"));
			#endif
		}
	}

	else	//dragging events
	{
		#ifdef __WXDEBUG__
		if(fDebugMode) g_pLogger->LogDebug(_T("dragging event"));
		#endif

		if (m_pDraggedGMO)
		{
			#ifdef __WXDEBUG__
			if(fDebugMode) g_pLogger->LogDebug(_T("draggin an object"));
			#endif

			//draggin an object
			if (event.LeftUp() && m_nDragState == lmDRAG_CONTINUE_LEFT)
			{
				#ifdef __WXDEBUG__
				if(fDebugMode) g_pLogger->LogDebug(_T("object: event.LeftUp() && m_nDragState == lmDRAG_CONTINUE_LEFT"));
				#endif

				m_nDragState = lmDRAG_NONE;
				m_fCheckTolerance = true;
				OnObjectContinueDragLeft(event, pDC, false, m_vEndDrag, m_vCanvasOffset, m_uMousePagePos, nKeysPressed);
				OnObjectEndDragLeft(event, pDC, m_vMouseCanvasPos, m_vCanvasOffset, m_uMousePagePos, nKeysPressed);
				m_pDraggedGMO = (lmGMObject*)NULL;
			}
			else if (event.RightUp() && m_nDragState == lmDRAG_CONTINUE_RIGHT)
			{
				#ifdef __WXDEBUG__
				if(fDebugMode) g_pLogger->LogDebug(_T("object: event.RightUp() && m_nDragState == lmDRAG_CONTINUE_RIGHT"));
				#endif

				m_nDragState = lmDRAG_NONE;
				m_fCheckTolerance = true;
				OnObjectContinueDragRight(event, pDC, false, m_vEndDrag, m_vCanvasOffset, m_uMousePagePos, nKeysPressed);
				OnObjectEndDragRight(event, pDC, m_vMouseCanvasPos, m_vCanvasOffset, m_uMousePagePos, nKeysPressed);
				m_pDraggedGMO = (lmGMObject*)NULL;
			}
			else if (m_nDragState == lmDRAG_START_LEFT)
			{
				#ifdef __WXDEBUG__
				if(fDebugMode) g_pLogger->LogDebug(_T("object: m_nDragState == lmDRAG_START_LEFT"));
				#endif

				m_nDragState = lmDRAG_CONTINUE_LEFT;

				if (m_pDraggedGMO->IsLeftDraggable())
                {
					OnObjectBeginDragLeft(event, pDC, m_vMouseCanvasPos, m_vCanvasOffset, m_uMousePagePos, nKeysPressed);
                }
				else
				{
					//the object is not draggable: transfer message to canvas
				    #ifdef __WXDEBUG__
				    if(fDebugMode) g_pLogger->LogDebug(_T("object is not left draggable. Drag cancelled"));
				    #endif
					m_pDraggedGMO = (lmGMObject*)NULL;
					OnCanvasBeginDragLeft(m_vStartDrag, m_uMousePagePos, nKeysPressed);
				}
				m_vEndDrag = m_vMouseCanvasPos;
			}
			else if (m_nDragState == lmDRAG_CONTINUE_LEFT)
			{
				#ifdef __WXDEBUG__
				if(fDebugMode) g_pLogger->LogDebug(_T("object: m_nDragState == lmDRAG_CONTINUE_LEFT"));
				#endif

				// Continue dragging
				OnObjectContinueDragLeft(event, pDC, false, m_vEndDrag, m_vCanvasOffset, m_uMousePagePos, nKeysPressed);
				OnObjectContinueDragLeft(event, pDC, true, m_vMouseCanvasPos, m_vCanvasOffset, m_uMousePagePos, nKeysPressed);
				m_vEndDrag = m_vMouseCanvasPos;
			}
			else if (m_nDragState == lmDRAG_START_RIGHT)
			{
				#ifdef __WXDEBUG__
				if(fDebugMode) g_pLogger->LogDebug(_T("object: m_nDragState == lmDRAG_START_RIGHT"));
				#endif

				m_nDragState = lmDRAG_CONTINUE_RIGHT;

				if (m_pDraggedGMO->IsRightDraggable())
				{
					OnObjectBeginDragRight(event, pDC, m_vMouseCanvasPos, m_vCanvasOffset, m_uMousePagePos, nKeysPressed);
                }
				else
				{
					//the object is not draggable: transfer message to canvas
					m_pDraggedGMO = (lmGMObject*)NULL;
					OnCanvasBeginDragRight(m_vStartDrag, m_uMousePagePos, nKeysPressed);
				}
				m_vEndDrag = m_vMouseCanvasPos;
			}
			else if (m_nDragState == lmDRAG_CONTINUE_RIGHT)
			{
				#ifdef __WXDEBUG__
				if(fDebugMode) g_pLogger->LogDebug(_T("object: m_nDragState == lmDRAG_CONTINUE_RIGHT"));
				#endif

				// Continue dragging
				OnObjectContinueDragRight(event, pDC, false, m_vEndDrag, m_vCanvasOffset, m_uMousePagePos, nKeysPressed);
				OnObjectContinueDragRight(event, pDC, true, m_vMouseCanvasPos, m_vCanvasOffset, m_uMousePagePos, nKeysPressed);
				m_vEndDrag = m_vMouseCanvasPos;
			}
            else
			{
				#ifdef __WXDEBUG__
				if(fDebugMode) g_pLogger->LogDebug(_T("object: no identified event"));
				#endif
			}
		}

		else	// dragging but no object: events sent to canvas
		{
			#ifdef __WXDEBUG__
			if(fDebugMode) g_pLogger->LogDebug(_T("dragging but no object: canvas"));
			#endif

			if (event.LeftUp() && m_nDragState == lmDRAG_CONTINUE_LEFT)
			{
				#ifdef __WXDEBUG__
				if(fDebugMode) g_pLogger->LogDebug(_T("canvas: event.LeftUp() && m_nDragState == lmDRAG_CONTINUE_LEFT"));
				#endif

				m_nDragState = lmDRAG_NONE;
				m_fCheckTolerance = true;

				OnCanvasContinueDragLeft(false, m_vEndDrag, m_uMousePagePos, nKeysPressed);
				OnCanvasEndDragLeft(m_vMouseCanvasPos, m_uMousePagePos, nKeysPressed);
				m_pDraggedGMO = (lmGMObject*)NULL;
			}
			else if (event.RightUp() && m_nDragState == lmDRAG_CONTINUE_RIGHT)
			{
				#ifdef __WXDEBUG__
				if(fDebugMode) g_pLogger->LogDebug(_T("canvas: event.RightUp() && m_nDragState == lmDRAG_CONTINUE_RIGHT"));
				#endif

				m_nDragState = lmDRAG_NONE;
				m_fCheckTolerance = true;

				OnCanvasContinueDragRight(false, m_vEndDrag, m_uMousePagePos, nKeysPressed);
				OnCanvasEndDragRight(m_vMouseCanvasPos, m_uMousePagePos, nKeysPressed);
				m_pDraggedGMO = (lmGMObject*)NULL;
			}
			else if (m_nDragState == lmDRAG_START_LEFT)
			{
				#ifdef __WXDEBUG__
				if(fDebugMode) g_pLogger->LogDebug(_T("canvas: m_nDragState == lmDRAG_START_LEFT"));
				#endif

				m_nDragState = lmDRAG_CONTINUE_LEFT;
				OnCanvasBeginDragLeft(m_vStartDrag, m_uMousePagePos, nKeysPressed);
				m_vEndDrag = m_vMouseCanvasPos;
			}
			else if (m_nDragState == lmDRAG_CONTINUE_LEFT)
			{
				#ifdef __WXDEBUG__
				if(fDebugMode) g_pLogger->LogDebug(_T("canvas: m_nDragState == lmDRAG_CONTINUE_LEFT"));
				#endif

				// Continue dragging
				OnCanvasContinueDragLeft(false, m_vEndDrag, m_uMousePagePos, nKeysPressed);
				OnCanvasContinueDragLeft(true, m_vMouseCanvasPos, m_uMousePagePos, nKeysPressed);
				m_vEndDrag = m_vMouseCanvasPos;
			}
			else if (m_nDragState == lmDRAG_START_RIGHT)
			{
				#ifdef __WXDEBUG__
				if(fDebugMode) g_pLogger->LogDebug(_T("canvas: m_nDragState == lmDRAG_START_RIGHT"));
				#endif

				m_nDragState = lmDRAG_CONTINUE_RIGHT;
				OnCanvasBeginDragRight(m_vStartDrag, m_uMousePagePos, nKeysPressed);
				m_vEndDrag = m_vMouseCanvasPos;
			}
			else if (m_nDragState == lmDRAG_CONTINUE_RIGHT)
			{
				#ifdef __WXDEBUG__
				if(fDebugMode) g_pLogger->LogDebug(_T("canvas: m_nDragState == lmDRAG_CONTINUE_RIGHT"));
				#endif

				// Continue dragging
				OnCanvasContinueDragRight(false, m_vEndDrag, m_uMousePagePos, nKeysPressed);
				OnCanvasContinueDragRight(true, m_vMouseCanvasPos, m_uMousePagePos, nKeysPressed);
				m_vEndDrag = m_vMouseCanvasPos;
			}
            else
			{
				#ifdef __WXDEBUG__
				if(fDebugMode) g_pLogger->LogDebug(_T("canvas: no identified event"));
				#endif
			}
		}
	}

}

void lmScoreCanvas::OnMouseEventToolMode(wxMouseEvent& event, wxDC* pDC)
{
    //This method is invoked to deal with mouse events when data entry mode is by using
    //mouse. At this point mouse position and page information has been already computed

    //determine type of area pointed by mouse and classify it
    long nOldMousePointedArea = m_nMousePointedArea;
	lmGMObject* pGMO = m_pView->FindShapeAt(m_nNumPage, m_uMousePagePos, true);
    m_pCurShapeStaff = (lmShapeStaff*)NULL;
    m_pCurBSI = (lmBoxSliceInstr*)NULL;
    if (pGMO)
    {
        //pointing to a selectable shape
        if (pGMO->IsShapeStaff())
        {
            m_nMousePointedArea = lmMOUSE_OnStaff;
            m_pCurShapeStaff = (lmShapeStaff*)pGMO;
            //get the SliceInstr
            pGMO = m_pView->FindBoxAt(m_nNumPage, m_uMousePagePos);
            wxASSERT(pGMO->IsBoxSliceInstr());
            m_pCurBSI = (lmBoxSliceInstr*)pGMO;
        }
        else
            m_nMousePointedArea = lmMOUSE_OnOtherShape;
    }
    else
    {
        //No shape pointed. Check if pointing to a box
        pGMO = m_pView->FindBoxAt(m_nNumPage, m_uMousePagePos);
        if (pGMO)
        {
            if (pGMO->IsBoxSliceInstr())
            {
                m_pCurBSI = (lmBoxSliceInstr*)pGMO;
                //determine staff
                if (m_pLastBSI != m_pCurBSI)
                {
                    //first time on this BoxInstrSlice, between two staves
                    m_pCurShapeStaff = m_pCurBSI->GetNearestStaff(m_uMousePagePos);
                }
                else
                {
                    //continue in this BoxInstrSlice, in same inter-staves area
                    m_pCurShapeStaff = m_pLastShapeStaff;
                }
                //determine position (above/below) relative to staff
                if (m_uMousePagePos.y > m_pCurShapeStaff->GetBounds().GetLeftBottom().y)
                    m_nMousePointedArea = lmMOUSE_OnBelowStaff;
                else
                    m_nMousePointedArea = lmMOUSE_OnAboveStaff;
            }
            else
                m_nMousePointedArea = lmMOUSE_OnOtherBox;
        }
        else
            m_nMousePointedArea = lmMOUSE_OnOther;
    }

    //wxLogMessage(_T("[OnMouseEventToolMode] LastBSI=0x%x, CurBSI=0x%x, LastStaff=0x%x, CurStaff=0x%x, Area=%d"),
    //             m_pLastBSI, m_pCurBSI, m_pLastShapeStaff, m_pCurShapeStaff,
    //             m_nMousePointedArea );

    //determine timepos
    if (m_pCurBSI)
    {
        lmBoxSlice* pBSlice = (lmBoxSlice*)m_pCurBSI->GetParentBox();
        m_rCurTime = pBSlice->GetTimeForPosition(m_uMousePagePos.x);
    }

    //Now we start dealing with mouse moving and mouse clicks. Dragging (moving the mouse
    //with a mouse button clicked) is a meaningless operation and will be treated as 
    //moving. Therefore, only two type of events will be considered: mouse click and
    //mouse move. Let's start with mouse click events.

	if (event.IsButton())
	{
        //mouse click: terminate any possible tool drag operation, process the click, and
        //restart the tool drag operation

        //first, terminate any possible tool drag operation
        if (m_fDraggingTool)
            TerminateToolDrag(pDC);

        //now process the click. To avoid double data entry (first on button down and the on
        //button up) only button up will trigger the processing
        if (event.GetEventType() == wxEVT_LEFT_UP)
            OnToolClick(pGMO, m_uMousePagePos, m_rCurTime);
        //AWARE: after processing the click the graphical model could have been chaged.
        //Therefore all pointers to GMObjects are no longer valid!!!

        //finally, set up information to restart the tool drag operation when moving again
        //the mouse
        m_nMousePointedArea = 0;
        m_pLastShapeStaff = m_pCurShapeStaff;
        m_pLastBSI = m_pCurBSI;

        return;
    }


    //process mouse moving events

    if (event.GetEventType() != wxEVT_MOTION)
        return;



    //check if pointed area has changed from valid to invalid or vice versa

    long nNowOnValidArea = m_nMousePointedArea & m_nValidAreas;
    long nBeforeOnValidArea = nOldMousePointedArea & m_nValidAreas;
    if (nBeforeOnValidArea != 0 && nNowOnValidArea == 0 || nBeforeOnValidArea == 0 && nNowOnValidArea != 0)
    {
        //change from valida area to invalid, or vice versa, change drag status
        if (nNowOnValidArea)
        {
            //entering on staff influence area. Start dragging tool
            StartToolDrag(pDC);
        }
        else
        {
            //exiting staff influence area
            TerminateToolDrag(pDC);
        }
    }
    else
    {
        //no change valid<->invalida area. If we continue in a valid area draw ledger 
        //lines if necessary
        if (nNowOnValidArea)
            ContinueToolDrag(event, pDC);
    }

    //update saved information
    m_pLastShapeStaff = m_pCurShapeStaff;
    m_pLastBSI = m_pCurBSI;

    //determine needed icon and change mouse icon if necessary
    wxCursor* pNeeded = m_pCursorElse;
    if (nNowOnValidArea)
        pNeeded = m_pCursorOnValidArea;

    if (m_pCursorCurrent != pNeeded)
    {
        //change cursor
        m_pCursorCurrent = pNeeded;
        const wxCursor& oCursor = *pNeeded;
        SetCursor( oCursor );
    }
}

void lmScoreCanvas::StartToolDrag(wxDC* pDC)
{
    PrepareToolDragImages();
    wxBitmap* pCursorDragImage = new wxBitmap(*m_pToolBitmap);
    m_vDragHotSpot = lmDPoint(pCursorDragImage->GetWidth() / 2,
                                pCursorDragImage->GetHeight() / 2 );

    //wxLogMessage(_T("[lmScoreCanvas::StartToolDrag] OnImageBeginDrag. m_nMousePointedArea=%d, MousePagePos=(%.2f, %.2f)"),
    //                m_nMousePointedArea, m_uMousePagePos.x, m_uMousePagePos.y);

    m_pView->OnImageBeginDrag(true, pDC, m_vCanvasOffset, m_uMousePagePos,
                              (lmGMObject*)NULL, m_vDragHotSpot, m_uHotSpotShift,
                              pCursorDragImage );
    m_fDraggingTool = true;
}

void lmScoreCanvas::ContinueToolDrag(wxMouseEvent& event, wxDC* pDC)
{
    //wxLogMessage(_T("[lmScoreCanvas::ContinueToolDrag] OnImageContinueDrag. m_nMousePointedArea=%d, MousePagePos=(%.2f, %.2f)"),
    //                m_nMousePointedArea, m_uMousePagePos.x, m_uMousePagePos.y);

    m_pView->OnImageContinueDrag(event, true, pDC, m_vCanvasOffset,
                                 m_uMousePagePos, m_vMouseCanvasPos);
}

void lmScoreCanvas::TerminateToolDrag(wxDC* pDC)
{
    //wxLogMessage(_T("[lmScoreCanvas::TerminateToolDrag] Terminate drag. m_nMousePointedArea=%d, MousePagePos=(%.2f, %.2f)"),
    //                m_nMousePointedArea, m_uMousePagePos.x, m_uMousePagePos.y);

    m_pView->OnImageEndDrag(true, pDC, m_vCanvasOffset, m_uMousePagePos);
    m_fDraggingTool = false;
}

void lmScoreCanvas::TerminateToolDrag()
{
    // Set a DC in logical units and scaled, so that
    // transformations logical/device and viceversa can be computed
    wxClientDC dc(this);
    m_pView->ScaleDC(&dc);
    TerminateToolDrag(&dc);
}

void lmScoreCanvas::StartToolDrag()
{
    // Set a DC in logical units and scaled, so that
    // transformations logical/device and viceversa can be computed
    wxClientDC dc(this);
    m_pView->ScaleDC(&dc);
    StartToolDrag(&dc);
}

//------------------------------------------------------------------------------------------
//call backs from lmScoreView to paint marks for mouse dragged tools.
//
//  - pPaper is already prepared for dirct XOR paint and is scaled and the origin set.
//  - uPos is the mouse current position and they must return the nearest
//    valid notehead position
//------------------------------------------------------------------------------------------

lmUPoint lmScoreCanvas::OnDrawToolMarks(lmPaper* pPaper, const lmUPoint& uPos)
{
    //draw ledger lines
    lmUPoint uFinalPos = uPos;
    if (m_pCurShapeStaff)
        uFinalPos = m_pCurShapeStaff->OnMouseStartMoving(pPaper, uPos);

    //update time in status bar
    GetMainFrame()->SetStatusBarCursorRelPos(m_rCurTime, 0);

    //draw time grid
    if (m_pCurBSI)
        m_pCurBSI->DrawTimeGrid(pPaper);

    return uFinalPos;
}

lmUPoint lmScoreCanvas::OnRedrawToolMarks(lmPaper* pPaper, const lmUPoint& uPos)
{
    //remove and redraw ledger lines
    lmUPoint uFinalPos = uPos;
    if (m_pLastShapeStaff && m_pLastShapeStaff != m_pCurShapeStaff)
    {
        m_pLastShapeStaff->OnMouseEndMoving(pPaper, uPos);
        if (m_pCurShapeStaff)
            uFinalPos = m_pCurShapeStaff->OnMouseStartMoving(pPaper, uPos);
    }
    else if (m_pCurShapeStaff)
        uFinalPos = m_pCurShapeStaff->OnMouseMoving(pPaper, uPos);

    //update time
    GetMainFrame()->SetStatusBarCursorRelPos(m_rCurTime, 0);

    //remove previous grid
    if (m_pLastBSI && m_pLastBSI != m_pCurBSI)
        m_pLastBSI->DrawTimeGrid(pPaper);

    //draw new grid 
    if (!m_pLastBSI || m_pLastBSI != m_pCurBSI)
        m_pCurBSI->DrawTimeGrid(pPaper);

    return uFinalPos;
}

lmUPoint lmScoreCanvas::OnRemoveToolMarks(lmPaper* pPaper, const lmUPoint& uPos)
{
    //remove ledger lines
    lmUPoint uFinalPos = uPos;
    if (m_pLastShapeStaff && m_pLastShapeStaff != m_pCurShapeStaff)
        m_pLastShapeStaff->OnMouseEndMoving(pPaper, uPos);
    else if (m_pCurShapeStaff)
        m_pCurShapeStaff->OnMouseEndMoving(pPaper, uPos);

    //update time
    GetMainFrame()->SetStatusBarCursorRelPos(m_rCurTime, 0);

    //remove previous time grid
    if (m_pLastBSI)
        m_pLastBSI->DrawTimeGrid(pPaper);

    return uFinalPos;
}



//------------------------------------------------------------------------------------------

void lmScoreCanvas::ChangeMouseIcon()
{
    //change mouse icon if necessary. Type of area currently pointed by mouse is in global variable
    //m_nMousePointedArea.

    //determine needed icon
    wxCursor* pNeeded = m_pCursorElse;
    if (m_nMousePointedArea & m_nValidAreas)
        pNeeded = m_pCursorOnValidArea;

    //get current cursor
    if (m_pCursorCurrent != pNeeded)
    {
        m_pCursorCurrent = pNeeded;
        const wxCursor& oCursor = *pNeeded;
        SetCursor( oCursor );
    }
}

void lmScoreCanvas::PlayScore(bool fFromCursor)
{
    //get the score
    lmScore* pScore = m_pDoc->GetScore();

	//determine measure from cursor or start of selection
	int nMeasure = 1;
	lmGMSelection* pSel = m_pView->GetSelection();
	bool fFromMeasure = fFromCursor || pSel->NumObjects() > 0;
	if (pSel->NumObjects() > 0)
	{
		nMeasure = ((lmNote*)pSel->GetFirst()->GetScoreOwner())->GetSegment()->GetNumSegment() + 1;
		m_pView->DeselectAllGMObjects(true);	//redraw, to remove selection highlight
	}
	else
		nMeasure = m_pView->GetCursorMeasure();

	//play back the score
	if (fFromMeasure)
		pScore->PlayFromMeasure(nMeasure, lmVISUAL_TRACKING, ePM_NormalInstrument, 0, this);
	else
		pScore->Play(lmVISUAL_TRACKING, lmNO_COUNTOFF, ePM_NormalInstrument, 0, this);
}

void lmScoreCanvas::StopPlaying(bool fWait)
{
    //get the score
    lmScore* pScore = m_pDoc->GetScore();
    if (!pScore) return;

    //request it to stop playing
    pScore->Stop();
    if (fWait)
        pScore->WaitForTermination();
}

void lmScoreCanvas::PausePlaying()
{
    //get the score
    lmScore* pScore = m_pDoc->GetScore();

    //request it to pause playing
    pScore->Pause();
}

void lmScoreCanvas::OnVisualHighlight(lmScoreHighlightEvent& event)
{
    m_pView->OnVisualHighlight(event);
}

//--------------------------------------------------------------------------------------------
// Commands
//--------------------------------------------------------------------------------------------


void lmScoreCanvas::AttachNewText(lmComponentObj* pTarget)
{
    //Create a new text and attach it to the received object

    //create the new text.
    //Text creation requires to create an empty TextItem and editing it using the properties
    //dialog. And this, in turn, requires the TextItem to edit to be already included in the
    //score. Therefore, I will attach it provisionally to the score

    lmScore* pScore = m_pDoc->GetScore();
    lmTextStyle* pStyle = pScore->GetStyleInfo(_("Normal text"));
    wxASSERT(pStyle);
    wxString sText = _T("");
    lmTextItem* pNewText = new lmTextItem(pScore, lmNEW_ID, sText, lmHALIGN_DEFAULT, pStyle);
	pScore->AttachAuxObj(pNewText);

    //show dialog to edit the text
	lmDlgProperties dlg((lmController*)NULL);
	pNewText->OnEditProperties(&dlg);
	dlg.Layout();
	dlg.ShowModal();

    //get text info
    sText = pNewText->GetText();
    pStyle = pNewText->GetStyle();
    lmEHAlign nAlign = pNewText->GetAlignment();

	//dettach the text from the score and delete the text item
	pScore->DetachAuxObj(pNewText);
    delete pNewText;

    //Now issue the command to attach the text to the received target object
	wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
    if (sText != _T(""))
	    pCP->Submit(new lmCmdAttachText(lmCMD_NORMAL, m_pDoc, sText, pStyle,
                                        nAlign, pTarget));
}

void lmScoreCanvas::AddTitle()
{
    //Create a new block of text and attach it to the score

    //create the new text.
    //Text creation requires to create an empty TextItem and editing it using the properties
    //dialog. And this, in turn, requires the TextItem to edit to be already included in the
    //score. Therefore, I will attach it provisionally to the score

    lmScore* pScore = m_pDoc->GetScore();
    lmTextStyle* pStyle = pScore->GetStyleInfo(_("Title"));
    wxASSERT(pStyle);
    wxString sTitle = _T("");
    lmScoreTitle* pNewTitle 
        = new lmScoreTitle(pScore, lmNEW_ID, sTitle, lmBLOCK_ALIGN_BOTH,
                           lmHALIGN_DEFAULT, lmVALIGN_DEFAULT, pStyle);
	pScore->AttachAuxObj(pNewTitle);

    //show dialog to create the text
	lmDlgProperties dlg((lmController*)NULL);
	pNewTitle->OnEditProperties(&dlg);
	dlg.Layout();
	if (dlg.ShowModal() == wxID_OK)
        pScore->OnPropertiesChanged();

    //get title info
    sTitle = pNewTitle->GetText();
    pStyle = pNewTitle->GetStyle();
    lmEHAlign nAlign = pNewTitle->GetAlignment();

	//dettach the text from the score and delete the text item
	pScore->DetachAuxObj(pNewTitle);
    delete pNewTitle;

    //Now issue the command to attach the title to to the score
	wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
    if (sTitle != _T(""))
	    pCP->Submit(new lmCmdAddTitle(lmCMD_NORMAL, m_pDoc, sTitle, pStyle,
                                      nAlign));
}

void lmScoreCanvas::MoveObject(lmGMObject* pGMO, const lmUPoint& uPos)
{
	//Generate move command to move the lmComponentObj and update the document

	wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = wxString::Format(_("Move %s"), pGMO->GetName().c_str() );
	pCP->Submit(new lmCmdMoveObject(lmCMD_NORMAL, sName, m_pDoc, pGMO, uPos));
}

void lmScoreCanvas::MoveObjectPoints(lmGMObject* pGMO, lmUPoint uShifts[],
                                     int nNumPoints, bool fUpdateViews)
{
	//Generate move command to move the lmComponentObj and update the document

	wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
    wxString sName = wxString::Format(_("%s: move points"), pGMO->GetName().c_str() );
	pCP->Submit(new lmCmdMoveObjectPoints(lmCMD_NORMAL, sName, m_pDoc, pGMO, uShifts, nNumPoints,
                                          fUpdateViews) );
}

void lmScoreCanvas::MoveNote(lmGMObject* pGMO, const lmUPoint& uPos, int nSteps)
{
	//Generate move command to move the note and change its pitch

	wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	pCP->Submit(new lmCmdMoveNote(lmCMD_NORMAL, m_pDoc, (lmNote*)pGMO->GetScoreOwner(), uPos, nSteps));
}

void lmScoreCanvas::ChangePageMargin(lmGMObject* pGMO, int nIdx, int nPage, lmLUnits uPos)
{
	//Updates the position of a margin

	wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	pCP->Submit(new lmCmdChangePageMargin(lmCMD_NORMAL, _("Change margin"), m_pDoc,
                                          pGMO, nIdx, nPage, uPos));
}

void lmScoreCanvas::DeleteStaffObj()
{
	//delete the StaffObj at current caret position

	//get object pointed by the cursor
    lmCursorState oState = m_pView->GetScoreCursor()->GetState();
    lmStaffObj* pCursorSO = oState.GetStaffObj(m_pDoc->GetScore());

    //if no object, ignore command. It is due, for example, to the user clicking 'Del' key
    //on no object
	if (!pCursorSO)
        return;

    //prepare command and submit it
    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = wxString::Format(_("Delete %s"), pCursorSO->GetName().c_str() );
	pCP->Submit(new lmCmdDeleteStaffObj(lmCMD_NORMAL, sName, m_pDoc, pCursorSO));
}

void lmScoreCanvas::DeleteCaretOrSelected()
{
    //If there is a selection, delete all objects in the selection.
    //Else delete staffobj pointed by caret

    if (m_pView->SomethingSelected())
        DeleteSelection();
    else
        DeleteStaffObj();
}

void lmScoreCanvas::DeleteSelection()
{
    //Deleted all objects in the selection.

    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = _("Delete selection");
	pCP->Submit(new lmCmdDeleteSelection(lmCMD_NORMAL, sName, m_pDoc, m_pView->GetSelection()) );
}

void lmScoreCanvas::BreakBeam()
{
    //Break beamed group at selected note (the one pointed by cursor)

    //get cursor state
    lmCursorState oState = m_pView->GetScoreCursor()->GetState();

	//get object pointed by the cursor
    lmStaffObj* pCursorSO = oState.GetStaffObj(m_pDoc->GetScore());
	wxASSERT(pCursorSO && pCursorSO->IsNoteRest());

    //prepare command and submit it
    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = _("Break a beam");
	pCP->Submit(new lmCmdBreakBeam(lmCMD_NORMAL, sName, m_pDoc, (lmNoteRest*)pCursorSO));
}

void lmScoreCanvas::JoinBeam()
{
    //depending on current selection content, either:
    // - create a beamed group with the selected notes,
    // - join two or more beamed groups
    // - or add a note to a beamed group

    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = _("Add beam");
	pCP->Submit(new lmCmdJoinBeam(lmCMD_NORMAL, sName, m_pDoc, m_pView->GetSelection()) );
}

void lmScoreCanvas::ChangeTie(lmNote* pStartNote, lmNote* pEndNote)
{
    //Add/remove tie from the two selected notes (there could be other objects selected beetween
    //the notes)

    if (pEndNote->IsTiedToPrev())
        DeleteTie(pEndNote);
    else
        AddTie(pStartNote, pEndNote);
}

void lmScoreCanvas::DeleteTie(lmNote* pEndNote)
{
    //remove tie between two notes

    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = _("Delete tie");
	pCP->Submit(new lmCmdDeleteTie(lmCMD_NORMAL, sName, m_pDoc, pEndNote) );
}

void lmScoreCanvas::AddTie(lmNote* pStartNote, lmNote* pEndNote)
{
    //Tie received note with previous one.
    //Preconditions: pEndNote is not tied and has been checked that can be tied to previous one

    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = _("Add tie");
	pCP->Submit(new lmCmdAddTie(lmCMD_NORMAL, sName, m_pDoc, pStartNote, pEndNote) );
}

void lmScoreCanvas::AddTuplet()
{
    // Add a tuplet to the selected notes/rests (there could be other objects selected
    // beetween the notes)
    //
    // Precondition:
    //      it has been checked that all notes/rest in the seleccion are not in a tuplet,
    //      are consecutive and are in the same voice.

    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = _("Add tuplet");
	pCP->Submit(new lmCmdAddTuplet(lmCMD_NORMAL, sName, m_pDoc, m_pView->GetSelection(),
                                   true, 3, true, ep_Default, 3, 2) );
                               //bool fShowNumber, int nNumber, bool fBracket,
                               //lmEPlacement nAbove, int nActual, int nNormal)
}

void lmScoreCanvas::DeleteTuplet(lmNoteRest* pStartNR)
{
    // Remove tuplet from tuplet group. The first note/rest of the tuplet group
    // is pStartNR
    //
    // Precondition:
    //      It has been checked that pStatNR is in a tuplet and that it is the 
    //      first note/rest.

    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = _("Delete tuplet");
	pCP->Submit(new lmCmdDeleteTuplet(lmCMD_NORMAL, sName, m_pDoc, pStartNR) );
}

void lmScoreCanvas::InsertClef(lmEClefType nClefType)
{
	//insert a Clef at current cursor position

    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = _("Insert clef");
	pCP->Submit(new lmCmdInsertClef(lmCMD_NORMAL, sName, m_pDoc, nClefType) );
}

void lmScoreCanvas::InsertTimeSignature(int nBeats, int nBeatType, bool fVisible)
{
    //insert a time signature at current cursor position

    //wxLogMessage(_T("[lmScoreCanvas::InsertTimeSignature] nBeats=%d, nBeatType=%d"), nBeats, nBeatType);

    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = _("Insert time signature");
	pCP->Submit(new lmCmdInsertTimeSignature(lmCMD_NORMAL, sName, m_pDoc,
                                             nBeats, nBeatType, fVisible) );
}

void lmScoreCanvas::InsertKeySignature(int nFifths, bool fMajor, bool fVisible)
{
    //insert a key signature at current cursor position

    //wxLogMessage(_T("[lmScoreCanvas::InsertKeySignature] fifths=%d, %s"),
    //             nFifths, (fMajor ? _T("major") : _T("minor")) );

    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = _("Insert key signature");
	pCP->Submit(new lmCmdInsertKeySignature(lmCMD_NORMAL, sName, m_pDoc, nFifths,
                                            fMajor, fVisible) );
}

void lmScoreCanvas::InsertBarline(lmEBarline nType)
{
	//insert a barline at current cursor position

    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = _("Insert barline");
	pCP->Submit(new lmCmdInsertBarline(lmCMD_NORMAL, sName, m_pDoc, nType) );
}

void lmScoreCanvas::InsertNote(lmEPitchType nPitchType, int nStep, int nOctave,
							   lmENoteType nNoteType, float rDuration, int nDots,
							   lmENoteHeads nNotehead, lmEAccidentals nAcc,
                               int nVoice, lmNote* pBaseOfChord, bool fTiedPrev)
{
	//insert a note at current cursor position

	//if new note in chord check that there is a base note at current cursor position

    //prepare command and submit it
    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = _("Insert note");
    wxString sOctave = wxString::Format(_T("%d"), nOctave);
    wxString sAllSteps = _T("cdefgab");
    wxString sStep = sAllSteps.GetChar( nStep );

	pCP->Submit(new lmCmdInsertNote(lmCMD_NORMAL, sName, m_pDoc, nPitchType, nStep, nOctave,
							        nNoteType, rDuration, nDots, nNotehead, nAcc,
                                    nVoice, pBaseOfChord, fTiedPrev) );

}

void lmScoreCanvas::InsertRest(lmENoteType nNoteType, float rDuration, int nDots, int nVoice)
{
	//insert a rest at current cursor position

    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = _("Insert rest");
	pCP->Submit(new lmCmdInsertRest(lmCMD_NORMAL, sName, m_pDoc, nNoteType,
                                    rDuration, nDots, nVoice) );
}

void lmScoreCanvas::ChangeNotePitch(int nSteps)
{
	//change pith of note at current cursor position
    lmScoreCursor* pCursor = m_pDoc->GetScore()->GetCursor();
	wxASSERT(pCursor);
    lmStaffObj* pCursorSO = pCursor->GetStaffObj();
	wxASSERT(pCursorSO);
	wxASSERT(pCursorSO->GetClass() == eSFOT_NoteRest && ((lmNoteRest*)pCursorSO)->IsNote() );
    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = _("Change note pitch");
	pCP->Submit(new lmCmdChangeNotePitch(lmCMD_NORMAL, sName, m_pDoc,
                                         (lmNote*)pCursorSO, nSteps) );
}

void lmScoreCanvas::ChangeNoteAccidentals(int nAcc)
{
	//change note accidentals for current selected notes

    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = _("Change note accidentals");
	pCP->Submit(new lmCmdChangeNoteAccidentals(lmCMD_NORMAL, sName, m_pDoc,
                            m_pView->GetSelection(), nAcc) );
}

void lmScoreCanvas::ChangeNoteDots(int nDots)
{
	//change note dots for current selected notes

    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = _("Change note dots");
	pCP->Submit(new lmCmdChangeNoteRestDots(lmCMD_NORMAL, sName, m_pDoc,
                                            m_pView->GetSelection(), nDots) );
}

void lmScoreCanvas::ChangeText(lmScoreText* pST, wxString sText, lmEHAlign nAlign,
                               lmLocation tPos, lmTextStyle* pStyle, int nHintOptions)
{
	//change properties of a lmTextItem object

    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = _("Change text");
	pCP->Submit(new lmCmdChangeText(lmCMD_NORMAL, sName, m_pDoc, pST, sText,
                                    nAlign, tPos, pStyle, nHintOptions) );
}

void lmScoreCanvas::ChangeBarline(lmBarline* pBL, lmEBarline nType, bool fVisible)
{
    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	pCP->Submit(new lmCmdChangeBarline(lmCMD_NORMAL, m_pDoc, pBL, nType, fVisible) );
}

void lmScoreCanvas::ChangeMidiSettings(lmInstrument* pInstr, int nMidiChannel,
                                       int nMidiInstr)
{
    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	pCP->Submit(new lmCmdChangeMidiSettings(lmCMD_NORMAL, m_pDoc, pInstr, nMidiChannel,
                                            nMidiInstr) );
}

wxCursor* lmScoreCanvas::LoadMouseCursor(wxString sFile, int nHotSpotX, int nHotSpotY)
{
    //loads all mouse cursors used in lmScoreCanvas

    wxString sPath = g_pPaths->GetCursorsPath();
    wxFileName oFilename(sPath, sFile, wxPATH_NATIVE);
    wxCursor* pCursor;

    //load image
    wxImage oImage(oFilename.GetFullPath(), wxBITMAP_TYPE_PNG);
    if (!oImage.IsOk())
    {
        wxLogMessage(_T("[lmScoreCanvas::LoadMouseCursor] Failure loading mouse cursor image '%s'"),
                     oFilename.GetFullPath().c_str());
        return NULL;
    }

    //set hot spot point
    oImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, nHotSpotX);
    oImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, nHotSpotY);

    //create the cursor
    pCursor = new wxCursor(oImage);
    if (!pCursor->IsOk())
    {
        wxLogMessage(_T("[lmScoreCanvas::LoadMouseCursor] Failure creating mouse cursor from image '%s'"),
                     oFilename.GetFullPath().c_str());
        delete pCursor;
        return NULL;
    }

    return pCursor;
}

void lmScoreCanvas::LoadAllMouseCursors()
{
    //loads all mouse cursors used in lmScoreCanvas

    //AWARE: Cursors MUST BE LOADED in the same order than enum lmEMouseCursor

    //lm_eCursor_Pointer
    wxCursor* pCursor = new wxCursor(wxCURSOR_ARROW);
    m_MouseCursors.push_back(pCursor);

    // lm_eCursor_Note
    m_MouseCursors.push_back( LoadMouseCursor(_T("cursor-note.png"), 8, 8) );

    // lm_eCursor_Note_Forbidden,
    pCursor = new wxCursor(wxCURSOR_NO_ENTRY);
    m_MouseCursors.push_back(pCursor);
    //m_MouseCursors.push_back( LoadMouseCursor(_T("cursor-note-forbidden.png"), 8, 8) );


    //set default cursors
    m_pCursorOnSelectedObject = GetMouseCursor(lm_eCursor_Pointer);
    m_pCursorOnValidArea = GetMouseCursor(lm_eCursor_Pointer);
    m_pCursorElse = GetMouseCursor(lm_eCursor_Pointer);

}

wxCursor* lmScoreCanvas::GetMouseCursor(lmEMouseCursor nCursorID)
{
    return m_MouseCursors[nCursorID];
}

void lmScoreCanvas::OnToolBoxPageChanged(lmToolBoxPageChangedEvent& event)
{
    //set data entry mode if in Notes page

    if (event.GetToolPageType() == lmPAGE_NOTES)
        GetDataEntryMode();
}

void lmScoreCanvas::GetDataEntryMode()
{
    //Get selected data entry mode and update the internal information

	lmToolBox* pToolBox = GetMainFrame()->GetActiveToolBox();
	wxASSERT(pToolBox);
	m_nEntryMode = pToolBox->GetEntryMode();
    if (m_nEntryMode == lm_DATA_ENTRY_MOUSE)
    {
        m_nValidAreas = lmMOUSE_OnStaff | lmMOUSE_OnAboveStaff | lmMOUSE_OnBelowStaff;
        m_pCursorOnSelectedObject = GetMouseCursor(lm_eCursor_Pointer);  //lm_eCursor_Note);
        m_pCursorOnValidArea = GetMouseCursor(lm_eCursor_Pointer);  //GetMouseCursor(lm_eCursor_Note);
        m_pCursorElse = GetMouseCursor(lm_eCursor_Note_Forbidden);
        //hide caret
        m_pView->CaretOff();
    }
    else
    {
        m_nValidAreas = lmMOUSE_OnAny;
        m_pCursorOnSelectedObject = GetMouseCursor(lm_eCursor_Pointer);
        m_pCursorOnValidArea = GetMouseCursor(lm_eCursor_Pointer);
        m_pCursorElse = GetMouseCursor(lm_eCursor_Pointer);
        //show caret
        m_pView->CaretOn();
    }

    //set cursor
    ChangeMouseIcon();
}

void lmScoreCanvas::OnToolBoxEvent(lmToolBoxToolSelectedEvent& event)
{
	lmToolBox* pToolBox = GetMainFrame()->GetActiveToolBox();
	wxASSERT(pToolBox);

    lmEToolPage nPage = event.GetToolPageType();
    lmEToolGroupID nGroup = event.GetToolGroupID();

    //verify if Data entry mode changed
    if (nGroup == lmGRP_EntryMode)
    {
        GetDataEntryMode();
        return;
    }

    //get values for current page
    GetToolBoxValuesForPage(nPage);

    //specific actions on selected objects
    switch (event.GetToolGroupID())
    {
        case lmGRP_NoteAcc:
            //selection of accidentals ----------------------------------------------------
            if (m_pView->SomethingSelected())
            {
			    int nAcc;
                switch(m_nSelAcc)
                {
                    case lm_eNatural:       nAcc = 0;   break;
                    case lm_eFlat:          nAcc = -1;  break;
                    case lm_eSharp:         nAcc = 1;   break;
                    case lm_eFlatFlat:      nAcc = -2;  break;
                    case lm_eDoubleSharp:   nAcc = 2;   break;
                    case lm_eSharpSharp:    nAcc = 2;   break;
                    case lm_eNaturalFlat:   nAcc = -1;  break;
                    case lm_eNaturalSharp:  nAcc = 1;   break;
                    default:
                        nAcc = 0;
                }
                ChangeNoteAccidentals(nAcc);
            }
            break;

        case lmGRP_NoteDots:
            //selection of dots -----------------------------------------------------------
            if (m_pView->SomethingSelected())
            {
			    ChangeNoteDots(m_nSelDots);
            }
            break;

        case lmGRP_TieTuplet:
            //Tie, Tuplet tools -----------------------------------------------------------
            if (m_pView->SomethingSelected())
            {
                switch(event.GetToolID())
                {
                    case lmTOOL_NOTE_TIE:
                        {
                            lmNote* pStartNote;
                            lmNote* pEndNote;
                            if (IsSelectionValidForTies(&pStartNote, &pEndNote))
                                ChangeTie(pStartNote, pEndNote);
                        }
                        break;

                    case lmTOOL_NOTE_TUPLET:
                        {
                            lmNoteRest* pStartNR = IsSelectionValidForTuplet();
                            if (pStartNR)
                            {
                                if (pStartNR->IsInTuplet())
                                    DeleteTuplet(pStartNR);
                                else
                                    AddTuplet();
                            }
                        }
                        break;

                    default:
                        wxASSERT(false);
                }
            }
            break;

        case lmGRP_Beams:
            //Beam tools ------------------------------------------------------------------
            switch(event.GetToolID())
            {
                case lmTOOL_BEAMS_CUT:
                    BreakBeam();
                    break;

                case lmTOOL_BEAMS_JOIN:
                    JoinBeam();
                    break;

                case lmTOOL_BEAMS_FLATTEN:
                case lmTOOL_BEAMS_SUBGROUP:
                    {
                        //TODO
                    }
                    break;

                default:
                    wxASSERT(false);
            }
            break;


        default:
            ;   //ignore the event
    }
}

void lmScoreCanvas::GetToolBoxValuesForPage(lmEToolPage nPage)
{
    //access ToolBox and get user selected values for current page.

	lmToolBox* pToolBox = GetMainFrame()->GetActiveToolBox();
	if (!pToolBox)
        return;

	switch(nPage)
	{
        case lmPAGE_NOTES:
        {
			lmToolPageNotes* pNoteOptions = pToolBox->GetNoteProperties();
			m_nSelNoteType = pNoteOptions->GetNoteDuration();
			m_nSelDots = pNoteOptions->GetNoteDots();
			m_nSelNotehead = pNoteOptions->GetNoteheadType();
			m_nSelAcc = pNoteOptions->GetNoteAccidentals();
			m_nOctave = pNoteOptions->GetOctave();
			m_nSelVoice = pNoteOptions->GetVoice();
            break;
        }

        default:
            ;
    }
}

void lmScoreCanvas::PrepareToolDragImages()
{
    //prepare drag images for current selected tool, for all staff sizes

    //delete previous bitmaps
    if (m_pToolBitmap)
        delete m_pToolBitmap;

    //create new ones
    wxColour colorF = *wxBLUE;
    wxColour colorB = *wxWHITE;
    lmStaff* pStaff = m_pView->GetDocument()->GetScore()->GetFirstInstrument()->GetVStaff()->GetFirstStaff();
    double rPointSize = pStaff->GetMusicFontSize();
    double rScale = m_pView->GetScale() * lmSCALE;

    m_pToolBitmap =
        GetBitmapForGlyph(rScale, GLYPH_NOTEHEAD_QUARTER, rPointSize, colorF, colorB);
}

void lmScoreCanvas::OnKeyDown(wxKeyEvent& event)
{
    //wxLogMessage(_T("EVT_KEY_DOWN"));
    switch ( event.GetKeyCode() )
    {
        case WXK_SHIFT:
        case WXK_ALT:
        case WXK_CONTROL:
            break;      //do nothing

        default:
            //save key down info
            m_nKeyDownCode = event.GetKeyCode();
            m_fShift = event.ShiftDown();
            m_fAlt = event.AltDown();
            m_fCmd = event.CmdDown();

            //If a key down (EVT_KEY_DOWN) event is caught and the event handler does not
            //call event.Skip() then the corresponding char event (EVT_CHAR) will not happen.
            //This is by design of wxWidgets and enables the programs that handle both types of
            //events to be a bit simpler.

            //event.Skip();       //to generate Key char event
            ProcessKey(event);
    }
}

void lmScoreCanvas::OnKeyPress(wxKeyEvent& event)
{
    //wxLogMessage(_T("[lmScoreCanvas::OnKeyPress] KeyCode=%s (%d), KeyDown data: Keycode=%s (%d), (flags = %c%c%c%c)"),
    //        KeyCodeToName(event.GetKeyCode()).c_str(), event.GetKeyCode(),
    //        KeyCodeToName(m_nKeyDownCode).c_str(), m_nKeyDownCode,
    //        (m_fCmd ? _T('C') : _T('-') ),
    //        (m_fAlt ? _T('A') : _T('-') ),
    //        (m_fShift ? _T('S') : _T('-') ),
    //        (event.MetaDown() ? _T('M') : _T('-') )
    //        );
    //ProcessKey(event);
}

void lmScoreCanvas::ProcessKey(wxKeyEvent& event)
{
    //We are processing a Key Down event
	lmToolBox* pToolBox = GetMainFrame()->GetActiveToolBox();
	if (!pToolBox)
        return;

	lmEToolPage nTool = pToolBox->GetSelectedToolPage();
    int nKeyCode = event.GetKeyCode();
	bool fUnknown = false;

	//Verify common keys working with all tools
	fUnknown = false;
	switch (nKeyCode)
	{
		case WXK_LEFT:
			m_pView->CaretLeft(event.AltDown());
			break;

		case WXK_RIGHT:
			m_pView->CaretRight(event.AltDown());
			break;

		case WXK_UP:
			m_pView->CaretUp();
			break;

		case WXK_DOWN:
			m_pView->CaretDown();
			break;

		case WXK_F2:
			if (pToolBox) pToolBox->SelectToolPage((lmEToolPage)0);
			break;

		case WXK_F3:
			if (pToolBox) pToolBox->SelectToolPage((lmEToolPage)1);
			break;

		case WXK_F4:
			if (pToolBox) pToolBox->SelectToolPage((lmEToolPage)2);
			break;

		case WXK_F5:
			if (pToolBox) pToolBox->SelectToolPage((lmEToolPage)3);
			break;

		case WXK_F6:
			if (pToolBox) pToolBox->SelectToolPage((lmEToolPage)4);
			break;

		case WXK_F7:
			if (pToolBox) pToolBox->SelectToolPage((lmEToolPage)5);
			break;

        case WXK_DELETE:
            //delete selected objects or object pointed by caret
			DeleteCaretOrSelected();
			break;

        case WXK_BACK:
			m_pView->CaretLeft(false);      //false: treat chords as a single object
			DeleteCaretOrSelected();
			break;

		default:
			fUnknown = true;
	}

    //fix ctrol+key codes
    if (nKeyCode > 0 && nKeyCode < 27)
        nKeyCode += int('A') - 1;

	//if not processed, check if specific for current selected tool panel
	if (fUnknown)
	{
	    switch(nTool)
	    {
            case lmPAGE_NONE:	//---------------------------------------------------------
		    {
                fUnknown = false;       //assume it will be processed
			    switch (nKeyCode)
			    {
				    case WXK_UP:
					    m_pView->CaretUp();
					    break;

				    case WXK_DOWN:
					    m_pView->CaretDown();
					    break;

				    default:
                        if (wxIsprint(nKeyCode))
                            m_sCmd += wxString::Format(_T("%c"), (char)nKeyCode);
					    fUnknown = true;
			    }
			    break;
		    }

            case lmPAGE_NOTES:	//---------------------------------------------------------
		    {
			    lmToolPageNotes* pNoteOptions = pToolBox->GetNoteProperties();
			    m_nSelNoteType = pNoteOptions->GetNoteDuration();
			    m_nSelDots = pNoteOptions->GetNoteDots();
			    m_nSelNotehead = pNoteOptions->GetNoteheadType();
			    m_nSelAcc = pNoteOptions->GetNoteAccidentals();
				m_nOctave = pNoteOptions->GetOctave();
				m_nSelVoice = pNoteOptions->GetVoice();

                bool fTiedPrev = false;

                //if terminal symbol, analyze full command
                if ((nKeyCode >= int('A') && nKeyCode <= int('G')) ||
                    (nKeyCode >= int('a') && nKeyCode <= int('g')) ||
                    nKeyCode == int(' ') )
                {
                    if (m_sCmd != _T(""))
                    {
                        lmKbdCmdParser oCmdParser;
                        if (oCmdParser.ParserCommand(m_sCmd))
                        {
                            m_nSelAcc = oCmdParser.GetAccidentals();
                            m_nSelDots = oCmdParser.GetDots();
                            fTiedPrev = oCmdParser.GetTiedPrev();
                        }
                    }
                }

                //compute note/rest duration
			    float rDuration = lmLDPParser::GetDefaultDuration(m_nSelNoteType, m_nSelDots, 0, 0);

                //insert note
                if ((nKeyCode >= int('A') && nKeyCode <= int('G')) ||
                    (nKeyCode >= int('a') && nKeyCode <= int('g')) )
                {
                    //convert key to upper case
                    if (nKeyCode > int('G'))
                        nKeyCode -= 32;

                    // determine octave
                    if (event.ShiftDown())
                        ++m_nOctave;
                    else if (event.CmdDown())
                        --m_nOctave;

                    //limit octave 0..9
                    if (m_nOctave < 0)
                        m_nOctave = 0;
                    else if (m_nOctave > 9)
                        m_nOctave = 9;

                    //get step
                    static wxString sSteps = _T("abcdefg");
                    int nStep = LetterToStep( sSteps.GetChar( nKeyCode - int('A') ));

					//check if the note is added to form a chord and determine base note
					lmNote* pBaseOfChord = (lmNote*)NULL;
					if (event.AltDown())
					{
						lmStaffObj* pSO = m_pDoc->GetScore()->GetCursor()->GetStaffObj();
						if (pSO && pSO->IsNoteRest() && ((lmNoteRest*)pSO)->IsNote())
							pBaseOfChord = (lmNote*)pSO;
					}

                    //do insert note
					InsertNote(lm_ePitchRelative, nStep, m_nOctave, m_nSelNoteType, rDuration,
							   m_nSelDots, m_nSelNotehead, m_nSelAcc, m_nSelVoice, pBaseOfChord, fTiedPrev);

                    fUnknown = false;
                }

                //insert rest
                if (nKeyCode == int(' '))
                {
                    //do insert rest
                    InsertRest(m_nSelNoteType, rDuration, m_nSelDots, m_nSelVoice);

                    fUnknown = false;
                }

                //commands to change options in Tool Box


                //Select note duration:     digits 0..9
                //Select octave:            ctrl + digits 0..9
                //Select voice:             alt + digits 0..9
                if (fUnknown && nKeyCode >= int('0') && nKeyCode <= int('9'))
			    {
                    if (event.CmdDown())
                        //octave: ctrl + digits 0..9
					    SelectOctave(nKeyCode - int('0'));

                    else if (event.AltDown())
                        //Voice: alt + digits 0..9
 					    SelectVoice(nKeyCode - int('0'));

                    else
                        //Note duration: digits 0..9
					    SelectNoteDuration(nKeyCode - int('0'));

                    fUnknown = false;
                }

                //increment/decrement octave: up (ctrl +), down (ctrl -)
                else if (fUnknown && event.CmdDown()
                         && (nKeyCode == int('+') || nKeyCode == int('-')) )
			    {
					SelectOctave(nKeyCode == int('+'));
                    fUnknown = false;
                }

                //increment/decrement voice: up (alt +), down (alt -)
                else if (fUnknown && event.AltDown()
                         && (nKeyCode == int('+') || nKeyCode == int('-')) )
			    {
					SelectVoice(nKeyCode == int('+'));
                    fUnknown = false;
                }


#if 0   //old code, to select accidentals and dots
           //     if (fUnknown)
           //     {
           //         fUnknown = false;       //assume it
			        //switch (nKeyCode)
			        //{
           //             //select accidentals
				       // case int('+'):      // '+' increment accidentals
           //                 SelectNoteAccidentals(true);
           //                 break;

           //             case int('-'):      // '-' decrement accidentals
           //                 SelectNoteAccidentals(false);
           //                 break;

           //             //select dots
				       // case int('.'):      // '.' increment/decrement dots
           //                 if (event.AltDown())
           //                     SelectNoteDots(false);      // Alt + '.' decrement dots
           //                 else
           //                     SelectNoteDots(true);       // '.' increment dots
           //                 break;

           //             //unknown
				       // default:
					      //  fUnknown = true;
           //         }
           //     }
#endif

				    //commands requiring to have a note/rest selected

				    ////change selected note pitch
				    //case WXK_UP:
				    //	if (nAuxKeys==0)
				    //		ChangeNotePitch(1);		//step up
				    //	else if (nAuxKeys && lmKEY_SHIFT)
				    //		ChangeNotePitch(7);		//octave up
				    //	else
				    //		fUnknown = true;
				    //	break;

				    //case WXK_DOWN:
				    //	if (nAuxKeys==0)
				    //		ChangeNotePitch(-1);		//step down
				    //	else if (nAuxKeys && lmKEY_SHIFT)
				    //		ChangeNotePitch(-7);		//octave down
				    //	else
				    //		fUnknown = true;
				    //	break;


				   // //invalid key
				   // default:
					  //  fUnknown = true;
			    //}

                //save char if unused
                if (fUnknown && wxIsprint(nKeyCode))
                    m_sCmd += wxString::Format(_T("%c"), (char)nKeyCode);

			    break;      //case lmPAGE_NOTES
		    }

            case lmPAGE_CLEFS:	//---------------------------------------------------------
		    {
       //         fUnknown = false;       //assume it will be processed
			    //switch (nKeyCode)
			    //{
				   // case int('G'):	// 'g' insert G clef
				   // case int('g'):
					  //  InsertClef(lmE_Sol);
					  //  break;

				   // case int('F'):	// 'f' insert F4 clef
				   // case int('f'):
					  //  InsertClef(lmE_Fa4);
					  //  break;

				   // case int('C'):    // 'c' insert C3 clef
				   // case int('c'):
					  //  InsertClef(lmE_Do3);
					  //  break;

				   // default:
       //                 if (wxIsprint(nKeyCode))
       //                     m_sCmd += wxString::Format(_T("%c"), (char)nKeyCode);
					  //  fUnknown = true;
			    //}
			    break;
		    }

            case lmPAGE_BARLINES:	//---------------------------------------------------------
		    {
                fUnknown = false;       //assume it will be processed
			    switch (nKeyCode)
			    {
				    case int('B'):	// 'b' insert duble barline
				    case int('b'):
					    InsertBarline(lm_eBarlineDouble);
					    break;

				    default:
                        if (wxIsprint(nKeyCode))
                            m_sCmd += wxString::Format(_T("%c"), (char)nKeyCode);
					    fUnknown = true;
			    }
			    break;
		    }

		    default:	// Unknown Tool -----------------------------------------------------
		    {
			    wxLogMessage(_T("[lmScoreCanvas::OnKeyPress] Unknown tool %d."), nTool);
			    fUnknown = true;
		    }


	    }
    }

    // If unidentified tool or unidentified key, log message and skip event.
    // Else, clear command buffer
	if (fUnknown)
    {
        LogKeyEvent(_T("Key Press"), event, nTool);
        event.Skip();       //pass the event. Perhaps it is a menu shortcut
    }
    else
    {
        //the command has been processed. Clear buffer
        m_sCmd = _T("");
    }

	//Display command
    //GetMainFrame()->SetStatusBarMsg(wxString::Format(_T("cmd: %s"), m_sCmd.c_str() ));
}

void lmScoreCanvas::LogKeyEvent(wxString name, wxKeyEvent& event, int nTool)
{
    wxString key = KeyCodeToName( event.GetKeyCode() );
    key += wxString::Format(_T(" (Unicode: %#04x)"), event.GetUnicodeKey());

    wxLogMessage( wxString::Format( _T("[lmScoreCanvas::LogKeyEvent] Event: %s - %s, nKeyCode=%d, (flags = %c%c%c%c). Tool=%d"),
            name.c_str(), key.c_str(), event.GetKeyCode(),
            (event.CmdDown() ? _T('C') : _T('-') ),
            (event.AltDown() ? _T('A') : _T('-') ),
            (event.ShiftDown() ? _T('S') : _T('-') ),
            (event.MetaDown() ? _T('M') : _T('-') ),
            nTool ));
}

wxString lmScoreCanvas::KeyCodeToName(int nKeyCode)
{
    wxString sKey;
    switch ( nKeyCode )
    {
        case WXK_BACK: sKey = _T("BACK"); break;
        case WXK_TAB: sKey = _T("TAB"); break;
        case WXK_RETURN: sKey = _T("RETURN"); break;
        case WXK_ESCAPE: sKey = _T("ESCAPE"); break;
        case WXK_SPACE: sKey = _T("SPACE"); break;
        case WXK_DELETE: sKey = _T("DELETE"); break;

        case WXK_START: sKey = _T("START"); break;
        case WXK_LBUTTON: sKey = _T("LBUTTON"); break;
        case WXK_RBUTTON: sKey = _T("RBUTTON"); break;
        case WXK_CANCEL: sKey = _T("CANCEL"); break;
        case WXK_MBUTTON: sKey = _T("MBUTTON"); break;
        case WXK_CLEAR: sKey = _T("CLEAR"); break;
        case WXK_SHIFT: sKey = _T("SHIFT"); break;
        case WXK_ALT: sKey = _T("ALT"); break;
        case WXK_CONTROL: sKey = _T("CONTROL"); break;
        case WXK_MENU: sKey = _T("MENU"); break;
        case WXK_PAUSE: sKey = _T("PAUSE"); break;
        case WXK_CAPITAL: sKey = _T("CAPITAL"); break;
        case WXK_END: sKey = _T("END"); break;
        case WXK_HOME: sKey = _T("HOME"); break;
        case WXK_LEFT: sKey = _T("LEFT"); break;
        case WXK_UP: sKey = _T("UP"); break;
        case WXK_RIGHT: sKey = _T("RIGHT"); break;
        case WXK_DOWN: sKey = _T("DOWN"); break;
        case WXK_SELECT: sKey = _T("SELECT"); break;
        case WXK_PRINT: sKey = _T("PRINT"); break;
        case WXK_EXECUTE: sKey = _T("EXECUTE"); break;
        case WXK_SNAPSHOT: sKey = _T("SNAPSHOT"); break;
        case WXK_INSERT: sKey = _T("INSERT"); break;
        case WXK_HELP: sKey = _T("HELP"); break;
        case WXK_NUMPAD0: sKey = _T("NUMPAD0"); break;
        case WXK_NUMPAD1: sKey = _T("NUMPAD1"); break;
        case WXK_NUMPAD2: sKey = _T("NUMPAD2"); break;
        case WXK_NUMPAD3: sKey = _T("NUMPAD3"); break;
        case WXK_NUMPAD4: sKey = _T("NUMPAD4"); break;
        case WXK_NUMPAD5: sKey = _T("NUMPAD5"); break;
        case WXK_NUMPAD6: sKey = _T("NUMPAD6"); break;
        case WXK_NUMPAD7: sKey = _T("NUMPAD7"); break;
        case WXK_NUMPAD8: sKey = _T("NUMPAD8"); break;
        case WXK_NUMPAD9: sKey = _T("NUMPAD9"); break;
        case WXK_MULTIPLY: sKey = _T("MULTIPLY"); break;
        case WXK_ADD: sKey = _T("ADD"); break;
        case WXK_SEPARATOR: sKey = _T("SEPARATOR"); break;
        case WXK_SUBTRACT: sKey = _T("SUBTRACT"); break;
        case WXK_DECIMAL: sKey = _T("DECIMAL"); break;
        case WXK_DIVIDE: sKey = _T("DIVIDE"); break;
        case WXK_F1: sKey = _T("F1"); break;
        case WXK_F2: sKey = _T("F2"); break;
        case WXK_F3: sKey = _T("F3"); break;
        case WXK_F4: sKey = _T("F4"); break;
        case WXK_F5: sKey = _T("F5"); break;
        case WXK_F6: sKey = _T("F6"); break;
        case WXK_F7: sKey = _T("F7"); break;
        case WXK_F8: sKey = _T("F8"); break;
        case WXK_F9: sKey = _T("F9"); break;
        case WXK_F10: sKey = _T("F10"); break;
        case WXK_F11: sKey = _T("F11"); break;
        case WXK_F12: sKey = _T("F12"); break;
        case WXK_F13: sKey = _T("F13"); break;
        case WXK_F14: sKey = _T("F14"); break;
        case WXK_F15: sKey = _T("F15"); break;
        case WXK_F16: sKey = _T("F16"); break;
        case WXK_F17: sKey = _T("F17"); break;
        case WXK_F18: sKey = _T("F18"); break;
        case WXK_F19: sKey = _T("F19"); break;
        case WXK_F20: sKey = _T("F20"); break;
        case WXK_F21: sKey = _T("F21"); break;
        case WXK_F22: sKey = _T("F22"); break;
        case WXK_F23: sKey = _T("F23"); break;
        case WXK_F24: sKey = _T("F24"); break;
        case WXK_NUMLOCK: sKey = _T("NUMLOCK"); break;
        case WXK_SCROLL: sKey = _T("SCROLL"); break;
        case WXK_PAGEUP: sKey = _T("PAGEUP"); break;
        case WXK_PAGEDOWN: sKey = _T("PAGEDOWN"); break;

        case WXK_NUMPAD_SPACE: sKey = _T("NUMPAD_SPACE"); break;
        case WXK_NUMPAD_TAB: sKey = _T("NUMPAD_TAB"); break;
        case WXK_NUMPAD_ENTER: sKey = _T("NUMPAD_ENTER"); break;
        case WXK_NUMPAD_F1: sKey = _T("NUMPAD_F1"); break;
        case WXK_NUMPAD_F2: sKey = _T("NUMPAD_F2"); break;
        case WXK_NUMPAD_F3: sKey = _T("NUMPAD_F3"); break;
        case WXK_NUMPAD_F4: sKey = _T("NUMPAD_F4"); break;
        case WXK_NUMPAD_HOME: sKey = _T("NUMPAD_HOME"); break;
        case WXK_NUMPAD_LEFT: sKey = _T("NUMPAD_LEFT"); break;
        case WXK_NUMPAD_UP: sKey = _T("NUMPAD_UP"); break;
        case WXK_NUMPAD_RIGHT: sKey = _T("NUMPAD_RIGHT"); break;
        case WXK_NUMPAD_DOWN: sKey = _T("NUMPAD_DOWN"); break;
        case WXK_NUMPAD_PAGEUP: sKey = _T("NUMPAD_PAGEUP"); break;
        case WXK_NUMPAD_PAGEDOWN: sKey = _T("NUMPAD_PAGEDOWN"); break;
        case WXK_NUMPAD_END: sKey = _T("NUMPAD_END"); break;
        case WXK_NUMPAD_BEGIN: sKey = _T("NUMPAD_BEGIN"); break;
        case WXK_NUMPAD_INSERT: sKey = _T("NUMPAD_INSERT"); break;
        case WXK_NUMPAD_DELETE: sKey = _T("NUMPAD_DELETE"); break;
        case WXK_NUMPAD_EQUAL: sKey = _T("NUMPAD_EQUAL"); break;
        case WXK_NUMPAD_MULTIPLY: sKey = _T("NUMPAD_MULTIPLY"); break;
        case WXK_NUMPAD_ADD: sKey = _T("NUMPAD_ADD"); break;
        case WXK_NUMPAD_SEPARATOR: sKey = _T("NUMPAD_SEPARATOR"); break;
        case WXK_NUMPAD_SUBTRACT: sKey = _T("NUMPAD_SUBTRACT"); break;
        case WXK_NUMPAD_DECIMAL: sKey = _T("NUMPAD_DECIMAL"); break;
        case WXK_NUMPAD_DIVIDE: sKey = _T("NUMPAD_DIVIDE"); break;

        // the following key codes are only generated under Windows currently
         case WXK_WINDOWS_LEFT: sKey = _T("WINDOWS_LEFT"); break;
         case WXK_WINDOWS_RIGHT: sKey = _T("WINDOWS_RIGHT"); break;
         case WXK_WINDOWS_MENU: sKey = _T("WINDOWS_MENU"); break;
         case WXK_COMMAND: sKey = _T("COMMAND"); break;

        // Hardware-specific buttons
         case WXK_SPECIAL1: sKey = _T("SPECIAL1"); break;
         case WXK_SPECIAL2: sKey = _T("SPECIAL2"); break;
         case WXK_SPECIAL3: sKey = _T("SPECIAL3"); break;
         case WXK_SPECIAL4: sKey = _T("SPECIAL4"); break;
         case WXK_SPECIAL5: sKey = _T("SPECIAL5"); break;
         case WXK_SPECIAL6: sKey = _T("SPECIAL6"); break;
         case WXK_SPECIAL7: sKey = _T("SPECIAL7"); break;
         case WXK_SPECIAL8: sKey = _T("SPECIAL8"); break;
         case WXK_SPECIAL9: sKey = _T("SPECIAL9"); break;
         case WXK_SPECIAL10: sKey = _T("SPECIAL10"); break;
         case WXK_SPECIAL11: sKey = _T("SPECIAL11"); break;
         case WXK_SPECIAL12: sKey = _T("SPECIAL12"); break;
         case WXK_SPECIAL13: sKey = _T("SPECIAL13"); break;
         case WXK_SPECIAL14: sKey = _T("SPECIAL14"); break;
         case WXK_SPECIAL15: sKey = _T("SPECIAL15"); break;
         case WXK_SPECIAL16: sKey = _T("SPECIAL16"); break;
         case WXK_SPECIAL17: sKey = _T("SPECIAL17"); break;
         case WXK_SPECIAL18: sKey = _T("SPECIAL18"); break;
         case WXK_SPECIAL19: sKey = _T("SPECIAL19"); break;
         case WXK_SPECIAL20: sKey = _T("SPECIAL20"); break;


        default:
        {
            if ( wxIsprint((int)nKeyCode) )
                sKey.Printf(_T("'%c'"), (char)nKeyCode);
            else if ( nKeyCode > 0 && nKeyCode < 27 )
                sKey.Printf(_T("Ctrl-%c"), _T('A') + nKeyCode - 1);
            else
                sKey.Printf(_T("unknown (%d)"), nKeyCode);
        }
    }
    return sKey;
}

void lmScoreCanvas::OnEraseBackground(wxEraseEvent& event)
{
	// AWARE: This method is empty on purpose

	// When wxWidgets wants to update the display it emits two events: an erase
	// background event and a paint event.
	// To prevent flickering we are not going to erase the background and the view
	// will paint it when needed, but only on the background areas not on all
	// canvas areas
}

wxMenu* lmScoreCanvas::GetContextualMenu(bool fInitialize)
{
	if (m_pMenu) delete m_pMenu;
	m_pMenu = new wxMenu();

	if (!fInitialize)
		return m_pMenu;

#if defined(__WXMSW__) || defined(__WXGTK__)

    wxMenuItem* pItem;
    wxSize nIconSize(16, 16);

    pItem = new wxMenuItem(m_pMenu, lmPOPUP_Cut, _("&Cut"));
    pItem->SetBitmap( wxArtProvider::GetBitmap(_T("tool_cut"), wxART_TOOLBAR, nIconSize) );
    m_pMenu->Append(pItem);

    //pItem = new wxMenuItem(m_pMenu, lmPOPUP_Copy, _("&Copy"));
    //pItem->SetBitmap( wxArtProvider::GetBitmap(_T("tool_copy"), wxART_TOOLBAR, nIconSize) );
    //m_pMenu->Append(pItem);

    //pItem = new wxMenuItem(m_pMenu, lmPOPUP_Paste, _("&Paste"));
    //pItem->SetBitmap( wxArtProvider::GetBitmap(_T("tool_paste"), wxART_TOOLBAR, nIconSize) );
    //m_pMenu->Append(pItem);

	//m_pMenu->AppendSeparator();

    //pItem = new wxMenuItem(m_pMenu, lmPOPUP_Color, _("Colour"));
    //pItem->SetBitmap( wxArtProvider::GetBitmap(_T("opt_colors"), wxART_TOOLBAR, nIconSize) );
    //m_pMenu->Append(pItem);

    pItem = new wxMenuItem(m_pMenu, lmPOPUP_Properties, _("Edit"));
    pItem->SetBitmap( wxArtProvider::GetBitmap(_T("tool_properties"), wxART_TOOLBAR, nIconSize) );
    m_pMenu->Append(pItem);

	//m_pMenu->AppendSeparator();


#else
	m_pMenu->Append(lmPOPUP_Cut, _("&Cut"));
	//m_pMenu->Append(lmPOPUP_Copy, _("&Copy"));
	//m_pMenu->Append(lmPOPUP_Paste, _("&Paste"));
	//m_pMenu->AppendSeparator();
	//m_pMenu->Append(lmPOPUP_Color, _("Colour"));
    m_pMenu->Append(lmPOPUP_Properties, _("Edit"));

	//m_pMenu->AppendSeparator();

#endif

#ifdef __WXDEBUG__
    m_pMenu->Append(lmPOPUP_DumpShape, _T("Dump shape"));
#endif

	return m_pMenu;
}

void lmScoreCanvas::OnCut(wxCommandEvent& event)
{
	WXUNUSED(event);
    DeleteSelection();
}

void lmScoreCanvas::OnCopy(wxCommandEvent& event)
{
	WXUNUSED(event);
}

void lmScoreCanvas::OnPaste(wxCommandEvent& event)
{
	WXUNUSED(event);
}

void lmScoreCanvas::OnColor(wxCommandEvent& event)
{
	WXUNUSED(event);
}

void lmScoreCanvas::OnProperties(wxCommandEvent& event)
{
	WXUNUSED(event);
	m_pMenuOwner->OnProperties(this, m_pMenuGMO);
}

void lmScoreCanvas::OnDeleteTiePrev(wxCommandEvent& event)
{
	WXUNUSED(event);
	wxASSERT(m_pMenuOwner->IsComponentObj());
    wxASSERT( ((lmComponentObj*)m_pMenuOwner)->IsStaffObj());
    wxASSERT( ((lmStaffObj*)m_pMenuOwner)->IsNoteRest());
    wxASSERT( ((lmNoteRest*)m_pMenuOwner)->IsNote());

    DeleteTie( (lmNote*)m_pMenuOwner );
}

void lmScoreCanvas::OnAttachText(wxCommandEvent& event)
{
	WXUNUSED(event);
	wxASSERT(m_pMenuOwner->IsComponentObj());

    AttachNewText( (lmComponentObj*)m_pMenuOwner );
}

void lmScoreCanvas::OnScoreTitles(wxCommandEvent& event)
{
	WXUNUSED(event);
	AddTitle();
}

void lmScoreCanvas::OnViewPageMargins(wxCommandEvent& event)
{
    g_fShowMargins = !g_fShowMargins;
    lmDocument* pDoc = GetMainFrame()->GetActiveDoc();
    if (pDoc)
    {
	    pDoc->Modify(true);
        pDoc->UpdateAllViews((wxView*)NULL, new lmUpdateHint() );
    }
}

#ifdef __WXDEBUG__
void lmScoreCanvas::OnDumpShape(wxCommandEvent& event)
{
	WXUNUSED(event);
    lmDlgDebug dlg(this, _T("GMObject dump"), m_pMenuGMO->Dump(0));
    dlg.ShowModal();
}
#endif

void lmScoreCanvas::SelectNoteDuration(int iButton)
{
	lmToolBox* pToolBox = GetMainFrame()->GetActiveToolBox();
	if (pToolBox)
		((lmToolPageNotes*)pToolBox->GetToolPanel(lmPAGE_NOTES))->SetNoteDurationButton(iButton);
}

void lmScoreCanvas::SelectOctave(bool fUp)
{
	lmToolBox* pToolBox = GetMainFrame()->GetActiveToolBox();
	if (pToolBox)
		((lmToolPageNotes*)pToolBox->GetToolPanel(lmPAGE_NOTES))->SetOctave(fUp);
}

void lmScoreCanvas::SelectOctave(int nOctave)
{
	lmToolBox* pToolBox = GetMainFrame()->GetActiveToolBox();
	if (pToolBox)
		((lmToolPageNotes*)pToolBox->GetToolPanel(lmPAGE_NOTES))->SetOctave(nOctave);
}

void lmScoreCanvas::SelectVoice(bool fUp)
{
	lmToolBox* pToolBox = GetMainFrame()->GetActiveToolBox();
	if (pToolBox)
		((lmToolPageNotes*)pToolBox->GetToolPanel(lmPAGE_NOTES))->SetVoice(fUp);
}

void lmScoreCanvas::SelectVoice(int nVoice)
{
	lmToolBox* pToolBox = GetMainFrame()->GetActiveToolBox();
	if (pToolBox)
		((lmToolPageNotes*)pToolBox->GetToolPanel(lmPAGE_NOTES))->SetVoice(nVoice);
}

void lmScoreCanvas::SelectNoteAccidentals(bool fNext)
{
	lmToolBox* pToolBox = GetMainFrame()->GetActiveToolBox();
	if (pToolBox)
    {
        if (fNext)
            ((lmToolPageNotes*)pToolBox->GetToolPanel(lmPAGE_NOTES))->SelectNextAccidental();
        else
            ((lmToolPageNotes*)pToolBox->GetToolPanel(lmPAGE_NOTES))->SelectPrevAccidental();
    }
}

void lmScoreCanvas::SelectNoteDots(bool fNext)
{
	lmToolBox* pToolBox = GetMainFrame()->GetActiveToolBox();
	if (pToolBox)
    {
        if (fNext)
            ((lmToolPageNotes*)pToolBox->GetToolPanel(lmPAGE_NOTES))->SelectNextDot();
        else
            ((lmToolPageNotes*)pToolBox->GetToolPanel(lmPAGE_NOTES))->SelectPrevDot();
    }
}

void lmScoreCanvas::SynchronizeToolBox()
{
    //synchronize toolbox selected options with current selection and cursor object

	lmToolBox* pToolBox = GetMainFrame()->GetActiveToolBox();
	if (!pToolBox) return;

    lmGMSelection* pSelection = m_pView->GetSelection();
    if (!pSelection->IsEmpty())
    {
        //there is a selection. Disable options related to cursor
        SynchronizeToolBoxWithCaret(false);
        SynchronizeToolBoxWithSelection(true);
    }
    else
    {
        //No selection. Disable options related to selections
        SynchronizeToolBoxWithCaret(true);
        SynchronizeToolBoxWithSelection(false);
    }

	//options independent from caret/selection

    switch( pToolBox->GetSelectedToolPage() )
    {
        case lmPAGE_NONE:
            return;         //nothing selected!

        case lmPAGE_NOTES:
            //voice and octave
            {
                lmToolPageNotes* pPage = (lmToolPageNotes*)pToolBox->GetToolPanel(lmPAGE_NOTES);
                lmGrpOctave* pGrp = (lmGrpOctave*)pPage->GetToolGroup(lmGRP_Octave);
                pGrp->SetOctave(m_nOctave);
            }
            break;

        case lmPAGE_SELECTION:
        case lmPAGE_CLEFS:
        case lmPAGE_KEY_SIGN:
        case lmPAGE_TIME_SIGN:
        case lmPAGE_BARLINES:
            lmTODO(_T("[lmScoreCanvas::SynchronizeToolBoxWithCaret] Code to sync. this tool"));
            break;

        default:
            wxASSERT_MSG(false, _T("[lmScoreCanvas::SynchronizeToolBox] Default case reached"));
    }


}

void lmScoreCanvas::SynchronizeToolBoxWithCaret(bool fEnable)
{
    //synchronize toolbox selected options with current pointed object properties

	lmToolBox* pToolBox = GetMainFrame()->GetActiveToolBox();
	if (!pToolBox) return;

    //get cursor
    lmScoreCursor* pCursor = m_pDoc->GetScore()->GetCursor();
	wxASSERT(pCursor);

	//get object pointed by the cursor
    lmStaffObj* pCursorSO = pCursor->GetStaffObj();
    switch( pToolBox->GetSelectedToolPage() )
    {
        case lmPAGE_NONE:
            return;         //nothing selected!

        case lmPAGE_NOTES:
            //cut beams
            {
                bool fCut = false;
                if (fEnable && pCursorSO)
                    fCut = IsCursorValidToCutBeam();

                //syncr. cut beam button
                lmToolPageNotes* pPage = (lmToolPageNotes*)pToolBox->GetToolPanel(lmPAGE_NOTES);
                lmGrpBeams* pGrp = (lmGrpBeams*)pPage->GetToolGroup(lmGRP_Beams);
                pGrp->EnableTool(lmTOOL_BEAMS_CUT, fCut);
            }
            break;

        case lmPAGE_SELECTION:
        case lmPAGE_CLEFS:
        case lmPAGE_KEY_SIGN:
        case lmPAGE_TIME_SIGN:
        case lmPAGE_BARLINES:
            lmTODO(_T("[lmScoreCanvas::SynchronizeToolBoxWithCaret] Code to sync. this tool"));
            break;

        default:
            wxASSERT(false);
    }
}

void lmScoreCanvas::SynchronizeToolBoxWithSelection(bool fEnable)
{
    //synchronize toolbox selected options with current selected object properties

	lmToolBox* pToolBox = GetMainFrame()->GetActiveToolBox();
	if (!pToolBox) return;

    lmGMSelection* pSelection = m_pView->GetSelection();

    switch( pToolBox->GetSelectedToolPage() )
    {
        case lmPAGE_NONE:
            return;         //nothing selected!

        case lmPAGE_NOTES:
            //sync. duration, dots, accidentals, ties, tuples
            {
                lmToolPageNotes* pPage = (lmToolPageNotes*)pToolBox->GetToolPanel(lmPAGE_NOTES);

                //flags to enable/disable tools
                bool fEnableTie = false;
                bool fCheckTie = false;
                bool fEnableTuplet = false;
                bool fCheckTuplet = false;
                bool fEnableJoinBeam = false;

                if (fEnable && !pSelection->IsEmpty())
                {
                    //find common values for all selected notes, if any
                    lmGMObject* pGMO = pSelection->GetFirst();
                    bool fNoteFound = false;
                    int nAcc, nDots, nDuration;
                    while (pGMO)
                    {
                        if (pGMO->GetType() == eGMO_ShapeNote)
                        {
                            lmNote* pNote = (lmNote*)pGMO->GetScoreOwner();
                            int nThisDuration = (int)pNote->GetNoteType() - 1;
                            int nThisDots = pNote->GetNumDots() - 1;
                            int nThisAcc = pNote->GetAPitch().Accidentals();
                            if (!fNoteFound)
                            {
                                fNoteFound = true;
                                nDuration = nThisDuration;
                                nDots = nThisDots;
                                nAcc = nThisAcc;
                            }
                            else
                            {
                                if (nDuration != nThisDuration)
                                    nDuration = -1;
                                if (nDots != nThisDots)
                                    nDots = -1;
                                if (nAcc != nThisAcc)
                                    nAcc = -10;
                            }
                        }
                        pGMO = pSelection->GetNext();
                    }

                    //if any note found, proceed to sync. the toolbox
                    if (fNoteFound)
                    {
                        //save current options
                        if (!m_fToolBoxSavedOptions)
                        {
                            m_fToolBoxSavedOptions = true;
                            m_nTbAcc = pPage->GetNoteAccButton();
                            m_nTbDots = pPage->GetNoteDotsButton();
                            m_nTbDuration = pPage->GetNoteDurationButton();
                        }
                        //translate Acc
                        switch(nAcc)
                        {
                            case -2:  nAcc = 3;  break;
                            case -1:  nAcc = 1;  break;
                            case  0:  nAcc = -1; break;
                            case  1:  nAcc = 2;  break;
                            case  2:  nAcc = 4;  break;
                            default:
                                nAcc = -1;
                        }

                        pPage->SetNoteDotsButton(nDots);
                        pPage->SetNoteAccButton(nAcc);
                        pPage->SetNoteDurationButton( nDuration );
                    }

                    //Ties status
                    lmNote* pStartNote;
                    fEnableTie = fNoteFound;
                    if (IsSelectionValidForTies(&pStartNote))
                        fCheckTie = (pStartNote && pStartNote->IsTiedToNext());
                    else
                        fEnableTie = false;


                    //Tuplets status
                    fEnableTuplet = fNoteFound;
                    if (fNoteFound)
                    {
                        lmNoteRest* pStartNR = IsSelectionValidForTuplet();
                        if (pStartNR)
                            fCheckTuplet = pStartNR->IsInTuplet();
                        else
                            fEnableTuplet = false;
                    }

                    //Join beams
                    fEnableJoinBeam = fNoteFound;
                    if (fNoteFound)
                        fEnableJoinBeam = IsSelectionValidToJoinBeam();

                }

                //enable/disable tools

                //Ties
                lmGrpTieTuplet* pGrp = (lmGrpTieTuplet*)pPage->GetToolGroup(lmGRP_TieTuplet);
                pGrp->EnableTool(lmTOOL_NOTE_TIE, fEnableTie);
                if (fEnableTie)
                    pPage->SetToolTie(fCheckTie);

                //Tuples
                pGrp = (lmGrpTieTuplet*)pPage->GetToolGroup(lmGRP_TieTuplet);
                pGrp->EnableTool(lmTOOL_NOTE_TUPLET, fEnableTuplet);
                if (fEnableTuplet)
                    pPage->SetToolTuplet(fCheckTuplet);

                //Join beams
                lmGrpBeams* pGrpBeams = (lmGrpBeams*)pPage->GetToolGroup(lmGRP_Beams);
                pGrpBeams->EnableTool(lmTOOL_BEAMS_JOIN, fEnableJoinBeam);

            }
            break;

        case lmPAGE_SELECTION:
        case lmPAGE_CLEFS:
        case lmPAGE_KEY_SIGN:
        case lmPAGE_TIME_SIGN:
        case lmPAGE_BARLINES:
            lmTODO(_T("[lmScoreCanvas::SynchronizeToolBoxWithSelection] Code to sync. this tool"));
            break;

        default:
            wxASSERT(false);
    }
}

void lmScoreCanvas::RestoreToolBoxSelections()
{
    //restore toolbox selected options to those previously selected by user

    if (!m_fToolBoxSavedOptions) return;        //nothing to do

    m_fToolBoxSavedOptions = false;

	lmToolBox* pToolBox = GetMainFrame()->GetActiveToolBox();
	if (!pToolBox) return;

    switch( pToolBox->GetSelectedToolPage() )
    {
        case lmPAGE_NONE:
            return;         //nothing selected!

        case lmPAGE_NOTES:
            //restore duration, dots, accidentals
            {
                lmToolPageNotes* pTool = (lmToolPageNotes*)pToolBox->GetToolPanel(lmPAGE_NOTES);
                pTool->SetNoteDotsButton(m_nTbDots);
                pTool->SetNoteAccButton(m_nTbAcc);
                pTool->SetNoteDurationButton(m_nTbDuration);
            }
            break;

        case lmPAGE_SELECTION:
        case lmPAGE_CLEFS:
        case lmPAGE_KEY_SIGN:
        case lmPAGE_TIME_SIGN:
        case lmPAGE_BARLINES:
            lmTODO(_T("[lmScoreCanvas::RestoreToolBoxSelections] Code to restore this tool"));
            break;

        default:
            wxASSERT(false);
    }
}

bool lmScoreCanvas::IsSelectionValidForTies(lmNote** ppStartNote, lmNote** ppEndNote)
{
    //Returns TRUE if current selection is valid for adding/removing a tie.
    //If valid, returns pointers to start and end notes, if not NULL parameters received


    //Conditions to be valid:
    //   1. The first note found either is tied to next or can be tied to next one
    //   2. If condition 1 is true, the next note must also be in the selection

    //verify conditions
    lmGMSelection* pSelection = m_pView->GetSelection();
    bool fValid = false;
    lmNote* pStart = (lmNote*)NULL;
    lmNote* pEnd = (lmNote*)NULL;

    lmGMObject* pGMO = pSelection->GetFirst();
    while (pGMO)
    {
        if (pGMO->GetType() == eGMO_ShapeNote)
        {
            if (!pStart)
            {
                //first note found.
                //Verify if it is tied to next or can be tied to next
                pStart = (lmNote*)pGMO->GetScoreOwner();
                if (pStart->IsTiedToNext())
                    pEnd = pStart->GetTiedNoteNext();   //Tied to next.
                else
                    pEnd = pStart->GetVStaff()->FindPossibleEndOfTie(pStart);
            }
            else
            {
                //Start note processed. verify if next note is also in the selection
                if (pEnd)
                {
                    if (pEnd == (lmNote*)pGMO->GetScoreOwner())
                    {
                        fValid = true;      //ok. End note is in the selection
                        break;
                    }
                }
                else
                    return false;
            }

        }
        pGMO = pSelection->GetNext();
    }

    if (fValid)
    {
        if (ppStartNote)
            *ppStartNote = pStart;
        if (ppEndNote)
            *ppEndNote = pEnd;
        return true;
    }
    else
        return (lmNote*)NULL;
}

lmNoteRest* lmScoreCanvas::IsSelectionValidForTuplet()
{
    //Checks if current selection is valid for adding/removing a tuplet.
    //If valid, returns a pointer to the first note, else returns NULL

    //Conditions to be valid:
    //  Either:
    //   1. All notes/rest in the seleccion are not in a tuplet, are consecutive, and are
    //      in the same voice.
    //   2. All notes/rest in the seleccion are in a tuplet, it is the same tuplet for all
    //      of them, and there are no more notes/rests in the tuplet.

    //verify conditions
    lmGMSelection* pSelection = m_pView->GetSelection();
    bool fValid = true;
    lmNoteRest* pStart = (lmNoteRest*)NULL;
    lmTupletBracket* pTuplet = (lmTupletBracket*)NULL;

    int nNumNotes = 0;
    int nVoice;
    lmGMObject* pGMO = pSelection->GetFirst();
    while (pGMO && fValid)
    {
        if (pGMO->GetType() == eGMO_ShapeNote || pGMO->GetType() == eGMO_ShapeRest)
        {
            nNumNotes++;
            if (!pStart)
            {
                //This is the first note/rest
                pStart = (lmNoteRest*)pGMO->GetScoreOwner();
                if (pStart->IsInTuplet())
                    pTuplet = pStart->GetTuplet();
                else
                    nVoice = pStart->GetVoice();
            }
            else
            {
                lmNoteRest* pNext = (lmNoteRest*)pGMO->GetScoreOwner();
                fValid &= pTuplet == pNext->GetTuplet();
                if (!pTuplet)
                    fValid &= nVoice == pNext->GetVoice();
            }
        }
        pGMO = pSelection->GetNext();
    }

    //check that all notes in the tuplet are selected
    if (fValid && pTuplet)
        fValid &= (pTuplet->NumNotes() == nNumNotes);

    //return results
    if (fValid)
        return pStart;
    else
        return (lmNoteRest*)NULL;
}

bool lmScoreCanvas::IsCursorValidToCutBeam()
{
    //Returns TRUE if object pointed by cursor is valid for breaking a beam.

    //Conditions to be valid:
    //  The object must be a note/rest in a beam
    //  It must not be the first one in the beam

    //get cursor
    lmScoreCursor* pCursor = m_pDoc->GetScore()->GetCursor();
	wxASSERT(pCursor);

	//get object pointed by the cursor
    lmStaffObj* pCursorSO = pCursor->GetStaffObj();
    if (pCursorSO && pCursorSO->IsNoteRest())
    {
        //it is a note/rest. Verify if it is beamed
        lmNoteRest* pNR = (lmNoteRest*)pCursorSO;
        if (pNR->IsBeamed())
        {
            //ok. it is in a beam. Verify that it is not the first object in the beam
            lmBeam* pBeam = pNR->GetBeam();
            if (pNR != pBeam->GetStartNoteRest())
                return true;
        }
    }

    return false;
}

bool lmScoreCanvas::IsSelectionValidToJoinBeam()
{
    //Returns TRUE if current selection is valid either:
    // - to create a beamed group with the selected notes,
    // - to join two or more beamed groups
    // - or to add a note to a beamed group

    //Conditions to be valid:
    //   1. All notes/rest in the seleccion are consecutive, are in the same
    //      voice (unless in chord), and must be eighths or shorter ones.
    //   2. If not beamed, first note/rest must be a note
    //   3. If not beamed, last note/rest must be a note
    //   4. If beamed, all selected note/rest must not be in the same beam

    //verify conditions
    lmGMSelection* pSelection = m_pView->GetSelection();
    bool fValid = true;
    lmNoteRest* pStart = (lmNoteRest*)NULL;

    int nNumNotes = 0;
    int nVoice;
    lmNoteRest* pLast = (lmNoteRest*)NULL;
    lmGMObject* pGMO = pSelection->GetFirst();
    bool fAllBeamed = true;     //assume that all are beamed in the same beam
    lmBeam* pCurBeam = (lmBeam*)NULL;
    while (pGMO && fValid)
    {
        if (pGMO->GetType() == eGMO_ShapeNote || pGMO->GetType() == eGMO_ShapeRest)
        {
            nNumNotes++;
            if (!pStart)
            {
                //This is the first note/rest. If not beamed, it must be a note
                pStart = (lmNoteRest*)pGMO->GetScoreOwner();
                nVoice = pStart->GetVoice();
                if (!pStart->IsBeamed())
                {
                    fValid &= pStart->IsNote();
                    fAllBeamed = false;
                }
                else
                    pCurBeam = pStart->GetBeam();
            }
            else
            {
                // verify voice, and that it is an eighth or shorter
                pLast = (lmNoteRest*)pGMO->GetScoreOwner();
                fValid &= pLast->GetNoteType() >= eEighth;
                fValid &= nVoice == pLast->GetVoice() ||
                          (pLast->IsNote() && ((lmNote*)pLast)->IsInChord());

                //verify that if beamed, all selected note/rest must not be in the same beam
                fAllBeamed &= pLast->IsBeamed();
                if (fValid && fAllBeamed)
                    fAllBeamed &= (pCurBeam == pLast->GetBeam());
            }
        }
        pGMO = pSelection->GetNext();
    }

    //verify last note/rest. If not beamed, it must be a note
    if (pLast && !pLast->IsBeamed())
        fValid &= pLast->IsNote();

    return fValid && !fAllBeamed && nNumNotes > 1;
}



//dragging on canvas with left button: selection
void lmScoreCanvas::OnCanvasBeginDragLeft(lmDPoint vCanvasPos, lmUPoint uPagePos,
                                          int nKeys)
{
    //Begin a selection with left button

	WXUNUSED(nKeys);

    wxClientDC dc(this);
	dc.SetLogicalFunction(wxINVERT);
    m_fDraggingObject = false;

	m_pView->DrawSelectionArea(dc, m_vStartDrag.x, m_vStartDrag.y, vCanvasPos.x, vCanvasPos.y);
}

void lmScoreCanvas::OnCanvasContinueDragLeft(bool fDraw, lmDPoint vCanvasPos,
                                             lmUPoint uPagePos, int nKeys)
{
    //Continue a selection with left button
	//fDraw:  true -> draw a rectangle, false -> remove rectangle

    WXUNUSED(fDraw);
    WXUNUSED(nKeys);

    wxClientDC dc(this);
    dc.SetLogicalFunction(wxINVERT);
    m_fDraggingObject = false;

    m_pView->DrawSelectionArea(dc, m_vStartDrag.x, m_vStartDrag.y, vCanvasPos.x, vCanvasPos.y);
}

void lmScoreCanvas::OnCanvasEndDragLeft(lmDPoint vCanvasPos, lmUPoint uPagePos,
                                        int nKeys)
{
    //End a selection with left button

    WXUNUSED(nKeys);

	//remove selection rectangle
    //dc.SetLogicalFunction(wxINVERT);
    //DrawSelectionArea(dc, m_vStartDrag.x, m_vStartDrag.y, vCanvasPos.x, vCanvasPos.y);
    m_fDraggingObject = false;

	//save final point
	m_vEndDrag = vCanvasPos;

    //select all objects within the selection area
    lmLUnits uXMin, uXMax, uYMin, uYMax;
    uXMin = wxMin(uPagePos.x, m_uStartDrag.x);
    uXMax = wxMax(uPagePos.x, m_uStartDrag.x);
    uYMin = wxMin(uPagePos.y, m_uStartDrag.y);
    uYMax = wxMax(uPagePos.y, m_uStartDrag.y);

    //find all objects whithin the selected area and create a selection
    //
    //TODO
    //  The selected area could cross page boundaries. Therefore it is necessary
    //  to locate the affected pages and invoke CreateSelection / AddToSelecction
    //  for each affected page
    //
    if (nKeys == lmKEY_NONE)
    {
        m_pView->SelectGMObjectsInArea(m_nNumPage, uXMin, uXMax, uYMin, uYMax, true);     //true: redraw view content
    }
    //else if (nKeys & lmKEY_CTRL)
    //{
    //    //find all objects in drag area and add them to 'selection'
    //    m_graphMngr.AddToSelection(m_nNumPage, uXMin, uXMax, uYMin, uYMax);
    //    //mark as 'selected' all objects in the selection
    //    m_pCanvas->SelectObjects(lmSELECT, m_graphMngr.GetSelection());
    //}
}

//dragging on canvas with right button
void lmScoreCanvas::OnCanvasBeginDragRight(lmDPoint vCanvasPos, lmUPoint uPagePos,
                                           int nKeys)
{
    WXUNUSED(vCanvasPos);
    WXUNUSED(uPagePos);
    WXUNUSED(nKeys);
    m_fDraggingObject = false;
}

void lmScoreCanvas::OnCanvasContinueDragRight(bool fDraw, lmDPoint vCanvasPos,
                                              lmUPoint uPagePos, int nKeys)
{
    WXUNUSED(fDraw);
    WXUNUSED(vCanvasPos);
    WXUNUSED(uPagePos);
    WXUNUSED(nKeys);
    m_fDraggingObject = false;
}

void lmScoreCanvas::OnCanvasEndDragRight(lmDPoint vCanvasPos, lmUPoint uPagePos,
                                         int nKeys)
{
    WXUNUSED(vCanvasPos);
    WXUNUSED(uPagePos);
    WXUNUSED(nKeys);

    m_fDraggingObject = false;
    SetFocus();
}


//dragging object with left button
void lmScoreCanvas::OnObjectBeginDragLeft(wxMouseEvent& event, wxDC* pDC,
                                          lmDPoint vCanvasPos, lmDPoint vCanvasOffset,
                                          lmUPoint uPagePos, int nKeys)
{
    m_fDraggingObject = true;
    if (!m_pView->OnObjectBeginDragLeft(event, pDC, vCanvasPos, vCanvasOffset,
                                        uPagePos, nKeys, m_pDraggedGMO,
                                        m_vDragHotSpot, m_uHotSpotShift) )
    {
        m_nDragState = lmDRAG_NONE;
        m_fDraggingObject = false;
    }
    //else
    //    DoCaptureMouse();
}

void lmScoreCanvas::OnObjectContinueDragLeft(wxMouseEvent& event, wxDC* pDC,
                                             bool fDraw, lmDPoint vCanvasPos,
                                             lmDPoint vCanvasOffset, lmUPoint uPagePos,
                                             int nKeys)
{
    m_fDraggingObject = true;
    m_pView->OnObjectContinueDragLeft(event, pDC, fDraw, vCanvasPos,
                                      vCanvasOffset, uPagePos, nKeys);
}

void lmScoreCanvas::OnObjectEndDragLeft(wxMouseEvent& event, wxDC* pDC,
                                        lmDPoint vCanvasPos, lmDPoint vCanvasOffset,
                                        lmUPoint uPagePos, int nKeys)
{
    m_pView->OnObjectEndDragLeft(event, pDC, vCanvasPos, vCanvasOffset,
                                 uPagePos, nKeys);
    m_fDraggingObject = false;
    //wxLogMessage(_T("[lmScoreCanvas::OnObjectEndDragLeft] will invoke DoReleaseMouse"));
    //DoReleaseMouse();
}


//dragging object with right button
void lmScoreCanvas::OnObjectBeginDragRight(wxMouseEvent& event, wxDC* pDC,
                                           lmDPoint vCanvasPos, lmDPoint vCanvasOffset,
                                           lmUPoint uPagePos, int nKeys)
{
    WXUNUSED(event);
    WXUNUSED(pDC);
    WXUNUSED(vCanvasPos);
    WXUNUSED(vCanvasOffset);
    WXUNUSED(nKeys);
    WXUNUSED(uPagePos);

    m_fDraggingObject = true;
	m_pView->HideCaret();
    SetFocus();

	#ifdef __WXDEBUG__
	g_pLogger->LogTrace(_T("OnMouseEvent"), _T("OnObjectBeginDragRight()"));
	#endif

}

void lmScoreCanvas::OnObjectContinueDragRight(wxMouseEvent& event, wxDC* pDC,
                                              bool fDraw, lmDPoint vCanvasPos,
                                              lmDPoint vCanvasOffset,
                                              lmUPoint uPagePos, int nKeys)
{
    WXUNUSED(event);
    WXUNUSED(pDC);
    WXUNUSED(fDraw);
    WXUNUSED(vCanvasPos);
    WXUNUSED(vCanvasOffset);
    WXUNUSED(uPagePos);
    WXUNUSED(nKeys);

	#ifdef __WXDEBUG__
	g_pLogger->LogTrace(_T("OnMouseEvent"), _T("OnObjectContinueDragRight()"));
	#endif

    m_fDraggingObject = true;
}

void lmScoreCanvas::OnObjectEndDragRight(wxMouseEvent& event, wxDC* pDC,
                                         lmDPoint vCanvasPos, lmDPoint vCanvasOffset,
                                         lmUPoint uPagePos, int nKeys)
{
    WXUNUSED(event);
    WXUNUSED(pDC);
    WXUNUSED(vCanvasPos);
    WXUNUSED(vCanvasOffset);
    WXUNUSED(uPagePos);
    WXUNUSED(nKeys);

	#ifdef __WXDEBUG__
	g_pLogger->LogTrace(_T("OnMouseEvent"), _T("OnObjectEndDragRight()"));
	#endif

	m_pView->ShowCaret();
    m_fDraggingObject = false;
}

void lmScoreCanvas::OnToolClick(lmGMObject* pGMO, lmUPoint uPagePos, float rTime)
{
    //Mouse click on a valid area while dragging a tool. Determine user action and issue
    //the appropriate edition command

    if(!pGMO) return;

    lmShapeStaff* pShapeStaff = (lmShapeStaff*)NULL;
    lmBoxSliceInstr* pBSI = (lmBoxSliceInstr*)NULL;
    if (pGMO->IsShapeStaff())
    {
        pShapeStaff = (lmShapeStaff*)pGMO;
        lmBox* pBox = m_pView->FindBoxAt(m_nNumPage, uPagePos);
        wxASSERT(pBox && pBox->IsBoxSliceInstr());
        pBSI = (lmBoxSliceInstr*)pBox;
    }
    else if (pGMO->IsBoxSliceInstr())
    {
        pBSI = (lmBoxSliceInstr*)pGMO;
        pShapeStaff = ((lmBoxSliceInstr*)pGMO)->GetOwnerSystem()->GetStaffShape(1);
    }
    else
        return;     //click out of valid area

    if (pShapeStaff && pBSI)
    {
        wxLogMessage(_T("[lmScoreCanvas::OnToolClick] Insert note. MousePagePos=(%.2f, %.2f)"),
                     uPagePos.x, uPagePos.y);
        //Move caret
	    lmVStaff* pVStaff = pBSI->GetInstrument()->GetVStaff();
	    int nMeasure = pBSI->GetNumMeasure();
	    int nStaff = pShapeStaff->GetNumStaff();
	    m_pView->MoveCaretNearTo(uPagePos, pVStaff, nStaff, nMeasure);
        //m_pView->MoveCaretTo(pVStaff, nStaff, nMeasure, rTime);

        GetToolBoxValuesForPage(lmPAGE_NOTES);
        //compute note/rest duration
		float rDuration = lmLDPParser::GetDefaultDuration(m_nSelNoteType, m_nSelDots, 0, 0);

        //get step and octave from mouse position on staff
        int nLineSpace = pShapeStaff->GetLineSpace(uPagePos.y);     //0=first ledger line below staff
        //to determine octave and step it is necessary to know the clef. As caret is
        //placed at insertion point we could get these information from caret
        lmContext* pContext = m_pDoc->GetScore()->GetCursor()->GetCurrentContext();
        lmEClefType nClefType = pContext->GetClefType();
        lmDPitch dpNote = ::GetFirstLineDPitch(nClefType);  //get diatonic pitch for first line
        dpNote += (nLineSpace - 2);     //pitch for note to insert
        //get step and octave
        int nOctave = DPitch_Octave(dpNote);
        int nStep = DPitch_Step(dpNote);
        wxLogMessage(_T("[lmScoreCanvas::OnToolClick] Click on line/space %d, octave=%d, step=%d, note=%s"),
                        nLineSpace, nOctave, nStep, DPitch_ToLDPName(dpNote).c_str() );

        //default values
        lmNote* pBaseOfChord = (lmNote*)NULL;
        bool fTiedPrev = false;

        //do insert note
		InsertNote(lm_ePitchRelative, nStep, nOctave, m_nSelNoteType, rDuration,
				   m_nSelDots, m_nSelNotehead, m_nSelAcc, m_nSelVoice,
                   pBaseOfChord, fTiedPrev);
    }
}

//non-dragging events: click on an object
void lmScoreCanvas::OnLeftClickOnObject(lmGMObject* pGMO, lmDPoint vCanvasPos,
                                        lmUPoint uPagePos, int nKeys)
{
    // mouse left click on object
    // uPagePos: click point, referred to current page origin

    WXUNUSED(vCanvasPos);
    WXUNUSED(nKeys);

    //AWARE: pGMO must exist and it must be a shape. It can not be an lmBox as, since renderization
    //of shapes is organized in layers, lmBoxes are no longer taken into account in hit testing.
    wxASSERT(pGMO && pGMO->IsShape());


	m_pView->HideCaret();

	#ifdef __WXDEBUG__
	g_pLogger->LogTrace(_T("OnMouseEvent"), _T("OnLeftClickOnObject()"));
	#endif

    m_pView->DeselectAllGMObjects(true);
    SetFocus();

    if (pGMO->IsShapeStaff())
    {
	    //Click on a staff. Move cursor to that staff and nearest note/rest to click point
        lmShapeStaff* pSS = (lmShapeStaff*)pGMO;
        lmBox* pBox = m_pView->FindBoxAt(m_nNumPage, uPagePos);
        wxASSERT(pBox && pBox->IsBoxSliceInstr());
        lmBoxSliceInstr* pBSI = (lmBoxSliceInstr*)pBox;
	    lmVStaff* pVStaff = pBSI->GetInstrument()->GetVStaff();
	    int nMeasure = pBSI->GetNumMeasure();
	    int nStaff = pSS->GetNumStaff();
	    m_pView->MoveCaretNearTo(uPagePos, pVStaff, nStaff, nMeasure);
    }
    else
    {
        //if it is a staffobj move cursor to it. Else do nothing
        wxLogMessage(_T("[lmScoreCanvas::OnLeftClickOnObject] Click on shape"));
        lmScoreObj* pSCO = pGMO->GetScoreOwner();
        if (pSCO->GetScoreObjType() == lmSOT_ComponentObj)
            m_pView->MoveCursorToObject(pGMO);
    }

    m_pView->ShowCaret();
}

void lmScoreCanvas::OnRightClickOnObject(lmGMObject* pGMO, lmDPoint vCanvasPos,
                                         lmUPoint uPagePos, int nKeys)
{
    // mouse right click on object: show contextual menu for that object

    WXUNUSED(uPagePos);

	#ifdef __WXDEBUG__
	g_pLogger->LogTrace(_T("OnMouseEvent"), _T("OnRightClickOnObject()"));
	#endif

	m_pView->HideCaret();
    m_pView->DeselectAllGMObjects();
    SetFocus();

    if (pGMO->IsSelectable())
        m_pView->SelectGMObject(pGMO, true);     //true: redraw view content
    pGMO->OnRightClick(this, vCanvasPos, nKeys);
	m_pView->ShowCaret();
}

void lmScoreCanvas::OnLeftDoubleClickOnObject(lmGMObject* pGMO, lmDPoint vCanvasPos,
                                              lmUPoint uPagePos, int nKeys)
{
    // mouse left double click: Select/deselect the object pointed by mouse

    WXUNUSED(vCanvasPos);
    WXUNUSED(uPagePos);
    WXUNUSED(nKeys);

	#ifdef __WXDEBUG__
	g_pLogger->LogTrace(_T("OnMouseEvent"), _T("OnLeftDoubleClickOnObject()"));
	#endif

	m_pView->HideCaret();
    SetFocus();

    //ComponentObjs and other score objects (lmBoxXXXX) has all its measurements
    //relative to each page start position

    //select/deselect the object
    if (pGMO->IsSelectable())
        m_pView->SelectGMObject(pGMO, true);     //true: redraw view content

	m_pView->ShowCaret();
}

void lmScoreCanvas::OnRightDoubleClickOnObject(lmGMObject* pGMO, lmDPoint vCanvasPos,
                                               lmUPoint uPagePos, int nKeys)
{
    // mouse right double click: To be defined

    WXUNUSED(vCanvasPos);
    WXUNUSED(uPagePos);
    WXUNUSED(nKeys);

	#ifdef __WXDEBUG__
	g_pLogger->LogTrace(_T("OnMouseEvent"), _T("OnRightDoubleClickOnObject()"));
	#endif

	m_pView->HideCaret();
    m_pView->DeselectAllGMObjects(true);
    SetFocus();
	m_pView->ShowCaret();
}


//non-dragging events: click on canvas
void lmScoreCanvas::OnRightClickOnCanvas(lmDPoint vCanvasPos, lmUPoint uPagePos,
                                         int nKeys)
{
    WXUNUSED(uPagePos);
    WXUNUSED(nKeys);

    m_pView->DeselectAllGMObjects(true);     //true: redraw view content

    lmScore* pScore = m_pDoc->GetScore();
    pScore->PopupMenu(this, (lmGMObject*)NULL, vCanvasPos);
}

void lmScoreCanvas::OnLeftClickOnCanvas(lmDPoint vCanvasPos, lmUPoint uPagePos,
                                        int nKeys)
{
    WXUNUSED(vCanvasPos);
    WXUNUSED(uPagePos);
    WXUNUSED(nKeys);

    m_pView->DeselectAllGMObjects(true);     //true: redraw view content
    SetFocus();
}

void lmScoreCanvas::OnViewUpdated()
{
    //The view informs that it has updated the display

    //clear mouse information
    m_nDragState = lmDRAG_NONE;
	m_vEndDrag = lmDPoint(0, 0);
	m_vStartDrag.x = 0;
	m_vStartDrag.y = 0;
}

void lmScoreCanvas::OnNewGraphicalModel()
{
    //Called by the view when the graphical model has been recreated.
    //This implies that any saved pointer to a lmObject is no longer valid.
    //This method should deal with these pointer.

	m_pDraggedGMO = (lmGMObject*)NULL;	    //object being dragged
	m_pMouseOverGMO = (lmGMObject*)NULL;	//object on which mouse was flying over
}


//---------------------------------------------------------------------------
// Implementation of class lmEditorMode: Helper class to define editor modes
//---------------------------------------------------------------------------


lmEditorMode::lmEditorMode(wxClassInfo* pControllerInfo, wxClassInfo* pScoreProcInfo)
    : m_pControllerInfo(pControllerInfo)
    , m_pScoreProcInfo(pScoreProcInfo)
    , m_sCreationModeName(wxEmptyString)
    , m_sCreationModeVers(wxEmptyString)
{
    for (int i=0; i < lmPAGE_MAX; ++i)
        m_ToolPagesInfo[i] = (wxClassInfo*)NULL;
}

lmEditorMode::lmEditorMode(wxString& sCreationMode, wxString& sCreationVers)
    : m_pControllerInfo(CLASSINFO(lmScoreCanvas))
    , m_pScoreProcInfo(CLASSINFO(lmHarmonyProcessor))
    , m_sCreationModeName(sCreationMode)
    , m_sCreationModeVers(sCreationVers)
{
    //TODO
    for (int i=0; i < lmPAGE_MAX; ++i)
        m_ToolPagesInfo[i] = (wxClassInfo*)NULL;

    m_ToolPagesInfo[lmPAGE_NOTES] = CLASSINFO(lmToolPageNotesHarmony);
}

lmEditorMode::~lmEditorMode()
{
}

void lmEditorMode::ChangeToolPage(int nPageID, wxClassInfo* pToolPageInfo)
{
	wxASSERT(nPageID > lmPAGE_NONE && nPageID < lmPAGE_MAX);
    m_ToolPagesInfo[nPageID] = pToolPageInfo;
}

void lmEditorMode::CustomizeToolBoxPages(lmToolBox* pToolBox)
{
    //create the configuration
    for (int i=0; i < lmPAGE_MAX; ++i)
    {
        if (m_ToolPagesInfo[i])
        {
            lmToolPage* pPage = (lmToolPage*)m_ToolPagesInfo[i]->CreateObject();
            pPage->Create(pToolBox);
            pPage->CreateGroups();
            pToolBox->AddPage(pPage, i);
            pToolBox->SetAsActive(pPage, i);
            pToolBox->SelectToolPage( pToolBox->GetSelectedToolPage() );
        }
    }
}

lmScoreProcessor* lmEditorMode::CreateScoreProcessor()
{
    if (m_pScoreProcInfo)
    {
        //create the score processor
        lmScoreProcessor* pScoreProc =
            (lmScoreProcessor*)m_pScoreProcInfo->CreateObject();
        pScoreProc->SetTools();
        return pScoreProc;
    }
    return (lmScoreProcessor*)NULL;
}



