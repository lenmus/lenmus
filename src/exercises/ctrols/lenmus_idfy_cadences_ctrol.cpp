//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2018 LenMus project
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
#include "lenmus_idfy_cadences_ctrol.h"

#include "lenmus_idfy_cadences_ctrol_params.h"
#include "lenmus_cadences_constrains.h"
#include "lenmus_dlg_cfg_cadences.h"
#include "lenmus_generators.h"
#include "lenmus_document_canvas.h"
#include "lenmus_chord.h"
#include "lenmus_injectors.h"
#include "lenmus_colors.h"
#include "lenmus_utilities.h"

//lomse
#include <lomse_doorway.h>
#include <lomse_internal_model.h>
#include <lomse_im_note.h>
#include <lomse_im_factory.h>
#include <lomse_logger.h>
using namespace lomse;


namespace lenmus
{

//=======================================================================================
// Implementation of IdfyCadencesCtrol
//=======================================================================================

//---------------------------------------------------------------------------------------
IdfyCadencesCtrol::IdfyCadencesCtrol(long dynId, ApplicationScope& appScope,
                                       DocumentWindow* pCanvas)
    : OneScoreCtrol(dynId, appScope, pCanvas)
{
}

//---------------------------------------------------------------------------------------
IdfyCadencesCtrol::~IdfyCadencesCtrol()
{
    delete m_pBaseConstrains;
}

//---------------------------------------------------------------------------------------
void IdfyCadencesCtrol::initialize_ctrol()
{
    m_pConstrains = dynamic_cast<CadencesConstrains*>(m_pBaseConstrains);
    m_pConstrains->set_height(5500.0);      //minimum problem box height = 50mm

    //initializatios to allow to play cadences when clicking on answer buttons
    m_nKey = k_key_C;

    create_controls();
}

//---------------------------------------------------------------------------------------
void IdfyCadencesCtrol::get_ctrol_options_from_params()
{
    m_pBaseConstrains = LENMUS_NEW CadencesConstrains("IdfyCadence", m_appScope);
    IdfyCadencesCtrolParams builder(m_pBaseConstrains);
    builder.process_params( m_pDyn->get_params() );
}

//---------------------------------------------------------------------------------------
void IdfyCadencesCtrol::set_problem_space()
{
    //Do nothing. For now, this exercise does not use Leitner method
}

//---------------------------------------------------------------------------------------
void IdfyCadencesCtrol::create_answer_buttons(LUnits height, LUnits WXUNUSED(spacing))
{
    //create buttons for the answers, 2 rows, 4 cols, no labels
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

    int iB;
    for (iB=0; iB < k_num_buttons; iB++) {
        m_pAnswerButton[iB] = nullptr;
    }

    //rows with buttons
    LibraryScope* pLibScope = m_appScope.get_lomse().get_library_scope();
    for (int iRow=0; iRow < k_num_rows; iRow++)
    {
        ImoParagraph* pKeyboardRow = m_pDyn->add_paragraph(pRowStyle);

        // the buttons for this row
        for (int iCol=0; iCol < k_num_cols; iCol++)
        {
            iB = iCol + iRow * k_num_cols;
            pBox = pKeyboardRow->add_inline_box(rowWidth, pDefStyle);
            m_pAnswerButton[iB] = pBox->add_button(*pLibScope, "?", buttonSize, pBtStyle);
            m_pAnswerButton[iB]->set_visible(false);
            m_pAnswerButton[iB]->enable(false);
        }
    }

    set_event_handlers();

    //inform base class about the settings
    set_buttons(m_pAnswerButton, k_num_buttons);
}

//---------------------------------------------------------------------------------------
void IdfyCadencesCtrol::initialize_strings()
{
}

//---------------------------------------------------------------------------------------
void IdfyCadencesCtrol::on_settings_changed()
{
    // The settings have been changed. Reconfigure answer keyboard for the new settings

    int iB;     // button index: 0 .. k_num_buttons-1

    //hide all rows and buttons so that later we only have to enable the valid ones
    for (iB=0; iB < k_num_buttons; iB++)
    {
        m_pAnswerButton[iB]->set_visible(false);
        m_pAnswerButton[iB]->enable(false);
    }

    //Terminal cadences
    iB = 0;
    if (m_pConstrains->IsValidButton(lm_eCadButtonTerminal))
    {
        iB = DisplayButton(iB, k_cadence_terminal, k_cadence_last_terminal, _("Terminal"));
    }

    //Transient cadences
    if (m_pConstrains->IsValidButton(lm_eCadButtonTransient))
    {
         iB = DisplayButton(iB, k_cadence_transient, k_cadence_last_transient, _("Transient"));
    }

    //Perfect cadences
    if (m_pConstrains->IsValidButton(lm_eCadButtonPerfect))
    {
         iB = DisplayButton(iB, k_cadence_perfect, k_cadence_last_perfect, _("Perfect"));
    }
    //Plagal cadences

    if (m_pConstrains->IsValidButton(lm_eCadButtonPlagal))
    {
         iB = DisplayButton(iB, k_cadence_plagal, k_cadence_last_plagal, _("Plagal"));
    }

    //Imperfect cadences
    if (m_pConstrains->IsValidButton(lm_eCadButtonImperfect))
    {
         iB = DisplayButton(iB, k_cadence_imperfect, k_cadence_last_imperfect, _("Imperfect"));
    }

    //Deceptive cadences
    if (m_pConstrains->IsValidButton(lm_eCadButtonDeceptive))
    {
         iB = DisplayButton(iB, k_cadence_deceptive, k_cadence_last_deceptive, _("Deceptive"));
    }

    //Half cadences
    if (m_pConstrains->IsValidButton(lm_eCadButtonHalf))
    {
         iB = DisplayButton(iB, k_cadence_half, k_cadence_last_half, _("Half cadence"));
    }
    m_pDoc->set_dirty();

//    if (m_pConstrains->is_theory_mode())
//        new_problem();
//    else
//        m_pProblemScore = nullptr;
}

//---------------------------------------------------------------------------------------
int IdfyCadencesCtrol::DisplayButton(int iB, ECadenceType iStartC,
                                     ECadenceType iEndC, wxString sButtonLabel)
{
    // Display a button
    // iB: number of button to display
    // iStartC-iEndC: range of cadences associated to this button
    // sButtonLabel: label for this button

    m_nStartCadence[iB] = iStartC;
    m_nEndCadence[iB] = iEndC;
    m_pAnswerButton[iB]->set_label( to_std_string(sButtonLabel) );
    m_pAnswerButton[iB]->set_visible(true);
    m_pAnswerButton[iB]->enable(false);
    iB++;
    return iB;
}

//---------------------------------------------------------------------------------------
wxDialog* IdfyCadencesCtrol::get_settings_dialog()
{
    wxWindow* pParent = dynamic_cast<wxWindow*>(m_pCanvas);
    return LENMUS_NEW DlgCfgIdfyCadence(pParent, m_pConstrains, m_pConstrains->is_theory_mode());
}

//---------------------------------------------------------------------------------------
ImoScore* IdfyCadencesCtrol::prepare_aux_score(int WXUNUSED(nButton))
{
    //answer buttons not allowed for playing cadences
    return nullptr;
}

//---------------------------------------------------------------------------------------
wxString IdfyCadencesCtrol::set_new_problem()
{
    //This method must prepare the problem score and set variables:
    //  m_pProblemScore, m_pSolutionScore, m_sAnswer, m_nRespIndex and m_nPlayMM

    // generate a random cadence
    ECadenceType nCadenceType = m_pConstrains->GetRandomCadence();

    // select a key signature
    RandomGenerator oGenerator;
    m_nKey = oGenerator.generate_key( m_pConstrains->GetKeyConstrains() );

    //create the score
    EClef nClef = k_clef_G2;
    if (m_pConstrains->is_theory_mode())
        m_sAnswer = prepare_score(nClef, nCadenceType, &m_pProblemScore);
    else
        m_sAnswer = prepare_score(nClef, nCadenceType, &m_pProblemScore, &m_pSolutionScore);

	// If it was not possible to create the cadence for this key signature, try
	// again with another cadence
    int nTimes = 0;
	while (m_sAnswer == "")
    {
		nCadenceType = m_pConstrains->GetRandomCadence();
        if (m_pConstrains->is_theory_mode())
            m_sAnswer = prepare_score(nClef, nCadenceType, &m_pProblemScore);
        else
            m_sAnswer = prepare_score(nClef, nCadenceType, &m_pProblemScore, &m_pSolutionScore);
		if (++nTimes == 1000)
        {
			LOMSE_LOG_ERROR("Loop. Impossible to get a cadence.");
			break;
		}
	}

    //// For debugging and testing. Force to display and use the problem score for the
    //// solution score; the tonic chord is then visible
    //if (!m_pConstrains->is_theory_mode()) {
    //    delete m_pSolutionScore;
    //    m_pSolutionScore = nullptr;
    //}


	//compute the index for the button that corresponds to the right answer
    if (m_pConstrains->IsValidButton(lm_eCadButtonTerminal))
    {
        //Terminal / transient cadences
        if (nCadenceType >= k_cadence_terminal && nCadenceType < k_cadence_last_terminal)
            m_nRespIndex = 0;
        else
            m_nRespIndex = 1;
    }
    else
    {
        //Perfect / Plagal cadences
        m_nRespIndex = -1;      //not set
        int iB = -1;
        if (m_pConstrains->IsValidButton(lm_eCadButtonPerfect))
        {
            iB++;
            if (nCadenceType >= k_cadence_perfect && nCadenceType < k_cadence_last_perfect)
                m_nRespIndex = iB;
        }
        //Plagal cadences
        if (m_nRespIndex == -1 && m_pConstrains->IsValidButton(lm_eCadButtonPlagal))
        {
            iB++;
            if (nCadenceType >= k_cadence_plagal && nCadenceType < k_cadence_last_plagal)
                m_nRespIndex = iB;
        }

        //Imperfect cadences
        if (m_nRespIndex == -1 && m_pConstrains->IsValidButton(lm_eCadButtonImperfect))
        {
            iB++;
            if (nCadenceType >= k_cadence_imperfect && nCadenceType < k_cadence_last_imperfect)
                m_nRespIndex = iB;
        }

        //Deceptive cadences
        if (m_nRespIndex == -1 && m_pConstrains->IsValidButton(lm_eCadButtonDeceptive))
        {
            iB++;
            if (nCadenceType >= k_cadence_deceptive && nCadenceType < k_cadence_last_deceptive)
                m_nRespIndex = iB;
        }

        //Half cadences
        if (m_nRespIndex == -1 && m_pConstrains->IsValidButton(lm_eCadButtonHalf))
        {
            iB++;
            if (nCadenceType >= k_cadence_half && nCadenceType < k_cadence_last_half)
                m_nRespIndex = iB;
        }
    }

    //return string to introduce the problem
    if (m_pConstrains->is_theory_mode())
        return _("Identify the next cadence:");
    else
        return _("Press 'Play' to hear the problem again.");
}

//---------------------------------------------------------------------------------------
string IdfyCadencesCtrol::get_initial_msge()
{
    wxString sText = _("Click on 'New problem' to start");
    sText += ". ";
    if (m_pConstrains->GetKeyDisplayMode() == 0)
        sText += _("An A4 note will be played before the cadence begins.");
    else
        sText += _("A tonic chord will be played before the cadence begins.");
    return to_std_string( sText );
}

//---------------------------------------------------------------------------------------
wxString IdfyCadencesCtrol::prepare_score(EClef WXUNUSED(nClef), ECadenceType nType,
                                           ImoScore** pProblemScore,
                                           ImoScore** pSolutionScore)
{
    //create the chords
    Cadence oCad;
    if (!oCad.create(nType, m_nKey))
        return "";

    //delete the previous score
    if (*pProblemScore)
    {
        delete *pProblemScore;
        *pProblemScore = nullptr;
    }
    if (pSolutionScore)
    {
        delete *pSolutionScore;
        *pSolutionScore = nullptr;
    }

    //create a score with the chord
    string sPattern;

    AScore score = m_doc.create_object(k_obj_score).downcast_to_score();
    *pProblemScore = score.internal_object();
    (*pProblemScore)->set_long_option("Render.SpacingMethod", long(k_spacing_fixed));
    ImoInstrument* pInstr = (*pProblemScore)->add_instrument();
    // (g_pMidi->get_default_voice_channel(), g_pMidi->get_default_voice_instr(), "");
    ImoSystemInfo* pInfo = (*pProblemScore)->get_first_system_info();
    pInfo->set_top_system_distance( pInstr->tenths_to_logical(30) );     // 3 lines
    pInstr->add_staff();                       //add second staff: five lines, standard size
    pInstr->add_clef( k_clef_G2, 1 );
    pInstr->add_clef( k_clef_F4, 2 );
    pInstr->add_key_signature( m_nKey );
    pInstr->add_time_signature(4, 4);

    //If ear training add A4/Tonic chord
    if (!m_pConstrains->is_theory_mode())
    {
        //it is ear training exercise
        if (m_pConstrains->GetKeyDisplayMode() == 0)
        {
            // Use A4 note
            sPattern = "(n =a4 w)";
            pInstr->add_object(sPattern);
        }
        else
        {
            // Use tonic chord
            Chord* pChord = oCad.get_tonic_chord();
            int nNumNotes = pChord->get_num_notes();
            sPattern = "(chord (n " + pChord->GetPattern(0) + " w)";
            for (int i=1; i < nNumNotes; i++)
            {
                sPattern += "(n ";
                sPattern += pChord->GetPattern(i);
                sPattern +=  " w)";
            }
            sPattern += ")";
            pInstr->add_staff_objects(sPattern);
        }
        pInstr->add_barline(k_barline_simple);

        pInstr->add_object("(r w)");
        pInstr->add_barline(k_barline_simple);
    }

    // Loop to add chords
    for (int iC=0; iC < oCad.get_num_chords(); iC++)
    {
        pInstr->add_spacer(15);
        if (iC != 0)
            pInstr->add_barline(k_barline_simple);

        // first and second notes on F4 clef staff
        sPattern = "(chord (n " + oCad.get_rel_ldp_name(iC, 0) + " w p2)";
        sPattern += "(n " + oCad.get_rel_ldp_name(iC, 1) + " w p2)";
        // third and fourth notes on G clef staff
        sPattern += "(n " + oCad.get_rel_ldp_name(iC, 2) + " w p1)";
        sPattern += "(n " + oCad.get_rel_ldp_name(iC, 3) + " w p1) )";
        pInstr->add_staff_objects( sPattern );
//        LOMSE_LOG_DEBUG("problem='%s'", sPattern.c_str());
    }
    pInstr->add_spacer(20);
    pInstr->add_barline(k_barline_end);

    (*pProblemScore)->end_of_changes();

    //Prepare Solution Score
    if (pSolutionScore)
    {
        AScore score = m_doc.create_object(k_obj_score).downcast_to_score();
        *pSolutionScore = score.internal_object();
        (*pSolutionScore)->set_long_option("Render.SpacingMethod", long(k_spacing_fixed));
        ImoInstrument* pInstr = (*pSolutionScore)->add_instrument();
        // (g_pMidi->get_default_voice_channel(), g_pMidi->get_default_voice_instr(), "");
        ImoSystemInfo* pInfo = (*pSolutionScore)->get_first_system_info();
        pInfo->set_top_system_distance( pInstr->tenths_to_logical(30) );     // 3 lines
        pInstr->add_staff();                       //add second staff: five lines, standard size
        pInstr->add_clef( k_clef_G2, 1 );
        pInstr->add_clef( k_clef_F4, 2 );
        pInstr->add_key_signature( m_nKey );
        pInstr->add_time_signature(4 ,4);

        // Loop to add chords
        for (int iC=0; iC < oCad.get_num_chords(); iC++)
        {
            pInstr->add_spacer(15);
            if (iC != 0)
                pInstr->add_barline(k_barline_simple);

            sPattern = "(chord (n " + oCad.get_rel_ldp_name(iC, 0) + " w p2)";
            sPattern += "(n " + oCad.get_rel_ldp_name(iC, 1) + " w p2)";
            sPattern += "(n " + oCad.get_rel_ldp_name(iC, 2) + " w p1)";
            sPattern += "(n " + oCad.get_rel_ldp_name(iC, 3) + " w p1) )";
//            wxLogMessage("[IdfyCadencesCtrol::prepare_score] solution='%s'",
//                         to_wx_string(sPattern).wx_str());
            pInstr->add_staff_objects( sPattern );
        }
        pInstr->add_spacer(20);
        pInstr->add_barline(k_barline_end);

        (*pSolutionScore)->end_of_changes();
    }

    //return cadence name
    return  get_key_signature_name(m_nKey) + ". " + oCad.get_name();
}


}  //namespace lenmus
