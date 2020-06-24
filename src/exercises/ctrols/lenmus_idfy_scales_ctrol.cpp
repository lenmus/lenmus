//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2020 LenMus project
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
#include "lenmus_idfy_scales_ctrol.h"

#include "lenmus_url_aux_ctrol.h"
#include "lenmus_constrains.h"
#include "lenmus_scales_constrains.h"
#include "lenmus_generators.h"
#include "lenmus_document_canvas.h"
#include "lenmus_dlg_cfg_idfy_scales.h"
#include "lenmus_idfy_scales_ctrol_params.h"
#include "lenmus_injectors.h"
#include "lenmus_colors.h"

//lomse
#include <lomse_doorway.h>
#include <lomse_internal_model.h>
#include <lomse_im_note.h>
#include <lomse_im_factory.h>
#include <lomse_score_utilities.h>
using namespace lomse;

//wxWidgets
#include <wx/wxprec.h>


namespace lenmus
{

//=======================================================================================
// Implementation of IdfyScalesCtrol
//=======================================================================================

static string m_sButtonLabel[est_Max];

//---------------------------------------------------------------------------------------
IdfyScalesCtrol::IdfyScalesCtrol(long dynId, ApplicationScope& appScope,
                                 DocumentWindow* pCanvas)
    : OneScoreCtrol(dynId, appScope, pCanvas)
{
}

//---------------------------------------------------------------------------------------
IdfyScalesCtrol::~IdfyScalesCtrol()
{
    delete m_pBaseConstrains;
}

//---------------------------------------------------------------------------------------
void IdfyScalesCtrol::initialize_ctrol()
{
    m_pConstrains = dynamic_cast<ScalesConstrains*>(m_pBaseConstrains);
    m_pConstrains->set_height(4000.0);      //minimum problem box height = 40mm

    //initializatios to allow to play scales
    m_nKey = k_key_C;
    m_fAscending = m_pConstrains->GetRandomPlayMode();

    create_controls();
}

//---------------------------------------------------------------------------------------
void IdfyScalesCtrol::get_ctrol_options_from_params()
{
    m_pBaseConstrains = LENMUS_NEW ScalesConstrains("IdfyScale", m_appScope);
    IdfyScalesCtrolParams builder(m_pBaseConstrains);
    builder.process_params( m_pDyn->get_params() );
}

//---------------------------------------------------------------------------------------
void IdfyScalesCtrol::set_problem_space()
{
    //Do nothing. For now, this exercise does not use Leitner method
}

//---------------------------------------------------------------------------------------
void IdfyScalesCtrol::create_answer_buttons(LUnits height, LUnits WXUNUSED(spacing))
{
    //create buttons for the answers, two rows

    ImoStyle* pDefStyle = m_pDoc->get_default_style();
    ImoInlineWrapper* pBox;

    ImoStyle* pBtStyle = m_pDoc->create_private_style();
    pBtStyle->font_name("sans")->font_size(8.0f);

    ImoStyle* pRowStyle = m_pDoc->create_private_style();
    pRowStyle->font_size(10.0f)->margin_bottom(100.0f);

    USize buttonSize(3400.0f, height);
    LUnits rowWidth = 3520.0f;

    //rows with buttons
    LibraryScope* pLibScope = m_appScope.get_lomse().get_library_scope();
    int iB=0;
    for (int iRow=0; iRow < k_num_rows; iRow++)
    {
        ImoParagraph* pKeyboardRow = m_pDyn->add_paragraph(pRowStyle);

        pBox = pKeyboardRow->add_inline_box(rowWidth, pDefStyle);
        m_pRowLabel[iRow] = pBox->add_text_item("?", pRowStyle);

        // the buttons for this row
        for (int iCol=0; iCol < k_num_cols; iCol++)
        {
            iB = iCol + iRow * k_num_cols;
            pBox = pKeyboardRow->add_inline_box(rowWidth, pDefStyle);
            m_pAnswerButton[iB] = pBox->add_button(*pLibScope, "?", buttonSize, pBtStyle);
        }
    }

    set_event_handlers();

    //inform base class about the settings
    set_buttons(m_pAnswerButton, k_num_buttons);
}

//---------------------------------------------------------------------------------------
void IdfyScalesCtrol::initialize_strings()
{

        //button labels.

    // Major scales
    m_sButtonLabel[est_MajorNatural] = to_std_string( _("Natural") );
    m_sButtonLabel[est_MajorTypeII] = to_std_string( _("Type II") );
    m_sButtonLabel[est_MajorTypeIII] = to_std_string( _("Type III") );
    m_sButtonLabel[est_MajorTypeIV] = to_std_string( _("Type IV") );

    // Minor scales
    m_sButtonLabel[est_MinorNatural] = to_std_string( _("Natural") );
    m_sButtonLabel[est_MinorDorian] = to_std_string( _("Dorian") );
    m_sButtonLabel[est_MinorHarmonic] = to_std_string( _("Harmonic") );
    m_sButtonLabel[est_MinorMelodic] = to_std_string( _("Melodic") );

    // Gregorian modes
    m_sButtonLabel[est_GreekIonian] = to_std_string( _("Ionian") );
    m_sButtonLabel[est_GreekDorian] = to_std_string( _("Dorian") );
    m_sButtonLabel[est_GreekPhrygian] = to_std_string( _("Phrygian") );
    m_sButtonLabel[est_GreekLydian] = to_std_string( _("Lydian") );
    m_sButtonLabel[est_GreekMixolydian] = to_std_string( _("Mixolydian") );
    m_sButtonLabel[est_GreekAeolian] = to_std_string( _("Aeolian") );
    m_sButtonLabel[est_GreekLocrian] = to_std_string( _("Locrian") );

    // Other scales
    m_sButtonLabel[est_PentatonicMinor] = to_std_string( _("Pentatonic minor") );
    m_sButtonLabel[est_PentatonicMajor] = to_std_string( _("Pentatonic major") );
    m_sButtonLabel[est_Blues] = to_std_string( _("Blues") );
    m_sButtonLabel[est_WholeTones] = to_std_string( _("Whole tones") );
    m_sButtonLabel[est_Chromatic] = to_std_string( _("Chromatic") );

}

//---------------------------------------------------------------------------------------
void IdfyScalesCtrol::on_settings_changed()
{
    //Reconfigure buttons keyboard depending on the scales allowed

    int iB;     // button index: 0 .. k_num_buttons-1

    //hide all rows and buttons so that later we only have to enable the valid ones
    for (iB=0; iB < k_num_buttons; iB++)
    {
        m_pAnswerButton[iB]->set_visible(false);
        m_pAnswerButton[iB]->enable(false);
    }
    for (int iRow=0; iRow < k_num_rows; iRow++)
    {
        m_pRowLabel[iRow]->set_text("");
    }

    //major scales
    iB = 0;
    if (m_pConstrains->IsValidGroup(k_scale_group_major))
        iB = ReconfigureGroup(iB, 0, est_LastMajor, _("Major:"));

    //minor scales
    if (m_pConstrains->IsValidGroup(k_scale_group_minor))
         iB = ReconfigureGroup(iB, est_LastMajor+1, est_LastMinor, _("Minor:"));

    //Gregorian modes
    if (m_pConstrains->IsValidGroup(k_scale_group_gregorian))
         iB = ReconfigureGroup(iB, est_LastMinor+1, est_LastGreek, _("Gregorian modes:"));

    //Other scales
    if (m_pConstrains->IsValidGroup(k_scale_group_other))
         iB = ReconfigureGroup(iB, est_LastGreek+1, est_Max-1, _("Other scales:"));

    m_pDoc->set_dirty();

//    if (m_pConstrains->is_theory_mode())
//        new_problem();
//    else
//        m_pProblemScore = nullptr;
}

//---------------------------------------------------------------------------------------
int IdfyScalesCtrol::ReconfigureGroup(int iBt, int iStartC, int iEndC,
                                      wxString sRowLabel)
{
    //Reconfigure a group of buttons


    int iC;     // real scale. Correspondence to EScaleTypes
    int iB;     // button index: 0 .. k_num_buttons-1
    int iR;     // row index: 0 .. k_num_rows-1

    iB = iBt;
    iR = iB / k_num_cols;
    m_pRowLabel[iR]->set_text( to_std_string(sRowLabel) );
    for (iC=iStartC; iC <= iEndC; iC++)
    {
        if (m_pConstrains->IsScaleValid((EScaleType)iC))
        {
            m_nRealScale[iB] = iC;
            m_pAnswerButton[iB]->set_label( m_sButtonLabel[iC] );
            m_pAnswerButton[iB]->set_visible(true);
            //m_pAnswerButton[iB]->enable(true);
            iB++;
            if (iB % k_num_cols == 0)
            {
                iR++;
                m_pRowLabel[iR]->set_text("");
            }
        }
    }

    if (iB % k_num_cols != 0)
        iB += (k_num_cols - (iB % k_num_cols));

    return iB;
}

//---------------------------------------------------------------------------------------
wxDialog* IdfyScalesCtrol::get_settings_dialog()
{
    wxWindow* pParent = dynamic_cast<wxWindow*>(m_pCanvas);
    return LENMUS_NEW DlgCfgIdfyScale(pParent, m_pConstrains, m_pConstrains->is_theory_mode());
}

//---------------------------------------------------------------------------------------
ImoScore* IdfyScalesCtrol::prepare_aux_score(int nButton)
{
    //ImoScore* pScore = static_cast<ImoScore*>(ImFactory::inject(k_imo_score, m_pDoc));
    AScore score = m_doc.create_object(k_obj_score).downcast_to_score();
    ImoScore* pScore = score.internal_object();
    prepare_score(k_clef_G2, (EScaleType)m_nRealScale[nButton], &pScore);
    return pScore;
}

//---------------------------------------------------------------------------------------
wxString IdfyScalesCtrol::set_new_problem()
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


    //select a random mode
    m_fAscending = m_pConstrains->GetRandomPlayMode();

    // generate a random scale
    EScaleType nScaleType = m_pConstrains->GetRandomScaleType();

    // select a key signature
    RandomGenerator oGenerator;
    m_nKey = oGenerator.generate_key( m_pConstrains->GetKeyConstrains() );

    // for minor scales use minor key signature and for major scales use a major key
    if (Scale::is_minor(nScaleType) && KeyUtilities::is_major_key(m_nKey))
        m_nKey = KeyUtilities::get_relative_minor_key(m_nKey);
    else if (!Scale::is_minor(nScaleType) && KeyUtilities::is_minor_key(m_nKey))
        m_nKey = KeyUtilities::get_relative_major_key(m_nKey);

    //Generate a random root note
    EClef nClef = k_clef_G2;
    m_fpRootNote = oGenerator.get_best_root_note(nClef, m_nKey);

    //hide key signature if requested or not tonal scale
    bool fDisplayKey = m_pConstrains->DisplayKey() && Scale::is_tonal(nScaleType);
    if (!fDisplayKey)
        m_nKey = k_key_C;

    //create the score
    m_sAnswer = prepare_score(nClef, nScaleType, &m_pProblemScore);

    //compute the index for the button that corresponds to the right answer
    int i;
    for (i = 0; i < k_num_buttons; i++) {
        if (m_nRealScale[i] == nScaleType) break;
    }
    m_nRespIndex = i;

    //if two solutions (minor/major or Gregorian mode) disable answer buttons
    //for the not valid answer
    DisableGregorianMajorMinor(nScaleType);

    //return string to introduce the problem
    if (m_pConstrains->is_theory_mode())
        return _("Identify the next scale:");
    else
        return "";
//        return _("Press 'Play' to hear it again");
}

//---------------------------------------------------------------------------------------
wxString IdfyScalesCtrol::prepare_score(EClef WXUNUSED(nClef), EScaleType nType, ImoScore** pScore)
{
    //create the scale
    Scale scale(m_fpRootNote, nType, m_nKey);

    ////dbg------------------------------------------------------
    //g_pLogger->LogTrace("IdfyScalesCtrol", "nClef = %d, nType = %d, m_sRootNote='%s', m_nKey=%d",
    //                nClef, nType, m_sRootNote.wx_str(), m_nKey );
    ////end dbg------------------------------------------------


    //delete the previous score
    if (*pScore)
    {
        delete *pScore;
        *pScore = nullptr;
    }

    //create a score with the scale
    string sPattern;

    int nNumNotes = scale.get_num_notes();
    AScore score = m_doc.create_object(k_obj_score).downcast_to_score();
    *pScore = score.internal_object();
    (*pScore)->set_long_option("Render.SpacingMethod", long(k_spacing_fixed));
    (*pScore)->set_long_option("StaffLines.Truncate", k_truncate_always);
    ImoInstrument* pInstr = (*pScore)->add_instrument();
    // (g_pMidi->get_default_voice_channel(), g_pMidi->get_default_voice_instr(), "");
    ImoSystemInfo* pInfo = (*pScore)->get_first_system_info();
    pInfo->set_top_system_distance( pInstr->tenths_to_logical(30) );     // 3 lines
    pInstr->add_clef( k_clef_G2 );
    pInstr->add_key_signature( m_nKey );
    pInstr->add_time_signature(4 ,4, k_no_visible );

    int i = (m_fAscending ? 0 : nNumNotes-1);
    sPattern = "(n " + scale.rel_ldp_name_for_note(i) + " w)";
    pInstr->add_object( sPattern );
    pInstr->add_barline(k_barline_simple, k_no_visible);   //so accidentals don't affect a 2nd note
    for (i=1; i < nNumNotes; i++)
    {
        sPattern = "(n ";
        sPattern += scale.rel_ldp_name_for_note((m_fAscending ? i : nNumNotes-1-i));
        sPattern +=  " w)";
        pInstr->add_object( sPattern );
        pInstr->add_barline(k_barline_simple, k_no_visible);   //so accidentals don't affect a 2nd note
    }
    pInstr->add_spacer(10);       // 1 lines

    (*pScore)->end_of_changes();

    //(*pScore)->Dump("IdfyScalesCtrol.prepare_score.ScoreDump.txt");  //dbg

    //set metronome. As the problem score is built using whole notes, we will
    //set metronome at MM=400 so the resulting note rate will be 100.
    m_nPlayMM = 400;

    //return the scale name
    return scale.get_name();
}

//---------------------------------------------------------------------------------------
void IdfyScalesCtrol::DisableGregorianMajorMinor(EScaleType nType)
{
    // Gregorian scale Ionian has the same notes than Major natural and
    // Gregorian scale Aeolian has the same notes than the Minor natural.
    // When Gregorian answer buttons and maor or minor are enabled there
    // will be cases in which the answer is nor clear as two anserws are
    // posible. To solve this, we are going to disable one of the answer
    // buttons

    if ((m_pConstrains->IsValidGroup(k_scale_group_major) || m_pConstrains->IsValidGroup(k_scale_group_minor))
        && m_pConstrains->IsValidGroup(k_scale_group_gregorian) )
    {
        EScaleType nDisable;
        if (nType == est_GreekIonian && m_pConstrains->IsScaleValid(est_MajorNatural))
        {
            //disable major natural
            nDisable = est_MajorNatural;
        }
        else if (nType == est_GreekAeolian && m_pConstrains->IsScaleValid(est_MinorNatural))
        {
            //disable minor natural
            nDisable = est_MinorNatural;
        }
        else if (nType == est_MajorNatural && m_pConstrains->IsScaleValid(est_GreekIonian))
        {
            //disable Gregorian ionian
            nDisable = est_GreekIonian;
        }
        else if (nType == est_MinorNatural && m_pConstrains->IsScaleValid(est_GreekAeolian))
        {
            //disable Gregorian aeolian
            nDisable = est_GreekAeolian;
        }
        else
            return;     //not necessary to disable any button

        //compute the index for the button to disable
        int iB;
        for (iB = 0; iB < k_num_buttons; iB++)
            if (m_nRealScale[iB] == nDisable) break;

        //coverity scan sanity check
        if (iB >= k_num_buttons)
        {
            stringstream msg;
            msg << "Logic error. iB should be lower than k_num_buttons, but not. iB="
                << iB << ", m_nRealScale={";
            for (int i=0; i < iB; i++)
                msg << m_nRealScale[i] << ",";

            msg << "}";
            LOMSE_LOG_ERROR(msg.str());

            return;
        }

        //disable button iB
        m_pAnswerButton[iB]->enable(false);
    }
}


}  //namespace lenmus
