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

//lenmus
#include "lenmus_theo_key_sign_ctrol.h"
//
#include "lenmus_theo_key_sign_ctrol_params.h"
#include "lenmus_key_sign_constrains.h"
#include "lenmus_generators.h"

//lomse
#include <lomse_doorway.h>
#include <lomse_document.h>
#include <lomse_internal_model.h>
#include <lomse_im_note.h>
#include <lomse_im_factory.h>
#include <lomse_score_utilities.h>
using namespace lomse;


namespace lenmus
{

//=======================================================================================
// Implementation of TheoKeySignCtrol
//=======================================================================================

static wxString m_sMajor[15];
static wxString m_sMinor[15];


//---------------------------------------------------------------------------------------
TheoKeySignCtrol::TheoKeySignCtrol(long dynId, ApplicationScope& appScope,
                                       DocumentWindow* pCanvas)
    : OneScoreCtrol(dynId, appScope, pCanvas)
{
}

//---------------------------------------------------------------------------------------
TheoKeySignCtrol::~TheoKeySignCtrol()
{
    delete m_pBaseConstrains;
}

//---------------------------------------------------------------------------------------
void TheoKeySignCtrol::initialize_ctrol()
{
    //initializations
    m_nRespIndex = 0;
    m_pConstrains = dynamic_cast<TheoKeySignConstrains*>(m_pBaseConstrains);

    //exercise configuration options
    m_pConstrains->SetPlayLink(false);        //no play link
    m_pConstrains->set_height(4000.0);      //minimum problem box height = 40mm

    create_controls();
}

//---------------------------------------------------------------------------------------
void TheoKeySignCtrol::get_ctrol_options_from_params()
{
    m_pBaseConstrains = LENMUS_NEW TheoKeySignConstrains("TheoKeys", m_appScope);
    TheoKeySignCtrolParams builder(m_pBaseConstrains);
    builder.process_params( m_pDyn->get_params() );
}

//---------------------------------------------------------------------------------------
void TheoKeySignCtrol::set_problem_space()
{
    //Do nothing. For now, this exercise does not use Leitner method
}

//---------------------------------------------------------------------------------------
void TheoKeySignCtrol::on_settings_changed()
{
    //it is not necessary to reconfigure answer buttons
//    if (m_pConstrains->is_theory_mode())
//        new_problem();
//    else
//        m_pProblemScore = nullptr;
}


//---------------------------------------------------------------------------------------
void TheoKeySignCtrol::initialize_strings()
{
    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated

    m_sMajor[0] = _("C major");
    m_sMajor[1] = _("G major");
    m_sMajor[2] = _("D major");
    m_sMajor[3] = _("A major");
    m_sMajor[4] = _("E major");
    m_sMajor[5] = _("B major");
    m_sMajor[6] = _("F # major");
    m_sMajor[7] = _("C # major");
    m_sMajor[8] = _("C b major");
    m_sMajor[9] = _("G b major");
    m_sMajor[10] = _("D b major");
    m_sMajor[11] = _("A b major");
    m_sMajor[12] = _("E b major");
    m_sMajor[13] = _("B b major");
    m_sMajor[14] = _("F major");

    m_sMinor[0] = _("A minor");
    m_sMinor[1] = _("E minor");
    m_sMinor[2] = _("B minor");
    m_sMinor[3] = _("F # minor");
    m_sMinor[4] = _("C # minor");
    m_sMinor[5] = _("G # minor");
    m_sMinor[6] = _("D # minor");
    m_sMinor[7] = _("A # minor");
    m_sMinor[8] = _("A b minor");
    m_sMinor[9] = _("E b minor");
    m_sMinor[10] = _("B b minor");
    m_sMinor[11] = _("F minor");
    m_sMinor[12] = _("C minor");
    m_sMinor[13] = _("G minor");
    m_sMinor[14] = _("D minor");

}

//---------------------------------------------------------------------------------------
void TheoKeySignCtrol::create_answer_buttons(LUnits height, LUnits WXUNUSED(spacing))
{
    //create 15 buttons for the answers: three rows, five buttons per row

    ImoStyle* pDefStyle = m_pDoc->get_default_style();
    ImoInlineWrapper* pBox;

    ImoStyle* pBtStyle = m_pDoc->create_private_style();
    pBtStyle->font_name( "sans");
    pBtStyle->font_size( 8.0f);

    ImoStyle* pRowStyle = m_pDoc->create_private_style();
    pRowStyle->font_size( 10.0f);
    pRowStyle->margin_bottom( 0.0f);

    USize buttonSize(4200.0f, height);
    LUnits rowWidth = 4400.0f;

    //rows with buttons
    LibraryScope* pLibScope = m_appScope.get_lomse().get_library_scope();
    int iB = 0;
    for (int iRow=0; iRow < k_num_rows; iRow++)
    {
        ImoParagraph* pKeyboardRow = m_pDyn->add_paragraph(pRowStyle);
        for (int iCol=0; iCol < k_num_cols; iCol++)
        {
            iB = iCol + iRow * k_num_cols;
            if (iB == k_num_buttons)
                break;
            pBox = pKeyboardRow->add_inline_box(rowWidth, pDefStyle);
            m_pAnswerButton[iB] = pBox->add_button(*pLibScope, "?", buttonSize, pBtStyle);
        }
    }

    set_event_handlers();

    //inform base class about the settings
    set_buttons(m_pAnswerButton, k_num_buttons);
}

//---------------------------------------------------------------------------------------
wxString TheoKeySignCtrol::set_new_problem()
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


    // choose mode
    RandomGenerator oGenerator;
    if (m_pConstrains->GetScaleMode() == k_scale_both)
        m_fMajorMode = oGenerator.flip_coin();
    else
        m_fMajorMode = (m_pConstrains->GetScaleMode() == k_scale_major);

    // choose key signature and prepare answer
    bool fFlats = oGenerator.flip_coin();
    int nAnswer = -1;
    int nAccidentals = oGenerator.random_number(0, m_pConstrains->GetMaxAccidentals());
    EKeySignature nKey = k_key_undefined;
    if (m_fMajorMode)
    {
        if (fFlats)
        {
            // Major mode, flats
            switch(nAccidentals)
            {
                case 0:
                    nKey = k_key_C;
                    nAnswer = 0;            // Do Mayor, La menor, no accidentals
                    m_nRespIndex = 1;
                    break;
                case 1:
                    nKey = k_key_F;
                    nAnswer = 14;           // Fa Mayor, Re menor"
                    m_nRespIndex = 7;
                    break;
               case 2:
                    nKey = k_key_Bf;
                    nAnswer = 13;   // Si b Mayor, Sol menor"
                    m_nRespIndex = 13;
                    break;
                case 3:
                    nKey = k_key_Ef;
                    nAnswer = 12;   // Mi b Mayor, Do menor"
                    m_nRespIndex = 5;
                    break;
                case 4:
                    nKey = k_key_Af;
                    nAnswer = 11;   // La b Mayor, Fa menor"
                    m_nRespIndex = 11;
                    break;
                case 5:
                    nKey = k_key_Df;
                    nAnswer = 10;   // Re b Mayor, Si b menor"
                    m_nRespIndex = 3;
                    break;
                case 6:
                    nKey = k_key_Gf;
                    nAnswer = 9;   // Sol b Mayor, Mi b menor"
                    m_nRespIndex = 9;
                    break;
                case 7:
                    nKey = k_key_Cf;
                    nAnswer = 8;   // Do b Mayor, La b menor"
                    m_nRespIndex = 0;
                    break;
            }
        }
        else
        {
            // Major mode, sharps
            switch(nAccidentals)
            {
                case 0:
                    nKey = k_key_C;
                    nAnswer = 0;   // Do Mayor, La menor"
                    m_nRespIndex = 1;
                    break;
                case 1:
                    nKey = k_key_G;
                    nAnswer = 1;   // Sol Mayor, Mi menor"
                    m_nRespIndex = 10;
                    break;
                case 2:
                    nKey = k_key_D;
                    nAnswer = 2;   // Re Mayor, Si menor"
                    m_nRespIndex = 4;
                    break;
                case 3:
                    nKey = k_key_A;
                    nAnswer = 3;   // La Mayor, Fa # menor"
                    m_nRespIndex = 12;
                    break;
                case 4:
                    nKey = k_key_E;
                    nAnswer = 4;   // Mi Mayor, Do # menor"
                    m_nRespIndex = 6;
                    break;
                case 5:
                    nKey = k_key_B;
                    nAnswer = 5;   // Si Mayor, Sol # menor"
                    m_nRespIndex = 14;
                    break;
                case 6:
                    nKey = k_key_Fs;
                    nAnswer = 6;   // Fa # Mayor, Re # menor"
                    m_nRespIndex = 8;
                    break;
                case 7:
                    nKey = k_key_Cs;
                    nAnswer = 7;   // Do # Mayor, La # menor"
                    m_nRespIndex = 2;
                    break;
            }
        }
    }
    else
    {
        if (fFlats)
        {
            // Minor mode, flats
            switch(nAccidentals)
            {
                case 0:
                    nKey = k_key_a;
                    nAnswer = 0;   // Do Mayor, La menor"
                    m_nRespIndex = 11;
                    break;
                case 1:
                    nKey = k_key_d;
                    nAnswer = 14;   // Fa Mayor, Re menor"
                    m_nRespIndex = 2;
                    break;
                case 2:
                    nKey = k_key_g;
                    nAnswer = 13;   // Si b Mayor, Sol menor"
                    m_nRespIndex = 8;
                    break;
                case 3:
                    nKey = k_key_c;
                    nAnswer = 12;   // Mi b Mayor, Do menor"
                    m_nRespIndex = 0;
                    break;
                case 4:
                    nKey = k_key_f;
                    nAnswer = 11;   // La b Mayor, Fa menor"
                    m_nRespIndex = 6;
                    break;
                case 5:
                    nKey = k_key_bf;
                    nAnswer = 10;   // Re b Mayor, Si b menor"
                    m_nRespIndex = 13;
                    break;
                case 6:
                    nKey = k_key_ef;
                    nAnswer = 9;   // Sol b Mayor, Mi b menor"
                    m_nRespIndex = 4;
                    break;
                case 7:
                    nKey = k_key_af;
                    nAnswer = 8;   // Do b Mayor, La b menor"
                    m_nRespIndex = 10;
                    break;
            }
        }
        else
        {
            // Minor mode, sharps
            switch(nAccidentals)
            {
                case 0:
                    nKey = k_key_a;
                    nAnswer = 0;   // Do Mayor, La menor"
                    m_nRespIndex = 11;
                    break;
                case 1:
                    nKey = k_key_e;
                    nAnswer = 1;   // Sol Mayor, Mi menor"
                    m_nRespIndex = 5;
                    break;
                case 2:
                    nKey = k_key_b;
                    nAnswer = 2;   // Re Mayor, Si menor"
                    m_nRespIndex = 14;
                    break;
                case 3:
                    nKey = k_key_fs;
                    nAnswer = 3;   // La Mayor, Fa # menor"
                    m_nRespIndex = 7;
                    break;
                case 4:
                    nKey = k_key_cs;
                    nAnswer = 4;   // Mi Mayor, Do # menor"
                    m_nRespIndex = 1;
                    break;
                case 5:
                    nKey = k_key_gs;
                    nAnswer = 5;   // Si Mayor, Sol # menor"
                    m_nRespIndex = 9;
                    break;
                case 6:
                    nKey = k_key_ds;
                    nAnswer = 6;   // Fa # Mayor, Re # menor"
                    m_nRespIndex = 3;
                    break;
                case 7:
                    nKey = k_key_as;
                    nAnswer = 7;   // Do # Mayor, La # menor"
                    m_nRespIndex = 12;
                    break;
            }
        }
    }

    //coverity scan sanity check
    if (nAnswer < 0 || nAnswer > 14 || nKey == k_key_undefined)
    {
        stringstream msg;
        if (nAnswer < 0 || nAnswer > 14)
            msg << "Logic error. nAnswer must be 0..14 but it is " << nAnswer;
        else
            msg << "Logic error. nKey not initialized";

        LOMSE_LOG_ERROR(msg.str());

        nAnswer = 0;
        nKey = k_key_C;
    }

    // choose type of problem
    if (m_pConstrains->GetProblemType() == eBothKeySignProblems)
        m_fIdentifyKey = oGenerator.flip_coin();
    else
        m_fIdentifyKey = (m_pConstrains->GetProblemType() == eIdentifyKeySignature);

//    wxLogMessage("[TheoKeySignCtrol::NewProblem] m_fIdentifyKey=%s, m_fMajorMode=%s, "
//                 "fFlats=%s, nAccidentals=%d, nKey=%d, nAnswer=%d, m_nRespIndex=%d",
//                 (m_fIdentifyKey ? "yes" : "no"),
//                 (m_fMajorMode ? "yes" : "no"),
//                 (fFlats ? "yes" : "no"),
//                 nAccidentals, nKey, nAnswer, m_nRespIndex);


    // store index to right answer button (for guess-number-of-accidentals problems)
    if (!m_fIdentifyKey)
    {
        m_nRespIndex = KeyUtilities::key_signature_to_num_fifths(nKey);
        if (m_nRespIndex < 0) m_nRespIndex = 7 - m_nRespIndex;
    }

    // choose clef
    EClef nClef = oGenerator.generate_clef(m_pConstrains->GetClefConstrains());

    // write buttons' labels, depending on mode
    if (m_fIdentifyKey)
    {
        if (m_fMajorMode)
        {
            m_pAnswerButton[0]->set_label( to_std_string(_("C flat major")) );
            m_pAnswerButton[1]->set_label( to_std_string(_("C major")) );
            m_pAnswerButton[2]->set_label( to_std_string(_("C sharp major")) );
            m_pAnswerButton[3]->set_label( to_std_string(_("D flat major")) );
            m_pAnswerButton[4]->set_label( to_std_string(_("D major")) );
            m_pAnswerButton[5]->set_label( to_std_string(_("E flat major")) );
            m_pAnswerButton[6]->set_label( to_std_string(_("E major")) );
            m_pAnswerButton[7]->set_label( to_std_string(_("F major")) );
            m_pAnswerButton[8]->set_label( to_std_string(_("F sharp major")) );
            m_pAnswerButton[9]->set_label( to_std_string(_("G flat major")) );
            m_pAnswerButton[10]->set_label( to_std_string(_("G major")) );
            m_pAnswerButton[11]->set_label( to_std_string(_("A flat major")) );
            m_pAnswerButton[12]->set_label( to_std_string(_("A major")) );
            m_pAnswerButton[13]->set_label( to_std_string(_("B flat major")) );
            m_pAnswerButton[14]->set_label( to_std_string(_("B major")) );
            //14,0,7,12,2,10,4,8,6,13,1,11,3,9,5
        }
        else
        {
            m_pAnswerButton[0]->set_label( to_std_string(_("C minor")) );
            m_pAnswerButton[1]->set_label( to_std_string(_("C sharp minor")) );
            m_pAnswerButton[2]->set_label( to_std_string(_("D minor")) );
            m_pAnswerButton[3]->set_label( to_std_string(_("D sharp minor")) );
            m_pAnswerButton[4]->set_label( to_std_string(_("E flat minor")) );
            m_pAnswerButton[5]->set_label( to_std_string(_("E minor")) );
            m_pAnswerButton[6]->set_label( to_std_string(_("F minor")) );
            m_pAnswerButton[7]->set_label( to_std_string(_("F sharp minor")) );
            m_pAnswerButton[8]->set_label( to_std_string(_("G minor")) );
            m_pAnswerButton[9]->set_label( to_std_string(_("G sharp minor")) );
            m_pAnswerButton[10]->set_label( to_std_string(_("A flat minor")) );
            m_pAnswerButton[11]->set_label( to_std_string(_("A minor")) );
            m_pAnswerButton[12]->set_label( to_std_string(_("A sharp minor")) );
            m_pAnswerButton[13]->set_label( to_std_string(_("B flat minor")) );
            m_pAnswerButton[14]->set_label( to_std_string(_("B minor")) );
        }
    }
    else {
        // type of problem: write key
        m_pAnswerButton[0]->set_label( to_std_string(_("No accidentals")) );
        m_pAnswerButton[1]->set_label( to_std_string(_("1 #")) );
        m_pAnswerButton[2]->set_label( to_std_string(_("2 #")) );
        m_pAnswerButton[3]->set_label( to_std_string(_("3 #")) );
        m_pAnswerButton[4]->set_label( to_std_string(_("4 #")) );
        m_pAnswerButton[5]->set_label( to_std_string(_("5 #")) );
        m_pAnswerButton[6]->set_label( to_std_string(_("6 #")) );
        m_pAnswerButton[7]->set_label( to_std_string(_("7 #")) );
        m_pAnswerButton[8]->set_label( to_std_string(_("1 b")) );
        m_pAnswerButton[9]->set_label( to_std_string(_("2 b")) );
        m_pAnswerButton[10]->set_label( to_std_string(_("3 b")) );
        m_pAnswerButton[11]->set_label( to_std_string(_("4 b")) );
        m_pAnswerButton[12]->set_label( to_std_string(_("5 b")) );
        m_pAnswerButton[13]->set_label( to_std_string(_("6 b")) );
        m_pAnswerButton[14]->set_label( to_std_string(_("7 b")) );
    }

    //create the score
    AScore score = m_doc.create_object(k_obj_score).downcast_to_score();
    m_pProblemScore = score.internal_object();
    ImoInstrument* pInstr = m_pProblemScore->add_instrument();    // (0,0,"");                   //one vstaff, MIDI channel 0, MIDI instr 0
    ImoSystemInfo* pInfo = m_pProblemScore->get_first_system_info();
    pInfo->set_top_system_distance( pInstr->tenths_to_logical(30) );     // 3 lines
    pInstr->add_clef( nClef );
    pInstr->add_key_signature(nKey);
    pInstr->add_barline(k_barline_end, k_no_visible);

    m_pProblemScore->end_of_changes();

    //wxLogMessage(wxString::Format(
    //    "[TheoKeySignCtrol::NewProblem] m_nRespIndex=%d, oIntv.GetIntervalNum()=%d",
    //    m_nRespIndex, oIntv.GetIntervalNum() ));

    if (m_fIdentifyKey)
    {
        //direct problem
        m_sAnswer = m_sMajor[nAnswer] + ", " + m_sMinor[nAnswer];
        return _("Problem. Identify the following key signature:");
    }
    else
    {
        //inverse problem
        m_sAnswer = ( m_fMajorMode ? m_sMajor[nAnswer] : m_sMinor[nAnswer] );
        m_pSolutionScore = m_pProblemScore;
        m_pProblemScore = (ImoScore*)nullptr;
        wxString question = _("Problem. How many accidentals has next key signature?:");
        question += " " + m_sAnswer;
        return question;
    }

}

wxDialog* TheoKeySignCtrol::get_settings_dialog()
{
    return (wxDialog*)nullptr;
}


}  //namespace lenmus
