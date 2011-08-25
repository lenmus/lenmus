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
//#pragma implementation "IdfyChordCtrol.h"
//#endif
//

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
//#include "../score/VStaff.h"
//#include "../score/Instrument.h"
//#include "IdfyChordCtrol.h"
//#include "lenmus_constrains.h"
//#include "lenmus_generators.h"
//#include "lenmus_score_canvas.h"
//#include "../auxmusic/Conversion.h"
//
//#include "../ldp_parser/LDPParser.h"
//#include "../auxmusic/Interval.h"
//#include "dialogs/DlgCfgIdfyChord.h"
//#include "../auxmusic/Chord.h"
//
//
//#include "lenmus_injectors.h"
//#include "lenmus_colors.h"
//
////access to MIDI manager to get default settings for instrument to use
//#include "../sound/MidiManager.h"


namespace lenmus
{

////------------------------------------------------------------------------------------
//// Implementation of IdfyChordCtrol
//
//
//
//
//static wxString m_sButtonLabel[ect_Max];
//
////IDs for controls
//enum {
//    ID_BUTTON = 3010,
//};
//
//
//BEGIN_EVENT_TABLE(IdfyChordCtrol, OneScoreCtrol)
//    EVT_COMMAND_RANGE (ID_BUTTON, ID_BUTTON+m_NUM_BUTTONS-1, wxEVT_COMMAND_BUTTON_CLICKED, IdfyChordCtrol::OnRespButton)
//END_EVENT_TABLE()
//
//
//IdfyChordCtrol::IdfyChordCtrol(long dynId, ApplicationScope& appScope,
//                                       DocumentCanvas* pCanvas)
//    : OneScoreCtrol(dynId, appScope, pCanvas)
//{
//}
//
////---------------------------------------------------------------------------------------
//void IdfyChordCtrol::initialize_ctrol()
//{
//    //initializations
//    m_pConstrains = pConstrains;
//
//    //allow to play chords
//    m_nKey = k_key_C;
//    m_sRootNote = _T("c4");
//    m_nInversion = 0;
//    m_nMode = m_pConstrains->GetRandomMode();
//
//    CreateControls();
//    if (m_pConstrains->is_theory_mode()) NewProblem();
//}
//
//IdfyChordCtrol::~IdfyChordCtrol()
//{
//}
//
////---------------------------------------------------------------------------------------
//void IdfyChordCtrol::get_ctrol_options_from_params()
//{
//    m_pBaseConstrains = new TheoIntervalsConstrains("TheoIntervals", m_appScope);
//    IdfyChordCtrolParams builder(m_pBaseConstrains);
//    builder.process_params( m_pDyn->get_params() );
//}
//
//void IdfyChordCtrol::initialize_strings()
//{
//    //language dependent strings. Can not be statically initiallized because
//    //then they do not get translated
//
//        //button labels.
//
//    // Triads
//    m_sButtonLabel[ect_MajorTriad] = _("Major ");
//    m_sButtonLabel[ect_MinorTriad] = _("Minor ");
//    m_sButtonLabel[ect_AugTriad] = _("Augmented ");
//    m_sButtonLabel[ect_DimTriad] = _("Diminished ");
//    m_sButtonLabel[ect_Suspended_4th] = _("Suspended (4th)");
//    m_sButtonLabel[ect_Suspended_2nd] = _("Suspended (2nd)");
//
//    // Seventh chords
//    m_sButtonLabel[ect_MajorSeventh] = _("Major 7th");
//    m_sButtonLabel[ect_DominantSeventh] = _("Dominant 7th");
//    m_sButtonLabel[ect_MinorSeventh] = _("Minor 7th");
//    m_sButtonLabel[ect_DimSeventh] = _("Diminished 7th");
//    m_sButtonLabel[ect_HalfDimSeventh] = _("Half dim. 7th");
//    m_sButtonLabel[ect_AugMajorSeventh] = _("Aug. major 7th");
//    m_sButtonLabel[ect_AugSeventh] = _("Augmented 7th");
//    m_sButtonLabel[ect_MinorMajorSeventh] = _("Minor-major 7th");
//
//    // Sixth chords
//    m_sButtonLabel[ect_MajorSixth] = _("Major 6th");
//    m_sButtonLabel[ect_MinorSixth] = _("Minor 6th");
//    m_sButtonLabel[ect_AugSixth] = _("Augmented 6th");
//
//}
//
//void IdfyChordCtrol::create_answer_buttons(LUnits height, LUnits spacing)
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
//        for (int iCol=0; iCol < m_NUM_COLS; iCol++) {
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
//void IdfyChordCtrol::on_settings_changed()
//{
//    //The settings have been changed.
//    //Reconfigure buttons keyboard depending on the chords allowed
//
//    int iC;     // real chord. Correspondence to EChordTypes
//    int iB;     // button index: 0 .. m_NUM_BUTTONS-1
//    int iR;     // row index: 0 .. m_NUM_ROWS-1
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
//    //triads
//    iB = 0;
//    if (m_pConstrains->IsValidGroup(ecg_Triads)) {
//        iR = 0;
//        m_pRowLabel[iR]->set_label(_("Triads:"));
//        for (iC=0; iC <= ect_LastTriad; iC++) {
//            if (m_pConstrains->IsChordValid((lmEChordType)iC)) {
//                m_nRealChord[iB] = iC;
//                m_pAnswerButton[iB]->set_label( m_sButtonLabel[iC] );
//                m_pAnswerButton[iB]->Show(true);
//                m_pAnswerButton[iB]->Enable(true);
//                iB++;
//                if (iB % m_NUM_COLS == 0) {
//                    iR++;
//                    m_pRowLabel[iR]->set_label(_T(""));
//                }
//           }
//        }
//    }
//    if (iB % m_NUM_COLS != 0) iB += (m_NUM_COLS - (iB % m_NUM_COLS));
//
//    //sevenths
//    if (m_pConstrains->IsValidGroup(ecg_Sevenths)) {
//        iR = iB / m_NUM_COLS;
//        m_pRowLabel[iR]->set_label(_("Seventh chords:"));
//        for (iC=ect_LastTriad+1; iC <= ect_LastSeventh; iC++) {
//            if (m_pConstrains->IsChordValid((lmEChordType)iC)) {
//                m_nRealChord[iB] = iC;
//                m_pAnswerButton[iB]->set_label( m_sButtonLabel[iC] );
//                m_pAnswerButton[iB]->Show(true);
//                m_pAnswerButton[iB]->Enable(true);
//                iB++;
//                if (iB % m_NUM_COLS == 0) {
//                    iR++;
//                    m_pRowLabel[iR]->set_label(_T(""));
//                }
//           }
//        }
//    }
//    if (iB % m_NUM_COLS != 0) iB += (m_NUM_COLS - (iB % m_NUM_COLS));
//
//    //Other
//    if (m_pConstrains->IsValidGroup(ecg_Sixths)) {
//        iR = iB / m_NUM_COLS;
//        m_pRowLabel[iR]->set_label(_("Other chords:"));
//        for (iC=ect_LastSeventh+1; iC < ect_MaxInExercises; iC++) {
//            if (m_pConstrains->IsChordValid((lmEChordType)iC)) {
//                m_nRealChord[iB] = iC;
//                m_pAnswerButton[iB]->set_label( m_sButtonLabel[iC] );
//                m_pAnswerButton[iB]->Show(true);
//                m_pAnswerButton[iB]->Enable(true);
//                iB++;
//                if (iB % m_NUM_COLS == 0) {
//                    iR++;
//                    m_pRowLabel[iR]->set_label(_T(""));
//                }
//           }
//        }
//    }
//
//    m_pKeyboardSizer->Layout();
//}
//
//wxDialog* IdfyChordCtrol::get_settings_dialog()
//{
//    wxWindow* pParent = dynamic_cast<wxWindow*>(m_pCanvas);
//    return new DlgCfgIdfyChord(pParent, m_pConstrains, m_pConstrains->is_theory_mode());
//}
//
//void IdfyChordCtrol::prepare_aux_score(int nButton)
//{
//    // No problem is presented and the user press the button to play a specific
//    // sound (chord, interval, scale, etc.)
//    // This method is then invoked to prepare the score with the requested sound.
//    // At return, base class will play it
//
//    prepare_score(lmE_Sol, (lmEChordType)m_nRealChord[nButton], &m_pAuxScore);
//}
//
//wxString IdfyChordCtrol::set_new_problem()
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
//    m_nMode = m_pConstrains->GetRandomMode();
//
//    // select a random key signature
//    RandomGenerator oGenerator;
//    m_nKey = oGenerator.GenerateKey( m_pConstrains->GetKeyConstrains() );
//
//    //Generate a random root note
//    EClefExercise nClef = lmE_Sol;
//    bool fAllowAccidentals = false;
//    m_sRootNote = oGenerator.GenerateRandomRootNote(nClef, m_nKey, fAllowAccidentals);
//
//    // generate a random chord
//    lmEChordType nChordType = m_pConstrains->GetRandomChordType();
//    m_nInversion = 0;
//    if (m_pConstrains->AreInversionsAllowed())
//        m_nInversion = oGenerator.RandomNumber(0, lmNumNotesInChord(nChordType) - 1);
//
//    if (!m_pConstrains->DisplayKey()) m_nKey = k_key_C;
//    m_sAnswer = prepare_score(nClef, nChordType, &m_pProblemScore);
//
//    //compute the index for the button that corresponds to the right answer
//    int i;
//    for (i = 0; i < m_NUM_BUTTONS; i++) {
//        if (m_nRealChord[i] == nChordType) break;
//    }
//    m_nRespIndex = i;
//
//    //return message to introduce the problem
//    if (m_pConstrains->is_theory_mode()) {
//        //theory
//        return _("Identify the next chord:");
//    } else {
//        //ear training
//        return _("Press 'Play' to hear it again");
//    }
//
//}
//
//wxString IdfyChordCtrol::prepare_score(EClefExercise nClef, lmEChordType nType, ImoScore** pScore)
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


//    //create the chord
//    lmChord oChord(m_sRootNote, nType, m_nInversion, m_nKey);
//
//    //wxLogMessage(_T("[IdfyChordCtrol::prepare_score] sRootNote=%s, nType=%d, nInversion=%d, nKey=%d, name='%s'"),
//    //    m_sRootNote.c_str(), nType, m_nInversion, m_nKey, oChord.GetNameFull().c_str() );
//
//    //delete the previous score
//    if (*pScore) {
//        delete *pScore;
//        *pScore = NULL;
//    }
//
//    //create a score with the chord
//    string sPattern;
//
//    int nNumNotes = oChord.GetNumNotes();
//    *pScore = static_cast<ImoScore*>(ImFactory::inject(k_imo_score, m_pDoc));
//    (*pScore)->SetOption(_T("Render.SpacingMethod"), (long)esm_Fixed);
//    ImoInstrument* pInstr = (*pScore)->add_instrument();    // (g_pMidi->DefaultVoiceChannel(),
//							 g_pMidi->DefaultVoiceInstr(), _T(""));
//    (*pScore)->SetTopSystemDistance( pVStaff->TenthsToLogical(30, 1) );     // 3 lines
//    pInstr->add_clef( lmE_Sol );
//    pInstr->add_key_signature( m_nKey );
//    pInstr->add_time_signature(4 ,4, NO_VISIBLE );
//
//    int i = (m_nMode == 2 ? nNumNotes-1 : 0);   // 2= melodic descending
//    sPattern = "(n " + oChord.GetPattern(i) + " w)";
//    pInstr->add_object( sPattern );
//    for (i=1; i < nNumNotes; i++) {
//        sPattern = (m_nMode == 0 ? "(na " : "(n " );     // mode=0 -> harmonic
//        sPattern += oChord.GetPattern((m_nMode == 2 ? nNumNotes-1-i : i));
//        sPattern +=  " w)";
//        pInstr->add_object( sPattern );
//    }
//    pInstr->add_spacer(30);       // 5 lines
//    pInstr->add_barline(ImoBarline::k_end, NO_VISIBLE);
//
//    //(*pScore)->Dump();  //dbg
//
//    //return the chord name
//    if (m_pConstrains->AreInversionsAllowed())
//        return oChord.GetNameFull();       //name including inversion
//    else
//        return oChord.GetName();           //only name
//
//}


}  //namespace lenmus
