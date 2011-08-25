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
//#pragma implementation "EarCompareIntvCtrol.h"
//#endif
//
//// For compilers that support precompilation, includes <wx.h>.
//#include <wx/wxprec.h>
//
//#ifdef __BORLANDC__
//#pragma hdrstop
//#endif
//
//#include "../score/VStaff.h"
//#include "../score/Instrument.h"
//#include "EarCompareIntvCtrol.h"
//#include "lenmus_constrains.h"
//#include "lenmus_generators.h"
//#include "lenmus_score_canvas.h"
//#include "../auxmusic/Conversion.h"
//
//#include "../ldp_parser/LDPParser.h"
//#include "../auxmusic/Interval.h"
//#include "dialogs/DlgCfgEarIntervals.h"
//
//#include "lenmus_injectors.h"
//#include "lenmus_colors.h"
//
////access to MIDI manager to get default settings for instrument to use
//#include "../sound/MidiManager.h"


namespace lenmus
{

////------------------------------------------------------------------------------------
//// Implementation of EarCompareIntvCtrol
//
//
//
//
//IMPLEMENT_CLASS(EarCompareIntvCtrol, CompareScoresCtrol)
//
//EarCompareIntvCtrol::EarCompareIntvCtrol(wxWindow* parent, wxWindowID id,
//                           EarIntervalsConstrains* pConstrains,
//                           const wxPoint& pos, const wxSize& size, int style)
//    : CompareScoresCtrol(parent, id, pConstrains, wxSize(400,150), pos, size, style )
//{
//    //initializations
//    m_pConstrains = pConstrains;
//
//    //options
//    m_pConstrains->SetButtonsEnabledAfterSolution(false);
//    m_nPlayMM = 320;    //score build with whole notes, so metronome rate 320
//                        //will force to play at 80 notes/minute
//
//    CreateControls();
//
//}
//
//EarCompareIntvCtrol::~EarCompareIntvCtrol()
//{
//}
//
////---------------------------------------------------------------------------------------
//void EarCompareIntvCtrol::get_ctrol_options_from_params()
//{
//    m_pBaseConstrains = new TheoIntervalsConstrains("TheoIntervals", m_appScope);
//    EarCompareIntvCtrolParams builder(m_pBaseConstrains);
//    builder.process_params( m_pDyn->get_params() );
//}
//
//wxDialog* EarCompareIntvCtrol::get_settings_dialog()
//{
//    wxWindow* pParent = dynamic_cast<wxWindow*>(m_pCanvas);
//    return new DlgCfgEarIntervals(pParent, m_pConstrains, true);    // true -> enable First note equal checkbox
//}
//
//wxString EarCompareIntvCtrol::set_new_problem()
//{
//
//    //
//    //generate the two intervals to compare
//    //
//
//    EClefExercise nClef = lmE_Sol;
//
//    // select interval type: ascending, descending or both
//    RandomGenerator oGenerator;
//    bool fAscending;
//    if (m_pConstrains->IsTypeAllowed(0) ||
//        (m_pConstrains->IsTypeAllowed(1) && m_pConstrains->IsTypeAllowed(2)))
//    {
//        // if harmonic scale or melodic ascending and descending,
//        // both, ascending and descending, are allowed. Choose one randomly
//        fAscending = oGenerator.FlipCoin();
//    }
//    else if (m_pConstrains->IsTypeAllowed(1)) {
//        // if melodic ascendig, allow only ascending intervals
//        fAscending = true;
//    }
//    else {
//        // allow only descending intervals
//        fAscending = false;
//    }
//    // select a random key signature satisfying the constraints
//    EKeySignature nKey;
//    if (m_pConstrains->OnlyNatural()) {
//        nKey = oGenerator.GenerateKey(m_pConstrains->GetKeyConstrains());
//    }
//    else {
//        nKey = k_key_C;
//    }
//    // generate the two intervals
//    Interval oIntv0(m_pConstrains->OnlyNatural(), m_pConstrains->MinNote(),
//        m_pConstrains->MaxNote(), m_pConstrains->AllowedIntervals(), fAscending, nKey);
//
//    bool fOnlyNatural;
//    int nMidi0;
//    if (m_pConstrains->FirstNoteEqual()) {
//        fOnlyNatural = false;
//        nMidi0 = oIntv0.GetMidiNote1();
//    }
//    else {
//        fOnlyNatural = m_pConstrains->OnlyNatural();
//        nMidi0 = 0;
//    }
//    Interval oIntv1(fOnlyNatural, m_pConstrains->MinNote(), m_pConstrains->MaxNote(),
//            m_pConstrains->AllowedIntervals(), fAscending, nKey, nMidi0);
//
//    //Convert problem to LDP pattern
//    wxString sPattern[2][2];
//    int i;
//    for (i=0; i < 2; i++) {
//        sPattern[0][i] = _T("(n ") + oIntv0.GetPattern(i) + _T(" w)");
//        sPattern[1][i] = _T("(n ") + oIntv1.GetPattern(i) + _T(" w)");
//    }
//
//    //
//    //create two scores, one for each interval to be played, and a third score with
//    //both intervals for displaying the solution
//    //
//
//    ImoNote* pNote[2];
//    lmLDPParser parserLDP;
//    lmLDPNode* pNode;
//    lmVStaff* pVStaff;
//
//    //create the two single-interval scores
//    for (i=0; i<2; i++) {
//        m_pScore[i] = static_cast<ImoScore*>(ImFactory::inject(k_imo_score, m_pDoc));
//        ImoInstrument* pInstr = m_pScore[i]->add_instrument();    // (
//						g_pMidi->DefaultVoiceChannel(), g_pMidi->DefaultVoiceInstr(), _T(""));                     //one vstaff, MIDI channel 0, MIDI instr 0
//        pVStaff = pInstr->GetVStaff();
//        m_pScore[i]->SetTopSystemDistance( pVStaff->TenthsToLogical(30, 1) );     // 3 lines
//        pInstr->add_clef( nClef );
//        pInstr->add_key_signature(nKey);
//        pInstr->add_time_signature(4 ,4, NO_VISIBLE );
//    //    pInstr->add_spacer(30);       // 3 lines
//        pInstr->add_object(( sPattern[i][0] );
//        pNote[0] = parserLDP.AnalyzeNote(pNode, pVStaff);
//        pInstr->add_barline(ImoBarline::k_simple, NO_VISIBLE);    //so that accidental doesn't affect 2nd note
//        pInstr->add_object(( sPattern[i][1] );
//        pNote[1] = parserLDP.AnalyzeNote(pNode, pVStaff);
//        pInstr->add_barline(ImoBarline::k_end, NO_VISIBLE);
//    }
//
//    //create the answer score with both intervals
//    m_pSolutionScore = static_cast<ImoScore*>(ImFactory::inject(k_imo_score, m_pDoc));
//    m_pSolutionScore->SetOption(_T("Render.SpacingMethod"), (long)esm_Fixed);
//    ImoInstrument* pInstr = m_pSolutionScore->add_instrument();    // (g_pMidi->DefaultVoiceChannel(),
//								 g_pMidi->DefaultVoiceInstr(), _T(""));
//    pVStaff = pInstr->GetVStaff();
//    m_pSolutionScore->SetTopSystemDistance( pVStaff->TenthsToLogical(30, 1) );     // 3 lines
//    pInstr->add_clef( nClef );
//    pInstr->add_key_signature(nKey);
//    pInstr->add_time_signature(4 ,4, NO_VISIBLE );
//        //fisrt interval
//    pInstr->add_object((_T("(text ''") + oIntv0.GetIntervalName() +
//                                _T("'' dy:-40 (font ''Arial'' 6))"));
//    parserLDP.AnalyzeText(pNode, pVStaff);
//    pInstr->add_object(( sPattern[0][0] );
//    pNote[0] = parserLDP.AnalyzeNote(pNode, pVStaff);
//    pInstr->add_barline(ImoBarline::k_simple, NO_VISIBLE);    //so that accidental doesn't affect 2nd note
//    pInstr->add_object(( sPattern[0][1] );
//    pNote[1] = parserLDP.AnalyzeNote(pNode, pVStaff);
//    pInstr->add_spacer(30);       // 3 lines
//    pVStaff->AddBarline(lm_eBarlineDouble);
//        // two invisible rests to do a pause when playing the score
//    pInstr->add_object(( _T("(r h noVisible)"));
//    parserLDP.AnalyzeNote(pNode, pVStaff);
//    pInstr->add_barline(ImoBarline::k_simple, NO_VISIBLE);
//    pInstr->add_object(( _T("(r h noVisible)"));
//    parserLDP.AnalyzeNote(pNode, pVStaff);
//    pInstr->add_barline(ImoBarline::k_simple, NO_VISIBLE);
//        //second interval
//    pInstr->add_object((_T("(text ''") + oIntv1.GetIntervalName() +
//                                _T("'' dy:-40 (font ''Arial'' 6))"));
//    parserLDP.AnalyzeText(pNode, pVStaff);
//    pInstr->add_object(( sPattern[1][0] );
//    parserLDP.AnalyzeNote(pNode, pVStaff);
//    pInstr->add_barline(ImoBarline::k_simple, NO_VISIBLE);    //so that accidental doesn't affect 2nd note
//    pInstr->add_object(( sPattern[1][1] );
//    parserLDP.AnalyzeNote(pNode, pVStaff);
//    pInstr->add_spacer(30);
//    pInstr->add_barline(ImoBarline::k_end);
//
//    //compute the right answer
//    m_sAnswer = _T("");
//    if (oIntv0.GetNumSemitones() > oIntv1.GetNumSemitones())
//        m_nRespIndex = 0;   //First is greater
//    else if (oIntv0.GetNumSemitones() == oIntv1.GetNumSemitones())
//        m_nRespIndex = 2;   //both are equal
//    else
//        m_nRespIndex = 1;   //second is greater
//
//    //return message to display to introduce the problem
//    return _T("");
//
//}


}  //namespace lenmus
