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

#ifndef _SCORECANVAS_H        //to avoid nested includes
#define _SCORECANVAS_H

#ifdef __GNUG__
#pragma interface "ScoreCanvas.cpp"
#endif

#if wxUSE_GENERIC_DRAGIMAGE
#include "wx/generic/dragimgg.h"
#define wxDragImage wxGenericDragImage
#else
#include "wx/dragimag.h"
#endif

#include "wx/docview.h"
#include "global.h"
class lmScoreHighlightEvent;
#include "../sound/SoundEvents.h"

class lmScoreView;
class lmScoreDocument;


//Abstract class. All controllers must derive from it
class lmController : public wxWindow
{
   DECLARE_ABSTRACT_CLASS(lmController)

public:
    lmController(wxWindow *pParent, lmScoreView *pView, lmScoreDocument* pDoc,
				 wxColor colorBg, wxWindowID id = wxID_ANY,
				 const wxPoint& pos = wxDefaultPosition,
				 const wxSize& size = wxDefaultSize, long style = 0);
	virtual ~lmController() {}

	//commands without Do/Undo support
	virtual void PlayScore() {}
    virtual void StopPlaying(bool fWait=false) {}
    virtual void PausePlaying() {}

	// commands with Do/Undo support
	virtual void MoveObject(lmScoreObj* pSO, const lmUPoint& uPos) {}
	virtual void SelectObject(lmScoreObj* pSO) {}

    // event handlers
    virtual void OnKeyPress(wxKeyEvent& event) { event.Skip(); }
	virtual void OnEraseBackground(wxEraseEvent& event);


private:
    //wxView*         m_pView;        //the associated view

    DECLARE_EVENT_TABLE()
};



class lmScoreCanvas : public lmController
{
	DECLARE_CLASS(lmScoreCanvas)

public:

    // constructors and destructor
    lmScoreCanvas(lmScoreView *pView, wxWindow *pParent, lmScoreDocument* pDoc, 
                  const wxPoint& pos, const wxSize& size, long style, wxColor colorBg);
    ~lmScoreCanvas();

    // event handlers
    void OnPaint(wxPaintEvent &WXUNUSED(event));
    void OnMouseEvent(wxMouseEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    void OnVisualHighlight(lmScoreHighlightEvent& event);
	void OnKeyPress(wxKeyEvent& event);

	//commands without Do/Undo support
    void PlayScore();
    void StopPlaying(bool fWait=false);
    void PausePlaying();

	// commands with Do/Undo support
	void MoveObject(lmScoreObj* pSO, const lmUPoint& uPos);
	void SelectObject(lmScoreObj* pSO);


private:

    // Tile the bitmap
    bool TileBitmap(const wxRect& rect, wxDC& dc, wxBitmap& bitmap);


private:
    lmScoreView*        m_pView;        //owner view
    wxWindow*           m_pOwner;       //parent window
    lmScoreDocument*    m_pDoc;         //the document rendered by the view

    wxColour        m_colorBg;              //colour for background

    DECLARE_EVENT_TABLE()
};


#endif  // _SCORECANVAS_H
