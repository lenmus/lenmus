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

//#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
//#pragma implementation "IdfyCadencesCtrol.h"
//#endif
//
//// For compilers that support precompilation, includes <wx.h>.
//#include <wx/wxprec.h>
//
//#ifdef __BORLANDC__
//#pragma hdrstop
//#endif
//
//#include "IdfyCadencesCtrol.h"

////lomse
//#include <lomse_doorway.h>
//#include <lomse_internal_model.h>
//#include <lomse_im_note.h>
//#include <lomse_staffobjs_table.h>
//#include <lomse_im_factory.h>
//using namespace lomse;

//#include "../score/VStaff.h"
//#include "../score/Instrument.h"
//#include "lenmus_constrains.h"
//#include "lenmus_generators.h"
//#include "lenmus_score_canvas.h"
//#include "../auxmusic/Conversion.h"
//
//#include "../ldp_parser/LDPParser.h"
//#include "dialogs/DlgCfgIdfyCadence.h"
//#include "../auxmusic/Chord.h"
//#include "../auxmusic/Cadence.h"
//
//
//#include "lenmus_injectors.h"
//#include "lenmus_colors.h"
//
////access to error's logger
//#include "../app/Logger.h"
//extern lmLogger* g_pLogger;
//
////access to MIDI manager to get default settings for instrument to use
//#include "../sound/MidiManager.h"


namespace lenmus
{

////------------------------------------------------------------------------------------
//// Implementation of IdfyCadencesCtrol
//
//
//
//
//static wxString m_sButtonLabel[lm_eCadMaxCadence];
//
////IDs for controls
//enum {
//    ID_BUTTON = 3010,
//};
//
//
//BEGIN_EVENT_TABLE(IdfyCadencesCtrol, OneScoreCtrol)
//    EVT_COMMAND_RANGE (ID_BUTTON, ID_BUTTON+k_num_buttons-1, wxEVT_COMMAND_BUTTON_CLICKED, IdfyCadencesCtrol::OnRespButton)
//END_EVENT_TABLE()
//
//
//IdfyCadencesCtrol::IdfyCadencesCtrol(long dynId, ApplicationScope& appScope,
//                                       DocumentCanvas* pCanvas)
//    : OneScoreCtrol(dynId, appScope, pCanvas)
//{
//}
//
////---------------------------------------------------------------------------------------
//void IdfyCadencesCtrol::get_ctrol_options_from_params()
//{
//    m_pBaseConstrains = new TheoIntervalsConstrains("TheoIntervals", m_appScope);
//    IdfyCadencesCtrolParams builder(m_pBaseConstrains);
//    builder.process_params( m_pDyn->get_params() );
//}
//
////---------------------------------------------------------------------------------------
//void IdfyCadencesCtrol::initialize_ctrol()
//{
//    //initializations
//    m_pConstrains = pConstrains;
//
//    //initializatios to allow to play cadences when clicking on answer buttons
//    //TODO: Review this
//    m_nKey = k_key_C;
//
//    CreateControls();
//    if (m_pConstrains->is_theory_mode())
//        NewProblem();
//}
//
//IdfyCadencesCtrol::~IdfyCadencesCtrol()
//{
//}
//
//void IdfyCadencesCtrol::create_answer_buttons(LUnits height, LUnits spacing)
//{
//    //create buttons for the answers, two rows
//    int iB = 0;
//    for (iB=0; iB < k_num_buttons; iB++)
//        m_pAnswerButton[iB] = (wxButton*)NULL;
//
//    m_pKeyboardSizer = new wxFlexGridSizer(k_num_rows+1, k_num_cols+1, 2*nSpacing, 0);
//    m_pMainSizer->Add(
//        m_pKeyboardSizer,
//        wxSizerFlags(0).Left().Border(wxALIGN_LEFT|wxTOP, 2*nSpacing) );
//
//    for (int iRow=0; iRow < k_num_rows; iRow++) {
//        // the buttons for this row
//        for (int iCol=0; iCol < k_num_cols; iCol++) {
//            iB = iCol + iRow * k_num_cols;    // button index
//            if (iB >= k_num_buttons) break;
//            m_pAnswerButton[iB] = new wxButton( this, ID_BUTTON + iB, _T("Undefined"),
//                wxDefaultPosition, wxSize(24*nSpacing, nHeight));
//            m_pAnswerButton[iB]->SetFont(font);
//
//            m_pKeyboardSizer->Add(
//                m_pAnswerButton[iB],
//                wxSizerFlags(0).Border(wxLEFT|wxRIGHT, nSpacing) );
//        }
//    }
//
//    //inform base class about the settings
//    SetButtons(m_pAnswerButton, k_num_buttons, ID_BUTTON);
//
//
//    //====================================================================================
//    //Example of new code taken from IdfyIntervalsCtrol
//    ImoStyle* pDefStyle = m_pDoc->get_default_style();
//    ImoInlineWrapper* pBox;
//
//    //create 48 buttons for the answers: six rows, eight buttons per row,
//    //plus two additional buttons, for 'unison' and 'chromatic semitone'
//
//    ImoStyle* pBtStyle = m_pDoc->create_private_style();
//    pBtStyle->set_string_property(ImoStyle::k_font_name, "sans-serif");
//    pBtStyle->set_float_property(ImoStyle::k_font_size, 8.0f);
//
//    ImoStyle* pRowStyle = m_pDoc->create_private_style();
//    pRowStyle->set_lunits_property(ImoStyle::k_font_size, 10.0f);
//    pRowStyle->set_lunits_property(ImoStyle::k_margin_bottom, 0.0f);
//
//    USize buttonSize(1500.0f, height);
//    USize bigButtonSize(3200.0f, height);
//    LUnits firstRowWidth = 4000.0f;
//    LUnits otherRowsWidth = buttonSize.width + spacing;
//    LUnits unisonRowsWidth = bigButtonSize.width + 2.0f * spacing;
//
//
//    int iB;
//    for (iB=0; iB < k_num_buttons; iB++) {
//        m_pAnswerButton[iB] = NULL;
//    }
//
//    //row with buttons for unison and related
//    ImoParagraph* pUnisonRow = m_pDyn->add_paragraph(pRowStyle);
//
//        //spacer to skip the labels
//    pBox = pUnisonRow->add_inline_box(firstRowWidth, pDefStyle);
//
//        //unison button
//    pBox = pUnisonRow->add_inline_box(unisonRowsWidth, pDefStyle);
//    iB = lmIDX_UNISON;
//    m_pAnswerButton[iB] = pBox->add_button(m_sIntvButtonLabel[iB],
//                                           bigButtonSize, pBtStyle);
//
//        // "chromatic semitone" button
//    pBox = pUnisonRow->add_inline_box(unisonRowsWidth, pDefStyle);
//    iB = lmIDX_SEMITONE;
//    m_pAnswerButton[iB] = pBox->add_button(m_sIntvButtonLabel[iB],
//                                           bigButtonSize, pBtStyle);
//
//        // "chromatic tone" button
//    pBox = pUnisonRow->add_inline_box(unisonRowsWidth, pDefStyle);
//    iB = lmIDX_TONE;
//    m_pAnswerButton[iB] = pBox->add_button(m_sIntvButtonLabel[iB],
//                                           bigButtonSize, pBtStyle);
//
//
//    //Now main keyboard with all other buttons
//
//    //row with column labels
//    ImoParagraph* pKeyboardRow = m_pDyn->add_paragraph(pRowStyle);
//
//    //spacer
//    pBox = pKeyboardRow->add_inline_box(firstRowWidth, pDefStyle);
//
//    for (int iCol=0; iCol < k_num_cols; iCol++)
//    {
//        pBox = pKeyboardRow->add_inline_box(otherRowsWidth, pDefStyle);
//        m_pColumnLabel[iCol] = pBox->add_text_item(m_sIntvColumnLabel[iCol],
//                                                   pRowStyle);
//    }
//
//    //remaining rows with buttons
//    for (int iRow=0; iRow < k_num_rows; iRow++)
//    {
//        ImoParagraph* pKeyboardRow = m_pDyn->add_paragraph(pRowStyle);
//
//        pBox = pKeyboardRow->add_inline_box(firstRowWidth, pDefStyle);
//        m_pRowLabel[iRow] = pBox->add_text_item(m_sIntvRowLabel[iRow], pRowStyle);
//
//        // the buttons for this row
//        for (int iCol=0; iCol < k_num_cols; iCol++)
//        {
//            iB = iCol + iRow * k_num_cols;    // button index: 0 .. 47
//            pBox = pKeyboardRow->add_inline_box(otherRowsWidth, pDefStyle);
//            m_pAnswerButton[iB] = pBox->add_button(m_sIntvButtonLabel[iB],
//                                                   buttonSize, pBtStyle);
//
//            if (m_sIntvButtonLabel[iB].empty())
//            {
//                m_pAnswerButton[iB]->set_visible(false);
//                m_pAnswerButton[iB]->enable(false);
//            }
//        }
//    }
//
//    set_event_handlers();
//
//    //inform base class about the settings
//    set_buttons(m_pAnswerButton, k_num_buttons);
//}
//
//void IdfyCadencesCtrol::initialize_strings()
//{
//}
//
//void IdfyCadencesCtrol::on_settings_changed()
//{
//    // The settings have been changed. Reconfigure answer keyboard for the new settings
//
//    int iB;     // button index: 0 .. k_num_buttons-1
//
//    //hide all rows and buttons so that later we only have to enable the valid ones
//    for (iB=0; iB < k_num_buttons; iB++) {
//        m_pAnswerButton[iB]->set_visible(false);
//        m_pAnswerButton[iB]->enable(false);
//    }
//
//    //Terminal cadences
//    iB = 0;
//    if (m_pConstrains->IsValidButton(lm_eCadButtonTerminal)) {
//        iB = DisplayButton(iB, lm_eCadTerminal, lm_eCadLastTerminal, _("Terminal"));
//    }
//    //Transient cadences
//    if (m_pConstrains->IsValidButton(lm_eCadButtonTransient)) {
//         iB = DisplayButton(iB, lm_eCadTransient, lm_eCadLastTransient, _("Transient"));
//    }
//
//    //Perfect cadences
//    if (m_pConstrains->IsValidButton(lm_eCadButtonPerfect)) {
//         iB = DisplayButton(iB, lm_eCadPerfect, lm_eCadLastPerfect, _("Perfect"));
//    }
//    //Plagal cadences
//    if (m_pConstrains->IsValidButton(lm_eCadButtonPlagal)) {
//         iB = DisplayButton(iB, lm_eCadPlagal, lm_eCadLastPlagal, _("Plagal"));
//    }
//
//    //Imperfect cadences
//    if (m_pConstrains->IsValidButton(lm_eCadButtonImperfect)) {
//         iB = DisplayButton(iB, lm_eCadImperfect, lm_eCadLastImperfect, _("Imperfect"));
//    }
//
//    //Deceptive cadences
//    if (m_pConstrains->IsValidButton(lm_eCadButtonDeceptive)) {
//         iB = DisplayButton(iB, lm_eCadDeceptive, lm_eCadLastDeceptive, _("Deceptive"));
//    }
//
//    //Half cadences
//    if (m_pConstrains->IsValidButton(lm_eCadButtonHalf)) {
//         iB = DisplayButton(iB, lm_eCadHalf, lm_eCadLastHalf, _("Half cadence"));
//    }
//
//
//    m_pKeyboardSizer->Layout();
//}
//
//int IdfyCadencesCtrol::DisplayButton(int iBt, lmECadenceType iStartC,
//                                       lmECadenceType iEndC, wxString sButtonLabel)
//{
//    // Display a button
//    // iB: number of button to display
//    // iStartC-iEndC: range of cadences associated to this button
//    // sButtonLabel: label for this button
//
//
//    int iB;     // button index: 0 .. k_num_buttons-1
//
//    iB = iBt;
//    m_nStartCadence[iB] = iStartC;
//    m_nEndCadence[iB] = iEndC;
//    m_pAnswerButton[iB]->set_label( sButtonLabel );
//    m_pAnswerButton[iB]->set_visible(true);
//    m_pAnswerButton[iB]->enable(true);
//    iB++;
//    return iB;
//
//}
//
//wxDialog* IdfyCadencesCtrol::get_settings_dialog()
//{
//    wxWindow* pParent = dynamic_cast<wxWindow*>(m_pCanvas);
//    return new DlgCfgIdfyCadence(pParent, m_pConstrains, m_pConstrains->is_theory_mode());
//}
//
//void IdfyCadencesCtrol::prepare_aux_score(int nButton)
//{
//    prepare_score(lmE_Sol, m_nStartCadence[nButton], &m_pAuxScore);
//}
//
//wxString IdfyCadencesCtrol::set_new_problem()
//{
//    //This method must prepare the problem score and set variables:
//    //  m_pProblemScore, m_pSolutionScore, m_sAnswer, m_nRespIndex and m_nPlayMM
//
//    // generate a random cadence
//    lmECadenceType nCadenceType = m_pConstrains->GetRandomCadence();
//
//    // select a key signature
//    RandomGenerator oGenerator;
//    m_nKey = oGenerator.GenerateKey( m_pConstrains->GetKeyConstrains() );
//
//    //create the score
//    EClefExercise nClef = lmE_Sol;
//    if (m_pConstrains->is_theory_mode())
//        m_sAnswer = prepare_score(nClef, nCadenceType, &m_pProblemScore);
//    else
//        m_sAnswer = prepare_score(nClef, nCadenceType, &m_pProblemScore, &m_pSolutionScore);
//
//	// If it was not possible to create the cadence for this key signature, try
//	// again with another cadence
//	int nTimes = 0;
//	while (m_sAnswer == _T("")) {
//		nCadenceType = m_pConstrains->GetRandomCadence();
//        if (m_pConstrains->is_theory_mode())
//            m_sAnswer = prepare_score(nClef, nCadenceType, &m_pProblemScore);
//        else
//            m_sAnswer = prepare_score(nClef, nCadenceType, &m_pProblemScore, &m_pSolutionScore);
//		if (++nTimes == 1000) {
//			wxLogMessage(_T("[IdfyCadencesCtrol::set_new_problem] Loop. Impossible to get a cadence."));
//			break;
//		}
//	}
//
//    //// For debugging and testing. Force to display and use the problem score for the
//    //// solution score; the tonic chord is then visible
//    //if (!m_pConstrains->is_theory_mode()) {
//    //    delete m_pSolutionScore;
//    //    m_pSolutionScore = NULL;
//    //}
//
//
//	//compute the index for the button that corresponds to the right answer
//    if (m_pConstrains->IsValidButton(lm_eCadButtonTerminal)) {
//        //Terminal / transient cadences
//        if (nCadenceType >= lm_eCadTerminal && nCadenceType < lm_eCadLastTerminal)
//            m_nRespIndex = 0;
//        else
//            m_nRespIndex = 1;
//    }
//    else
//    {
//        //Perfect / Plagal cadences
//        m_nRespIndex = -1;      //not set
//        int iB = -1;
//        if (m_pConstrains->IsValidButton(lm_eCadButtonPerfect)) {
//            iB++;
//            if (nCadenceType >= lm_eCadPerfect && nCadenceType < lm_eCadLastPerfect)
//                m_nRespIndex = iB;
//        }
//        //Plagal cadences
//        if (m_nRespIndex == -1 && m_pConstrains->IsValidButton(lm_eCadButtonPlagal)) {
//            iB++;
//            if (nCadenceType >= lm_eCadPlagal && nCadenceType < lm_eCadLastPlagal)
//                m_nRespIndex = iB;
//        }
//
//        //Imperfect cadences
//        if (m_nRespIndex == -1 && m_pConstrains->IsValidButton(lm_eCadButtonImperfect)) {
//            iB++;
//            if (nCadenceType >= lm_eCadImperfect && nCadenceType < lm_eCadLastImperfect)
//                m_nRespIndex = iB;
//        }
//
//        //Deceptive cadences
//        if (m_nRespIndex == -1 && m_pConstrains->IsValidButton(lm_eCadButtonDeceptive)) {
//            iB++;
//            if (nCadenceType >= lm_eCadDeceptive && nCadenceType < lm_eCadLastDeceptive)
//                m_nRespIndex = iB;
//        }
//
//        //Half cadences
//        if (m_nRespIndex == -1 && m_pConstrains->IsValidButton(lm_eCadButtonHalf)) {
//            iB++;
//            if (nCadenceType >= lm_eCadHalf && nCadenceType < lm_eCadLastHalf)
//                m_nRespIndex = iB;
//        }
//    }
//
//    //return string to introduce the problem
//    if (m_pConstrains->is_theory_mode()) {
//        return _("Identify the next cadence:");
//    } else {
//        //ear training
//		wxString sText;
//        if (m_pConstrains->GetKeyDisplayMode() == 0)
//            sText = _("An A4 note will be played before the cadence begins.");
//        else
//            sText = _("A tonic chord will be played before the cadence begins.");
//		sText += _T("\n");
//		sText += _("Press 'Play' to hear the problem again.");
//        return sText;
//    }
//
//}
//
//wxString IdfyCadencesCtrol::prepare_score(EClefExercise nClef, lmECadenceType nType,
//                                           ImoScore** pProblemScore,
//                                           ImoScore** pSolutionScore)
//{
//    //====================================================================================
//    //Example of new code for creating a score
//    static int iNote = 0;
//    static string notes[] = {"(n e4 w)", "(n f4 w)", "(n g4 w)", "(n a4 w)", "(n b4 w)" };
//
//    ImoScore* pScore = static_cast<ImoScore*>(ImFactory::inject(k_imo_score, m_pDoc));
//    ImoInstrument* pInstr = pScore->add_instrument();
//    pInstr->add_clef(k_clef_G2);
//    pInstr->add_object("(n c4 w)");
//    pInstr->add_object( notes[(iNote++)%5] );
//    pInstr->add_object("(barline simple)");
//    //pInstr->add_barline(ImoBarline::k_simple);
//
//    ColStaffObjsBuilder builder;
//    builder.build(pScore);
//    //====================================================================================


//    //create the chords
//    lmCadence oCad;
//    if (!oCad.Create(nType, m_nKey, true)) return _T("");
//
//    //delete the previous score
//    if (*pProblemScore) {
//        delete *pProblemScore;
//        *pProblemScore = NULL;
//    }
//    if (pSolutionScore) {
//        delete *pSolutionScore;
//        *pSolutionScore = NULL;
//    }
//
//    //create a score with the chord
//    wxString sPattern;
//    ImoNote* pNote;
//
//    *pProblemScore = static_cast<ImoScore*>(ImFactory::inject(k_imo_score, m_pDoc));
//    (*pProblemScore)->SetOption(_T("Render.SpacingMethod"), (long)esm_Fixed);
//    ImoInstrument* pInstr = (*pProblemScore)->add_instrument();    // (g_pMidi->DefaultVoiceChannel(),
//							g_pMidi->DefaultVoiceInstr(), _T(""));
//    pVStaff = pInstr->GetVStaff();
//    (*pProblemScore)->SetTopSystemDistance( pVStaff->TenthsToLogical(30, 1) );     // 3 lines
//    pVStaff->AddStaff(5);                       //add second staff: five lines, standard size
//    pInstr->add_clef( lmE_Sol, 1 );
//    pInstr->add_clef( lmE_Fa4, 2 );
//    pInstr->add_key_signature( m_nKey );
//    pInstr->add_time_signature(2 ,4);
//
//    //If ear training add A4/Tonic chord
//    if (!m_pConstrains->is_theory_mode())
//    {
//        //it is ear training exercise
//        if (m_pConstrains->GetKeyDisplayMode() == 0) {
//            // Use A4 note
//            sPattern = "(n =a4 w)";
//            pInstr->add_object( sPattern );
//        }
//        else {
//            // Use tonic chord
//            lmChord* pChord = oCad.GetTonicChord();
//            int nNumNotes = pChord->GetNumNotes();
//            sPattern = "(n " + pChord->GetPattern(0) + " w)";
//            pInstr->add_object(( sPattern );
//            for (int i=1; i < nNumNotes; i++) {
//                sPattern = "(na ";
//                sPattern += pChord->GetPattern(i);
//                sPattern +=  " w)";
//                pInstr->add_object( sPattern );
//            }
//        }
//        pInstr->add_barline(ImoBarline::k_simple);
//
//        pInstr->add_object("(r w)");
//        pInstr->add_barline(ImoBarline::k_simple);
//    }
//
//    // Loop to add chords
//    for (int iC=0; iC < oCad.GetNumChords(); iC++)
//    {
//        pInstr->add_spacer(15);
//        if (iC != 0) pInstr->add_barline(ImoBarline::k_simple);
//        // first and second notes on F4 clef staff
//        sPattern = "(n " + oCad.GetNotePattern(iC, 0) + " w p2)";
//    //wxLogMessage(_T("[IdfyCadencesCtrol::prepare_score] sPattern='%s'"), sPattern.c_str());
//        pInstr->add_object( sPattern );
//        sPattern = "(na " + oCad.GetNotePattern(iC, 1) + " w p2)";
//    //wxLogMessage(_T("[IdfyCadencesCtrol::prepare_score] sPattern='%s'"), sPattern.c_str());
//        pInstr->add_object( sPattern );
//        // third and fourth notes on G clef staff
//        sPattern = "(na " + oCad.GetNotePattern(iC, 2) + " w p1)";
//    //wxLogMessage(_T("[IdfyCadencesCtrol::prepare_score] sPattern='%s'"), sPattern.c_str());
//        pInstr->add_object( sPattern );
//        sPattern = "(na " + oCad.GetNotePattern(iC, 3) + " w p1)";
//    //wxLogMessage(_T("[IdfyCadencesCtrol::prepare_score] sPattern='%s'"), sPattern.c_str());
//        pInstr->add_object( sPattern );
//    }
//    pInstr->add_spacer(20);
//    pInstr->add_barline(ImoBarline::k_end);
//
//    //Prepare Solution Score
//    if (pSolutionScore) {
//        *pSolutionScore = static_cast<ImoScore*>(ImFactory::inject(k_imo_score, m_pDoc));
//        (*pSolutionScore)->SetOption(_T("Render.SpacingMethod"), (long)esm_Fixed);
//        ImoInstrument* pInstr = (*pSolutionScore)->add_instrument();    // (g_pMidi->DefaultVoiceChannel(),
//							    g_pMidi->DefaultVoiceInstr(), _T(""));
//        pVStaff = pInstr->GetVStaff();
//        (*pSolutionScore)->SetTopSystemDistance( pVStaff->TenthsToLogical(30, 1) );     // 3 lines
//        pVStaff->AddStaff(5);                       //add second staff: five lines, standard size
//        pInstr->add_clef( lmE_Sol, 1 );
//        pInstr->add_clef( lmE_Fa4, 2 );
//        pInstr->add_key_signature( m_nKey );
//        pInstr->add_time_signature(2 ,4);
//
//        // Loop to add chords
//        for (int iC=0; iC < oCad.GetNumChords(); iC++)
//        {
//            pInstr->add_spacer(15);
//            if (iC != 0) pInstr->add_barline(ImoBarline::k_simple);
//            // first and second notes on F4 clef staff
//            sPattern = "(n " + oCad.GetNotePattern(iC, 0) + " w p2)";
//        //wxLogMessage(_T("[IdfyCadencesCtrol::prepare_score] sPattern='%s'"), sPattern.c_str());
//            pInstr->add_object( sPattern );
//            sPattern = "(na " + oCad.GetNotePattern(iC, 1) + " w p2)";
//        //wxLogMessage(_T("[IdfyCadencesCtrol::prepare_score] sPattern='%s'"), sPattern.c_str());
//            pInstr->add_object( sPattern );
//            // third and fourth notes on G clef staff
//            sPattern = "(na " + oCad.GetNotePattern(iC, 2) + " w p1)";
//        //wxLogMessage(_T("[IdfyCadencesCtrol::prepare_score] sPattern='%s'"), sPattern.c_str());
//            pInstr->add_object( sPattern );
//            sPattern = "(na " + oCad.GetNotePattern(iC, 3) + " w p1)";
//        //wxLogMessage(_T("[IdfyCadencesCtrol::prepare_score] sPattern='%s'"), sPattern.c_str());
//            pInstr->add_object(( sPattern );
//        }
//        pInstr->add_spacer(20);
//        pInstr->add_barline(ImoBarline::k_end);
//    }
//
//    //return cadence name
//    return  oCad.GetName();
//
//}


}  //namespace lenmus
