//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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
#include "ArtProvider.h"        // to use ArtProvider for managing icons
#include "toolbox/ToolsBox.h"
#include "toolbox/ToolNotes.h"
#include "toolbox/ToolBoxEvents.h"
#include "global.h"
#include "KbdCmdParser.h"
#include "../ldp_parser/LDPParser.h"
#include "../ldp_parser/AuxString.h"
#include "../graphic/GMObject.h"
#include "../score/VStaff.h"

// access to global external variables (to disable mouse interaction with the score)
extern bool g_fReleaseVersion;            // in TheApp.cpp
extern bool g_fReleaseBehaviour;        // in TheApp.cpp


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

wxMenu* lmController::GetContextualMenu(bool fInitialize)
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
    LM_EVT_TOOLBOX(lmScoreCanvas::OnToolBoxEvent)

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
    m_fToolBoxSavedOptions = false;
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

    if (!m_pView) return;

    wxClientDC dc(this);
    m_pView->OnMouseEvent(event, &dc);
}


void lmScoreCanvas::PlayScore(bool fFromCursor)
{
    //get the score
    lmScore* pScore = m_pDoc->GetScore();

	//determine measure from cursor or start of selection
	int nMeasure = 1;
	lmVStaffCursor* pVCursor = m_pView->GetVCursor();
	lmGMSelection* pSel = m_pView->GetSelection();
	bool fFromMeasure = fFromCursor || pSel->NumObjects() > 0;
	if (pSel->NumObjects() > 0)
	{
		nMeasure = ((lmNote*)pSel->GetFirst()->GetScoreOwner())->GetSegment()->GetNumSegment() + 1;
		m_pView->DeselectAllGMObjects(true);	//redraw, to remove selction highlight
	}
	else if (pVCursor)
		nMeasure = pVCursor->GetSegment() + 1;

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


void lmScoreCanvas::AttachNewText(lmComponentObj* pCO)
{
    //Create a new text and attach it to the received object

	wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = _("attach text");
	pCP->Submit(new lmCmdAttachNewText(sName, m_pDoc, pCO));
}

void lmScoreCanvas::AddTitle()
{
    //Create a new block of text and attach it to the score

	wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	pCP->Submit(new lmCmdAddNewTitle(m_pDoc));
}

void lmScoreCanvas::MoveObject(lmGMObject* pGMO, const lmUPoint& uPos)
{
	//Generate move command to move the lmComponentObj and update the document

	wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = wxString::Format(_("Move %s"), pGMO->GetName().c_str() );
	pCP->Submit(new lmCmdUserMoveScoreObj(sName, m_pDoc, pGMO, uPos));
}

void lmScoreCanvas::MoveNote(lmGMObject* pGMO, const lmUPoint& uPos, int nSteps)
{
	//Generate move command to move the note and change its pitch

	wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	pCP->Submit(new lmCmdMoveNote(m_pDoc, (lmNote*)pGMO->GetScoreOwner(), uPos, nSteps));
}

void lmScoreCanvas::ChangePageMargin(lmGMObject* pGMO, int nIdx, int nPage, lmLUnits uPos)
{
	//Updates the position of a margin

	wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	pCP->Submit(new lmCmdChangePageMargin(_("Change margin"), m_pDoc, pGMO, nIdx, nPage, uPos));
}

void lmScoreCanvas::DeleteCaretSatffobj()
{
	//delete the StaffObj at current caret position

    //get cursor
    lmVStaffCursor* pVCursor = m_pView->GetVCursor();
	wxASSERT(pVCursor);

	//get object pointed by the cursor
    lmStaffObj* pCursorSO = pVCursor->GetStaffObj();

    //if no object, ignore command. It is due, for example, to the user clicking 'Del' key
    //on no object
	if (!pCursorSO)
        return;

    //prepare command and submit it
    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = wxString::Format(_("Delete %s"), pCursorSO->GetName().c_str() );
	pCP->Submit(new lmCmdDeleteStaffObj(pVCursor, sName, m_pDoc, pCursorSO));
}

void lmScoreCanvas::DeleteStaffObj(lmStaffObj* pSO)
{
	//delete the StaffObj.
    //caret is updated only if it was pointing to the deleted object

    //get cursor
    lmVStaffCursor* pVCursor = m_pView->GetVCursor();
	wxASSERT(pVCursor);

    //prepare command and submit it
    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = wxString::Format(_("Delete %s"), pSO->GetName().c_str() );
	pCP->Submit(new lmCmdDeleteStaffObj(pVCursor, sName, m_pDoc, pSO));
}

void lmScoreCanvas::DeleteCaretOrSelected()
{
    //If there is a selection, delete all objects in the selection.
    //Else delete staffobj pointed by caret

    if (m_pView->SomethingSelected())
        DeleteSelection();
    else
        DeleteCaretSatffobj();
}

void lmScoreCanvas::DeleteSelection()
{
    //Deleted all objects in the selection.

    //get cursor
    lmVStaffCursor* pVCursor = m_pView->GetVCursor();
	wxASSERT(pVCursor);

    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = _("Delete selection");
	pCP->Submit(new lmCmdDeleteSelection(pVCursor, sName, m_pDoc, m_pView->GetSelection()) );
}

void lmScoreCanvas::BreakBeam()
{
    //Break beamed group at selected note (the one pointed by cursor)

    //get cursor
    lmVStaffCursor* pVCursor = m_pView->GetVCursor();
	wxASSERT(pVCursor);

	//get object pointed by the cursor
    lmStaffObj* pCursorSO = pVCursor->GetStaffObj();
	wxASSERT(pCursorSO && pCursorSO->IsNoteRest());

    //prepare command and submit it
    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = _("Break a beam");
	pCP->Submit(new lmCmdBreakBeam(pVCursor, sName, m_pDoc, (lmNoteRest*)pCursorSO));
}

void lmScoreCanvas::JoinBeam() 
{
    //depending on current selection content, either:
    // - create a beamed group with the selected notes,
    // - join two or more beamed groups
    // - or add a note to a beamed group

    //get cursor
    lmVStaffCursor* pVCursor = m_pView->GetVCursor();
	wxASSERT(pVCursor);

    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = _("Add beam");
	pCP->Submit(new lmCmdJoinBeam(pVCursor, sName, m_pDoc, m_pView->GetSelection()) );
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
	pCP->Submit(new lmCmdDeleteTie(sName, m_pDoc, pEndNote) );
}

void lmScoreCanvas::AddTie(lmNote* pStartNote, lmNote* pEndNote)
{
    //Tie received note with previous one.
    //Preconditions: pEndNote is not tied and has been checked that can be tied to previous one

    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = _("Add tie");
	pCP->Submit(new lmCmdAddTie(sName, m_pDoc, pStartNote, pEndNote) );
}

void lmScoreCanvas::AddTuplet()
{
    // Add a tuplet to the selected notes/rests (there could be other objects selected
    // beetween the notes)
    //
    // Precondition:
    //      it has been checked that all notes/rest in the seleccion are not in a tuplet,
    //      are consecutive and are in the same voice.

    //get cursor
    lmVStaffCursor* pVCursor = m_pView->GetVCursor();
	wxASSERT(pVCursor);

    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = _("Add tuplet");
	pCP->Submit(new lmCmdAddTuplet(pVCursor, sName, m_pDoc, m_pView->GetSelection(),
                                   true, 3, true, ep_Default, 3, 2) );
                               //bool fShowNumber, int nNumber, bool fBracket,
                               //lmEPlacement nAbove, int nActual, int nNormal)
}

void lmScoreCanvas::DeleteTuplet(lmNoteRest* pStartNR)
{
    // Remove tuplet from tuplet group. The first note/rest of the tuplet group is pStartNR
    //
    // Precondition:
    // It has been checked that pStatNR is in a tuplet and that it is the first note/rest.

    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = _("Delete tuplet");
	pCP->Submit(new lmCmdDeleteTuplet(sName, m_pDoc, pStartNR) );
}

void lmScoreCanvas::InsertClef(lmEClefType nClefType)
{
	//insert a Clef at current cursor position

    //get cursor
    lmVStaffCursor* pVCursor = m_pView->GetVCursor();
	wxASSERT(pVCursor);

    //prepare command and submit it
    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = _("Insert clef");
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
	wxString sName = _("Insert time signature");
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
	wxString sName = _("Insert key signature");
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
	wxString sName = _("Insert barline");
	pCP->Submit(new lmCmdInsertBarline(pVCursor, sName, m_pDoc, nType) );
}

void lmScoreCanvas::InsertNote(lmEPitchType nPitchType, int nStep, int nOctave,
							   lmENoteType nNoteType, float rDuration, int nDots,
							   lmENoteHeads nNotehead, lmEAccidentals nAcc,
                               int nVoice, lmNote* pBaseOfChord, bool fTiedPrev)
{
	//insert a note at current cursor position

    //get cursor
    lmVStaffCursor* pVCursor = m_pView->GetVCursor();
	wxASSERT(pVCursor);

	//if new note in chord check that there is a base note at current cursor position

    //prepare command and submit it
    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = _("Insert note");
    wxString sOctave = wxString::Format(_T("%d"), nOctave);

    wxString sAllSteps = _T("cdefgab");
    wxString sStep = sAllSteps.GetChar( nStep );

	pCP->Submit(new lmCmdInsertNote(pVCursor, sName, m_pDoc, nPitchType, nStep, nOctave,
							        nNoteType, rDuration, nDots, nNotehead, nAcc,
                                    nVoice, pBaseOfChord, fTiedPrev) );
}

void lmScoreCanvas::InsertRest(lmENoteType nNoteType, float rDuration, int nDots, int nVoice)
{
	//insert a rest at current cursor position

    //get cursor
    lmVStaffCursor* pVCursor = m_pView->GetVCursor();
	wxASSERT(pVCursor);

    //prepare command and submit it
    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = _("Insert rest");

	pCP->Submit(new lmCmdInsertRest(pVCursor, sName, m_pDoc, nNoteType, rDuration,
                                    nDots, nVoice) );
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
	wxString sName = _("Change note pitch");
	pCP->Submit(new lmCmdChangeNotePitch(sName, m_pDoc, (lmNote*)pCursorSO, nSteps) );
}

void lmScoreCanvas::ChangeNoteAccidentals(int nAcc)
{
	//change note accidentals for current selected notes

    lmVStaffCursor* pVCursor = m_pView->GetVCursor();
	wxASSERT(pVCursor);

    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = _("Change note accidentals");
	pCP->Submit(new lmCmdChangeNoteAccidentals(pVCursor, sName, m_pDoc,
                            m_pView->GetSelection(), nAcc) );
}

void lmScoreCanvas::ChangeNoteDots(int nDots)
{
	//change note dots for current selected notes

    lmVStaffCursor* pVCursor = m_pView->GetVCursor();
	wxASSERT(pVCursor);

    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = _("Change note dots");
	pCP->Submit(new lmCmdChangeNoteRestDots(pVCursor, sName, m_pDoc,
                            m_pView->GetSelection(), nDots) );
}

void lmScoreCanvas::ChangeText(lmScoreText* pST, wxString sText, lmEHAlign nAlign,
                               lmLocation tPos, lmTextStyle* pStyle, int nHintOptions)
{
	//change properties of a lmTextItem object

    lmVStaffCursor* pVCursor = m_pView->GetVCursor();
	wxASSERT(pVCursor);

    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = _("Change text");
	pCP->Submit(new lmCmdChangeText(pVCursor, sName, m_pDoc, pST, sText,
                                    nAlign, tPos, pStyle, nHintOptions) );
}

void lmScoreCanvas::ChangeBarline(lmBarline* pBL, lmEBarline nType, bool fVisible)
{
    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	pCP->Submit(new lmCmdChangeBarline(m_pDoc, pBL, nType, fVisible) );
}

void lmScoreCanvas::ChangeMidiSettings(lmInstrument* pInstr, int nMidiChannel,
                                       int nMidiInstr)
{
    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	pCP->Submit(new lmCmdChangeMidiSettings(m_pDoc, pInstr, nMidiChannel,
                                            nMidiInstr) );
}

void lmScoreCanvas::OnToolBoxEvent(lmToolBoxEvent& event)
{
	lmToolBox* pToolBox = GetMainFrame()->GetActiveToolBox();
	wxASSERT(pToolBox);

    switch (event.GetToolGroupID())
    {
        case lmGRP_NoteAcc:
            //selection of accidentals ----------------------------------------------------
            if (m_pView->SomethingSelected())
            {
			    lmToolPageNotes* pNoteOptions = pToolBox->GetNoteProperties();
			    int nAcc;
                switch(pNoteOptions->GetNoteAccidentals())
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
			    lmToolPageNotes* pNoteOptions = pToolBox->GetNoteProperties();
			    ChangeNoteDots( pNoteOptions->GetNoteDots() );
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


//--------------------------------------------------------------------------------------------

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
			    lmENoteType nNoteType = pNoteOptions->GetNoteDuration();
			    int nDots = pNoteOptions->GetNoteDots();
			    lmENoteHeads nNotehead = pNoteOptions->GetNoteheadType();
			    lmEAccidentals nAcc = pNoteOptions->GetNoteAccidentals();
				m_nOctave = pNoteOptions->GetOctave();
				int nVoice = pNoteOptions->GetVoice();

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
                            nAcc = oCmdParser.GetAccidentals();
                            nDots = oCmdParser.GetDots();
                            fTiedPrev = oCmdParser.GetTiedPrev();
                        }
                    }
                }

                //compute note/rest duration
			    float rDuration = lmLDPParser::GetDefaultDuration(nNoteType, nDots, 0, 0);

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
						lmStaffObj* pSO = m_pView->GetVCursor()->GetStaffObj();
						if (pSO && pSO->IsNoteRest() && ((lmNoteRest*)pSO)->IsNote())
							pBaseOfChord = (lmNote*)pSO;
					}

                    //do insert note
					InsertNote(lm_ePitchRelative, nStep, m_nOctave, nNoteType, rDuration,
							   nDots, nNotehead, nAcc, nVoice, pBaseOfChord, fTiedPrev);

                    fUnknown = false;
                }

                //insert rest
                if (nKeyCode == int(' '))
                {
                    //do insert rest
                    InsertRest(nNoteType, rDuration, nDots, nVoice);

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
    GetMainFrame()->GetActiveDoc()->UpdateAllViews();
}

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
    lmVStaffCursor* pVCursor = m_pView->GetVCursor();
	wxASSERT(pVCursor);

	//get object pointed by the cursor
    lmStaffObj* pCursorSO = pVCursor->GetStaffObj();
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
    lmVStaffCursor* pVCursor = m_pView->GetVCursor();
	wxASSERT(pVCursor);

	//get object pointed by the cursor
    lmStaffObj* pCursorSO = pVCursor->GetStaffObj();
    if (pCursorSO && pCursorSO->IsNoteRest())
    {
        //it is a note/rest. Verify if it is beamed
        lmNoteRest* pNR = (lmNoteRest*)pCursorSO;
        if (pNR->IsBeamed())
        {
            //ok. it is in a beam. Verify that it is not the first object in the beam
            lmBeam* pBeam = pNR->GetBeam();
            if (pNR != pBeam->GetFirstNoteRest())
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
