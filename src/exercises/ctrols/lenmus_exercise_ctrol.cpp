//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2015 LenMus project
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
#include "lenmus_exercise_ctrol.h"

#include "lenmus_url_aux_ctrol.h"
#include "lenmus_constrains.h"
#include "lenmus_generators.h"
#include "lenmus_string.h"
#include "lenmus_document_canvas.h"
#include "lenmus_injectors.h"
#include "lenmus_colors.h"
#include "lenmus_counters_ctrol.h"
#include "lenmus_events.h"
#include "lenmus_problem_displayer.h"
#include "lenmus_midi_server.h"


//lomse
#include <lomse_doorway.h>
#include <lomse_internal_model.h>
#include <lomse_im_note.h>
#include <lomse_im_factory.h>
#include <lomse_staffobjs_table.h>
#include <lomse_score_player.h>
#include <lomse_midi_table.h>
#include <lomse_ldp_exporter.h>
#include <lomse_shapes.h>
#include <lomse_hyperlink_ctrl.h>
#include <lomse_logger.h>

using namespace lomse;

//wxWidgets
#include <wx/wxprec.h>
#include <wx/textctrl.h>
#include <wx/image.h>

//using namespace std;

namespace lenmus
{

//=======================================================================================
// EBookCtrol implementation
//=======================================================================================


//---------------------------------------------------------------------------------------
EBookCtrol::EBookCtrol(long dynId, ApplicationScope& appScope, DocumentWindow* pCanvas)
    : DynControl(dynId, appScope)
    , EventHandler()
    , PlayerNoGui()
    , m_pCanvas(pCanvas)
    , m_pBaseConstrains(nullptr)
    , m_pDyn(nullptr)
    , m_pDoc(nullptr)
    , m_fControlsCreated(false)
    , m_state(0)
{
}

//---------------------------------------------------------------------------------------
EBookCtrol::~EBookCtrol()
{
}

//---------------------------------------------------------------------------------------
void EBookCtrol::generate_content(ADynamic dyn, ADocument doc)
{
    m_doc = doc;
    m_dyn = dyn;
    m_pDyn = m_dyn.internal_object();
    m_pDoc = m_doc.internal_object();
    m_dynId = dyn.object_id();

    get_ctrol_options_from_params();
    initialize_ctrol();
}

//---------------------------------------------------------------------------------------
void EBookCtrol::handle_event(SpEventInfo pEvent)
{
    //Default handler. Should never arrive here.

    if (pEvent->is_on_click_event())
    {
        SpEventMouse pEv( static_pointer_cast<EventMouse>(pEvent) );
        if (pEv->is_still_valid())
        {
            ImoContentObj* pImo = dynamic_cast<ImoContentObj*>( pEv->get_source() );

            if (pImo && pImo->is_link() )
            {
                ImoLink* pLink = dynamic_cast<ImoLink*>(pImo);
                string& url = pLink->get_url();
                wxString msg = wxString::Format("[ExerciseCtrol::handle_event] "
                                                "url = '%s'"
                                                , to_wx_string(url).wx_str() );
                wxMessageBox(msg);
            }
            else
            {
                if (pImo)
                {
                    wxString msg = wxString::Format("[EBookCtrol::handle_event] "
                                                    "click on ImoObj of type %d, id=%d"
                                                    , pImo->get_obj_type()
                                                    , pImo->get_id() );
                    wxMessageBox(msg);
                }
                else
                    wxMessageBox("[EBookCtrol::handle_event] click on GmoObj, no Imo");
            }
        }
    }
}

//---------------------------------------------------------------------------------------
void EBookCtrol::on_settings_button()
{
    wxDialog* pDlg = get_settings_dialog();
    if (pDlg)
    {
        int retcode = pDlg->ShowModal();
        if (retcode == wxID_OK)
        {
            m_pBaseConstrains->save_settings();

            // When changing settings it is necessary to review answer buttons
            // or other issues. Give derived classes a chance to do it.
            on_settings_changed();
            m_pDoc->notify_if_document_modified();
        }
        delete pDlg;
    }
}

//---------------------------------------------------------------------------------------
void EBookCtrol::on_go_back()
{
    wxWindow* pWnd = m_pCanvas->GetParent();
    PageRequestEvent event( m_pBaseConstrains->GetGoBackURL() );
    ::wxPostEvent(pWnd, event);
}

//---------------------------------------------------------------------------------------
int EBookCtrol::get_metronome_mm()
{
    //overrides of PlayerNoGui for using general metronome for speed settings

    Metronome* pMtr = m_appScope.get_metronome();
    return pMtr->get_mm();
}

//---------------------------------------------------------------------------------------
Metronome* EBookCtrol::get_metronome()
{
    //overrides of PlayerNoGui for using general metronome for speed settings

    return m_appScope.get_metronome();
}

//---------------------------------------------------------------------------------------
wxWindow* EBookCtrol::get_parent_window()
{
    return dynamic_cast<wxWindow*>(m_pCanvas);
}


//=======================================================================================
// Implementation of ExerciseCtrol:
//=======================================================================================
ExerciseCtrol::ExerciseCtrol(long dynId, ApplicationScope& appScope, DocumentWindow* pCanvas)
    : EBookCtrol(dynId, appScope, pCanvas)
    , m_pScoreToPlay(nullptr)
    , m_pDisplay(nullptr)
    , m_pCounters(nullptr)
    , m_fCountersValid(false)
    , m_pCountersWrapper(nullptr)
    , m_pCountersPara(nullptr)
    , m_fQuestionAsked(false)
    , m_nRespAltIndex(-1)
    , m_fSolutionDisplayed(false)
    , m_pPlayButton(nullptr)
    , m_pNewProblem(nullptr)
    , m_pShowSolution(nullptr)
    , m_nNumButtons(0)
    , m_pProblemManager(nullptr)
    , m_sKeyPrefix("")
{
}

//---------------------------------------------------------------------------------------
ExerciseCtrol::~ExerciseCtrol()
{
    delete m_pProblemManager;
    delete m_pDisplay;
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::create_controls()
{
    ExerciseOptions* pConstrains
        = dynamic_cast<ExerciseOptions*>( m_pBaseConstrains );

    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated
    initialize_strings();

    //Create the problem manager
    m_nGenerationMode = k_exam_mode;        //pConstrains->GetGenerationMode();
    create_problem_manager();

    LibraryScope* pLibScope = m_appScope.get_lomse().get_library_scope();

    ImoStyle* pSpacerStyle = m_pDoc->create_private_style();
    pSpacerStyle->margin(0.0f)->padding(0.0f);


    // prepare layout info for answer buttons and spacing
    LUnits nButtonsHeight = 600.0f;    //6 millimeters
    LUnits nSpacing = 100.0f;   //1 millimeter

    //create a paragraph for settings and debug options
    if (pConstrains->IncludeSettingsLink()
        || pConstrains->IncludeGoBackLink()
        || m_appScope.show_debug_links()
       )
    {
        ImoStyle* pTopLineStyle = m_pDoc->create_private_style();
        pTopLineStyle->margin(0.0f)->text_align(ImoStyle::k_align_left);
        pTopLineStyle->margin_bottom(200.0f);
        ImoParagraph* pTopLinePara = m_pDyn->add_paragraph(pTopLineStyle);
        bool fAddSpace = false;

        // settings link
        if (pConstrains->IncludeSettingsLink())
        {
            HyperlinkCtrl* pSettingsLink =
                LENMUS_NEW HyperlinkCtrl(*pLibScope, nullptr, m_pDoc,
                                         to_std_string(_("Exercise options")) );
            pTopLinePara->add_control( pSettingsLink );
            pSettingsLink->add_event_handler(k_on_click_event, this, on_settings);
            fAddSpace = true;
        }

        // "Go back to theory" link
        if (pConstrains->IncludeGoBackLink())
        {
            if (fAddSpace)
                pTopLinePara->add_inline_box(1000.0f, pSpacerStyle);
            HyperlinkCtrl* pGoBackLink =
                LENMUS_NEW HyperlinkCtrl(*pLibScope, nullptr, m_pDoc,
                                         to_std_string(_("Go back to theory")) );
            pTopLinePara->add_control( pGoBackLink );
            pGoBackLink->add_event_handler(k_on_click_event, this, on_go_back_event);
            fAddSpace = true;
        }

        // debug links
        if (m_appScope.show_debug_links())
        {
            if (fAddSpace)
                pTopLinePara->add_inline_box(1000.0f, pSpacerStyle);

            // "See source score"
            HyperlinkCtrl* pSeeSourceLink =
                LENMUS_NEW HyperlinkCtrl(*pLibScope, nullptr, m_pDoc,
                                         to_std_string(_("See source score")) );
            pTopLinePara->add_control( pSeeSourceLink );
            pSeeSourceLink->add_event_handler(k_on_click_event, this, on_see_source_score);
            pTopLinePara->add_inline_box(1000.0f, pSpacerStyle);

            // "See MIDI events"
            HyperlinkCtrl* pSeeMidiLink =
                LENMUS_NEW HyperlinkCtrl(*pLibScope, nullptr, m_pDoc,
                                         to_std_string(_("See MIDI events")) );
            pTopLinePara->add_control( pSeeMidiLink );
            pSeeMidiLink->add_event_handler(k_on_click_event, this, on_see_midi_events);
        }
    }

    // create display ctrol and counters control
    create_display_and_counters();

        //
        // links
        //

    ImoStyle* pParaStyle = m_pDoc->create_private_style();
    pParaStyle->margin_top(0.0f)->margin_bottom(500.0f);
    ImoParagraph* pLinksPara = m_pDyn->add_paragraph(pParaStyle);

    // "New problem" button
    m_pNewProblem =
        LENMUS_NEW HyperlinkCtrl(*pLibScope, nullptr, m_pDoc,
                                 to_std_string(_("New problem")) );
    m_pNewProblem->add_event_handler(k_on_click_event, this, on_new_problem);
    pLinksPara->add_control( m_pNewProblem );
    pLinksPara->add_inline_box(1000.0f, pSpacerStyle);

    // "Show solution" button
    if (pConstrains->IncludeSolutionLink())
    {
        m_pShowSolution =
            LENMUS_NEW HyperlinkCtrl(*pLibScope, nullptr, m_pDoc,
                                     to_std_string(_("Show solution")) );
        m_pShowSolution->add_event_handler(k_on_click_event, this, on_display_solution);
        pLinksPara->add_control( m_pShowSolution );
        pLinksPara->add_inline_box(1000.0f, pSpacerStyle);
    }

    // "Play" button
    if (pConstrains->IncludePlayLink())
    {
        m_pPlayButton =
            LENMUS_NEW HyperlinkCtrl(*pLibScope, nullptr, m_pDoc,
                                     to_std_string(_("Play")) );
        m_pPlayButton->add_event_handler(k_on_click_event, this, on_play_event);
        pLinksPara->add_control( m_pPlayButton );
    }


    create_answer_buttons(nButtonsHeight, nSpacing);
    m_fControlsCreated = true;

    //finish creation

    // disable links
    if (m_pPlayButton) m_pPlayButton->enable(false);
    if (m_pShowSolution) m_pShowSolution->enable(false);

    //reconfigure buttons in accordance with constraints
    on_settings_changed();

    //display problem or initial message
    display_first_time_content();
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::create_display_and_counters()
{
    ImoStyle* style = m_pDoc->create_private_style();
    style->margin(0.0f);
    ImoMultiColumn* pDisplay = m_pDyn->add_multicolumn_wrapper(2, style);
    pDisplay->set_column_width(0, 74.0f);  //display: 74%
    pDisplay->set_column_width(1, 26.0f);  //counters: 26%

    create_problem_display_box( pDisplay->get_column(0) );
    m_pCountersWrapper = pDisplay->get_column(1);
    m_pCounters = create_counters_ctrol( m_pCountersWrapper );
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::create_problem_display_box(ImoContent* pWrapper, ImoStyle* pStyle)
{
    LUnits minHeight = m_pBaseConstrains->get_height();
    m_pDisplay = LENMUS_NEW ProblemDisplayer(m_pCanvas, pWrapper, m_pDoc,
                                             minHeight, pStyle);
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::change_generation_mode(int nMode)
{
    if (m_nGenerationMode != nMode)
    {
        m_nGenerationMode = nMode;
        m_fCountersValid = false;
        create_problem_manager();
        change_counters_ctrol();
        new_problem();
        m_pDoc->notify_if_document_modified();      //force repaint
    }
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::change_counters_ctrol()
{
    if (m_fControlsCreated)
    {
        remove_counters_ctrol();
        //TODO: To continue without counters, just prevent execution of next line
        //if (global_option_use_counters == true)
        m_pCounters = create_counters_ctrol(m_pCountersWrapper);
    }
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::remove_counters_ctrol()
{
    if (m_pCounters && m_pCountersWrapper && m_pCountersPara)
    {
        m_pCountersWrapper->remove_item(m_pCountersPara);
        delete m_pCountersPara;     //this añso deletes the CounterControl
        m_pCountersPara = nullptr;
        m_pCounters = nullptr;
    }
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::create_problem_manager()
{
    delete m_pProblemManager;

    switch(m_nGenerationMode)
    {
        case k_quiz_mode:
        case k_exam_mode:
            m_pProblemManager = LENMUS_NEW QuizManager(m_appScope);
            break;

        case k_learning_mode:
            m_pProblemManager = LENMUS_NEW LeitnerManager(m_appScope, true);
            break;

        case k_practise_mode:
            m_pProblemManager = LENMUS_NEW LeitnerManager(m_appScope, false);
            break;

        default:
            wxASSERT(false);
    }

    set_problem_space();
}

//---------------------------------------------------------------------------------------
CountersCtrol* ExerciseCtrol::create_counters_ctrol(ImoContent* pWrapper)
{
    CountersCtrol* pNewCtrol = nullptr;
    ExerciseOptions* pConstrains = dynamic_cast<ExerciseOptions*>(m_pBaseConstrains);
    if (pConstrains->IsUsingCounters() )
    {
        switch(m_nGenerationMode)
        {
            case k_quiz_mode:
                pNewCtrol = LENMUS_NEW QuizCounters(m_appScope, m_pDoc, this,
                                             pConstrains, (QuizManager*)m_pProblemManager, 2);
                break;

            case k_exam_mode:
                pNewCtrol = LENMUS_NEW QuizCounters(m_appScope, m_pDoc, this,
                                             pConstrains, (QuizManager*)m_pProblemManager, 1);
                break;

            case k_learning_mode:
                if (((LeitnerManager*)m_pProblemManager)->IsLearningMode())
                    pNewCtrol = LENMUS_NEW LeitnerCounters(m_appScope, m_pDoc, this,
                                             pConstrains, (LeitnerManager*)m_pProblemManager);
                else
                    pNewCtrol = LENMUS_NEW PractiseCounters(m_appScope, m_pDoc, this,
                                             pConstrains, (LeitnerManager*)m_pProblemManager);
                break;

            case k_practise_mode:
                pNewCtrol = LENMUS_NEW PractiseCounters(m_appScope, m_pDoc, this,
                                             pConstrains, (LeitnerManager*)m_pProblemManager);
                break;

            default:
                wxASSERT(false);
        }
    }
    m_fCountersValid = true;

    //create an ImoControl containing the counters control
    if (pNewCtrol)
    {
        ImoStyle* wrapperStyle = m_pDoc->create_private_style();
        wrapperStyle->border_width(0.0f)->margin_top(0.0f)->margin_bottom(0.0f);
        wrapperStyle->padding(0.0f)->margin_left(500.0f);  //5 millimeters
        pWrapper->set_style(wrapperStyle);

        ImoStyle* style = m_pDoc->create_private_style();
        style->border_width(0.0f)->padding(0.0f)->margin(0.0f);
        m_pCountersPara = pWrapper->add_paragraph(style);
        m_pCountersPara->add_control(pNewCtrol);
    }

    return pNewCtrol;
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::change_from_learning_to_practising()
{
    m_nGenerationMode = k_practise_mode;
    m_fCountersValid = false;
    static_cast<LeitnerManager*>(m_pProblemManager)->change_to_practise_mode();
    change_counters_ctrol();
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::on_exercise_activated(void* WXUNUSED(pThis), SpEventInfo pEvent)
{
    if (pEvent->is_on_click_event())
    {
        SpEventMouse pEv( static_pointer_cast<EventMouse>(pEvent) );
        if (pEv->is_still_valid())
        {
            ImoContentObj* pImo = dynamic_cast<ImoContentObj*>( pEv->get_source() );
            if (pImo)
                wxMessageBox("Click on exercise");
        }
    }
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::on_new_problem(void* pThis, SpEventInfo WXUNUSED(pEvent))
{
    (static_cast<ExerciseCtrol*>(pThis))->on_new_problem();
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::on_play_event(void* pThis, SpEventInfo WXUNUSED(pEvent))
{
    (static_cast<ExerciseCtrol*>(pThis))->play();
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::on_display_solution(void* pThis, SpEventInfo WXUNUSED(pEvent))
{
    (static_cast<ExerciseCtrol*>(pThis))->on_display_solution();
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::on_settings(void* pThis, SpEventInfo WXUNUSED(pEvent))
{
    (static_cast<ExerciseCtrol*>(pThis))->on_settings_button();
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::on_see_source_score(void* pThis, SpEventInfo WXUNUSED(pEvent))
{
    (static_cast<ExerciseCtrol*>(pThis))->on_debug_show_source_score();
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::on_see_midi_events(void* pThis, SpEventInfo WXUNUSED(pEvent))
{
    (static_cast<ExerciseCtrol*>(pThis))->on_debug_show_midi_events();
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::on_go_back_event(void* pThis, SpEventInfo WXUNUSED(pEvent))
{
    (static_cast<ExerciseCtrol*>(pThis))->on_go_back();
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::handle_event(SpEventInfo pEvent)
{
    if (pEvent->is_mouse_in_event() || pEvent->is_mouse_out_event())
    {
        SpEventMouse pEv( static_pointer_cast<EventMouse>(pEvent) );
        if (!pEv->is_still_valid())
            return;

        ImoObj* pImo = pEv->get_imo_object();
        if (pImo && pImo->is_button())
        {
            if (pEvent->is_mouse_in_event())
                on_button_mouse_in(pEv);
            else
                on_button_mouse_out(pEv);
        }
        return;
    }

    if (pEvent->is_on_click_event())
    {
        SpEventMouse pEv( static_pointer_cast<EventMouse>(pEvent) );
        if (!pEv->is_still_valid())
            return;

        ImoObj* pImo = pEv->get_imo_object();
        if (pImo && pImo->is_button())
        {
            ImoId id = pImo->get_id();
            for (int i=0; i < m_nNumButtons; ++i)
            {
                ButtonCtrl* pButton = *(m_pAnswerButtons + i);
                if (pButton->get_owner_imo_id() == id)
                {
                    on_resp_button(i);
                    return;
                }
            }
        }
        else if (pImo && pImo->is_dynamic())
        {
            //DEBUG: Keep this code. It is harmless and usefull when adding more
            //exercises
            wxMessageBox("Click on exercise");
            return;
        }
    }

    //Should not arrive here. But if it arrives the default handler will display
    //a debug message.
    EBookCtrol::handle_event(pEvent);
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::on_button_mouse_in(SpEventMouse pEvent)
{
    if (!pEvent->is_still_valid())
        return;

    ImoControl* pImo = static_cast<ImoControl*>( pEvent->get_imo_object() );
    ButtonCtrl* pCtrl = static_cast<ButtonCtrl*>( pImo->get_control() );
    pCtrl->set_mouse_in(true);
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::on_button_mouse_out(SpEventMouse pEvent)
{
    if (!pEvent->is_still_valid())
        return;

    ImoControl* pImo = static_cast<ImoControl*>( pEvent->get_imo_object() );
    ButtonCtrl* pCtrl = static_cast<ButtonCtrl*>( pImo->get_control() );
    pCtrl->set_mouse_in(false);
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::on_display_solution()
{
    //First, stop any possible chord being played to avoid crashes
    stop_sounds();

    //inform problem manager of the result
    OnQuestionAnswered(m_iQ, false);

    //produce feedback sound, and update statistics display
    if (m_pCounters)
    {
        m_pCounters->UpdateDisplay();
        m_pCounters->RightWrongSound(false);
    }

    do_display_solution();

    m_pDoc->notify_if_document_modified();
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::on_resp_button(int nIndex)
{
    //First, stop any possible score being played to avoid crashes
    stop_sounds();
    Colors* pColors = m_appScope.get_colors();

    if (m_fQuestionAsked)
    {
        // There is a question asked but not answered.
        // The user press the button to give the answer

        //verify if success or failure
        bool fSuccess = check_success(nIndex);

        //inform problem manager of the result
        OnQuestionAnswered(m_iQ, fSuccess);

        //produce feedback sound, and update statistics display
        if (m_pCounters)
        {
            m_pCounters->UpdateDisplay();
            m_pCounters->RightWrongSound(fSuccess);
        }

        //if failure or not auto-new problem, display the solution.
        //Else, if success and auto-new problem, generate a new problem
        if (!fSuccess || !m_appScope.is_auto_new_problem_enabled())
        {
            if (!fSuccess)
            {
                //failure: mark wrong button in red and right one in green
                set_button_color(m_nRespIndex, pColors->Success() );
                set_button_color(nIndex, pColors->Failure() );
            }

            //show the solucion
            do_display_solution();
        }
        else
        {
            new_problem();
        }
    }
    else {
        // No problem presented. The user press the button to play a specific
        // sound (chord, interval, scale, etc.)
        play_specific_sound(nIndex);
    }

    m_pDoc->notify_if_document_modified();
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::OnQuestionAnswered(int iQ, bool fSuccess)
{
    //inform problem manager of the result
    if (m_pProblemManager)
    {
        //determine user response time
        wxTimeSpan tsResponse = wxDateTime::Now().Subtract( m_tmAsked );
        wxASSERT(!tsResponse.IsNegative());
        m_pProblemManager->UpdateQuestion(iQ, fSuccess, tsResponse);
    }
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::on_new_problem()
{
    new_problem();
    m_pDoc->notify_if_document_modified();
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::new_problem()
{
    //wxLogMessage("[ExerciseCtrol::new_problem]");
    reset_exercise();

    //if Leitner mode check if there are more questions or move to practise mode
    if (is_learning_mode() && !m_pProblemManager->more_questions())
        change_from_learning_to_practising();

    //prepare answer buttons and counters
    if (m_pCounters && m_fCountersValid)
        m_pCounters->OnNewQuestion();
    enable_buttons(true);

    //set m_pProblemScore, m_pSolutionScore, m_sAnswer, m_nRespIndex, m_nPlayMM
    wxString sProblemMessage = set_new_problem();

    //display the problem
    m_fQuestionAsked = true;
    m_pDisplay->set_problem_text( to_std_string(sProblemMessage) );
    display_problem_score();
    m_fSolutionDisplayed = false;

    if (m_pPlayButton) m_pPlayButton->enable( is_play_button_initially_enabled() );
    if (m_pShowSolution) m_pShowSolution->enable(true);

    //save time
    m_tmAsked = wxDateTime::Now();
}

//---------------------------------------------------------------------------------------
bool ExerciseCtrol::is_play_button_initially_enabled()
{
    //deafult implementation: enabled only for ear training exercises
    return !is_theory_mode();
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::display_first_time_content()
{
    if (is_theory_mode())
        new_problem();
    else
        display_initial_msge();
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::display_initial_msge()
{
    m_fQuestionAsked = false;
    m_pDisplay->set_problem_text( get_initial_msge() );
}

//---------------------------------------------------------------------------------------
string ExerciseCtrol::get_initial_msge()
{
    return to_std_string( _("Click on 'New problem' to start") );
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::do_display_solution()
{
    stop_sounds();
    display_solution();

    // mark right button in green
    Colors* pColors = m_appScope.get_colors();
    set_button_color(m_nRespIndex, pColors->Success());

    if (m_pPlayButton) m_pPlayButton->enable(true);
    if (m_pShowSolution) m_pShowSolution->enable(false);
    m_fQuestionAsked = false;
    m_fSolutionDisplayed = true;
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::reset_exercise()
{
    stop_sounds();

    // restore buttons' normal color
    Colors* pColors = m_appScope.get_colors();
    for (int iB=0; iB < m_nNumButtons; iB++)
    {
        set_button_color(iB, pColors->Normal() );
    }

    //clear the display ctrol
    m_pDisplay->clear();

    delete_scores();
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::enable_buttons(bool fEnable)
{
    ButtonCtrl* pButton;
    for (int iB=0; iB < m_nNumButtons; iB++)
    {
        pButton = *(m_pAnswerButtons + iB);
        if (pButton)
            pButton->enable(fEnable);
    }
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::set_buttons(ButtonCtrl* pButtons[], int nNumButtons)
{
    m_pAnswerButtons = pButtons;
    m_nNumButtons = nNumButtons;

    Colors* pColors = m_appScope.get_colors();
    for (int iB=0; iB < m_nNumButtons; iB++)
    {
        ButtonCtrl* pButton = *(m_pAnswerButtons + iB);
        if (pButton)
        {
            pButton->set_bg_color(pColors->Normal());
            pButton->set_mouse_over_color(pColors->Highlight());
        }
    }
    set_event_handlers();
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::set_button_color(int i, Color color)
{
    ButtonCtrl* pButton = *(m_pAnswerButtons + i);
    if (pButton)
        pButton->set_bg_color(color);
}

//---------------------------------------------------------------------------------------
bool ExerciseCtrol::check_success(int nButton)
{
    if (m_nRespAltIndex == -1)
        return m_nRespIndex == nButton;
    else
        return m_nRespAltIndex == nButton || m_nRespIndex == nButton;
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::set_event_handlers()
{
    for (int iB=0; iB < m_nNumButtons; iB++)
    {
        ButtonCtrl* pButton = *(m_pAnswerButtons + iB);
        if (pButton)
        {
            pButton->add_event_handler(k_on_click_event, this);
            pButton->add_event_handler(k_mouse_in_event, this);
            pButton->add_event_handler(k_mouse_out_event, this);
        }
    }
}




//=======================================================================================
// Implementation of CompareCtrol
//  A control with three answer buttons
//=======================================================================================

static string m_sButtonLabel[CompareCtrol::k_num_buttons];

//---------------------------------------------------------------------------------------
CompareCtrol::CompareCtrol(long dynId, ApplicationScope& appScope,
                           DocumentWindow* pCanvas)
    : ExerciseCtrol(dynId, appScope, pCanvas)
{
}

//---------------------------------------------------------------------------------------
void CompareCtrol::initialize_strings()
{
    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated
    m_sButtonLabel[0] = to_std_string( _("First one greater") );
    m_sButtonLabel[1] = to_std_string( _("Second one greater") );
    m_sButtonLabel[2] = to_std_string( _("Both are equal") );
}

//---------------------------------------------------------------------------------------
void CompareCtrol::create_answer_buttons(LUnits height, LUnits WXUNUSED(spacing))
{
    //create buttons for the answers: three buttons in one row

    ImoStyle* pDefStyle = m_pDoc->get_default_style();
    ImoInlineWrapper* pBox;

    ImoStyle* pBtStyle = m_pDoc->create_private_style();
    pBtStyle->font_name("sans")->font_size(8.0f);

    ImoStyle* pRowStyle = m_pDoc->create_private_style();
    pRowStyle->font_size(10.0f)->margin_bottom(0.0f);

    USize buttonSize(4000.0f, height);
    LUnits rowWidth = 4500.0f;

    // the buttons
    LibraryScope* pLibScope = m_appScope.get_lomse().get_library_scope();
    ImoParagraph* pKeyboardRow = m_pDyn->add_paragraph(pRowStyle);
    for (int iB=0; iB < k_num_cols; iB++)
    {
        pBox = pKeyboardRow->add_inline_box(rowWidth, pDefStyle);
        m_pAnswerButton[iB] = pBox->add_button(*pLibScope, m_sButtonLabel[iB],
                                               buttonSize, pBtStyle);
        m_pAnswerButton[iB]->enable(false);
    }

    set_event_handlers();

    //inform base class about the settings
    set_buttons(m_pAnswerButton, k_num_buttons);
}

//---------------------------------------------------------------------------------------
CompareCtrol::~CompareCtrol()
{
}



//=======================================================================================
// Implementation of CompareScoresCtrol
//  A CompareCtrol with two scores
//=======================================================================================
IMPLEMENT_CLASS(CompareScoresCtrol, wxEvtHandler)

//---------------------------------------------------------------------------------------
CompareScoresCtrol::CompareScoresCtrol(long dynId, ApplicationScope& appScope,
                                       DocumentWindow* pCanvas)
    : wxEvtHandler()
    , CompareCtrol(dynId, appScope, pCanvas)
    , m_pPlayer( m_appScope.get_score_player() )
    , m_pSolutionScore(nullptr)
    , m_nPlayMM(80)
    , m_fPlayingProblem(false)
{
    m_pScore[0] = nullptr;
    m_pScore[1] = nullptr;
    Connect(wxID_ANY, wxEVT_TIMER,
        (wxObjectEventFunction)(void (wxEvtHandler::*)(wxTimerEvent&))&CompareScoresCtrol::on_timer_event);
}

//---------------------------------------------------------------------------------------
CompareScoresCtrol::~CompareScoresCtrol()
{
    stop_sounds();
    delete_scores();
}

//---------------------------------------------------------------------------------------
void CompareScoresCtrol::play(bool WXUNUSED(fVisualTracking))
{
    if (m_pCanvas->is_loading_document())
    {
        return;
    }

    if (!m_pPlayer->is_playing())
    {
        // 'play' or 'new problem' pressed

        //change link from "play" to "Stop playing" label
        m_pPlayButton->change_label(to_std_string( _("Stop playing") ));

        //remove informative message
        if (!is_solution_displayed() && !is_theory_mode())
        {
            m_pDisplay->remove_problem_text();
            m_pDoc->notify_if_document_modified();
        }

        //AWARE: The link label is restored to "play" when the EndOfPlay event is
        //       received.

        if (m_fQuestionAsked)
        {
            //Introducing the problem. play the first score
            m_fPlayingProblem = true;
            PlayScore(0, k_no_visual_tracking);
            //AWARE:
            // when 1st score is finished an event will be generated. Then method
            // OnEndOfPlay() will handle the event and play the second score.
        }
        else
        {
            //Asking to play the solution: Play total score
            m_fPlayingProblem = false;
            m_pPlayer->load_score(m_pDisplay->get_problem_score(), this);

            m_nPlayMM = 320;
            countoff_status(k_no_countoff);
            metronome_status(k_no_metronome);
            set_play_mode(k_play_normal_instrument);
            SpInteractor spInteractor = m_pCanvas ? m_pCanvas->get_interactor_shared_ptr()
                                                  : SpInteractor();
            Interactor* pInteractor = (spInteractor ? spInteractor.get() : nullptr);
            m_pPlayer->play(k_do_visual_tracking, m_nPlayMM, pInteractor);
        }
    }
    else
    {
        // "Stop" button pressed. Inform that no more sound is desired
        m_pPlayButton->change_label(to_std_string( _("Play") ));
        m_fPlayingProblem = false;
    }
}

//---------------------------------------------------------------------------------------
void CompareScoresCtrol::PlayScore(int nIntv, bool fVisualTracking)
{
    //hightlight button associated to this score
    m_nNowPlaying = nIntv;
    Colors* pColors = m_appScope.get_colors();
    set_button_color(nIntv, pColors->Highlight() );
    m_pDoc->set_dirty();
    m_pDoc->notify_if_document_modified();

    //play the score
    //AWARE: As the intervals are built using whole notes, we will play them at
    // MM=320 so that real note rate will be 80.
    m_pPlayer->load_score(m_pScore[nIntv], this);

    m_nPlayMM = 320;
    countoff_status(k_no_countoff);
    metronome_status(k_no_metronome);
    set_play_mode(k_play_normal_instrument);
    //Interactor* pInteractor = m_pCanvas ? m_pCanvas->get_interactor() : nullptr;
    SpInteractor spInteractor = m_pCanvas ? m_pCanvas->get_interactor_shared_ptr()
                                          : SpInteractor();
    Interactor* pInteractor = (spInteractor ? spInteractor.get() : nullptr);
    m_pPlayer->play(fVisualTracking, m_nPlayMM, pInteractor);
}

//---------------------------------------------------------------------------------------
void CompareScoresCtrol::on_end_of_playback()
{
    //wxLogMessage("EndOfPlay event received");
    if (m_fQuestionAsked)
    {
        if (m_nNowPlaying == 0 && m_fPlayingProblem)
        {
            //wxLogMessage("EndOfPlay event: Starting timer");
            m_oPauseTimer.SetOwner( this, wxID_ANY );
            m_oPauseTimer.Start(1000, wxTIMER_CONTINUOUS );     //wait for 1sec (1000ms)
        }
        else
        {
            //wxLogMessage("EndOfPlay event: play stopped");
            m_fPlayingProblem = false;
            m_pPlayButton->change_label(to_std_string( _("Play") ));
            m_pDisplay->set_problem_text(to_std_string(_("Press 'Play' to hear it again")));
        }
    }
    else
    {
        //playing after solution is displayed: just change link label
        m_fPlayingProblem = false;
        m_pPlayButton->change_label(to_std_string( _("Play") ));
    }

    //remove highlight in buttons
    Colors* pColors = m_appScope.get_colors();
    set_button_color(0, pColors->Normal() );
    set_button_color(1, pColors->Normal() );
    m_pDoc->set_dirty();
    m_pDoc->notify_if_document_modified();
}

//---------------------------------------------------------------------------------------
void CompareScoresCtrol::on_timer_event(wxTimerEvent& WXUNUSED(event))
{
    m_oPauseTimer.Stop();
    if (m_fPlayingProblem)
    {
        //Still paying the problem. Proceed with second score
        //wxLogMessage("Timer event: still alive. Proceed with second score");
        PlayScore(1, k_no_visual_tracking);
    }
    else
    {
        //Playing problem was stopped. Stop.
        //wxLogMessage("Timer event: play was stopped. Stop");
        m_pPlayButton->change_label(to_std_string( _("Play") ));
    }
}

//---------------------------------------------------------------------------------------
void CompareScoresCtrol::display_solution()
{
    m_pDisplay->remove_problem_text();
    m_pDisplay->set_problem_score(m_pSolutionScore);
    m_pSolutionScore = nullptr;
    m_pDoc->notify_if_document_modified();
}

//---------------------------------------------------------------------------------------
void CompareScoresCtrol::display_problem_score()
{
    //remove any displayed score
    m_pDisplay->set_problem_score(nullptr);

    //play problem
    m_pPlayButton->change_label(to_std_string( _("Stop playing") ));
    play();
}

//---------------------------------------------------------------------------------------
void CompareScoresCtrol::delete_scores()
{
    if (m_pSolutionScore) {
        delete m_pSolutionScore;
        m_pSolutionScore = (ImoScore*)nullptr;
    }
    if (m_pScore[0]) {
        delete m_pScore[0];
        m_pScore[0] = (ImoScore*)nullptr;
    }
    if (m_pScore[1]) {
        delete m_pScore[1];
        m_pScore[1] = (ImoScore*)nullptr;
    }
}

//---------------------------------------------------------------------------------------
void CompareScoresCtrol::stop_sounds()
{
    m_fPlayingProblem = false;
    m_oPauseTimer.Stop();
    m_pPlayer->stop();
}

//---------------------------------------------------------------------------------------
void CompareScoresCtrol::on_debug_show_source_score()
{
    m_pDisplay->debug_show_source_score();
}

//---------------------------------------------------------------------------------------
void CompareScoresCtrol::on_debug_show_midi_events()
{
    m_pDisplay->debug_show_midi_events();
}



//=======================================================================================
// Implementation of OneScoreCtrol
//  An ExerciseCtrol with one score for the problem and one optional score for
//  the solution. If no solution score is defined the problem score is used as
//  solution.
//=======================================================================================
OneScoreCtrol::OneScoreCtrol(long dynId, ApplicationScope& appScope,
                             DocumentWindow* pCanvas)
    : ExerciseCtrol(dynId, appScope, pCanvas)
    , m_pPlayer( m_appScope.get_score_player() )
    , m_pProblemScore(nullptr)
	, m_pSolutionScore(nullptr)
    , m_pAuxScore(nullptr)
    , m_nPlayMM(320)    //it is assumed whole notes
{
}

//---------------------------------------------------------------------------------------
OneScoreCtrol::~OneScoreCtrol()
{
    stop_sounds();
    delete_scores();
}

//---------------------------------------------------------------------------------------
void OneScoreCtrol::play(bool fVisualTracking)
{
    if (!m_pPlayer->is_playing())
    {
        // play button pressed

        //change link from "play" to "Stop playing" label
        m_pPlayButton->change_label(to_std_string( _("Stop playing") ));

        //remove informative message
        if (!is_solution_displayed() && !is_theory_mode())
        {
            m_pDisplay->remove_problem_text();
            m_pDoc->notify_if_document_modified();
        }

        //play the score
        MidiServer* pMIDI = m_appScope.get_midi_server();
        int channel = pMIDI->MtrChannel();
        int instr = pMIDI->MtrInstr();
        int tone1 = pMIDI->MtrTone1();
        int tone2 = pMIDI->MtrTone2();
        m_pPlayer->load_score(m_pScoreToPlay, this, channel, instr, tone1, tone2);

        set_play_mode(k_play_normal_instrument);
        fVisualTracking &= m_pDisplay->is_displayed(m_pScoreToPlay);
        SpInteractor spInteractor = m_pCanvas ?
                                    m_pCanvas->get_interactor_shared_ptr() : SpInteractor();
        Interactor* pInteractor = (spInteractor ? spInteractor.get() : nullptr);
        m_pPlayer->play(fVisualTracking, m_nPlayMM, pInteractor);

        if (m_pCanvas && pInteractor)
            m_pCanvas->customize_playback(spInteractor);

        //AWARE The link label is restored to "play" when the EndOfPlay event is
        //received.
    }
    else
    {
        // "Stop" button pressed
        m_pPlayer->stop();
        m_pPlayButton->change_label(to_std_string( _("Play") ));
    }
}

//---------------------------------------------------------------------------------------
void OneScoreCtrol::on_end_of_playback()
{
    LOMSE_LOG_DEBUG(lomse::Logger::k_events | lomse::Logger::k_score_player, string(""));

    m_pPlayButton->change_label(to_std_string( _("Play") ));
    if (is_play_again_message_allowed())
        m_pDisplay->set_problem_text(to_std_string(_("Press 'Play' to hear it again")));
}

//---------------------------------------------------------------------------------------
bool OneScoreCtrol::is_play_again_message_allowed()
{
    //deafault behaviour: only if solution not displayed
    return !is_solution_displayed();
}

//---------------------------------------------------------------------------------------
void OneScoreCtrol::play_specific_sound(int nButton)
{
    stop_sounds();

    if (are_answer_buttons_allowed_for_playing())
    {
        delete m_pAuxScore;
        m_pAuxScore = prepare_aux_score(nButton);
        if (m_pAuxScore)
        {
            m_pPlayer->load_score(m_pAuxScore, this);
            set_play_mode(k_play_normal_instrument);
            m_pPlayer->play(k_no_visual_tracking, m_nPlayMM, nullptr);
        }
    }
}

//---------------------------------------------------------------------------------------
void OneScoreCtrol::display_solution()
{
    if (remove_problem_text_when_displaying_solution())
        m_pDisplay->set_problem_text("");

    if (m_pSolutionScore)
    {
        //if there is a solution score, display it as solution
        m_pDisplay->set_problem_score(m_pSolutionScore);
        m_pScoreToPlay = m_pSolutionScore;
        m_pSolutionScore = nullptr; //ownership transferred to m_pDisplay
	}
	else if (m_pProblemScore && !m_pDisplay->is_score_displayed())
    {
        //if problem score not yet displayed, display it as the solution
        m_pDisplay->set_problem_score(m_pProblemScore);
        m_pScoreToPlay = m_pProblemScore;
        m_pProblemScore = nullptr;         //ownership transferred to m_pDisplay
    }
    m_pDisplay->set_solution_text( to_std_string(m_sAnswer) );
    if (are_answer_buttons_allowed_for_playing())
        m_pDisplay->set_problem_text( to_std_string(
            _("Press any button below to hear how it sounds, and compare with the right answer") ));

    //enable 'Play' button
    if (m_pPlayButton)
        m_pPlayButton->enable(true);
}

//---------------------------------------------------------------------------------------
void OneScoreCtrol::display_problem_score()
{
    if (m_pProblemScore)
    {
        if (is_theory_mode())
        {
            //theory
            m_pDisplay->set_problem_score(m_pProblemScore);
            m_pScoreToPlay = m_pProblemScore;
            m_pProblemScore = nullptr; //ownership transferred to m_pDisplay
        }
        else
        {
            //ear training
            m_pDisplay->set_problem_score(nullptr);
            m_pScoreToPlay = m_pProblemScore;
            play(k_no_visual_tracking);
        }
    }
}

//---------------------------------------------------------------------------------------
bool OneScoreCtrol::are_answer_buttons_allowed_for_playing()
{
    //default implementation: allowed only for ear training exercises
    return !is_theory_mode();
}

//---------------------------------------------------------------------------------------
bool OneScoreCtrol::remove_problem_text_when_displaying_solution()
{
    //default implementation: remove in era training exercises
    return !is_theory_mode();
}

//---------------------------------------------------------------------------------------
void OneScoreCtrol::delete_scores()
{
    delete m_pProblemScore;
    m_pProblemScore = nullptr;

    delete m_pSolutionScore;
    m_pSolutionScore = nullptr;

    delete m_pAuxScore;
    m_pAuxScore = nullptr;
}

//---------------------------------------------------------------------------------------
void OneScoreCtrol::stop_sounds()
{
    //wxLogMessage("[OneScoreCtrol::stop_sounds]");
    m_pPlayer->stop();
}

//---------------------------------------------------------------------------------------
void OneScoreCtrol::on_debug_show_source_score()
{
    m_pDisplay->debug_show_source_score();
}

//---------------------------------------------------------------------------------------
void OneScoreCtrol::on_debug_show_midi_events()
{
    m_pDisplay->debug_show_midi_events();
}


}   //namespace lenmus
