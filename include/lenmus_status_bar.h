//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2010-2015 LenMus project
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
//---------------------------------------------------------------------------------------

#ifndef __LENMUS_STATUS_BAR_H__        //to avoid nested includes
#define __LENMUS_STATUS_BAR_H__

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_status_reporter.h"


namespace lenmus
{

//---------------------------------------------------------------------------------------
//different layouts
enum EStatusBarLayout
{
    lm_eStatBar_Books =0,
    lm_eStatBar_ScoreEdit,
};

//---------------------------------------------------------------------------------------
//the application status bar
class StatusBar : public wxStatusBar
                , public StatusReporter
{
public:
    StatusBar(wxFrame* pFrame, EStatusBarLayout nType, wxWindowID id);
    virtual ~StatusBar();

	//event handlers
	void OnSize(wxSizeEvent& event);
    void OnButtonMouse(wxCommandEvent& WXUNUSED(event));
    void OnButtonCaret(wxCommandEvent& WXUNUSED(event));

    //mandatory overrides
    void report_status(const wxString& sText);
    void report_mouse_data(int nPage, TimeUnits rTime, int nMeasure, UPoint uPos);
    void report_caret_data(int nPage, TimeUnits rTime, int nMeasure);
    void report_caret_time(const string& timecode);

    //info
    inline EStatusBarLayout GetType() const { return m_nType; }


private:
    //updating fields
    void SetNumPage(int nPage);
    void SetMousePos(float x, float y);
    void SetTimePosInfo(TimeUnits rTime, int nMeasure, bool fEmpty);
    void DoSelectMouseInfo();
    void DoSelectCaretInfo();
    void UpdateTimeInfo();

    wxFrame*            m_pFrame;       //parent frame
    int                 m_nNumFields;
    EStatusBarLayout    m_nType;
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
    TimeUnits       m_rMouseTime;
    int             m_nMouseMeasure;

    //caret info
    int             m_nCaretPage;
    TimeUnits       m_rCaretTime;
    int             m_nCaretMeasure;


    wxDECLARE_EVENT_TABLE();
};


}   //namespace lenmus

#endif    // __LENMUS_STATUS_BAR_H__
