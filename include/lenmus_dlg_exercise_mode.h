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

#ifndef __LENMUS_DLG_EXERCISE_MODE_H__        //to avoid nested includes
#define __LENMUS_DLG_EXERCISE_MODE_H__

//lenmus
#include "lenmus_standard_header.h"

//wxWidgets
#include <wx/intl.h>

#include <wx/string.h>
#include <wx/radiobut.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>



namespace lenmus
{
//forward declarations
class ExerciseOptions;

//---------------------------------------------------------------------------------------
class DlgExerciseMode : public wxDialog
{
private:
    int*            m_pMode;
    wxRadioButton*  m_pRadLearning;
    wxStaticText*   m_pLblLearning;
    wxRadioButton*  m_pRadPractice;
    wxStaticText*   m_pLblPractice;
    wxRadioButton*  m_pRadExam;
    wxStaticText*   m_pLblExam;
    wxRadioButton*  m_pRadQuiz;
    wxStaticText*   m_pLblQuiz;

	wxButton*       m_pBtnAccept;
    wxWindow*       m_pParent;
    ExerciseOptions*  m_pConstrains;

public:
    DlgExerciseMode(wxWindow* parent, int* pMode, ExerciseOptions* pConstrains);
    virtual ~DlgExerciseMode();

private:
    void create_controls();
    void set_current_mode();
    void OnAcceptClicked(wxCommandEvent& WXUNUSED(event));
    void OnCancelClicked(wxCommandEvent& WXUNUSED(event)) { EndDialog(wxID_CANCEL); }

    wxDECLARE_EVENT_TABLE();
};


}   //namespace lenmus

#endif    // __LENMUS_DLG_EXERCISE_MODE_H__
