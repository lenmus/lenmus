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
#include "lenmus_dictation_ctrol.h"

#include "lenmus_url_aux_ctrol.h"
#include "lenmus_constrains.h"
#include "lenmus_scores_constrains.h"
#include "lenmus_dictation_constrains.h"
#include "lenmus_generators.h"
#include "lenmus_document_canvas.h"
//#include "lenmus_dlg_cfg_idfy_scales.h"
#include "lenmus_dictation_ctrol_params.h"
#include "lenmus_injectors.h"
#include "lenmus_colors.h"
#include "lenmus_composer.h"
#include "lenmus_edit_interface.h"
#include "lenmus_scale.h"
#include "lenmus_score_corrector.h"
#include "lenmus_problem_displayer.h"

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
// Implementation of DictationCtrol
//=======================================================================================
DictationCtrol::DictationCtrol(long dynId, ApplicationScope& appScope,
                                 DocumentWindow* pCanvas)
    : FullEditorCtrol(dynId, appScope, pCanvas)
{
    for (int i=0; i < 4; ++i)
    {
        m_pPlayFragment[i] = nullptr;
    }
    clear_play_counters();
}

//---------------------------------------------------------------------------------------
DictationCtrol::~DictationCtrol()
{
    delete m_pBaseConstrains;
}

//---------------------------------------------------------------------------------------
void DictationCtrol::clear_play_counters()
{
    for (int i=0; i < 4; ++i)
    {
        m_numTimesFragmentPlayed[i] = 0;
    }
    m_numTimesProblemPlayed = 0;
}

//---------------------------------------------------------------------------------------
void DictationCtrol::initialize_ctrol()
{
    m_pConstrains = dynamic_cast<DictationConstrains*>(m_pBaseConstrains);
    m_pScoreConstrains = m_pConstrains->get_score_constrains();
    m_pConstrains->set_theory_mode(false);

    create_controls();
}

//---------------------------------------------------------------------------------------
void DictationCtrol::add_optional_content_block_1()
{

    if (num_fragments() > 1)
    {
        //create box for 'play fragment' links
        ImoStyle* pParaStyle = m_pDoc->create_private_style("Default style");
        pParaStyle->margin_top(0.0f)->margin_bottom(1000.0f);
        ImoParagraph* pContainer = m_pDyn->add_paragraph(pParaStyle);

        //fragments 1 & 2
        add_play_fragment_link(0, pContainer);
        pContainer->add_inline_box(1000.0f, m_pLinksSpacerStyle);
        add_play_fragment_link(1, pContainer);

        if (num_fragments() > 2)
        {
            pContainer->add_inline_box(1000.0f, m_pLinksSpacerStyle);
            add_play_fragment_link(2, pContainer);
            pContainer->add_inline_box(1000.0f, m_pLinksSpacerStyle);
            add_play_fragment_link(3, pContainer);
        }
    }
}

//---------------------------------------------------------------------------------------
void DictationCtrol::add_play_fragment_link(int i, ImoParagraph* pContainer)
{
    LibraryScope* pLibScope = m_appScope.get_lomse().get_library_scope();
    m_pPlayFragment[i] = LENMUS_NEW HyperlinkCtrl(*pLibScope, nullptr, m_pDoc, m_label_play[i]);
    switch (i)
    {
        case 0:
            m_pPlayFragment[i]->add_event_handler(k_on_click_event, this, on_play_fragment_1);
            break;
        case 1:
            m_pPlayFragment[i]->add_event_handler(k_on_click_event, this, on_play_fragment_2);
            break;
        case 2:
            m_pPlayFragment[i]->add_event_handler(k_on_click_event, this, on_play_fragment_3);
            break;
        case 3:
            m_pPlayFragment[i]->add_event_handler(k_on_click_event, this, on_play_fragment_4);
            break;
    }
    m_pPlayFragment[i]->enable(false);
    pContainer->add_control( m_pPlayFragment[i] );
}

//---------------------------------------------------------------------------------------
void DictationCtrol::get_ctrol_options_from_params()
{
    m_pBaseConstrains = LENMUS_NEW DictationConstrains("RhythmicDictation", m_appScope);
    DictationCtrolParams builder(m_pBaseConstrains);
    builder.process_params( m_pDyn->get_params() );
}

//---------------------------------------------------------------------------------------
void DictationCtrol::initialize_strings()
{
    m_label_play[0] = to_std_string(_("Play fragment 1"));
    m_label_play[1] = to_std_string(_("Play fragment 2"));
    m_label_play[2] = to_std_string(_("Play fragment 3"));
    m_label_play[3] = to_std_string(_("Play fragment 4"));

    m_label_stop[0] = to_std_string(_("Stop fragment 1"));
    m_label_stop[1] = to_std_string(_("Stop fragment 2"));
    m_label_stop[2] = to_std_string(_("Stop fragment 3"));
    m_label_stop[3] = to_std_string(_("Stop fragment 4"));
}

//---------------------------------------------------------------------------------------
void DictationCtrol::on_settings_changed()
{
    //TODO
}

//---------------------------------------------------------------------------------------
wxDialog* DictationCtrol::get_settings_dialog()
{
    //TODO
//    wxWindow* pParent = dynamic_cast<wxWindow*>(m_pCanvas);
//    return LENMUS_NEW DlgCfgRhythmicDictation(pParent, m_pConstrains, m_pConstrains->is_theory_mode());
    return nullptr;
}

//---------------------------------------------------------------------------------------
wxString DictationCtrol::generate_new_problem()
{
    //This method must prepare the problem score and set variables:
    //  m_pProblemScore - The score with the problem to propose
    //  m_pUserScore - The score to be edited by user
    //  m_nPlayMM - the speed to play the score (if not default speed)
    //
    //It must return the message to display to introduce the problem.

    delete_scores();
    clear_play_counters();

    prepare_problem_score();
    if (is_melodic_dictation())
        prepare_context_score();
    prepare_user_score();

    return "";
}

//---------------------------------------------------------------------------------------
void DictationCtrol::prepare_context_score()
{
    int step = KeyUtilities::get_step_for_root_note(m_keyType);   //0..6, 0=Do, 1=Re, 3=Mi, ... , 6=Si
    int accidentals[7];
    KeyUtilities::get_accidentals_for_key(m_keyType, accidentals);
    int octave = step > 4 ? 3 : 4;
    FPitch fpRootNote(step, octave, accidentals[step]);

    Scale scale(fpRootNote, est_MajorNatural, m_keyType);
    int numNotes = scale.get_num_notes();
    m_pContextScore = static_cast<ImoScore*>(ImFactory::inject(k_imo_score, m_pDoc));
    ImoInstrument* pInstr = m_pContextScore->add_instrument();
    ImoSoundInfo* pSound = pInstr->get_sound_info(0);
    ImoMidiInfo* pMidi = pSound->get_midi_info();
    pMidi->set_midi_program(0);     //Acoustic Grand Piano
    pInstr->add_clef(k_clef_G2);
    pInstr->add_key_signature(m_keyType);

    //ascending scale
    for (int i=0; i < numNotes; ++i)
        pInstr->add_object("(n " + scale.rel_ldp_name_for_note(i) + " s)");

    //descending scale
    for (int i=numNotes-1; i > 0; --i)
        pInstr->add_object("(n " + scale.rel_ldp_name_for_note(i) + " s)");
    pInstr->add_object("(n " + scale.rel_ldp_name_for_note(0) + " n)");

    //add chord progression I-IV-V-I
    add_chord(step, pInstr, "n");           // I
    add_chord((step+3)%7, pInstr, "n");     // IV
    add_chord((step+4)%7, pInstr, "n");     // V
    add_chord(step, pInstr, "h");           // I

    m_pContextScore->end_of_changes();
}

//---------------------------------------------------------------------------------------
void DictationCtrol::add_chord(int chordRoot, ImoInstrument* pInstr,
                               const string& noteType)
{
    //AWARE: This method is only valid for I, IV and V chords. In all others, it is
    //       necessary to add accidentals, and this is not taken into account here.

    stringstream pattern;
    const string steps = "cdefgab";

    int i = chordRoot;          //root, octave 2
    pattern << "(chord (n "<< steps[i] << "2 " << noteType << ")";

    i = (chordRoot+4) % 7;      //5th degree, octave 3 or 4
    int oct = chordRoot > 3 ? 4 : 3;
    pattern << "(n "<< steps[i] << oct << " " << noteType << ")";

    i = (chordRoot+2) % 7;      //3rd degree, octave 4
    pattern << "(n "<< steps[i] << "4 " << noteType << ")";

    i = chordRoot;              //root, octave 5
    pattern << "(n "<< steps[i] << "5 " << noteType << "))";

    pInstr->add_staff_objects( pattern.str() );
}

//---------------------------------------------------------------------------------------
void DictationCtrol::initial_play_of_problem()
{
    process_event(DictationCtrol::k_event_play_context_score);
}

//---------------------------------------------------------------------------------------
void DictationCtrol::do_correct_exercise()
{
    ScoreComparer cmp;
    if (cmp.are_equal(m_pUserScoreToPlay, m_pProblemScore))
    {
        display_no_error_message();
    }
    else
    {
        cmp.mark_scores();
        display_problem_score();
        //  - display tips, with links to specific exercises
        //  - update user score to show correction marks
    }

    m_pDoc->notify_if_document_modified();
}

//---------------------------------------------------------------------------------------
void DictationCtrol::display_problem_score()
{
    m_pDisplay->set_solution_text( to_std_string(
                    _("This is the solution (in colour, missing in your score):") ));
    m_pDisplay->set_solution_score(m_pProblemScore);
    m_pProblemScore = nullptr;     //ownership transferred to Document.
}

//---------------------------------------------------------------------------------------
void DictationCtrol::display_no_error_message()
{
    ImoStyle* pMsgStyle = m_pDoc->create_private_style("Default style");
    pMsgStyle->font_size(30.0)->font_weight(ImoStyle::k_font_weight_bold)->color(Color(20,140,40));
    m_pDisplay->set_solution_text( to_std_string(_("Perfect!")), pMsgStyle );
}

//---------------------------------------------------------------------------------------
void DictationCtrol::on_play_fragment_1(void* pThis, SpEventInfo WXUNUSED(pEvent))
{
    (static_cast<DictationCtrol*>(pThis))->process_event(
                                    DictationCtrol::k_click_play_or_stop_fragment_1);
}

//---------------------------------------------------------------------------------------
void DictationCtrol::on_play_fragment_2(void* pThis, SpEventInfo WXUNUSED(pEvent))
{
    (static_cast<DictationCtrol*>(pThis))->process_event(
                                    DictationCtrol::k_click_play_or_stop_fragment_2);
}

//---------------------------------------------------------------------------------------
void DictationCtrol::on_play_fragment_3(void* pThis, SpEventInfo WXUNUSED(pEvent))
{
    (static_cast<DictationCtrol*>(pThis))->process_event(
                                    DictationCtrol::k_click_play_or_stop_fragment_3);
}

//---------------------------------------------------------------------------------------
void DictationCtrol::on_play_fragment_4(void* pThis, SpEventInfo WXUNUSED(pEvent))
{
    (static_cast<DictationCtrol*>(pThis))->process_event(
                                    DictationCtrol::k_click_play_or_stop_fragment_4);
}

//---------------------------------------------------------------------------------------
void DictationCtrol::play_or_stop_fragment(int i)
{
    if (!m_pPlayer->is_playing())
    {
        // "Play" button pressed

        ++m_numTimesFragmentPlayed[i];

        //change link from "play" to "Stop playing" label
        m_pPlayFragment[i]->change_label(m_label_stop[i]);

        //play the score
        m_pPlayer->load_score(m_pProblemScoreToPlay, this);
        set_play_mode(k_play_normal_instrument);
        SpInteractor spInteractor = m_pCanvas ?
                                    m_pCanvas->get_interactor_shared_ptr() : SpInteractor();
        Interactor* pInteractor = (spInteractor ? spInteractor.get() : nullptr);
        this->countoff_status(true);
        bool fVisualTracking = (m_pProblemScore == nullptr);   //if nullptr it is displayed
        int startMeasure = 2 * i + 1;
        int numMeasures = 2;
        m_pPlayer->play_measures(startMeasure, numMeasures, fVisualTracking,
                                 m_nPlayMM, pInteractor);

        //AWARE The link label is restored to "play" when the EndOfPlay event is
        //received.
    }
    else
    {
        // "Stop" button pressed
        m_pPlayer->stop();
        on_end_of_playback();
    }
}

//---------------------------------------------------------------------------------------
void DictationCtrol::play_context_score()
{
    m_pPlayer->load_score(m_pContextScore, this);
    set_play_mode(k_play_normal_instrument);
    SpInteractor spInteractor = m_pCanvas ?
                                m_pCanvas->get_interactor_shared_ptr() : SpInteractor();
    Interactor* pInteractor = (spInteractor ? spInteractor.get() : nullptr);
    m_pPlayer->play(false, m_nPlayMM, pInteractor);
}

//---------------------------------------------------------------------------------------
void DictationCtrol::enable_links_for_current_state()
{
    switch (m_state)
    {
        case DictationCtrol::k_state_start:     //user has entered in exercise page
            m_pNewProblem->enable(true);
            m_pPlayProblem->enable(false);
            m_pPlayUserScore->enable(false);
            m_pDoneButton->enable(false);
            if (num_fragments() > 1)
            {
                for(int i=0; i < num_fragments(); ++i)
                    m_pPlayFragment[i]->enable(false);
            }
            break;

        case DictationCtrol::k_state_playing_context:    //program is playing tonal context
            m_pNewProblem->enable(false);
            m_pPlayProblem->enable(false);
            m_pPlayUserScore->enable(false);
            m_pDoneButton->enable(false);
            if (num_fragments() > 1)
            {
                for(int i=0; i < num_fragments(); ++i)
                    m_pPlayFragment[i]->enable(false);
            }
            break;

        case DictationCtrol::k_state_playing_fragment_1:
            m_pNewProblem->enable(false);
            m_pPlayProblem->enable(false);
            m_pPlayUserScore->enable(false);
            m_pDoneButton->enable(false);
            if (num_fragments() > 1)
            {
                for(int i=0; i < num_fragments(); ++i)
                    m_pPlayFragment[i]->enable(false);
                m_pPlayFragment[0]->enable(true);
            }
            break;

        case DictationCtrol::k_state_playing_fragment_2:
            m_pNewProblem->enable(false);
            m_pPlayProblem->enable(false);
            m_pPlayUserScore->enable(false);
            m_pDoneButton->enable(false);
            if (num_fragments() > 1)
            {
                for(int i=0; i < num_fragments(); ++i)
                    m_pPlayFragment[i]->enable(false);
                m_pPlayFragment[1]->enable(true);
            }
            break;

        case DictationCtrol::k_state_playing_fragment_3:
            m_pNewProblem->enable(false);
            m_pPlayProblem->enable(false);
            m_pPlayUserScore->enable(false);
            m_pDoneButton->enable(false);
            if (num_fragments() > 1)
            {
                for(int i=0; i < num_fragments(); ++i)
                    m_pPlayFragment[i]->enable(false);
                m_pPlayFragment[2]->enable(true);
            }
            break;

        case DictationCtrol::k_state_playing_fragment_4:
            m_pNewProblem->enable(false);
            m_pPlayProblem->enable(false);
            m_pPlayUserScore->enable(false);
            m_pDoneButton->enable(false);
            if (num_fragments() > 1)
            {
                for(int i=0; i < num_fragments(); ++i)
                    m_pPlayFragment[i]->enable(false);
                m_pPlayFragment[3]->enable(true);
            }
            break;

        case DictationCtrol::k_state_ready:              //user can start solving the problem
        {
            m_pNewProblem->enable(false);
            m_pPlayUserScore->enable(false);
            m_pDoneButton->enable(true);
            if (num_fragments() == 1)
            {
//                m_pDoneButton->enable(false);
                m_pPlayProblem->enable(m_numTimesProblemPlayed < 3);
            }
            else if (num_fragments() == 2)
            {
                bool fEnable = m_numTimesFragmentPlayed[0] < 3
                               && m_numTimesFragmentPlayed[1] == 0
                               && m_numTimesProblemPlayed == 0;
                m_pPlayFragment[0]->enable(fEnable);

                fEnable = m_numTimesFragmentPlayed[1] < 3
                          && m_numTimesProblemPlayed == 0;
                m_pPlayFragment[1]->enable(fEnable);

                fEnable = m_numTimesFragmentPlayed[1] > 0
                          && m_numTimesProblemPlayed == 0;
                m_pPlayProblem->enable(fEnable);

                fEnable = (m_numTimesFragmentPlayed[0] > 0
                           && m_numTimesFragmentPlayed[1] > 0)
                           || m_numTimesProblemPlayed > 0;
//                m_pDoneButton->enable(fEnable);
            }
            else
            {
                bool fEnable = m_numTimesFragmentPlayed[0] < 3
                               && m_numTimesFragmentPlayed[1] == 0
                               && m_numTimesProblemPlayed == 0;
                m_pPlayFragment[0]->enable(fEnable);

                fEnable = m_numTimesFragmentPlayed[0] > 0
                          && m_numTimesFragmentPlayed[1] < 3
                          && m_numTimesFragmentPlayed[2] == 0
                          && m_numTimesProblemPlayed == 0;
                m_pPlayFragment[1]->enable(fEnable);

                fEnable = m_numTimesFragmentPlayed[1] > 0
                          && m_numTimesFragmentPlayed[2] < 3
                          && m_numTimesFragmentPlayed[3] == 0
                          && m_numTimesProblemPlayed == 0;
                m_pPlayFragment[2]->enable(fEnable);

                fEnable = m_numTimesFragmentPlayed[2] > 0
                          && m_numTimesFragmentPlayed[3] < 3
                          && m_numTimesProblemPlayed == 0;
                m_pPlayFragment[3]->enable(fEnable);

                fEnable = m_numTimesFragmentPlayed[3] > 0
                          && m_numTimesProblemPlayed == 0;
                m_pPlayProblem->enable(fEnable);

                fEnable = (m_numTimesFragmentPlayed[0] > 0
                           && m_numTimesFragmentPlayed[1] > 0
                           && m_numTimesFragmentPlayed[2] > 0
                           && m_numTimesFragmentPlayed[3] > 0)
                          || m_numTimesProblemPlayed > 0;
//                m_pDoneButton->enable(fEnable);
            }
            break;
        }

        case DictationCtrol::k_state_playing_complete:   //program is playing complete problem score
            m_pNewProblem->enable(false);
            m_pPlayProblem->enable(true);
            m_pPlayUserScore->enable(false);
            m_pDoneButton->enable(false);
            if (num_fragments() > 1)
            {
                for(int i=0; i < num_fragments(); ++i)
                    m_pPlayFragment[i]->enable(false);
            }
            break;

        case DictationCtrol::k_state_solution:           //solution is displayed
            m_pNewProblem->enable(true);
            m_pPlayProblem->enable(true);
            m_pPlayUserScore->enable(true);
            m_pDoneButton->enable(false);
            if (num_fragments() > 1)
            {
                for(int i=0; i < num_fragments(); ++i)
                    m_pPlayFragment[i]->enable(true);
            }
            disable_edition();
            break;

        case DictationCtrol::k_state_playing_user:       //program is playing user solution
        case DictationCtrol::k_state_replaying_problem:  //program is playing again problem score
        case DictationCtrol::k_state_replaying_fragment_1:    //program is replaying fragment 1
        case DictationCtrol::k_state_replaying_fragment_2:    //program is replaying fragment 2
        case DictationCtrol::k_state_replaying_fragment_3:    //program is replaying fragment 3
        case DictationCtrol::k_state_replaying_fragment_4:    //program is replaying fragment 4
            break;

        default:
            return;
    }
}

//---------------------------------------------------------------------------------------
void DictationCtrol::process_event(int event)
{
    switch (m_state)
    {
        //--------------------------------------------------------------------------
        case DictationCtrol::k_state_start:              //user has entered in exercise page
            if (event == DictationCtrol::k_click_new_problem)
            {
                if (is_theory_mode())
                    change_state(DictationCtrol::k_state_ready);
                else if (is_melodic_dictation())
                    change_state(DictationCtrol::k_state_playing_context);
                else if (num_fragments() == 1)
                    change_state(DictationCtrol::k_state_playing_complete);
                else
                    change_state(DictationCtrol::k_state_playing_fragment_1);

                new_problem();
            }
            return;

        //--------------------------------------------------------------------------
        case DictationCtrol::k_state_playing_context:    //program is playing tonal context
        {
            if (event == FullEditorCtrol::k_end_of_playback)
            {
                if (num_fragments() == 1)
                {
                    change_state(DictationCtrol::k_state_playing_complete);
                    play_or_stop_problem(true /*do count off*/);
                }
                else
                {
                    change_state(DictationCtrol::k_state_playing_fragment_1);
                    play_or_stop_fragment(0);
                }
            }

            else if (event == DictationCtrol::k_event_play_context_score)
                play_context_score();

            return;
        }

        //--------------------------------------------------------------------------
        case DictationCtrol::k_state_playing_fragment_1:    //program is playing fragment 1
        case DictationCtrol::k_state_playing_fragment_2:    //program is playing fragment 2
        case DictationCtrol::k_state_playing_fragment_3:    //program is playing fragment 3
        case DictationCtrol::k_state_playing_fragment_4:    //program is playing fragment 4
        {
            int i = m_state - DictationCtrol::k_state_playing_fragment_1;       //i=0..3
            if (event == FullEditorCtrol::k_end_of_playback)
            {
                m_pPlayFragment[i]->change_label(m_label_play[i]);
                change_state(DictationCtrol::k_state_ready);
            }
            else    //k_click_play_or_stop_fragment_i
                play_or_stop_fragment(i);

            return;
        }

        //--------------------------------------------------------------------------
        case DictationCtrol::k_state_playing_complete:      //program is playing complete problem score
            if (event == DictationCtrol::k_click_play_or_stop_problem)
                play_or_stop_problem();

            else if (event == DictationCtrol::k_end_of_playback)
            {
                m_pPlayProblem->change_label(m_label_play_problem);
                change_state(DictationCtrol::k_state_ready);
            }
            return;

        //--------------------------------------------------------------------------
        case DictationCtrol::k_state_ready:              //user can start solving the problem
            if (event == DictationCtrol::k_click_play_or_stop_problem)
            {
                change_state(DictationCtrol::k_state_playing_complete);
                play_or_stop_problem();
            }
            else if (event == DictationCtrol::k_click_play_or_stop_fragment_1
                     || event == DictationCtrol::k_click_play_or_stop_fragment_2
                     || event == DictationCtrol::k_click_play_or_stop_fragment_3
                     || event == DictationCtrol::k_click_play_or_stop_fragment_4
                    )
            {
                int i = event - DictationCtrol::k_click_play_or_stop_fragment_1;   //i=0..3
                change_state(DictationCtrol::k_state_playing_fragment_1 + i);
                play_or_stop_fragment(i);
            }
            else if (event == DictationCtrol::k_click_done)
            {
                correct_exercise();
                change_state(k_state_solution);
            }
            return;

        //--------------------------------------------------------------------------
        case DictationCtrol::k_state_solution:           //solution is displayed
            if (event == DictationCtrol::k_click_play_or_stop_user_score)
            {
                change_state(DictationCtrol::k_state_playing_user);
                play_or_stop_user_score();
            }
            else if (event == DictationCtrol::k_click_play_or_stop_problem)
            {
                change_state(DictationCtrol::k_state_replaying_problem);
                play_or_stop_problem();
            }
            else if (event == DictationCtrol::k_click_play_or_stop_fragment_1
                     || event == DictationCtrol::k_click_play_or_stop_fragment_2
                     || event == DictationCtrol::k_click_play_or_stop_fragment_3
                     || event == DictationCtrol::k_click_play_or_stop_fragment_4
                    )
            {
                int i = event - DictationCtrol::k_click_play_or_stop_fragment_1;   //i=0..3
                change_state(DictationCtrol::k_state_replaying_fragment_1 + i);
                play_or_stop_fragment(i);
            }
            else if (event == DictationCtrol::k_click_new_problem)
            {
                if (is_theory_mode())
                    change_state(DictationCtrol::k_state_ready);
                else if (is_melodic_dictation())
                    change_state(DictationCtrol::k_state_playing_context);
                else if (num_fragments() == 1)
                    change_state(DictationCtrol::k_state_playing_complete);
                else
                    change_state(DictationCtrol::k_state_playing_fragment_1);

                new_problem();
            }
            return;

        //--------------------------------------------------------------------------
        case DictationCtrol::k_state_playing_user:       //program is playing user solution
            if (event == DictationCtrol::k_click_play_or_stop_user_score)
                play_or_stop_user_score();

            else if (event == DictationCtrol::k_end_of_playback)
            {
                m_pPlayUserScore->change_label(m_label_play_user_score);
                change_state(DictationCtrol::k_state_solution);
            }
            return;

        //--------------------------------------------------------------------------
        case DictationCtrol::k_state_replaying_problem:  //program is playing again problem score
            if (event == DictationCtrol::k_click_play_or_stop_problem)
                play_or_stop_problem();

            else if (event == DictationCtrol::k_end_of_playback)
            {
                m_pPlayProblem->change_label(m_label_play_problem);
                change_state(DictationCtrol::k_state_solution);
            }
            return;

        //--------------------------------------------------------------------------
        case DictationCtrol::k_state_replaying_fragment_1:    //program is replaying fragment 1
        case DictationCtrol::k_state_replaying_fragment_2:    //program is replaying fragment 2
        case DictationCtrol::k_state_replaying_fragment_3:    //program is replaying fragment 3
        case DictationCtrol::k_state_replaying_fragment_4:    //program is replaying fragment 4
        {
            int i = m_state - DictationCtrol::k_state_replaying_fragment_1;       //i=0..3
            if (event == FullEditorCtrol::k_end_of_playback)
            {
                m_pPlayFragment[i]->change_label(m_label_play[i]);
                change_state(DictationCtrol::k_state_solution);
            }
            else    //k_click_play_or_stop_fragment_i
                play_or_stop_fragment(i);

            return;
        }

        //--------------------------------------------------------------------------
        default:
            return;
    }
}


//=======================================================================================
// RhythmicDictationCtrol implementation
//=======================================================================================
RhythmicDictationCtrol::RhythmicDictationCtrol(long dynId, ApplicationScope& appScope,
                                 DocumentWindow* pCanvas)
    : DictationCtrol(dynId, appScope, pCanvas)
{
}

//---------------------------------------------------------------------------------------
RhythmicDictationCtrol::~RhythmicDictationCtrol()
{
}

//---------------------------------------------------------------------------------------
void RhythmicDictationCtrol::prepare_problem_score()
{
    //Generate a random score
    Composer composer(m_doc);
    //TODO: take midi instrument from exercise config / options
    composer.midi_instrument(0);     //Acoustic Grand Piano (0)
//    composer.midi_instrument(68);    //Oboe (68)
    m_pScoreConstrains->SetClef(k_clef_percussion, true);
    m_pScoreConstrains->SetClef(k_clef_G2, false);
    m_pScoreConstrains->SetClef(k_clef_F3, false);
    m_pScoreConstrains->SetClef(k_clef_F4, false);
    m_pScoreConstrains->SetClef(k_clef_C1, false);
    m_pScoreConstrains->SetClef(k_clef_C2, false);
    m_pScoreConstrains->SetClef(k_clef_C3, false);
    m_pScoreConstrains->SetClef(k_clef_C4, false);
    m_pProblemScore = composer.generate_score(m_pScoreConstrains);

    //get details about composed score
    m_clefType = composer.get_score_clef();
    m_keyType = composer.get_score_key_signature();
    m_timeType = composer.get_score_time_signature();
}

//---------------------------------------------------------------------------------------
void RhythmicDictationCtrol::prepare_user_score()
{
    AScore score = m_doc.create_object(k_obj_score).downcast_to_score();
    m_pUserScore = score.internal_object();
    ImoInstrument* pInstr = m_pUserScore->add_instrument();
    ImoSoundInfo* pSound = pInstr->get_sound_info(0);
    ImoMidiInfo* pMidi = pSound->get_midi_info();
    pMidi->set_midi_program(m_midiVoice);
    ImoSystemInfo* pInfo = m_pUserScore->get_first_system_info();
    pInfo->set_top_system_distance( pInstr->tenths_to_logical(30) );     // 3 lines
    ImoObj* pImo = pInstr->add_clef(m_clefType);
    pImo->set_editable(false);
    pImo = pInstr->add_key_signature(m_keyType);
    pImo->set_editable(false);
    int beats = get_top_number_for(m_timeType);
    int beatType = get_bottom_number_for(m_timeType);
    pImo = pInstr->add_time_signature(beats, beatType);
    pImo->set_editable(false);

    m_pUserScore->end_of_changes();
}



//=======================================================================================
// MelodicDictationCtrol implementation
//=======================================================================================
MelodicDictationCtrol::MelodicDictationCtrol(long dynId, ApplicationScope& appScope,
                                 DocumentWindow* pCanvas)
    : DictationCtrol(dynId, appScope, pCanvas)
{
}

//---------------------------------------------------------------------------------------
MelodicDictationCtrol::~MelodicDictationCtrol()
{
}

//---------------------------------------------------------------------------------------
void MelodicDictationCtrol::prepare_problem_score()
{
    //Generate a random score
    Composer composer(m_doc);
    //TODO: take midi instrument from exercise config / options
    composer.midi_instrument(0);     //Acoustic Grand Piano (0)
//    composer.midi_instrument(68);    //Oboe (68)
    m_pProblemScore = composer.generate_score(m_pScoreConstrains);

    //get details about composed score
    m_clefType = composer.get_score_clef();
    m_keyType = composer.get_score_key_signature();
    m_timeType = composer.get_score_time_signature();
}

//---------------------------------------------------------------------------------------
void MelodicDictationCtrol::prepare_user_score()
{
    AScore score = m_doc.create_object(k_obj_score).downcast_to_score();
    m_pUserScore = score.internal_object();
    ImoInstrument* pInstr = m_pUserScore->add_instrument();
    ImoSoundInfo* pSound = pInstr->get_sound_info(0);
    ImoMidiInfo* pMidi = pSound->get_midi_info();
    pMidi->set_midi_program(m_midiVoice);
    ImoSystemInfo* pInfo = m_pUserScore->get_first_system_info();
    pInfo->set_top_system_distance( pInstr->tenths_to_logical(30) );     // 3 lines
    ImoObj* pImo = pInstr->add_clef(m_clefType);
    pImo->set_editable(false);
    pImo = pInstr->add_key_signature(m_keyType);
    pImo->set_editable(false);
    int beats = get_top_number_for(m_timeType);
    int beatType = get_bottom_number_for(m_timeType);
    pImo = pInstr->add_time_signature(beats, beatType);
    pImo->set_editable(false);

    m_pUserScore->end_of_changes();
}



//=======================================================================================
// HarmonicDictationCtrol implementation
//=======================================================================================
HarmonicDictationCtrol::HarmonicDictationCtrol(long dynId, ApplicationScope& appScope,
                                 DocumentWindow* pCanvas)
    : DictationCtrol(dynId, appScope, pCanvas)
{
}

//---------------------------------------------------------------------------------------
HarmonicDictationCtrol::~HarmonicDictationCtrol()
{
}

//---------------------------------------------------------------------------------------
void HarmonicDictationCtrol::prepare_problem_score()
{
    //TODO: clef, key and time should be parameters for Composer
    m_clefType = k_clef_G2;
    m_keyType = k_key_C;                //k_key_D;
    m_timeType = k_time_4_4;

    //Generate a random score
    Composer composer(m_doc);
    m_pProblemScore = composer.generate_score(m_pScoreConstrains);

    //set instrument
    ImoInstrument* pInstr = m_pProblemScore->get_instrument(0);
    //TODO: take instrument from exercise config / options
    m_midiVoice = 68;       //Oboe
    ImoSoundInfo* pSound = pInstr->get_sound_info(0);
    ImoMidiInfo* pMidi = pSound->get_midi_info();
    pMidi->set_midi_program(m_midiVoice);
}

//---------------------------------------------------------------------------------------
void HarmonicDictationCtrol::prepare_user_score()
{
    AScore score = m_doc.create_object(k_obj_score).downcast_to_score();
    m_pUserScore = score.internal_object();
    ImoInstrument* pInstr = m_pUserScore->add_instrument();
    ImoSystemInfo* pInfo = m_pUserScore->get_first_system_info();
    pInfo->set_top_system_distance( pInstr->tenths_to_logical(30) );     // 3 lines
    pInstr->add_staff();    //second staff
    ImoSoundInfo* pSound = pInstr->get_sound_info(0);
    ImoMidiInfo* pMidi = pSound->get_midi_info();
    pMidi->set_midi_program(m_midiVoice);
    ImoObj* pClef1 = pInstr->add_clef(k_clef_G2, 1);
    ImoObj* pClef2 = pInstr->add_clef(k_clef_F4, 2);
    pClef1->set_editable(false);
    pClef2->set_editable(false);
    ImoObj* pImo = pInstr->add_key_signature(m_keyType);
    pImo->set_editable(false);
    int beats = get_top_number_for(m_timeType);
    int beatType = get_bottom_number_for(m_timeType);
    pImo = pInstr->add_time_signature(beats, beatType);
    pImo->set_editable(false);

    m_pUserScore->end_of_changes();
}



}  //namespace lenmus
