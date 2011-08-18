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

#ifndef __LENMUS_THEO_INTERVALS_CTROL_H__
#define __LENMUS_THEO_INTERVALS_CTROL_H__

//lenmus
#include "lenmus_exercise_ctrol.h"
//#include "TheoIntervalsConstrains.h"
//#include "Generators.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>


namespace lenmus
{

//forward declarations
class TheoIntervalsConstrains;
class DocumentCanvas;


//---------------------------------------------------------------------------------------
//abstract class for exercises on intervals theory
class TheoIntervalsCtrol : public OneScoreCtrol
{
protected:
//    //to show the answer
//    EClefType         m_nClef;
//    EKeySignature    m_nKey;
//    lmFIntval           m_fpIntv;
//    lmFPitch            m_fpStart;
//    lmFPitch            m_fpEnd;

    TheoIntervalsCtrol(long dynId, ApplicationScope& appScope, DocumentCanvas* pCanvas);

public:
    virtual ~TheoIntervalsCtrol();

    //implementation of virtual pure in parent EBookCtrol
    virtual void get_ctrol_options_from_params();


    //implementation of virtual methods
    void prepare_aux_score(int nButton);

    wxDialog* get_settings_dialog();
    void on_settings_changed();
    wxString set_new_problem();


protected:
    virtual void reconfigure_keyboard() =0; //{};
//    void SetProblemSpace();
    virtual wxString prepare_scores()=0;
//    void CreateQuestionsSet(wxString& sSetName, EKeySignature nKey);
//    void SetSpaceLevel0();
//
};

//class BuildIntervalCtrol : public TheoIntervalsCtrol
//{
//public:
//
//    // constructor and destructor
//    BuildIntervalCtrol(wxWindow* parent, wxWindowID id,
//               lmTheoIntervalsConstrains* pConstrains,
//               const wxPoint& pos = wxDefaultPosition,
//               const wxSize& size = wxDefaultSize, int style = 0);
//
//    ~BuildIntervalCtrol();
//
//    //implementation of virtual methods
//    void initialize_strings();
//    void create_answer_buttons(int nHeight, int nSpacing, wxFont& font);
//
//    enum {
//        k_num_cols = 7,
//        k_num_rows = 5,
//        k_num_buttons = 35,
//    };
//
//protected:
//    wxString prepare_scores();
//
//private:
//    //buttons for the answers: 5 rows, 7 cols = 35 buttons
//    ImoButton*       m_pAnswerButton[k_num_buttons];
//    wxStaticText*   m_pRowLabel[k_num_rows];
//    wxStaticText*   m_pColumnLabel[k_num_cols];
//};


//---------------------------------------------------------------------------------------
class IdfyIntervalsCtrol : public TheoIntervalsCtrol
{
public:
    IdfyIntervalsCtrol(long dynId, ApplicationScope& appScope, DocumentCanvas* pCanvas);
    ~IdfyIntervalsCtrol();

    //implementation of virtual methods
    void initialize_strings();
    void initialize_ctrol();
    void create_initial_layout();

    enum {
        k_num_cols = 8,
        k_num_rows = 6,
        k_num_buttons = 51,
    };

    void enable_buttons(bool fEnable);
    void reconfigure_keyboard();

    //mandatory override for EventHandler
    void handle_event(EventInfo* pEvent);


protected:
    void create_answer_buttons(LUnits height, LUnits spacing);
    wxString prepare_scores();
    void set_event_handlers();

        // member variables

    //buttons for the answers: 6 rows, 8 cols + 3 extra buttons = 51 buttons
    ImoButton*      m_pAnswerButton[k_num_buttons];
    ImoTextItem*    m_pRowLabel[k_num_rows];
    ImoTextItem*    m_pColumnLabel[k_num_cols];

    //to give the answer
    int                 m_nFirstRow;
};


}   //namespace lenmus

#endif  // __LENMUS_THEO_INTERVALS_CTROL_H__
