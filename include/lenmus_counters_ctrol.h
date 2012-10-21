//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2012 LenMus project
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

#ifndef __LENMUS_COUNTERS_CTROL_H__
#define __LENMUS_COUNTERS_CTROL_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_injectors.h"

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

//lomse
#include <lomse_internal_model.h>
#include <lomse_document.h>
#include <lomse_events.h>
#include <lomse_shape_text.h>
#include <lomse_control.h>
#include <lomse_static_text_ctrl.h>
using namespace lomse;


//lomse
#include <lomse_hyperlink_ctrl.h>
using namespace lomse;


namespace lenmus
{

//forward declaration
class ProblemManager;
class LeitnerManager;
class QuizManager;
class ExerciseOptions;
class ExerciseCtrol;

//---------------------------------------------------------------------------------------
// CountersCtrol: an abstract control to embed in exercises for displaying
// statistics about user performance in doing the exercise
class CountersCtrol : public Control
{
protected:
    ApplicationScope&   m_appScope;
    ExerciseCtrol*      m_pOwner;
    ExerciseOptions*    m_pConstrains;

public:
    CountersCtrol(ApplicationScope& appScope, Document* pDoc,
                  ExerciseCtrol* pOwner, ExerciseOptions* pConstrains);
    virtual ~CountersCtrol();

    virtual void UpdateDisplay()=0;
    virtual void OnNewQuestion() {}

    //EventHandler mandatory overrides
    virtual void handle_event(SpEventInfo pEvent) = 0;

    //other
    void RightWrongSound(bool fSuccess);

protected:
    void add_mode_controls(ImoContent* pWrapper);
    void on_mode_changed(wxCommandEvent& WXUNUSED(event));
    void change_generation_mode(int mode);
    void change_generation_mode_label(int mode);

};

//---------------------------------------------------------------------------------------
// QuizCounters: a control to embed in exercises to display number of right and
// wrong student answers, in a session, and the total score (percentage)
class QuizCounters : public CountersCtrol
{
protected:
    USize           m_size;
    QuizManager*    m_pProblemMngr;
    bool            m_fTwoTeamsMode;

    //displays
    GmoBoxControl*     m_pMainBox;
    StaticTextCtrl*   m_pRightCounter[2];
    StaticTextCtrl*   m_pWrongCounter[2];
    StaticTextCtrl*   m_pTotalCounter[2];

    //bitmaps
	wxImage m_imgWrong;
	wxImage m_imgRight;
	wxImage m_imgTotal;
    wxImage m_imgRed;
    wxImage m_imgBlue;
    wxImage m_imgGrey;

	wxImage* m_pImgTeam[2];

public:
    QuizCounters(ApplicationScope& appScope, Document* pDoc, ExerciseCtrol* pOwner,
                 ExerciseOptions* pConstrains, QuizManager* pProblemMngr, int nNumTeams);
    ~QuizCounters();

    //Control pure virtual methods implementation
    USize measure();
    LUnits width() { return m_size.width; }
    LUnits height() { return m_size.height; }
    LUnits top() { return m_pMainBox->get_top(); }
    LUnits bottom() { return m_pMainBox->get_bottom(); }
    LUnits left() { return m_pMainBox->get_left(); }
    LUnits right() { return m_pMainBox->get_right(); }
    GmoBoxControl* layout(LibraryScope& libraryScope, UPoint pos);
    void on_draw(Drawer* pDrawer, RenderOptions& opt);

    //CounterCtrol pure virtual methods implementation
    void UpdateDisplay();

    //overrides
    void OnNewQuestion();
    void handle_event(SpEventInfo pEvent);

    // settings
    void ResetCounters();
    void NextTeam();

    //event handlers
    static void on_reset_counters(void* pThis, SpEventInfo pEvent);

protected:
    void UpdateDisplays(int nTeam);
    void CreateCountersGroup(int nTeam, GmoBox* pMainBox, UPoint pos);
    void reset_counters();
};


////---------------------------------------------------------------------------------------
//// LeitnerCounters: a control to embed in html exercises to display statistics
//// on user performance in learning the subject. It uses the Leitner system of
//// spaced repetitions
//class LeitnerCounters : public CountersCtrol
//{
//public:
//    LeitnerCounters(wxWindow* parent, wxWindowID id,
//                    ExerciseCtrol* pOwner, ExerciseOptions* pConstrains,
//                    LeitnerManager* pProblemMngr,
//                    const wxPoint& pos = wxDefaultPosition);
//    ~LeitnerCounters();
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
//	ImoTextItem* m_pTxtNumQuestions;
//	ImoTextItem* m_pLblEST;
//	ImoTextItem* m_pTxtTime;
//	ImoTextItem* m_pLblSession;
//	ImoTextItem* m_pTxtSession;
//	wxGauge* m_pGaugeSession;
//	ImoTextItem* m_pLblGlobal;
//	ImoTextItem* m_pTxtGlobal;
//	wxGauge* m_pGaugeGlobal;
//};



////---------------------------------------------------------------------------------------
//// PractiseCounters: a control to embed in html exercises to display statistics
//// on user performance in learning the subject. It uses the Leitner system in
//// practise mode
//class PractiseCounters : public CountersCtrol
//{
//public:
//    PractiseCounters(wxWindow* parent, wxWindowID id,
//                     ExerciseCtrol* pOwner, ExerciseOptions* pConstrains,
//                     LeitnerManager* pProblemMngr,
//                     const wxPoint& pos = wxDefaultPosition);
//    ~PractiseCounters();
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
//	wxStaticBitmap*     m_pImgWrong;
//	ImoTextItem*       m_pTxtWrong;
//	wxStaticBitmap*     m_pImgRight;
//	ImoTextItem*       m_pTxtRight;
//	wxStaticBitmap*     m_pImgTotal;
//	ImoTextItem*       m_pTxtTotal;
//};


}   // namespace lenmus

#endif  // __LENMUS_COUNTERS_CTROL_H__
