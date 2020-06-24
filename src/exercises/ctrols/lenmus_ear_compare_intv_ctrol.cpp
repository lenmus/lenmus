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

#include "lenmus_ear_compare_intv_ctrol.h"

#include "lenmus_ear_compare_intv_ctrol_params.h"
#include "lenmus_ear_intervals_constrains.h"
#include "lenmus_generators.h"
#include "lenmus_document_canvas.h"
#include "lenmus_interval.h"
#include "lenmus_dlg_cfg_ear_intervals.h"
#include "lenmus_injectors.h"
#include "lenmus_colors.h"

//wxWidgets
#include <wx/wxprec.h>

//lomse
#include <lomse_doorway.h>
#include <lomse_internal_model.h>
#include <lomse_im_note.h>
#include <lomse_im_factory.h>
using namespace lomse;


namespace lenmus
{

//=======================================================================================
// Implementation of EarCompareIntvCtrol
//=======================================================================================
EarCompareIntvCtrol::EarCompareIntvCtrol(long dynId, ApplicationScope& appScope,
                                         DocumentWindow* pCanvas)
    : CompareScoresCtrol(dynId, appScope, pCanvas)
{
}

//---------------------------------------------------------------------------------------
EarCompareIntvCtrol::~EarCompareIntvCtrol()
{
    delete m_pBaseConstrains;
}

//---------------------------------------------------------------------------------------
void EarCompareIntvCtrol::initialize_ctrol()
{
    //options
    m_pConstrains->SetButtonsEnabledAfterSolution(false);
    m_pConstrains->set_height(4000.0);      //minimum problem box height = 40mm
    m_nPlayMM = 320;    //score build with whole notes, so metronome rate 320
                        //will force to play at 80 notes/minute

    create_controls();
}

//---------------------------------------------------------------------------------------
void EarCompareIntvCtrol::get_ctrol_options_from_params()
{
    m_pConstrains = LENMUS_NEW EarIntervalsConstrains("EarCompare", m_appScope);
    m_pBaseConstrains = m_pConstrains;
    m_pConstrains->set_theory_mode(false);    //force aural training mode

    EarCompareIntvCtrolParms builder(m_pBaseConstrains);
    builder.process_params( m_pDyn->get_params() );
}

//---------------------------------------------------------------------------------------
wxDialog* EarCompareIntvCtrol::get_settings_dialog()
{
    wxWindow* pParent = dynamic_cast<wxWindow*>(m_pCanvas);
    return LENMUS_NEW DlgCfgEarIntervals(pParent, m_pConstrains, true);    // true -> enable First note equal checkbox
}

//---------------------------------------------------------------------------------------
void EarCompareIntvCtrol::set_problem_space()
{
    //Do nothing. For now, this exercise does not use Leitner method
}

//---------------------------------------------------------------------------------------
void EarCompareIntvCtrol::on_settings_changed()
{
    //Nothing to do, as it is not necessary to reconfigure answer buttons
}

//---------------------------------------------------------------------------------------
wxString EarCompareIntvCtrol::set_new_problem()
{
    EClef nClef = k_clef_G2;

    // select interval type: ascending, descending or both
    RandomGenerator oGenerator;
    bool fAscending;
    if (m_pConstrains->IsTypeAllowed(0) ||
        (m_pConstrains->IsTypeAllowed(1) && m_pConstrains->IsTypeAllowed(2)))
    {
        // if harmonic scale or melodic ascending and descending,
        // both, ascending and descending, are allowed. Choose one randomly
        fAscending = oGenerator.flip_coin();
    }
    else if (m_pConstrains->IsTypeAllowed(1))
    {
        // if melodic ascendig, allow only ascending intervals
        fAscending = true;
    }
    else
    {
        // allow only descending intervals
        fAscending = false;
    }

    // select a random key signature satisfying the constraints
    EKeySignature nKey;
    if (m_pConstrains->OnlyNatural())
        nKey = oGenerator.generate_key(m_pConstrains->GetKeyConstrains());
    else
        nKey = k_key_C;

    // generate first interval
    Interval oIntv0(m_pConstrains->OnlyNatural(), m_pConstrains->MinNote(),
        m_pConstrains->MaxNote(), m_pConstrains->AllowedIntervals(), fAscending, nKey);

    //if user likes first note equal in both intervals, force first note
    bool fOnlyNatural = m_pConstrains->OnlyNatural();
    FPitch fp0 = k_undefined_fpitch;
    if (m_pConstrains->FirstNoteEqual())
    {
        fp0 = oIntv0.get_pitch(0);
    }
    else
    {
        fp0 = k_undefined_fpitch;
    }
    Interval oIntv1(fOnlyNatural, m_pConstrains->MinNote(), m_pConstrains->MaxNote(),
            m_pConstrains->AllowedIntervals(), fAscending, nKey, fp0);

    //Convert problem to LDP pattern
    string sPattern[2][2];
    for (int i=0; i < 2; i++)
    {
        FPitch fp0 = oIntv0.get_pitch(i);
        sPattern[0][i] = "(n " + fp0.to_rel_ldp_name(nKey) + " w)";
        FPitch fp1 = oIntv1.get_pitch(i);
        sPattern[1][i] = "(n " + fp1.to_rel_ldp_name(nKey) + " w)";
    }

    //create two scores, one for each interval to be played, and a third score with
    //both intervals for displaying the solution

    //create the two single-interval scores
    for (int i=0; i<2; i++)
    {
        AScore score = m_doc.create_object(k_obj_score).downcast_to_score();
        m_pScore[i] = score.internal_object();
        ImoInstrument* pInstr = m_pScore[i]->add_instrument();
            // (g_pMidi->get_default_voice_channel(), g_pMidi->get_default_voice_instr(), "");
        ImoSystemInfo* pInfo = m_pScore[i]->get_first_system_info();
        pInfo->set_top_system_distance( pInstr->tenths_to_logical(30) );     // 3 lines
        pInstr->add_clef( nClef );
        pInstr->add_key_signature(nKey);
        pInstr->add_time_signature(4 ,4, k_no_visible );
        pInstr->add_object( sPattern[i][0] );
        pInstr->add_barline(k_barline_simple, k_no_visible);    //so that accidental doesn't affect 2nd note
        pInstr->add_object( sPattern[i][1] );
        pInstr->add_barline(k_barline_end, k_no_visible);

        m_pScore[i]->end_of_changes();
    }

    //create the answer score with both intervals
    m_pSolutionScore = static_cast<ImoScore*>(ImFactory::inject(k_imo_score, m_pDoc));
    m_pSolutionScore->set_long_option("Render.SpacingMethod", k_spacing_fixed);
    ImoInstrument* pInstr = m_pSolutionScore->add_instrument();
    //    // (g_pMidi->get_default_voice_channel(), g_pMidi->get_default_voice_instr(), "");
    ImoSystemInfo* pInfo = m_pSolutionScore->get_first_system_info();
    pInfo->set_top_system_distance( pInstr->tenths_to_logical(80) );     // 8 lines
    pInstr->add_clef( nClef );
    pInstr->add_key_signature(nKey);
    pInstr->add_time_signature(4 ,4, k_no_visible );
        //first interval
    pInstr->add_object("(spacer 10 (text ''" + to_std_string(oIntv0.get_interval_name()) +
                       "'' dy:-40))");
    pInstr->add_object( sPattern[0][0] );
    pInstr->add_barline(k_barline_simple, k_no_visible);    //so that accidental doesn't affect 2nd note
    pInstr->add_object( sPattern[0][1] );
    pInstr->add_spacer(80);       // 8 lines
        pInstr->add_barline(k_barline_double);
        // two invisible rests to do a pause when playing the score
    pInstr->add_object("(r h noVisible)");
    pInstr->add_barline(k_barline_simple, k_no_visible);
    pInstr->add_object("(r h noVisible)");
    pInstr->add_barline(k_barline_simple, k_no_visible);
        //second interval
    pInstr->add_object("(spacer 0 (text ''" + to_std_string(oIntv1.get_interval_name()) +
                       "'' dy:-40))");
    pInstr->add_object( sPattern[1][0] );
    pInstr->add_barline(k_barline_simple, k_no_visible);    //so that accidental doesn't affect 2nd note
    pInstr->add_object( sPattern[1][1] );
    pInstr->add_spacer(80);
    pInstr->add_barline(k_barline_end);

    m_pSolutionScore->end_of_changes();

    //compute the right answer
    m_sAnswer = "";
    if (oIntv0.get_num_semitones() > oIntv1.get_num_semitones())
        m_nRespIndex = 0;   //First is greater
    else if (oIntv0.get_num_semitones() == oIntv1.get_num_semitones())
        m_nRespIndex = 2;   //both are equal
    else
        m_nRespIndex = 1;   //second is greater

    //return message to display to introduce the problem
    return "";
}


}  //namespace lenmus
