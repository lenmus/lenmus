//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2011 LenMus project
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

#ifndef __LENMUS_COUNTERS_AUX_CTROL_H__        //to avoid nested includes
#define __LENMUS_COUNTERS_AUX_CTROL_H__

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/intl.h>

#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/gauge.h>
#include <wx/panel.h>


namespace lenmus
{

////forward declarations
//class ProblemManager;
//class LeitnerManager;
//class QuizManager;


//---------------------------------------------------------------------------------------
// CountersAuxCtrol: an abstract control to embed in exercises for displaying
// statistics about user performance in doing the exercise
class CountersAuxCtrol //5.0*: public wxPanel
{

public:
    CountersAuxCtrol(wxWindow* parent, wxWindowID id, double rScale,
                       const wxPoint& pos = wxDefaultPosition,
                       const wxSize& size = wxDefaultSize);

    virtual ~CountersAuxCtrol() {}

    virtual void UpdateDisplay() {} //5.0*=0;
    virtual void OnNewQuestion() {}

//    //other
//    void RightWrongSound(bool fSuccess);
//
//
//protected:
//    double              m_rScale;           //current scaling factor


};

////---------------------------------------------------------------------------------------
//// QuizAuxCtrol: a control to embed in html exercises to display number
//// on right and wrong student answers, in a session, and the total score
//// (percentage)
////----------------------------------------------------------------------------
//class QuizAuxCtrol : public CountersAuxCtrol
//{
//public:
//
//    // constructor and destructor
//    QuizAuxCtrol(wxWindow* parent, wxWindowID id, int nNumTeams, double rScale,
//                   QuizManager* pProblemMngr,
//                   const wxPoint& pos = wxDefaultPosition);
//
//    ~QuizAuxCtrol();
//
//    //base class virtual methods implementation
//    void UpdateDisplay();
//
//    //overrides
//    void OnNewQuestion();
//
//    // event handlers
//    void OnResetCounters(wxCommandEvent& WXUNUSED(event));
//
//    // settings
//    void ResetCounters();
//    void NextTeam();
//
//protected:
//    void UpdateDisplays(int nTeam);
//    void CreateCountersGroup(int nTeam, wxBoxSizer* pMainSizer);
//
//    QuizManager*   m_pProblemMngr;
//
//    bool    m_fTwoTeamsMode;
//
//    //displays
//    wxStaticText*   m_pRightCounter[2];
//    wxStaticText*   m_pWrongCounter[2];
//    wxStaticText*   m_pTotalCounter[2];
//
//    //labels
//	wxStaticBitmap*     m_pBmpWrong;
//	wxStaticBitmap*     m_pBmpRight;
//	wxStaticBitmap*     m_pBmpTotal;
//	wxStaticBitmap*     m_pBmpTeam[2];
//
//    //bitmaps for teams
//    wxBitmap            m_bmpRed;
//    wxBitmap            m_bmpBlue;
//    wxBitmap            m_bmpGrey;
//
//
//    DECLARE_EVENT_TABLE()
//};
//
//
////---------------------------------------------------------------------------------------
//// LeitnerAuxCtrol: a control to embed in html exercises to display statistics
//// on user performance in learning the subject. It uses the Leitner system of
//// spaced repetitions
////----------------------------------------------------------------------------
//class LeitnerAuxCtrol : public CountersAuxCtrol
//{
//
//public:
//
//    // constructor and destructor
//    LeitnerAuxCtrol(wxWindow* parent, wxWindowID id, double rScale,
//                      LeitnerManager* pProblemMngr,
//                      const wxPoint& pos = wxDefaultPosition);
//
//    ~LeitnerAuxCtrol();
//
//    //event handlers
//    void OnExplainProgress(wxCommandEvent& WXUNUSED(event));
//
//    //base class virtual methods implementation
//    void UpdateDisplay();
//
//
//protected:
//    void CreateControls();
//
//    LeitnerManager*       m_pProblemMngr;
//
//	wxStaticText* m_pTxtNumQuestions;
//	wxStaticText* m_pLblEST;
//	wxStaticText* m_pTxtTime;
//	wxStaticText* m_pLblSession;
//	wxStaticText* m_pTxtSession;
//	wxGauge* m_pGaugeSession;
//	wxStaticText* m_pLblGlobal;
//	wxStaticText* m_pTxtGlobal;
//	wxGauge* m_pGaugeGlobal;
//
//    DECLARE_EVENT_TABLE()
//};
//
//
//
////---------------------------------------------------------------------------------------
//// PractiseAuxCtrol: a control to embed in html exercises to display statistics
//// on user performance in learning the subject. It uses the Leitner system in
//// practise mode
////----------------------------------------------------------------------------
//class PractiseAuxCtrol : public CountersAuxCtrol
//{
//
//public:
//
//    // constructor and destructor
//    PractiseAuxCtrol(wxWindow* parent, wxWindowID id, double rScale,
//                      LeitnerManager* pProblemMngr,
//                      const wxPoint& pos = wxDefaultPosition);
//
//    ~PractiseAuxCtrol();
//
//    //event handlers
//    void OnResetCounters(wxCommandEvent& WXUNUSED(event));
//
//    //base class virtual methods implementation
//    void UpdateDisplay();
//
//
//protected:
//    void CreateControls();
//
//    LeitnerManager*       m_pProblemMngr;
//
//	wxStaticBitmap*     m_pBmpWrong;
//	wxStaticText*       m_pTxtWrong;
//	wxStaticBitmap*     m_pBmpRight;
//	wxStaticText*       m_pTxtRight;
//	wxStaticBitmap*     m_pBmpTotal;
//	wxStaticText*       m_pTxtTotal;
//
//
//    DECLARE_EVENT_TABLE()
//};


}   //namespace lenmus

#endif  // __LENMUS_COUNTERS_AUX_CTROL_H__

