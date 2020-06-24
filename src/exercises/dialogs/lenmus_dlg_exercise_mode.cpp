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

//lenmus
#include "lenmus_dlg_exercise_mode.h"
#include "lenmus_constrains.h"

//
////lomse
//#include "lomse_pitch.h"
//#include "lomse_internal_model.h"
//using namespace lomse;

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/button.h>


namespace lenmus
{

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(DlgExerciseMode, wxDialog)
   EVT_BUTTON(wxID_OK, DlgExerciseMode::OnAcceptClicked)
wxEND_EVENT_TABLE()



//---------------------------------------------------------------------------------------
DlgExerciseMode::DlgExerciseMode(wxWindow* parent, int* pMode,
                                 ExerciseOptions* pConstrains)
    : wxDialog(parent, wxID_ANY, _("Choose the operation mode"),
#if (LENMUS_PLATFORM_UNIX == 1 || LENMUS_PLATFORM_MAC == 1)
               //BUG BYPASS: In linux doesn't auto-resize. I don't know why
               wxDefaultPosition, wxSize(700, 520),
#else
               wxDefaultPosition, wxSize(700, -1),
#endif
               wxCAPTION | wxCLOSE_BOX | wxRESIZE_BORDER )
    , m_pMode(pMode)
    , m_pRadLearning(nullptr)
    , m_pRadPractice(nullptr)
    , m_pRadExam(nullptr)
    , m_pRadQuiz(nullptr)
    , m_pParent(parent)
    , m_pConstrains(pConstrains)
{
    create_controls();
    set_current_mode();
    CentreOnScreen();
}

//---------------------------------------------------------------------------------------
DlgExerciseMode::~DlgExerciseMode()
{
}

//---------------------------------------------------------------------------------------
void DlgExerciseMode::create_controls()
{
    //AWARE: Code created with wxFormBuilder and copied here.
    //Modifications:
    //added if-then blocks to control which buttons to create
    //added window style wxRB_GROUP to first radio button

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* pMainSizer;
	pMainSizer = new wxBoxSizer( wxVERTICAL );

    if (m_pConstrains->IsGenerationModeSupported(k_learning_mode))
    {
        m_pRadLearning = new wxRadioButton( this, wxID_ANY, _("Learning mode"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
        pMainSizer->Add( m_pRadLearning, 0, wxTOP|wxRIGHT|wxLEFT|wxBOTTOM, 10 );

        m_pLblLearning = new wxStaticText( this, wxID_ANY, _("The program will record and analyse your answers.  Questions will focus on those issues in which the program detects that you are having more trouble. This mode minimizes your study time and optimises your learning rate. It is based on the 'Leitner' methodology, also known as 'spaced repetition'."), wxPoint( -1,-1 ), wxDefaultSize, 0 );
        m_pLblLearning->Wrap( 650 );
        pMainSizer->Add( m_pLblLearning, 0, wxLEFT, 30 );
    }

    if (m_pConstrains->IsGenerationModeSupported(k_practise_mode))
    {
        m_pRadPractice = new wxRadioButton( this, wxID_ANY, _("Practice mode"), wxDefaultPosition, wxDefaultSize, 0 );
        pMainSizer->Add( m_pRadPractice, 0, wxTOP|wxRIGHT|wxLEFT|wxBOTTOM, 10 );

        m_pLblPractice = new wxStaticText( this, wxID_ANY, _("The program uses your performance data, saved in learning mode, to choose questions. It selects questions at random but giving more probability to those that are troubling you. This mode is useful when you have finished your daily assignment in 'learning' mode and you would like to practise more."), wxPoint( -1,-1 ), wxDefaultSize, 0 );
        m_pLblPractice->Wrap( 650 );
        pMainSizer->Add( m_pLblPractice, 0, wxLEFT, 30 );
    }

    if (m_pConstrains->IsGenerationModeSupported(k_exam_mode))
    {
        m_pRadExam = new wxRadioButton( this, wxID_ANY, _("Exam mode"), wxDefaultPosition, wxDefaultSize, 0 );
        pMainSizer->Add( m_pRadExam, 0, wxTOP|wxRIGHT|wxLEFT|wxBOTTOM, 10 );

        m_pLblExam = new wxStaticText( this, wxID_ANY, _("Neither your saved performance data not your answers to previous questions are taken into account to formulate questions. At any moment, all possible questions have the same probability of being asked. This mode is useful for testing your knowledge before taking an examination, but is less useful for learning."), wxPoint( -1,-1 ), wxDefaultSize, 0 );
        m_pLblExam->Wrap( 650 );
        pMainSizer->Add( m_pLblExam, 0, wxLEFT, 30 );
    }

    if (m_pConstrains->IsGenerationModeSupported(k_quiz_mode))
    {
        m_pRadQuiz = new wxRadioButton( this, wxID_ANY, _("Quiz mode"), wxDefaultPosition, wxDefaultSize, 0 );
        pMainSizer->Add( m_pRadQuiz, 0, wxTOP|wxRIGHT|wxLEFT|wxBOTTOM, 10 );

        m_pLblQuiz = new wxStaticText( this, wxID_ANY, _("This mode is similar to the 'exam' mode but two answer counters are displayed and questions are accounted in both counters: one in the first counter and the next one in the second counter. This mode is useful to work in pairs or in teams at classroom."), wxPoint( -1,-1 ), wxDefaultSize, 0 );
        m_pLblQuiz->Wrap( 650 );
        pMainSizer->Add( m_pLblQuiz, 0, wxBOTTOM|wxLEFT, 30 );
    }

	wxBoxSizer* pButtonSizer;
	pButtonSizer = new wxBoxSizer( wxHORIZONTAL );


	pButtonSizer->Add( 0, 0, 1, wxEXPAND, 5 );

	m_pBtnAccept = new wxButton( this, wxID_OK, _("Accept"), wxDefaultPosition, wxDefaultSize, 0 );
	pButtonSizer->Add( m_pBtnAccept, 0, wxBOTTOM, 5 );


	pButtonSizer->Add( 0, 0, 1, wxEXPAND, 5 );

	pMainSizer->Add( pButtonSizer, 1, wxEXPAND|wxBOTTOM, 20 );

	this->SetSizer( pMainSizer );
	this->Layout();

}

//---------------------------------------------------------------------------------------
void DlgExerciseMode::set_current_mode()
{
    if (*m_pMode == k_learning_mode)
        m_pRadLearning->SetValue(true);
    else if (*m_pMode == k_practise_mode)
        m_pRadPractice->SetValue(true);
    else if (*m_pMode == k_exam_mode)
        m_pRadExam->SetValue(true);
    else
        m_pRadQuiz->SetValue(true);
}

//---------------------------------------------------------------------------------------
void DlgExerciseMode::OnAcceptClicked(wxCommandEvent& WXUNUSED(event))
{
    if (m_pRadLearning && m_pRadLearning->GetValue())
        *m_pMode = k_learning_mode;
    else if (m_pRadPractice && m_pRadPractice->GetValue())
        *m_pMode = k_practise_mode;
    else if (m_pRadQuiz && m_pRadQuiz->GetValue())
        *m_pMode = k_quiz_mode;
    else
        *m_pMode = k_exam_mode;

    //terminate the dialog
    EndModal(wxID_OK);
}


}   //namespace lenmus
