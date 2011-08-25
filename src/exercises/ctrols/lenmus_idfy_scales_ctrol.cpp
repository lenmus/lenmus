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
//#pragma implementation "IdfyScalesCtrol.h"
//#endif

////lomse
//#include <lomse_doorway.h>
//#include <lomse_internal_model.h>
//#include <lomse_im_note.h>
//#include <lomse_staffobjs_table.h>
//#include <lomse_im_factory.h>
//using namespace lomse;

//// For compilers that support precompilation, includes <wx.h>.
//#include <wx/wxprec.h>
//
//#ifdef __BORLANDC__
//#pragma hdrstop
//#endif
//
//#include "IdfyScalesCtrol.h"
//
//#include "../score/VStaff.h"
//#include "../score/Instrument.h"
//#include "lenmus_url_aux_ctrol.h"
//#include "lenmus_constrains.h"
//#include "lenmus_generators.h"
//#include "lenmus_score_canvas.h"
//#include "../auxmusic/Conversion.h"
//
//#include "../ldp_parser/LDPParser.h"
//#include "../auxmusic/Interval.h"
//#include "dialogs/DlgCfgIdfyScale.h"
//#include "../auxmusic/ScalesManager.h"
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
//// Implementation of IdfyScalesCtrol
//
//
//
//
//static wxString m_sButtonLabel[est_Max];
//
////IDs for controls
//enum {
//    ID_BUTTON = 3010,
//};
//
//
//BEGIN_EVENT_TABLE(IdfyScalesCtrol, OneScoreCtrol)
//    EVT_COMMAND_RANGE (ID_BUTTON, ID_BUTTON+m_NUM_BUTTONS-1, wxEVT_COMMAND_BUTTON_CLICKED, IdfyScalesCtrol::OnRespButton)
//END_EVENT_TABLE()
//
//
//IdfyScalesCtrol::IdfyScalesCtrol(long dynId, ApplicationScope& appScope,
//                                       DocumentCanvas* pCanvas)
//    : OneScoreCtrol(dynId, appScope, pCanvas)
//{
//}
//
////---------------------------------------------------------------------------------------
//void IdfyScalesCtrol::initialize_ctrol()
//{
//    //initializations
//    m_pConstrains = pConstrains;
//
//    //initializatios to allow to play scales
//    m_nKey = k_key_C;
//    m_sRootNote = _T("c4");
//    m_fAscending = m_pConstrains->GetRandomPlayMode();
//
//    CreateControls();
//    if (m_pConstrains->is_theory_mode()) NewProblem();
//}
//
//IdfyScalesCtrol::~IdfyScalesCtrol()
//{
//}
//
////---------------------------------------------------------------------------------------
//void IdfyScalesCtrol::get_ctrol_options_from_params()
//{
//    m_pBaseConstrains = new TheoIntervalsConstrains("TheoIntervals", m_appScope);
//    IdfyScalesCtrolParams builder(m_pBaseConstrains);
//    builder.process_params( m_pDyn->get_params() );
//}
//
//void IdfyScalesCtrol::create_answer_buttons(LUnits height, LUnits spacing)
//{
//    //create buttons for the answers, two rows
//    int iB = 0;
//    for (iB=0; iB < m_NUM_BUTTONS; iB++)
//        m_pAnswerButton[iB] = (wxButton*)NULL;
//
//    m_pKeyboardSizer = new wxFlexGridSizer(m_NUM_ROWS+1, m_NUM_COLS+1, 2*nSpacing, 0);
//    m_pMainSizer->Add(
//        m_pKeyboardSizer,
//        wxSizerFlags(0).Left().Border(wxALIGN_LEFT|wxTOP, 2*nSpacing) );
//
//    for (int iRow=0; iRow < m_NUM_ROWS; iRow++)
//    {
//        m_pRowLabel[iRow] = new wxStaticText(this, -1, _T(""));
//        m_pRowLabel[iRow]->SetFont(font);
//        m_pKeyboardSizer->Add(
//            m_pRowLabel[iRow],
//            wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, nSpacing) );
//
//        // the buttons for this row
//        for (int iCol=0; iCol < m_NUM_COLS; iCol++)
//        {
//            iB = iCol + iRow * m_NUM_COLS;    // button index
//            if (iB >= m_NUM_BUTTONS) break;
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
//    SetButtons(m_pAnswerButton, m_NUM_BUTTONS, ID_BUTTON);
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
//void IdfyScalesCtrol::initialize_strings()
//{
//
//        //button labels.
//
//    // Major scales
//    m_sButtonLabel[est_MajorNatural] = _("Natural");
//    m_sButtonLabel[est_MajorTypeII] = _("Type II");
//    m_sButtonLabel[est_MajorTypeIII] = _("Type III");
//    m_sButtonLabel[est_MajorTypeIV] = _("Type IV");
//
//    // Minor scales
//    m_sButtonLabel[est_MinorNatural] = _("Natural");
//    m_sButtonLabel[est_MinorDorian] = _("Dorian");
//    m_sButtonLabel[est_MinorHarmonic] = _("Harmonic");
//    m_sButtonLabel[est_MinorMelodic] = _("Melodic");
//
//    // Gregorian modes
//    m_sButtonLabel[est_GreekIonian] = _("Ionian");
//    m_sButtonLabel[est_GreekDorian] = _("Dorian");
//    m_sButtonLabel[est_GreekPhrygian] = _("Phrygian");
//    m_sButtonLabel[est_GreekLydian] = _("Lydian");
//    m_sButtonLabel[est_GreekMixolydian] = _("Mixolydian");
//    m_sButtonLabel[est_GreekAeolian] = _("Aeolian");
//    m_sButtonLabel[est_GreekLocrian] = _("Locrian");
//
//    // Other scales
//    m_sButtonLabel[est_PentatonicMinor] = _("Pentatonic minor");
//    m_sButtonLabel[est_PentatonicMajor] = _("Pentatonic major");
//    m_sButtonLabel[est_Blues] = _("Blues");
//    m_sButtonLabel[est_WholeTones] = _("Whole tones");
//    m_sButtonLabel[est_Chromatic] = _("Chromatic");
//
//}
//
//void IdfyScalesCtrol::on_settings_changed()
//{
//    //Reconfigure buttons keyboard depending on the scales allowed
//
//    int iB;     // button index: 0 .. m_NUM_BUTTONS-1
//
//    //hide all rows and buttons so that later we only have to enable the valid ones
//    for (iB=0; iB < m_NUM_BUTTONS; iB++) {
//        m_pAnswerButton[iB]->Show(false);
//        m_pAnswerButton[iB]->Enable(false);
//    }
//    for (int iRow=0; iRow < m_NUM_ROWS; iRow++) {
//        m_pRowLabel[iRow]->set_label(_T(""));
//    }
//
//    //major scales
//    iB = 0;
//    if (m_pConstrains->IsValidGroup(esg_Major)) {
//        iB = ReconfigureGroup(iB, 0, est_LastMajor, _("Major:"));
//    }
//    //minor scales
//    if (m_pConstrains->IsValidGroup(esg_Minor)) {
//         iB = ReconfigureGroup(iB, est_LastMajor+1, est_LastMinor, _("Minor:"));
//    }
//    //Gregorian modes
//    if (m_pConstrains->IsValidGroup(esg_Gregorian)) {
//         iB = ReconfigureGroup(iB, est_LastMinor+1, est_LastGreek, _("Gregorian modes:"));
//    }
//    //Other scales
//    if (m_pConstrains->IsValidGroup(esg_Other)) {
//         iB = ReconfigureGroup(iB, est_LastGreek+1, est_Max-1, _("Other scales:"));
//    }
//
//    m_pKeyboardSizer->Layout();
//}
//
//int IdfyScalesCtrol::ReconfigureGroup(int iBt, int iStartC, int iEndC, wxString sRowLabel)
//{
//    //Reconfigure a group of buttons
//
//
//    int iC;     // real scale. Correspondence to EScaleTypes
//    int iB;     // button index: 0 .. m_NUM_BUTTONS-1
//    int iR;     // row index: 0 .. m_NUM_ROWS-1
//
//    iB = iBt;
//    iR = iB / m_NUM_COLS;
//    m_pRowLabel[iR]->set_label(sRowLabel);
//    for (iC=iStartC; iC <= iEndC; iC++) {
//        if (m_pConstrains->IsScaleValid((lmEScaleType)iC)) {
//            m_nRealScale[iB] = iC;
//            m_pAnswerButton[iB]->set_label( m_sButtonLabel[iC] );
//            m_pAnswerButton[iB]->Show(true);
//            m_pAnswerButton[iB]->Enable(true);
//            iB++;
//            if (iB % m_NUM_COLS == 0) {
//                iR++;
//                m_pRowLabel[iR]->set_label(_T(""));
//            }
//        }
//    }
//
//    if (iB % m_NUM_COLS != 0) iB += (m_NUM_COLS - (iB % m_NUM_COLS));
//    return iB;
//
//}
//
//wxDialog* IdfyScalesCtrol::get_settings_dialog()
//{
//    wxWindow* pParent = dynamic_cast<wxWindow*>(m_pCanvas);
//    return new DlgCfgIdfyScale(pParent, m_pConstrains, m_pConstrains->is_theory_mode());
//}
//
//void IdfyScalesCtrol::prepare_aux_score(int nButton)
//{
//    prepare_score(lmE_Sol, (lmEScaleType)m_nRealScale[nButton], &m_pAuxScore);
//}
//
//wxString IdfyScalesCtrol::set_new_problem()
//{
//    //This method must prepare the problem score and set variables:
//    //  m_pProblemScore - The score with the problem to propose
//    //  m_pSolutionScore - The score with the solution or NULL if it is the
//    //              same score than the problem score.
//    //  m_sAnswer - the message to present when displaying the solution
//    //  m_nRespIndex - the number of the button for the right answer
//    //  m_nPlayMM - the speed to play the score
//    //
//    //It must return the message to display to introduce the problem.
//
//
//    //select a random mode
//    m_fAscending = m_pConstrains->GetRandomPlayMode();
//
//    // generate a random scale
//    lmEScaleType nScaleType = m_pConstrains->GetRandomScaleType();
//
//    // select a key signature
//    RandomGenerator oGenerator;
//    m_nKey = oGenerator.GenerateKey( m_pConstrains->GetKeyConstrains() );
//
//    // for minor scales use minor key signature and for major scales use a major key
//    if (lmIsMinorScale(nScaleType) && lmIsMajorKey(m_nKey))
//        m_nKey = lmGetRelativeMinorKey(m_nKey);
//    else if (!lmIsMinorScale(nScaleType) && lmIsMinorKey(m_nKey))
//        m_nKey = lmGetRelativeMajorKey(m_nKey);
//
//    //Generate a random root note
//    EClefExercise nClef = lmE_Sol;
//    m_sRootNote = oGenerator.GenerateRandomRootNote(nClef, m_nKey, false);  //false = do not allow accidentals. Only those in the key signature
//
//    //hide key signature if requested or not tonal scale
//    bool fDisplayKey = m_pConstrains->DisplayKey() && IsTonalScale(nScaleType);
//    if (!fDisplayKey)
//        m_nKey = k_key_C;
//
//    //create the score
//    m_sAnswer = prepare_score(nClef, nScaleType, &m_pProblemScore);
//
//    //compute the index for the button that corresponds to the right answer
//    int i;
//    for (i = 0; i < m_NUM_BUTTONS; i++) {
//        if (m_nRealScale[i] == nScaleType) break;
//    }
//    m_nRespIndex = i;
//
//    //if two solutions (minor/major or Gregorian mode) disable answer buttons
//    //for the not valid answer
//    DisableGregorianMajorMinor(nScaleType);
//
//    //return string to introduce the problem
//    if (m_pConstrains->is_theory_mode()) {
//        return _("Identify the next scale:");
//    } else {
//        //ear training
//        return _("Press 'Play' to hear it again");
//    }
//
//}
//
//wxString IdfyScalesCtrol::prepare_score(EClefExercise nClef, lmEScaleType nType, ImoScore** pScore)
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


////    //create the scale object
//    lmScalesManager oScaleMngr(m_sRootNote, nType, m_nKey);
//
//    //dbg------------------------------------------------------
//    g_pLogger->LogTrace(_T("IdfyScalesCtrol"), _T("nClef = %d, nType = %d, m_sRootNote='%s', m_nKey=%d"),
//                    nClef, nType, m_sRootNote.c_str(), m_nKey );
//    //end dbg------------------------------------------------
//
//
//    //delete the previous score
//    if (*pScore) {
//        delete *pScore;
//        *pScore = NULL;
//    }
//
//    //create a score with the scale
//    string sPattern;
//
//    int nNumNotes = oScaleMngr.GetNumNotes();
//    *pScore = static_cast<ImoScore*>(ImFactory::inject(k_imo_score, m_pDoc));
//    (*pScore)->SetOption(_T("Render.SpacingMethod"), (long)esm_Fixed);
//    ImoInstrument* pInstr = (*pScore)->add_instrument();    // (g_pMidi->DefaultVoiceChannel(),
//							 g_pMidi->DefaultVoiceInstr(), _T(""));
//    (*pScore)->SetTopSystemDistance( pVStaff->TenthsToLogical(30, 1) );     // 3 lines
//    pInstr->add_clef( lmE_Sol );
//    pInstr->add_key_signature( m_nKey );
//    pInstr->add_time_signature(4 ,4, NO_VISIBLE );
//
////    pInstr->add_spacer(30);       // 3 lines
//    int i = (m_fAscending ? 0 : nNumNotes-1);
//    sPattern = "(n " + oScaleMngr.GetPattern(i) + " w)";
//    pInstr->add_object( sPattern );
//    pNote = parserLDP.AnalyzeNote(pNode, pVStaff);
//    pInstr->add_spacer(10);       // 1 lines
//    for (i=1; i < nNumNotes; i++) {
//        sPattern = "(n ";
//        sPattern += oScaleMngr.GetPattern((m_fAscending ? i : nNumNotes-1-i));
//        sPattern +=  " w)";
//        pInstr->add_object(( sPattern );
//        pInstr->add_spacer(10);       // 1 lines
//        pInstr->add_barline(ImoBarline::k_simple, NO_VISIBLE);   //so accidentals doesn't affect a 2nd note
//    }
//    pInstr->add_barline(ImoBarline::k_end, NO_VISIBLE);
//
//    //(*pScore)->Dump(_T("IdfyScalesCtrol.prepare_score.ScoreDump.txt"));  //dbg
//
//    //set metronome. As the problem score is built using whole notes, we will
//    //set metronome at MM=400 so the resulting note rate will be 100.
//    m_nPlayMM = 400;
//
//    //return the scale name
//    return oScaleMngr.GetName();
//
//}
//
//void IdfyScalesCtrol::DisableGregorianMajorMinor(lmEScaleType nType)
//{
//    // Gregorian scale Ionian has the same notes than Major natural and
//    // Gregorian scale Aeolian has the same notes than the Minor natural.
//    // When Gregorian answer buttons and maor or minor are enabled there
//    // will be cases in which the answer is nor clear as two anserws are
//    // posible. To solve this, we are going to disable one of the answer
//    // buttons
//
//    if ((m_pConstrains->IsValidGroup(esg_Major) || m_pConstrains->IsValidGroup(esg_Minor)) &&
//         m_pConstrains->IsValidGroup(esg_Gregorian) )
//    {
//        lmEScaleType nDisable;
//        if (nType == est_GreekIonian && m_pConstrains->IsScaleValid(est_MajorNatural))
//        {
//            //disable major natural
//            nDisable = est_MajorNatural;
//        }
//        else if (nType == est_GreekAeolian && m_pConstrains->IsScaleValid(est_MinorNatural))
//        {
//            //disable minor natural
//            nDisable = est_MinorNatural;
//        }
//        else if (nType == est_MajorNatural && m_pConstrains->IsScaleValid(est_GreekIonian))
//        {
//            //disable Gregorian ionian
//            nDisable = est_GreekIonian;
//        }
//        else if (nType == est_MinorNatural && m_pConstrains->IsScaleValid(est_GreekAeolian))
//        {
//            //disable Gregorian aeolian
//            nDisable = est_GreekAeolian;
//        }
//        else
//            return;     //not necessary to disable any button
//
//        //compute the index for the button to disable
//        int iB;
//        for (iB = 0; iB < m_NUM_BUTTONS; iB++) {
//            if (m_nRealScale[iB] == nDisable) break;
//        }
//
//        //disable button iB
//        m_pAnswerButton[iB]->Enable(false);
//
//    }
//
//}


}  //namespace lenmus
