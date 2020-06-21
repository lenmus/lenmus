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

//lenmus headers
#include "lenmus_counters_ctrol.h"
#include "lenmus_standard_header.h"

#include "lenmus_generators.h"
#include "lenmus_constrains.h"
#include "lenmus_exercise_ctrol.h"
#include "lenmus_art_provider.h"
#include "lenmus_injectors.h"
#include "lenmus_paths.h"
#include "lenmus_dlg_exercise_mode.h"
#include "lenmus_constrains.h"
#include "lenmus_dlg_debug.h"
#include "lenmus_wave_player.h"

//lomse
#include <lomse_shapes.h>
#include <lomse_image_reader.h>
using namespace lomse;


namespace lenmus
{

//=======================================================================================
// CountersCtrol implementation
// An abstract control to embed in html exercises to display statistics about
// user performance in doing the exercise
//=======================================================================================
CountersCtrol::CountersCtrol(ApplicationScope& appScope, Document* pDoc,
                             ExerciseCtrol* pOwner, ExerciseOptions* pConstrains)
    : Control(*(appScope.get_lomse().get_library_scope()), pDoc, nullptr)
    , m_appScope(appScope)
    , m_pOwner(pOwner)
    , m_pConstrains(pConstrains)
{
}

//---------------------------------------------------------------------------------------
CountersCtrol::~CountersCtrol()
{
}

//---------------------------------------------------------------------------------------
void CountersCtrol::RightWrongSound(bool fSuccess)
{
    if (m_appScope.are_answer_sounds_enabled())
    {
        WavePlayer* pWave = m_appScope.get_wave_player();
        if (fSuccess)
            pWave->play_right_answer_sound();
        else
            pWave->play_wrong_answer_sound();
    }
}

//---------------------------------------------------------------------------------------
void CountersCtrol::add_mode_label(GmoBoxControl* pWrapper, UPoint pos)
{
	int nMode = m_pOwner->get_generation_mode();
    wxString mode = _("Mode:");
    mode += " " + get_generation_mode_name(nMode);

    ImoStyle* style = m_pDoc->create_private_style();
    style->padding(0.0f)->margin(0.0f)->font_size(11.0f);
    style->text_align(ImoTextStyle::k_align_left);

    StaticTextCtrl* pLblMode =
        LENMUS_NEW StaticTextCtrl(m_libraryScope, this, m_pDoc, to_std_string(mode),
                                  25000.0f, 500.0f, style);
    pWrapper->add_child_box( pLblMode->layout(m_libraryScope, pos) );
}

//---------------------------------------------------------------------------------------
void CountersCtrol::on_change_mode_requested(void* pThis, SpEventInfo WXUNUSED(pEvent))
{
    static_cast<CountersCtrol*>(pThis)->change_mode_requested();
}

//---------------------------------------------------------------------------------------
void CountersCtrol::change_mode_requested()
{
	int nMode = m_pOwner->get_generation_mode();
    DlgExerciseMode dlg(m_pOwner->get_parent_window(), &nMode, m_pConstrains);
    if (dlg.ShowModal() == wxID_OK)
        m_pOwner->change_generation_mode(nMode);
}



//=======================================================================================
// QuizCounters implementation
// A control to embed in html exercises to display the number of right and
// wrong student answers, in current session, and the total score (percentage)
//=======================================================================================
#define QUIZ_COUNTERS_WIDTH 4000.0f
#define QUIZ_COUNTERS_HEIGHT_EXAM 3600.0f
#define QUIZ_COUNTERS_HEIGHT_QUIZ 4700.0f

QuizCounters::QuizCounters(ApplicationScope& appScope,
                           Document* pDoc, ExerciseCtrol* pOwner,
                           ExerciseOptions* pConstrains, QuizManager* pProblemMngr,
                           int nNumTeams)
    : CountersCtrol(appScope, pDoc, pOwner, pConstrains)
    , m_size(QUIZ_COUNTERS_WIDTH,
             nNumTeams == 2 ? QUIZ_COUNTERS_HEIGHT_QUIZ : QUIZ_COUNTERS_HEIGHT_EXAM)
    , m_pProblemMngr(pProblemMngr)
    , m_fTwoTeamsMode(nNumTeams == 2)
{
    wxASSERT(nNumTeams == 1 || nNumTeams == 2);

    m_pRightCounter[0] = nullptr;

    ArtProvider art(m_appScope);
    m_pProblemMngr->ResetCounters();
    m_pProblemMngr->SetNumTeams(nNumTeams);

    load_images();
}

//---------------------------------------------------------------------------------------
QuizCounters::~QuizCounters()
{
}

//---------------------------------------------------------------------------------------
USize QuizCounters::measure()
{
    return m_size;
}

//---------------------------------------------------------------------------------------
void QuizCounters::load_images()
{
    Paths* pPaths = m_appScope.get_paths();
    string sFolder = to_std_string( pPaths->GetImagePath() );
    ImageReader reader;

    string sPath = sFolder + "right_answers_24.png";
    m_imgRight = reader.load_image(sPath);

    sPath = sFolder + "wrong_answers_24.png";
    m_imgWrong = reader.load_image(sPath);

    sPath = sFolder + "total_marks_24.png";
    m_imgTotal = reader.load_image(sPath);

    sPath = sFolder + "team_red_24.png";
    m_imgRed = reader.load_image(sPath);

    sPath = sFolder + "team_blue_24.png";
    m_imgBlue = reader.load_image(sPath);

    sPath = sFolder + "team_grey_24.png";
    m_imgGrey = reader.load_image(sPath);

    //determine images size, in LUnits
    float ppi = float( m_appScope.get_lomse().get_screen_ppi() );
    // 100 millimeters/LUnit, 25.4 millimeters/inch ==> 2540.0 LUnits/inch
    m_imagesSize = USize(24.0f * 2540.0f / ppi, 24.0f * 2540.0f / ppi);

    //default images when quiz mode starts
    m_imgTeam[0] = m_imgRed;
    m_imgTeam[1] = m_imgGrey;
}

//---------------------------------------------------------------------------------------
GmoBoxControl* QuizCounters::layout(LibraryScope& WXUNUSED(libraryScope), UPoint pos)
{
    UPoint cursor(pos);

    ImoStyle* mainBoxStyle = m_pDoc->create_private_style();
    mainBoxStyle->border_width(0.0f)->padding(0.0f)->margin(0.0f);

    m_pMainBox = LENMUS_NEW GmoBoxControl(this, pos, m_size.width, m_size.height,
                                          mainBoxStyle);

    add_mode_label(m_pMainBox, cursor);
    cursor.y += 555.0f;  //counters ctrol border

    //row with icons ---------------------------

    cursor.y += 15.0f;  //counters ctrol border
    if (m_fTwoTeamsMode)
        cursor.x += m_imagesSize.width + 200.0f;

    GmoShapeImage* pImgRight =
        LENMUS_NEW GmoShapeImage(nullptr, m_imgRight, cursor, m_imagesSize);
    m_pMainBox->add_shape(pImgRight, GmoShape::k_layer_top);
 ////       m_imgRight->SetToolTip(_("Right answers counter"));

    //second icon
    cursor.x += m_imagesSize.width + 250.0f;
    GmoShapeImage* pImgWrong =
        LENMUS_NEW GmoShapeImage(nullptr, m_imgWrong, cursor, m_imagesSize);
    m_pMainBox->add_shape(pImgWrong, GmoShape::k_layer_top);

    //third icon
    cursor.x += m_imagesSize.width + 350.0f;
    GmoShapeImage* pImgTotal =
        LENMUS_NEW GmoShapeImage(nullptr, m_imgTotal, cursor, m_imagesSize);
    m_pMainBox->add_shape(pImgTotal, GmoShape::k_layer_top);
    //m_imgTotal->SetToolTip(_("Total: your marks"));

    // Create counters -------------------------------------
    cursor.x = pos.x;
    cursor.y += m_imagesSize.height + 200.0f;
    CreateCountersGroup(0, m_pMainBox, cursor);
    cursor.x = pos.x;
    cursor.y += 1050.0f;
    if (m_fTwoTeamsMode)
    {
        CreateCountersGroup(1, m_pMainBox, cursor);
        cursor.x = pos.x;
        cursor.y += 1050.0f;
    }


    //'reset counters' link -----------------------------
    HyperlinkCtrl* pResetLink =
        LENMUS_NEW HyperlinkCtrl(m_libraryScope, this, m_pDoc,
                                 to_std_string(_("Reset counters")) );
    m_pMainBox->add_child_box( pResetLink->layout(m_libraryScope, cursor) );
    pResetLink->add_event_handler(k_on_click_event, this, on_reset_counters);
    cursor.y += 600.0f;


    //'change mode' link -----------------------------
    HyperlinkCtrl* pModeLink =
        LENMUS_NEW HyperlinkCtrl(m_libraryScope, this, m_pDoc,
                                 to_std_string(_("Change mode")) );
    m_pMainBox->add_child_box( pModeLink->layout(m_libraryScope, cursor) );
    pModeLink->add_event_handler(k_on_click_event, this, on_change_mode_requested);

    UpdateDisplay();

    return m_pMainBox;
}

//---------------------------------------------------------------------------------------
void QuizCounters::CreateCountersGroup(int nTeam, GmoBox* m_pMainBox, UPoint pos)
{
    // Create the controls for counter group nTeam (0...n)

    UPoint cursor(pos);

    //add team icon
    if (m_fTwoTeamsMode)
    {
        GmoShapeImage* pImgTeam =
            LENMUS_NEW GmoShapeImage(nullptr, m_imgTeam[nTeam], cursor, m_imagesSize);
        m_pMainBox->add_shape(pImgTeam, GmoShape::k_layer_top);
        cursor.x += m_imagesSize.width + 200.0f;
//        m_pImgTeam->SetToolTip(
//            (nTeam == 0 ? _("Counters for Team A") : _("Counters for Team B")) );
   }


    ImoStyle* style = m_pDoc->create_private_style();
    style->border_width(15.0f)->padding(0.0f)->margin(0.0f)->font_size(14.0f);
    style->text_align(ImoTextStyle::k_align_center);

    //display for right answers
    m_pRightCounter[nTeam] =
        LENMUS_NEW StaticTextCtrl(m_libraryScope, this, m_pDoc, "4",
                                  660.0f, 760.0f, style);
    m_pRightCounter[nTeam]->set_tooltip( to_std_string(_("Right answers counter")) );
    m_pMainBox->add_child_box( m_pRightCounter[nTeam]->layout(m_libraryScope, cursor) );
    cursor.x = m_pRightCounter[nTeam]->right() + 200.0f;

    //display for wrong answers
    m_pWrongCounter[nTeam] =
        LENMUS_NEW StaticTextCtrl(m_libraryScope, this, m_pDoc, "5",
                                  660.0f, 760.0f, style);
    m_pWrongCounter[nTeam]->set_tooltip( to_std_string(_("Wrong answers counter")) );
    m_pMainBox->add_child_box( m_pWrongCounter[nTeam]->layout(m_libraryScope, cursor) );
    cursor.x = m_pWrongCounter[nTeam]->right() + 200.0f;


    //display for total score
    m_pTotalCounter[nTeam] =
        LENMUS_NEW StaticTextCtrl(m_libraryScope, this, m_pDoc, "0.0",
                                  1000.0f, 760.0f, style);
    m_pTotalCounter[nTeam]->set_tooltip( to_std_string(_("Total: your marks")) );
    m_pMainBox->add_child_box( m_pTotalCounter[nTeam]->layout(m_libraryScope, cursor) );
}

//---------------------------------------------------------------------------------------
void QuizCounters::on_draw(Drawer* WXUNUSED(pDrawer), RenderOptions& WXUNUSED(opt))
{
}

//---------------------------------------------------------------------------------------
void QuizCounters::handle_event(SpEventInfo WXUNUSED(pEvent))
{
}

//---------------------------------------------------------------------------------------
void QuizCounters::UpdateDisplay()
{
    if (m_pRightCounter[0] == nullptr) return;
    for (int i=0; i < m_pProblemMngr->GetNumTeams(); i++)
        UpdateDisplays(i);
}

//---------------------------------------------------------------------------------------
void QuizCounters::on_reset_counters(void* pThis, SpEventInfo WXUNUSED(pEvent))
{
    static_cast<QuizCounters*>(pThis)->reset_counters();
}

//---------------------------------------------------------------------------------------
void QuizCounters::reset_counters()
{
    m_pProblemMngr->ResetCounters();
    for (int i=0; i < m_pProblemMngr->GetNumTeams(); i++)
        UpdateDisplays(i);
}

//---------------------------------------------------------------------------------------
void QuizCounters::UpdateDisplays(int nTeam)
{
    int nRight = m_pProblemMngr->GetRight(nTeam);
    int nWrong = m_pProblemMngr->GetWrong(nTeam);

    //update display for right answers
    m_pRightCounter[nTeam]->set_text( to_std_string(wxString::Format("%d", nRight)) );

    //update display for wrong answers
    m_pWrongCounter[nTeam]->set_text( to_std_string(wxString::Format("%d", nWrong)) );

    //update display for total score
    int nTotal = nRight + nWrong;
    if (nTotal == 0)
        m_pTotalCounter[nTeam]->set_text("-");
    else
    {
        float rScore = 10.0f * float(nRight) / float(nTotal);
        m_pTotalCounter[nTeam]->set_text( to_std_string(wxString::Format("%.01f", rScore)) );
    }
}

//---------------------------------------------------------------------------------------
void QuizCounters::NextTeam()
{
    m_pProblemMngr->NextTeam();

    //update team image
    if (m_fTwoTeamsMode)
    {
        if (m_pProblemMngr->GetCurrentTeam() == 0)
        {
            m_imgTeam[0] = m_imgRed;
            m_imgTeam[1] = m_imgGrey;
        }
        else
        {
            m_imgTeam[0] = m_imgGrey;
            m_imgTeam[1] = m_imgBlue;
        }
    }
}

//---------------------------------------------------------------------------------------
void QuizCounters::OnNewQuestion()
{
    NextTeam();
}


//=======================================================================================
// LeitnerCounters implementation
// A control to embed in html exercises to display statistics
// on user performance in learning the subject. It uses the Leitner system of
// spaced repetitions
//=======================================================================================
#define LEITNER_COUNTERS_WIDTH 4000.0f
#define LEITNER_COUNTERS_HEIGHT 4900.0f

//---------------------------------------------------------------------------------------
LeitnerCounters::LeitnerCounters(ApplicationScope& appScope, Document* pDoc,
                                 ExerciseCtrol* pOwner, ExerciseOptions* pConstrains,
                                 LeitnerManager* pProblemMngr)
    : CountersCtrol(appScope, pDoc, pOwner, pConstrains)
    , m_size(LEITNER_COUNTERS_WIDTH, LEITNER_COUNTERS_HEIGHT)
    , m_pProblemMngr(pProblemMngr)
{
    //load_images();
}

//---------------------------------------------------------------------------------------
LeitnerCounters::~LeitnerCounters()
{
}

//---------------------------------------------------------------------------------------
USize LeitnerCounters::measure()
{
    return m_size;
}

//---------------------------------------------------------------------------------------
GmoBoxControl* LeitnerCounters::layout(LibraryScope& WXUNUSED(libraryScope), UPoint pos)
{
    UPoint cursor(pos);

    ImoStyle* mainBoxStyle = m_pDoc->create_private_style();
    mainBoxStyle->border_width(0.0f)->padding(0.0f)->margin(0.0f);

    m_pMainBox = LENMUS_NEW GmoBoxControl(this, pos, m_size.width, m_size.height,
                                          mainBoxStyle);

    add_mode_label(m_pMainBox, cursor);
    cursor.y += 555.0f;  //counters ctrol border

    ImoStyle* style = m_pDoc->create_private_style();
    style->border_width(0.0f)->padding(0.0f)->margin(0.0f);

    //display for num questions  -----------------------------
    wxString label = _("Questions:");
    m_pTxtNumQuestions =
        LENMUS_NEW StaticTextCtrl(m_libraryScope, this, m_pDoc, to_std_string(label),
                                  660.0f, 760.0f, style);
    m_pTxtNumQuestions->set_tooltip( to_std_string(_("New questions / to review questions")) );
    m_pMainBox->add_child_box( m_pTxtNumQuestions->layout(m_libraryScope, cursor) );
    cursor.y += 600.0f;

    //display for estimated time  -----------------------------
    m_pTxtTime =
        LENMUS_NEW StaticTextCtrl(m_libraryScope, this, m_pDoc, "EST: 1h:27m",
                                  660.0f, 760.0f, style);
    m_pTxtTime->set_tooltip( to_std_string(_("Estimated time to finish this session")) );
    m_pMainBox->add_child_box( m_pTxtTime->layout(m_libraryScope, cursor) );
    cursor.y += 600.0f;

    //display for session progress gauge  -----------------------------
    label = _("Session:");
    m_pTxtSession =
        LENMUS_NEW StaticTextCtrl(m_libraryScope, this, m_pDoc, to_std_string(label),
                                  660.0f, 760.0f, style);
    //m_pTxtSession->set_tooltip( to_std_string(_("Session: completed ")) );
    m_pMainBox->add_child_box( m_pTxtSession->layout(m_libraryScope, cursor) );
    cursor.x += 1500.0f;
    cursor.y += 200.0f;

	m_pGaugeSession =
        LENMUS_NEW ProgressBarCtrl(m_libraryScope, this, m_pDoc, 100.0f, 3000.0f);
    m_pMainBox->add_child_box( m_pGaugeSession->layout(m_libraryScope, cursor) );
	cursor.x = pos.x;
    cursor.y += 400.0f;


    //labels for short, medium and long term achievement  ------------------------------
    label = _("Short");
    m_pLblShort =
        LENMUS_NEW StaticTextCtrl(m_libraryScope, this, m_pDoc, to_std_string(label),
                                  660.0f, 760.0f, style);
    m_pMainBox->add_child_box( m_pLblShort->layout(m_libraryScope, cursor) );
    cursor.x += 1600.0f;

    label = _("Medium");
    m_pLblMedium =
        LENMUS_NEW StaticTextCtrl(m_libraryScope, this, m_pDoc, to_std_string(label),
                                  660.0f, 760.0f, style);
    m_pMainBox->add_child_box( m_pLblMedium->layout(m_libraryScope, cursor) );
    cursor.x += 1600.0f;

    label = _("Long");
    m_pLblLong =
        LENMUS_NEW StaticTextCtrl(m_libraryScope, this, m_pDoc, to_std_string(label),
                                  660.0f, 760.0f, style);
    m_pMainBox->add_child_box( m_pLblLong->layout(m_libraryScope, cursor) );
	cursor.x = pos.x;
    cursor.y += 650.0f;


    //display for short term achievement  ------------------------------
    ImoStyle* styleShort = m_pDoc->create_private_style();
    styleShort->border_width(15.0f)->padding(0.0f)->margin(0.0f)->font_size(12.0f);
    styleShort->text_align(ImoTextStyle::k_align_center);
    styleShort->background_color(Color(255,0,0));   //red
    styleShort->color(Color(255,255,255))->font_weight(ImoStyle::k_font_weight_bold);   //white bold
    wxString value = "100%";
    m_pTxtShort =
        LENMUS_NEW StaticTextCtrl(m_libraryScope, this, m_pDoc, to_std_string(value),
                                  1600.0f, 700.0f, styleShort);
    m_pTxtShort->set_tooltip( to_std_string(_("Short term achievement")) );
    m_pMainBox->add_child_box( m_pTxtShort->layout(m_libraryScope, cursor) );
    cursor.x += 1600.0f;


    //display for medium term achievement  ------------------------------
    ImoStyle* styleMedium = m_pDoc->create_private_style();
    styleMedium->border_width(15.0f)->padding(0.0f)->margin(0.0f)->font_size(12.0f);
    styleMedium->text_align(ImoTextStyle::k_align_center);
    styleMedium->background_color(Color(255,255,0));   //orange
    styleMedium->color(Color(0,0,0))->font_weight(ImoStyle::k_font_weight_bold);   //black bold
    value = "100%";
    m_pTxtMedium =
        LENMUS_NEW StaticTextCtrl(m_libraryScope, this, m_pDoc, to_std_string(value),
                                  1600.0f, 700.0f, styleMedium);
    m_pTxtMedium->set_tooltip( to_std_string(_("Medium term achievement")) );
    m_pMainBox->add_child_box( m_pTxtMedium->layout(m_libraryScope, cursor) );
    cursor.x += 1600.0f;


    //display for long term achievement  ------------------------------
    ImoStyle* styleLong = m_pDoc->create_private_style();
    styleLong->border_width(15.0f)->padding(0.0f)->margin(0.0f)->font_size(12.0f);
    styleLong->text_align(ImoTextStyle::k_align_center);
    styleLong->background_color(Color(0,255,0));   //pale green
    styleLong->color(Color(0,0,0))->font_weight(ImoStyle::k_font_weight_bold);   //black bold
    value = "100%";
    m_pTxtLong =
        LENMUS_NEW StaticTextCtrl(m_libraryScope, this, m_pDoc, to_std_string(value),
                                  1600.0f, 700.0f, styleLong);
    m_pTxtLong->set_tooltip( to_std_string(_("Long term achievement")) );
    m_pMainBox->add_child_box( m_pTxtLong->layout(m_libraryScope, cursor) );
	cursor.x = pos.x;
    cursor.y += 800.0f;

    //'explain' link -----------------------------
    HyperlinkCtrl* pExplainLink =
        LENMUS_NEW HyperlinkCtrl(m_libraryScope, this, m_pDoc,
                                 to_std_string(_("Explain")) );
    m_pMainBox->add_child_box( pExplainLink->layout(m_libraryScope, cursor) );
    pExplainLink->add_event_handler(k_on_click_event, this, on_explain_progress);
    cursor.y += 600.0f;

    //'change mode' link -----------------------------
    HyperlinkCtrl* pModeLink =
        LENMUS_NEW HyperlinkCtrl(m_libraryScope, this, m_pDoc,
                                 to_std_string(_("Change mode")) );
    m_pMainBox->add_child_box( pModeLink->layout(m_libraryScope, cursor) );
    pModeLink->add_event_handler(k_on_click_event, this, on_change_mode_requested);

    UpdateDisplay();

    return m_pMainBox;
}

//---------------------------------------------------------------------------------------
void LeitnerCounters::on_draw(Drawer* WXUNUSED(pDrawer), RenderOptions& WXUNUSED(opt))
{
}

//---------------------------------------------------------------------------------------
void LeitnerCounters::handle_event(SpEventInfo WXUNUSED(pEvent))
{
}

//---------------------------------------------------------------------------------------
void LeitnerCounters::UpdateDisplay()
{
    m_pProblemMngr->compute_achievement_indicators();

    //questions
    int nNew = m_pProblemMngr->get_new();
    int nExpired = m_pProblemMngr->get_expired();
    //int nTotal = m_pProblemMngr->get_total();
    wxString value = _("Questions:");
    value += wxString::Format(" %d / %d", nNew, nExpired);
    m_pTxtNumQuestions->set_text( to_std_string(value) );

    //EST
    wxTimeSpan tsEST = m_pProblemMngr->get_estimated_session_time();
    m_pTxtTime->set_text( to_std_string( tsEST.Format("EST: %Hh %Mm %Ss") ) );

    //Session progress
	m_pGaugeSession->set_value( m_pProblemMngr->get_session_progress() );

    //short, medium and long term achievment
    value = wxString::Format("%.01f%", m_pProblemMngr->get_short_term_progress() * 100.0f);
    m_pTxtShort->set_text( to_std_string(value) );
    value = wxString::Format("%.01f%", m_pProblemMngr->get_medium_term_progress() * 100.0f);
    m_pTxtMedium->set_text( to_std_string(value) );
    value = wxString::Format("%.01f%", m_pProblemMngr->get_long_term_progress() * 100.0f);
    m_pTxtLong->set_text( to_std_string(value) );
}

//---------------------------------------------------------------------------------------
void LeitnerCounters::on_explain_progress(void* pThis, SpEventInfo WXUNUSED(pEvent))
{
    static_cast<LeitnerCounters*>(pThis)->explain_progress();
}

//---------------------------------------------------------------------------------------
void LeitnerCounters::explain_progress()
{
    lmHtmlDlg dlg(m_pOwner->get_parent_window(), _("Progress report"));
    dlg.SetContent( m_pProblemMngr->get_progress_report() );
    dlg.ShowModal();
}


//=======================================================================================
// PractiseCounters implementation
// A control to embed in exercises to display statistics
// on user performance in learning the subject. It uses the Leitner system of
// spaced repetitions
//=======================================================================================
#define PRACTISE_COUNTERS_WIDTH 4000.0f
#define PRACTISE_COUNTERS_HEIGHT 3600.0f

//---------------------------------------------------------------------------------------
PractiseCounters::PractiseCounters(ApplicationScope& appScope, Document* pDoc,
                                   ExerciseCtrol* pOwner, ExerciseOptions* pConstrains,
                                   LeitnerManager* pProblemMngr)
    : CountersCtrol(appScope, pDoc, pOwner, pConstrains)
    , m_size(PRACTISE_COUNTERS_WIDTH, PRACTISE_COUNTERS_HEIGHT)
    , m_pProblemMngr(pProblemMngr)
    , m_pRightCounter(nullptr)
{
    m_pProblemMngr->ResetPractiseCounters();
    load_images();
}

//---------------------------------------------------------------------------------------
PractiseCounters::~PractiseCounters()
{
}

//---------------------------------------------------------------------------------------
USize PractiseCounters::measure()
{
    return m_size;
}

//---------------------------------------------------------------------------------------
void PractiseCounters::load_images()
{
    Paths* pPaths = m_appScope.get_paths();
    string sFolder = to_std_string( pPaths->GetImagePath() );
    ImageReader reader;

    string sPath = sFolder + "right_answers_24.png";
    m_imgRight = reader.load_image(sPath);

    sPath = sFolder + "wrong_answers_24.png";
    m_imgWrong = reader.load_image(sPath);

    sPath = sFolder + "total_marks_24.png";
    m_imgTotal = reader.load_image(sPath);

    //determine images size, in LUnits
    float ppi = float( m_appScope.get_lomse().get_screen_ppi() );
    // 100 millimeters/LUnit, 25.4 millimeters/inch ==> 2540.0 LUnits/inch
    m_imagesSize = USize(24.0f * 2540.0f / ppi, 24.0f * 2540.0f / ppi);
}

//---------------------------------------------------------------------------------------
GmoBoxControl* PractiseCounters::layout(LibraryScope& WXUNUSED(libraryScope), UPoint pos)
{
    UPoint cursor(pos);

    ImoStyle* mainBoxStyle = m_pDoc->create_private_style();
    mainBoxStyle->border_width(0.0f)->padding(0.0f)->margin(0.0f);

    m_pMainBox = LENMUS_NEW GmoBoxControl(this, pos, m_size.width, m_size.height,
                                          mainBoxStyle);

    add_mode_label(m_pMainBox, cursor);
    cursor.y += 555.0f;  //counters ctrol border

    //row with icons ---------------------------

    cursor.y += 15.0f;  //counters ctrol border

    GmoShapeImage* pImgRight =
        LENMUS_NEW GmoShapeImage(nullptr, m_imgRight, cursor, m_imagesSize);
    m_pMainBox->add_shape(pImgRight, GmoShape::k_layer_top);
 ////       m_imgRight->SetToolTip(_("Right answers counter"));

    //second icon
    cursor.x += m_imagesSize.width + 250.0f;
    GmoShapeImage* pImgWrong =
        LENMUS_NEW GmoShapeImage(nullptr, m_imgWrong, cursor, m_imagesSize);
    m_pMainBox->add_shape(pImgWrong, GmoShape::k_layer_top);

    //third icon
    cursor.x += m_imagesSize.width + 350.0f;
    GmoShapeImage* pImgTotal =
        LENMUS_NEW GmoShapeImage(nullptr, m_imgTotal, cursor, m_imagesSize);
    m_pMainBox->add_shape(pImgTotal, GmoShape::k_layer_top);
    //m_imgTotal->SetToolTip(_("Total: your marks"));

    // Create counters -------------------------------------
    cursor.x = pos.x;
    cursor.y += m_imagesSize.height + 200.0f;
    create_counters(m_pMainBox, cursor);
    cursor.x = pos.x;
    cursor.y += 1050.0f;

    //'reset counters' link -----------------------------
    HyperlinkCtrl* pResetLink =
        LENMUS_NEW HyperlinkCtrl(m_libraryScope, this, m_pDoc,
                                 to_std_string(_("Reset counters")) );
    m_pMainBox->add_child_box( pResetLink->layout(m_libraryScope, cursor) );
    pResetLink->add_event_handler(k_on_click_event, this, on_reset_counters);
    cursor.y += 600.0f;


    //'change mode' link -----------------------------
    HyperlinkCtrl* pModeLink =
        LENMUS_NEW HyperlinkCtrl(m_libraryScope, this, m_pDoc,
                                 to_std_string(_("Change mode")) );
    m_pMainBox->add_child_box( pModeLink->layout(m_libraryScope, cursor) );
    pModeLink->add_event_handler(k_on_click_event, this, on_change_mode_requested);

    UpdateDisplay();

    return m_pMainBox;
}

//---------------------------------------------------------------------------------------
void PractiseCounters::create_counters(GmoBox* m_pMainBox, UPoint pos)
{
    UPoint cursor(pos);

    ImoStyle* style = m_pDoc->create_private_style();
    style->border_width(15.0f)->padding(0.0f)->margin(0.0f)->font_size(14.0f);
    style->text_align(ImoTextStyle::k_align_center);

    //display for right answers
    m_pRightCounter =
        LENMUS_NEW StaticTextCtrl(m_libraryScope, this, m_pDoc, "5",
                                  660.0f, 760.0f, style);
    m_pRightCounter->set_tooltip( to_std_string(_("Right answers counter")) );
    m_pMainBox->add_child_box( m_pRightCounter->layout(m_libraryScope, cursor) );
    cursor.x = m_pRightCounter->right() + 200.0f;

    //display for wrong answers
    m_pWrongCounter =
        LENMUS_NEW StaticTextCtrl(m_libraryScope, this, m_pDoc, "4",
                                  660.0f, 760.0f, style);
    m_pWrongCounter->set_tooltip( to_std_string(_("Wrong answers counter")) );
    m_pMainBox->add_child_box( m_pWrongCounter->layout(m_libraryScope, cursor) );
    cursor.x = m_pWrongCounter->right() + 200.0f;


    //display for total score
    m_pTotalCounter =
        LENMUS_NEW StaticTextCtrl(m_libraryScope, this, m_pDoc, "0.0",
                                  1000.0f, 760.0f, style);
    m_pTotalCounter->set_tooltip( to_std_string(_("Total: your marks")) );
    m_pMainBox->add_child_box( m_pTotalCounter->layout(m_libraryScope, cursor) );
}

//---------------------------------------------------------------------------------------
void PractiseCounters::on_draw(Drawer* WXUNUSED(pDrawer), RenderOptions& WXUNUSED(opt))
{
}

//---------------------------------------------------------------------------------------
void PractiseCounters::handle_event(SpEventInfo WXUNUSED(pEvent))
{
}

//---------------------------------------------------------------------------------------
void PractiseCounters::on_reset_counters(void* pThis, SpEventInfo WXUNUSED(pEvent))
{
    static_cast<PractiseCounters*>(pThis)->reset_counters();
}

//---------------------------------------------------------------------------------------
void PractiseCounters::reset_counters()
{
    m_pProblemMngr->ResetPractiseCounters();
    UpdateDisplay();
}

//---------------------------------------------------------------------------------------
void PractiseCounters::UpdateDisplay()
{
    if (m_pRightCounter == nullptr) return;

    int nRight = m_pProblemMngr->GetRight();
    int nWrong = m_pProblemMngr->GetWrong();

    //update display for right answers
    m_pRightCounter->set_text( to_std_string(wxString::Format("%d", nRight)) );

    //update display for wrong answers
    m_pWrongCounter->set_text( to_std_string(wxString::Format("%d", nWrong)) );

    //update display for total score
    int nTotal = nRight + nWrong;
    if (nTotal == 0)
        m_pTotalCounter->set_text("-");
    else
    {
        float rScore = 10.0f * float(nRight) / float(nTotal);
        m_pTotalCounter->set_text( to_std_string(wxString::Format("%.01f", rScore)) );
    }
}


}   // namespace lenmus
