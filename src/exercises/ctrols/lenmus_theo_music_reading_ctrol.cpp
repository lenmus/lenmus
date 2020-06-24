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
#include "lenmus_theo_music_reading_ctrol.h"

#include "lenmus_theo_music_reading_ctrol_params.h"
#include "lenmus_scores_constrains.h"
#include "lenmus_dlg_cfg_score_reading.h"
#include "lenmus_generators.h"
#include "lenmus_document_canvas.h"
#include "lenmus_chord.h"
#include "lenmus_injectors.h"
#include "lenmus_colors.h"
#include "lenmus_composer.h"
#include "lenmus_url_aux_ctrol.h"

//lomse
#include <lomse_doorway.h>
#include <lomse_internal_model.h>
#include <lomse_im_note.h>
#include <lomse_staffobjs_table.h>
#include <lomse_im_factory.h>
using namespace lomse;


namespace lenmus
{

//=======================================================================================
// Implementation of TheoMusicReadingCtrol
//=======================================================================================
TheoMusicReadingCtrol::TheoMusicReadingCtrol(long dynId, ApplicationScope& appScope,
                                             DocumentWindow* pCanvas)
    : OneScoreCtrol(dynId, appScope, pCanvas)
{
}

//---------------------------------------------------------------------------------------
TheoMusicReadingCtrol::~TheoMusicReadingCtrol()
{
    delete m_pBaseConstrains;
}

//---------------------------------------------------------------------------------------
void TheoMusicReadingCtrol::initialize_ctrol()
{
    m_pConstrains = dynamic_cast<MusicReadingConstrains*>(m_pBaseConstrains);
    m_pScoreConstrains = m_pConstrains->GetScoreConstrains();

    //configuration options
    m_pConstrains->set_theory_mode(true);
    m_pConstrains->SetSolutionLink(false);
    m_pConstrains->SetUsingCounters(false);
    m_nPlayMM = 0;       //use metronome settings

    create_controls();
    new_problem();
}

//---------------------------------------------------------------------------------------
void TheoMusicReadingCtrol::get_ctrol_options_from_params()
{
    m_pBaseConstrains = LENMUS_NEW MusicReadingConstrains("MusicReading", m_appScope);
    TheoMusicReadingCtrolParams builder(m_pBaseConstrains);
    builder.process_params( m_pDyn->get_params() );
}

//---------------------------------------------------------------------------------------
void TheoMusicReadingCtrol::create_controls()
{
    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated
    initialize_strings();

    ImoStyle* pParaStyle = m_pDoc->create_private_style("Default style");
    pParaStyle->margin_top(500.0f)->margin_bottom(1000.0f);

    ImoStyle* pSpacerStyle = m_pDoc->create_private_style();
    pSpacerStyle->margin(0.0f)->padding(0.0f);

    LibraryScope* pLibScope = m_appScope.get_lomse().get_library_scope();

    //Create the problem manager and the problem space
    m_nGenerationMode = m_pConstrains->GetGenerationMode();
    create_problem_manager();

    //the window is divided into two regions: top for links, and bottom for the score

        // settings and debug options

    //create a paragraph for settings and debug options
    if (m_pConstrains->IncludeSettingsLink() || m_appScope.show_debug_links())
    {
        ImoParagraph* pTopLinePara = m_pDyn->add_paragraph(pParaStyle);

        // settings link
        if (m_pConstrains->IncludeSettingsLink())
        {
            HyperlinkCtrl* pSettingsLink =
                LENMUS_NEW HyperlinkCtrl(*pLibScope, nullptr, m_pDoc,
                                         to_std_string(_("Exercise options")) );
            pTopLinePara->add_control( pSettingsLink );
            pSettingsLink->add_event_handler(k_on_click_event, this, on_settings);
        }

        // debug links
        if (m_appScope.show_debug_links())
        {
            pTopLinePara->add_inline_box(1000.0f, pSpacerStyle);

            // "See source score"
            HyperlinkCtrl* pSeeSourceLink =
                LENMUS_NEW HyperlinkCtrl(*pLibScope, nullptr, m_pDoc,
                                         to_std_string(_("See source score")) );
            pTopLinePara->add_control( pSeeSourceLink );
            pSeeSourceLink->add_event_handler(k_on_click_event, this, on_see_source_score);
            pTopLinePara->add_inline_box(1000.0f, pSpacerStyle);

            // "See MIDI events"
            HyperlinkCtrl* pSeeMidiLink =
                LENMUS_NEW HyperlinkCtrl(*pLibScope, nullptr, m_pDoc,
                                         to_std_string(_("See MIDI events")) );
            pTopLinePara->add_control( pSeeMidiLink );
            pSeeMidiLink->add_event_handler(k_on_click_event, this, on_see_midi_events);
        }
    }

    //first line: check boxes for countoff and metronome
    ImoStyle* pParaStyle1 = m_pDoc->create_private_style("Default style");
    pParaStyle1->margin_top(500.0f)->margin_bottom(0.0f);
    ImoParagraph* pLinksPara1 = m_pDyn->add_paragraph(pParaStyle1);

    // "count off" check box
    m_pChkCountOff =
        LENMUS_NEW CheckboxCtrl(*pLibScope, nullptr, m_pDoc,
                                 to_std_string(_("Start with count off")) );
    pLinksPara1->add_control( m_pChkCountOff );
    pLinksPara1->add_inline_box(2000.0f, pSpacerStyle);

    // "use metronome" check box
    m_pChkMetronome =
        LENMUS_NEW CheckboxCtrl(*pLibScope, nullptr, m_pDoc,
                                 to_std_string(_("Play with metronome")) );
    pLinksPara1->add_control( m_pChkMetronome );

    //second line: links for go back, new problem and play
    ImoStyle* pParaStyle2 = m_pDoc->create_private_style("Default style");
    pParaStyle2->margin_top(0.0f)->margin_bottom(1000.0f);
    ImoParagraph* pLinksPara2 = m_pDyn->add_paragraph(pParaStyle2);

    // "Go back to theory" link
    if (m_pConstrains->IncludeGoBackLink())
    {
        HyperlinkCtrl* pGoBackLink =
            LENMUS_NEW HyperlinkCtrl(*pLibScope, nullptr, m_pDoc,
                                     to_std_string(_("Go back to theory")) );
        pGoBackLink->add_event_handler(k_on_click_event, this, on_go_back_event);
        pLinksPara2->add_control( pGoBackLink );
        pLinksPara2->add_inline_box(1000.0f, pSpacerStyle);
    }

    // "New problem" button
    m_pNewProblem =
        LENMUS_NEW HyperlinkCtrl(*pLibScope, nullptr, m_pDoc,
                                 to_std_string(_("New problem")) );
    m_pNewProblem->add_event_handler(k_on_click_event, this, on_new_problem);
    pLinksPara2->add_control( m_pNewProblem );
    pLinksPara2->add_inline_box(1000.0f, pSpacerStyle);

    // "Play" button
    if (m_pConstrains->IncludePlayLink())
    {
        m_pPlayButton =
            LENMUS_NEW HyperlinkCtrl(*pLibScope, nullptr, m_pDoc,
                                     to_std_string(_("Play")) );
        m_pPlayButton->add_event_handler(k_on_click_event, this, on_play_event);
        pLinksPara2->add_control( m_pPlayButton );
        pLinksPara2->add_inline_box(5000.0f, pSpacerStyle);
        //m_pDoc->add_event_handler(k_end_of_playback_event, this, on_end_of_play_event);
    }

//TODO "solfa" button
//    // "solfa" button
//    if (m_pConstrains->fSolfaCtrol) {
//        //m_pSolfaLink = LENMUS_NEW UrlAuxCtrol(this, ID_LINK_SOLFA, m_rScale, m_pConstrains->sSolfaLabel, lmNO_BITMAP, m_pConstrains->sStopSolfaLabel );
//        //m_pButtonsSizer->Add(
//        //    m_pPlayButton,
//        //    wxSizerFlags(0).Left().Border(wxALL, nSpacing) );
//    }
//

//    // horizontal line
//    wxStaticLine* pStaticLine1 = LENMUS_NEW wxStaticLine(this, wxID_ANY,
//                            wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
//    ((wxWindow*)pStaticLine1)->SetBackgroundColour(*wxRED);
//    ((wxWindow*)pStaticLine1)->SetForegroundColour(*wxBLUE);
//    m_pMainSizer->Add(pStaticLine1, 0, wxGROW|wxALL, nSpacing);
//
//
//    // create score ctrl

    // create a box to display problem
    ImoStyle* pProblemStyle = m_pDoc->create_private_style();
    create_problem_display_box( m_pDyn->add_content_wrapper(), pProblemStyle );

    m_pCounters = nullptr;
    m_fControlsCreated = true;
}

//---------------------------------------------------------------------------------------
wxDialog* TheoMusicReadingCtrol::get_settings_dialog()
{
    wxWindow* pParent = dynamic_cast<wxWindow*>(m_pCanvas);
    return LENMUS_NEW DlgCfgScoreReading(pParent, m_pScoreConstrains, m_pConstrains->GetSection());
}

//---------------------------------------------------------------------------------------
wxString TheoMusicReadingCtrol::set_new_problem()
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

    //delete the previous score
    if (m_pProblemScore)
    {
        delete m_pProblemScore;
        m_pProblemScore = nullptr;
    }

    //Generate a random score
    Composer composer(m_doc);
    m_pProblemScore = composer.generate_score(m_pScoreConstrains);
    return "";
}

//---------------------------------------------------------------------------------------
void TheoMusicReadingCtrol::set_problem_space()
{
    //Do nothing. For now, this exercise does not use Leitner method
}

//---------------------------------------------------------------------------------------
bool TheoMusicReadingCtrol::metronome_status()
{
    return m_pChkMetronome->is_checked();
}

//---------------------------------------------------------------------------------------
bool TheoMusicReadingCtrol::countoff_status()
{
    return m_pChkCountOff->is_checked();
}


}  //namespace lenmus
