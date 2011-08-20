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

#include "lenmus_exercise_ctrol.h"

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

//lenmus
//#include "auxctrols/UrlAuxCtrol.h"
#include "lenmus_constrains.h"
#include "lenmus_generators.h"
#include "lenmus_string.h"
#include "lenmus_score_canvas.h"
#include "lenmus_injectors.h"
#include "lenmus_colors.h"

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
    , m_pConstrains(NULL)
    , m_pDyn(NULL)
    , m_pDoc(NULL)
//    , m_fDoCountOff(true)
//    , m_fControlsCreated(false)
//    , m_rScale(1.0)
//    , m_pPlayButton((lmUrlAuxCtrol*)NULL)
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
    create_initial_layout();
}

//---------------------------------------------------------------------------------------
void EBookCtrol::handle_event(EventInfo* pEvent)
{
//    LM_EVT_URL_CLICK    (ID_LINK_SEE_SOURCE, EBookCtrol::OnDebugShowSourceScore)
//    LM_EVT_URL_CLICK    (ID_LINK_DUMP, EBookCtrol::OnDebugDumpScore)
//    LM_EVT_URL_CLICK    (ID_LINK_MIDI_EVENTS, EBookCtrol::OnDebugShowMidiEvents)
//+    LM_EVT_URL_CLICK    (ID_LINK_PLAY, EBookCtrol::on_play)
//+    LM_EVT_URL_CLICK    (ID_LINK_SETTINGS, EBookCtrol::on_settings_button)
//    LM_EVT_URL_CLICK    (ID_LINK_GO_BACK, EBookCtrol::OnGoBackButton)
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
            {
                on_play();
                delete pEvent;
                return;
            }
            else if (url == "link_settings")
            {
                on_settings_button();
                delete pEvent;
                return;
            }
            else
            {
                wxString msg = wxString::Format(_T("[ExerciseCtrol::handle_event] ")
                                                _T("url = '%s'")
                                                , to_wx_string(url).c_str() );
                wxMessageBox(msg);
            }
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
            m_pConstrains->save_settings();

            // When changing settings it is necessary to review answer buttons
            // or other issues. Give derived classes a chance to do it.
            on_settings_changed();
            m_pDoc->notify_that_document_has_been_modified();
        }
        delete pDlg;
    }
}

////---------------------------------------------------------------------------------------
//void EBookCtrol::OnGoBackButton(EventInfo* pEvent)
//{
//    lmMainFrame* pFrame = GetMainFrame();
//    TextBookController* pBookController = pFrame->GetBookController();
//    pBookController->Display( m_pOptions->GetGoBackURL() );
//}

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
//// Implementation of ExerciseCtrol:
//=======================================================================================
ExerciseCtrol::ExerciseCtrol(long dynId, ApplicationScope& appScope, DocumentCanvas* pCanvas)
    : EBookCtrol(dynId, appScope, pCanvas)
    , m_pDisplayCtrol(NULL)
    , m_pCurPara(NULL)
    , m_pCurScore(NULL)
//    , m_nDisplaySize(nDisplaySize)
//    , m_pConstrains(pConstrains)
    , m_fQuestionAsked(false)
    , m_nRespAltIndex(-1)
    , m_nNumButtons(0)
//    , m_pProblemManager((lmProblemManager*)NULL)
//    , m_sKeyPrefix(_T(""))
//    , m_pCboMode((wxChoice*)NULL)
//    , m_pCounters((lmCountersAuxCtrol*)NULL)
//    , m_fCountersValid(false)
//    , m_pShowSolution((lmUrlAuxCtrol*)NULL)
//    , m_pNewProblem(NULL)
//    , m_pAuxCtrolSizer((wxBoxSizer*)NULL)
//    , m_nGenerationMode( pConstrains->GetGenerationMode() )
{
//    //initializations
//    SetBackgroundColour(*wxWHITE);
}

//---------------------------------------------------------------------------------------
ExerciseCtrol::~ExerciseCtrol()
{
    //    delete m_pProblemManager;
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::create_controls()
{
    ExerciseOptions* pConstrains
        = dynamic_cast<ExerciseOptions*>( m_pConstrains );

    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated
    initialize_strings();

    ImoStyle* pDefStyle = m_pDoc->find_style("Default style");

    ImoStyle* pParaStyle = m_pDoc->create_private_style("Default style");
    pParaStyle->set_lunits_property(ImoStyle::k_margin_top, 500.0f);
    pParaStyle->set_lunits_property(ImoStyle::k_margin_bottom, 1000.0f);

    ImoStyle* pLinkStyle = m_pDoc->create_private_style("Default style");
    pLinkStyle->set_color_property(ImoStyle::k_color, Color(0,0,255) );
    pLinkStyle->set_int_property(ImoStyle::k_text_decoration,
                                   ImoTextStyle::k_decoration_underline);

//    //Create the problem manager and the problem space
//    CreateProblemManager();

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
    if (pConstrains->IncludeSettingsLink()
        || pConstrains->IncludeGoBackLink()
        || m_appScope.show_debug_links()
       )
    {
        ImoParagraph* pTopLinePara = m_pDyn->add_paragraph(pParaStyle);

        // settings link
        if (pConstrains->IncludeSettingsLink())
        {
            ImoLink* pLink = pTopLinePara->add_link("link_settings", pLinkStyle);
            pLink->add_text_item("Exercise options", pLinkStyle);
            m_pDoc->add_event_handler(pLink, k_on_click_event, this);
        }

        // "Go back to theory" link
        if (pConstrains->IncludeGoBackLink())
        {
            ImoLink* pLink = pTopLinePara->add_link("link_back", pLinkStyle);
            pLink->add_text_item("Go back to theory", pLinkStyle);
            m_pDoc->add_event_handler(pLink, k_on_click_event, this);
        }
    }

//    // debug links
//    if (m_appScope.show_debug_links())
//    {
//        // "See source score"
////        ImoLink* pLink = pTopLinePara->add_link("link_settings", pLinkStyle);
////        pLink->add_text_item("Exercise options", pLinkStyle);
////        m_pDoc->add_event_handler(pLink, k_on_click_event, this);
//
//        pTopLineSizer->Add(
//            new lmUrlAuxCtrol(this, ID_LINK_SEE_SOURCE, m_rScale, _("See source score"),
//                              lmNO_BITMAP),
//            wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 2*nSpacing) );
//
//        // "Dump score"
//        pTopLineSizer->Add(
//            new lmUrlAuxCtrol(this, ID_LINK_DUMP, m_rScale, _("Dump score"), lmNO_BITMAP),
//            wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 2*nSpacing) );
//
//        // "See MIDI events"
//        pTopLineSizer->Add(
//            new lmUrlAuxCtrol(this, ID_LINK_MIDI_EVENTS, m_rScale, _("See MIDI events"),
//                              lmNO_BITMAP),
//            wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 2*nSpacing) );
//    }


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
//	    ChangeGenerationModeLabel( m_nGenerationMode );
//
//	    pModeSizer->Add( m_pCboMode, 1, wxALL|wxALIGN_CENTER_VERTICAL, nSpacing);
//
//	    pCountersSizer->Add( pModeSizer, 0, wxEXPAND, nSpacing);
//
//	    m_pAuxCtrolSizer = new wxBoxSizer( wxVERTICAL );
//
//        m_pCounters = CreateCountersCtrol();
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
    LUnits linksWidth = 4000.0f;
    ImoInlineWrapper* pBox = pLinksPara->add_inline_box(linksWidth, pDefStyle);
    ImoLink* pLink = pBox->add_link("link_new", pLinkStyle);
    pLink->add_text_item("New problem", pLinkStyle);
    m_pDoc->add_event_handler(pLink, k_on_click_event, this);


    // "play" button
    if (pConstrains->IncludePlayLink())
    {
//        m_pPlayButton = new lmUrlAuxCtrol(this, ID_LINK_PLAY, m_rScale,
//                                          _("play"), _T("link_play"),
//                                          _("Stop playing"), _T("link_stop") );
        pBox = pLinksPara->add_inline_box(linksWidth, pDefStyle);
        ImoLink* pLink = pBox->add_link("link_play", pLinkStyle);
        pLink->add_text_item("Play", pLinkStyle);
        m_pDoc->add_event_handler(pLink, k_on_click_event, this);
    }

    // "show solution" button
    if (pConstrains->IncludeSolutionLink())
    {
        pBox = pLinksPara->add_inline_box(linksWidth, pDefStyle);
        ImoLink* pLink = pBox->add_link("link_solution", pLinkStyle);
        pLink->add_text_item("Show solution", pLinkStyle);
        m_pDoc->add_event_handler(pLink, k_on_click_event, this);
    }

    create_answer_buttons(nButtonsHeight, nSpacing);

    //finish creation

    //show start message
    wxString sMsg = _("Click on 'New problem' to start");
    display_message(sMsg, true);

//    // final buttons/links enable/setup
//    if (m_pPlayButton) m_pPlayButton->Enable(false);
//    if (m_pShowSolution) m_pShowSolution->Enable(false);
//
//    on_settings_changed();     //reconfigure buttons in accordance with constraints
//
//    m_fControlsCreated = true;
}

////---------------------------------------------------------------------------------------
//void ExerciseCtrol::ChangeGenerationMode(int nMode)
//{
//    m_nGenerationMode = nMode;          //set new generation mode
//    m_fCountersValid = false;
//    CreateProblemManager();             //change problem manager
//    ChangeCountersCtrol();              //replace statistics control
//}
//
////---------------------------------------------------------------------------------------
//void ExerciseCtrol::ChangeGenerationModeLabel(int nMode)
//{
//    m_nGenerationMode = nMode;
//    if (m_pCboMode)
//        m_pCboMode->SetStringSelection( get_generation_mode_name(m_nGenerationMode) );
//}
//
////---------------------------------------------------------------------------------------
//void ExerciseCtrol::ChangeCountersCtrol()
//{
//    //replace current control if exists
//    if (m_fControlsCreated)
//    {
//        lmCountersAuxCtrol* pNewCtrol = CreateCountersCtrol();
//        m_pAuxCtrolSizer->Replace(m_pCounters, pNewCtrol);
//        delete m_pCounters;
//        m_pCounters = pNewCtrol;
//        m_pMainSizer->Layout();
//        m_pCounters->UpdateDisplay();
//    }
//}
//
////---------------------------------------------------------------------------------------
//void ExerciseCtrol::CreateProblemManager()
//{
//    if (m_pProblemManager)
//        delete m_pProblemManager;
//
//    switch(m_nGenerationMode)
//    {
//        case lm_eQuizMode:
//        case lm_eExamMode:
//            m_pProblemManager = new lmQuizManager(this);
//            break;
//
//        case lm_eLearningMode:
//            m_pProblemManager = new lmLeitnerManager(this, true);
//            break;
//
//        case lm_ePractiseMode:
//            m_pProblemManager = new lmLeitnerManager(this, false);
//            break;
//
//        default:
//            wxASSERT(false);
//    }
//
//    SetProblemSpace();
//}
//
////---------------------------------------------------------------------------------------
//void ExerciseCtrol::SetProblemSpace()
//{
//}
//
////---------------------------------------------------------------------------------------
//lmCountersAuxCtrol* ExerciseCtrol::CreateCountersCtrol()
//{
//    lmCountersAuxCtrol* pNewCtrol = (lmCountersAuxCtrol*)NULL;
//    if (m_pConstrains->IsUsingCounters() )
//    {
//        switch(m_nGenerationMode)
//        {
//            case lm_eQuizMode:
//                pNewCtrol = new lmQuizAuxCtrol(this, wxID_ANY, 2, m_rScale,
//                                              (lmQuizManager*)m_pProblemManager);
//                break;
//
//            case lm_eExamMode:
//                pNewCtrol = new lmQuizAuxCtrol(this, wxID_ANY, 1, m_rScale,
//                                              (lmQuizManager*)m_pProblemManager);
//                break;
//
//            case lm_eLearningMode:
//                if (((lmLeitnerManager*)m_pProblemManager)->IsLearningMode())
//                    pNewCtrol = new lmLeitnerAuxCtrol(this, wxID_ANY, m_rScale,
//                                                 (lmLeitnerManager*)m_pProblemManager);
//                else
//                    pNewCtrol = new lmPractiseAuxCtrol(this, wxID_ANY, m_rScale,
//                                                 (lmLeitnerManager*)m_pProblemManager);
//                break;
//
//            case lm_ePractiseMode:
//                pNewCtrol = new lmPractiseAuxCtrol(this, wxID_ANY, m_rScale,
//                                                (lmLeitnerManager*)m_pProblemManager);
//                break;
//
//            default:
//                wxASSERT(false);
//        }
//    }
//    m_fCountersValid = true;
//    return pNewCtrol;
//}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::handle_event(EventInfo* pEvent)
{
//+    LM_EVT_URL_CLICK    (ID_LINK_NEW_PROBLEM, ExerciseCtrol::on_new_problem)
//+    LM_EVT_URL_CLICK    (ID_LINK_SOLUTION, ExerciseCtrol::on_display_solution)
//    EVT_CHOICE          (lmID_CBO_MODE, ExerciseCtrol::OnModeChanged)

    if (pEvent->is_on_click_event())
    {
        EventOnClick* pEv = dynamic_cast<EventOnClick*>(pEvent);
        ImoObj* pImo = pEv->get_originator_imo();
        if (pImo->is_link() ) //&& id >= ID_BUTTON && id < ID_BUTTON+k_num_buttons)
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
//        this->ChangeGenerationMode(nMode);
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
//    if (m_pPlayButton) m_pPlayButton->enable(true);
//    if (m_pShowSolution) m_pShowSolution->enable(true);
//
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

//    if (m_pPlayButton) m_pPlayButton->Enable(true);
//    if (m_pShowSolution) m_pShowSolution->Enable(false);
    m_fQuestionAsked = false;
//    if (!m_pConstrains->ButtonsEnabledAfterSolution()) enable_buttons(false);
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
            pButton->enabled(fEnable);
    }
}

//---------------------------------------------------------------------------------------
void ExerciseCtrol::set_buttons(ImoButton* pButtons[], int nNumButtons)
{
    m_pAnswerButtons = pButtons;
    m_nNumButtons = nNumButtons;
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




//=======================================================================================
//// Implementation of CompareCtrol
////  A control with three answer buttons
//=======================================================================================
//
//IMPLEMENT_CLASS(CompareCtrol, ExerciseCtrol)
//
//static wxString m_sButtonLabel[CompareCtrol::m_NUM_BUTTONS];
//
//BEGIN_EVENT_TABLE(CompareCtrol, ExerciseCtrol)
//    EVT_COMMAND_RANGE (m_ID_BUTTON, m_ID_BUTTON+m_NUM_BUTTONS-1, wxEVT_COMMAND_BUTTON_CLICKED, ExerciseCtrol::OnRespButton)
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
//void CompareCtrol::create_answer_buttons(int nHeight, int nSpacing, wxFont& font)
//{
//
//    //create buttons for the answers: three buttons in one row
//    m_pKeyboardSizer = new wxFlexGridSizer(m_NUM_ROWS, m_NUM_COLS, 2*nSpacing, 0);
//    m_pMainSizer->Add(
//        m_pKeyboardSizer,
//        wxSizerFlags(0).Left().Border(wxALIGN_LEFT|wxTOP, 2*nSpacing) );
//
//    // the buttons
//    for (int iB=0; iB < m_NUM_COLS; iB++) {
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
//    set_buttons(m_pAnswerButton, m_NUM_BUTTONS, m_ID_BUTTON);
//
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
//// Implementation of lmCompareScoresCtrol
////  A CompareCtrol with two scores
//=======================================================================================
//
//IMPLEMENT_CLASS(lmCompareScoresCtrol, CompareCtrol)
//
//BEGIN_EVENT_TABLE(lmCompareScoresCtrol, CompareCtrol)
//    LM_EVT_END_OF_PLAY  (lmCompareScoresCtrol::OnEndOfPlay)
//    EVT_TIMER           (wxID_ANY, lmCompareScoresCtrol::OnTimerEvent)
//END_EVENT_TABLE()
//
////---------------------------------------------------------------------------------------
//lmCompareScoresCtrol::lmCompareScoresCtrol(wxWindow* parent, wxWindowID id,
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
//lmCompareScoresCtrol::~lmCompareScoresCtrol()
//{
//    stop_sounds();
//    delete_scores();
//}
//
////---------------------------------------------------------------------------------------
//ImoBoxContainer* lmCompareScoresCtrol::create_problem_display_box()
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
//void lmCompareScoresCtrol::play()
//{
//    if (!m_fPlaying) {
//        // play button pressed
//
//        //change link from "play" to "Stop"
//        //m_pPlayButton->SetLabel(_("Stop"));
//        m_pPlayButton->SetAlternativeLabel();
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
//void lmCompareScoresCtrol::PlayScore(int nIntv)
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
//void lmCompareScoresCtrol::OnEndOfPlay(lmEndOfPlayEvent& WXUNUSED(event))
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
//            m_pPlayButton->SetNormalLabel();
//        }
//    }
//    else
//    {
//        //playing after solution is displayed: just change link label
//        m_fPlaying = false;
//        //m_pPlayButton->SetLabel(_("play"));
//        m_pPlayButton->SetNormalLabel();
//    }
//
//}
//
////---------------------------------------------------------------------------------------
//void lmCompareScoresCtrol::OnTimerEvent(wxTimerEvent& WXUNUSED(event))
//{
//    m_oPauseTimer.Stop();
//    if (m_fPlaying) {
//        //wxLogMessage(_T("Timer event: play(1)"));
//        PlayScore(1);
//    }
//    else {
//        //wxLogMessage(_T("Timer event: play stopped"));
//        //m_pPlayButton->SetLabel(_("play"));
//        m_pPlayButton->SetNormalLabel();
//    }
//}
//
////---------------------------------------------------------------------------------------
//void lmCompareScoresCtrol::display_solution()
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
//void lmCompareScoresCtrol::display_problem()
//{
//    if (m_pConstrains->IsTheoryMode()) {
//        //TODO
//        ////theory
//        //m_pDisplayCtrol->DisplayScore(m_pProblemScore);
//    }
//    else {
//        //ear training
//        m_pPlayButton->SetAlternativeLabel();
//        play();
//    }
//}
//
////---------------------------------------------------------------------------------------
//void lmCompareScoresCtrol::delete_scores()
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
//void lmCompareScoresCtrol::stop_sounds()
//{
//    //Stop any possible score being played to avoid crashes
//    if (m_pScore[0]) m_pScore[0]->Stop();
//    if (m_pScore[1]) m_pScore[1]->Stop();
//    if (m_pSolutionScore) m_pSolutionScore->Stop();
//
//}
//
////---------------------------------------------------------------------------------------
//void lmCompareScoresCtrol::OnDebugShowSourceScore(EventInfo* pEvent)
//{
//    ((lmScoreAuxCtrol*)m_pDisplayCtrol)->SourceLDP(false);  //false: do not export undo data
//}
//
////---------------------------------------------------------------------------------------
//void lmCompareScoresCtrol::OnDebugDumpScore(EventInfo* pEvent)
//{
//    ((lmScoreAuxCtrol*)m_pDisplayCtrol)->Dump();
//}
//
////---------------------------------------------------------------------------------------
//void lmCompareScoresCtrol::OnDebugShowMidiEvents(EventInfo* pEvent)
//{
//    ((lmScoreAuxCtrol*)m_pDisplayCtrol)->DumpMidiEvents();
//}
//
////---------------------------------------------------------------------------------------
//void lmCompareScoresCtrol::display_message(const wxString& sMsg, bool fClearDisplay)
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

//        ////change link from "play" to "Stop"
//        //m_pPlayButton->SetLabel(_("Stop"));
//        //change link from "play" to "Stop playing" label
//        m_pPlayButton->SetAlternativeLabel();

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
//    m_pPlayButton->SetNormalLabel();
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
//    if (m_pConstrains->IsTheoryMode())
//    {
//        //theory
//        ((lmScoreAuxCtrol*)m_pDisplayCtrol)->DisplayScore(m_pProblemScore);
//    }
//    else {
//        //ear training
//        m_pPlayButton->SetAlternativeLabel();
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

////---------------------------------------------------------------------------------------
//void OneScoreCtrol::OnDebugShowSourceScore(EventInfo* pEvent)
//{
//    ((lmScoreAuxCtrol*)m_pDisplayCtrol)->SourceLDP(false);  //false: do not export undo data
//}
//
////---------------------------------------------------------------------------------------
//void OneScoreCtrol::OnDebugDumpScore(EventInfo* pEvent)
//{
//    ((lmScoreAuxCtrol*)m_pDisplayCtrol)->Dump();
//}
//
////---------------------------------------------------------------------------------------
//void OneScoreCtrol::OnDebugShowMidiEvents(EventInfo* pEvent)
//{
//    ((lmScoreAuxCtrol*)m_pDisplayCtrol)->DumpMidiEvents();
//}

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
//        m_pPlayButton->SetAlternativeLabel();
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
//        m_pPlayButton->SetNormalLabel();
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
//    m_pPlayButton->SetAlternativeLabel();
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
//        m_pPlayButton->SetNormalLabel();
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
//    LM_EVT_URL_CLICK    (ID_LINK_GO_BACK, FullEditorExercise::OnGoBackButton)
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
//    , m_pConstrains(pConstrains)
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
//    if (m_pConstrains)
//        delete m_pConstrains;
//}
//
////---------------------------------------------------------------------------------------
//void FullEditorExercise::on_settings_button(EventInfo* pEvent)
//{
//    wxDialog* pDlg = get_settings_dialog();
//    if (pDlg) {
//        int retcode = pDlg->ShowModal();
//        if (retcode == wxID_OK) {
//            m_pConstrains->save_settings();
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
//void FullEditorExercise::OnGoBackButton(EventInfo* pEvent)
//{
//    lmMainFrame* pFrame = GetMainFrame();
//    TextBookController* pBookController = pFrame->GetBookController();
//    pBookController->Display( m_pConstrains->GetGoBackURL() );
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
//    if (m_pConstrains->IncludeSettingsLink()) {
//        lmUrlAuxCtrol* pSettingsLink = new lmUrlAuxCtrol(this, ID_LINK_SETTINGS, m_rScale,
//             _("Exercise options"), _T("link_settings"));
//        m_pMainSizer->Add(pSettingsLink, wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 5) );
//    }
//    // "Go back to theory" link
//    if (m_pConstrains->IncludeGoBackLink()) {
//        lmUrlAuxCtrol* pGoBackLink = new lmUrlAuxCtrol(this, ID_LINK_GO_BACK, m_rScale,
//            _("Go back to theory"), _T("link_back"));
//        m_pMainSizer->Add(pGoBackLink, wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 5) );
//    }
//
//    // "new problem" button
//    m_pMainSizer->Add(
//        new lmUrlAuxCtrol(this, ID_LINK_NEW_PROBLEM, m_rScale, _("New problem"),
//                          _T("link_new")),
//        wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 5) );
//
//    //finish creation
//    SetSizer( m_pMainSizer );                 // use the sizer for window layout
//    m_pMainSizer->SetSizeHints( this );       // set size hints to honour minimum size
//}


}   //namespace lenmus
