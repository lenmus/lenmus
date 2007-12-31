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
#include "../ldp_parser/LDPParser.h"

#include "global.h"

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

void lmScoreCanvas::MoveObject(lmGMObject* pGMO, const lmUPoint& uPos)
{
	//Generate move command to move the lmComponentObj and update the document

	wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
    lmScoreObj* pSO = pGMO->GetScoreOwner();
	wxString sName = wxString::Format(_T("Move %s"), pGMO->GetName().c_str() );
	pCP->Submit(new lmCmdMoveScoreObj(sName, m_pDoc, pSO, uPos));
}

void lmScoreCanvas::SelectObject(lmGMObject* pGMO)
{
	//select/deselect a ComponentObj

    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = wxString::Format(_T("Select %s"), pGMO->GetName().c_str() );
	pCP->Submit(new lmCmdSelectSingle(sName, m_pDoc, pGMO));
}

void lmScoreCanvas::DeleteObject()
{
	//delete the StaffObj at current cursor position

	//get object pointed by the cursor
    lmStaffObj* pCursorSO = m_pView->GetCursorPosition();
	wxASSERT(pCursorSO);

	//the EOS Barline can not be deleted
	if (pCursorSO->GetClass() == eSFOT_Barline 
		&& ((lmBarline*)pCursorSO)->GetBarlineType() == lm_eBarlineEOS) return;

	//advance cursor; otherwise the View will try to render cursor over the deleted
	//StaffObj and will fail
	m_pView->CursorRight();

	//send delete command
    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = wxString::Format(_T("Delete %s"), pCursorSO->GetName().c_str() );
	pCP->Submit(new lmCmdDeleteObject(sName, m_pDoc, pCursorSO));
}


void lmScoreCanvas::InsertClef(lmEClefType nClefType)
{
	//insert a Clef at current cursor position
    lmStaffObj* pCursorSO = m_pView->GetCursorPosition();
	wxASSERT(pCursorSO);
    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = _T("Insert clef");
	pCP->Submit(new lmCmdInsertClef(sName, m_pDoc, pCursorSO, nClefType) );
}

void lmScoreCanvas::InsertBarline(lmEBarline nType)
{
	//insert a barline at current cursor position
    lmStaffObj* pCursorSO = m_pView->GetCursorPosition();
	wxASSERT(pCursorSO);
    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = _T("Insert barline");
	pCP->Submit(new lmCmdInsertBarline(sName, m_pDoc, pCursorSO, nType) );
}

void lmScoreCanvas::InsertNote(lmEPitchType nPitchType,
							   wxString sStep, wxString sOctave, 
							   lmENoteType nNoteType, float rDuration)
{
	//insert a note at current cursor position
    lmStaffObj* pCursorSO = m_pView->GetCursorPosition();
	wxASSERT(pCursorSO);
    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	wxString sName = _T("Insert note");
	pCP->Submit(new lmCmdInsertNote(sName, m_pDoc, pCursorSO, nPitchType, sStep, sOctave, 
							        nNoteType, rDuration) );
}

void lmScoreCanvas::OnKeyPress(wxKeyEvent& event)
{
	lmEEditTool nTool = lmTOOL_NONE;
	lmToolBox* pToolBox = GetMainFrame()->GetActiveToolBox();
	if (!pToolBox) {
		wxLogMessage(_T("[lmScoreCanvas::OnKeyPress] No ToolBox!"));
	}
	else
		nTool = pToolBox->GetSelectedTool();

    int nKeyCode = event.GetKeyCode();
	bool fUnknown = false;

	switch(nTool)
	{
        case lmTOOL_NONE:	//---------------------------------------------------------
		{
			switch (nKeyCode)
			{
				case WXK_UP:
					m_pView->CursorUp();
					break;

				case WXK_DOWN:
					m_pView->CursorDown();
					break;


				default:
					fUnknown = true;
			}
			break;
		}

        case lmTOOL_NOTES:	//---------------------------------------------------------
		{
			float rDuration = lmLDPParser::GetDefaultDuration(eEighth, false, false, 0, 0);
			switch (nKeyCode)
			{
				case 97:    // 'a' insert A note
					InsertNote(lm_ePitchRelative, _T("a"), _T("4"), eEighth, rDuration);
					break;

				case 98:    // 'b' insert B note
					InsertNote(lm_ePitchRelative, _T("b"), _T("4"), eEighth, rDuration);
					break;

				case 99:    // 'c' insert C note
					InsertNote(lm_ePitchRelative, _T("c"), _T("4"), eEighth, rDuration);
					break;

				case 100:   // 'd' insert D note
					InsertNote(lm_ePitchRelative, _T("d"), _T("4"), eEighth, rDuration);
					break;

				case 101:   // 'e' insert E note
					InsertNote(lm_ePitchRelative, _T("e"), _T("4"), eEighth, rDuration);
					break;

				case 102:   // 'f' insert F note
					InsertNote(lm_ePitchRelative, _T("f"), _T("4"), eEighth, rDuration);
					break;

				case 103:   // 'g' insert G	 note
					InsertNote(lm_ePitchRelative, _T("g"), _T("4"), eEighth, rDuration);
					break;

				default:
					fUnknown = true;
			}
			break;
		}

        case lmTOOL_CLEFS:	//---------------------------------------------------------
		{
			switch (nKeyCode)
			{
				case 103:	// 'g' insert G clef
					InsertClef(lmE_Sol);
					break;

				case 102:	// 'f' insert F4 clef
					InsertClef(lmE_Fa4);
					break;

				case 99:    // 'c' insert C3 clef
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
				case 98:	// 'b' insert duble barline
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

	//Not yet processed key. Verify common keys working with all tools
	if (fUnknown) 
	{
		fUnknown = false;
		switch (nKeyCode)
		{
			case WXK_LEFT:
				m_pView->CursorLeft();
				break;

			case WXK_RIGHT:
				m_pView->CursorRight();
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

            case WXK_DELETE:
				DeleteObject();
				break;

			default:
				fUnknown = true;
		}
	}


	//Debug: Unidentified tool or unidentified key. Log message
	if (fUnknown) 
	{
        wxString key;
        switch ( nKeyCode )
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
                if ( wxIsprint((int)nKeyCode) )
                    key.Printf(_T("'%c'"), (char)nKeyCode);
                else if ( nKeyCode > 0 && nKeyCode < 27 )
                    key.Printf(_("Ctrl-%c"), _T('A') + nKeyCode - 1);
                else
                    key.Printf(_T("unknown (%ld)"), nKeyCode);
            }
        }
        #if wxUSE_UNICODE
            key += wxString::Format(_T(" (Unicode: %#04x)"), event.GetUnicodeKey());
        #endif // wxUSE_UNICODE

        wxMessageBox( wxString::Format( _T("[lmScoreCanvas::OnKeyPress] Unknown char event: %s (flags = %c%c%c%c). Tool = %d"),
                key.c_str(),
                (event.ControlDown() ? _T('C') : _T('-') ),
                (event.AltDown() ? _T('A') : _T('-') ),
                (event.ShiftDown() ? _T('S') : _T('-') ),
                (event.MetaDown() ? _T('M') : _T('-') ),
				nTool ));
        event.Skip();
    }

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

