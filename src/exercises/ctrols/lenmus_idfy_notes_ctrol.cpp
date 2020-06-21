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

#include "lenmus_idfy_notes_ctrol.h"

#include "lenmus_constrains.h"
#include "lenmus_generators.h"
#include "lenmus_document_canvas.h"
#include "lenmus_notes_constrains.h"
#include "lenmus_idfy_notes_ctrol_params.h"
#include "lenmus_dlg_cfg_idfy_notes.h"
#include "lenmus_url_aux_ctrol.h"
#include "lenmus_injectors.h"
#include "lenmus_colors.h"
#include "lenmus_problem_displayer.h"
#include "lenmus_scale.h"

//lomse
#include <lomse_doorway.h>
#include <lomse_internal_model.h>
#include <lomse_im_note.h>
#include <lomse_staffobjs_table.h>
#include <lomse_im_factory.h>
#include <lomse_score_utilities.h>
#include <lomse_button_ctrl.h>
using namespace lomse;

//wxWidgets
#include <wx/wxprec.h>



namespace lenmus
{

//=======================================================================================
// Implementation of IdfyNotesCtrol
//=======================================================================================
IdfyNotesCtrol::IdfyNotesCtrol(long dynId, ApplicationScope& appScope,
                               DocumentWindow* pCanvas)
    : OneScoreCtrol(dynId, appScope, pCanvas)
{
}

//---------------------------------------------------------------------------------------
void IdfyNotesCtrol::initialize_ctrol()
{
    m_pConstrains = dynamic_cast<NotesConstrains*>(m_pBaseConstrains);
    m_pConstrains->set_theory_mode(false);
    m_pConstrains->set_height(4000.0);      //minimum problem box height = 40mm
    create_controls();
    set_initial_state();
}

//---------------------------------------------------------------------------------------
IdfyNotesCtrol::~IdfyNotesCtrol()
{
    delete m_pBaseConstrains;
}

//---------------------------------------------------------------------------------------
void IdfyNotesCtrol::get_ctrol_options_from_params()
{
    m_pBaseConstrains = LENMUS_NEW NotesConstrains("IdfyNotes", m_appScope);
    IdfyNotesCtrolParams builder(m_pBaseConstrains);
    builder.process_params( m_pDyn->get_params() );
}

//---------------------------------------------------------------------------------------
void IdfyNotesCtrol::set_problem_space()
{
    //Do nothing. For now, this exercise does not use Leitner method
}

//---------------------------------------------------------------------------------------
void IdfyNotesCtrol::create_answer_buttons(LUnits height, LUnits WXUNUSED(spacing))
{
    ImoStyle* pDefStyle = m_pDoc->get_default_style();
    ImoInlineWrapper* pBox;

    ImoStyle* pSpacerStyle = m_pDoc->create_private_style();
    pSpacerStyle->margin(0.0f)->padding(0.0f);

    LibraryScope* pLibScope = m_appScope.get_lomse().get_library_scope();

    ImoStyle* pBtStyle = m_pDoc->create_private_style();
    pBtStyle->font_name("sans")->font_size(8.0f);

    ImoStyle* pRowStyle = m_pDoc->create_private_style();
    pRowStyle->font_size(10.0f)->margin_bottom(500.0f);

    //Change 'Play' label to 'Play again'
    m_pPlayButton->change_label(to_std_string( _("Play again") ));

    //a paragraph for extra links
    ImoParagraph* pPlayRefPara = m_pDyn->add_paragraph(pRowStyle);

    // "Play all notes to identify" link
    m_pPlayAllNotes =
        LENMUS_NEW HyperlinkCtrl(*pLibScope, nullptr, m_pDoc,
                                 to_std_string(_("Play all notes to identify")) );
    m_pPlayAllNotes->add_event_handler(k_on_click_event, this, on_play_all_notes_event);
    pPlayRefPara->add_control( m_pPlayAllNotes );


    // "Play A4 reference note" link
    pPlayRefPara->add_inline_box(1000.0f, pSpacerStyle);
    m_pPlayA4 =
        LENMUS_NEW HyperlinkCtrl(*pLibScope, nullptr, m_pDoc,
                                 to_std_string(_("Play A4 reference note")) );
    m_pPlayA4->add_event_handler(k_on_click_event, this, on_play_a4_event);
    pPlayRefPara->add_control( m_pPlayA4 );


    // "Continue" link
    pPlayRefPara->add_inline_box(1000.0f, pSpacerStyle);
    m_pContinue =
        LENMUS_NEW HyperlinkCtrl(*pLibScope, nullptr, m_pDoc,
                                 to_std_string(_("Continue")) );
    m_pContinue->add_event_handler(k_on_click_event, this, on_continue_event);
    pPlayRefPara->add_control( m_pContinue );

    //keyboard layout: two rows, like a piano keyboard

    USize buttonSize(2000.0f, height);
    LUnits colWidth = 2200.0f;
    LUnits spacerWidth = colWidth / 2.0f;

    //first row: sharp/flat notes
    ImoStyle* pButtonsRowStyle = m_pDoc->create_private_style();
    pButtonsRowStyle->font_size(10.0f)->margin_top(200.0f);

    ImoParagraph* pFirstRow = m_pDyn->add_paragraph(pButtonsRowStyle);
    pBox = pFirstRow->add_inline_box(spacerWidth, pDefStyle);
    //
    pBox = pFirstRow->add_inline_box(colWidth, pDefStyle);
    m_pAnswerButton[1] = pBox->add_button(*pLibScope, m_sButtonLabel[1], buttonSize, pBtStyle);
    pBox = pFirstRow->add_inline_box(colWidth, pDefStyle);
    m_pAnswerButton[3] = pBox->add_button(*pLibScope, m_sButtonLabel[3], buttonSize, pBtStyle);
    //
    pBox = pFirstRow->add_inline_box(colWidth, pDefStyle);
    //
    pBox = pFirstRow->add_inline_box(colWidth, pDefStyle);
    m_pAnswerButton[6] = pBox->add_button(*pLibScope, m_sButtonLabel[6], buttonSize, pBtStyle);
    pBox = pFirstRow->add_inline_box(colWidth, pDefStyle);
    m_pAnswerButton[8] = pBox->add_button(*pLibScope, m_sButtonLabel[8], buttonSize, pBtStyle);
    pBox = pFirstRow->add_inline_box(colWidth, pDefStyle);
    m_pAnswerButton[10] = pBox->add_button(*pLibScope, m_sButtonLabel[10], buttonSize, pBtStyle);

    //second row: natural notes
    ImoParagraph* pSecondRow = m_pDyn->add_paragraph(pButtonsRowStyle);
    pBox = pSecondRow->add_inline_box(colWidth, pDefStyle);
    m_pAnswerButton[0] = pBox->add_button(*pLibScope, m_sButtonLabel[0], buttonSize, pBtStyle);
    pBox = pSecondRow->add_inline_box(colWidth, pDefStyle);
    m_pAnswerButton[2] = pBox->add_button(*pLibScope, m_sButtonLabel[2], buttonSize, pBtStyle);
    pBox = pSecondRow->add_inline_box(colWidth, pDefStyle);
    m_pAnswerButton[4] = pBox->add_button(*pLibScope, m_sButtonLabel[4], buttonSize, pBtStyle);
    pBox = pSecondRow->add_inline_box(colWidth, pDefStyle);
    m_pAnswerButton[5] = pBox->add_button(*pLibScope, m_sButtonLabel[5], buttonSize, pBtStyle);
    pBox = pSecondRow->add_inline_box(colWidth, pDefStyle);
    m_pAnswerButton[7] = pBox->add_button(*pLibScope, m_sButtonLabel[7], buttonSize, pBtStyle);
    pBox = pSecondRow->add_inline_box(colWidth, pDefStyle);
    m_pAnswerButton[9] = pBox->add_button(*pLibScope, m_sButtonLabel[9], buttonSize, pBtStyle);
    pBox = pSecondRow->add_inline_box(colWidth, pDefStyle);
    m_pAnswerButton[11] = pBox->add_button(*pLibScope, m_sButtonLabel[11], buttonSize, pBtStyle);

    //inform base class about the settings
    set_buttons(m_pAnswerButton, k_num_buttons);
    enable_buttons(false);
}

//---------------------------------------------------------------------------------------
void IdfyNotesCtrol::initialize_strings()
{
    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated

        // button labels
    m_sButtonLabel[0] = to_std_string( _("C") );
    m_sButtonLabel[1] = to_std_string( _("C#/Db") );
    m_sButtonLabel[2] = to_std_string( _("D") );
    m_sButtonLabel[3] = to_std_string( _("D#/Eb") );
    m_sButtonLabel[4] = to_std_string( _("E") );
    m_sButtonLabel[5] = to_std_string( _("F") );
    m_sButtonLabel[6] = to_std_string( _("F#/Gb") );
    m_sButtonLabel[7] = to_std_string( _("G") );
    m_sButtonLabel[8] = to_std_string( _("G#/Ab") );
    m_sButtonLabel[9] = to_std_string( _("A") );
    m_sButtonLabel[10] = to_std_string( _("A#/Bb") );
    m_sButtonLabel[11] = to_std_string( _("B") );
}

//---------------------------------------------------------------------------------------
void IdfyNotesCtrol::on_settings_changed()
{
    // The settings have been changed. Set button labels as needed

    if (m_pConstrains->SelectNotesFromKeySignature())
    {
        int nAcc[7];
        KeyUtilities::get_accidentals_for_key(m_pConstrains->GetKeySignature(), nAcc);

        ButtonCtrl* pNaturalButton[7];
        pNaturalButton[0] = m_pAnswerButton[0];
        pNaturalButton[1] = m_pAnswerButton[2];
        pNaturalButton[2] = m_pAnswerButton[4];
        pNaturalButton[3] = m_pAnswerButton[5];
        pNaturalButton[4] = m_pAnswerButton[7];
        pNaturalButton[5] = m_pAnswerButton[9];
        pNaturalButton[6] = m_pAnswerButton[11];

        static const wxString sSteps = "CDEFGAB";
        for (int iB=0; iB < 7; iB++)
        {
            wxString sLabel = sSteps.Mid(iB, 1);
            if (nAcc[iB] == 1)
                sLabel += " #";
            else if (nAcc[iB] == -1)
                sLabel += " b";
            pNaturalButton[iB]->set_label( to_std_string( wxGetTranslation(sLabel) ));
        }
    }
    else
    {
        for (int iB=0; iB < k_num_buttons; iB++)
        {
            m_pAnswerButton[iB]->set_label( m_sButtonLabel[iB] );
        }
    }

    EnableButtons(false);
//    if (m_pConstrains->is_theory_mode())
//        new_problem();
//    else
//        m_pProblemScore = nullptr;
}

//---------------------------------------------------------------------------------------
void IdfyNotesCtrol::EnableButtons(bool fEnable)
{
    //enable/disable valid buttons

    if (m_pConstrains->SelectNotesFromKeySignature())
    {
        m_pAnswerButton[0]->enable(fEnable);
        m_pAnswerButton[2]->enable(fEnable);
        m_pAnswerButton[4]->enable(fEnable);
        m_pAnswerButton[5]->enable(fEnable);
        m_pAnswerButton[7]->enable(fEnable);
        m_pAnswerButton[9]->enable(fEnable);
        m_pAnswerButton[11]->enable(fEnable);

        m_pAnswerButton[1]->enable(false);
        m_pAnswerButton[3]->enable(false);
        m_pAnswerButton[6]->enable(false);
        m_pAnswerButton[8]->enable(false);
        m_pAnswerButton[10]->enable(false);

        m_pAnswerButton[1]->set_visible(false);
        m_pAnswerButton[3]->set_visible(false);
        m_pAnswerButton[6]->set_visible(false);
        m_pAnswerButton[8]->set_visible(false);
        m_pAnswerButton[10]->set_visible(false);
    }
    else
    {
        for (int iB=0; iB < k_num_buttons; iB++)
        {
            m_pAnswerButton[iB]->enable( fEnable && m_pConstrains->IsValidNote(iB) );
            m_pAnswerButton[iB]->set_visible(true);
        }
    }
    m_pDoc->set_dirty();

}

//---------------------------------------------------------------------------------------
wxDialog* IdfyNotesCtrol::get_settings_dialog()
{
    wxWindow* pParent = dynamic_cast<wxWindow*>(m_pCanvas);
    return LENMUS_NEW DlgCfgIdfyNotes(pParent, m_pConstrains);
}

//---------------------------------------------------------------------------------------
ImoScore* IdfyNotesCtrol::prepare_aux_score(int WXUNUSED(nButton))
{
    return nullptr;
}

//---------------------------------------------------------------------------------------
wxString IdfyNotesCtrol::set_new_problem()
{
    //This method must prepare the problem score and set variables:
    //  m_pProblemScore, m_pSolutionScore, m_sAnswer, m_nRespIndex and m_nPlayMM

    RandomGenerator oGenerator;
    EClef nClef = m_pConstrains->GetClef();

    //select octave
    int nSecondOctave = m_pConstrains->GetOctaves() == 2 ? oGenerator.flip_coin() : 0;
    int nOctave = 4;
    switch (nClef)
    {
        case k_clef_G2:   nOctave = 4 + nSecondOctave;    break;  //4,5
        case k_clef_F4:   nOctave = 3 - nSecondOctave;    break;  //3,2
        case k_clef_F3:   nOctave = 3 - nSecondOctave;    break;  //3,2
        case k_clef_C1:   nOctave = 4 - nSecondOctave;    break;  //4,3
        case k_clef_C2:   nOctave = 4 - nSecondOctave;    break;  //4,3
        case k_clef_C3:   nOctave = 4 - nSecondOctave;    break;  //4,3
        case k_clef_C4:   nOctave = 3 + nSecondOctave;    break;  //3,4
        default:
        {
            LOMSE_LOG_ERROR("Invalid clef %d", nClef);
            nClef = k_clef_G2;
            nOctave = 4 + nSecondOctave;
        }
    }

    // generate a random note and set m_nKey, nStep, nAcc and nNoteIndex
    int nNoteIndex = 0;
    int nStep = 0;
    int nAcc = 0;
    if (m_pConstrains->SelectNotesFromKeySignature())
    {
        m_nKey = m_pConstrains->GetKeySignature();

        //Generate a random root note
        FPitch fpRoot = oGenerator.get_best_root_note(nClef, m_nKey);
        Scale scale(fpRoot, est_MajorNatural, m_nKey);

        //generate a random note, different from previous one
        static int iPrev = 0;
        int iN = oGenerator.random_number(0, 6);
        while (iPrev == iN)
            iN = oGenerator.random_number(0, 6);
        iPrev = iN;

        FPitch fpNote = scale.get_note(iN);
        nStep = fpNote.step();
        nAcc = fpNote.num_accidentals();
        switch(nStep)
        {
            case 0: nNoteIndex = 0; break;
            case 1: nNoteIndex = 2; break;
            case 2: nNoteIndex = 4; break;
            case 3: nNoteIndex = 5; break;
            case 4: nNoteIndex = 7; break;
            case 5: nNoteIndex = 9; break;
            case 6: nNoteIndex = 11; break;
        }
    }
    else
    {
        m_nKey = k_key_C;
        nNoteIndex = m_pConstrains->GetRandomNoteIndex();
        static const int nNoteStep[12] = { 0,0,1,1,2,3,3,4,4,5,5,6 };
        static const int nNoteAcc[12] = { 0,1,0,1,0,0,1,0,1,0,1,0 };
        nStep = nNoteStep[nNoteIndex];
        nAcc = nNoteAcc[nNoteIndex];
    }
    FPitch fpNote(nStep, nOctave, nAcc);
    string sNote = fpNote.to_abs_ldp_name();

    //prepare answer
    static const wxString sSteps = "CDEFGAB";
    wxString sAnswer = sSteps.Mid(nStep, 1);
    if (nAcc == 1)
        sAnswer += " sharp";
    else if (nAcc == -1)
        sAnswer += " flat";
    m_sAnswer = _("The note is: ");
    m_sAnswer += wxGetTranslation(sAnswer);
    m_sAnswer += ". --- ";
    m_sAnswer += _("Click on 'Continue' to listen a new note");

    //create the score
    if (m_pConstrains->is_theory_mode())
        prepare_score(nClef, sNote, &m_pProblemScore);
    else
        prepare_score(nClef, sNote, &m_pProblemScore, &m_pSolutionScore);

	//compute the index for the button that corresponds to the right answer
    m_nRespIndex = nNoteIndex;

    return _("Identify the following note:");
}

//---------------------------------------------------------------------------------------
void IdfyNotesCtrol::prepare_score(EClef nClef, const string& sNotePitch,
                                   ImoScore** pProblemScore,
                                   ImoScore** pSolutionScore)
{
    //delete the previous score
    delete *pProblemScore;
    *pProblemScore = nullptr;
    delete *pSolutionScore;
    *pSolutionScore = nullptr;

    //create the score with the note
    AScore score = m_doc.create_object(k_obj_score).downcast_to_score();
    *pProblemScore = score.internal_object();
    (*pProblemScore)->set_long_option("StaffLines.Truncate", k_truncate_always);
    ImoInstrument* pInstr = (*pProblemScore)->add_instrument();
    // (g_pMidi->get_default_voice_channel(), g_pMidi->get_default_voice_instr(), "");
    //ImoSystemInfo* pInfo = pScore->get_first_system_info();
    //pInfo->set_top_system_distance( pInstr->tenths_to_logical(30) );     // 3 lines
    pInstr->add_clef( nClef );
    pInstr->add_key_signature(m_nKey);
    pInstr->add_object("(n " + sNotePitch + " w)");
    (*pProblemScore)->end_of_changes();      //for generating StaffObjs collection
}

//---------------------------------------------------------------------------------------
void IdfyNotesCtrol::on_play_all_notes_event(void* pThis, SpEventInfo WXUNUSED(pEvent))
{
    (static_cast<IdfyNotesCtrol*>(pThis))->play_all_notes();
}

//---------------------------------------------------------------------------------------
void IdfyNotesCtrol::on_play_a4_event(void* pThis, SpEventInfo WXUNUSED(pEvent))
{
    (static_cast<IdfyNotesCtrol*>(pThis))->play_a4();
}

//---------------------------------------------------------------------------------------
void IdfyNotesCtrol::on_continue_event(void* pThis, SpEventInfo WXUNUSED(pEvent))
{
    (static_cast<IdfyNotesCtrol*>(pThis))->on_continue();
}

//---------------------------------------------------------------------------------------
void IdfyNotesCtrol::play_a4()
{
    stop_sounds();

    delete m_pAuxScore;
    AScore score = m_doc.create_object(k_obj_score).downcast_to_score();
    m_pAuxScore = score.internal_object();
    ImoInstrument* pInstr = m_pAuxScore->add_instrument();
        // (g_pMidi->get_default_voice_channel(), g_pMidi->get_default_voice_instr(), "");
    pInstr->add_clef( k_clef_G2 );
    pInstr->add_key_signature( k_key_C );
    pInstr->add_time_signature(2 ,4);
    pInstr->add_object("(n a4 w)");
    m_pAuxScore->end_of_changes();      //for generating StaffObjs collection

    m_pPlayer->load_score(m_pAuxScore, this);

    set_play_mode(k_play_normal_instrument);
    countoff_status(k_no_countoff);
    metronome_status(k_no_metronome);
    m_pPlayer->play(k_no_visual_tracking, m_nPlayMM, nullptr);
}

//---------------------------------------------------------------------------------------
void IdfyNotesCtrol::play_all_notes()
{
    stop_sounds();
    m_pPlayer->load_score(m_pScoreToPlay, this);
    Interactor* pInteractor = m_pDisplay->is_displayed(m_pScoreToPlay) ?
                              m_pCanvas->get_interactor() : nullptr;
    set_play_mode(k_play_normal_instrument);
    countoff_status(k_no_countoff);
    metronome_status(k_no_metronome);
    m_pPlayer->play(k_do_visual_tracking, m_nPlayMM, pInteractor);
}

//---------------------------------------------------------------------------------------
int IdfyNotesCtrol::get_first_octave_for_clef(EClef nClef)
{
    switch (nClef)
    {
        case k_clef_G2:   return 4;
        case k_clef_F4:   return 3;
        case k_clef_F3:   return 3;
        case k_clef_C1:   return 4;
        case k_clef_C2:   return 4;
        case k_clef_C3:   return 4;
        case k_clef_C4:   return 3;
        default:
            return 4;
    }
}

//---------------------------------------------------------------------------------------
void IdfyNotesCtrol::prepare_score_with_all_notes()
{
    //This method prepares a score with all the notes to identify and
    //stores it in m_pProblemScore

    EClef nClef = m_pConstrains->GetClef();

    //select octave
    int nFirstOctave = get_first_octave_for_clef(nClef);
    int nSecondOctave = nFirstOctave;
    bool fTwoOctaves = (m_pConstrains->GetOctaves() == 2);
    if (fTwoOctaves)
    {
        switch (nClef)
        {
            case k_clef_G2:   nSecondOctave += 1;    break;  //4,5
            case k_clef_F4:   nSecondOctave -= 1;    break;  //3,2
            case k_clef_F3:   nSecondOctave -= 1;    break;  //3,2
            case k_clef_C1:   nSecondOctave -= 1;    break;  //4,3
            case k_clef_C2:   nSecondOctave -= 1;    break;  //4,3
            case k_clef_C3:   nSecondOctave -= 1;    break;  //4,3
            case k_clef_C4:   nSecondOctave += 1;    break;  //3,4
            default:
                break;
        }
    }

    //create the score
    AScore score = m_doc.create_object(k_obj_score).downcast_to_score();
    ImoScore* pScore = score.internal_object();
    pScore->set_long_option("Render.SpacingMethod", long(k_spacing_fixed));
    pScore->set_long_option("StaffLines.Truncate", k_truncate_always);
    ImoInstrument* pInstr = pScore->add_instrument();
    // (g_pMidi->get_default_voice_channel(), g_pMidi->get_default_voice_instr(), "");
    //ImoSystemInfo* pInfo = pScore->get_first_system_info();
    //pInfo->set_top_system_distance( pInstr->tenths_to_logical(30) );     // 3 lines
    pInstr->add_clef( nClef );
    pInstr->add_key_signature( k_key_C );

    //generate all valid notes
    for (int i=0; i < 12; i++)
    {
        if (m_pConstrains->IsValidNote(i))
        {
            FPitch fpNote;
            if (m_pConstrains->SelectNotesFromKeySignature())
            {
                int nStep;
                int nAcc[7];
                KeyUtilities::get_accidentals_for_key(m_pConstrains->GetKeySignature(), nAcc);

                switch(i)
                {
                    case 0: //C or B#
                        nStep = (nAcc[6] == 1 ? 6 : 0);
                        break;

                    case 1: //C# or Db
                        nStep = (nAcc[1] == -1 ? 1 : 0);
                        break;

                    case 2: //D
                        nStep = 1;
                        break;

                    case 3: //D# or Eb
                        nStep = (nAcc[2] == -1 ? 2 : 1);
                        break;

                    case 4: //E or Fb
                        nStep = (nAcc[3] == -1 ? 3 : 2);
                        break;

                    case 5: //E# or F
                        nStep = (nAcc[2] == 1 ? 2 : 3);
                        break;

                    case 6: //F# or Gb
                        nStep = (nAcc[3] == 1 ? 3 : 4);
                        break;

                    case 7: //G
                        nStep = 4;
                        break;

                    case 8: //G# or Ab
                        nStep = (nAcc[4] == 1 ? 4 : 5);
                        break;

                    case 9: //A
                        nStep = 5;
                        break;

                    case 10: //A# or Bb
                        nStep = (nAcc[5] == 1 ? 5 : 6);
                        break;

                    case 11: //B or Cb
                        nStep = (nAcc[0] == -1 ? 0 : 6);
                        break;
                }
                fpNote = FPitch(nStep, nFirstOctave, nAcc[nStep]);
            }
            else
            {
                int nStep;
                int nAcc = 0;
                switch(i)
                {
                    case 0: //C or B#
                        nStep = 0;
                        break;

                    case 1: //C# or Db
                        nStep = 0;
                        nAcc = 1;
                        break;

                    case 2: //D
                        nStep = 1;
                        break;

                    case 3: //D# or Eb
                        nStep = 1;
                        nAcc = 1;
                        break;

                    case 4: //E or Fb
                        nStep = 2;
                        break;

                    case 5: //E# or F
                        nStep = 2;
                        break;

                    case 6: //F# or Gb
                        nStep = 3;
                        nAcc = 1;
                        break;

                    case 7: //G
                        nStep = 4;
                        break;

                    case 8: //G# or Ab
                        nStep = 4;
                        nAcc = 1;
                        break;

                    case 9: //A
                        nStep = 5;
                        break;

                    case 10: //A# or Bb
                        nStep = 5;
                        nAcc = 1;
                        break;

                    case 11: //B or Cb
                        nStep = 6;
                        break;
                }
                fpNote = FPitch(nStep, nFirstOctave, nAcc);
            }

            //add note
            pInstr->add_object( "(n " + fpNote.to_abs_ldp_name() + " w)" );
            pInstr->add_barline(k_barline_simple, k_no_visible);
        }
    }
    pScore->end_of_changes();      //for generating StaffObjs collection

    delete m_pProblemScore;
    m_pProblemScore = pScore;
}

//---------------------------------------------------------------------------------------
void IdfyNotesCtrol::set_initial_state()
{
    reset_exercise();

    //display the intro
    m_fQuestionAsked = false;
    m_pDisplay->clear();
    string sMsg = to_std_string(_("Click on 'New problem' to start"));
    m_pDisplay->set_problem_text(sMsg);
    m_pPlayButton->enable(false);
    m_pShowSolution->enable(false);
    m_pPlayA4->enable(false);
    m_pPlayAllNotes->enable(false);
    m_pContinue->enable(false);
}

//---------------------------------------------------------------------------------------
void IdfyNotesCtrol::on_new_problem()
{
    display_all_notes();
    m_pPlayButton->enable(false);
    m_pShowSolution->enable(false);
    m_pDoc->notify_if_document_modified();
}

//---------------------------------------------------------------------------------------
void IdfyNotesCtrol::display_all_notes()
{
    prepare_score_with_all_notes();
    wxString sProblemMessage = _("You will have to identify the following notes:");
    m_pDisplay->set_problem_text( to_std_string(sProblemMessage) );
    m_pDisplay->set_problem_score(m_pProblemScore);
    m_pScoreToPlay = m_pProblemScore;
    m_pProblemScore = nullptr; //ownership transferred to m_pDisplay

    m_pPlayA4->enable(true);
    m_pNewProblem->enable(false);
    m_pPlayAllNotes->enable(true);
    m_pContinue->enable(true);
}

//---------------------------------------------------------------------------------------
void IdfyNotesCtrol::on_continue()
{
    reset_exercise();

    //prepare answer buttons and counters
    if (m_pCounters && m_fCountersValid)
        m_pCounters->OnNewQuestion();

    //set m_pProblemScore, m_pSolutionScore, m_sAnswer, m_nRespIndex, m_nPlayMM
    wxString sProblemMessage = set_new_problem();

    //display the problem
    m_fQuestionAsked = true;
    display_problem_score();
    m_pDisplay->set_problem_text( to_std_string(sProblemMessage) );
    EnableButtons(true);

    //enable/disable links
    m_pPlayButton->enable(true);
    m_pShowSolution->enable(true);
    m_pPlayA4->enable(false);
    m_pNewProblem->enable(true);
    m_pPlayAllNotes->enable(false);
    m_pContinue->enable(false);

    //save time
    m_tmAsked = wxDateTime::Now();
}

//---------------------------------------------------------------------------------------
void IdfyNotesCtrol::on_resp_button(int iButton)
{
    //First, stop any possible score being played to avoid crashes
    stop_sounds();

    if (m_fQuestionAsked)
    {
        // There is a question asked. The user press the button to give the answer

        //verify if success or failure
        bool fSuccess = check_success(iButton);

        //inform problem manager of the result
        OnQuestionAnswered(m_iQ, fSuccess);

        //produce feedback sound, and update statistics display
        if (m_pCounters)
        {
            m_pCounters->UpdateDisplay();
            m_pCounters->RightWrongSound(fSuccess);
        }

        //if success generate a new problem. Else, ask user to try again
        if (fSuccess)
            new_problem();
        else
        {
            m_fQuestionAsked = true;
            display_problem_score();
            m_pDisplay->set_problem_text( to_std_string(_("Try again!")) );
            EnableButtons(true);
        }
    }
    //else
        // No problem presented. Ignore click on answer button

    m_pDoc->notify_if_document_modified();
}

//---------------------------------------------------------------------------------------
void IdfyNotesCtrol::display_solution()
{
    // mark right button in green
    Colors* pColors = m_appScope.get_colors();
    set_button_color(m_nRespIndex, pColors->Success());

    m_pContinue->enable(true);

    OneScoreCtrol::display_solution();
}


}  //namespace lenmus
