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

#ifndef __LENMUS_IDFY_NOTES_CTROL_H__        //to avoid nested includes
#define __LENMUS_IDFY_NOTES_CTROL_H__

//lenmus
#include "lenmus_exercise_ctrol.h"
//#include "../score/Score.h"

//// For compilers that support precompilation, includes <wx/wx.h>.
//#include <wx/wxprec.h>
//#include <wx/wx.h>


namespace lenmus
{

//forward declarations
class UrlAuxCtrol;
class NotesConstrains;

//---------------------------------------------------------------------------------------
class IdfyNotesCtrol : public OneScoreCtrol
{
public:

    // constructor and destructor
    IdfyNotesCtrol(long dynId, ApplicationScope& appScope, DocumentCanvas* pCanvas);

    ~IdfyNotesCtrol();

    //implementation of virtual pure in parent EBookCtrol
    virtual void get_ctrol_options_from_params();

    //implementation of virtual methods
    void initialize_strings();
    void initialize_ctrol();
    void create_answer_buttons(LUnits height, LUnits spacing);
    void prepare_aux_score(int nButton);
    wxString set_new_problem();
    wxDialog* get_settings_dialog();
    void on_settings_changed();
    void EnableButtons(bool fEnable);

    //specific event handlers
    void OnPlayA4(wxCommandEvent& event);
    void OnPlayAllNotes(wxCommandEvent& event);
    void OnContinue(wxCommandEvent& event);

    //overrides
    void OnNewProblem(wxCommandEvent& event);
    void OnRespButton(wxCommandEvent& event);
    void display_solution();

protected:
    wxString prepare_score(EClefExercise nClef, wxString& sNotePitch, ImoScore** pProblemScore,
                          ImoScore** pSolutionScore = NULL );
    int GetFirstOctaveForClef(EClefExercise nClef);
    void PrepareAllNotesScore();
    void set_initial_state();
    void DisplayAllNotes();
    void set_problem_space();


        // member variables

    enum {
        k_num_buttons = 12,
    };

    NotesConstrains* m_pConstrains;       //constraints for the exercise

    //problem asked
    EKeySignature  m_nKey;

    //answer
    ImoButton*  m_pAnswerButton[k_num_buttons];     //buttons for the answers
    string      m_sButtonLabel[k_num_buttons];

    //specific controls and data
    UrlAuxCtrol*      m_pPlayA4;          //"Play A4 reference note" link
    UrlAuxCtrol*      m_pPlayAllNotes;    //"Play all notes to identify" link
    UrlAuxCtrol*      m_pContinue;        //"Continue" link
};


}   // namespace lenmus

#endif  // __LENMUS_IDFY_NOTES_CTROL_H__
