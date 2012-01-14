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
#include "lenmus_theo_intervals_ctrol.h"

#include "lenmus_theo_intervals_constrains.h"
#include "lenmus_theo_intervals_ctrol_params.h"
#include "lenmus_string.h"
#include "lenmus_dlg_cfg_theo_intervals.h"
#include "lenmus_score_canvas.h"
#include "lenmus_interval.h"
#include "lenmus_generators.h"

//lomse
#include <lomse_doorway.h>
#include <lomse_internal_model.h>
#include <lomse_im_note.h>
#include <lomse_im_factory.h>
using namespace lomse;

//wxWidgets
#include <wx/wxprec.h>


namespace lenmus
{

//Data about intervals to generate for each problem level
static FIntval m_aProblemDataL0[] = {
    lm_p1, lm_m2, lm_M2, lm_m3, lm_M3, lm_p4, lm_p5, lm_m6, lm_M6, lm_m7, lm_M7, lm_p8 };
static FIntval m_aProblemDataL1[] = {
    lm_p1, lm_m2, lm_M2, lm_m3, lm_M3, lm_p4, lm_p5, lm_m6, lm_M6, lm_m7, lm_M7, lm_p8 };
static FIntval m_aProblemDataL2[] = {
    lm_p1, lm_a1, lm_d2, lm_m2, lm_M2, lm_a2, lm_d3, lm_m3, lm_M3, lm_a3, lm_d4, lm_p4, lm_a4,
    lm_d5, lm_p5, lm_a5, lm_d6, lm_m6, lm_M6, lm_a6, lm_d7, lm_m7, lm_M7, lm_a7, lm_d8, lm_p8 };
static FIntval m_aProblemDataL3[] = {
    lm_p1, lm_a1, lm_da1, lm_dd2, lm_d2, lm_m2, lm_M2, lm_a2, lm_da2, lm_dd3, lm_d3, lm_m3, lm_M3,
    lm_a3, lm_da3, lm_dd4, lm_d4, lm_p4, lm_a4, lm_da4, lm_dd5, lm_d5, lm_p5, lm_a5, lm_da5, lm_dd6,
    lm_d6, lm_m6, lm_M6, lm_a6, lm_da6, lm_dd7, lm_d7, lm_m7, lm_M7, lm_a7, lm_da7, lm_dd8, lm_d8,
    lm_p8 };

//Questions. Params to generate a question
enum
{
    lmINTVAL_INDEX = 0,
    lmKEY_SIGNATURE,
};

//=======================================================================================
// TheoIntervalsCtrol implementation
//=======================================================================================
TheoIntervalsCtrol::TheoIntervalsCtrol(long dynId, ApplicationScope& appScope,
                                       DocumentWindow* pCanvas)
    : OneScoreCtrol(dynId, appScope, pCanvas)
{
    //initializations
    m_nRespIndex = 0;
}

//---------------------------------------------------------------------------------------
TheoIntervalsCtrol::~TheoIntervalsCtrol()
{
    delete m_pBaseConstrains;
}

//---------------------------------------------------------------------------------------
void TheoIntervalsCtrol::get_ctrol_options_from_params()
{
    m_pBaseConstrains = LENMUS_NEW TheoIntervalsConstrains(_T("TheoIntervals"), m_appScope);
    TheoIntervalsCtrolParams builder(m_pBaseConstrains);
    builder.process_params( m_pDyn->get_params() );
}

//---------------------------------------------------------------------------------------
void TheoIntervalsCtrol::prepare_aux_score(int nButton)
{
    // No problem is presented and the user press the button to play a specific
    // sound (chord, interval, scale, etc.)
    // This method is then invoked to prepare the score with the requested sound.
    // At return, base class will play it

    // In theory interval exercises it is not allowed to play an interval so
    // we return a Null score
    m_pAuxScore = NULL;
}

//---------------------------------------------------------------------------------------
wxDialog* TheoIntervalsCtrol::get_settings_dialog()
{
    // 'Settings' link has been clicked. This method must return the dialog to invoke

    wxWindow* pParent = dynamic_cast<wxWindow*>(m_pCanvas);
    return LENMUS_NEW DlgCfgTheoIntervals(pParent, m_pConstrains);
}

//---------------------------------------------------------------------------------------
void TheoIntervalsCtrol::on_settings_changed()
{
    // The settings have been changed.

    //if problem level has changed set up the new problem space
    set_problem_space();

    //Reconfigure answer keyboard for the new settings
    reconfigure_keyboard();
}

//---------------------------------------------------------------------------------------
void TheoIntervalsCtrol::set_problem_space()
{
    if (m_sKeyPrefix == _T("")) return;     //Ctrol constructor not yet finished

    //save current problem space data
    m_pProblemManager->save_problem_space();

    //For TheoIntervals exercises, question sets are defined by combination of
    //problem level and key signature, except for level 0 (only interval names).
    //For level 0 there is only one set
    m_nProblemLevel = m_pConstrains->GetProblemLevel();
    if (m_nProblemLevel == 0)
    {
        set_space_level_0();
    }
    else
    {
        //Problem Space: TheoIntervals
        //Question params:
        //  Param0 - Index on  m_aProblemDataLx[] to define interval
        //  Param1 - Key signature
        //  All others not used -> Mandatory params = 2
        m_pProblemManager->NewSpace(m_sKeyPrefix, 3, 2);
        KeyConstrains* pKeyConstrains = m_pConstrains->GetKeyConstrains();
        for (int i=0; i < k_key_F+1; i++)
        {
            if ( pKeyConstrains->IsValid((EKeySignature)i) )
            {
                wxString sSetName = wxString::Format(_T("Level%d/Key%d"),
                                                     m_nProblemLevel, i);
                //ask problem manager to load this Set.
                if ( !m_pProblemManager->LoadSet(sSetName) )
                {
                    //No questions saved for this set. Create the set
                    create_questions_set(sSetName, (EKeySignature)i);
                }
            }
        }
    }
    //new space loaded. Inform problem manager
    m_pProblemManager->OnProblemSpaceChanged();

    //update counters and discard any currently formulated question
    if (m_pCounters && m_fCountersValid)
    {
        m_pCounters->UpdateDisplay();
        if (m_fQuestionAsked)
            new_problem();
    }
}

//---------------------------------------------------------------------------------------
void TheoIntervalsCtrol::set_space_level_0()
{
    //Problem Space: Initiation to intervals
    //Question params:
    //  Param0 - Index on  m_aProblemDataL0[] to define interval
    //  All others not used -> Mandatory params = 1

    wxString sSpaceName = m_sKeyPrefix + _T("/Level0");
    m_pProblemManager->NewSpace(sSpaceName, 3, 1);
    wxString sSetName = _T("Level0");

    //ask problem manager to load the set.
    if ( !m_pProblemManager->LoadSet(sSetName) )
    {
        //No questions saved for this set. Create the set
        m_pProblemManager->StartNewSet(sSetName);
        for (int i=0; i < 8; i++)
            m_pProblemManager->AddQuestionToSet(i);

        m_pProblemManager->EndOfNewSet();
    }
}

//---------------------------------------------------------------------------------------
void TheoIntervalsCtrol::create_questions_set(wxString& sSetName, EKeySignature nKey)
{
    wxASSERT(m_nProblemLevel > 0 && m_nProblemLevel < 4);

    int nNumQuestions;
    if (m_nProblemLevel == 1)
        nNumQuestions = sizeof(m_aProblemDataL1)/sizeof(FIntval);
    else if (m_nProblemLevel == 2)
        nNumQuestions = sizeof(m_aProblemDataL2)/sizeof(FIntval);
    else
        nNumQuestions = sizeof(m_aProblemDataL3)/sizeof(FIntval);

    m_pProblemManager->StartNewSet(sSetName);
    for (int i=0; i <nNumQuestions; i++)
        m_pProblemManager->AddQuestionToSet(i, (long)nKey);

    m_pProblemManager->EndOfNewSet();
}

//---------------------------------------------------------------------------------------
wxString TheoIntervalsCtrol::set_new_problem()
{
    // This method must prepare the interval for the problem and set variables:
    // m_iQ, m_fpIntv, m_fpStart, m_fpEnd, m_sAnswer

    //Get parameters controlled by problem space

    //Param0: index to interval number
    m_iQ = m_pProblemManager->ChooseQuestion();
    wxASSERT(m_iQ>= 0 && m_iQ < m_pProblemManager->GetSpaceSize());

    wxASSERT(m_pProblemManager->IsQuestionParamMandatory(lmINTVAL_INDEX));
    long nIntvNdx = m_pProblemManager->GetQuestionParam(m_iQ, lmINTVAL_INDEX);
    if (m_nProblemLevel <= 1)
        m_fpIntv = m_aProblemDataL1[nIntvNdx];
    else if (m_nProblemLevel == 2)
        m_fpIntv = m_aProblemDataL2[nIntvNdx];
    else
        m_fpIntv = m_aProblemDataL3[nIntvNdx];

    int nIntvNum = m_fpIntv.get_number();           //get interval number

    //Param1: key signature
    RandomGenerator oGenerator;
    if (m_pProblemManager->IsQuestionParamMandatory(lmKEY_SIGNATURE))
        m_nKey = (EKeySignature)m_pProblemManager->GetQuestionParam(m_iQ, lmKEY_SIGNATURE);
    else
        m_nKey = oGenerator.generate_key(m_pConstrains->GetKeyConstrains());


    //Get other parameters: selectable by the user

    int nMinPos = 2 - (2 * m_pConstrains->GetLedgerLinesBelow());
    int nMaxPos = 10 + (2 * m_pConstrains->GetLedgerLinesAbove());
    nMaxPos -= nIntvNum - 1;

    //Generate start note and end note
    bool fValid = false;
    m_nClef = oGenerator.generate_clef(m_pConstrains->GetClefConstrains());
    while (!fValid)
    {
        DiatonicPitch dpStart =
            oGenerator.GenerateRandomDiatonicPitch(nMinPos, nMaxPos, false, m_nClef);
        m_fpStart = dpStart.to_FPitch(m_nKey);
        m_fpEnd = m_fpStart + m_fpIntv;
        fValid = m_fpEnd.is_valid();
        if (!fValid)
            wxLogMessage(_T("[TheoIntervalsCtrol::set_new_problem] INVALID: m_iQ=%d, nIntvNdx=%d, m_fpIntv=%d, m_fpStart=%d, m_fpEnd=%d"),
                 m_iQ, nIntvNdx, (int)m_fpIntv, (int)m_fpStart, (int)m_fpEnd);
    }

    //compute the interval name
    if (m_fpIntv == FIntval(0))
        m_sAnswer = _("Unison");
    else if (m_fpIntv == FIntval(1))
        m_sAnswer = _("Chromatic semitone");
    else if (m_fpIntv == FIntval(2))
        m_sAnswer = _("Chromatic tone");
    else
        m_sAnswer = m_fpIntv.get_name();

    if (m_fpIntv > FIntval(0))
        m_sAnswer += (m_fpEnd > m_fpStart ? _(", ascending") : _(", descending") );

    wxLogMessage(_T("[TheoIntervalsCtrol::set_new_problem] m_iQ=%d, nIntvNdx=%d, m_fpIntv=%s (%d), m_fpStart=%s (%d), m_fpEnd=%s (%d), sAnswer=%s"),
                 m_iQ, nIntvNdx, m_fpIntv.get_code().c_str(), (int)m_fpIntv,
                 to_wx_string(m_fpStart.to_abs_ldp_name()).c_str(), (int)m_fpStart,
                 to_wx_string(m_fpEnd.to_abs_ldp_name()).c_str(), (int)m_fpEnd,
                 m_sAnswer.c_str());

    return prepare_scores();
}



//=======================================================================================
// Implementation of BuildIntervalsCtrol
//=======================================================================================


//type of keyboard currently displayed
enum {
    eKeyboardNone = 0,
    eKeyboardIntv,          //identify interval
    eKeyboardNotes,         //build interval
};

//internationalized strings
static string m_sNotesButtonLabel[35];
static string m_sNotesRowLabel[BuildIntervalsCtrol::k_num_rows];
static string m_sNotesColumnLabel[BuildIntervalsCtrol::k_num_cols];


//---------------------------------------------------------------------------------------
BuildIntervalsCtrol::BuildIntervalsCtrol(long dynId, ApplicationScope& appScope,
                                       DocumentWindow* pCanvas)
    : TheoIntervalsCtrol(dynId, appScope, pCanvas)
{
}

//---------------------------------------------------------------------------------------
BuildIntervalsCtrol::~BuildIntervalsCtrol()
{
}

//---------------------------------------------------------------------------------------
void BuildIntervalsCtrol::initialize_ctrol()
{
    m_pConstrains = dynamic_cast<TheoIntervalsConstrains*>(m_pBaseConstrains);

    //set key
    m_sKeyPrefix = wxString::Format(_T("/BuildIntval/%s/"),
                                    m_pBaseConstrains->GetSection().c_str() );
    //create controls
    m_pConstrains->SetGenerationModeSupported(k_learning_mode, true);
    m_pConstrains->SetGenerationModeSupported(k_practise_mode, true);
    m_pConstrains->SetGenerationMode(k_learning_mode);
    create_controls();

    //update display
    if (m_pCounters && m_fCountersValid)
        m_pCounters->UpdateDisplay();

    if (m_pConstrains->is_theory_mode())
        new_problem();
}

//---------------------------------------------------------------------------------------
void BuildIntervalsCtrol::create_answer_buttons(LUnits height, LUnits spacing)
{
    //create 35 buttons for the answers: five rows, seven buttons per row

    ImoStyle* pDefStyle = m_pDoc->get_default_style();
    ImoInlineWrapper* pBox;
    ImoStyle* pBtStyle = m_pDoc->create_private_style();
    pBtStyle->font_name("sans-serif")->font_size(8.0f);

    ImoStyle* pRowStyle = m_pDoc->create_private_style();
    pRowStyle->font_size(10.0f)->margin_bottom(100.0f);

    USize buttonSize(1500.0f, height);
    LUnits firstRowWidth = 4000.0f;
    LUnits otherRowsWidth = buttonSize.width + spacing;

    int iB;
    for (iB=0; iB < k_num_buttons; iB++)
        m_pAnswerButton[iB] = NULL;

    //row with column labels
    ImoParagraph* pKeyboardRow = m_pDyn->add_paragraph(pRowStyle);

    //spacer for labels column
    pBox = pKeyboardRow->add_inline_box(firstRowWidth, pDefStyle);

    for (int iCol=0; iCol < k_num_cols; iCol++)
    {
        pBox = pKeyboardRow->add_inline_box(otherRowsWidth, pDefStyle);
        m_pColumnLabel[iCol] = pBox->add_text_item(m_sNotesColumnLabel[iCol],
                                                   pRowStyle);
    }

    //remaining rows with buttons
    for (int iRow=0; iRow < k_num_rows; iRow++)
    {
        ImoParagraph* pKeyboardRow = m_pDyn->add_paragraph(pRowStyle);

        pBox = pKeyboardRow->add_inline_box(firstRowWidth, pDefStyle);
        m_pRowLabel[iRow] = pBox->add_text_item(m_sNotesRowLabel[iRow], pRowStyle);

        // the buttons for this row
        for (int iCol=0; iCol < k_num_cols; iCol++)
        {
            iB = iCol + iRow * k_num_cols;    // button index: 0 .. 34
            pBox = pKeyboardRow->add_inline_box(otherRowsWidth, pDefStyle);
            m_pAnswerButton[iB] = pBox->add_button(m_sNotesButtonLabel[iB],
                                                   buttonSize, pBtStyle);

            if (m_sNotesButtonLabel[iB].empty())
            {
                m_pAnswerButton[iB]->set_visible(false);
                m_pAnswerButton[iB]->enable(false);
            }
        }
    }

    //inform base class about the settings
    set_buttons(m_pAnswerButton, k_num_buttons);
}

//---------------------------------------------------------------------------------------
void BuildIntervalsCtrol::reconfigure_keyboard()
{
    // Reconfigure answer keyboard for the new settings

    // TODO 5.0: Where is the original code?
}

//---------------------------------------------------------------------------------------
wxString BuildIntervalsCtrol::prepare_scores()
{
    //The problem interval has been set.
    //This method must prepare the problem score and set variables:
    //  m_pProblemScore - The score with the problem to propose
    //  m_pSolutionScore - The score with the solution or NULL if it is the
    //              same score than the problem score.
    //  m_sAnswer - the message to present when displaying the solution
    //  m_nRespIndex - the number of the button for the right answer
    //  m_nPlayMM - the speed to play the score
    //
    //It must return the message to display to introduce the problem.

    //prepare LDP pattern
    string sPattern0 = "(n ";
    sPattern0 += m_fpStart.to_rel_ldp_name(m_nKey);
    sPattern0 += " w)";

    string sPattern1 = "(n ";
    sPattern1 += m_fpEnd.to_rel_ldp_name(m_nKey);
    sPattern1 += " w)";

    //prepare solution score
    ImoScore* pScore = static_cast<ImoScore*>(ImFactory::inject(k_imo_score, m_pDoc));
    pScore->set_long_option("Render.SpacingMethod", long(k_spacing_fixed));
    ImoInstrument* pInstr = pScore->add_instrument();    // (0,0,_T(""));		//MIDI channel 0, MIDI instr 0
    ImoSystemInfo* pInfo = pScore->get_first_system_info();
    pInfo->set_top_system_distance( pInstr->tenths_to_logical(30) );     // 3 lines
    pInstr->add_clef( m_nClef );
    pInstr->add_key_signature(m_nKey);
    pInstr->add_time_signature(4 ,4, NO_VISIBLE );
    pInstr->add_spacer(30);       // 3 lines
    pInstr->add_object( sPattern0 );
    pInstr->add_barline(ImoBarline::k_simple, NO_VISIBLE);    //so that accidental doesn't affect 2nd note
    pInstr->add_object( sPattern1 );
    pInstr->add_spacer(50);       // 5 lines
    pInstr->add_barline(ImoBarline::k_end, NO_VISIBLE);

    pScore->close();

    //for building intervals exercise the created score is the solution and
    //we need to create another score with the problem
    m_pSolutionScore = pScore;
    m_pProblemScore = static_cast<ImoScore*>(ImFactory::inject(k_imo_score, m_pDoc));
    pInstr = m_pProblemScore->add_instrument();    // (0,0,_T(""));		//MIDI channel 0, MIDI instr 0
    pInfo = m_pProblemScore->get_first_system_info();
    pInfo->set_top_system_distance( pInstr->tenths_to_logical(30) );     // 3 lines
    pInstr->add_clef( m_nClef );
    pInstr->add_key_signature(m_nKey);
    pInstr->add_time_signature(4 ,4, NO_VISIBLE );
    pInstr->add_spacer(30);       // 3 lines
    pInstr->add_object( sPattern0 );
    pInstr->add_spacer(75);       // 7.5 lines
    pInstr->add_barline(ImoBarline::k_end, NO_VISIBLE);

    m_pProblemScore->close();

    //cumpute right answer button index
    int iCol = m_fpEnd.step();
    int iRow = m_fpEnd.num_accidentals() + 2;
    m_nRespIndex = iCol + iRow * k_num_cols;

    //return question string
    m_sAnswer = _("Build a ") + m_sAnswer;
    return m_sAnswer;
}

//---------------------------------------------------------------------------------------
void BuildIntervalsCtrol::initialize_strings()
{
    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated

        // button row labels
    m_sNotesRowLabel[0] = to_std_string( _("Double flat") );
    m_sNotesRowLabel[1] = to_std_string( _("Flat") );
    m_sNotesRowLabel[2] = to_std_string( _("Natural") );
    m_sNotesRowLabel[3] = to_std_string( _("Sharp") );
    m_sNotesRowLabel[4] = to_std_string( _("Double sharp") );

        // button column labels
    m_sNotesColumnLabel[0] = to_std_string( _("C") );
    m_sNotesColumnLabel[1] = to_std_string( _("D") );
    m_sNotesColumnLabel[2] = to_std_string( _("E") );
    m_sNotesColumnLabel[3] = to_std_string( _("F") );
    m_sNotesColumnLabel[4] = to_std_string( _("G") );
    m_sNotesColumnLabel[5] = to_std_string( _("A") );
    m_sNotesColumnLabel[6] = to_std_string( _("B") );

        //button labels (for notes)
    m_sNotesButtonLabel[0] = to_std_string( _("bb C") );
    m_sNotesButtonLabel[1] = to_std_string( _("bb D") );
    m_sNotesButtonLabel[2] = to_std_string( _("bb E") );
    m_sNotesButtonLabel[3] = to_std_string( _("bb F") );
    m_sNotesButtonLabel[4] = to_std_string( _("bb G") );
    m_sNotesButtonLabel[5] = to_std_string( _("bb A") );
    m_sNotesButtonLabel[6] = to_std_string( _("bb B") );

    m_sNotesButtonLabel[7] = to_std_string( _("b C") );
    m_sNotesButtonLabel[8] = to_std_string( _("b D") );
    m_sNotesButtonLabel[9] = to_std_string( _("b E") );
    m_sNotesButtonLabel[10] = to_std_string( _("b F") );
    m_sNotesButtonLabel[11] = to_std_string( _("b G") );
    m_sNotesButtonLabel[12] = to_std_string( _("b A") );
    m_sNotesButtonLabel[13] = to_std_string( _("b B") );

    m_sNotesButtonLabel[14] = to_std_string( _("C") );
    m_sNotesButtonLabel[15] = to_std_string( _("D") );
    m_sNotesButtonLabel[16] = to_std_string( _("E") );
    m_sNotesButtonLabel[17] = to_std_string( _("F") );
    m_sNotesButtonLabel[18] = to_std_string( _("G") );
    m_sNotesButtonLabel[19] = to_std_string( _("A") );
    m_sNotesButtonLabel[20] = to_std_string( _("B") );

    m_sNotesButtonLabel[21] = to_std_string( _("# C") );
    m_sNotesButtonLabel[22] = to_std_string( _("# D") );
    m_sNotesButtonLabel[23] = to_std_string( _("# E") );
    m_sNotesButtonLabel[24] = to_std_string( _("# F") );
    m_sNotesButtonLabel[25] = to_std_string( _("# G") );
    m_sNotesButtonLabel[26] = to_std_string( _("# A") );
    m_sNotesButtonLabel[27] = to_std_string( _("# B") );

    m_sNotesButtonLabel[28] = to_std_string( _("x C") );
    m_sNotesButtonLabel[29] = to_std_string( _("x D") );
    m_sNotesButtonLabel[30] = to_std_string( _("x E") );
    m_sNotesButtonLabel[31] = to_std_string( _("x F") );
    m_sNotesButtonLabel[32] = to_std_string( _("x G") );
    m_sNotesButtonLabel[33] = to_std_string( _("x A") );
    m_sNotesButtonLabel[34] = to_std_string( _("x B") );
}




//=======================================================================================
// IdfyIntervalsCtrol implementation
//=======================================================================================


//internationalized strings
static string m_sIntvButtonLabel[IdfyIntervalsCtrol::k_num_buttons];
static string m_sIntvRowLabel[IdfyIntervalsCtrol::k_num_rows];
static string m_sIntvColumnLabel[IdfyIntervalsCtrol::k_num_cols];
static string m_sIntvNumber[8];


//indexes for special buttons
#define lmIDX_UNISON        48
#define lmIDX_SEMITONE      49
#define lmIDX_TONE          50


//---------------------------------------------------------------------------------------
IdfyIntervalsCtrol::IdfyIntervalsCtrol(long dynId, ApplicationScope& appScope,
                                       DocumentWindow* pCanvas)
    : TheoIntervalsCtrol(dynId, appScope, pCanvas)
{
}

//---------------------------------------------------------------------------------------
IdfyIntervalsCtrol::~IdfyIntervalsCtrol()
{
}

//---------------------------------------------------------------------------------------
void IdfyIntervalsCtrol::initialize_ctrol()
{
    m_pConstrains = dynamic_cast<TheoIntervalsConstrains*>(m_pBaseConstrains);

    //set key
    m_sKeyPrefix = wxString::Format(_T("/IdfyIntval/%s/"),
                                    m_pBaseConstrains->GetSection().c_str() );

    //create controls
    m_pConstrains->SetGenerationModeSupported(k_learning_mode, true);
    m_pConstrains->SetGenerationModeSupported(k_practise_mode, true);
    m_pConstrains->SetGenerationMode(k_learning_mode);
    create_controls();

    //update display
    if (m_pCounters && m_fCountersValid)
        m_pCounters->UpdateDisplay();

    if (m_pConstrains->is_theory_mode())
        new_problem();
}

//---------------------------------------------------------------------------------------
void IdfyIntervalsCtrol::create_answer_buttons(LUnits height, LUnits spacing)
{
    ImoStyle* pDefStyle = m_pDoc->get_default_style();
    ImoInlineWrapper* pBox;

    //create 48 buttons for the answers: six rows, eight buttons per row,
    //plus two additional buttons, for 'unison' and 'chromatic semitone'

    ImoStyle* pBtStyle = m_pDoc->create_private_style();
    pBtStyle->font_name("sans-serif")->font_size(8.0f);

    ImoStyle* pRowStyle = m_pDoc->create_private_style();
    pRowStyle->font_size(10.0f)->margin_bottom(100.0f);
    pRowStyle->vertical_align(ImoStyle::k_valign_middle);

    USize buttonSize(1500.0f, height);
    USize bigButtonSize(3200.0f, height);
    LUnits firstRowWidth = 4000.0f;
    LUnits otherRowsWidth = buttonSize.width + spacing;
    LUnits unisonRowsWidth = bigButtonSize.width + 2.0f * spacing;


    int iB;
    for (iB=0; iB < k_num_buttons; iB++) {
        m_pAnswerButton[iB] = NULL;
    }

    //row with buttons for unison and related
    ImoParagraph* pUnisonRow = m_pDyn->add_paragraph(pRowStyle);

        //spacer to skip the labels
    pBox = pUnisonRow->add_inline_box(firstRowWidth, pDefStyle);

        //unison button
    pBox = pUnisonRow->add_inline_box(unisonRowsWidth, pDefStyle);
    iB = lmIDX_UNISON;
    m_pAnswerButton[iB] = pBox->add_button(m_sIntvButtonLabel[iB],
                                           bigButtonSize, pBtStyle);

        // "chromatic semitone" button
    pBox = pUnisonRow->add_inline_box(unisonRowsWidth, pDefStyle);
    iB = lmIDX_SEMITONE;
    m_pAnswerButton[iB] = pBox->add_button(m_sIntvButtonLabel[iB],
                                           bigButtonSize, pBtStyle);

        // "chromatic tone" button
    pBox = pUnisonRow->add_inline_box(unisonRowsWidth, pDefStyle);
    iB = lmIDX_TONE;
    m_pAnswerButton[iB] = pBox->add_button(m_sIntvButtonLabel[iB],
                                           bigButtonSize, pBtStyle);


    //Now main keyboard with all other buttons

    //row with column labels
    ImoParagraph* pKeyboardRow = m_pDyn->add_paragraph(pRowStyle);

    //spacer
    pBox = pKeyboardRow->add_inline_box(firstRowWidth, pDefStyle);

    for (int iCol=0; iCol < k_num_cols; iCol++)
    {
        pBox = pKeyboardRow->add_inline_box(otherRowsWidth, pDefStyle);
        m_pColumnLabel[iCol] = pBox->add_text_item(m_sIntvColumnLabel[iCol],
                                                   pRowStyle);
    }

    //remaining rows with buttons
    for (int iRow=0; iRow < k_num_rows; iRow++)
    {
        ImoParagraph* pKeyboardRow = m_pDyn->add_paragraph(pRowStyle);

        pBox = pKeyboardRow->add_inline_box(firstRowWidth, pDefStyle);
        m_pRowLabel[iRow] = pBox->add_text_item(m_sIntvRowLabel[iRow], pRowStyle);

        // the buttons for this row
        for (int iCol=0; iCol < k_num_cols; iCol++)
        {
            iB = iCol + iRow * k_num_cols;    // button index: 0 .. 47
            pBox = pKeyboardRow->add_inline_box(otherRowsWidth, pDefStyle);
            m_pAnswerButton[iB] = pBox->add_button(m_sIntvButtonLabel[iB],
                                                   buttonSize, pBtStyle);

            if (m_sIntvButtonLabel[iB].empty())
            {
                m_pAnswerButton[iB]->set_visible(false);
                m_pAnswerButton[iB]->enable(false);
            }
        }
    }

    //inform base class about the settings
    set_buttons(m_pAnswerButton, k_num_buttons);
}

//---------------------------------------------------------------------------------------
void IdfyIntervalsCtrol::enable_buttons(bool fEnable)
{
    if (m_pConstrains->GetProblemLevel() == 0)
    {
        for (int iB=0; iB < 7; iB++)
            m_pAnswerButton[iB]->enable(fEnable);
    }
    else
    {
        for (int iB=0; iB < k_num_buttons; iB++)
        {
            if (!m_sIntvButtonLabel[iB].empty())
                m_pAnswerButton[iB]->enable(fEnable);
        }
    }
}

//---------------------------------------------------------------------------------------
wxString IdfyIntervalsCtrol::prepare_scores()
{
    //The problem interval has been set.
    //This method must prepare the problem score and set variables:
    //  m_pProblemScore - The score with the problem to propose
    //  m_pSolutionScore - The score with the solution or NULL if it is the
    //              same score than the problem score.
    //  m_sAnswer - the message to present when displaying the solution
    //  m_nRespIndex - the number of the button for the right answer
    //  m_nPlayMM - the speed to play the score
    //
    //It must return the message to display to introduce the problem.

    //prepare LDP pattern
    string sPattern0 = "(n ";
    sPattern0 += m_fpStart.to_rel_ldp_name(m_nKey);
    sPattern0 += " w)";

    string sPattern1 = "(n ";
    sPattern1 += m_fpEnd.to_rel_ldp_name(m_nKey);
    sPattern1 += " w)";

    wxLogMessage(_T("[IdfyIntervalsCtrol::prepare_scores] notes = %s %s"),
                 to_wx_string(sPattern0).c_str(),
                 to_wx_string(sPattern1).c_str() );

    //create the score with the interval
    ImoScore* pScore = static_cast<ImoScore*>(ImFactory::inject(k_imo_score, m_pDoc));
    pScore->set_long_option("Render.SpacingMethod", long(k_spacing_fixed));
    ImoInstrument* pInstr = pScore->add_instrument();    // (0,0,_T(""));		//MIDI channel 0, MIDI instr 0
    ImoSystemInfo* pInfo = pScore->get_first_system_info();
    pInfo->set_top_system_distance( pInstr->tenths_to_logical(30) );     // 3 lines
    pInstr->add_clef(m_nClef);
    pInstr->add_key_signature(m_nKey);
    pInstr->add_time_signature(4 ,4, NO_VISIBLE);
    pInstr->add_spacer(30);       // 3 lines
    pInstr->add_object( sPattern0 );
    pInstr->add_barline(ImoBarline::k_simple, NO_VISIBLE);    //so that accidental doesn't affect 2nd note
    pInstr->add_object( sPattern1 );
    pInstr->add_spacer(75);       // 7.5 lines
    pInstr->add_barline(ImoBarline::k_simple, NO_VISIBLE);

    pScore->close();


    //compute button index for right answer
    if (m_fpIntv == FIntval(0))
        m_nRespIndex = lmIDX_UNISON;
    else if (m_fpIntv == FIntval(1))
        m_nRespIndex = lmIDX_SEMITONE;
    else if (m_fpIntv == FIntval(2))
        m_nRespIndex = lmIDX_TONE;
    else
    {
        int iRow;
        int iCol = m_fpIntv.get_number() - 2;
        switch (m_fpIntv.get_type())
        {
            case k_double_diminished:   iRow = 0;   break;
            case k_diminished:          iRow = 1;   break;
            case k_minor:               iRow = 2;   break;
            case k_major:               iRow = 3;   break;
            case k_perfect:             iRow = 3;   break;
            case k_augmented:           iRow = 4;   break;
            case k_double_augmented:    iRow = 5;   break;
            default:
                wxASSERT(false);
        }
        m_nRespIndex = iCol + (iRow-m_nFirstRow) * k_num_cols;
    }
    //fix button index for level 0 (only numbers)
    if (m_pConstrains->GetProblemLevel() == 0)
        m_nRespIndex = m_fpIntv.get_number() - 1;

    //set score with the problem
    m_pProblemScore = pScore;
    m_pSolutionScore = NULL;

    //return question string
    return _("Identify the next interval:");
}

//---------------------------------------------------------------------------------------
void IdfyIntervalsCtrol::reconfigure_keyboard()
{
    // Reconfigure answer keyboard for the new settings

    if (m_pConstrains->GetProblemLevel() == 0)
    {
        //Level 0: answer buttons only to name interval number, no qualification

        //hide all column labels
        for (int iCol=0; iCol < k_num_cols; iCol++)
        {
            m_pColumnLabel[iCol]->set_visible(false);
        }

        //show first eight buttons and change their labels
        int iB;
        for (iB=0; iB < 8; iB++)
        {
            m_pAnswerButton[iB]->set_label( m_sIntvNumber[iB] );
            m_pAnswerButton[iB]->set_visible(true);
            m_pAnswerButton[iB]->enable(true);
        }
        //hide all buttons remaining buttons
        for (; iB < k_num_buttons; iB++)
        {
            m_pAnswerButton[iB]->set_visible(false);
            m_pAnswerButton[iB]->enable(false);
        }

        //hide row labels
        for (int iRow=0; iRow < k_num_rows; iRow++)
        {
            m_pRowLabel[iRow]->set_visible(false);
        }
    }

    else
    {
        //show all column labels
        for (int iCol=0; iCol < k_num_cols; iCol++)
        {
            m_pColumnLabel[iCol]->set_visible(true);
        }

        //determine rows to show
        m_nFirstRow = 0;
        int nLastRow = k_num_rows;
        bool fUnison = true;
        bool fSemitone = true;
        bool fTone = true;
        if (m_pConstrains->GetProblemLevel() == 1)
        {
            //Only minor and perfect/major
            m_nFirstRow = 2;
            nLastRow = 4;
            fUnison = true;
            fSemitone = false;
            fTone = false;
        }
        else if (m_pConstrains->GetProblemLevel() == 2)
        {
            //also augmented and diminished
            m_nFirstRow = 1;
            nLastRow = 5;
            fUnison = true;
            fSemitone = true;
            fTone = false;
        }
        else
        {
            //all columns
            m_nFirstRow = 0;
            nLastRow = 6;
            fUnison = true;
            fSemitone = true;
            fTone = true;
        }

        //show all buttons and change their labels
        int nShift = m_nFirstRow * k_num_cols;
        int iLast = (nLastRow-m_nFirstRow) * k_num_cols;
        int iB;
        for (iB=0; iB < iLast; iB++)
        {
            int iLBL = iB + nShift;
            m_pAnswerButton[iB]->set_label( m_sIntvButtonLabel[iLBL] );
            bool fEnable = !m_sIntvButtonLabel[iLBL].empty();
            m_pAnswerButton[iB]->set_visible(fEnable);
            m_pAnswerButton[iB]->enable(fEnable);
        }
        for (; iB < k_num_buttons-2; iB++)
        {
            m_pAnswerButton[iB]->set_visible(false);
            m_pAnswerButton[iB]->enable(false);
        }
        m_pAnswerButton[lmIDX_UNISON]->set_visible(fUnison);
        m_pAnswerButton[lmIDX_UNISON]->enable(fUnison);
        m_pAnswerButton[lmIDX_SEMITONE]->set_visible(fSemitone);
        m_pAnswerButton[lmIDX_SEMITONE]->enable(fSemitone);
        m_pAnswerButton[lmIDX_TONE]->set_visible(fTone);
        m_pAnswerButton[lmIDX_TONE]->enable(fTone);

        //show row labels
        int iRow, iLBL;
        for (iRow=0, iLBL = m_nFirstRow; iLBL < nLastRow; iRow++, iLBL++)
        {
            m_pRowLabel[iRow]->set_text( m_sIntvRowLabel[iLBL] );
            m_pRowLabel[iRow]->set_visible(true);
        }
        for (; iRow < k_num_rows; iRow++)
        {
            m_pRowLabel[iRow]->set_visible(false);
        }
    }
    m_pDoc->set_dirty();
}

//---------------------------------------------------------------------------------------
void IdfyIntervalsCtrol::initialize_strings()
{
    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated

        // button row labels
    m_sIntvRowLabel[0] = to_std_string( _("dd - double diminished") );
    m_sIntvRowLabel[1] = to_std_string( _("dim - diminished") );
    m_sIntvRowLabel[2] = to_std_string( _("m - minor") );
    m_sIntvRowLabel[3] = to_std_string( _("M, P - Major, Perfect") );
    m_sIntvRowLabel[4] = to_std_string( _("aug - augmented") );
    m_sIntvRowLabel[5] = to_std_string( _("da - double augmented") );

        // button column labels
    m_sIntvColumnLabel[0] = to_std_string( _("2nd") );
    m_sIntvColumnLabel[1] = to_std_string( _("3rd") );
    m_sIntvColumnLabel[2] = to_std_string( _("4th") );
    m_sIntvColumnLabel[3] = to_std_string( _("5th") );
    m_sIntvColumnLabel[4] = to_std_string( _("6th") );
    m_sIntvColumnLabel[5] = to_std_string( _("7th") );
    m_sIntvColumnLabel[6] = to_std_string( _("8ve") );
    m_sIntvColumnLabel[7] = "";

        //button labels (intervals)
    m_sIntvButtonLabel[0] = to_std_string( _("dd2") );
    m_sIntvButtonLabel[1] = to_std_string( _("dd3") );
    m_sIntvButtonLabel[2] = to_std_string( _("dd4") );
    m_sIntvButtonLabel[3] = to_std_string( _("dd5") );
    m_sIntvButtonLabel[4] = to_std_string( _("dd6") );
    m_sIntvButtonLabel[5] = to_std_string( _("dd7") );
    m_sIntvButtonLabel[6] = to_std_string( _("dd8") );
    m_sIntvButtonLabel[7] = "";

    m_sIntvButtonLabel[8]  = to_std_string( _("dim2") );
    m_sIntvButtonLabel[9]  = to_std_string( _("dim3") );
    m_sIntvButtonLabel[10] = to_std_string( _("dim4") );
    m_sIntvButtonLabel[11] = to_std_string( _("dim5") );
    m_sIntvButtonLabel[12] = to_std_string( _("dim6") );
    m_sIntvButtonLabel[13] = to_std_string( _("dim7") );
    m_sIntvButtonLabel[14] = to_std_string( _("dim8") );
    m_sIntvButtonLabel[15] = "";

    m_sIntvButtonLabel[16] = to_std_string( _("m2") );
    m_sIntvButtonLabel[17] = to_std_string( _("m3") );
    m_sIntvButtonLabel[18] = "";
    m_sIntvButtonLabel[19] = "";
    m_sIntvButtonLabel[20] = to_std_string( _("m6") );
    m_sIntvButtonLabel[21] = to_std_string( _("m7") );
    m_sIntvButtonLabel[22] = "";
    m_sIntvButtonLabel[23] = "";

    m_sIntvButtonLabel[24] = to_std_string( _("M2") );
    m_sIntvButtonLabel[25] = to_std_string( _("M3") );
    m_sIntvButtonLabel[26] = to_std_string( _("P4") );
    m_sIntvButtonLabel[27] = to_std_string( _("P5") );
    m_sIntvButtonLabel[28] = to_std_string( _("M6") );
    m_sIntvButtonLabel[29] = to_std_string( _("M7") );
    m_sIntvButtonLabel[30] = to_std_string( _("P8") );
    m_sIntvButtonLabel[31] = "";

    m_sIntvButtonLabel[32] = to_std_string( _("aug2") );
    m_sIntvButtonLabel[33] = to_std_string( _("aug3") );
    m_sIntvButtonLabel[34] = to_std_string( _("aug4") );
    m_sIntvButtonLabel[35] = to_std_string( _("aug5") );
    m_sIntvButtonLabel[36] = to_std_string( _("aug6") );
    m_sIntvButtonLabel[37] = to_std_string( _("aug7") );
    m_sIntvButtonLabel[38] = to_std_string( _("aug8") );
    m_sIntvButtonLabel[39] = "";

    m_sIntvButtonLabel[40] = to_std_string( _("da2") );
    m_sIntvButtonLabel[41] = to_std_string( _("da3") );
    m_sIntvButtonLabel[42] = to_std_string( _("da4") );
    m_sIntvButtonLabel[43] = to_std_string( _("da5") );
    m_sIntvButtonLabel[44] = to_std_string( _("da6") );
    m_sIntvButtonLabel[45] = to_std_string( _("da7") );
    m_sIntvButtonLabel[46] = to_std_string( _("da7") );
    m_sIntvButtonLabel[47] = "";

    m_sIntvButtonLabel[48] = to_std_string( _("Unison") );
    m_sIntvButtonLabel[49] = to_std_string( _("Chromatic semitone") );
    m_sIntvButtonLabel[50] = to_std_string( _("Chromatic tone") );

    //Buttons for interval number
    m_sIntvNumber[0] = to_std_string( _("Unison") );
    m_sIntvNumber[1] = to_std_string( _("2nd") );
    m_sIntvNumber[2] = to_std_string( _("3rd") );
    m_sIntvNumber[3] = to_std_string( _("4th") );
    m_sIntvNumber[4] = to_std_string( _("5th") );
    m_sIntvNumber[5] = to_std_string( _("6th") );
    m_sIntvNumber[6] = to_std_string( _("7th") );
    m_sIntvNumber[7] = to_std_string( _("8ve") );
}


}   //namespace lenmus
