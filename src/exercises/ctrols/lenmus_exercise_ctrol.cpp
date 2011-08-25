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

//lenmus
#include "lenmus_exercise_ctrol.h"

#include "lenmus_url_aux_ctrol.h"
#include "lenmus_constrains.h"
#include "lenmus_generators.h"
#include "lenmus_string.h"
#include "lenmus_score_canvas.h"
#include "lenmus_injectors.h"
#include "lenmus_colors.h"


//lomse
#include <lomse_doorway.h>
#include <lomse_internal_model.h>
#include <lomse_im_note.h>
#include <lomse_im_factory.h>
#include <lomse_staffobjs_table.h>
#include <lomse_score_player.h>
using namespace lomse;

//wxWidgets
//#include <wx/wxprec.h>
//#include <wx/textctrl.h>

//using namespace std;

namespace lenmus
{

//=======================================================================================
// EBookCtrol implementation
//=======================================================================================


//---------------------------------------------------------------------------------------
EBookCtrol::EBookCtrol(long dynId, ApplicationScope& appScope, DocumentCanvas* pCanvas)
    : DynControl(dynId, appScope)
    , m_pCanvas(pCanvas)
    , m_pBaseConstrains(NULL)
    , m_pDyn(NULL)
    , m_pDoc(NULL)
    , m_pPlayButton(NULL)
    , m_fControlsCreated(false)
//    , m_fDoCountOff(true)
{
}

//---------------------------------------------------------------------------------------
EBookCtrol::~EBookCtrol()
{
    ////delete objects
    //    delete m_pOptions;
}

//---------------------------------------------------------------------------------------
void EBookCtrol::generate_content(ImoDynamic* pDyn, Document* pDoc)
{
    m_pDyn = pDyn;
    m_pDoc = pDoc;

    get_ctrol_options_from_params();
    initialize_ctrol();
}

//---------------------------------------------------------------------------------------
void EBookCtrol::handle_event(EventInfo* pEvent)
{
//    EVT_CHECKBOX        (ID_LINK_COUNTOFF, EBookCtrol::OnDoCountoff)

    if (pEvent->is_on_click_event())
    {
        EventOnClick* pEv = dynamic_cast<EventOnClick*>(pEvent);
        ImoObj* pImo = pEv->get_originator_imo();
        if (pImo->is_link() ) //&& id >= ID_BUTTON && id < ID_BUTTON+k_num_buttons)
        {
            ImoLink* pLink = dynamic_cast<ImoLink*>(pImo);
            string& url = pLink->get_url();

            if (url == "link_play")
                on_play();

            else if (url == "link_settings")
                on_settings_button();

            else if (url == "link_go_back")
                on_go_back();

            else if (url == "link_debug_see_source")
                on_debug_show_source_score();

            else if (url == "link_debug_dump_score")
                on_debug_dump_score();

            else if (url == "link_debug_see_midi_events")
                on_debug_show_midi_events();

            else
            {
                wxString msg = wxString::Format(_T("[ExerciseCtrol::handle_event] ")
                                                _T("url = '%s'")
                                                , to_wx_string(url).c_str() );
                wxMessageBox(msg);
            }
            delete pEvent;
            return;
        }
        else
        {
            wxString msg = wxString::Format(_T("[EBookCtrol::handle_event] ")
                                            _T("click on ImoObj of type %d, id=%d")
                                            , pImo->get_obj_type()
                                            , pImo->get_id() );
            wxMessageBox(msg);
        }
    }
    delete pEvent;
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
            m_pDoc->notify_that_document_has_been_modified();
        }
        delete pDlg;
    }
}

//---------------------------------------------------------------------------------------
void EBookCtrol::on_go_back()
{
        //TODO: 5.0 commented out

//    lmMainFrame* pFrame = GetMainFrame();
//    TextBookController* pBookController = pFrame->GetBookController();
//    pBookController->Display( m_pOptions->GetGoBackURL() );
}

//---------------------------------------------------------------------------------------
void EBookCtrol::on_play()
{
    play();
}

////---------------------------------------------------------------------------------------
//void EBookCtrol::OnDoCountoff(EventInfo* pEvent)
//{
//    m_fDoCountOff = event.IsChecked();
//}


//=======================================================================================
// Implementation of ExerciseCtrol:
//=======================================================================================
ExerciseCtrol::ExerciseCtrol(long dynId, ApplicationScope& appScope, DocumentCanvas* pCanvas)
    : EBookCtrol(dynId, appScope, pCanvas)
    , m_pDisplayCtrol(NULL)
    , m_pCurPara(NULL)
    , m_pCurScore(NULL)
    , m_pCounters(NULL)
    , m_fCountersValid(false)
//    , m_nDisplaySize(nDisplaySize)
//    , m_pBaseConstrains(pConstrains)
    , m_fQuestionAsked(false)
    , m_nRespAltIndex(-1)
    , m_pNewProblem(NULL)
    , m_pShowSolution(NULL)
    , m_nNumButtons(0)
    , m_pProblemManager(NULL)
//    , m_pCboMode((wxChoice*)NULL)
//    , m_pAuxCtrolSizer((wxBoxSizer*)NULL)
    , m_sKeyPrefix(_T(""))
{
//    //initializations
//    SetBackgroundColour(*wxWHITE);
}

//---------------------------------------------------------------------------------------
ExerciseCtrol::~ExerciseCtrol()
{
    delete m_pProblemManager;
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::create_controls()
{
    ExerciseOptions* pConstrains
        = dynamic_cast<ExerciseOptions*>( m_pBaseConstrains );

    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated
    initialize_strings();

//    ImoStyle* pDefStyle = m_pDoc->find_style("Default style");

    ImoStyle* pParaStyle = m_pDoc->create_private_style("Default style");
    pParaStyle->set_lunits_property(ImoStyle::k_margin_top, 500.0f);
    pParaStyle->set_lunits_property(ImoStyle::k_margin_bottom, 1000.0f);

    //Create the problem manager and the problem space
    m_nGenerationMode = pConstrains->GetGenerationMode();
    create_problem_manager();

    // prepare layout info for answer buttons and spacing
    LUnits nButtonsHeight = 600.0f;    //6 millimeters
    LUnits nSpacing = 100.0f;   //1 millimeter

    //the window is divided into two regions: top, for score on left and counters and
    //links on the right, and bottom region, for answer buttons
//    m_pMainSizer = new wxBoxSizer( wxVERTICAL );

        //
        // settings and debug options
        //
//    wxBoxSizer* pTopLineSizer = new wxBoxSizer( wxHORIZONTAL );
//    m_pMainSizer->Add(pTopLineSizer, wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, nSpacing));
    //create a paragraph for settings and debug options
    LUnits linkWidth = 4000.0f;
    if (pConstrains->IncludeSettingsLink()
        || pConstrains->IncludeGoBackLink()
        || m_appScope.show_debug_links()
       )
    {
        ImoParagraph* pTopLinePara = m_pDyn->add_paragraph(pParaStyle);

        // settings link
        if (pConstrains->IncludeSettingsLink())
        {
            new UrlAuxCtrol(this, k_on_click_event, pTopLinePara, m_pDoc,
                            "link_settings", "Exercise options", linkWidth);
        }

        // "Go back to theory" link
        if (pConstrains->IncludeGoBackLink())
        {
            new UrlAuxCtrol(this, k_on_click_event, pTopLinePara, m_pDoc,
                            "link_back", "Go back to theory", linkWidth);
        }

        // debug links
        if (m_appScope.show_debug_links())
        {
            // "See source score"
            new UrlAuxCtrol(this, k_on_click_event, pTopLinePara, m_pDoc,
                            "link_debug_see_source", "See source score", linkWidth);

            // "Dump score"
            new UrlAuxCtrol(this, k_on_click_event, pTopLinePara, m_pDoc,
                            "link_debug_dump_score", "Dump score", linkWidth);

            // "See MIDI events"
            new UrlAuxCtrol(this, k_on_click_event, pTopLinePara, m_pDoc,
                            "link_debug_see_midi_events", "See MIDI events", linkWidth);
        }
    }


//    // sizer for the scoreCtrol and the CountersAuxCtrol
//    wxBoxSizer* pTopSizer = new wxBoxSizer( wxHORIZONTAL );
//    m_pMainSizer->Add(
//        pTopSizer,
//        wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 2*nSpacing) );
//

    // create a box to display problem & solution in it
    create_problem_display_box();
//    pTopSizer->Add(m_pDisplayCtrol,
//                   wxSizerFlags(1).Left().Border(wxTOP|wxBOTTOM, 2*nSpacing));
//
//    // sizer for the CountersAuxCtrol
//    if (pConstrains->IsUsingCounters())
//    {
//        wxStaticBoxSizer* pCountersSizer =
//	        new wxStaticBoxSizer( new wxStaticBox(this, wxID_ANY, wxEmptyString),
//                                  wxVERTICAL);
//
//	    wxBoxSizer* pModeSizer = new wxBoxSizer(wxHORIZONTAL);
//
//	    wxStaticText* pLblMode = new wxStaticText(
//            this, wxID_ANY, _("Mode:"), wxDefaultPosition, wxDefaultSize, 0);
//	    pLblMode->Wrap( -1 );
//	    pModeSizer->Add( pLblMode, 0, wxALL|wxALIGN_CENTER_VERTICAL, nSpacing);
//
//        //load strings for Mode combo
//        int nNumValidModes = 0;
//	    wxString sCboModeChoices[lm_eNumGenerationModes];
//        for (long i=0; i < lm_eNumGenerationModes; i++)
//        {
//            if (pConstrains->IsGenerationModeSupported(i))
//                sCboModeChoices[nNumValidModes++] = get_generation_mode_name(i);
//        }
//	    m_pCboMode = new wxChoice(this, lmID_CBO_MODE, wxDefaultPosition,
//                                  wxDefaultSize, nNumValidModes, sCboModeChoices, 0);
	    change_generation_mode_label( m_nGenerationMode );

//	    pModeSizer->Add( m_pCboMode, 1, wxALL|wxALIGN_CENTER_VERTICAL, nSpacing);
//
//	    pCountersSizer->Add( pModeSizer, 0, wxEXPAND, nSpacing);
//
//	    m_pAuxCtrolSizer = new wxBoxSizer( wxVERTICAL );

        m_pCounters = create_counters_ctrol();
//	    m_pAuxCtrolSizer->Add( m_pCounters, 0, wxALL, nSpacing);
//
//	    pCountersSizer->Add( m_pAuxCtrolSizer, 0, wxEXPAND, nSpacing);
//	    pTopSizer->Add( pCountersSizer, 0, wxLEFT, 3*nSpacing);
//    }

        //
        // links
        //

    ImoParagraph* pLinksPara = m_pDyn->add_paragraph(pParaStyle);

    // "new problem" button
    m_pNewProblem = new UrlAuxCtrol(this, k_on_click_event, pLinksPara, m_pDoc,
                                    "link_new", "New problem", linkWidth);

    // "play" button
    if (pConstrains->IncludePlayLink())
    {
//        m_pPlayButton = new UrlAuxCtrol(this, ID_LINK_PLAY, m_rScale,
//                                          _("play"), _T("link_play"),
//                                          _("Stop playing"), _T("link_stop") );
        m_pPlayButton = new UrlAuxCtrol(this, k_on_click_event, pLinksPara, m_pDoc,
                                        "link_play", "Play", linkWidth);
    }

    // "show solution" button
    if (pConstrains->IncludeSolutionLink())
    {
        m_pShowSolution = new UrlAuxCtrol(this, k_on_click_event, pLinksPara, m_pDoc,
                                          "link_solution", "Show solution", linkWidth);
    }

    create_answer_buttons(nButtonsHeight, nSpacing);

    //finish creation

    //show start message
    wxString sMsg = _("Click on 'New problem' to start");
    display_message(sMsg, true);

    // final buttons/links enable/setup
    if (m_pPlayButton) m_pPlayButton->enable(false);
    if (m_pShowSolution) m_pShowSolution->enable(false);

    on_settings_changed();     //reconfigure buttons in accordance with constraints

    m_fControlsCreated = true;
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::change_generation_mode(int nMode)
{
    m_nGenerationMode = nMode;          //set new generation mode
    m_fCountersValid = false;
    create_problem_manager();             //change problem manager
    change_counters_ctrol();              //replace statistics control
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::change_generation_mode_label(int nMode)
{
    m_nGenerationMode = nMode;
//TODO 5.0 commented out
//    if (m_pCboMode)
//        m_pCboMode->SetStringSelection( get_generation_mode_name(m_nGenerationMode) );
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::change_counters_ctrol()
{
//TODO 5.0 commented out
//    //replace current control if exists
//    if (m_fControlsCreated)
//    {
        CountersAuxCtrol* pNewCtrol = create_counters_ctrol();
//        m_pAuxCtrolSizer->Replace(m_pCounters, pNewCtrol);
//        delete m_pCounters;
//        m_pCounters = pNewCtrol;
//        m_pMainSizer->Layout();
//        m_pCounters->UpdateDisplay();
//    }
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::create_problem_manager()
{
    if (m_pProblemManager)
        delete m_pProblemManager;

    switch(m_nGenerationMode)
    {
        case lm_eQuizMode:
        case lm_eExamMode:
            m_pProblemManager = new QuizManager(this);
            break;

        case lm_eLearningMode:
            m_pProblemManager = new LeitnerManager(this, true);
            break;

        case lm_ePractiseMode:
            m_pProblemManager = new LeitnerManager(this, false);
            break;

        default:
            wxASSERT(false);
    }

    set_problem_space();
}

//---------------------------------------------------------------------------------------
CountersAuxCtrol* ExerciseCtrol::create_counters_ctrol()
{
//TODO 5.0 commented out
//    CountersAuxCtrol* pNewCtrol = (CountersAuxCtrol*)NULL;
//    if (m_pBaseConstrains->IsUsingCounters() )
//    {
//        switch(m_nGenerationMode)
//        {
//            case lm_eQuizMode:
//                pNewCtrol = new QuizAuxCtrol(this, wxID_ANY, 2, m_rScale,
//                                              (QuizManager*)m_pProblemManager);
//                break;
//
//            case lm_eExamMode:
//                pNewCtrol = new QuizAuxCtrol(this, wxID_ANY, 1, m_rScale,
//                                              (QuizManager*)m_pProblemManager);
//                break;
//
//            case lm_eLearningMode:
//                if (((LeitnerManager*)m_pProblemManager)->IsLearningMode())
//                    pNewCtrol = new LeitnerAuxCtrol(this, wxID_ANY, m_rScale,
//                                                 (LeitnerManager*)m_pProblemManager);
//                else
//                    pNewCtrol = new PractiseAuxCtrol(this, wxID_ANY, m_rScale,
//                                                 (LeitnerManager*)m_pProblemManager);
//                break;
//
//            case lm_ePractiseMode:
//                pNewCtrol = new PractiseAuxCtrol(this, wxID_ANY, m_rScale,
//                                                (LeitnerManager*)m_pProblemManager);
//                break;
//
//            default:
//                wxASSERT(false);
//        }
//    }
//    m_fCountersValid = true;
//    return pNewCtrol;
return NULL;
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::handle_event(EventInfo* pEvent)
{
//    EVT_CHOICE          (lmID_CBO_MODE, ExerciseCtrol::OnModeChanged)

    if (pEvent->is_on_click_event())
    {
        EventOnClick* pEv = dynamic_cast<EventOnClick*>(pEvent);
        ImoObj* pImo = pEv->get_originator_imo();

        if (pImo->is_link() )
        {
            ImoLink* pLink = dynamic_cast<ImoLink*>(pImo);
            string& url = pLink->get_url();
            if (url == "link_new")
            {
                delete pEvent;
                on_new_problem();
                return;
            }
            else if (url == "link_solution")
            {
                delete pEvent;
                on_display_solution();
                return;
            }
        }


        else if (pImo->is_button())
        {
            long id = pImo->get_id();
            for (int i=0; i < m_nNumButtons; ++i)
            {
                ImoButton* pButton = *(m_pAnswerButtons + i);
                if (pButton->get_id() == id)
                {
                    on_resp_button(i);
                    delete pEvent;
                    return;
                }
            }
        }
    }

    EBookCtrol::handle_event(pEvent);
}

////---------------------------------------------------------------------------------------
//void ExerciseCtrol::OnModeChanged(EventInfo* pEvent)
//{
//    //locate new mode
//    wxString sMode = m_pCboMode->GetStringSelection();
//	int nMode;
//    for (nMode=0; nMode < lm_eNumGenerationModes; nMode++)
//    {
//        if (sMode == get_generation_mode_name(nMode))
//            break;
//    }
//    wxASSERT(nMode < lm_eNumGenerationModes);
//
//    if (m_nGenerationMode != nMode)
//        this->change_generation_mode(nMode);
//}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::on_display_solution()
{
    //First, stop any possible chord being played to avoid crashes
    stop_sounds();

//    //inform problem manager of the result
//    OnQuestionAnswered(m_iQ, false);
//
//    //produce feedback sound, and update statistics display
//    if (m_pCounters)
//    {
//        m_pCounters->UpdateDisplay();
//        m_pCounters->RightWrongSound(false);
//    }

    do_display_solution();

    m_pDoc->notify_that_document_has_been_modified();
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
        bool fSuccess = check_success_or_failure(nIndex);

//        //inform problem manager of the result
//        OnQuestionAnswered(m_iQ, fSuccess);
//
//        //produce feedback sound, and update statistics display
//        if (m_pCounters)
//        {
//            m_pCounters->UpdateDisplay();
//            m_pCounters->RightWrongSound(fSuccess);
//        }

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
//    else {
//        // No problem presented. The user press the button to play a specific
//        // sound (chord, interval, scale, etc.)
//        PlaySpecificSound(nIndex);
//    }

    m_pDoc->notify_that_document_has_been_modified();
}

////---------------------------------------------------------------------------------------
//void ExerciseCtrol::OnQuestionAnswered(int iQ, bool fSuccess)
//{
//    //inform problem manager of the result
//    if (m_pProblemManager)
//    {
//        //determine user response time
//        wxTimeSpan tsResponse = wxDateTime::Now().Subtract( m_tmAsked );
//        wxASSERT(!tsResponse.IsNegative());
//        m_pProblemManager->UpdateQuestion(m_iQ, fSuccess, tsResponse);
//    }
//}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::on_new_problem()
{
    new_problem();
    m_pDoc->notify_that_document_has_been_modified();
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::new_problem()
{
    reset_exercise();

    //prepare answer buttons and counters
//    if (m_pCounters && m_fCountersValid)
//        m_pCounters->OnNewQuestion();
    enable_buttons(true);

    //set m_pProblemScore, m_pSolutionScore, m_sAnswer, m_nRespIndex, m_nPlayMM
    wxString sProblemMessage = set_new_problem();

    //display the problem
    m_fQuestionAsked = true;
    display_problem();
    display_message(sProblemMessage, false);
    if (m_pPlayButton) m_pPlayButton->enable(true);
    if (m_pShowSolution) m_pShowSolution->enable(true);

//    //save time
//    m_tmAsked = wxDateTime::Now();
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
//    if (!m_pBaseConstrains->ButtonsEnabledAfterSolution()) enable_buttons(false);
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::reset_exercise()
{
    stop_sounds();

//    //clear the display ctrol
//    wxString sMsg = _T("");
//    display_message(sMsg, true);   //true: clear the display ctrol

    // restore buttons' normal color
    Colors* pColors = m_appScope.get_colors();
    for (int iB=0; iB < m_nNumButtons; iB++)
    {
        set_button_color(iB, pColors->Normal() );
    }

    delete_scores();
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::enable_buttons(bool fEnable)
{
    ImoButton* pButton;
    for (int iB=0; iB < m_nNumButtons; iB++)
    {
        pButton = *(m_pAnswerButtons + iB);
        if (pButton)
            pButton->enable(fEnable);
    }
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::set_buttons(ImoButton* pButtons[], int nNumButtons)
{
    m_pAnswerButtons = pButtons;
    m_nNumButtons = nNumButtons;

    set_event_handlers();
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::set_button_color(int i, Color color)
{
    ImoButton* pButton = *(m_pAnswerButtons + i);
    if (pButton )   //&& pButton->IsEnabled())
        pButton->set_bg_color(color);
}

//---------------------------------------------------------------------------------------
bool ExerciseCtrol::check_success_or_failure(int nButton)
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
        ImoButton* pButton = *(m_pAnswerButtons + iB);
        if (pButton)
            m_pDoc->add_event_handler(pButton, k_on_click_event, this);
    }
}




//=======================================================================================
//// Implementation of CompareCtrol
////  A control with three answer buttons
//=======================================================================================
//
//IMPLEMENT_CLASS(CompareCtrol, ExerciseCtrol)
//
//static wxString m_sButtonLabel[CompareCtrol::k_num_buttons];
//
//BEGIN_EVENT_TABLE(CompareCtrol, ExerciseCtrol)
//    EVT_COMMAND_RANGE (m_ID_BUTTON, m_ID_BUTTON+k_num_buttons-1, wxEVT_COMMAND_BUTTON_CLICKED, ExerciseCtrol::OnRespButton)
//END_EVENT_TABLE()
//
//
////---------------------------------------------------------------------------------------
//CompareCtrol::CompareCtrol(wxWindow* parent, wxWindowID id,
//               ExerciseOptions* pConstrains, wxSize nDisplaySize,
//               const wxPoint& pos, const wxSize& size, int style)
//    : ExerciseCtrol(parent, id, pConstrains, nDisplaySize, pos, size, style )
//{
//}
//
////---------------------------------------------------------------------------------------
//void CompareCtrol::initialize_strings()
//{
//
//    //language dependent strings. Can not be statically initiallized because
//    //then they do not get translated
//    m_sButtonLabel[0] = _("First one greater");
//    m_sButtonLabel[1] = _("Second one greater");
//    m_sButtonLabel[2] = _("Both are equal");
//}
//
////---------------------------------------------------------------------------------------
//void CompareCtrol::create_answer_buttons(LUnits height, LUnits spacing)
//{
//
//    //create buttons for the answers: three buttons in one row
//    m_pKeyboardSizer = new wxFlexGridSizer(k_num_rows, k_num_cols, 2*nSpacing, 0);
//    m_pMainSizer->Add(
//        m_pKeyboardSizer,
//        wxSizerFlags(0).Left().Border(wxALIGN_LEFT|wxTOP, 2*nSpacing) );
//
//    // the buttons
//    for (int iB=0; iB < k_num_cols; iB++) {
//        m_pAnswerButton[iB] = new ImoButton( this, m_ID_BUTTON + iB, m_sButtonLabel[iB],
//            wxDefaultPosition, wxSize(38*nSpacing, nHeight));
//        m_pAnswerButton[iB]->SetFont(font);
//
//        m_pKeyboardSizer->Add(
//            m_pAnswerButton[iB],
//            wxSizerFlags(0).Border(wxLEFT|wxRIGHT, nSpacing) );
//    }
//
//    //inform base class about the settings
//    set_buttons(m_pAnswerButton, k_num_buttons, m_ID_BUTTON);
//
//
//    //====================================================================================
//    //Example of new code taken from IdfyIntervalsCtrol
//    ImoStyle* pDefStyle = m_pDoc->get_default_style();
//    ImoInlineWrapper* pBox;
//
//    //create 48 buttons for the answers: six rows, eight buttons per row,
//    //plus two additional buttons, for 'unison' and 'chromatic semitone'
//
//    ImoStyle* pBtStyle = m_pDoc->create_private_style();
//    pBtStyle->set_string_property(ImoStyle::k_font_name, "sans-serif");
//    pBtStyle->set_float_property(ImoStyle::k_font_size, 8.0f);
//
//    ImoStyle* pRowStyle = m_pDoc->create_private_style();
//    pRowStyle->set_lunits_property(ImoStyle::k_font_size, 10.0f);
//    pRowStyle->set_lunits_property(ImoStyle::k_margin_bottom, 0.0f);
//
//    USize buttonSize(1500.0f, height);
//    USize bigButtonSize(3200.0f, height);
//    LUnits firstRowWidth = 4000.0f;
//    LUnits otherRowsWidth = buttonSize.width + spacing;
//    LUnits unisonRowsWidth = bigButtonSize.width + 2.0f * spacing;
//
//
//    int iB;
//    for (iB=0; iB < k_num_buttons; iB++) {
//        m_pAnswerButton[iB] = NULL;
//    }
//
//    //row with buttons for unison and related
//    ImoParagraph* pUnisonRow = m_pDyn->add_paragraph(pRowStyle);
//
//        //spacer to skip the labels
//    pBox = pUnisonRow->add_inline_box(firstRowWidth, pDefStyle);
//
//        //unison button
//    pBox = pUnisonRow->add_inline_box(unisonRowsWidth, pDefStyle);
//    iB = lmIDX_UNISON;
//    m_pAnswerButton[iB] = pBox->add_button(m_sIntvButtonLabel[iB],
//                                           bigButtonSize, pBtStyle);
//
//        // "chromatic semitone" button
//    pBox = pUnisonRow->add_inline_box(unisonRowsWidth, pDefStyle);
//    iB = lmIDX_SEMITONE;
//    m_pAnswerButton[iB] = pBox->add_button(m_sIntvButtonLabel[iB],
//                                           bigButtonSize, pBtStyle);
//
//        // "chromatic tone" button
//    pBox = pUnisonRow->add_inline_box(unisonRowsWidth, pDefStyle);
//    iB = lmIDX_TONE;
//    m_pAnswerButton[iB] = pBox->add_button(m_sIntvButtonLabel[iB],
//                                           bigButtonSize, pBtStyle);
//
//
//    //Now main keyboard with all other buttons
//
//    //row with column labels
//    ImoParagraph* pKeyboardRow = m_pDyn->add_paragraph(pRowStyle);
//
//    //spacer
//    pBox = pKeyboardRow->add_inline_box(firstRowWidth, pDefStyle);
//
//    for (int iCol=0; iCol < k_num_cols; iCol++)
//    {
//        pBox = pKeyboardRow->add_inline_box(otherRowsWidth, pDefStyle);
//        m_pColumnLabel[iCol] = pBox->add_text_item(m_sIntvColumnLabel[iCol],
//                                                   pRowStyle);
//    }
//
//    //remaining rows with buttons
//    for (int iRow=0; iRow < k_num_rows; iRow++)
//    {
//        ImoParagraph* pKeyboardRow = m_pDyn->add_paragraph(pRowStyle);
//
//        pBox = pKeyboardRow->add_inline_box(firstRowWidth, pDefStyle);
//        m_pRowLabel[iRow] = pBox->add_text_item(m_sIntvRowLabel[iRow], pRowStyle);
//
//        // the buttons for this row
//        for (int iCol=0; iCol < k_num_cols; iCol++)
//        {
//            iB = iCol + iRow * k_num_cols;    // button index: 0 .. 47
//            pBox = pKeyboardRow->add_inline_box(otherRowsWidth, pDefStyle);
//            m_pAnswerButton[iB] = pBox->add_button(m_sIntvButtonLabel[iB],
//                                                   buttonSize, pBtStyle);
//
//            if (m_sIntvButtonLabel[iB].empty())
//            {
//                m_pAnswerButton[iB]->set_visible(false);
//                m_pAnswerButton[iB]->enable(false);
//            }
//        }
//    }
//
//    set_event_handlers();
//
//    //inform base class about the settings
//    set_buttons(m_pAnswerButton, k_num_buttons);
//}
//
////---------------------------------------------------------------------------------------
//CompareCtrol::~CompareCtrol()
//{
//}
//
//
//
//
//=======================================================================================
//// Implementation of CompareScoresCtrol
////  A CompareCtrol with two scores
//=======================================================================================
//
//IMPLEMENT_CLASS(CompareScoresCtrol, CompareCtrol)
//
//BEGIN_EVENT_TABLE(CompareScoresCtrol, CompareCtrol)
//    LM_EVT_END_OF_PLAY  (CompareScoresCtrol::OnEndOfPlay)
//    EVT_TIMER           (wxID_ANY, CompareScoresCtrol::OnTimerEvent)
//END_EVENT_TABLE()
//
////---------------------------------------------------------------------------------------
//CompareScoresCtrol::CompareScoresCtrol(wxWindow* parent, wxWindowID id,
//               ExerciseOptions* pConstrains, wxSize nDisplaySize,
//               const wxPoint& pos, const wxSize& size, int style)
//    : CompareCtrol(parent, id, pConstrains, nDisplaySize, pos, size, style )
//{
//    //initializations
//    m_pSolutionScore = (ImoScore*)NULL;
//    m_pScore[0] = (ImoScore*)NULL;
//    m_pScore[1] = (ImoScore*)NULL;
//    m_nPlayMM = 80;          //metronome default setting to play the scores
//
//}
//
////---------------------------------------------------------------------------------------
//CompareScoresCtrol::~CompareScoresCtrol()
//{
//    stop_sounds();
//    delete_scores();
//}
//
////---------------------------------------------------------------------------------------
//ImoBoxContainer* CompareScoresCtrol::create_problem_display_box()
//{
//    // Using scores and ScoreAuxCtrol
//    lmScoreAuxCtrol* pScoreCtrol = new lmScoreAuxCtrol(this, -1, (ImoScore*)NULL, wxDefaultPosition,
//                                        m_nDisplaySize, eSIMPLE_BORDER);
//    pScoreCtrol->SetMargins(lmToLogicalUnits(5, lmMILLIMETERS),     //left
//                            lmToLogicalUnits(5, lmMILLIMETERS),     //right
//                            lmToLogicalUnits(10, lmMILLIMETERS));   //top
//    pScoreCtrol->SetScale( pScoreCtrol->GetScale() * (float)m_rScale );
//    return pScoreCtrol;
//}
//
////---------------------------------------------------------------------------------------
//void CompareScoresCtrol::play()
//{
//    if (!m_fPlaying) {
//        // play button pressed
//
//        //change link from "play" to "Stop"
//        //m_pPlayButton->SetLabel(_("Stop"));
//        m_pPlayButton->set_alternative_label();
//        m_fPlaying = true;
//
//        //AWARE: The link label is restored to "play" when the EndOfPlay() event is
//        //       received. Flag m_fPlaying is also reset there
//
//        if (m_fQuestionAsked)
//        {
//            //Introducing the problem. play the first score
//            PlayScore(0);
//            //AWARE:
//            // when 1st score is finished an event will be generated. Then method
//            // OnEndOfPlay() will handle the event and play the second score.
//            // It is programmed this way (asynchonously) to avoid crashes if program/exercise
//            // is closed.
//        }
//        else
//        {
//            //Playing the solution. play total score
//            ((lmScoreAuxCtrol*)m_pDisplayCtrol)->PlayScore(lmVISUAL_TRACKING,
//                    k_no_countoff, ePM_NormalInstrument, m_nPlayMM);
//        }
//    }
//    else {
//        // "Stop" button pressed. Inform that no more sound is desired
//        m_fPlaying = false;
//    }
//
//}
//
////---------------------------------------------------------------------------------------
//void CompareScoresCtrol::PlayScore(int nIntv)
//{
//    m_nNowPlaying = nIntv;
//    Colors* pColors = m_appScope.get_colors();
//    m_pAnswerButton[nIntv]->SetBackgroundColour( pColors->ButtonHighlight() );
//    m_pAnswerButton[nIntv]->Update();    //Refresh works by events and, so, it is not inmediate
//
//    //AWARE: As the intervals are built using whole notes, we will play them at
//    // MM=320 so that real note rate will be 80.
//    m_pScore[nIntv]->play(lmNO_VISUAL_TRACKING, k_no_countoff,
//                             ePM_NormalInstrument, 320, this);
//
//}
//
////---------------------------------------------------------------------------------------
//void CompareScoresCtrol::OnEndOfPlay(lmEndOfPlayEvent& WXUNUSED(event))
//{
//    if (m_fQuestionAsked)
//    {
//        //if introducing the problem remove highlight in button
//    Colors* pColors = m_appScope.get_colors();
//        m_pAnswerButton[m_nNowPlaying]->SetBackgroundColour( pColors->Normal() );
//        m_pAnswerButton[m_nNowPlaying]->Update();
//
//        if (m_nNowPlaying == 0 && m_fPlaying) {
//            //wxLogMessage(_T("EndOfPlay event: Starting timer"));
//            m_oPauseTimer.SetOwner( this, wxID_ANY );
//            m_oPauseTimer.Start(1000, wxTIMER_CONTINUOUS );     //wait for 1sec (1000ms)
//        }
//        else {
//            //wxLogMessage(_T("EndOfPlay event: play stopped"));
//            m_fPlaying = false;
//            //m_pPlayButton->SetLabel(_("play"));
//            m_pPlayButton->set_normal_label();
//        }
//    }
//    else
//    {
//        //playing after solution is displayed: just change link label
//        m_fPlaying = false;
//        //m_pPlayButton->SetLabel(_("play"));
//        m_pPlayButton->set_normal_label();
//    }
//
//}
//
////---------------------------------------------------------------------------------------
//void CompareScoresCtrol::OnTimerEvent(wxTimerEvent& WXUNUSED(event))
//{
//    m_oPauseTimer.Stop();
//    if (m_fPlaying) {
//        //wxLogMessage(_T("Timer event: play(1)"));
//        PlayScore(1);
//    }
//    else {
//        //wxLogMessage(_T("Timer event: play stopped"));
//        //m_pPlayButton->SetLabel(_("play"));
//        m_pPlayButton->set_normal_label();
//    }
//}
//
////---------------------------------------------------------------------------------------
//void CompareScoresCtrol::display_solution()
//{
//    //show the solution score
//	if (m_pSolutionScore) {
//		//There must be a solution score. Display it
//		((lmScoreAuxCtrol*)m_pDisplayCtrol)->SetScore(m_pSolutionScore);
//	}
//    ((lmScoreAuxCtrol*)m_pDisplayCtrol)->display_message(
//                    m_sAnswer, lmToLogicalUnits(5, lmMILLIMETERS), false);
//}
//
////---------------------------------------------------------------------------------------
//void CompareScoresCtrol::display_problem()
//{
//    if (m_pBaseConstrains->is_theory_mode()) {
//        //TODO
//        ////theory
//        //m_pDisplayCtrol->DisplayScore(m_pProblemScore);
//    }
//    else {
//        //ear training
//        m_pPlayButton->set_alternative_label();
//        play();
//    }
//}
//
////---------------------------------------------------------------------------------------
//void CompareScoresCtrol::delete_scores()
//{
//    if (m_pSolutionScore) {
//        delete m_pSolutionScore;
//        m_pSolutionScore = (ImoScore*)NULL;
//    }
//    if (m_pScore[0]) {
//        delete m_pScore[0];
//        m_pScore[0] = (ImoScore*)NULL;
//    }
//    if (m_pScore[1]) {
//        delete m_pScore[1];
//        m_pScore[1] = (ImoScore*)NULL;
//    }
//}
//
////---------------------------------------------------------------------------------------
//void CompareScoresCtrol::stop_sounds()
//{
//    //Stop any possible score being played to avoid crashes
//    if (m_pScore[0]) m_pScore[0]->Stop();
//    if (m_pScore[1]) m_pScore[1]->Stop();
//    if (m_pSolutionScore) m_pSolutionScore->Stop();
//
//}
//
////---------------------------------------------------------------------------------------
//void CompareScoresCtrol::on_debug_show_source_score()
//{
//    ((lmScoreAuxCtrol*)m_pDisplayCtrol)->SourceLDP(false);  //false: do not export undo data
//}
//
////---------------------------------------------------------------------------------------
//void CompareScoresCtrol::on_debug_dump_score()
//{
//    ((lmScoreAuxCtrol*)m_pDisplayCtrol)->Dump();
//}
//
////---------------------------------------------------------------------------------------
//void CompareScoresCtrol::on_debug_show_midi_events()
//{
//    ((lmScoreAuxCtrol*)m_pDisplayCtrol)->DumpMidiEvents();
//}
//
////---------------------------------------------------------------------------------------
//void CompareScoresCtrol::display_message(const wxString& sMsg, bool fClearDisplay)
//{
//    ((lmScoreAuxCtrol*)m_pDisplayCtrol)->display_message(
//            sMsg, lmToLogicalUnits(5, lmMILLIMETERS), fClearDisplay);
//}
//
//


//=======================================================================================
// Implementation of OneScoreCtrol
//  An ExerciseCtrol with one score for the problem and one optional score for
//  the solution. If no solution score is defined the problem score is used as
//  solution.
//=======================================================================================
OneScoreCtrol::OneScoreCtrol(long dynId, ApplicationScope& appScope,
                             DocumentCanvas* pCanvas)
    : ExerciseCtrol(dynId, appScope, pCanvas)
    , m_pPlayer( m_appScope.get_score_player() )
    , m_pProblemScore(NULL)
	, m_pSolutionScore(NULL)
    , m_pAuxScore(NULL)
    , m_nPlayMM(320)    //it is assumed whole notes
{
}
//
//IMPLEMENT_CLASS(OneScoreCtrol, ExerciseCtrol)
//
//BEGIN_EVENT_TABLE(OneScoreCtrol, ExerciseCtrol)
//    LM_EVT_END_OF_PLAY  (OneScoreCtrol::OnEndOfPlay)
//END_EVENT_TABLE()
//
//
////---------------------------------------------------------------------------------------
//OneScoreCtrol::OneScoreCtrol(wxWindow* parent, wxWindowID id,
//               ExerciseOptions* pConstrains, wxSize nDisplaySize,
//               const wxPoint& pos, const wxSize& size, int style)
//    : ExerciseCtrol(parent, id, pConstrains, nDisplaySize, pos, size, style )
//{
//}

//---------------------------------------------------------------------------------------
OneScoreCtrol::~OneScoreCtrol()
{
    stop_sounds();
    delete_scores();
}

//---------------------------------------------------------------------------------------
void OneScoreCtrol::create_problem_display_box()
{
//    // Use a ScoreAuxCtrol
//    lmScoreAuxCtrol* pScoreCtrol = new lmScoreAuxCtrol(this, -1, (ImoScore*)NULL, wxDefaultPosition,
//                                        m_nDisplaySize, eSIMPLE_BORDER);
//    pScoreCtrol->SetMargins(lmToLogicalUnits(5, lmMILLIMETERS),     //left
//                            lmToLogicalUnits(5, lmMILLIMETERS),     //right
//                            lmToLogicalUnits(10, lmMILLIMETERS));   //top

    ImoStyle* displayStyle = m_pDoc->create_private_style("Default style");
    displayStyle->set_border_width_property(15.0f);
    displayStyle->set_lunits_property(ImoStyle::k_padding_top, 1000.0f);  //10 millimeters
    displayStyle->set_lunits_property(ImoStyle::k_padding_bottom, 1000.0f);  //10 millimeters
    displayStyle->set_lunits_property(ImoStyle::k_padding_left, 1000.0f);  //10 millimeters
    displayStyle->set_lunits_property(ImoStyle::k_margin_bottom,
                                      displayStyle->em_to_LUnits(2.0f) );  //2em

    m_pDisplayCtrol = m_pDyn->add_content_wrapper(displayStyle);
    m_pCurScore = NULL;
    m_pCurPara = NULL;
}

//---------------------------------------------------------------------------------------
void OneScoreCtrol::play()
{
    do_play(k_no_countoff);
}

//---------------------------------------------------------------------------------------
void OneScoreCtrol::do_play(bool fCountOff)
{
    if (!m_pPlayer->is_playing())
    {
        // play button pressed

        //change link from "play" to "Stop playing" label
        m_pPlayButton->set_alternative_label();

        //play the score
        m_pPlayer->prepare_to_play(m_pCurScore);

        bool fVisualTracking = true;
        int playMode = k_play_normal_instrument;
        Interactor* pInteractor = m_pCanvas ? m_pCanvas->get_interactor() : NULL;

        m_pPlayer->play(fVisualTracking, fCountOff, playMode, m_nPlayMM, pInteractor);

        //! AWARE The link label is restored to "play" when the EndOfPlay event is
        //! received.
    }
    else
    {
        // "Stop" button pressed
        m_pPlayer->stop();
    }
}

////---------------------------------------------------------------------------------------
//void OneScoreCtrol::OnEndOfPlay(lmEndOfPlayEvent& WXUNUSED(event))
//{
//    //m_pPlayButton->SetLabel(_("play"));
//    m_pPlayButton->set_normal_label();
//}
//
////---------------------------------------------------------------------------------------
//void OneScoreCtrol::PlaySpecificSound(int nButton)
//{
//    stop_sounds();
//
//    //delete any previous score
//    if (m_pAuxScore) {
//        delete m_pAuxScore;
//        m_pAuxScore = (ImoScore*)NULL;
//    }
//
//    //prepare the score with the requested sound and play it
//    prepare_aux_score(nButton);
//    if (m_pAuxScore) {
//        m_pAuxScore->play(lmNO_VISUAL_TRACKING, k_no_countoff,
//                            ePM_NormalInstrument, m_nPlayMM, (wxWindow*) NULL);
//    }
//}

//---------------------------------------------------------------------------------------
void OneScoreCtrol::display_solution()
{
//    //show the score
//	if (m_pSolutionScore) {
//		//There is a solution score. Display it
//		((lmScoreAuxCtrol*)m_pDisplayCtrol)->SetScore(m_pSolutionScore);
//	}
//	else {
//		//No solution score. Display problem score
//		((lmScoreAuxCtrol*)m_pDisplayCtrol)->HideScore(false);
//	}
//    ((lmScoreAuxCtrol*)m_pDisplayCtrol)->display_message(m_sAnswer, lmToLogicalUnits(5, lmMILLIMETERS), false);
    display_message(m_sAnswer, false);
}

//---------------------------------------------------------------------------------------
void OneScoreCtrol::display_problem()
{
    if (m_pDisplayCtrol)
    {
        if (m_pCurScore)
        {
            m_pDisplayCtrol->remove_child(m_pCurScore);
            delete m_pCurScore;
        }
        m_pDisplayCtrol->append_child(m_pProblemScore);
        m_pProblemScore->set_style( m_pDoc->get_default_style() );
        m_pCurScore = m_pProblemScore;
        m_pProblemScore = NULL;
    }

//    //load the problem score into the control
//    ((lmScoreAuxCtrol*)m_pDisplayCtrol)->SetScore(m_pProblemScore, true);   //true: the score must be hidden
//    ((lmScoreAuxCtrol*)m_pDisplayCtrol)->display_message(_T(""), 0, true);   //true: clear the canvas
//
//    if (m_pBaseConstrains->is_theory_mode())
//    {
//        //theory
//        ((lmScoreAuxCtrol*)m_pDisplayCtrol)->DisplayScore(m_pProblemScore);
//    }
//    else {
//        //ear training
//        m_pPlayButton->set_alternative_label();
//        play();
//    }
}

//---------------------------------------------------------------------------------------
void OneScoreCtrol::delete_scores()
{
    delete m_pProblemScore;
    m_pProblemScore = NULL;

    delete m_pSolutionScore;
    m_pSolutionScore = NULL;

    delete m_pAuxScore;
    m_pAuxScore = NULL;
}

//---------------------------------------------------------------------------------------
void OneScoreCtrol::stop_sounds()
{
    m_pPlayer->stop();
}

//---------------------------------------------------------------------------------------
void OneScoreCtrol::on_debug_show_source_score()
{
    //TODO: 5.0 commented out
    //((lmScoreAuxCtrol*)m_pDisplayCtrol)->SourceLDP(false);  //false: do not export undo data
}

//---------------------------------------------------------------------------------------
void OneScoreCtrol::on_debug_dump_score()
{
    //TODO: 5.0 commented out
    //((lmScoreAuxCtrol*)m_pDisplayCtrol)->Dump();
}

//---------------------------------------------------------------------------------------
void OneScoreCtrol::on_debug_show_midi_events()
{
    //TODO: 5.0 commented out
    //((lmScoreAuxCtrol*)m_pDisplayCtrol)->DumpMidiEvents();
}

//---------------------------------------------------------------------------------------
void OneScoreCtrol::display_message(const wxString& sMsg, bool fClearDisplay)
{
    if (m_pDisplayCtrol)
    {
        if (m_pCurPara)
        {
            m_pDisplayCtrol->remove_item(m_pCurPara);
            delete m_pCurPara;
        }

        ImoStyle* pDefStyle = m_pDoc->get_default_style();
        ImoParagraph* pPara = m_pDisplayCtrol->add_paragraph(pDefStyle);
        pPara->add_text_item( to_std_string(sMsg), pDefStyle );
        m_pCurPara = pPara;
    }

//    ((lmScoreAuxCtrol*)m_pDisplayCtrol)->display_message(
//            sMsg, lmToLogicalUnits(5, lmMILLIMETERS), fClearDisplay);
}


//=======================================================================================
//// Implementation of CompareMidiCtrol
////  An ExerciseCtrol without scores. It uses MIDI pitches for the problem and
////  the solution.
//=======================================================================================
//
//IMPLEMENT_CLASS(CompareMidiCtrol, CompareCtrol)
//
//BEGIN_EVENT_TABLE(CompareMidiCtrol, CompareCtrol)
//    EVT_TIMER           (wxID_ANY, CompareMidiCtrol::OnTimerEvent)
//END_EVENT_TABLE()
//
////---------------------------------------------------------------------------------------
//CompareMidiCtrol::CompareMidiCtrol(wxWindow* parent, wxWindowID id,
//               ExerciseOptions* pConstrains, wxSize nDisplaySize,
//               const wxPoint& pos, const wxSize& size, int style)
//    : CompareCtrol(parent, id, pConstrains, nDisplaySize, pos, size, style )
//{
//    //initializations
//    m_oTimer.SetOwner( this, wxID_ANY );    //needed to receive the timer events
//
//    m_mpPitch[0] = m_mpPitch[1] = -1;
//    m_nTimeIntval[0] = 500;     //500 ms between first and second pitch
//    m_nTimeIntval[1] = 2000;    //stop all sounds after 2s from start of second pitch
//    m_fStopPrev = false;        //do not stop first sound when sounding the second pitch
//    m_nNowPlaying = -1;
//
//    //default channels and instruments
//    m_nChannel[0] = m_nChannel[1] = g_pMidi->DefaultVoiceChannel();
//    m_nInstr[0] = m_nInstr[1] = g_pMidi->DefaultVoiceInstr();
//
//}
//
////---------------------------------------------------------------------------------------
//CompareMidiCtrol::~CompareMidiCtrol()
//{
//    stop_sounds();
//}
//
////---------------------------------------------------------------------------------------
//ImoBoxContainer* CompareMidiCtrol::create_problem_display_box()
//{
//    // Using MIDI Pitches and Static Text box
//    return new wxStaticText(this, -1, _T(""), wxDefaultPosition,
//                          m_nDisplaySize,
//                          wxBORDER_SIMPLE | wxST_NO_AUTORESIZE );
//
//}
//
////---------------------------------------------------------------------------------------
//void CompareMidiCtrol::play()
//{
//    //wxLogMessage(_T("[CompareMidiCtrol::play] m_nNowPlaying=%d"), m_nNowPlaying);
//    if (m_nNowPlaying == -1)
//    {
//        // Starting to play
//
//        //change link from "play" to "Stop"
//        //m_pPlayButton->SetLabel(_("Stop"));
//        m_pPlayButton->set_alternative_label();
//
//        //AWARE: The link label is restored to "play" when the OnTimerEvent() event is
//        //       received.
//
//        if (m_fQuestionAsked)
//        {
//            //Introducing the problem. play the first sound
//            PlaySound(0);
//            //AWARE: method OnTimerEvent() will handle the event and play the
//            //next sound.
//        }
//    }
//    else {
//        // "Stop" button pressed
//        m_oTimer.Stop();
//        m_nNowPlaying = -1;
//        //m_pPlayButton->SetLabel(_("play"));
//        m_pPlayButton->set_normal_label();
//        stop_sounds();
//    }
//
//}
//
////---------------------------------------------------------------------------------------
//void CompareMidiCtrol::PlaySound(int iSound)
//{
//    //wxLogMessage(_T("[CompareMidiCtrol::PlaySound] iSound=%d"), iSound);
//    m_nNowPlaying = iSound;
//    g_pMidiOut->NoteOn(m_nChannel[iSound], m_mpPitch[iSound], 127);
//    m_oTimer.Start(m_nTimeIntval[iSound], wxTIMER_ONE_SHOT);
//}
//
////---------------------------------------------------------------------------------------
//void CompareMidiCtrol::display_solution()
//{
//    display_message(m_sAnswer, true);
//}
//
//
////---------------------------------------------------------------------------------------
//void CompareMidiCtrol::display_problem()
//{
//    m_pPlayButton->set_alternative_label();
//    play();
//}
//
////---------------------------------------------------------------------------------------
//void CompareMidiCtrol::stop_sounds()
//{
//    g_pMidiOut->AllSoundsOff();
//}
//
////---------------------------------------------------------------------------------------
//void CompareMidiCtrol::display_message(const wxString& sMsg, bool fClearDisplay)
//{
//    ((wxStaticText*)m_pDisplayCtrol)->SetLabel(sMsg);
//}
//
////---------------------------------------------------------------------------------------
//void CompareMidiCtrol::OnTimerEvent(wxTimerEvent& WXUNUSED(event))
//{
//    //wxLogMessage(_T("[CompareMidiCtrol::OnTimerEvent] m_nNowPlaying=%d"), m_nNowPlaying);
//    m_oTimer.Stop();
//    if (m_nNowPlaying == -1) return;
//
//    if (m_nNowPlaying == 0)
//    {
//        //play next sound
//        //wxLogMessage(_T("Timer event: play(1)"));
//        if (m_fStopPrev)
//            g_pMidiOut->NoteOff(m_nChannel[m_nNowPlaying], m_mpPitch[m_nNowPlaying], 127);
//        PlaySound(++m_nNowPlaying);
//    }
//    else
//    {
//        //wxLogMessage(_T("Timer event: play stopped"));
//        m_nNowPlaying = -1;
//        stop_sounds();
//        //m_pPlayButton->SetLabel(_("play"));
//        m_pPlayButton->set_normal_label();
//    }
//}
//
//
//
//=======================================================================================
//// Implementation of FullEditorExercise:
//=======================================================================================
//
//
//BEGIN_EVENT_TABLE(FullEditorExercise, wxWindow)
//    EVT_SIZE            (FullEditorExercise::OnSize)
//    LM_EVT_URL_CLICK    (ID_LINK_SETTINGS, FullEditorExercise::on_settings_button)
//    LM_EVT_URL_CLICK    (ID_LINK_GO_BACK, FullEditorExercise::on_go_back)
//    LM_EVT_URL_CLICK    (ID_LINK_NEW_PROBLEM, FullEditorExercise::on_new_problem)
//
//END_EVENT_TABLE()
//
//IMPLEMENT_CLASS(FullEditorExercise, wxWindow)
//
////---------------------------------------------------------------------------------------
//FullEditorExercise::FullEditorExercise(wxWindow* parent, wxWindowID id,
//                           ExerciseOptions* pConstrains,
//                           const wxPoint& pos, const wxSize& size, int style)
//    : wxWindow(parent, id, pos, size, style )
//    , m_pBaseConstrains(pConstrains)
//    , m_pProblemScore((ImoScore*)NULL)
//    , m_rScale(1.0)
//{
//    //initializations
//    SetBackgroundColour(*wxWHITE);
//}
//
////---------------------------------------------------------------------------------------
//FullEditorExercise::~FullEditorExercise()
//{
//    //AWARE: score ownership is transferred to the Score Editor window. It MUST NOT be deleted.
//
//    //delete objects
//    if (m_pBaseConstrains)
//        delete m_pBaseConstrains;
//}
//
////---------------------------------------------------------------------------------------
//void FullEditorExercise::on_settings_button(EventInfo* pEvent)
//{
//    wxDialog* pDlg = get_settings_dialog();
//    if (pDlg) {
//        int retcode = pDlg->ShowModal();
//        if (retcode == wxID_OK) {
//            m_pBaseConstrains->save_settings();
//            // When changing settings it could be necessary to review answer buttons
//            // or other issues. Give derived classes a chance to do it.
//            on_settings_changed();
//        }
//        delete pDlg;
//    }
//
//}
//
////---------------------------------------------------------------------------------------
//void FullEditorExercise::on_go_back()
//{
//    lmMainFrame* pFrame = GetMainFrame();
//    TextBookController* pBookController = pFrame->GetBookController();
//    pBookController->Display( m_pBaseConstrains->GetGoBackURL() );
//}
//
////---------------------------------------------------------------------------------------
//void FullEditorExercise::OnSize(wxSizeEvent& event)
//{
//    Layout();
//}
//
////---------------------------------------------------------------------------------------
//void FullEditorExercise::on_new_problem(EventInfo* pEvent)
//{
//    set_new_problem();
//    lmMainFrame* pMainFrame = GetMainFrame();
//    lmEditorMode* pEditMode = CreateEditMode();        //editor mode for the exercise
//    pMainFrame->NewScoreWindow(pEditMode, m_pProblemScore);
//}
//
////---------------------------------------------------------------------------------------
//void FullEditorExercise::create_controls()
//{
//    //language dependent strings. Can not be statically initiallized because
//    //then they do not get translated
//    initialize_strings();
//
//    // ensure that sizes are properly scaled
//    m_rScale = g_pMainFrame->GetHtmlWindow()->GetScale();
//
//    //the window contains just a sizer to add links
//    m_pMainSizer = new wxBoxSizer( wxVERTICAL );
//
//    // settings link
//    if (m_pBaseConstrains->IncludeSettingsLink()) {
//        UrlAuxCtrol* pSettingsLink = new UrlAuxCtrol(this, ID_LINK_SETTINGS, m_rScale,
//             _("Exercise options"), _T("link_settings"));
//        m_pMainSizer->Add(pSettingsLink, wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 5) );
//    }
//    // "Go back to theory" link
//    if (m_pBaseConstrains->IncludeGoBackLink()) {
//        UrlAuxCtrol* pGoBackLink = new UrlAuxCtrol(this, ID_LINK_GO_BACK, m_rScale,
//            _("Go back to theory"), _T("link_back"));
//        m_pMainSizer->Add(pGoBackLink, wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 5) );
//    }
//
//    // "new problem" button
//    m_pMainSizer->Add(
//        new UrlAuxCtrol(this, ID_LINK_NEW_PROBLEM, m_rScale, _("New problem"),
//                          _T("link_new")),
//        wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 5) );
//
//    //finish creation
//    SetSizer( m_pMainSizer );                 // use the sizer for window layout
//    m_pMainSizer->SetSizeHints( this );       // set size hints to honour minimum size
//}


}   //namespace lenmus
