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

#include "lenmus_ear_intervals_ctrol.h"

#include "lenmus_ear_intervals_constrains.h"
#include "lenmus_ear_intervals_ctrol_params.h"
#include "lenmus_dlg_cfg_ear_intervals.h"
#include "lenmus_constrains.h"
#include "lenmus_generators.h"
#include "lenmus_document_canvas.h"
//#include "../auxmusic/Conversion.h"
#include "lenmus_interval.h"

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

//=======================================================================================
// Implementation of EarIntervalsCtrol
//=======================================================================================

//button labels (translated)
static string m_sButtonLabel[EarIntervalsCtrol::k_num_buttons];


//---------------------------------------------------------------------------------------
EarIntervalsCtrol::EarIntervalsCtrol(long dynId, ApplicationScope& appScope,
                                     DocumentWindow* pCanvas)
    : OneScoreCtrol(dynId, appScope, pCanvas)
{
}

//---------------------------------------------------------------------------------------
EarIntervalsCtrol::~EarIntervalsCtrol()
{
    delete m_pBaseConstrains;
}

//---------------------------------------------------------------------------------------
void EarIntervalsCtrol::get_ctrol_options_from_params()
{
    m_pBaseConstrains = LENMUS_NEW EarIntervalsConstrains("EarIntervals", m_appScope);
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
    m_pConstrains->set_height(4000.0);      //minimum problem box height = 40mm
    m_nPlayMM = 320;    //score build with whole notes, so metronome rate 320
                        //will force to play at 80 notes/minute

    //allow to play chords
    m_fAscending = true;
    m_nKey = k_key_C;
    m_pitch[0] = FPitch(k_step_C, k_octave_4, 0);

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
void EarIntervalsCtrol::create_answer_buttons(LUnits height, LUnits WXUNUSED(spacing))
{
    //create up to 25 buttons for the answers: five rows, five buttons per row
    //Buttons are created disabled and no visible

    ImoStyle* pDefStyle = m_pDoc->get_default_style();
    ImoInlineWrapper* pBox;

    ImoStyle* pBtStyle = m_pDoc->create_private_style();
    pBtStyle->font_name( "sans");
    pBtStyle->font_size( 8.0f);

    ImoStyle* pRowStyle = m_pDoc->create_private_style();
    pRowStyle->font_size( 10.0f);
    pRowStyle->margin_bottom( 0.0f);

    USize buttonSize(3300.0f, height);
    LUnits rowWidth = 3500.0f;

    int iB = 0;
    for (iB=0; iB < k_num_buttons; iB++)
        m_pAnswerButton[iB] = nullptr;

    LibraryScope* pLibScope = m_appScope.get_lomse().get_library_scope();
    for (int iRow=0; iRow < k_num_rows; iRow++)
    {
        ImoParagraph* pKeyboardRow = m_pDyn->add_paragraph(pRowStyle);

        for (int iCol=0; iCol < k_num_cols; iCol++)
        {
            iB = iCol + iRow * k_num_cols;    // button index: 0 .. 24
            pBox = pKeyboardRow->add_inline_box(rowWidth, pDefStyle);
            m_pAnswerButton[iB] = pBox->add_button(*pLibScope, "Undefined",
                                                   buttonSize, pBtStyle);
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
    m_pDoc->set_dirty();

//    if (m_pConstrains->is_theory_mode())
//        new_problem();
//    else
//        m_pProblemScore = nullptr;
}

//---------------------------------------------------------------------------------------
wxDialog* EarIntervalsCtrol::get_settings_dialog()
{
    wxWindow* pParent = dynamic_cast<wxWindow*>(m_pCanvas);
    return LENMUS_NEW DlgCfgEarIntervals(pParent, m_pConstrains);
}

//---------------------------------------------------------------------------------------
ImoScore* EarIntervalsCtrol::prepare_aux_score(int nButton)
{

    // Get the interval associated to the pressed button
    FIntval intval;
    switch (m_nRealIntval[nButton])
    {
        case ein_1:         intval = k_interval_p1;    break;
        case ein_2min:      intval = k_interval_m2;    break;
        case ein_2maj:      intval = k_interval_M2;    break;
        case ein_3min:      intval = k_interval_m3;    break;
        case ein_3maj:      intval = k_interval_M3;    break;
        case ein_4:         intval = k_interval_p4;    break;
        case ein_4aug:      intval = k_interval_a4;    break;
        case ein_5:         intval = k_interval_p5;    break;
        case ein_6min:      intval = k_interval_m6;    break;
        case ein_6maj:      intval = k_interval_M6;    break;
        case ein_7min:      intval = k_interval_m7;    break;
        case ein_7maj:      intval = k_interval_M7;    break;
        case ein_8:         intval = k_interval_p8;    break;
        case ein_9min:      intval = k_interval_m9;    break;
        case ein_9maj:      intval = k_interval_M9;    break;
        case ein_10min:     intval = k_interval_m10;   break;
        case ein_10maj:     intval = k_interval_M10;   break;
        case ein_11:        intval = k_interval_p11;   break;
        case ein_11aug:     intval = k_interval_a11;   break;
        case ein_12:        intval = k_interval_p12;   break;
        case ein_13min:     intval = k_interval_m13;   break;
        case ein_13maj:     intval = k_interval_M13;   break;
        case ein_14min:     intval = k_interval_m14;   break;
        case ein_14maj:     intval = k_interval_M14;   break;
        case ein_2oct:      intval = k_interval_p15;   break;
    }

    //prepare the requested interval
    FPitch endNote;
    FPitch startNote;
    if (m_fAscending)
    {
        startNote = m_pitch[0];
        endNote = startNote + intval;
    }
    else
    {
        startNote = m_pitch[1];
        endNote = startNote - intval;
    }
    return prepare_score(startNote, endNote);
}

//---------------------------------------------------------------------------------------
void EarIntervalsCtrol::set_problem_space()
{
    //Do nothing. For now, this exercise does not use Leitner method
}

//---------------------------------------------------------------------------------------
wxString EarIntervalsCtrol::set_new_problem()
{
    //This method must prepare the problem score and set variables:
    //  m_pProblemScore - The score with the problem to propose
    //  m_pSolutionScore - The score with the solution or nullptr if it is the
    //              same score than the problem score.
    //  m_sAnswer - the message to present when displaying the solution
    //  m_nRespIndex - the number of the button for the right answer
    //  m_nPlayMM - the speed to play the score
    //
    //It must return the message to display to introduce the problem.


    //choose if harmonic or melodic
    if (m_pConstrains->IsTypeAllowed(0) &&
        !(m_pConstrains->IsTypeAllowed(1) || m_pConstrains->IsTypeAllowed(2)))
    {
        // if only harmonic force it
        m_fHarmonic = true;
    }
    else
        m_fHarmonic = m_pConstrains->IsTypeAllowed(0) && RandomGenerator::flip_coin();

    // select interval type: ascending or descending
    if (m_fHarmonic)
    {
        // if harmonic it doesn't matter. Choose ascending
        m_fAscending = true;
    }
    else
    {
        if (m_pConstrains->IsTypeAllowed(1) && !m_pConstrains->IsTypeAllowed(2))
            m_fAscending = true;
        else if (!m_pConstrains->IsTypeAllowed(1) && m_pConstrains->IsTypeAllowed(2))
            m_fAscending = false;
        else
            m_fAscending = RandomGenerator::flip_coin();
    }

    // select a random key signature satisfying the constraints
    if (m_pConstrains->OnlyNatural())
        m_nKey = RandomGenerator::generate_key(m_pConstrains->GetKeyConstrains());
    else
        m_nKey = k_key_C;

    // generate interval
    Interval oIntv(m_pConstrains->OnlyNatural(), m_pConstrains->MinNote(),
        m_pConstrains->MaxNote(), m_pConstrains->AllowedIntervals(), m_fAscending, m_nKey);
    m_pitch[0] = oIntv.get_pitch(0);
    m_pitch[1] = oIntv.get_pitch(1);

    //prepare the score
    m_pProblemScore = prepare_score(m_pitch[0], m_pitch[1]);
    m_pSolutionScore = nullptr;

    //compute the right answer
    m_sAnswer = oIntv.get_interval_name();

    //compute the index for the button that corresponds to the right answer
    int i;
    for (i = 0; i <= m_nValidIntervals; i++)
    {
        if (m_nRealIntval[i] == oIntv.get_num_semitones())
            break;
    }
    m_nRespIndex = i;

	return _("Press 'Play' to hear the problem again.");
//    return "";
}

//---------------------------------------------------------------------------------------
ImoScore* EarIntervalsCtrol::prepare_score(FPitch note0, FPitch note1)
{
    //create a score with the interval

    string sPattern0 = "(n " + note0.to_rel_ldp_name(m_nKey) + " w)";
    string sPattern1 = "(n " + note1.to_rel_ldp_name(m_nKey) + " w)";

    AScore score = m_doc.create_object(k_obj_score).downcast_to_score();
    ImoScore* pScore = score.internal_object();
    pScore->set_long_option("Render.SpacingMethod", long(k_spacing_fixed));
    pScore->set_long_option("StaffLines.Truncate", k_truncate_always);
    ImoInstrument* pInstr = pScore->add_instrument();
        // (g_pMidi->get_default_voice_channel(), g_pMidi->get_default_voice_instr(), "");
    ImoSystemInfo* pInfo = pScore->get_first_system_info();
    pInfo->set_top_system_distance( pInstr->tenths_to_logical(30) );     // 3 lines
    pInstr->add_clef( k_clef_G2 );
    pInstr->add_key_signature(m_nKey);
    pInstr->add_time_signature(4, 4, k_no_visible);
    if (m_fHarmonic)
    {
        string sPattern = "(chord " + sPattern0 + sPattern1 + ")";
        pInstr->add_staff_objects( sPattern );
        //TO_FIX: if note1 has an accidental it is automatically included
        //        in note2- How to avoid it?
    }
    else
    {
        pInstr->add_object( sPattern0 );
        pInstr->add_spacer(20);
        pInstr->add_barline(k_barline_simple, k_no_visible);    //so that accidental doesn't affect 2nd note
        pInstr->add_object( sPattern1 );
    }
    pInstr->add_spacer(60);
    pInstr->add_barline(k_barline_simple, k_no_visible);

    pScore->end_of_changes();
    return pScore;
}


}  //namespace lenmus
