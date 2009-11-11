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

#ifndef __LM_STATUSBAR_H__        //to avoid nested includes
#define __LM_STATUSBAR_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "StatusBar.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "../score/defs.h"


//different layouts
enum lmEStatusBarLayout
{
    lm_eStatBar_Books =0,
    lm_eStatBar_ScoreEdit,
};

//the application status bar
class lmStatusBar : public wxStatusBar
{
public:
    lmStatusBar(wxFrame* pFrame, lmEStatusBarLayout nType, wxWindowID id);
    virtual ~lmStatusBar();

	//event handlers
	void OnSize(wxSizeEvent& event);
    void OnButtonMouse(wxCommandEvent& WXUNUSED(event));
    void OnButtonCaret(wxCommandEvent& WXUNUSED(event));

    //updating fields
    void SetMsgText(const wxString& sText);
    void SetMouseData(int nPage, float rTime, int nMeasure, lmUPoint uPos);
    void SetCaretData(int nPage, float rTime, int nMeasure);

    //info
    inline lmEStatusBarLayout GetType() const { return m_nType; }


private:
    //updating fields
    void SetNumPage(int nPage);
    void SetMousePos(float x, float y);
    void SetTimePosInfo(float rTime, int nMeasure, bool fEmpty);
    void DoSelectMouseInfo();
    void DoSelectCaretInfo();
    void UpdateTimeInfo();

    wxFrame*            m_pFrame;       //parent frame
    int                 m_nNumFields;
    lmEStatusBarLayout  m_nType;
	wxStaticBitmap*		m_pBmpClock;
	wxStaticBitmap*		m_pBmpPage;
	wxStaticBitmap*		m_pBmpMouse;
	wxString			m_sIconSpace;
    wxBitmapButton*     m_pBtMouse;
    wxBitmapButton*     m_pBtCaret;

    //for mouse/caret buttons
    wxBitmap            m_bmpMouseInfoNormal;
    wxBitmap            m_bmpMouseInfoSel;
    wxBitmap            m_bmpCaretInfoNormal;
    wxBitmap            m_bmpCaretInfoSel;


    bool            m_fMouseDisplay;        //display mouse or caret info

    //mouse info
    int             m_nMousePage;
    float           m_rMouseTime;
    int             m_nMouseMeasure;

    //caret info
    int             m_nCaretPage;
    float           m_rCaretTime;
    int             m_nCaretMeasure;


    DECLARE_EVENT_TABLE()
};


#endif    // __LM_STATUSBAR_H__
