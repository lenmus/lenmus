//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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
#include "ArtProvider.h"        // to use ArtProvider for managing icons
#include "toolbox/ToolsBox.h"
#include "toolbox/ToolNotes.h"
#include "global.h"

#include "../ldp_parser/LDPParser.h"
#include "../ldp_parser/AuxString.h"

// access to global external variables (to disable mouse interaction with the score)
extern bool g_fReleaseVersion;            // in TheApp.cpp
extern bool g_fReleaseBehaviour;        // in TheApp.cpp


//-------------------------------------------------------------------------------------
// implementation of lmController
//-------------------------------------------------------------------------------------

//IDs for contextual menus
const int lmID_CUT = wxNewId();
const int lmID_COPY = wxNewId();
const int lmID_PASTE = wxNewId();
const int lmID_COLOR = wxNewId();
const int lmID_PROPERTIES = wxNewId();


BEGIN_EVENT_TABLE(lmController, wxEvtHandler)
	//EVT_CHAR(lmController::OnKeyPress)
	EVT_KEY_DOWN(lmController::OnKeyDown)
	EVT_KEY_UP(lmController::OnKeyUp)
    EVT_ERASE_BACKGROUND(lmController::OnEraseBackground)

	//contextual menus
	EVT_MENU	(lmID_CUT, lmController::OnCut)
    EVT_MENU	(lmID_COPY, lmController::OnCopy)
    EVT_MENU	(lmID_PASTE, lmController::OnPaste)
    EVT_MENU	(lmID_COLOR, lmController::OnColor)
    EVT_MENU	(lmID_PROPERTIES, lmController::OnProperties)

END_EVENT_TABLE()

IMPLEMENT_ABSTRACT_CLASS(lmController, wxWindow)


lmController::lmController(wxWindow *pParent, lmScoreView *pView, lmScoreDocument* pDoc,
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

wxMenu* lmController::GetContextualMenu()
{
	return (wxMenu*)NULL;
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

	//attach the edit menu to the command processor
	m_pDoc->GetCommandProcessor()->SetEditMenu( GetMainFrame()->GetEditMenu() );

    //initializations
    m_nOctave = 4;      //start in octave 4
    m_fCtrl = false;
    m_fAlt = false;
    m_fShift = false;
}

lmScoreCanvas::~lmScoreCanvas()
{
}

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

void lmScoreCanvas::MoveObject(lmGMObject* pGMO, const lmUPoint& uPos)
{
	//Generate move command to move the lmComponentObj and update the document

	wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = wxString::Format(_T("Move %s"), pGMO->GetName().c_str() );
	pCP->Submit(new lmCmdUserMoveScoreObj(sName, m_pDoc, pGMO, uPos));
}

void lmScoreCanvas::SelectObject(lmGMObject* pGMO)
{
	//select/deselect a ComponentObj

    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = wxString::Format(_T("Select %s"), pGMO->GetName().c_str() );
	pCP->Submit(new lmCmdSelectSingle(sName, m_pDoc, m_pView, pGMO));
}

void lmScoreCanvas::SelectObjects(bool fSelect, lmGMSelection* pSelection)
{
	//select a set of objects

    //TODO
    //wxLogMessage(pSelection->Dump());

    if (pSelection->NumObjects() < 1) return;

    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = wxString::Format(
        (fSelect ? _T("Select %d objects") : _T("Unselect %d objects")), pSelection->NumObjects() );
	pCP->Submit(new lmCmdSelectMultiple(sName, m_pDoc, m_pView, pSelection, fSelect));

}

void lmScoreCanvas::DeleteObject()
{
	//delete the StaffObj at current cursor position

    //get cursor
    lmVStaffCursor* pVCursor = m_pView->GetVCursor();
	wxASSERT(pVCursor);

	//get object pointed by the cursor
    lmStaffObj* pCursorSO = pVCursor->GetStaffObj();

    //if no object, ignore command. It is due, for example, to the user clicking 'Del' key
    //on no object
	if (!pCursorSO)
        return;

	//the EOS Barline can not be deleted
	if (pCursorSO->GetClass() == eSFOT_Barline 
		&& ((lmBarline*)pCursorSO)->GetBarlineType() == lm_eBarlineEOS) return;

    //prepare command and submit it
    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = wxString::Format(_T("Delete %s"), pCursorSO->GetName().c_str() );
	pCP->Submit(new lmCmdDeleteObject(pVCursor, sName, m_pDoc));
}


void lmScoreCanvas::InsertClef(lmEClefType nClefType)
{
	//insert a Clef at current cursor position

    //get cursor
    lmVStaffCursor* pVCursor = m_pView->GetVCursor();
	wxASSERT(pVCursor);

    //prepare command and submit it
    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = _T("Insert clef");
	pCP->Submit(new lmCmdInsertClef(pVCursor, sName, m_pDoc, nClefType) );
}

void lmScoreCanvas::InsertTimeSignature(int nBeats, int nBeatType, bool fVisible)
{
    //insert a time signature at current cursor position

    //wxLogMessage(_T("[lmScoreCanvas::InsertTimeSignature] nBeats=%d, nBeatType=%d"), nBeats, nBeatType);

    //get cursor
    lmVStaffCursor* pVCursor = m_pView->GetVCursor();
	wxASSERT(pVCursor);

    //prepare command and submit it
    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = _T("Insert time signature");
	pCP->Submit(new lmCmdInsertTimeSignature(pVCursor, sName, m_pDoc, nBeats,
                                             nBeatType, fVisible) );
}

void lmScoreCanvas::InsertKeySignature(int nFifths, bool fMajor, bool fVisible)
{
    //insert a key signature at current cursor position

    wxLogMessage(_T("[lmScoreCanvas::InsertKeySignature] fifths=%d, %s"),
                 nFifths, (fMajor ? _T("major") : _T("minor")) );

    //get cursor
    lmVStaffCursor* pVCursor = m_pView->GetVCursor();
	wxASSERT(pVCursor);

    //prepare command and submit it
    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = _T("Insert key signature");
	pCP->Submit(new lmCmdInsertKeySignature(pVCursor, sName, m_pDoc, nFifths, fMajor, fVisible) );
}

void lmScoreCanvas::InsertBarline(lmEBarline nType)
{
	//insert a barline at current cursor position

    //get cursor
    lmVStaffCursor* pVCursor = m_pView->GetVCursor();
	wxASSERT(pVCursor);

    //prepare command and submit it
    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = _T("Insert barline");
	pCP->Submit(new lmCmdInsertBarline(pVCursor, sName, m_pDoc, nType) );
}

void lmScoreCanvas::InsertNote(lmEPitchType nPitchType, int nStep, int nOctave, 
							   lmENoteType nNoteType, float rDuration, int nDots,
							   lmENoteHeads nNotehead, lmEAccidentals nAcc)
{
	//insert a note at current cursor position

    //get cursor
    lmVStaffCursor* pVCursor = m_pView->GetVCursor();
	wxASSERT(pVCursor);

    //prepare command and submit it
    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = _T("Insert note");
    wxString sOctave = wxString::Format(_T("%d"), nOctave);

    //TODO:
    wxString sAllSteps = _T("cdefgab");
    wxString sStep = sAllSteps.GetChar( nStep );

	pCP->Submit(new lmCmdInsertNote(pVCursor, sName, m_pDoc, nPitchType, nStep, nOctave, 
							        nNoteType, rDuration, nDots, nNotehead, nAcc) );
}

void lmScoreCanvas::ChangeNotePitch(int nSteps)
{
	//change pith of note at current cursor position
    lmVStaffCursor* pVCursor = m_pView->GetVCursor();
	wxASSERT(pVCursor);
    lmStaffObj* pCursorSO = pVCursor->GetStaffObj();
	wxASSERT(pCursorSO);
	wxASSERT(pCursorSO->GetClass() == eSFOT_NoteRest && ((lmNoteRest*)pCursorSO)->IsNote() );
    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = _T("Change note pitch");
	pCP->Submit(new lmCmdChangeNotePitch(sName, m_pDoc, (lmNote*)pCursorSO, nSteps) );
}

void lmScoreCanvas::ChangeNoteAccidentals(int nSteps)
{
	//change note accidentals for note at current cursor position
    lmVStaffCursor* pVCursor = m_pView->GetVCursor();
	wxASSERT(pVCursor);
    lmStaffObj* pCursorSO = pVCursor->GetStaffObj();
	wxASSERT(pCursorSO);
	wxASSERT(pCursorSO->GetClass() == eSFOT_NoteRest && ((lmNoteRest*)pCursorSO)->IsNote() );
    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = _T("Change note accidentals");
	pCP->Submit(new lmCmdChangeNoteAccidentals(sName, m_pDoc, (lmNote*)pCursorSO, nSteps) );
}

void lmScoreCanvas::OnKeyDown(wxKeyEvent& event)
{
    switch ( event.GetKeyCode() )
    {
        case WXK_SHIFT:     m_fShift = true;    break;
        case WXK_ALT:       m_fAlt = true;      break;
        case WXK_CONTROL:   m_fCtrl = true;     break;
        default:
            OnKeyPress(event);
            ;   //LogKeyEvent( wxT("Key down"), event);
    }

    //event.Skip();       //to generate Key press event
}

void lmScoreCanvas::OnKeyUp(wxKeyEvent& event)
{
    switch ( event.GetKeyCode() )
    {
        case WXK_SHIFT:     m_fShift = false;   break;
        case WXK_ALT:       m_fAlt = false;     break;
        case WXK_CONTROL:   m_fCtrl = false;    break;
        default:
            ; //LogKeyEvent( wxT("Key up"), event);
    }

    //event.Skip();       //to generate Key press event
}

void lmScoreCanvas::OnKeyPress(wxKeyEvent& event)
{
    //We are processing a Key Down event 
	lmEEditTool nTool = lmTOOL_NONE;
	lmToolBox* pToolBox = GetMainFrame()->GetActiveToolBox();
	if (!pToolBox) {
		wxLogMessage(_T("[lmScoreCanvas::OnKeyPress] No ToolBox!"));
	}
	else
		nTool = pToolBox->GetSelectedTool();

    int nKeyCode = event.GetKeyCode();
	bool fUnknown = false;

    // check if an auxiliary key (Shift, Ctrl/Meta, Alt) is pressed
    enum {
        lmKEY_ALT = 0x0001,
        lmKEY_CTRL = 0x0002,
        lmKEY_SHIFT = 0x0004,
    };

    int nAuxKeys = 0;
    if (event.ShiftDown())
        nAuxKeys |= lmKEY_SHIFT;
    if (event.CmdDown())        //Ctrl for non-Mac platforms, Meta for Mac platform
        nAuxKeys |= lmKEY_CTRL;
    if (event.AltDown())
        nAuxKeys |= lmKEY_ALT;

	//Verify common keys working with all tools
	fUnknown = false;
	switch (nKeyCode)
	{
		case WXK_LEFT:
			m_pView->CaretLeft((bool)(nAuxKeys & lmKEY_ALT));
			break;

		case WXK_RIGHT:
			m_pView->CaretRight((bool)(nAuxKeys & lmKEY_ALT));
			break;

		case WXK_UP:
			m_pView->CaretUp();
			break;

		case WXK_DOWN:
			m_pView->CaretDown();
			break;

		case WXK_F1:
			if (pToolBox) pToolBox->SelectTool((lmEEditTool)0);
			break;

		case WXK_F2:
			if (pToolBox) pToolBox->SelectTool((lmEEditTool)1);
			break;

		case WXK_F3:
			if (pToolBox) pToolBox->SelectTool((lmEEditTool)2);
			break;

		case WXK_F4:
			if (pToolBox) pToolBox->SelectTool((lmEEditTool)3);
			break;

		case WXK_F5:
			if (pToolBox) pToolBox->SelectTool((lmEEditTool)4);
			break;

		case WXK_F6:
			if (pToolBox) pToolBox->SelectTool((lmEEditTool)5);
			break;

        case WXK_DELETE:
			DeleteObject();
			break;

		default:
			fUnknown = true;
	}

	//if not processed, check if specific for current selected tool panel
	if (fUnknown) 
	{
	    switch(nTool)
	    {
            case lmTOOL_NONE:	//---------------------------------------------------------
		    {
			    switch (nKeyCode)
			    {
				    case WXK_UP:
					    m_pView->CaretUp();
					    break;

				    case WXK_DOWN:
					    m_pView->CaretDown();
					    break;

				    default:
					    fUnknown = true;
			    }
			    break;
		    }

            case lmTOOL_NOTES:	//---------------------------------------------------------
		    {
			    lmToolNotes* pNoteOptions = pToolBox->GetNoteProperties();
			    lmENoteType nNoteType = pNoteOptions->GetNoteDuration();
			    int nDots = pNoteOptions->GetNoteDots();
			    float rDuration = lmLDPParser::GetDefaultDuration(nNoteType, nDots, 0, 0);
			    lmENoteHeads nNotehead = pNoteOptions->GetNoteheadType();
			    lmEAccidentals nAcc = pNoteOptions->GetNoteAccidentals();
                
                //insert note
                if (nKeyCode >= int('A') && nKeyCode <= int('G'))
                {
                    // determine octave
                    if (nAuxKeys & lmKEY_SHIFT)
                        ++m_nOctave;
                    else if (nAuxKeys & lmKEY_CTRL)
                        --m_nOctave;
                    
                    //limit octave 0..9
                    if (m_nOctave < 0)
                        m_nOctave = 0;
                    else if (m_nOctave > 9)
                        m_nOctave = 9;

                    //get step 
                    static wxString sSteps = _T("abcdefg");
                    int nStep = LetterToStep( sSteps.GetChar( nKeyCode - int('A') ));

                    //do insert note
					InsertNote(lm_ePitchRelative, nStep, m_nOctave, nNoteType, rDuration,
							   nDots, nNotehead, nAcc);

                }

                //commands to change options in Tool Box


                //select note duration: digits 0..9
                if (fUnknown && nKeyCode >= int('0') && nKeyCode <= int('9'))
			    {
					SelectNoteDuration(nKeyCode - int('0'));
                    fUnknown = false;
                }

                if (fUnknown)
                {
                    fUnknown = false;       //assume it
			        switch (nKeyCode)
			        {
                        //select accidentals
				        case int('+'):      // '+' increment accidentals  
                            SelectNoteAccidentals(true);
                            break;

                        case int('-'):      // '-' decrement accidentals
                            SelectNoteAccidentals(false);
                            break;

                        //select dots
				        case int('.'):      // '.' increment/decrement dots 
                            if (event.AltDown())
                                SelectNoteDots(false);      // Alt + '.' decrement dots     
                            else
                                SelectNoteDots(true);       // '.' increment dots 
                            break;

                        //unknown
				        default:
					        fUnknown = true;
                    }
                }


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

			    break;      //case lmTOOL_NOTES
		    }

            case lmTOOL_CLEFS:	//---------------------------------------------------------
		    {
			    switch (nKeyCode)
			    {
				    case int('G'):	// 'g' insert G clef
					    InsertClef(lmE_Sol);
					    break;

				    case int('F'):	// 'f' insert F4 clef
					    InsertClef(lmE_Fa4);
					    break;

				    case int('C'):    // 'c' insert C3 clef
					    InsertClef(lmE_Do3);
					    break;

				    default:
					    fUnknown = true;
			    }
			    break;
		    }

            case lmTOOL_BARLINES:	//---------------------------------------------------------
		    {
			    switch (nKeyCode)
			    {
				    case int('B'):	// 'b' insert duble barline
					    InsertBarline(lm_eBarlineDouble);
					    break;

				    default:
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

	//Debug: Unidentified tool or unidentified key. Log message
	//if (fUnknown) 
        LogKeyEvent(_T("Key Press"), event, nTool);

}

void lmScoreCanvas::LogKeyEvent(wxString name, wxKeyEvent& event, int nTool) const
{
    wxString key;
    long keycode = event.GetKeyCode();
    {
        switch ( keycode )
        {
            case WXK_BACK: key = _T("BACK"); break;
            case WXK_TAB: key = _T("TAB"); break;
            case WXK_RETURN: key = _T("RETURN"); break;
            case WXK_ESCAPE: key = _T("ESCAPE"); break;
            case WXK_SPACE: key = _T("SPACE"); break;
            case WXK_DELETE: key = _T("DELETE"); break;
            case WXK_START: key = _T("START"); break;
            case WXK_LBUTTON: key = _T("LBUTTON"); break;
            case WXK_RBUTTON: key = _T("RBUTTON"); break;
            case WXK_CANCEL: key = _T("CANCEL"); break;
            case WXK_MBUTTON: key = _T("MBUTTON"); break;
            case WXK_CLEAR: key = _T("CLEAR"); break;
            case WXK_SHIFT: key = _T("SHIFT"); break;
            case WXK_ALT: key = _T("ALT"); break;
            case WXK_CONTROL: key = _T("CONTROL"); break;
            case WXK_MENU: key = _T("MENU"); break;
            case WXK_PAUSE: key = _T("PAUSE"); break;
            case WXK_CAPITAL: key = _T("CAPITAL"); break;
            case WXK_END: key = _T("END"); break;
            case WXK_HOME: key = _T("HOME"); break;
            case WXK_LEFT: key = _T("LEFT"); break;
            case WXK_UP: key = _T("UP"); break;
            case WXK_RIGHT: key = _T("RIGHT"); break;
            case WXK_DOWN: key = _T("DOWN"); break;
            case WXK_SELECT: key = _T("SELECT"); break;
            case WXK_PRINT: key = _T("PRINT"); break;
            case WXK_EXECUTE: key = _T("EXECUTE"); break;
            case WXK_SNAPSHOT: key = _T("SNAPSHOT"); break;
            case WXK_INSERT: key = _T("INSERT"); break;
            case WXK_HELP: key = _T("HELP"); break;
            case WXK_NUMPAD0: key = _T("NUMPAD0"); break;
            case WXK_NUMPAD1: key = _T("NUMPAD1"); break;
            case WXK_NUMPAD2: key = _T("NUMPAD2"); break;
            case WXK_NUMPAD3: key = _T("NUMPAD3"); break;
            case WXK_NUMPAD4: key = _T("NUMPAD4"); break;
            case WXK_NUMPAD5: key = _T("NUMPAD5"); break;
            case WXK_NUMPAD6: key = _T("NUMPAD6"); break;
            case WXK_NUMPAD7: key = _T("NUMPAD7"); break;
            case WXK_NUMPAD8: key = _T("NUMPAD8"); break;
            case WXK_NUMPAD9: key = _T("NUMPAD9"); break;
            case WXK_MULTIPLY: key = _T("MULTIPLY"); break;
            case WXK_ADD: key = _T("ADD"); break;
            case WXK_SEPARATOR: key = _T("SEPARATOR"); break;
            case WXK_SUBTRACT: key = _T("SUBTRACT"); break;
            case WXK_DECIMAL: key = _T("DECIMAL"); break;
            case WXK_DIVIDE: key = _T("DIVIDE"); break;
            case WXK_F1: key = _T("F1"); break;
            case WXK_F2: key = _T("F2"); break;
            case WXK_F3: key = _T("F3"); break;
            case WXK_F4: key = _T("F4"); break;
            case WXK_F5: key = _T("F5"); break;
            case WXK_F6: key = _T("F6"); break;
            case WXK_F7: key = _T("F7"); break;
            case WXK_F8: key = _T("F8"); break;
            case WXK_F9: key = _T("F9"); break;
            case WXK_F10: key = _T("F10"); break;
            case WXK_F11: key = _T("F11"); break;
            case WXK_F12: key = _T("F12"); break;
            case WXK_F13: key = _T("F13"); break;
            case WXK_F14: key = _T("F14"); break;
            case WXK_F15: key = _T("F15"); break;
            case WXK_F16: key = _T("F16"); break;
            case WXK_F17: key = _T("F17"); break;
            case WXK_F18: key = _T("F18"); break;
            case WXK_F19: key = _T("F19"); break;
            case WXK_F20: key = _T("F20"); break;
            case WXK_F21: key = _T("F21"); break;
            case WXK_F22: key = _T("F22"); break;
            case WXK_F23: key = _T("F23"); break;
            case WXK_F24: key = _T("F24"); break;
            case WXK_NUMLOCK: key = _T("NUMLOCK"); break;
            case WXK_SCROLL: key = _T("SCROLL"); break;
            case WXK_PAGEUP: key = _T("PAGEUP"); break;
            case WXK_PAGEDOWN: key = _T("PAGEDOWN"); break;
            case WXK_NUMPAD_SPACE: key = _T("NUMPAD_SPACE"); break;
            case WXK_NUMPAD_TAB: key = _T("NUMPAD_TAB"); break;
            case WXK_NUMPAD_ENTER: key = _T("NUMPAD_ENTER"); break;
            case WXK_NUMPAD_F1: key = _T("NUMPAD_F1"); break;
            case WXK_NUMPAD_F2: key = _T("NUMPAD_F2"); break;
            case WXK_NUMPAD_F3: key = _T("NUMPAD_F3"); break;
            case WXK_NUMPAD_F4: key = _T("NUMPAD_F4"); break;
            case WXK_NUMPAD_HOME: key = _T("NUMPAD_HOME"); break;
            case WXK_NUMPAD_LEFT: key = _T("NUMPAD_LEFT"); break;
            case WXK_NUMPAD_UP: key = _T("NUMPAD_UP"); break;
            case WXK_NUMPAD_RIGHT: key = _T("NUMPAD_RIGHT"); break;
            case WXK_NUMPAD_DOWN: key = _T("NUMPAD_DOWN"); break;
            case WXK_NUMPAD_PAGEUP: key = _T("NUMPAD_PAGEUP"); break;
            case WXK_NUMPAD_PAGEDOWN: key = _T("NUMPAD_PAGEDOWN"); break;
            case WXK_NUMPAD_END: key = _T("NUMPAD_END"); break;
            case WXK_NUMPAD_BEGIN: key = _T("NUMPAD_BEGIN"); break;
            case WXK_NUMPAD_INSERT: key = _T("NUMPAD_INSERT"); break;
            case WXK_NUMPAD_DELETE: key = _T("NUMPAD_DELETE"); break;
            case WXK_NUMPAD_EQUAL: key = _T("NUMPAD_EQUAL"); break;
            case WXK_NUMPAD_MULTIPLY: key = _T("NUMPAD_MULTIPLY"); break;
            case WXK_NUMPAD_ADD: key = _T("NUMPAD_ADD"); break;
            case WXK_NUMPAD_SEPARATOR: key = _T("NUMPAD_SEPARATOR"); break;
            case WXK_NUMPAD_SUBTRACT: key = _T("NUMPAD_SUBTRACT"); break;
            case WXK_NUMPAD_DECIMAL: key = _T("NUMPAD_DECIMAL"); break;

            default:
            {
               if ( wxIsprint((int)keycode) )
                   key.Printf(_T("'%c'"), (char)keycode);
               else if ( keycode > 0 && keycode < 27 )
                   key.Printf(_("Ctrl-%c"), _T('A') + keycode - 1);
               else
                   key.Printf(_T("unknown (%ld)"), keycode);
            }
        }
    }

    key += wxString::Format(_T(" (Unicode: %#04x)"), event.GetUnicodeKey());

    wxLogMessage( wxString::Format( _T("[lmScoreCanvas::LogKeyEvent] Event: %s - %s, nKeyCode=%d, (flags = %c%c%c%c). Tool=%d"),
            name, key.c_str(), keycode, 
            (event.ControlDown() ? _T('C') : _T('-') ),
            (event.AltDown() ? _T('A') : _T('-') ),
            (event.ShiftDown() ? _T('S') : _T('-') ),
            (event.MetaDown() ? _T('M') : _T('-') ),
            nTool ));
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

wxMenu* lmScoreCanvas::GetContextualMenu()
{
	if (m_pMenu) delete m_pMenu;
	m_pMenu = new wxMenu();

#if defined(__WXMSW__) || defined(__WXGTK__)

    wxMenuItem* pItem;
    wxSize nIconSize(16, 16);

    pItem = new wxMenuItem(m_pMenu, lmID_CUT, _("&Cut"));
    pItem->SetBitmap( wxArtProvider::GetBitmap(_T("tool_cut"), wxART_TOOLBAR, nIconSize) );
    m_pMenu->Append(pItem);

    pItem = new wxMenuItem(m_pMenu, lmID_COPY, _("&Copy"));
    pItem->SetBitmap( wxArtProvider::GetBitmap(_T("tool_copy"), wxART_TOOLBAR, nIconSize) );
    m_pMenu->Append(pItem);

    pItem = new wxMenuItem(m_pMenu, lmID_PASTE, _("&Paste"));
    pItem->SetBitmap( wxArtProvider::GetBitmap(_T("tool_paste"), wxART_TOOLBAR, nIconSize) );
    m_pMenu->Append(pItem);

	m_pMenu->AppendSeparator();

    pItem = new wxMenuItem(m_pMenu, lmID_COLOR, _("Colour"));
    pItem->SetBitmap( wxArtProvider::GetBitmap(_T("opt_colors"), wxART_TOOLBAR, nIconSize) );
    m_pMenu->Append(pItem);

    pItem = new wxMenuItem(m_pMenu, lmID_PROPERTIES, _("Properties"));
    pItem->SetBitmap( wxArtProvider::GetBitmap(_T("opt_tools"), wxART_TOOLBAR, nIconSize) );
    m_pMenu->Append(pItem);

	//m_pMenu->AppendSeparator();


#else
	m_pMenu->Append(lmID_CUT, _("&Cut"));
	m_pMenu->Append(lmID_COPY, _("&Copy"));
	m_pMenu->Append(lmID_PASTE, _("&Paste"));
	m_pMenu->AppendSeparator();
	m_pMenu->Append(lmID_COLOR, _("Colour"));
	//m_pMenu->AppendSeparator();

#endif

	return m_pMenu;
}

void lmScoreCanvas::OnCut(wxCommandEvent& event)
{
	WXUNUSED(event);
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
	m_pMenuOwner->OnProperties(m_pMenuGMO);
}

void lmScoreCanvas::SelectNoteDuration(int iButton)
{
	lmToolBox* pToolBox = GetMainFrame()->GetActiveToolBox();
	if (pToolBox) 
		((lmToolNotes*)pToolBox->GetToolPanel(lmTOOL_NOTES))->SetNoteDuration(iButton);
}

void lmScoreCanvas::SelectNoteAccidentals(bool fNext)
{
	lmToolBox* pToolBox = GetMainFrame()->GetActiveToolBox();
	if (pToolBox) 
    {
        if (fNext)
            ((lmToolNotes*)pToolBox->GetToolPanel(lmTOOL_NOTES))->SelectNextAccidental();
        else
            ((lmToolNotes*)pToolBox->GetToolPanel(lmTOOL_NOTES))->SelectPrevAccidental();
    }
}

void lmScoreCanvas::SelectNoteDots(bool fNext)
{
	lmToolBox* pToolBox = GetMainFrame()->GetActiveToolBox();
	if (pToolBox) 
    {
        if (fNext)
            ((lmToolNotes*)pToolBox->GetToolPanel(lmTOOL_NOTES))->SelectNextDot();
        else
            ((lmToolNotes*)pToolBox->GetToolPanel(lmTOOL_NOTES))->SelectPrevDot();
    }
}
