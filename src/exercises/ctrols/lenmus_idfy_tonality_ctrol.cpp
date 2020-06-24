//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2010-2015 LenMus project
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
#include "lenmus_idfy_tonality_ctrol.h"

#include "lenmus_url_aux_ctrol.h"
#include "lenmus_constrains.h"
#include "lenmus_tonality_constrains.h"
#include "lenmus_generators.h"
#include "lenmus_document_canvas.h"
#include "lenmus_dlg_cfg_idfy_tonality.h"
#include "lenmus_idfy_tonality_ctrol_params.h"
#include "lenmus_injectors.h"
#include "lenmus_colors.h"
#include "lenmus_utilities.h"

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
// Implementation of IdfyTonalityCtrol
//=======================================================================================

//wxBEGIN_EVENT_TABLE(IdfyTonalityCtrol, OneScoreCtrol)
//    EVT_COMMAND_RANGE (ID_BUTTON, ID_BUTTON+k_num_buttons-1, wxEVT_COMMAND_BUTTON_CLICKED, IdfyTonalityCtrol::OnRespButton)
//wxEND_EVENT_TABLE()


//---------------------------------------------------------------------------------------
IdfyTonalityCtrol::IdfyTonalityCtrol(long dynId, ApplicationScope& appScope,
                                     DocumentWindow* pCanvas)
    : OneScoreCtrol(dynId, appScope, pCanvas)
{
}

//---------------------------------------------------------------------------------------
void IdfyTonalityCtrol::initialize_ctrol()
{
    m_pConstrains = dynamic_cast<TonalityConstrains*>(m_pBaseConstrains);
    m_pConstrains->set_height(4000.0);      //minimum problem box height = 40mm

    create_controls();
}

//---------------------------------------------------------------------------------------
IdfyTonalityCtrol::~IdfyTonalityCtrol()
{
    delete m_pBaseConstrains;
}

//---------------------------------------------------------------------------------------
void IdfyTonalityCtrol::get_ctrol_options_from_params()
{
    m_pBaseConstrains = LENMUS_NEW TonalityConstrains("IdfyTonality", m_appScope);
    IdfyTonalityCtrolParams builder(m_pBaseConstrains);
    builder.process_params( m_pDyn->get_params() );
}

//---------------------------------------------------------------------------------------
void IdfyTonalityCtrol::set_problem_space()
{
    //Do nothing. For now, this exercise does not use Leitner method
}

//---------------------------------------------------------------------------------------
void IdfyTonalityCtrol::create_answer_buttons(LUnits height, LUnits WXUNUSED(spacing))
{
    //create buttons for the answers, 5 columns, 7 rows

    ImoStyle* pDefStyle = m_pDoc->get_default_style();
    ImoInlineWrapper* pBox;

    ImoStyle* pBtStyle = m_pDoc->create_private_style();
    pBtStyle->font_name("sans")->font_size(8.0f);

    ImoStyle* pRowStyle = m_pDoc->create_private_style();
    pRowStyle->font_size(10.0f)->margin_bottom(100.0f);

    USize buttonSize(3300.0f, height);
    LUnits rowWidth = 3500.0f;

    //rows with label and buttons
    LibraryScope* pLibScope = m_appScope.get_lomse().get_library_scope();
    for (int iRow=0; iRow < k_num_rows; iRow++)
    {
        ImoParagraph* pKeyboardRow = m_pDyn->add_paragraph(pRowStyle);

        // the buttons for this row
        for (int iCol=0; iCol < k_num_cols; iCol++)
        {
            int iB = iCol + iRow * k_num_cols;
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
void IdfyTonalityCtrol::initialize_strings()
{
}

//---------------------------------------------------------------------------------------
void IdfyTonalityCtrol::on_settings_changed()
{
    // The settings have been changed. Reconfigure answer keyboard for the new settings

    Colors* pColors = m_appScope.get_colors();

    //set buttons
    int iB = 0;
    if (m_pConstrains->UseMajorMinorButtons())
    {
        //Only major / minor buttons
        m_pAnswerButton[iB]->set_label( to_std_string(_("Major")) );
        m_pAnswerButton[iB]->set_visible(true);
        //m_pAnswerButton[iB]->enable(true);
        set_button_color(iB, pColors->Normal());
        iB++;
        m_pAnswerButton[iB]->set_label( to_std_string(_("Minor")) );
        m_pAnswerButton[iB]->set_visible(true);
        //m_pAnswerButton[iB]->enable(true);
        set_button_color(iB, pColors->Normal());
        iB++;
    }

    else
    {
        EKeySignature nKeys[] = {
            k_key_C, k_key_Cs, k_key_c, k_key_cs, k_key_Cf,
            k_key_D, k_key_Df, k_key_d, k_key_ds, k_key_undefined,
            k_key_E, k_key_Ef, k_key_e, k_key_ef, k_key_undefined,
            k_key_F, k_key_Fs, k_key_f, k_key_fs, k_key_undefined,
            k_key_G, k_key_Gf, k_key_g, k_key_gs, k_key_undefined,
            k_key_A, k_key_Af, k_key_a, k_key_as, k_key_af,
            k_key_B, k_key_Bf, k_key_b, k_key_bf, k_key_undefined,
        };

        //use a button for each enabled key signature
        iB=0;
        for (unsigned i = 0; i < sizeof(nKeys)/sizeof(EKeySignature); i++, iB++)
        {
            EKeySignature nKey = nKeys[i];
            if (nKey != k_key_undefined)
            {
                m_pAnswerButton[iB]->set_label(
                                            to_std_string(get_key_signature_name(nKey)) );
            }
            m_nRealKey[iB] = nKey;
            m_pAnswerButton[iB]->set_visible(nKey != k_key_undefined);
//            bool fEnable = m_pConstrains->IsValidKey(nKey);
//            m_pAnswerButton[iB]->enable(fEnable);
//            set_button_color(iB, fEnable ? pColors->Normal() : Color(255,255,255));
        }
    }

    //hide all other buttons
    while (iB < k_num_buttons)
    {
        m_pAnswerButton[iB]->set_visible(false);
        m_pAnswerButton[iB]->enable(false);
        iB++;
    }

    m_pDoc->set_dirty();

//    if (m_pConstrains->is_theory_mode())
//        new_problem();
//    else
//        m_pProblemScore = nullptr;
}

//---------------------------------------------------------------------------------------
void IdfyTonalityCtrol::EnableButtons(bool WXUNUSED(value))
{
}

//---------------------------------------------------------------------------------------
wxDialog* IdfyTonalityCtrol::get_settings_dialog()
{
    wxWindow* pParent = dynamic_cast<wxWindow*>(m_pCanvas);
    return LENMUS_NEW DlgCfgIdfyTonality(pParent, m_pConstrains);
}

//---------------------------------------------------------------------------------------
ImoScore* IdfyTonalityCtrol::prepare_aux_score(int WXUNUSED(nButton))
{
    //answer buttons no allowed to play tonalities
    return nullptr;
}

//---------------------------------------------------------------------------------------
wxString IdfyTonalityCtrol::set_new_problem()
{
    //This method must prepare the problem score and set variables:
    //  m_pProblemScore, m_pSolutionScore, m_sAnswer, m_nRespIndex and m_nPlayMM

    // select a key signature
    RandomGenerator oGenerator;
    m_nKey = oGenerator.generate_key( m_pConstrains->GetKeyConstrains() );

    //create the score
    EClef nClef = k_clef_G2;
    m_sAnswer = prepare_score(nClef, m_nKey, &m_pProblemScore);

	//compute the index for the button that corresponds to the right answer
    ComputeRightAnswerButtons();

    //return string to introduce the problem
	return _("Press 'Play' to hear the problem again.");
}

//---------------------------------------------------------------------------------------
void IdfyTonalityCtrol::ComputeRightAnswerButtons()
{
	//compute the index for the button that corresponds to the right answer
    m_nRespIndex = -1;
    m_nRespAltIndex = -1;
    if (m_pConstrains->UseMajorMinorButtons())
    {
        m_nRespIndex = (KeyUtilities::is_major_key(m_nKey) ? 0 : 1);
        return;
    }
    else
    {
        for (int iB=0; iB < k_num_buttons; iB++)
        {
            if (m_nRealKey[iB] == m_nKey)
            {
                m_nRespIndex = iB;
                break;
            }
        }
    }
    wxASSERT(m_nRespIndex >=0 && m_nRespIndex < k_num_buttons);

    //set alternative (enarmonic) right answer
    int key;
    switch(m_nKey)
    {
        case k_key_B:  key = k_key_Cf; break;
        case k_key_Fs: key = k_key_Gf; break;
        case k_key_Cs: key = k_key_Df; break;
        case k_key_gs: key = k_key_af; break;
        case k_key_ds: key = k_key_ef; break;
        case k_key_as: key = k_key_bf; break;
        case k_key_Cf: key = k_key_B; break;
        case k_key_Gf: key = k_key_Fs; break;
        case k_key_Df: key = k_key_Cs; break;
        case k_key_af: key = k_key_gs; break;
        case k_key_ef: key = k_key_ds; break;
        case k_key_bf: key = k_key_as; break;
        default:
            return;
    }

    for (int iB=0; iB < k_num_buttons; iB++)
    {
        if (m_nRealKey[iB] == key)
        {
            m_nRespAltIndex = iB;
            break;
        }
    }

}

//---------------------------------------------------------------------------------------
wxString IdfyTonalityCtrol::prepare_score(EClef WXUNUSED(nClef), EKeySignature nKey,
                                          ImoScore** pProblemScore)
{
    //delete the previous score
    if (*pProblemScore)
    {
        delete *pProblemScore;
        *pProblemScore = nullptr;
    }

    //determine tonic note
    //                        1 1 1 1 1 2 2 2 2 2 3 3 3 3 3 4 4 4 4 4 5 5 5 5 5 6 6 6 6 6
    //              0 2 4 6 8 0 2 4 6 8 0 2 4 6 8 0 2 4 6 8 0 2 4 6 8 0 2 4 6 8 0 2 4 6 8
    string notes = "c2d2e2f2g2a2b2c3d3e3f3g3a3b3c4d4e4f4g4a4b4c5d5e5f5g5a5b5c6d6e6f6g6a6b6";
    int nRoot = KeyUtilities::get_step_for_root_note(nKey)* 2 + 14;  //note in octave 3
    string note[16];  //4 notes per chord

    if (RandomGenerator::flip_coin())
    {
        //Marcelo Galvez. Tónica en los extremos
        //I
        note[0] = notes.substr(nRoot, 2);       //I
        note[1] = notes.substr(nRoot+8, 2);     //V
        note[2] = notes.substr(nRoot+18, 2);    //III
        note[3] = notes.substr(nRoot+28, 2);    //I
        //IV
        note[4] = notes.substr(nRoot-8, 2);     //IV
        note[5] = notes.substr(nRoot+10, 2);    //VI
        note[6] = notes.substr(nRoot+20, 2);    //IV
        note[7] = notes.substr(nRoot+28, 2);    //I
        //V
        note[8] = notes.substr(nRoot-6, 2);     //V
        note[9] = notes.substr(nRoot+8, 2);     //V
        note[10] = notes.substr(nRoot+16, 2);   //II
        note[11] = notes.substr(nRoot+26, 2);   //VII
    }
    else
    {
        //Marcelo Galvez. Tónica en los extremos. Opción 2
        //I
        note[0] = notes.substr(nRoot, 2);       //I
        note[1] = notes.substr(nRoot+4, 2);     //III
        note[2] = notes.substr(nRoot+22, 2);    //V
        note[3] = notes.substr(nRoot+28, 2);    //I
        //IV
        note[4] = notes.substr(nRoot-8, 2);     //IV
        note[5] = notes.substr(nRoot+6, 2);     //IV
        note[6] = notes.substr(nRoot+24, 2);    //VI
        note[7] = notes.substr(nRoot+28, 2);    //I
        //V
        note[8] = notes.substr(nRoot-6, 2);     //V
        note[9] = notes.substr(nRoot+2, 2);     //II
        note[10] = notes.substr(nRoot+22, 2);   //V
        note[11] = notes.substr(nRoot+26, 2);   //VII
    }
    //I
    note[12] = note[0];     //I
    note[13] = note[1];     //V
    note[14] = note[2];     //I
    note[15] = note[3];     //III

//    //Emilio Mesias
//    //I
//    note[0] = notes.substr(nRoot, 2);       //I
//    note[1] = notes.substr(nRoot+8, 2);     //V
//    note[2] = notes.substr(nRoot+14, 2);    //I
//    note[3] = notes.substr(nRoot+18, 2);    //III
//    //IV
//    note[4] = notes.substr(nRoot-8, 2);     //IV
//    note[5] = notes.substr(nRoot, 2);       //I
//    note[6] = notes.substr(nRoot+6, 2);     //IV
//    note[7] = notes.substr(nRoot+10, 2);    //VI
//    //V
//    note[8] = notes.substr(nRoot-6, 2);     //V
//    note[9] = notes.substr(nRoot+2, 2);     //II
//    note[10] = notes.substr(nRoot+8, 2);    //V
//    note[11] = notes.substr(nRoot+12, 2);   //VII
//    //I
//    note[12] = notes.substr(nRoot, 2);       //I
//    note[13] = notes.substr(nRoot+8, 2);     //V
//    note[14] = notes.substr(nRoot+14, 2);    //I
//    note[15] = notes.substr(nRoot+18, 2);    //III

    //create the score
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
    pInstr->add_key_signature( nKey );
    pInstr->add_time_signature(2 ,4);

    //add A4 note
    pInstr->add_object("(n =a4 w p1)");
    pInstr->add_barline(k_barline_simple);

    pInstr->add_object("(r w)");

    // Loop to add chords
    int i=0;
    for (int iC=0; iC < 4; iC++)
    {
        pInstr->add_spacer(15);
        pInstr->add_barline(k_barline_simple);

        sPattern = "(chord (n " + note[i++] + " w p2)";
        sPattern += "(n " + note[i++] + " w p2)";
        for (int iN=2; iN < 4; iN++)
            sPattern += "(n " + note[i++] + " w p1)";
        sPattern += ")";
        pInstr->add_staff_objects( sPattern );
    }
    pInstr->add_spacer(20);
    pInstr->add_barline(k_barline_end);

    (*pProblemScore)->end_of_changes();      //for generating StaffObjs collection

    //return key signature name
    return get_key_signature_name(nKey);
}


}  //namespace lenmus
