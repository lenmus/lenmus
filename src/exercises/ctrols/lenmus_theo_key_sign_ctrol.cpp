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
//#pragma implementation "TheoKeySignCtrol.h"
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
//#include "../score/VStaff.h"
//#include "../score/Instrument.h"
//#include "TheoKeySignCtrol.h"
//#include "lenmus_constrains.h"
//#include "lenmus_generators.h"
//#include "lenmus_score_canvas.h"
//#include "../auxmusic/Conversion.h"
//
////access to error's logger
//#include "../app/Logger.h"
//extern lmLogger* g_pLogger;


namespace lenmus
{

////------------------------------------------------------------------------------------
//// Implementation of TheoKeySignCtrol
//
//
////IDs for controls
//enum {
//    ID_BUTTON = 3010,
//};
//
//BEGIN_EVENT_TABLE(TheoKeySignCtrol, OneScoreCtrol)
//    EVT_COMMAND_RANGE   (ID_BUTTON, ID_BUTTON+m_NUM_BUTTONS-1, wxEVT_COMMAND_BUTTON_CLICKED, lmExerciseCtrol::OnRespButton)
//END_EVENT_TABLE()
//
//IMPLEMENT_CLASS(TheoKeySignCtrol, OneScoreCtrol)
//
//static string m_sMajor[15];
//static string m_sMinor[15];
//
//
//TheoKeySignCtrol::TheoKeySignCtrol(long dynId, ApplicationScope& appScope,
//                                       DocumentCanvas* pCanvas)
//    : OneScoreCtrol(dynId, appScope, pCanvas)
//{
//}
//
////---------------------------------------------------------------------------------------
//void TheoKeySignCtrol::get_ctrol_options_from_params()
//{
//    m_pBaseConstrains = new TheoIntervalsConstrains("TheoIntervals", m_appScope);
//    TheoKeySignCtrolParams builder(m_pBaseConstrains);
//    builder.process_params( m_pDyn->get_params() );
//}
//
////---------------------------------------------------------------------------------------
//void TheoKeySignCtrol::initialize_ctrol()
//{
//    //initializations
//    m_nRespIndex = 0;
//    m_pConstrains = pConstrains;
//
//    //exercise configuration options
//    pConstrains->SetPlayLink(false);        //no play link
//
//    CreateControls();
//    if (m_pConstrains->is_theory_mode()) NewProblem();
//}
//
//void TheoKeySignCtrol::initialize_strings()
//{
//    //language dependent strings. Can not be statically initiallized because
//    //then they do not get translated
//
//    m_sMajor[0] = _("C major");
//    m_sMajor[1] = _("G major");
//    m_sMajor[2] = _("D major");
//    m_sMajor[3] = _("A major");
//    m_sMajor[4] = _("E major");
//    m_sMajor[5] = _("B major");
//    m_sMajor[6] = _("F # major");
//    m_sMajor[7] = _("C # major");
//    m_sMajor[8] = _("C b major");
//    m_sMajor[9] = _("G b major");
//    m_sMajor[10] = _("D b major");
//    m_sMajor[11] = _("A b major");
//    m_sMajor[12] = _("E b major");
//    m_sMajor[13] = _("B b major");
//    m_sMajor[14] = _("F major");
//
//    m_sMinor[0] = _("A minor");
//    m_sMinor[1] = _("E minor");
//    m_sMinor[2] = _("B minor");
//    m_sMinor[3] = _("F # minor");
//    m_sMinor[4] = _("C # minor");
//    m_sMinor[5] = _("G # minor");
//    m_sMinor[6] = _("D # minor");
//    m_sMinor[7] = _("A # minor");
//    m_sMinor[8] = _("A b minor");
//    m_sMinor[9] = _("E b minor");
//    m_sMinor[10] = _("B b minor");
//    m_sMinor[11] = _("F minor");
//    m_sMinor[12] = _("C minor");
//    m_sMinor[13] = _("G minor");
//    m_sMinor[14] = _("D minor");
//
//}
//
//void TheoKeySignCtrol::create_answer_buttons(LUnits height, LUnits spacing)
//{
//    //create 15 buttons for the answers: three rows, five buttons per row
//
//    m_pKeyboardSizer = new wxFlexGridSizer(m_NUM_ROWS, m_NUM_COLS, 2*nSpacing, 0);
//    m_pMainSizer->Add(
//        m_pKeyboardSizer,
//        wxSizerFlags(0).Left().Border(wxALIGN_LEFT|wxTOP, 2*nSpacing) );
//
//    int iB = 0;
//    for (int iRow=0; iRow < m_NUM_ROWS; iRow++)
//    {
//        for (int iCol=0; iCol < m_NUM_COLS; iCol++)
//        {
//            iB = iCol + iRow * m_NUM_COLS;    // button index: 0 .. 24
//            m_pAnswerButton[iB] = new wxButton( this, ID_BUTTON + iB, _T(""),
//                wxDefaultPosition, wxSize(18*nSpacing, nHeight));
//            m_pAnswerButton[iB]->SetFont(font);
//
//            m_pKeyboardSizer->Add(
//                m_pAnswerButton[iB],
//                wxSizerFlags(0).Border(wxALL, nSpacing) );
//        }
//    }
//
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
//TheoKeySignCtrol::~TheoKeySignCtrol()
//{
//}
//
//wxString TheoKeySignCtrol::set_new_problem()
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
//    // choose mode
//    RandomGenerator oGenerator;
//    if (m_pConstrains->GetScaleMode() == eMayorAndMinorModes) {
//        m_fMajorMode = oGenerator.FlipCoin();
//    }
//    else {
//        m_fMajorMode = (m_pConstrains->GetScaleMode() == eMajorMode);
//    }
//
//    // choose key signature and prepare answer
//    bool fFlats = oGenerator.FlipCoin();
//    int nAnswer;
//    int nAccidentals = oGenerator.RandomNumber(0, m_pConstrains->GetMaxAccidentals());
//    EKeySignature nKey;
//    if (m_fMajorMode) {
//        if (fFlats) {
//            // Major mode, flats
//            switch(nAccidentals)
//            {
//                case 0:
//                    nKey = k_key_C;
//                    nAnswer = 0;            // Do Mayor, La menor, no accidentals
//                    m_nRespIndex = 1;
//                    break;
//                case 1:
//                    nKey = k_key_F;
//                    nAnswer = 14;           // Fa Mayor, Re menor"
//                    m_nRespIndex = 7;
//                    break;
//               case 2:
//                    nKey = earmSib;
//                    nAnswer = 13;   // Si b Mayor, Sol menor"
//                    m_nRespIndex = 13;
//                    break;
//                case 3:
//                    nKey = earmMib;
//                    nAnswer = 12;   // Mi b Mayor, Do menor"
//                    m_nRespIndex = 5;
//                    break;
//                case 4:
//                    nKey = earmLab;
//                    nAnswer = 11;   // La b Mayor, Fa menor"
//                    m_nRespIndex = 11;
//                    break;
//                case 5:
//                    nKey = earmReb;
//                    nAnswer = 10;   // Re b Mayor, Si b menor"
//                    m_nRespIndex = 3;
//                    break;
//                case 6:
//                    nKey = earmSolb;
//                    nAnswer = 9;   // Sol b Mayor, Mi b menor"
//                    m_nRespIndex = 9;
//                    break;
//                case 7:
//                    nKey = earmDob;
//                    nAnswer = 8;   // Do b Mayor, La b menor"
//                    m_nRespIndex = 0;
//                    break;
//            }
//        } else {
//            // Major mode, sharps
//            switch(nAccidentals)
//            {
//                case 0:
//                    nKey = k_key_C;
//                    nAnswer = 0;   // Do Mayor, La menor"
//                    m_nRespIndex = 1;
//                    break;
//                case 1:
//                    nKey = earmSol;
//                    nAnswer = 1;   // Sol Mayor, Mi menor"
//                    m_nRespIndex = 10;
//                    break;
//                case 2:
//                    nKey = earmRe;
//                    nAnswer = 2;   // Re Mayor, Si menor"
//                    m_nRespIndex = 4;
//                    break;
//                case 3:
//                    nKey = earmLa;
//                    nAnswer = 3;   // La Mayor, Fa # menor"
//                    m_nRespIndex = 12;
//                    break;
//                case 4:
//                    nKey = earmMi;
//                    nAnswer = 4;   // Mi Mayor, Do # menor"
//                    m_nRespIndex = 6;
//                    break;
//                case 5:
//                    nKey = earmSi;
//                    nAnswer = 5;   // Si Mayor, Sol # menor"
//                    m_nRespIndex = 14;
//                    break;
//                case 6:
//                    nKey = earmFas;
//                    nAnswer = 6;   // Fa # Mayor, Re # menor"
//                    m_nRespIndex = 8;
//                    break;
//                case 7:
//                    nKey = earmDos;
//                    nAnswer = 7;   // Do # Mayor, La # menor"
//                    m_nRespIndex = 2;
//                    break;
//            }
//        }
//    } else {
//        if (fFlats) {
//            // Minor mode, flats
//            switch(nAccidentals)
//            {
//                case 0:
//                    nKey = earmLam;
//                    nAnswer = 0;   // Do Mayor, La menor"
//                    m_nRespIndex = 11;
//                    break;
//                case 1:
//                    nKey = earmRem;
//                    nAnswer = 14;   // Fa Mayor, Re menor"
//                    m_nRespIndex = 2;
//                    break;
//                case 2:
//                    nKey = earmSolm;
//                    nAnswer = 13;   // Si b Mayor, Sol menor"
//                    m_nRespIndex = 8;
//                    break;
//                case 3:
//                    nKey = earmDom;
//                    nAnswer = 12;   // Mi b Mayor, Do menor"
//                    m_nRespIndex = 0;
//                    break;
//                case 4:
//                    nKey = earmFam;
//                    nAnswer = 11;   // La b Mayor, Fa menor"
//                    m_nRespIndex = 6;
//                    break;
//                case 5:
//                    nKey = earmSibm;
//                    nAnswer = 10;   // Re b Mayor, Si b menor"
//                    m_nRespIndex = 13;
//                    break;
//                case 6:
//                    nKey = earmMibm;
//                    nAnswer = 9;   // Sol b Mayor, Mi b menor"
//                    m_nRespIndex = 4;
//                    break;
//                case 7:
//                    nKey = earmLabm;
//                    nAnswer = 8;   // Do b Mayor, La b menor"
//                    m_nRespIndex = 10;
//                    break;
//            }
//        } else {
//            // Minor mode, sharps
//            switch(nAccidentals)
//            {
//                case 0:
//                    nKey = earmLam;
//                    nAnswer = 0;   // Do Mayor, La menor"
//                    m_nRespIndex = 11;
//                    break;
//                case 1:
//                    nKey = earmMim;
//                    nAnswer = 1;   // Sol Mayor, Mi menor"
//                    m_nRespIndex = 5;
//                    break;
//                case 2:
//                    nKey = earmSim;
//                    nAnswer = 2;   // Re Mayor, Si menor"
//                    m_nRespIndex = 14;
//                    break;
//                case 3:
//                    nKey = earmFasm;
//                    nAnswer = 3;   // La Mayor, Fa # menor"
//                    m_nRespIndex = 7;
//                    break;
//                case 4:
//                    nKey = earmDosm;
//                    nAnswer = 4;   // Mi Mayor, Do # menor"
//                    m_nRespIndex = 1;
//                    break;
//                case 5:
//                    nKey = earmSolsm;
//                    nAnswer = 5;   // Si Mayor, Sol # menor"
//                    m_nRespIndex = 9;
//                    break;
//                case 6:
//                    nKey = earmResm;
//                    nAnswer = 6;   // Fa # Mayor, Re # menor"
//                    m_nRespIndex = 3;
//                    break;
//                case 7:
//                    nKey = earmLasm;
//                    nAnswer = 7;   // Do # Mayor, La # menor"
//                    m_nRespIndex = 12;
//                    break;
//            }
//        }
//    }
//
//    // choose type of problem
//    if (m_pConstrains->GetProblemType() == eBothKeySignProblems) {
//        m_fIdentifyKey = oGenerator.FlipCoin();
//    }
//    else {
//        m_fIdentifyKey = (m_pConstrains->GetProblemType() == eIdentifyKeySignature);
//    }
//
//    g_pLogger->LogTrace(_T("TheoKeySignCtrol"),
//        _T("[TheoKeySignCtrol::NewProblem] m_fIdentifyKey=%s, m_fMajorMode=%s, fFlats=%s, nKey=%d, nAnswer=%d, m_nRespIndex=%d"),
//            (m_fIdentifyKey ? _T("yes") : _T("no")),
//            (m_fMajorMode ? _T("yes") : _T("no")),
//            (fFlats ? _T("yes") : _T("no")),
//            nKey, nAnswer, m_nRespIndex);
//
//
//    // store index to right answer button (for guess-number-of-accidentals problems)
//    if (!m_fIdentifyKey) {
//        m_nRespIndex = KeySignatureToNumFifths(nKey);
//        if (m_nRespIndex < 0) m_nRespIndex = 7 - m_nRespIndex;
//    }
//
//    // choose clef
//    EClefExercise nClef = oGenerator.GenerateClef(m_pConstrains->GetClefConstrains());
//
//    // write buttons' labels, depending on mode
//    if (m_fIdentifyKey) {
//        if (m_fMajorMode) {
//            m_pAnswerButton[0]->set_label(_("C flat major"));
//            m_pAnswerButton[1]->set_label(_("C major"));
//            m_pAnswerButton[2]->set_label(_("C sharp major"));
//            m_pAnswerButton[3]->set_label(_("D flat major"));
//            m_pAnswerButton[4]->set_label(_("D major"));
//            m_pAnswerButton[5]->set_label(_("E flat major"));
//            m_pAnswerButton[6]->set_label(_("E major"));
//            m_pAnswerButton[7]->set_label(_("F major"));
//            m_pAnswerButton[8]->set_label(_("F sharp major"));
//            m_pAnswerButton[9]->set_label(_("G flat major"));
//            m_pAnswerButton[10]->set_label(_("G major"));
//            m_pAnswerButton[11]->set_label(_("A flat major"));
//            m_pAnswerButton[12]->set_label(_("A major"));
//            m_pAnswerButton[13]->set_label(_("B flat major"));
//            m_pAnswerButton[14]->set_label(_("B major"));
//            //14,0,7,12,2,10,4,8,6,13,1,11,3,9,5
//        }
//        else {
//            m_pAnswerButton[0]->set_label(_("C minor"));
//            m_pAnswerButton[1]->set_label(_("C sharp minor"));
//            m_pAnswerButton[2]->set_label(_("D minor"));
//            m_pAnswerButton[3]->set_label(_("D sharp minor"));
//            m_pAnswerButton[4]->set_label(_("E flat minor"));
//            m_pAnswerButton[5]->set_label(_("E minor"));
//            m_pAnswerButton[6]->set_label(_("F minor"));
//            m_pAnswerButton[7]->set_label(_("F sharp minor"));
//            m_pAnswerButton[8]->set_label(_("G minor"));
//            m_pAnswerButton[9]->set_label(_("G sharp minor"));
//            m_pAnswerButton[10]->set_label(_("A flat minor"));
//            m_pAnswerButton[11]->set_label(_("A minor"));
//            m_pAnswerButton[12]->set_label(_("A sharp minor"));
//            m_pAnswerButton[13]->set_label(_("B flat minor"));
//            m_pAnswerButton[14]->set_label(_("B minor"));
//        }
//    }
//    else {
//        // type of problem: write key
//        m_pAnswerButton[0]->set_label(_("No accidentals"));
//        m_pAnswerButton[1]->set_label(_("1 #"));
//        m_pAnswerButton[2]->set_label(_("2 #"));
//        m_pAnswerButton[3]->set_label(_("3 #"));
//        m_pAnswerButton[4]->set_label(_("4 #"));
//        m_pAnswerButton[5]->set_label(_("5 #"));
//        m_pAnswerButton[6]->set_label(_("6 #"));
//        m_pAnswerButton[7]->set_label(_("7 #"));
//        m_pAnswerButton[8]->set_label(_("1 b"));
//        m_pAnswerButton[9]->set_label(_("2 b"));
//        m_pAnswerButton[10]->set_label(_("3 b"));
//        m_pAnswerButton[11]->set_label(_("4 b"));
//        m_pAnswerButton[12]->set_label(_("5 b"));
//        m_pAnswerButton[13]->set_label(_("6 b"));
//        m_pAnswerButton[14]->set_label(_("7 b"));
//    }
//
//    //create the score
//    m_pProblemScore = static_cast<ImoScore*>(ImFactory::inject(k_imo_score, m_pDoc));
//    ImoInstrument* pInstr = m_pProblemScore->add_instrument();    // (0,0,_T(""));                   //one vstaff, MIDI channel 0, MIDI instr 0
//    lmVStaff *pVStaff = pInstr->GetVStaff();
//    m_pProblemScore->SetTopSystemDistance( pVStaff->TenthsToLogical(30, 1) );     // 3 lines
//    pInstr->add_clef( nClef );
//    pInstr->add_key_signature(nKey);
//    pInstr->add_barline(ImoBarline::k_end, NO_VISIBLE);
//
//    //wxLogMessage(wxString::Format(
//    //    _T("[TheoKeySignCtrol::NewProblem] m_nRespIndex=%d, oIntv.GetIntervalNum()=%d"),
//    //    m_nRespIndex, oIntv.GetIntervalNum() ));
//
//    if (m_fIdentifyKey)
//    {
//        //direct problem
//        m_sAnswer = m_sMajor[nAnswer] + _T(", ") + m_sMinor[nAnswer];
//        return _T("");
//    }
//    else
//    {
//        //inverse problem
//        m_sAnswer = (m_fMajorMode ? m_sMajor[nAnswer] : m_sMinor[nAnswer] );
//        m_pSolutionScore = m_pProblemScore;
//        m_pProblemScore = (ImoScore*)NULL;
//        return m_sAnswer;
//    }
//
//}
//
//wxDialog* TheoKeySignCtrol::get_settings_dialog()
//{
//    return (wxDialog*)NULL;
//}


}  //namespace lenmus
