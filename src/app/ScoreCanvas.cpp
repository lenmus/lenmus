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
#include "ArtProvider.h"        // to use ArtProvider for managing icons

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
	pCP->Submit(new lmScoreCommandMove(_T("Move object"), m_pDoc, pSO, uPos));
}

void lmScoreCanvas::SelectObject(lmGMObject* pGMO)
{
	//select/deselect a ComponentObj

    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
	pCP->Submit(new lmCmdSelectSingle(_T("Select object"), m_pDoc, pGMO));
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

