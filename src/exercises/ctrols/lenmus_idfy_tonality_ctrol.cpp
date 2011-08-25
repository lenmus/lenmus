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
//#pragma implementation "IdfyTonalityCtrol.h"
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
//#include "IdfyTonalityCtrol.h"
//#include "lenmus_constrains.h"
//#include "lenmus_generators.h"
//#include "lenmus_score_canvas.h"
//#include "../score/VStaff.h"
//#include "../score/Instrument.h"
//#include "../auxmusic/Conversion.h"
//#include "../score/KeySignature.h"
//#include "../ldp_parser/LDPParser.h"
//#include "../auxmusic/Chord.h"
//#include "../auxmusic/Cadence.h"
//#include "../auxmusic/ScalesManager.h"
//#include "dialogs/DlgCfgIdfyTonality.h"
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
//// Implementation of IdfyTonalityCtrol
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
//BEGIN_EVENT_TABLE(IdfyTonalityCtrol, OneScoreCtrol)
//    EVT_COMMAND_RANGE (ID_BUTTON, ID_BUTTON+m_NUM_BUTTONS-1, wxEVT_COMMAND_BUTTON_CLICKED, IdfyTonalityCtrol::OnRespButton)
//END_EVENT_TABLE()
//
//
//IdfyTonalityCtrol::IdfyTonalityCtrol(long dynId, ApplicationScope& appScope,
//                                       DocumentCanvas* pCanvas)
//    : OneScoreCtrol(dynId, appScope, pCanvas)
//    , m_pConstrains(pConstrains)
//{
//}
//
////---------------------------------------------------------------------------------------
//void IdfyTonalityCtrol::initialize_ctrol()
//{
//    CreateControls();
//}
//
//IdfyTonalityCtrol::~IdfyTonalityCtrol()
//{
//}
//
////---------------------------------------------------------------------------------------
//void IdfyTonalityCtrol::get_ctrol_options_from_params()
//{
//    m_pBaseConstrains = new TheoIntervalsConstrains("TheoIntervals", m_appScope);
//    IdfyTonalityCtrolParams builder(m_pBaseConstrains);
//    builder.process_params( m_pDyn->get_params() );
//}
//
//void IdfyTonalityCtrol::create_answer_buttons(LUnits height, LUnits spacing)
//{
//    //create buttons for the answers, 5 columns, 7 rows
//    int iB = 0;
//    for (iB=0; iB < m_NUM_BUTTONS; iB++)
//        m_pAnswerButton[iB] = (wxButton*)NULL;
//
//    m_pKeyboardSizer = new wxFlexGridSizer(m_NUM_ROWS, m_NUM_COLS+1, 2*nSpacing, 0);
//    m_pMainSizer->Add(
//        m_pKeyboardSizer,
//        wxSizerFlags(0).Left().Border(wxALIGN_LEFT|wxTOP, 2*nSpacing) );
//
//    for (int iRow=0; iRow < m_NUM_ROWS; iRow++)
//    {
//        //row labels
//        m_pRowLabel[iRow] = new wxStaticText(this, -1, _T("  ") );
//        m_pKeyboardSizer->Add(
//            m_pRowLabel[iRow],
//            wxSizerFlags(0).Border(wxLEFT|wxRIGHT, nSpacing) );
//
//        // the buttons for this row
//        for (int iCol=0; iCol < m_NUM_COLS; iCol++)
//        {
//            iB = iCol + iRow * m_NUM_COLS;    // button index
//            if (iB >= m_NUM_BUTTONS) break;
//            m_pAnswerButton[iB] = new wxButton( this, ID_BUTTON + iB, _T("Undefined"),
//                wxDefaultPosition, wxSize(20*nSpacing, nHeight));
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
//void IdfyTonalityCtrol::initialize_strings()
//{
//}
//
//void IdfyTonalityCtrol::on_settings_changed()
//{
//    // The settings have been changed. Reconfigure answer keyboard for the new settings
//
//    Colors* pColors = m_appScope.get_colors();

//    //set buttons
//    int iB = 0;
//    if (m_pConstrains->UseMajorMinorButtons())
//    {
//        //Only major / minor buttons
//        m_pAnswerButton[iB]->set_label( _("Major") );
//        m_pAnswerButton[iB]->set_visible(true);
//        m_pAnswerButton[iB]->enable(true);
//        m_pAnswerButton[iB]->SetBackgroundColour(pColors->Normal());
//        iB++;
//        m_pAnswerButton[iB]->set_label( _("Minor") );
//        m_pAnswerButton[iB]->set_visible(true);
//        m_pAnswerButton[iB]->enable(true);
//        m_pAnswerButton[iB]->SetBackgroundColour(pColors->Normal());
//        iB++;
//    }
//
//    else
//    {
//        EKeySignature nKeys[] = {
//            k_key_C, earmDos, earmDom, earmDosm, earmDob,
//            earmRe, earmReb, earmRem, earmResm, lm_eKeyUndefined,
//            earmMi, earmMib, earmMim, earmMibm, lm_eKeyUndefined,
//            k_key_F, earmFas, earmFam, earmFasm, lm_eKeyUndefined,
//            earmSol, earmSolb, earmSolm, earmSolsm, lm_eKeyUndefined,
//            earmLa, earmLab, earmLam, earmLasm, earmLabm,
//            earmSi, earmSib, earmSim, earmSibm, lm_eKeyUndefined,
//        };
//
//        //use a button for each enabled key signature
//        iB=0;
//        for (unsigned i = 0; i < sizeof(nKeys)/sizeof(EKeySignature); i++, iB++)
//        {
//            EKeySignature nKey = nKeys[i];
//            if (nKey != lm_eKeyUndefined)
//                m_pAnswerButton[iB]->set_label( lmGetKeySignatureName(nKey) );
//            m_nRealKey[iB] = nKey;
//            m_pAnswerButton[iB]->set_visible(nKey != lm_eKeyUndefined);
//            bool fEnable = m_pConstrains->IsValidKey(nKey);
//            m_pAnswerButton[iB]->enable(fEnable);
//            m_pAnswerButton[iB]->SetBackgroundColour(
//                                    fEnable ? pColors->Normal() : *wxWHITE);
//
//        }
//    }
//
//    //hide all other buttons
//    while (iB < m_NUM_BUTTONS)
//    {
//        m_pAnswerButton[iB]->set_visible(false);
//        m_pAnswerButton[iB]->enable(false);
//        iB++;
//    }
//
//    m_pKeyboardSizer->Layout();
//}
//
//void IdfyTonalityCtrol::EnableButtons(bool value)
//{
//}
//
//wxDialog* IdfyTonalityCtrol::get_settings_dialog()
//{
//    wxWindow* pParent = dynamic_cast<wxWindow*>(m_pCanvas);
//    return new DlgCfgIdfyTonality(pParent, m_pConstrains);
//}
//
//void IdfyTonalityCtrol::prepare_aux_score(int nButton)
//{
//    if (m_pConstrains->UseMajorMinorButtons())
//    {
//        //if major/minor buttons do not generate score
//        if (m_pAuxScore)
//            delete m_pAuxScore;
//        m_pAuxScore = (ImoScore*)NULL;
//    }
//    else
//        prepare_score(lmE_Sol, m_nRealKey[nButton], &m_pAuxScore);
//}
//
//wxString IdfyTonalityCtrol::set_new_problem()
//{
//    //This method must prepare the problem score and set variables:
//    //  m_pProblemScore, m_pSolutionScore, m_sAnswer, m_nRespIndex and m_nPlayMM
//
//    // select a key signature
//    RandomGenerator oGenerator;
//    m_nKey = oGenerator.GenerateKey( m_pConstrains->GetKeyConstrains() );
//
//    //create the score
//    EClefExercise nClef = lmE_Sol;
//    m_sAnswer = prepare_score(nClef, m_nKey, &m_pProblemScore);
//
//	//compute the index for the button that corresponds to the right answer
//    ComputeRightAnswerButtons();
//
//    //return string to introduce the problem
//	wxString sText = _("Press 'Play' to hear the problem again.");
//    return sText;
//}
//
//void IdfyTonalityCtrol::ComputeRightAnswerButtons()
//{
//	//compute the index for the button that corresponds to the right answer
//    m_nRespIndex = -1;
//    m_nRespAltIndex = -1;
//    if (m_pConstrains->UseMajorMinorButtons())
//    {
//        m_nRespIndex = (lmIsMajorKey(m_nKey) ? 0 : 1);
//        return;
//    }
//    else
//    {
//        for (int iB=0; iB < m_NUM_BUTTONS; iB++)
//        {
//            if (m_nRealKey[iB] == m_nKey)
//            {
//                m_nRespIndex = iB;
//                break;
//            }
//        }
//    }
//    wxASSERT(m_nRespIndex >=0 && m_nRespIndex < m_NUM_BUTTONS);
//
//    //set alternative (enarmonic) right answer
//    int key;
//    switch(m_nKey)
//    {
//        case earmSi: key = earmDob; break;
//        case earmFas: key = earmSolb; break;
//        case earmDos: key = earmReb; break;
//        case earmSolsm: key = earmLabm; break;
//        case earmResm: key = earmMibm; break;
//        case earmLasm: key = earmSibm; break;
//        case earmDob: key = earmSi; break;
//        case earmSolb: key = earmFas; break;
//        case earmReb: key = earmDos; break;
//        case earmLabm: key = earmSolsm; break;
//        case earmMibm: key = earmResm; break;
//        case earmSibm: key = earmLasm; break;
//        default:
//            return;
//    }
//
//    for (int iB=0; iB < m_NUM_BUTTONS; iB++)
//    {
//        if (m_nRealKey[iB] == key)
//        {
//            m_nRespAltIndex = iB;
//            break;
//        }
//    }
//
//}
//
//wxString IdfyTonalityCtrol::prepare_score(EClefExercise nClef, EKeySignature nKey,
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


//    //delete the previous score
//    if (*pProblemScore) {
//        delete *pProblemScore;
//        *pProblemScore = NULL;
//    }
//
//    //determine tonic note
//    //                           1 1 1 1 1 2 2 2 2 2 3 3 3 3 3 4 4 4 4 4 5 5 5 5 5 6 6 6 6 6
//    //                 0 2 4 6 8 0 2 4 6 8 0 2 4 6 8 0 2 4 6 8 0 2 4 6 8 0 2 4 6 8 0 2 4 6 8
//    string notes = _T("c2d2e2f2g2a2b2c3d3e3f3g3a3b3c4d4e4f4g4a4b4c5d5e5f5g5a5b5c6d6e6f6g6a6b6");
//    int nRoot = lmGetRootNoteStep(nKey)* 2 + 14;  //note in octave 3
//    string note[16];  //4 notes per chord
//
//    RandomGenerator oGenerator;
//    if (oGenerator.FlipCoin())
//    {
//        //Marcelo Galvez. Tónica en los extremos
//        //I
//        note[0] = notes.substr(nRoot, 2);       //I
//        note[1] = notes.substr(nRoot+8, 2);     //V
//        note[2] = notes.substr(nRoot+18, 2);    //III
//        note[3] = notes.substr(nRoot+28, 2);    //I
//        //IV
//        note[4] = notes.substr(nRoot-8, 2);     //IV
//        note[5] = notes.substr(nRoot+10, 2);    //VI
//        note[6] = notes.substr(nRoot+20, 2);    //IV
//        note[7] = notes.substr(nRoot+28, 2);    //I
//        //V
//        note[8] = notes.substr(nRoot-6, 2);     //V
//        note[9] = notes.substr(nRoot+8, 2);     //V
//        note[10] = notes.substr(nRoot+16, 2);   //II
//        note[11] = notes.substr(nRoot+26, 2);   //VII
//    }
//    else
//    {
//        //Marcelo Galvez. Tónica en los extremos. Opción 2
//        //I
//        note[0] = notes.substr(nRoot, 2);       //I
//        note[1] = notes.substr(nRoot+4, 2);     //III
//        note[2] = notes.substr(nRoot+22, 2);    //V
//        note[3] = notes.substr(nRoot+28, 2);    //I
//        //IV
//        note[4] = notes.substr(nRoot-8, 2);     //IV
//        note[5] = notes.substr(nRoot+6, 2);     //IV
//        note[6] = notes.substr(nRoot+24, 2);    //VI
//        note[7] = notes.substr(nRoot+28, 2);    //I
//        //V
//        note[8] = notes.substr(nRoot-6, 2);     //V
//        note[9] = notes.substr(nRoot+2, 2);     //II
//        note[10] = notes.substr(nRoot+22, 2);   //V
//        note[11] = notes.substr(nRoot+26, 2);   //VII
//    }
//    //I
//    note[12] = note[0];     //I
//    note[13] = note[1];     //V
//    note[14] = note[2];     //I
//    note[15] = note[3];     //III
//
////    //Emilio Mesias
////    //I
////    note[0] = notes.substr(nRoot, 2);       //I
////    note[1] = notes.substr(nRoot+8, 2);     //V
////    note[2] = notes.substr(nRoot+14, 2);    //I
////    note[3] = notes.substr(nRoot+18, 2);    //III
////    //IV
////    note[4] = notes.substr(nRoot-8, 2);     //IV
////    note[5] = notes.substr(nRoot, 2);       //I
////    note[6] = notes.substr(nRoot+6, 2);     //IV
////    note[7] = notes.substr(nRoot+10, 2);    //VI
////    //V
////    note[8] = notes.substr(nRoot-6, 2);     //V
////    note[9] = notes.substr(nRoot+2, 2);     //II
////    note[10] = notes.substr(nRoot+8, 2);    //V
////    note[11] = notes.substr(nRoot+12, 2);   //VII
////    //I
////    note[12] = notes.substr(nRoot, 2);       //I
////    note[13] = notes.substr(nRoot+8, 2);     //V
////    note[14] = notes.substr(nRoot+14, 2);    //I
////    note[15] = notes.substr(nRoot+18, 2);    //III
//
//    //create the score
//    string sPattern;
//
//    *pProblemScore = static_cast<ImoScore*>(ImFactory::inject(k_imo_score, m_pDoc));
//    (*pProblemScore)->SetOption(_T("Render.SpacingMethod"), (long)esm_Fixed);
//    ImoInstrument* pInstr = (*pProblemScore)->add_instrument();    // (g_pMidi->DefaultVoiceChannel(),
//							g_pMidi->DefaultVoiceInstr(), _T(""));
//    (*pProblemScore)->SetTopSystemDistance( pVStaff->TenthsToLogical(30, 1) );     // 3 lines
//    pVStaff->AddStaff(5);                       //add second staff: five lines, standard size
//    pInstr->add_clef( lmE_Sol, 1 );
//    pInstr->add_clef( lmE_Fa4, 2 );
//    pInstr->add_key_signature( nKey );
//    pInstr->add_time_signature(2 ,4);
//
//    //add A4 note
//    pInstr->add_object("(n =a4 w p1)");
//    pInstr->add_barline(ImoBarline::k_simple);
//
//    pInstr->add_object("(r w)");
//    pNote = parserLDP.AnalyzeNote(pNode, pVStaff);
//
//    // Loop to add chords
//    int i=0;
//    for (int iC=0; iC < 4; iC++)
//    {
//        pInstr->add_spacer(15);
//        pInstr->add_barline(ImoBarline::k_simple);
//
//        sPattern = "(n " + note[i++] + " w p2)";
//        pInstr->add_object( sPattern );
//
//        sPattern = "(na " + note[i++] + " w p2)";
//        pInstr->add_object( sPattern );
//
//        for (int iN=2; iN < 4; iN++)
//        {
//            sPattern = "(na " + note[i++] + " w p1)";
//            pInstr->add_object(( sPattern );
//        }
//    }
//    pInstr->add_spacer(20);
//    pInstr->add_barline(ImoBarline::k_end);
//
//    //return key signature name
//    return lmGetKeySignatureName(nKey);
//}


}  //namespace lenmus
