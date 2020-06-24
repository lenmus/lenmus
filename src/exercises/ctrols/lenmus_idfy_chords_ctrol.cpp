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
#include "lenmus_idfy_chords_ctrol.h"

#include "lenmus_idfy_chords_ctrol_params.h"
#include "lenmus_chords_constrains.h"
#include "lenmus_dlg_cfg_idfy_chords.h"
#include "lenmus_generators.h"
#include "lenmus_document_canvas.h"
#include "lenmus_interval.h"
#include "lenmus_injectors.h"
#include "lenmus_colors.h"

//lomse
#include <lomse_doorway.h>
#include <lomse_internal_model.h>
#include <lomse_im_note.h>
#include <lomse_im_factory.h>
using namespace lomse;


namespace lenmus
{

//=======================================================================================
// Implementation of IdfyChordCtrol
//=======================================================================================

static string m_sButtonLabel[ect_MaxInExercises];

//---------------------------------------------------------------------------------------
IdfyChordCtrol::IdfyChordCtrol(long dynId, ApplicationScope& appScope,
                               DocumentWindow* pCanvas)
    : OneScoreCtrol(dynId, appScope, pCanvas)
{
}

//---------------------------------------------------------------------------------------
IdfyChordCtrol::~IdfyChordCtrol()
{
    delete m_pBaseConstrains;
}

//---------------------------------------------------------------------------------------
void IdfyChordCtrol::initialize_ctrol()
{
    m_pConstrains = dynamic_cast<ChordConstrains*>(m_pBaseConstrains);
    m_pConstrains->set_height(5000.0);      //minimum problem box height = 50mm

    //allow to play chords
    m_nKey = k_key_C;
    m_fpRootNote = FPitch(k_step_C, k_octave_4, 0);
    m_nInversion = 0;
    m_nMode = m_pConstrains->GetRandomMode();

    create_controls();
}

//---------------------------------------------------------------------------------------
void IdfyChordCtrol::get_ctrol_options_from_params()
{
    m_pBaseConstrains = LENMUS_NEW ChordConstrains("IdfyChord", m_appScope);
    IdfyChordCtrolParams builder(m_pBaseConstrains);
    builder.process_params( m_pDyn->get_params() );
}

//---------------------------------------------------------------------------------------
void IdfyChordCtrol::set_problem_space()
{
    //Do nothing. For now, this exercise does not use Leitner method
}

//---------------------------------------------------------------------------------------
void IdfyChordCtrol::initialize_strings()
{
    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated

        //button labels.

    // Triads
    m_sButtonLabel[ect_MajorTriad] = to_std_string( _("Major ") );
    m_sButtonLabel[ect_MinorTriad] = to_std_string( _("Minor ") );
    m_sButtonLabel[ect_AugTriad] = to_std_string( _("Augmented ") );
    m_sButtonLabel[ect_DimTriad] = to_std_string( _("Diminished ") );
    m_sButtonLabel[ect_Suspended_4th] = to_std_string( _("Suspended (4th)") );
    m_sButtonLabel[ect_Suspended_2nd] = to_std_string( _("Suspended (2nd)") );

    // Seventh chords
    m_sButtonLabel[ect_MajorSeventh] = to_std_string( _("Major 7th") );
    m_sButtonLabel[ect_DominantSeventh] = to_std_string( _("Dominant 7th") );
    m_sButtonLabel[ect_MinorSeventh] = to_std_string( _("Minor 7th") );
    m_sButtonLabel[ect_DimSeventh] = to_std_string( _("Diminished 7th") );
    m_sButtonLabel[ect_HalfDimSeventh] = to_std_string( _("Half dim. 7th") );
    m_sButtonLabel[ect_AugMajorSeventh] = to_std_string( _("Aug. major 7th") );
    m_sButtonLabel[ect_AugSeventh] = to_std_string( _("Augmented 7th") );
    m_sButtonLabel[ect_MinorMajorSeventh] = to_std_string( _("Minor-major 7th") );

    // Sixth chords
    m_sButtonLabel[ect_MajorSixth] = to_std_string( _("Major 6th") );
    m_sButtonLabel[ect_MinorSixth] = to_std_string( _("Minor 6th") );
    m_sButtonLabel[ect_AugSixth] = to_std_string( _("Augmented 6th") );

}

//---------------------------------------------------------------------------------------
void IdfyChordCtrol::create_answer_buttons(LUnits height, LUnits spacing)
{
    //create buttons for the answers, 5 rows, 4 columns, with row labels
    //Now main keyboard with all other buttons

    ImoStyle* pDefStyle = m_pDoc->get_default_style();
    ImoInlineWrapper* pBox;

    ImoStyle* pBtStyle = m_pDoc->create_private_style();
    pBtStyle->font_name("sans")->font_size(8.0f);

    ImoStyle* pRowStyle = m_pDoc->create_private_style();
    pRowStyle->font_size(10.0f)->margin_bottom(100.0f);

    USize buttonSize(3300.0f, height);
    LUnits firstRowWidth = 4000.0f;
    LUnits otherRowsWidth = buttonSize.width + spacing;

    int iB;
    for (iB=0; iB < k_num_buttons; iB++)
        m_pAnswerButton[iB] = nullptr;

    //rows with label and buttons
    LibraryScope* pLibScope = m_appScope.get_lomse().get_library_scope();
    for (int iRow=0; iRow < k_num_rows; iRow++)
    {
        ImoParagraph* pKeyboardRow = m_pDyn->add_paragraph(pRowStyle);

        pBox = pKeyboardRow->add_inline_box(firstRowWidth, pDefStyle);
        m_pRowLabel[iRow] = pBox->add_text_item("", pRowStyle);

        // the buttons for this row
        for (int iCol=0; iCol < k_num_cols; iCol++)
        {
            iB = iCol + iRow * k_num_cols;
            pBox = pKeyboardRow->add_inline_box(otherRowsWidth, pDefStyle);
            m_pAnswerButton[iB] = pBox->add_button(*pLibScope, "Undefined",
                                                   buttonSize, pBtStyle);
            m_pAnswerButton[iB]->set_visible(false);
            m_pAnswerButton[iB]->enable(false);
        }
    }

    set_event_handlers();

    //inform base class about the settings
    set_buttons(m_pAnswerButton, k_num_buttons);
}

//---------------------------------------------------------------------------------------
void IdfyChordCtrol::on_settings_changed()
{
    //The settings have been changed.
    //Reconfigure buttons keyboard depending on the chords allowed

    int iC;     // real chord. Correspondence to EChordTypes
    int iB;     // button index: 0 .. k_num_buttons-1
    int iR;     // row index: 0 .. k_num_rows-1

    //hide all rows and buttons so that later we only have to enable the valid ones
    for (iB=0; iB < k_num_buttons; iB++)
    {
        m_pAnswerButton[iB]->set_visible(false);
        m_pAnswerButton[iB]->enable(false);
    }
    for (int iRow=0; iRow < k_num_rows; iRow++)
        m_pRowLabel[iRow]->set_text("");

    //triads
    iB = 0;
    if (m_pConstrains->IsValidGroup(ecg_Triads))
    {
        iR = 0;
        m_pRowLabel[iR]->set_text( to_std_string(_("Triads:")) );
        for (iC=0; iC <= ect_LastTriad; iC++)
        {
            if (m_pConstrains->IsChordValid((EChordType)iC))
            {
                m_nRealChord[iB] = iC;
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
    }
    if (iB % k_num_cols != 0)
        iB += (k_num_cols - (iB % k_num_cols));

    //sevenths
    if (m_pConstrains->IsValidGroup(ecg_Sevenths))
    {
        iR = iB / k_num_cols;
        m_pRowLabel[iR]->set_text( to_std_string(_("Seventh chords:")) );
        for (iC=ect_LastTriad+1; iC <= ect_LastSeventh; iC++)
        {
            if (m_pConstrains->IsChordValid((EChordType)iC))
            {
                m_nRealChord[iB] = iC;
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
    }
    if (iB % k_num_cols != 0)
        iB += (k_num_cols - (iB % k_num_cols));

    //Other
    if (m_pConstrains->IsValidGroup(ecg_Sixths))
    {
        iR = iB / k_num_cols;
        m_pRowLabel[iR]->set_text( to_std_string(_("Other chords:")) );
        for (iC=ect_LastSeventh+1; iC < ect_MaxInExercises; iC++)
        {
            if (m_pConstrains->IsChordValid((EChordType)iC))
            {
                m_nRealChord[iB] = iC;
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
    }
    m_pDoc->set_dirty();
}

//---------------------------------------------------------------------------------------
wxDialog* IdfyChordCtrol::get_settings_dialog()
{
    wxWindow* pParent = dynamic_cast<wxWindow*>(m_pCanvas);
    return LENMUS_NEW DlgCfgIdfyChord(pParent, m_pConstrains, m_pConstrains->is_theory_mode());
}

//---------------------------------------------------------------------------------------
ImoScore* IdfyChordCtrol::prepare_aux_score(int nButton)
{
    // the user press the button to play a specific sound (chord, interval, scale, etc.)
    // This method is then invoked to prepare the score with the requested sound.

    AScore score = m_doc.create_object(k_obj_score).downcast_to_score();
    ImoScore* pScore = score.internal_object();
    prepare_score(k_clef_G2, (EChordType)m_nRealChord[nButton], &pScore);
    return pScore;
}

//---------------------------------------------------------------------------------------
wxString IdfyChordCtrol::set_new_problem()
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
    m_nMode = m_pConstrains->GetRandomMode();

    // select a random key signature
    m_nKey = RandomGenerator::generate_key( m_pConstrains->GetKeyConstrains() );

    //Generate a random root note
    EClef nClef = k_clef_G2;
    m_fpRootNote = RandomGenerator::get_best_root_note(nClef, m_nKey);

    // generate a random chord
    EChordType nChordType = m_pConstrains->GetRandomChordType();
    m_nInversion = 0;
    if (m_pConstrains->AreInversionsAllowed())
        m_nInversion = RandomGenerator::random_number(0, Chord::num_notes(nChordType) - 1);

    if (!m_pConstrains->DisplayKey())
        m_nKey = k_key_C;
    m_sAnswer = prepare_score(nClef, nChordType, &m_pProblemScore);

    //compute the index for the button that corresponds to the right answer
    for (m_nRespIndex = 0; m_nRespIndex < k_num_buttons; ++m_nRespIndex)
    {
        if (m_nRealChord[m_nRespIndex] == nChordType)
            break;
    }

    //return message to introduce the problem
    if (m_pConstrains->is_theory_mode())
        return _("Identify the next chord:");
    else
        return "";
}

//---------------------------------------------------------------------------------------
wxString IdfyChordCtrol::prepare_score(EClef WXUNUSED(nClef), EChordType nType, ImoScore** pScore)
{
    //create the chord
    Chord oChord(m_fpRootNote, nType, m_nInversion, m_nKey);

    //wxLogMessage("[IdfyChordCtrol::prepare_score] sRootNote=%s, nType=%d, nInversion=%d, nKey=%d, name='%s'",
    //    m_fpRootNote.wx_str(), nType, m_nInversion, m_nKey, oChord.get_name_and_inversion().wx_str() );

    //delete the previous score
    if (*pScore)
    {
        delete *pScore;
        *pScore = nullptr;
    }

    //create a score with the chord
    string sPattern;

    int nNumNotes = oChord.get_num_notes();
    AScore score = m_doc.create_object(k_obj_score).downcast_to_score();
    *pScore = score.internal_object();
    (*pScore)->set_long_option("Render.SpacingMethod", long(k_spacing_fixed));
    ImoInstrument* pInstr = (*pScore)->add_instrument();
    // (g_pMidi->get_default_voice_channel(), g_pMidi->get_default_voice_instr(), "");
    ImoSystemInfo* pInfo = (*pScore)->get_first_system_info();
    pInfo->set_top_system_distance( pInstr->tenths_to_logical(30) );     // 3 lines
    pInstr->add_clef( k_clef_G2 );
    pInstr->add_key_signature( m_nKey );
    pInstr->add_time_signature(4 ,4, k_no_visible );

    if (m_nMode == 0)   // mode=0 -> harmonic
    {
        sPattern = "(chord ";
        for (int i=0; i < nNumNotes; i++)
        {
            sPattern += "(n " + oChord.GetPattern(i) + " w)";
        }
        sPattern += ")";
        pInstr->add_staff_objects( sPattern );
    }
    else
    {
        int i = (m_nMode == 2 ? nNumNotes-1 : 0);   // 2= melodic descending
        sPattern = "(n " + oChord.GetPattern(i) + " w)";
        pInstr->add_object( sPattern );
        for (i=1; i < nNumNotes; i++)
        {
            sPattern = "(n " + oChord.GetPattern((m_nMode == 2 ? nNumNotes-1-i : i))
                       + " w)";
            pInstr->add_staff_objects( sPattern );
        }
    }
    pInstr->add_spacer(30);       // 5 lines
    pInstr->add_barline(k_barline_end, k_no_visible);

    (*pScore)->end_of_changes();

    //(*pScore)->Dump();  //dbg

    //return the chord name
    if (m_pConstrains->AreInversionsAllowed())
        return oChord.get_name_and_inversion();
    else
        return oChord.get_name();           //only name
}


}  //namespace lenmus
