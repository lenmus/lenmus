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
	EVT_CHAR(lmController::OnKeyPress)
	EVT_KEY_DOWN(lmController::OnKeyDown)
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
    m_sCmd = _T("");
    m_fCmd = false;
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

            event.Skip();       //to generate Key char event
    }
}

void lmScoreCanvas::OnKeyPress(wxKeyEvent& event)
{
    wxLogMessage(_T("[lmScoreCanvas::OnKeyPress] KeyCode=%s (%d), KeyDown data: Keycode=%s (%d), (flags = %c%c%c%c)"),
            KeyCodeToName(event.GetKeyCode()), event.GetKeyCode(),
            KeyCodeToName(m_nKeyDownCode), m_nKeyDownCode, 
            (m_fCmd ? _T('C') : _T('-') ),
            (m_fAlt ? _T('A') : _T('-') ),
            (m_fShift ? _T('S') : _T('-') ),
            (event.MetaDown() ? _T('M') : _T('-') )
            );
    ProcessKey(event);
}

void lmScoreCanvas::ProcessKey(wxKeyEvent& event)
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

    //fix ctrol+key codes
    if (nKeyCode > 0 && nKeyCode < 27)
        nKeyCode += int('A') - 1;

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
			if (pToolBox) pToolBox->SelectTool((lmEEditTool)0);
			break;

		case WXK_F3:
			if (pToolBox) pToolBox->SelectTool((lmEEditTool)1);
			break;

		case WXK_F4:
			if (pToolBox) pToolBox->SelectTool((lmEEditTool)2);
			break;

		case WXK_F5:
			if (pToolBox) pToolBox->SelectTool((lmEEditTool)3);
			break;

		case WXK_F6:
			if (pToolBox) pToolBox->SelectTool((lmEEditTool)4);
			break;

		case WXK_F7:
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

            case lmTOOL_NOTES:	//---------------------------------------------------------
		    {
			    lmToolNotes* pNoteOptions = pToolBox->GetNoteProperties();
			    lmENoteType nNoteType = pNoteOptions->GetNoteDuration();
			    int nDots = pNoteOptions->GetNoteDots();
			    float rDuration = lmLDPParser::GetDefaultDuration(nNoteType, nDots, 0, 0);
			    lmENoteHeads nNotehead = pNoteOptions->GetNoteheadType();
			    lmEAccidentals nAcc = pNoteOptions->GetNoteAccidentals();
                
                //analyse accidentals
                if (m_sCmd != _T(""))
                {
                    if (m_sCmd.StartsWith( _T("+") )) {
                        if (m_sCmd.StartsWith( _T("++") )) {
                            nAcc = lm_eSharpSharp;
                        } else {
                            nAcc = lm_eSharp;
                        }
                    } else if (m_sCmd.StartsWith( _T("-") )) {
                        if (m_sCmd.StartsWith( _T("--") )) {
                            nAcc = lm_eFlatFlat;
                        } else {
                            nAcc = lm_eFlat;
                        }
                    } else if (m_sCmd.StartsWith( _T("=+") )) {
                        nAcc = lm_eNaturalSharp;
                    } else if (m_sCmd.StartsWith( _T("=-") )) {
                        nAcc = lm_eNaturalFlat;
                    } else if (m_sCmd.StartsWith( _T("=") )) {
                        nAcc = eNatural;
                    } else if (m_sCmd.StartsWith( _T("x") )) {
                        nAcc = lm_eDoubleSharp;
                    } else {
                        ; //ignore m_sCmd
                    }
                }

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

                    //do insert note
					InsertNote(lm_ePitchRelative, nStep, m_nOctave, nNoteType, rDuration,
							   nDots, nNotehead, nAcc);

                    fUnknown = false;
                }

                //commands to change options in Tool Box


                //select note duration: digits 0..9
                if (fUnknown && nKeyCode >= int('0') && nKeyCode <= int('9'))
			    {
					SelectNoteDuration(nKeyCode - int('0'));
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

			    break;      //case lmTOOL_NOTES
		    }

            case lmTOOL_CLEFS:	//---------------------------------------------------------
		    {
                fUnknown = false;       //assume it will be processed
			    switch (nKeyCode)
			    {
				    case int('G'):	// 'g' insert G clef
				    case int('g'):
					    InsertClef(lmE_Sol);
					    break;

				    case int('F'):	// 'f' insert F4 clef
				    case int('f'):
					    InsertClef(lmE_Fa4);
					    break;

				    case int('C'):    // 'c' insert C3 clef
				    case int('c'):
					    InsertClef(lmE_Do3);
					    break;

				    default:
                        if (wxIsprint(nKeyCode))
                            m_sCmd += wxString::Format(_T("%c"), (char)nKeyCode);
					    fUnknown = true;
			    }
			    break;
		    }

            case lmTOOL_BARLINES:	//---------------------------------------------------------
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

	//Debug: Unidentified tool or unidentified key. Log message
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
}

void lmScoreCanvas::LogKeyEvent(wxString name, wxKeyEvent& event, int nTool)
{
    wxString key = KeyCodeToName( event.GetKeyCode() );
    key += wxString::Format(_T(" (Unicode: %#04x)"), event.GetUnicodeKey());

    wxLogMessage( wxString::Format( _T("[lmScoreCanvas::LogKeyEvent] Event: %s - %s, nKeyCode=%d, (flags = %c%c%c%c). Tool=%d"),
            name, key.c_str(), event.GetKeyCode(), 
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
                sKey.Printf(_("Ctrl-%c"), _T('A') + nKeyCode - 1);
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
