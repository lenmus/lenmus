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

#include "lenmus_ear_intervals_ctrol.h"

#include "lenmus_ear_intervals_constrains.h"
#include "lenmus_ear_intervals_ctrol_params.h"
#include "lenmus_dlg_cfg_ear_intervals.h"
#include "lenmus_constrains.h"
#include "lenmus_generators.h"
#include "lenmus_score_canvas.h"
//#include "../auxmusic/Conversion.h"

//lomse
#include <lomse_doorway.h>
#include <lomse_internal_model.h>
#include <lomse_im_note.h>
#include <lomse_staffobjs_table.h>
#include <lomse_im_factory.h>
using namespace lomse;

//wxWidgets
#include <wx/wxprec.h>


namespace lenmus
{

//------------------------------------------------------------------------------------
// Implementation of EarIntervalsCtrol


//button labels (translated)
static string m_sButtonLabel[EarIntervalsCtrol::k_num_buttons];


//---------------------------------------------------------------------------------------
EarIntervalsCtrol::EarIntervalsCtrol(long dynId, ApplicationScope& appScope,
                                     DocumentCanvas* pCanvas)
    : OneScoreCtrol(dynId, appScope, pCanvas)
{
}

//---------------------------------------------------------------------------------------
void EarIntervalsCtrol::get_ctrol_options_from_params()
{
    m_pBaseConstrains = new EarIntervalsConstrains(_T("EarIntervals"), m_appScope);
    EarIntervalsCtrolParms builder(m_pBaseConstrains);
    builder.process_params( m_pDyn->get_params() );
}

//---------------------------------------------------------------------------------------
void EarIntervalsCtrol::initialize_ctrol()
{
    m_pConstrains = dynamic_cast<EarIntervalsConstrains*>(m_pBaseConstrains);

    //ctrol options
    m_pConstrains->set_theory_mode(false);
    m_pConstrains->SetButtonsEnabledAfterSolution(true);
    m_nPlayMM = 320;    //score build with whole notes, so metronome rate 320
                        //will force to play at 80 notes/minute

    //allow to play chords
    m_fAscending = true;
    m_nKey = k_key_C;
//TODO 5.0 commented out
//    m_tNote[0].nAccidentals = 0;    //c4
//    m_tNote[0].nOctave = 4;
//    m_tNote[0].nStep = 0;
//    m_tNote[0].nStepSemitones = 0;

    create_controls();
}

//---------------------------------------------------------------------------------------
void EarIntervalsCtrol::initialize_strings()
{
    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated
    m_sButtonLabel[0] = to_std_string( _("Unison") );
    m_sButtonLabel[1] = to_std_string( _("minor 2nd") );
    m_sButtonLabel[2] = to_std_string( _("major 2nd") );
    m_sButtonLabel[3] = to_std_string( _("minor 3rd") );
    m_sButtonLabel[4] = to_std_string( _("major 3rd") );
    m_sButtonLabel[5] = to_std_string( _("4th perfect") );
    m_sButtonLabel[6] = to_std_string( _("aug.4th/dim.5th") );
    m_sButtonLabel[7] = to_std_string( _("perfect 5th") );
    m_sButtonLabel[8] = to_std_string( _("minor 6th") );
    m_sButtonLabel[9] = to_std_string( _("major 6th") );
    m_sButtonLabel[10] = to_std_string( _("minor 7th") );
    m_sButtonLabel[11] = to_std_string( _("major 7th") );
    m_sButtonLabel[12] = to_std_string( _("perfect 8th") );
    m_sButtonLabel[13] = to_std_string( _("minor 9th") );
    m_sButtonLabel[14] = to_std_string( _("major 9th") );
    m_sButtonLabel[15] = to_std_string( _("minor 10th") );
    m_sButtonLabel[16] = to_std_string( _("major 10th") );
    m_sButtonLabel[17] = to_std_string( _("perfect 11th") );
    m_sButtonLabel[18] = to_std_string( _("aug.11th / dim.12th") );
    m_sButtonLabel[19] = to_std_string( _("perfect 12th") );
    m_sButtonLabel[20] = to_std_string( _("minor 13th") );
    m_sButtonLabel[21] = to_std_string( _("major 13th") );
    m_sButtonLabel[22] = to_std_string( _("minor 14th") );
    m_sButtonLabel[23] = to_std_string( _("major 14th") );
    m_sButtonLabel[24] = to_std_string( _("two octaves") );

}

//---------------------------------------------------------------------------------------
void EarIntervalsCtrol::create_answer_buttons(LUnits height, LUnits spacing)
{
    //create up to 25 buttons for the answers: five rows, five buttons per row
    //Buttons are created disabled and no visible

    ImoStyle* pDefStyle = m_pDoc->get_default_style();
    ImoInlineWrapper* pBox;

    ImoStyle* pBtStyle = m_pDoc->create_private_style();
    pBtStyle->set_string_property(ImoStyle::k_font_name, "sans-serif");
    pBtStyle->set_float_property(ImoStyle::k_font_size, 8.0f);

    ImoStyle* pRowStyle = m_pDoc->create_private_style();
    pRowStyle->set_lunits_property(ImoStyle::k_font_size, 10.0f);
    pRowStyle->set_lunits_property(ImoStyle::k_margin_bottom, 0.0f);

    USize buttonSize(3000.0f, height);
    LUnits rowWidth = 3200.0f;

    int iB = 0;
    for (iB=0; iB < k_num_buttons; iB++)
        m_pAnswerButton[iB] = NULL;

    for (int iRow=0; iRow < k_num_rows; iRow++)
    {
        ImoParagraph* pKeyboardRow = m_pDyn->add_paragraph(pRowStyle);

        for (int iCol=0; iCol < k_num_cols; iCol++)
        {
            iB = iCol + iRow * k_num_cols;    // button index: 0 .. 24
            pBox = pKeyboardRow->add_inline_box(rowWidth, pDefStyle);
            m_pAnswerButton[iB] = pBox->add_button("Undefined", buttonSize, pBtStyle);
            m_pAnswerButton[iB]->set_visible(false);
            m_pAnswerButton[iB]->enable(false);
        }
    }

    //inform base class about the settings
    set_buttons(m_pAnswerButton, k_num_buttons);
}

//---------------------------------------------------------------------------------------
void EarIntervalsCtrol::on_settings_changed()
{
    // The settings have been changed. Reconfigure answer keyboard for the new settings

    // compute interval associated to each button
    int j = 0;
    for (int i=0; i < k_num_buttons; i++)
    {
        if (m_pConstrains->IsIntervalAllowed(i))
        {
            m_nRealIntval[j] = i;
            j++;
        }
    }
    m_nValidIntervals = j;


    //set up the button labels
    for (int iB = 0; iB < m_nValidIntervals; iB++)
    {
        m_pAnswerButton[iB]->set_label( m_sButtonLabel[m_nRealIntval[iB]] );
        m_pAnswerButton[iB]->set_visible(true);
        m_pAnswerButton[iB]->enable(false);
    }

    // hide all non used buttons
    if (m_nValidIntervals < k_num_buttons)
    {
        for (int iB = m_nValidIntervals; iB < k_num_buttons; iB++)
            m_pAnswerButton[iB]->set_visible(false);
    }

}

//---------------------------------------------------------------------------------------
wxDialog* EarIntervalsCtrol::get_settings_dialog()
{
    wxWindow* pParent = dynamic_cast<wxWindow*>(m_pCanvas);
    return new DlgCfgEarIntervals(pParent, m_pConstrains);
}

//---------------------------------------------------------------------------------------
void EarIntervalsCtrol::prepare_aux_score(int nButton)
{

    // Get the interval associated to the pressed button
    wxString sCode;
    switch (m_nRealIntval[nButton]) {
        case ein_1:         sCode = _T("p1");    break;
        case ein_2min:      sCode = _T("m2");    break;
        case ein_2maj:      sCode = _T("M2");    break;
        case ein_3min:      sCode = _T("m3");    break;
        case ein_3maj:      sCode = _T("M3");    break;
        case ein_4:         sCode = _T("p4");    break;
        case ein_4aug:      sCode = _T("a4");    break;
        case ein_5:         sCode = _T("p5");    break;
        case ein_6min:      sCode = _T("m6");    break;
        case ein_6maj:      sCode = _T("M6");    break;
        case ein_7min:      sCode = _T("m7");    break;
        case ein_7maj:      sCode = _T("M7");    break;
        case ein_8:         sCode = _T("p8");    break;
        case ein_9min:      sCode = _T("m9");    break;
        case ein_9maj:      sCode = _T("M9");    break;
        case ein_10min:     sCode = _T("m10");   break;
        case ein_10maj:     sCode = _T("M10");   break;
        case ein_11:        sCode = _T("p11");   break;
        case ein_11aug:     sCode = _T("a11");   break;
        case ein_12:        sCode = _T("p12");   break;
        case ein_13min:     sCode = _T("m13");   break;
        case ein_13maj:     sCode = _T("M13");   break;
        case ein_14min:     sCode = _T("m14");   break;
        case ein_14maj:     sCode = _T("M14");   break;
        case ein_2oct:      sCode = _T("p15");   break;
    }

    //prepare the requested interval
    prepare_score(sCode, &m_pAuxScore);

}

//---------------------------------------------------------------------------------------
void EarIntervalsCtrol::set_problem_space()
{
    //Do nothing. For now, this exercise does not use problem spaces
}

//---------------------------------------------------------------------------------------
wxString EarIntervalsCtrol::set_new_problem()
{
    //This method must prepare the problem score and set variables:
    //  m_pProblemScore - The score with the problem to propose
    //  m_pSolutionScore - The score with the solution or NULL if it is the
    //              same score than the problem score.
    //  m_sAnswer - the message to present when displaying the solution
    //  m_nRespIndex - the number of the button for the right answer
    //  m_nPlayMM - the speed to play the score
    //
    //It must return the message to display to introduce the problem.


    //choose if harmonic or melodic
    RandomGenerator oGenerator;
    if (m_pConstrains->IsTypeAllowed(0) &&
        !(m_pConstrains->IsTypeAllowed(1) || m_pConstrains->IsTypeAllowed(2)))
    {
        // if only harmonic (harmonic && !(melodic ascending or descending))
        // force harmonic
        m_fHarmonic = true;
    }
    else {
        m_fHarmonic = m_pConstrains->IsTypeAllowed(0) && oGenerator.FlipCoin();
    }

    // select interval type: ascending or descending
    if (m_fHarmonic) {
        // if harmonic it doesn't matter. Choose ascending
        m_fAscending = true;
    }
    else {
        if (m_pConstrains->IsTypeAllowed(1) && !m_pConstrains->IsTypeAllowed(2))
            m_fAscending = true;
        else if (!m_pConstrains->IsTypeAllowed(1) && m_pConstrains->IsTypeAllowed(2))
            m_fAscending = false;
        else
            m_fAscending = oGenerator.FlipCoin();
    }

    // select a random key signature satisfying the constraints
    if (m_pConstrains->OnlyNatural())
        m_nKey = oGenerator.GenerateKey(m_pConstrains->GetKeyConstrains());
    else
        m_nKey = k_key_C;

//TODO 5.0 commented out
//    // generate interval
//    Interval oIntv(m_pConstrains->OnlyNatural(), m_pConstrains->MinNote(),
//        m_pConstrains->MaxNote(), m_pConstrains->AllowedIntervals(), m_fAscending, m_nKey);
//
//    //save the interval data
//    m_sIntvCode = oIntv.GetIntervalCode();
//    oIntv.GetNoteBits(0, &m_tNote[0]);
//    oIntv.GetNoteBits(1, &m_tNote[1]);
m_sIntvCode = _T("m3");

    // all data ready to prepare the score: proceed
    prepare_score(m_sIntvCode, &m_pProblemScore);

//TODO 5.0 commented out
//    //compute the right answer
//    m_sAnswer = oIntv.GetIntervalName();
//
//    //compute the index for the button that corresponds to the right answer
//    int i;
//    for (i = 0; i <= m_nValidIntervals; i++) {
//        if (m_nRealIntval[i] == oIntv.GetNumSemitones()) break;
//    }
//    m_nRespIndex = i;
m_sAnswer = _T("minr third");
m_nRespIndex = 5;

    return _T("");

}

//---------------------------------------------------------------------------------------
void EarIntervalsCtrol::prepare_score(wxString& sIntvCode, ImoScore** pScore)
{
    //====================================================================================
    //Example of new code for creating a score
    static int iNote = 0;
    static string notes[] = {"(n e4 w)", "(n f4 w)", "(n g4 w)", "(n a4 w)", "(n b4 w)" };

    (*pScore) = static_cast<ImoScore*>(ImFactory::inject(k_imo_score, m_pDoc));
    ImoInstrument* pInstr = (*pScore)->add_instrument();
    pInstr->add_clef(k_clef_G2);
    pInstr->add_object("(n c4 w)");
    pInstr->add_object( notes[(iNote++)%5] );
    pInstr->add_object("(barline simple)");
    //pInstr->add_barline(ImoBarline::k_simple);

    ColStaffObjsBuilder builder;
    builder.build((*pScore));

    m_pProblemScore = (*pScore);
    m_pSolutionScore = NULL;
    //====================================================================================


//    //create the interval
//    NoteBits tBits[2];
//    tBits[0] = m_tNote[0];
//    ComputeInterval( &tBits[0], sIntvCode, m_fAscending, &tBits[1] );
//
//    //delete the previous score
//    if (*pScore) {
//        delete *pScore;
//        *pScore = NULL;
//    }
//
//    //create a score with the interval
//    string sPattern;
//    *pScore = static_cast<ImoScore*>(ImFactory::inject(k_imo_score, m_pDoc));
//    (*pScore)->SetOption(_T("Render.SpacingMethod"), (long)esm_Fixed);
//    ImoInstrument* pInstr = (*pScore)->add_instrument();    // (g_pMidi->DefaultVoiceChannel(),
//							 g_pMidi->DefaultVoiceInstr(), _T(""));
//    (*pScore)->SetTopSystemDistance( pVStaff->TenthsToLogical(30, 1) );     // 3 lines
//    pInstr->add_clef( lmE_Sol );
//    pInstr->add_key_signature(m_nKey);
//    pInstr->add_time_signature(4, 4, NO_VISIBLE);
////    pInstr->add_spacer(30);       // 3 lines
//    //First note
//    sPattern = "(n " + lmConverter::NoteBitsToName(tBits[0], m_nKey) + " w)";
//    pInstr->add_object( sPattern );
//    //second note
//    if (m_fHarmonic)
//        sPattern = "(na ";
//        //todo: is it necessary to avoid propagation of the accidental to the second note
//    else {
//        pInstr->add_spacer(20);
//        pInstr->add_barline(ImoBarline::k_simple, NO_VISIBLE);    //so that accidental doesn't affect 2nd note
//        sPattern = "(n ";
//    }
//    sPattern += lmConverter::NoteBitsToName(tBits[1], m_nKey) + " w)";
//    pInstr->add_object( sPattern );
//    pInstr->add_spacer(60);
//    pInstr->add_barline(ImoBarline::k_simple, NO_VISIBLE);

}


}  //namespace lenmus
