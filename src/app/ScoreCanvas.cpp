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
#ifdef __GNUG__
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

#include "global.h"

// access to global external variables (to disable mouse interaction with the score)
extern bool g_fReleaseVersion;            // in TheApp.cpp
extern bool g_fReleaseBehaviour;        // in TheApp.cpp


//-------------------------------------------------------------------------------------
// implementation of lmController
//-------------------------------------------------------------------------------------


BEGIN_EVENT_TABLE(lmController, wxEvtHandler)
	EVT_CHAR(lmController::OnKeyPress) 
    EVT_ERASE_BACKGROUND(lmController::OnEraseBackground)
END_EVENT_TABLE()

IMPLEMENT_ABSTRACT_CLASS(lmController, wxWindow)


lmController::lmController(wxWindow *pParent, lmScoreView *pView, lmScoreDocument* pDoc,
				 wxColor colorBg, wxWindowID id, const wxPoint& pos,
				 const wxSize& size, long style)
        : wxWindow(pParent, -1, pos, size, style)
{
}

void lmController::OnEraseBackground(wxEraseEvent& event)
{
	// When wxWidgets wants to update the display it emits two events: an erase 
	// background event and a paint event.
	// We are going to intercept the Erase Background event in order to prevent
	// that the default implementation in wxWindow erases the background, as this
	// will cause flickering
}


//-------------------------------------------------------------------------------------
// implementation of lmScoreCanvas
//-------------------------------------------------------------------------------------

IMPLEMENT_CLASS(lmScoreCanvas, lmController)


BEGIN_EVENT_TABLE(lmScoreCanvas, lmController)
    EVT_ERASE_BACKGROUND(lmScoreCanvas::OnEraseBackground)
    EVT_MOUSE_EVENTS(lmScoreCanvas::OnMouseEvent)
    EVT_PAINT(lmScoreCanvas::OnPaint)
    LM_EVT_SCORE_HIGHLIGHT(lmScoreCanvas::OnVisualHighlight)
END_EVENT_TABLE()

// Define a constructor for my canvas
lmScoreCanvas::lmScoreCanvas(lmScoreView *pView, wxWindow *pParent, lmScoreDocument* pDoc,
        const wxPoint& pos, const wxSize& size, long style, wxColor colorBg)
        : lmController(pParent, pView, pDoc, colorBg, wxID_ANY, pos, size, style)
{
    m_pView = pView;
    m_pOwner = pParent;
    m_pDoc = pDoc;
    m_colorBg = colorBg;

}

lmScoreCanvas::~lmScoreCanvas()
{
}


// Repainting behaviour
//
// We are going to use the technique of the virtual window. A bitmap, to act as the virtual
// window is mateined in the lmScoreView. All score display output is drawn onto this bitmap.
// Thus, to respond to a paint event we only have to copy the bitmap to the physical window.
// For optimization, only the damaged window rectangles will be repainted.
//
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
    while (upd) {
        wxRect rect = upd.GetRect();
        m_pView->RepaintScoreRectangle(&dc, rect);
        upd++;
    }

}

void lmScoreCanvas::OnMouseEvent(wxMouseEvent& event)
{
    if (!m_pView) return;
    wxClientDC dc(this);

    //Disable interaction with the score. Only mouse wheel allowed
    //Only for release version
    if (g_fReleaseVersion || g_fReleaseBehaviour) {
        if (event.GetEventType() == wxEVT_MOUSEWHEEL) {
            m_pView->OnMouseEvent(event, &dc);
        }
    }
    else
        m_pView->OnMouseEvent(event, &dc);

}


void lmScoreCanvas::PlayScore()
{
    //get the score
    lmScore* pScore = m_pDoc->GetScore();

    //play the score. Use current metronome setting
    pScore->Play(lmVISUAL_TRACKING, NO_MARCAR_COMPAS_PREVIO, ePM_NormalInstrument,
                 0, this);

}

void lmScoreCanvas::StopPlaying(bool fWait)
{
    //get the score
    lmScore* pScore = m_pDoc->GetScore();

    //request it to stop playing
    pScore->Stop();
    if (fWait) pScore->WaitForTermination();

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

void lmScoreCanvas::MoveObject(lmScoreObj* pSO, const lmUPoint& uPos)
{
	//Generate move command to move the lmStaffObj and update the document
	wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
    pCP->Submit(new lmScoreCommandMove(_T("Move object"), m_pDoc, pSO, uPos));

}

void lmScoreCanvas::SelectObject(lmScoreObj* pSO)
{
	//select/deselect an ScoreObj
    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	pCP->Submit(new lmScoreCommand(_T("Select object"),
								   lmScoreCommand::lmCMD_SelectObject, m_pDoc, pSO));
}

void lmScoreCanvas::OnKeyPress(wxKeyEvent& event)
{
    wxLogMessage(_T("lmScoreCanvas::OnKeyPress"));
    int nKeyCode = event.GetKeyCode();
    switch (nKeyCode)
    {
        case WXK_UP:
	        m_pView->CursorUp();
            break;

        case WXK_DOWN:
	        m_pView->CursorDown();
            break;

        case WXK_LEFT:
	        m_pView->CursorLeft();
            break;

        case WXK_RIGHT:
	        m_pView->CursorRight();
            break;

        default:
            event.Skip();
    }
}



/*
void wxShapeCanvas::OnMouseEvent(wxMouseEvent& event)
{
  wxClientDC dc(this);

  wxPoint logPos(event.GetLogicalPosition(dc));

  double x, y;
  x = (double) logPos.x;
  y = (double) logPos.y;

  int keys = 0;
  if (event.ShiftDown())
    keys = keys | KEY_SHIFT;
  if (event.ControlDown())
    keys = keys | KEY_CTRL;

  bool dragging = event.Dragging();

  // Check if we're within the tolerance for mouse movements.
  // If we're very close to the position we started dragging
  // from, this may not be an intentional drag at all.
  if (dragging)
  {
    int dx = abs(dc.LogicalToDeviceX((long) (x - m_firstDragX)));
    int dy = abs(dc.LogicalToDeviceY((long) (y - m_firstDragY)));
    if (m_checkTolerance && (dx <= GetDiagram()->GetMouseTolerance()) && (dy <= GetDiagram()->GetMouseTolerance()))
    {
      return;
    }
    else
      // If we've ignored the tolerance once, then ALWAYS ignore
      // tolerance in this drag, even if we come back within
      // the tolerance range.
      m_checkTolerance = false;
  }

  // Dragging - note that the effect of dragging is left entirely up
  // to the object, so no movement is done unless explicitly done by
  // object.
  if (dragging && m_draggedShape && m_dragState == StartDraggingLeft)
  {
    m_dragState = ContinueDraggingLeft;

    // If the object isn't m_draggable, transfer message to canvas
    if (m_draggedShape->Draggable())
      m_draggedShape->GetEventHandler()->OnBeginDragLeft((double)x, (double)y, keys, m_draggedAttachment);
    else
    {
      m_draggedShape = NULL;
      OnBeginDragLeft((double)x, (double)y, keys);
    }

    m_oldDragX = x; m_oldDragY = y;
  }
  else if (dragging && m_draggedShape && m_dragState == ContinueDraggingLeft)
  {
    // Continue dragging
    m_draggedShape->GetEventHandler()->OnDragLeft(false, m_oldDragX, m_oldDragY, keys, m_draggedAttachment);
    m_draggedShape->GetEventHandler()->OnDragLeft(true, (double)x, (double)y, keys, m_draggedAttachment);
    m_oldDragX = x; m_oldDragY = y;
  }
  else if (event.LeftUp() && m_draggedShape && m_dragState == ContinueDraggingLeft)
  {
    m_dragState = NoDragging;
    m_checkTolerance = true;

    m_draggedShape->GetEventHandler()->OnDragLeft(false, m_oldDragX, m_oldDragY, keys, m_draggedAttachment);

    m_draggedShape->GetEventHandler()->OnEndDragLeft((double)x, (double)y, keys, m_draggedAttachment);
    m_draggedShape = NULL;
  }
  else if (dragging && m_draggedShape && m_dragState == StartDraggingRight)
  {
    m_dragState = ContinueDraggingRight;

    if (m_draggedShape->Draggable())
      m_draggedShape->GetEventHandler()->OnBeginDragRight((double)x, (double)y, keys, m_draggedAttachment);
    else
    {
      m_draggedShape = NULL;
      OnBeginDragRight((double)x, (double)y, keys);
    }
    m_oldDragX = x; m_oldDragY = y;
  }
  else if (dragging && m_draggedShape && m_dragState == ContinueDraggingRight)
  {
    // Continue dragging
    m_draggedShape->GetEventHandler()->OnDragRight(false, m_oldDragX, m_oldDragY, keys, m_draggedAttachment);
    m_draggedShape->GetEventHandler()->OnDragRight(true, (double)x, (double)y, keys, m_draggedAttachment);
    m_oldDragX = x; m_oldDragY = y;
  }
  else if (event.RightUp() && m_draggedShape && m_dragState == ContinueDraggingRight)
  {
    m_dragState = NoDragging;
    m_checkTolerance = true;

    m_draggedShape->GetEventHandler()->OnDragRight(false, m_oldDragX, m_oldDragY, keys, m_draggedAttachment);

    m_draggedShape->GetEventHandler()->OnEndDragRight((double)x, (double)y, keys, m_draggedAttachment);
    m_draggedShape = NULL;
  }

  // All following events sent to canvas, not object
  else if (dragging && !m_draggedShape && m_dragState == StartDraggingLeft)
  {
    m_dragState = ContinueDraggingLeft;
    OnBeginDragLeft((double)x, (double)y, keys);
    m_oldDragX = x; m_oldDragY = y;
  }
  else if (dragging && !m_draggedShape && m_dragState == ContinueDraggingLeft)
  {
    // Continue dragging
    OnDragLeft(false, m_oldDragX, m_oldDragY, keys);
    OnDragLeft(true, (double)x, (double)y, keys);
    m_oldDragX = x; m_oldDragY = y;
  }
  else if (event.LeftUp() && !m_draggedShape && m_dragState == ContinueDraggingLeft)
  {
    m_dragState = NoDragging;
    m_checkTolerance = true;

    OnDragLeft(false, m_oldDragX, m_oldDragY, keys);
    OnEndDragLeft((double)x, (double)y, keys);
    m_draggedShape = NULL;
  }
  else if (dragging && !m_draggedShape && m_dragState == StartDraggingRight)
  {
    m_dragState = ContinueDraggingRight;
    OnBeginDragRight((double)x, (double)y, keys);
    m_oldDragX = x; m_oldDragY = y;
  }
  else if (dragging && !m_draggedShape && m_dragState == ContinueDraggingRight)
  {
    // Continue dragging
    OnDragRight(false, m_oldDragX, m_oldDragY, keys);
    OnDragRight(true, (double)x, (double)y, keys);
    m_oldDragX = x; m_oldDragY = y;
  }
  else if (event.RightUp() && !m_draggedShape && m_dragState == ContinueDraggingRight)
  {
    m_dragState = NoDragging;
    m_checkTolerance = true;

    OnDragRight(false, m_oldDragX, m_oldDragY, keys);
    OnEndDragRight((double)x, (double)y, keys);
    m_draggedShape = NULL;
  }

  // Non-dragging events
  else if (event.IsButton())
  {
    m_checkTolerance = true;

    // Find the nearest object
    int attachment = 0;
    wxShape *nearest_object = FindShape(x, y, &attachment);
    if (nearest_object) // Object event
    {
      if (event.LeftDown())
      {
        m_draggedShape = nearest_object;
        m_draggedAttachment = attachment;
        m_dragState = StartDraggingLeft;
        m_firstDragX = x;
        m_firstDragY = y;
      }
      else if (event.LeftUp())
      {
        // N.B. Only register a click if the same object was
        // identified for down *and* up.
        if (nearest_object == m_draggedShape)
          nearest_object->GetEventHandler()->OnLeftClick((double)x, (double)y, keys, attachment);

        m_draggedShape = NULL;
        m_dragState = NoDragging;
      }
      else if (event.LeftDClick())
      {
        nearest_object->GetEventHandler()->OnLeftDoubleClick((double)x, (double)y, keys, attachment);

        m_draggedShape = NULL;
        m_dragState = NoDragging;
      }
      else if (event.RightDown())
      {
        m_draggedShape = nearest_object;
        m_draggedAttachment = attachment;
        m_dragState = StartDraggingRight;
        m_firstDragX = x;
        m_firstDragY = y;
      }
      else if (event.RightUp())
      {
        if (nearest_object == m_draggedShape)
          nearest_object->GetEventHandler()->OnRightClick((double)x, (double)y, keys, attachment);

        m_draggedShape = NULL;
        m_dragState = NoDragging;
      }
    }
    else // Canvas event (no nearest object)
    {
      if (event.LeftDown())
      {
        m_draggedShape = NULL;
        m_dragState = StartDraggingLeft;
        m_firstDragX = x;
        m_firstDragY = y;
      }
      else if (event.LeftUp())
      {
        OnLeftClick((double)x, (double)y, keys);

        m_draggedShape = NULL;
        m_dragState = NoDragging;
      }
      else if (event.RightDown())
      {
        m_draggedShape = NULL;
        m_dragState = StartDraggingRight;
        m_firstDragX = x;
        m_firstDragY = y;
      }
      else if (event.RightUp())
      {
        OnRightClick((double)x, (double)y, keys);

        m_draggedShape = NULL;
        m_dragState = NoDragging;
      }
    }
  }
}

void csCanvas::OnLeftClick(double x, double y, int WXUNUSED(keys))
{
    csEditorToolPalette *palette = wxGetApp().GetDiagramPalette();

    if (palette->GetSelection() == PALETTE_ARROW)
    {
        GetView()->SelectAll(false);

        wxClientDC dc(this);
        PrepareDC(dc);

        Redraw(dc);
        return;
    }

    if (palette->GetSelection() == PALETTE_TEXT_TOOL)
    {
        wxString newLabel;

#if wxUSE_WX_RESOURCES
        // Ask for a label and create a new free-floating text region
        csLabelEditingDialog* dialog = new csLabelEditingDialog(GetParent());

        dialog->SetShapeLabel( wxEmptyString );
        dialog->SetTitle(_T("New text box"));
        if (dialog->ShowModal() == wxID_CANCEL)
        {
            dialog->Destroy();
            return;
        }

        newLabel = dialog->GetShapeLabel();
        dialog->Destroy();
#endif // wxUSE_WX_RESOURCES

        wxShape* shape = new csTextBoxShape;
        shape->AssignNewIds();
        shape->SetEventHandler(new csEvtHandler(shape, shape, newLabel));

        wxComboBox* comboBox = wxGetApp().GetPointSizeComboBox();
        wxString str(comboBox->GetValue());
        long pointSize;
        str.ToLong( &pointSize );

        wxFont* newFont = wxTheFontList->FindOrCreateFont(pointSize,
                shape->GetFont()->GetFamily(),
                shape->GetFont()->GetStyle(),
                shape->GetFont()->GetWeight(),
                shape->GetFont()->GetUnderlined(),
                shape->GetFont()->GetFaceName());

        shape->SetFont(newFont);

        shape->SetX(x);
        shape->SetY(y);

        csDiagramCommand* cmd = new csDiagramCommand(_T("Text box"),
            (csDiagramDocument *)GetView()->GetDocument(),
            new csCommandState(ID_CS_ADD_SHAPE, shape, NULL));
        GetView()->GetDocument()->GetCommandProcessor()->Submit(cmd);

        palette->SetSelection(PALETTE_ARROW);

        return;
    }

    csSymbol* symbol = wxGetApp().GetSymbolDatabase()->FindSymbol(palette->GetSelection());
    if (symbol)
    {
        wxShape* theShape = symbol->GetShape()->CreateNewCopy();

        wxComboBox* comboBox = wxGetApp().GetPointSizeComboBox();
        wxString str(comboBox->GetValue());
        long pointSize;
        str.ToLong( &pointSize );

        wxFont* newFont = wxTheFontList->FindOrCreateFont(pointSize,
                symbol->GetShape()->GetFont()->GetFamily(),
                symbol->GetShape()->GetFont()->GetStyle(),
                symbol->GetShape()->GetFont()->GetWeight(),
                symbol->GetShape()->GetFont()->GetUnderlined(),
                symbol->GetShape()->GetFont()->GetFaceName());

        theShape->SetFont(newFont);

        theShape->AssignNewIds();
        theShape->SetX(x);
        theShape->SetY(y);

        csDiagramCommand* cmd = new csDiagramCommand(symbol->GetName(),
            (csDiagramDocument *)GetView()->GetDocument(),
            new csCommandState(ID_CS_ADD_SHAPE, theShape, NULL));
        GetView()->GetDocument()->GetCommandProcessor()->Submit(cmd);

        palette->SetSelection(PALETTE_ARROW);
    }
}
*/

void lmScoreCanvas::OnEraseBackground(wxEraseEvent& event)
{
	// AWARE: This method is empty on purpose

	// When wxWidgets wants to update the display it emits two events: an erase 
	// background event and a paint event.
	// To prevent flickering we are not going to erase the background and the view
	// will paint it when needed, but only on the background areas not on all
	// canvas areas
}

