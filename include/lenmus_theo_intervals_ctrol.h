//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2014 LenMus project
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
#include "lenmus_standard_header.h"
#include "lenmus_exercise_ctrol.h"
#include "lenmus_interval.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>

//lomse
#include <lomse_pitch.h>
using namespace lomse;


namespace lenmus
{

//forward declarations
class TheoIntervalsConstrains;
class DocumentWindow;


//---------------------------------------------------------------------------------------
//abstract class for exercises on intervals theory
class TheoIntervalsCtrol : public OneScoreCtrol
{
protected:
    TheoIntervalsConstrains* m_pConstrains;

    //to show the answer
    EClef           m_nClef;
    EKeySignature   m_nKey;
    FIntval         m_fpIntv;
    FPitch          m_fpStart;
    FPitch          m_fpEnd;
    int             m_type;     //interval type: 0-harmonic, 1-ascending, 2-descending

    TheoIntervalsCtrol(long dynId, ApplicationScope& appScope, DocumentWindow* pCanvas);

public:
    virtual ~TheoIntervalsCtrol();

    //implementation of virtual pure in parent EBookCtrol
    virtual void get_ctrol_options_from_params();

    //implementation of virtual methods
    ImoScore* prepare_aux_score(int nButton);

    wxDialog* get_settings_dialog();
    void on_settings_changed();
    wxString set_new_problem();

protected:
    virtual void reconfigure_keyboard()=0;
    void set_problem_space();
    virtual wxString prepare_scores()=0;
    void create_deck(wxString& sSetName, EKeySignature nKey);
    void set_space_level_0();

};

//---------------------------------------------------------------------------------------
class BuildIntervalsCtrol : public TheoIntervalsCtrol
{
public:
    BuildIntervalsCtrol(long dynId, ApplicationScope& appScope, DocumentWindow* pCanvas);
    virtual ~BuildIntervalsCtrol();

    //implementation of virtual methods
    void initialize_strings();
    void initialize_ctrol();
    void create_answer_buttons(LUnits height, LUnits spacing);
    void reconfigure_keyboard();

    enum {
        k_num_cols = 7,
        k_num_rows = 5,
        k_num_buttons = 35,
    };

protected:
    wxString prepare_scores();

private:
    //buttons for the answers: 5 rows, 7 cols = 35 buttons
    ButtonCtrl*     m_pAnswerButton[k_num_buttons];
    ImoTextItem*    m_pRowLabel[k_num_rows];
    ImoTextItem*    m_pColumnLabel[k_num_cols];
};


//---------------------------------------------------------------------------------------
class IdfyIntervalsCtrol : public TheoIntervalsCtrol
{
public:
    IdfyIntervalsCtrol(long dynId, ApplicationScope& appScope, DocumentWindow* pCanvas);
    virtual ~IdfyIntervalsCtrol();

    //implementation of virtual methods
    void initialize_strings();
    void initialize_ctrol();

    enum {
        k_num_cols = 8,
        k_num_rows = 6,
        k_num_buttons = 51,
    };

    void enable_buttons(bool fEnable);
    void reconfigure_keyboard();

protected:
    void create_answer_buttons(LUnits height, LUnits spacing);
    wxString prepare_scores();

        // member variables

    //buttons for the answers: 6 rows, 8 cols + 3 extra buttons = 51 buttons
    ButtonCtrl*     m_pAnswerButton[k_num_buttons];
    ImoTextItem*    m_pRowLabel[k_num_rows];
    ImoTextItem*    m_pColumnLabel[k_num_cols];

    //to give the answer
    int             m_nFirstRow;
};


}   //namespace lenmus

#endif  // __LENMUS_THEO_INTERVALS_CTROL_H__
