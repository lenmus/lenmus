//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2011 LenMus project
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

//#include "../score/VStaff.h"
//#include "../score/Instrument.h"
//#include "../score/Pitch.h"
#include "lenmus_constrains.h"
#include "lenmus_generators.h"
#include "lenmus_score_canvas.h"
#include "lenmus_notes_constrains.h"
#include "lenmus_idfy_notes_ctrol_params.h"
#include "lenmus_dlg_cfg_idfy_notes.h"
//#include "lenmus_scales_manager.h"
#include "lenmus_url_aux_ctrol.h"
#include "lenmus_injectors.h"
#include "lenmus_colors.h"

////access to error's logger
//#include "../app/Logger.h"
//extern lmLogger* g_pLogger;
//
////access to MIDI manager to get default settings for instrument to use
//#include "../sound/MidiManager.h"

//lomse
#include <lomse_doorway.h>
#include <lomse_internal_model.h>
#include <lomse_im_note.h>
#include <lomse_staffobjs_table.h>
#include <lomse_im_factory.h>
using namespace lomse;

//wxWidgets
#include <wx/wxprec.h>



namespace lenmus
{

//------------------------------------------------------------------------------------
// Implementation of IdfyNotesCtrol




//BEGIN_EVENT_TABLE(IdfyNotesCtrol, OneScoreCtrol)
//    EVT_COMMAND_RANGE(lmID_BUTTON, lmID_BUTTON+k_num_buttons-1, wxEVT_COMMAND_BUTTON_CLICKED, IdfyNotesCtrol::OnRespButton)
//    LM_EVT_URL_CLICK(lmID_PLAY_A4, IdfyNotesCtrol::OnPlayA4)
//    LM_EVT_URL_CLICK(lmID_PLAY_ALL_NOTES, IdfyNotesCtrol::OnPlayAllNotes)
//    LM_EVT_URL_CLICK(lmID_CONTINUE, IdfyNotesCtrol::OnContinue)
//END_EVENT_TABLE()


IdfyNotesCtrol::IdfyNotesCtrol(long dynId, ApplicationScope& appScope,
                                       DocumentCanvas* pCanvas)
    : OneScoreCtrol(dynId, appScope, pCanvas)
{
}

//---------------------------------------------------------------------------------------
void IdfyNotesCtrol::initialize_ctrol()
{
    m_pConstrains = dynamic_cast<NotesConstrains*>(m_pBaseConstrains);
    m_pConstrains->set_theory_mode(false);
    create_controls();
    set_initial_state();
}

//---------------------------------------------------------------------------------------
IdfyNotesCtrol::~IdfyNotesCtrol()
{
}

//---------------------------------------------------------------------------------------
void IdfyNotesCtrol::get_ctrol_options_from_params()
{
    m_pBaseConstrains = new NotesConstrains(_T("IdfyNotes"), m_appScope);
    IdfyNotesCtrolParams builder(m_pBaseConstrains);
    builder.process_params( m_pDyn->get_params() );
}

//---------------------------------------------------------------------------------------
void IdfyNotesCtrol::set_problem_space()
{
    //Do nothing. For now, this exercise does not use problem spaces
}

//---------------------------------------------------------------------------------------
void IdfyNotesCtrol::create_answer_buttons(LUnits height, LUnits spacing)
{
    ImoStyle* pDefStyle = m_pDoc->get_default_style();
    ImoInlineWrapper* pBox;

    ImoStyle* pBtStyle = m_pDoc->create_private_style();
    pBtStyle->set_string_property(ImoStyle::k_font_name, "sans-serif");
    pBtStyle->set_float_property(ImoStyle::k_font_size, 8.0f);

    ImoStyle* pRowStyle = m_pDoc->create_private_style();
    pRowStyle->set_lunits_property(ImoStyle::k_font_size, 10.0f);
    pRowStyle->set_lunits_property(ImoStyle::k_margin_bottom, 0.0f);

    USize buttonSize(1500.0f, height);
    USize bigButtonSize(3200.0f, height);
    LUnits otherRowsWidth = buttonSize.width + spacing;

    LUnits linkWidth = 4000.0f;

    //Change 'Play' label to 'Play again'
    m_pPlayButton->replace_normal_label(_("Play again"));

    //a paragraph for extra links
    ImoParagraph* pPlayRefPara = m_pDyn->add_paragraph(pRowStyle);

    // "Play all notes to identify" link
//    m_pPlayAllNotes = new UrlAuxCtrol(this, lmID_PLAY_ALL_NOTES, m_rScale,
//                                        _("Play all notes to identify"), _T(""),
//                                        _("Stop playing"), _T("") );
    m_pPlayAllNotes = new UrlAuxCtrol(this, k_on_click_event, pPlayRefPara, m_pDoc,
                                      "link_play_all_notes", "Play all notes to identify",
                                      linkWidth);

    // "Play A4 reference note" link
//    m_pPlayA4 = new UrlAuxCtrol(this, lmID_PLAY_A4, m_rScale,
//                                        _("Play A4 reference note"), _T(""),
//                                        _("Stop playing"), _T("") );
    m_pPlayA4 = new UrlAuxCtrol(this, k_on_click_event, pPlayRefPara, m_pDoc,
                                "link_play_a4", "Play A4 reference note",
                                linkWidth);

    // "Continue" link
    m_pContinue = new UrlAuxCtrol(this, k_on_click_event, pPlayRefPara, m_pDoc,
                                  "link_continue", "Continue", linkWidth);

    //keyboard laout: two rows, like a piano keyboard

    //first row: sharp/flat notes
    ImoParagraph* pFirstRow = m_pDyn->add_paragraph(pRowStyle);
    pBox = pFirstRow->add_inline_box(otherRowsWidth, pDefStyle);
    //
    pBox = pFirstRow->add_inline_box(otherRowsWidth, pDefStyle);
    m_pAnswerButton[1] = pBox->add_button(m_sButtonLabel[1], buttonSize, pBtStyle);
    pBox = pFirstRow->add_inline_box(otherRowsWidth, pDefStyle);
    m_pAnswerButton[3] = pBox->add_button(m_sButtonLabel[3], buttonSize, pBtStyle);
    //
    pBox = pFirstRow->add_inline_box(otherRowsWidth, pDefStyle);
    //
    pBox = pFirstRow->add_inline_box(otherRowsWidth, pDefStyle);
    m_pAnswerButton[6] = pBox->add_button(m_sButtonLabel[6], buttonSize, pBtStyle);
    pBox = pFirstRow->add_inline_box(otherRowsWidth, pDefStyle);
    m_pAnswerButton[8] = pBox->add_button(m_sButtonLabel[8], buttonSize, pBtStyle);
    pBox = pFirstRow->add_inline_box(otherRowsWidth, pDefStyle);
    m_pAnswerButton[10] = pBox->add_button(m_sButtonLabel[10], buttonSize, pBtStyle);

    //second row: natural notes
    ImoParagraph* pSecondRow = m_pDyn->add_paragraph(pRowStyle);
    pBox = pSecondRow->add_inline_box(otherRowsWidth, pDefStyle);
    m_pAnswerButton[0] = pBox->add_button(m_sButtonLabel[0], buttonSize, pBtStyle);
    pBox = pSecondRow->add_inline_box(otherRowsWidth, pDefStyle);
    m_pAnswerButton[2] = pBox->add_button(m_sButtonLabel[2], buttonSize, pBtStyle);
    pBox = pSecondRow->add_inline_box(otherRowsWidth, pDefStyle);
    m_pAnswerButton[4] = pBox->add_button(m_sButtonLabel[4], buttonSize, pBtStyle);
    pBox = pSecondRow->add_inline_box(otherRowsWidth, pDefStyle);
    m_pAnswerButton[7] = pBox->add_button(m_sButtonLabel[7], buttonSize, pBtStyle);
    pBox = pSecondRow->add_inline_box(otherRowsWidth, pDefStyle);
    m_pAnswerButton[9] = pBox->add_button(m_sButtonLabel[9], buttonSize, pBtStyle);
    pBox = pSecondRow->add_inline_box(otherRowsWidth, pDefStyle);
    m_pAnswerButton[11] = pBox->add_button(m_sButtonLabel[11], buttonSize, pBtStyle);

    //inform base class about the settings
    set_buttons(m_pAnswerButton, k_num_buttons);
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
//TODO 5.0 commented out
//        lmComputeAccidentals(m_pConstrains->GetKeySignature(), nAcc);

        ImoButton* pNaturalButton[7];
        pNaturalButton[0] = m_pAnswerButton[0];
        pNaturalButton[1] = m_pAnswerButton[2];
        pNaturalButton[2] = m_pAnswerButton[4];
        pNaturalButton[3] = m_pAnswerButton[5];
        pNaturalButton[4] = m_pAnswerButton[7];
        pNaturalButton[5] = m_pAnswerButton[9];
        pNaturalButton[6] = m_pAnswerButton[11];

        static const wxString sSteps = _T("CDEFGAB");
        for (int iB=0; iB < 7; iB++)
        {
            wxString sLabel = sSteps.Mid(iB, 1);
            if (nAcc[iB] == 1)
                sLabel += _T(" #");
            else if (nAcc[iB] == -1)
                sLabel += _T(" b");
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

    EnableButtons(true);
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

}

//---------------------------------------------------------------------------------------
wxDialog* IdfyNotesCtrol::get_settings_dialog()
{
    wxWindow* pParent = dynamic_cast<wxWindow*>(m_pCanvas);
    return new DlgCfgIdfyNotes(pParent, m_pConstrains);
}

//---------------------------------------------------------------------------------------
void IdfyNotesCtrol::prepare_aux_score(int nButton)
{
    if (m_pAuxScore)
        delete m_pAuxScore;
    m_pAuxScore = NULL;
}

//---------------------------------------------------------------------------------------
wxString IdfyNotesCtrol::set_new_problem()
{
    //This method must prepare the problem score and set variables:
    //  m_pProblemScore, m_pSolutionScore, m_sAnswer, m_nRespIndex and m_nPlayMM

    wxString sNote;
    RandomGenerator oGenerator;
    EClefExercise nClef = m_pConstrains->GetClef();

    //select octave
    int nSecondOctave = m_pConstrains->GetOctaves() == 2 ? oGenerator.FlipCoin() : 0;
    int nOctave;
    switch (nClef)
    {
        case lmE_Sol:   nOctave = 4 + nSecondOctave;    break;  //4,5
        case lmE_Fa4:   nOctave = 3 - nSecondOctave;    break;  //3,2
        case lmE_Fa3:   nOctave = 3 - nSecondOctave;    break;  //3,2
        case lmE_Do1:   nOctave = 4 - nSecondOctave;    break;  //4,3
        case lmE_Do2:   nOctave = 4 - nSecondOctave;    break;  //4,3
        case lmE_Do3:   nOctave = 4 - nSecondOctave;    break;  //4,3
        case lmE_Do4:   nOctave = 3 + nSecondOctave;    break;  //3,4
    }

    // generate a random note and set m_nKey, nStep, nAcc and nNoteIndex
    int nNoteIndex;
    int nStep;
    int nAcc;
//TODO 5.0 commented out
//    if (m_pConstrains->SelectNotesFromKeySignature())
//    {
//        m_nKey = m_pConstrains->GetKeySignature();
//
//        //Generate a random root note
//        wxString sRootNote = oGenerator.GenerateRandomRootNote(nClef, m_nKey, false);  //false = do not allow accidentals. Only those in the key signature
//        lmScalesManager oScaleMngr(sRootNote, est_MajorNatural, m_nKey);
//
//        //generate a random note, different from previous one
//        static int iPrev = 0;
//        int iN = oGenerator.RandomNumber(0, 6);
//        while (iPrev == iN)
//            iN = oGenerator.RandomNumber(0, 6);
//        iPrev = iN;
//
//        FPitch fpNote = oScaleMngr.GetNote(iN);
//        nStep = FPitch_Step(fpNote);
//        nAcc = FPitch_Accidentals(fpNote);
//        switch(nStep)
//        {
//            case 0: nNoteIndex = 0; break;
//            case 1: nNoteIndex = 2; break;
//            case 2: nNoteIndex = 4; break;
//            case 3: nNoteIndex = 5; break;
//            case 4: nNoteIndex = 7; break;
//            case 5: nNoteIndex = 9; break;
//            case 6: nNoteIndex = 11; break;
//        }
//    }
//    else
//    {
//        m_nKey = k_key_C;
//        nNoteIndex = m_pConstrains->GetRandomNoteIndex();
//        static const int nNoteStep[12] = { 0,0,1,1,2,3,3,4,4,5,5,6 };
//        static const int nNoteAcc[12] = { 0,1,0,1,0,0,1,0,1,0,1,0 };
//        nStep = nNoteStep[nNoteIndex];
//        nAcc = nNoteAcc[nNoteIndex];
//    }
//    sNote = FPitch_ToAbsLDPName( FPitch(nStep, nOctave, nAcc) );

    //prepare answer
    static const wxString sSteps = _T("CDEFGAB");
    wxString sAnswer = sSteps.Mid(nStep, 1);
    if (nAcc == 1)
        sAnswer += _T(" sharp");
    else if (nAcc == -1)
        sAnswer += _T(" flat");
    m_sAnswer = _("The note is: ");
    m_sAnswer += wxGetTranslation(sAnswer);
    m_sAnswer += _T("\n");
    m_sAnswer += _("Click on 'Continue' to listen a new note");

    //create the score
    if (m_pConstrains->is_theory_mode())
        prepare_score(nClef, sNote, &m_pProblemScore);
    else
        prepare_score(nClef, sNote, &m_pProblemScore, &m_pSolutionScore);

	//compute the index for the button that corresponds to the right answer
    m_nRespIndex = nNoteIndex;

    return _("Identify the next note:");
}

//---------------------------------------------------------------------------------------
wxString IdfyNotesCtrol::prepare_score(EClefExercise nClef, wxString& sNotePitch,
                                       ImoScore** pProblemScore,
                                       ImoScore** pSolutionScore)
{
    //====================================================================================
    //Example of new code for creating a score
    static int iNote = 0;
    static string notes[] = {"(n e4 w)", "(n f4 w)", "(n g4 w)", "(n a4 w)", "(n b4 w)" };

    ImoScore* pScore = static_cast<ImoScore*>(ImFactory::inject(k_imo_score, m_pDoc));
    ImoInstrument* pInstr = pScore->add_instrument();
    pInstr->add_clef(k_clef_G2);
    pInstr->add_object("(n c4 w)");
    pInstr->add_object( notes[(iNote++)%5] );
    pInstr->add_object("(barline simple)");
    //pInstr->add_barline(ImoBarline::k_simple);

    ColStaffObjsBuilder builder;
    builder.build(pScore);

    *pProblemScore = pScore;
    *pSolutionScore = NULL;

    //====================================================================================
    return sNotePitch;


//TODO 5.0 commented out
//    //delete the previous score
//    if (*pProblemScore)
//    {
//        delete *pProblemScore;
//        *pProblemScore = NULL;
//    }
//    if (pSolutionScore)
//    {
//        delete *pSolutionScore;
//        *pSolutionScore = NULL;
//    }

//    //create the score with the note
//    string sPattern;
//
//    *pProblemScore = static_cast<ImoScore*>(ImFactory::inject(k_imo_score, m_pDoc));
//    (*pProblemScore)->SetOption(_T("Render.SpacingMethod"), (long)esm_Fixed);
//    ImoInstrument* pInstr = (*pProblemScore)->add_instrument();    // (g_pMidi->DefaultVoiceChannel(),
//							g_pMidi->DefaultVoiceInstr(), _T(""));
//    (*pProblemScore)->SetTopSystemDistance( pVStaff->TenthsToLogical(50, 1) );     // 5 lines
//    pInstr->add_clef( nClef );
//    pInstr->add_key_signature(m_nKey);
//    pInstr->add_time_signature(2, 4);
//
//    // Now add the note to identify
//    sPattern = "(n " + sNotePitch + " w)";
//    pInstr->add_object(( sPattern );
//
    return sNotePitch;
}

//---------------------------------------------------------------------------------------
void IdfyNotesCtrol::OnPlayA4(wxCommandEvent& event)
{
//TODO 5.0 commented out
//    StopSounds();
//
//    //delete any previous score
//    if (m_pAuxScore)
//        delete m_pAuxScore;
//
//    //create an score with the A4 note
//    m_pAuxScore = static_cast<ImoScore*>(ImFactory::inject(k_imo_score, m_pDoc));
//    ImoInstrument* pInstr = m_pAuxScore->add_instrument();    // (g_pMidi->DefaultVoiceChannel(),
//							g_pMidi->DefaultVoiceInstr(), _T(""));
//    lmVStaff* pVStaff = pInstr->GetVStaff();
//    pInstr->add_clef( lmE_Sol );
//    pInstr->add_key_signature( k_key_C );
//    pInstr->add_time_signature(2 ,4);
//    lmLDPParser parser;
//    lmLDPNode* pNode = parser.ParseText( _T("(n a4 w)") );
//    parser.AnalyzeNote(pNode, pVStaff);
//
//    m_pAuxScore->Play(lmNO_VISUAL_TRACKING, lmNO_COUNTOFF,
//                      ePM_NormalInstrument, m_nPlayMM, (wxWindow*) NULL);
}

//---------------------------------------------------------------------------------------
void IdfyNotesCtrol::OnPlayAllNotes(wxCommandEvent& event)
{
//TODO 5.0 commented out
//    ((ImoScoreAuxCtrol*)m_pDisplayCtrol)->PlayScore(lmVISUAL_TRACKING, lmNO_COUNTOFF,
//                            ePM_NormalInstrument, m_nPlayMM);
}

//---------------------------------------------------------------------------------------
int IdfyNotesCtrol::GetFirstOctaveForClef(EClefExercise nClef)
{
    switch (nClef)
    {
        case lmE_Sol:   return 4;
        case lmE_Fa4:   return 3;
        case lmE_Fa3:   return 3;
        case lmE_Do1:   return 4;
        case lmE_Do2:   return 4;
        case lmE_Do3:   return 4;
        case lmE_Do4:   return 3;
        default:
            return 4;
    }
}

//---------------------------------------------------------------------------------------
void IdfyNotesCtrol::PrepareAllNotesScore()
{
    //This method prepares a score with all the notes to identify and
    //stores it in m_pProblemScore

    EClefExercise nClef = m_pConstrains->GetClef();

    //select octave
    int nFirstOctave = GetFirstOctaveForClef(nClef);
    int nSecondOctave = nFirstOctave;
    bool fTwoOctaves = (m_pConstrains->GetOctaves() == 2);
    if (fTwoOctaves)
    {
        switch (nClef)
        {
            case lmE_Sol:   nSecondOctave += 1;    break;  //4,5
            case lmE_Fa4:   nSecondOctave -= 1;    break;  //3,2
            case lmE_Fa3:   nSecondOctave -= 1;    break;  //3,2
            case lmE_Do1:   nSecondOctave -= 1;    break;  //4,3
            case lmE_Do2:   nSecondOctave -= 1;    break;  //4,3
            case lmE_Do3:   nSecondOctave -= 1;    break;  //4,3
            case lmE_Do4:   nSecondOctave += 1;    break;  //3,4
        }
    }

//TODO 5.0 commented out
//    //create the score
//    wxString sPattern;
//    ImoNote* pNote;
//    lmLDPParser parserLDP;
//    lmLDPNode* pNode;
//    lmVStaff* pVStaff;
//
//    ImoScore* pScore = static_cast<ImoScore*>(ImFactory::inject(k_imo_score, m_pDoc));
//    pScore->SetOption(_T("Render.SpacingMethod"), (long)esm_Fixed);
//    ImoInstrument* pInstr = pScore->add_instrument();    // (g_pMidi->DefaultVoiceChannel(),
//							g_pMidi->DefaultVoiceInstr(), _T(""));
//    pVStaff = pInstr->GetVStaff();
//    pScore->SetTopSystemDistance( pVStaff->TenthsToLogical(30, 1) );     // 3 lines
//    pInstr->add_clef( nClef );
//    pInstr->add_key_signature( k_key_C );
//
//    //generate all valid notes
//    for (int i=0; i < 12; i++)
//    {
//        if (m_pConstrains->IsValidNote(i))
//        {
//            wxString sNote;
//            if (m_pConstrains->SelectNotesFromKeySignature())
//            {
//                int nStep;
//                int nAcc[7];
//                lmComputeAccidentals(m_pConstrains->GetKeySignature(), nAcc);
//
//                switch(i)
//                {
//                    case 0: //C or B#
//                        nStep = (nAcc[6] == 1 ? 6 : 0);
//                        break;
//
//                    case 1: //C# or Db
//                        nStep = (nAcc[1] == -1 ? 1 : 0);
//                        break;
//
//                    case 2: //D
//                        nStep = 1;
//                        break;
//
//                    case 3: //D# or Eb
//                        nStep = (nAcc[2] == -1 ? 2 : 1);
//                        break;
//
//                    case 4: //E or Fb
//                        nStep = (nAcc[3] == -1 ? 3 : 2);
//                        break;
//
//                    case 5: //E# or F
//                        nStep = (nAcc[2] == 1 ? 2 : 3);
//                        break;
//
//                    case 6: //F# or Gb
//                        nStep = (nAcc[3] == 1 ? 3 : 4);
//                        break;
//
//                    case 7: //G
//                        nStep = 4;
//                        break;
//
//                    case 8: //G# or Ab
//                        nStep = (nAcc[4] == 1 ? 4 : 5);
//                        break;
//
//                    case 9: //A
//                        nStep = 5;
//                        break;
//
//                    case 10: //A# or Bb
//                        nStep = (nAcc[5] == 1 ? 5 : 6);
//                        break;
//
//                    case 11: //B or Cb
//                        nStep = (nAcc[0] == -1 ? 0 : 6);
//                        break;
//                }
//                sNote = FPitch_ToAbsLDPName( FPitch(nStep, nFirstOctave, nAcc[nStep]) );
//            }
//            else
//            {
//                int nStep;
//                int nAcc = 0;
//                switch(i)
//                {
//                    case 0: //C or B#
//                        nStep = 0;
//                        break;
//
//                    case 1: //C# or Db
//                        nStep = 0;
//                        nAcc = 1;
//                        break;
//
//                    case 2: //D
//                        nStep = 1;
//                        break;
//
//                    case 3: //D# or Eb
//                        nStep = 1;
//                        nAcc = 1;
//                        break;
//
//                    case 4: //E or Fb
//                        nStep = 2;
//                        break;
//
//                    case 5: //E# or F
//                        nStep = 2;
//                        break;
//
//                    case 6: //F# or Gb
//                        nStep = 3;
//                        nAcc = 1;
//                        break;
//
//                    case 7: //G
//                        nStep = 4;
//                        break;
//
//                    case 8: //G# or Ab
//                        nStep = 4;
//                        nAcc = 1;
//                        break;
//
//                    case 9: //A
//                        nStep = 5;
//                        break;
//
//                    case 10: //A# or Bb
//                        nStep = 5;
//                        nAcc = 1;
//                        break;
//
//                    case 11: //B or Cb
//                        nStep = 6;
//                        break;
//                }
//                sNote = FPitch_ToAbsLDPName( FPitch(nStep, nFirstOctave, nAcc) );
//            }
//
//            //add note
//            sPattern = _T("(n ") + sNote + _T(" w)");
//            pInstr->add_object(( sPattern );
//            pNote = parserLDP.AnalyzeNote(pNode, pVStaff);
//
//        }
//    }
//    pInstr->add_spacer(50);
//    pInstr->add_barline(ImoBarline::k_simple, NO_VISIBLE);
//    if (m_pProblemScore)
//        delete m_pProblemScore;
//    m_pProblemScore = pScore;
}

//---------------------------------------------------------------------------------------
void IdfyNotesCtrol::set_initial_state()
{
    reset_exercise();

    //display the intro
    m_fQuestionAsked = false;
    wxString sMsg = _("Click on 'New problem' to start");
    display_message(sMsg, false);
    m_pPlayButton->enable(false);
    m_pShowSolution->enable(false);
    m_pPlayA4->enable(false);
    m_pPlayAllNotes->enable(false);
    m_pContinue->enable(false);
}

//---------------------------------------------------------------------------------------
void IdfyNotesCtrol::OnNewProblem(wxCommandEvent& event)
{
    DisplayAllNotes();
    m_pPlayButton->enable(false);
    m_pShowSolution->enable(false);
}

//---------------------------------------------------------------------------------------
void IdfyNotesCtrol::DisplayAllNotes()
{
//TODO 5.0 commented out
//    wxString sProblemMessage = _("You will have to identify the following notes:");
//    PrepareAllNotesScore();
//    ((ImoScoreAuxCtrol*)m_pDisplayCtrol)->SetScore(m_pProblemScore);
//    display_message(sProblemMessage, false);
//    m_pPlayA4->enable(true);
//    m_pNewProblem->enable(false);
//    m_pPlayAllNotes->enable(true);
//    m_pContinue->enable(true);
}

//---------------------------------------------------------------------------------------
void IdfyNotesCtrol::OnContinue(wxCommandEvent& event)
{
//TODO 5.0 commented out
//    reset_exercise();
//
//    //prepare answer buttons and counters
//    if (m_pCounters && m_fCountersValid)
//        m_pCounters->OnNewQuestion();
//    EnableButtons(true);
//
//    //set m_pProblemScore, m_pSolutionScore, m_sAnswer, m_nRespIndex, m_nPlayMM
//    wxString sProblemMessage = set_new_problem();
//
//    //display the problem
//    m_fQuestionAsked = true;
//    display_problem();
//    display_message(sProblemMessage, false);
//
//    //enable/disable links
//    m_pPlayButton->enable(true);
//    m_pShowSolution->enable(true);
//    m_pPlayA4->enable(false);
//    m_pNewProblem->enable(true);
//    m_pPlayAllNotes->enable(false);
//    m_pContinue->enable(false);
//
//    //save time
//    m_tmAsked = wxDateTime::Now();
}

//---------------------------------------------------------------------------------------
void IdfyNotesCtrol::OnRespButton(wxCommandEvent& event)
{
//TODO 5.0 commented out
//    //First, stop any possible score being played to avoid crashes
//    StopSounds();
//
//    //identify button pressed
//    int nIndex = event.GetId() - m_nIdFirstButton;
//
//    if (m_fQuestionAsked)
//    {
//        // There is a question asked. The user press the button to give the answer
//
//        //verify if success or failure
//        bool fSuccess = (nIndex == m_nRespIndex);
//
//        //inform problem manager of the result
//        OnQuestionAnswered(m_iQ, fSuccess);
//
//        //produce feedback sound, and update statistics display
//        if (m_pCounters)
//        {
//            m_pCounters->UpdateDisplay();
//            m_pCounters->RightWrongSound(fSuccess);
//        }
//
//        //if success generate a new problem. Else, ask user to tray again
//        if (fSuccess)
//            NewProblem();
//        else
//        {
//            m_fQuestionAsked = true;
//            display_problem();
//            display_message(_("Try again!"), false);
//        }
//    }
//    //else
//        // No problem presented. Ignore click on answer button
}

//---------------------------------------------------------------------------------------
void IdfyNotesCtrol::display_solution()
{
    OneScoreCtrol::display_solution();

    m_pContinue->enable(true);
}


}  //namespace lenmus
