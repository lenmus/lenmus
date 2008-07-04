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

#ifndef _LM_SCORECANVAS_H        //to avoid nested includes
#define _LM_SCORECANVAS_H

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
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
class lmGMSelection;

#define lmUNSELECT      false       //remove selection
#define lmSELECT        true        //select objects


//Abstract class. All controllers must derive from it
class lmController : public wxWindow
{
   DECLARE_ABSTRACT_CLASS(lmController)

public:
    lmController(wxWindow *pParent, lmScoreView *pView, lmScoreDocument* pDoc,
				 wxColor colorBg, wxWindowID id = wxID_ANY,
				 const wxPoint& pos = wxDefaultPosition,
				 const wxSize& size = wxDefaultSize, long style = 0);
	virtual ~lmController();

	//commands without Do/Undo support
	virtual void PlayScore() {}
    virtual void StopPlaying(bool fWait=false) {}
    virtual void PausePlaying() {}

	// commands with Do/Undo support
	virtual void InsertBarline(lmEBarline nType = lm_eBarlineSimple) {}
    virtual void InsertClef(lmEClefType nClefType) {}
	virtual void InsertNote(lmEPitchType nPitchType, int nStep, int nOctave, 
					lmENoteType nNoteType, float rDuration, int nDots,
					lmENoteHeads nNotehead, lmEAccidentals nAcc = lm_eNoAccidentals ) {}
    virtual void InsertTimeSignature(int nBeats, int nBeatType, bool fVisible = true) {}    //for type eTS_Normal
    virtual void InsertKeySignature(int nFifths, bool fMajor, bool fVisible = true) {}
	virtual void ChangeNotePitch(int nSteps) {}
	virtual void ChangeNoteAccidentals(int nSteps) {}
	virtual void MoveObject(lmGMObject* pGMO, const lmUPoint& uPos) {}
	virtual void SelectObject(lmGMObject* pGMO) {}
    virtual void SelectObjects(bool fSelect, lmGMSelection* pSelection) {};
	virtual void DeleteObject() {}


    // event handlers
    virtual void OnKeyPress(wxKeyEvent& event) { event.Skip(); }
    virtual void OnKeyDown(wxKeyEvent& event) { event.Skip(); }
    virtual void OnKeyUp(wxKeyEvent& event) { event.Skip(); }
	virtual void OnEraseBackground(wxEraseEvent& event);

	//contextual menus
	void ShowContextualMenu(lmScoreObj* pOwner, lmGMObject* pGMO, wxMenu* pMenu, int x, int y);
	virtual wxMenu* GetContextualMenu();

	//event handlers for contextual menus
	virtual void OnCut(wxCommandEvent& event) {}
    virtual void OnCopy(wxCommandEvent& event) {}
    virtual void OnPaste(wxCommandEvent& event) {}
    virtual void OnColor(wxCommandEvent& event) {}
    virtual void OnProperties(wxCommandEvent& event) {}


protected:
	wxMenu*			m_pMenu;			//contextual menu
	lmScoreObj*		m_pMenuOwner;		//contextual menu owner
	lmGMObject*		m_pMenuGMO;			//graphic object who displayed the contextual menu

    int             m_nOctave;          //current octave for note insertion

private:

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
    void OnKeyDown(wxKeyEvent& event);
    void OnKeyUp(wxKeyEvent& event);

	//commands without Do/Undo support
    void PlayScore();
    void StopPlaying(bool fWait=false);
    void PausePlaying();

	// commands with Do/Undo support
	void InsertBarline(lmEBarline nType = lm_eBarlineSimple);
    void InsertClef(lmEClefType nClefType);
	void InsertNote(lmEPitchType nPitchType, int nStep, int nOctave, 
					lmENoteType nNoteType, float rDuration, int nDots,
					lmENoteHeads nNotehead, lmEAccidentals nAcc = lm_eNoAccidentals );
    void InsertTimeSignature(int nBeats, int nBeatType, bool fVisible = true);    //for type eTS_Normal
    void InsertKeySignature(int nFifths, bool fMajor, bool fVisible = true);
	void ChangeNotePitch(int nSteps);
	void ChangeNoteAccidentals(int nSteps);
	void MoveObject(lmGMObject* pGMO, const lmUPoint& uPos);
	void SelectObject(lmGMObject* pGMO);
    void SelectObjects(bool fSelect, lmGMSelection* pSelection);
	void DeleteObject();

	//contextual menus
	wxMenu* GetContextualMenu();

	//event handlers for contextual menus
	void OnCut(wxCommandEvent& event);
    void OnCopy(wxCommandEvent& event);
    void OnPaste(wxCommandEvent& event);
    void OnColor(wxCommandEvent& event);
    void OnProperties(wxCommandEvent& event);



private:

    bool TileBitmap(const wxRect& rect, wxDC& dc, wxBitmap& bitmap);
    void LogKeyEvent(wxString name, wxKeyEvent& event, int nTool=-1) const;

    //tools' selection
    void SelectNoteDuration(int iButton);
    void SelectNoteAccidentals(bool fNext);
    void SelectNoteDots(bool fNext);


    lmScoreView*        m_pView;        //owner view
    wxWindow*           m_pOwner;       //parent window
    lmScoreDocument*    m_pDoc;         //the document rendered by the view

    wxColour        m_colorBg;			//colour for background

	//to control octave when inserting several consecutive notes
	bool			m_fInsertionSequence;
	int				m_nLastOctave;

    //to control aux.keys
    bool            m_fCtrl;        //Ctrol pressed
    bool            m_fAlt;         //Alt pressed
    bool            m_fShift;       //Shift pressed


    DECLARE_EVENT_TABLE()
};


#endif  // _LM_SCORECANVAS_H
