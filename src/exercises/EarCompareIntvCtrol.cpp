//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2010 LenMus project
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
//-------------------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "EarCompareIntvCtrol.h"
#endif

// For compilers that support precompilation, includes <wx.h>.
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "../score/VStaff.h"
#include "../score/Instrument.h"
#include "EarCompareIntvCtrol.h"
#include "Constrains.h"
#include "Generators.h"
#include "../auxmusic/Conversion.h"

#include "../ldp_parser/LDPParser.h"
#include "../auxmusic/Interval.h"
#include "dialogs/DlgCfgEarIntervals.h"


#include "../globals/Colors.h"
extern lmColors* g_pColors;

// access to global external variables
extern bool g_fReleaseVersion;          // in TheApp.cpp
extern bool g_fReleaseBehaviour;        // in TheApp.cpp
extern bool g_fShowDebugLinks;          // in TheApp.cpp
extern bool g_fAutoNewProblem;          // in Preferences.cpp

//access to MIDI manager to get default settings for instrument to use
#include "../sound/MidiManager.h"

//------------------------------------------------------------------------------------
// Implementation of lmEarCompareIntvCtrol




IMPLEMENT_CLASS(lmEarCompareIntvCtrol, lmCompareScoresCtrol)

lmEarCompareIntvCtrol::lmEarCompareIntvCtrol(wxWindow* parent, wxWindowID id,
                           lmEarIntervalsConstrains* pConstrains,
                           const wxPoint& pos, const wxSize& size, int style)
    : lmCompareScoresCtrol(parent, id, pConstrains, wxSize(400,150), pos, size, style )
{
    //initializations
    m_pConstrains = pConstrains;

    //options
    m_pConstrains->SetButtonsEnabledAfterSolution(false);
    m_nPlayMM = 320;    //score build with whole notes, so metronome rate 320
                        //will force to play at 80 notes/minute

    CreateControls();

}

lmEarCompareIntvCtrol::~lmEarCompareIntvCtrol()
{
}

wxDialog* lmEarCompareIntvCtrol::GetSettingsDlg()
{
    return new lmDlgCfgEarIntervals(this, m_pConstrains, true);    // true -> enable First note equal checkbox
}

wxString lmEarCompareIntvCtrol::SetNewProblem()
{

    //
    //generate the two intervals to compare
    //

    lmEClefType nClef = lmE_Sol;

    // select interval type: ascending, descending or both
    lmRandomGenerator oGenerator;
    bool fAscending;
    if (m_pConstrains->IsTypeAllowed(0) ||
        (m_pConstrains->IsTypeAllowed(1) && m_pConstrains->IsTypeAllowed(2)))
    {
        // if harmonic scale or melodic ascending and descending,
        // both, ascending and descending, are allowed. Choose one randomly
        fAscending = oGenerator.FlipCoin();
    }
    else if (m_pConstrains->IsTypeAllowed(1)) {
        // if melodic ascendig, allow only ascending intervals
        fAscending = true;
    }
    else {
        // allow only descending intervals
        fAscending = false;
    }
    // select a random key signature satisfying the constraints
    lmEKeySignatures nKey;
    if (m_pConstrains->OnlyNatural()) {
        nKey = oGenerator.GenerateKey(m_pConstrains->GetKeyConstrains());
    }
    else {
        nKey = earmDo;
    }
    // generate the two intervals
    lmInterval oIntv0(m_pConstrains->OnlyNatural(), m_pConstrains->MinNote(),
        m_pConstrains->MaxNote(), m_pConstrains->AllowedIntervals(), fAscending, nKey);

    bool fOnlyNatural;
    int nMidi0;
    if (m_pConstrains->FirstNoteEqual()) {
        fOnlyNatural = false;
        nMidi0 = oIntv0.GetMidiNote1();
    }
    else {
        fOnlyNatural = m_pConstrains->OnlyNatural();
        nMidi0 = 0;
    }
    lmInterval oIntv1(fOnlyNatural, m_pConstrains->MinNote(), m_pConstrains->MaxNote(),
            m_pConstrains->AllowedIntervals(), fAscending, nKey, nMidi0);

    //Convert problem to LDP pattern
    wxString sPattern[2][2];
    int i;
    for (i=0; i < 2; i++) {
        sPattern[0][i] = _T("(n ") + oIntv0.GetPattern(i) + _T(" w)");
        sPattern[1][i] = _T("(n ") + oIntv1.GetPattern(i) + _T(" w)");
    }

    //
    //create two scores, one for each interval to be played, and a third score with
    //both intervals for displaying the solution
    //

    lmNote* pNote[2];
    lmLDPParser parserLDP;
    lmLDPNode* pNode;
    lmVStaff* pVStaff;

    //create the two single-interval scores
    for (i=0; i<2; i++) {
        m_pScore[i] = new_score();
        lmInstrument* pInstr = m_pScore[i]->AddInstrument(
						g_pMidi->DefaultVoiceChannel(), g_pMidi->DefaultVoiceInstr(), _T(""));                     //one vstaff, MIDI channel 0, MIDI instr 0
        pVStaff = pInstr->GetVStaff();
        m_pScore[i]->SetTopSystemDistance( pVStaff->TenthsToLogical(30, 1) );     // 3 lines
        pVStaff->AddClef( nClef );
        pVStaff->AddKeySignature(nKey);
        pVStaff->AddTimeSignature(4 ,4, lmNO_VISIBLE );
    //    pVStaff->AddSpacer(30);       // 3 lines
        pNode = parserLDP.ParseText( sPattern[i][0] );
        pNote[0] = parserLDP.AnalyzeNote(pNode, pVStaff);
        pVStaff->AddBarline(lm_eBarlineSimple, lmNO_VISIBLE);    //so that accidental doesn't affect 2nd note
        pNode = parserLDP.ParseText( sPattern[i][1] );
        pNote[1] = parserLDP.AnalyzeNote(pNode, pVStaff);
        pVStaff->AddBarline(lm_eBarlineEnd, lmNO_VISIBLE);
    }

    //create the answer score with both intervals
    m_pSolutionScore = new_score();
    m_pSolutionScore->SetOption(_T("Render.SpacingMethod"), (long)esm_Fixed);
    lmInstrument* pInstr = m_pSolutionScore->AddInstrument(g_pMidi->DefaultVoiceChannel(),
								 g_pMidi->DefaultVoiceInstr(), _T(""));
    pVStaff = pInstr->GetVStaff();
    m_pSolutionScore->SetTopSystemDistance( pVStaff->TenthsToLogical(30, 1) );     // 3 lines
    pVStaff->AddClef( nClef );
    pVStaff->AddKeySignature(nKey);
    pVStaff->AddTimeSignature(4 ,4, lmNO_VISIBLE );
        //fisrt interval
    pNode = parserLDP.ParseText(_T("(text ''") + oIntv0.GetIntervalName() +
                                _T("'' dy:-40 (font ''Arial'' 6))"));
    parserLDP.AnalyzeText(pNode, pVStaff);
    pNode = parserLDP.ParseText( sPattern[0][0] );
    pNote[0] = parserLDP.AnalyzeNote(pNode, pVStaff);
    pVStaff->AddBarline(lm_eBarlineSimple, lmNO_VISIBLE);    //so that accidental doesn't affect 2nd note
    pNode = parserLDP.ParseText( sPattern[0][1] );
    pNote[1] = parserLDP.AnalyzeNote(pNode, pVStaff);
    pVStaff->AddSpacer(30);       // 3 lines
    pVStaff->AddBarline(lm_eBarlineDouble);
        // two invisible rests to do a pause when playing the score
    pNode = parserLDP.ParseText( _T("(r h noVisible)"));
    parserLDP.AnalyzeNote(pNode, pVStaff);
    pVStaff->AddBarline(lm_eBarlineSimple, lmNO_VISIBLE);
    pNode = parserLDP.ParseText( _T("(r h noVisible)"));
    parserLDP.AnalyzeNote(pNode, pVStaff);
    pVStaff->AddBarline(lm_eBarlineSimple, lmNO_VISIBLE);
        //second interval
    pNode = parserLDP.ParseText(_T("(text ''") + oIntv1.GetIntervalName() +
                                _T("'' dy:-40 (font ''Arial'' 6))"));
    parserLDP.AnalyzeText(pNode, pVStaff);
    pNode = parserLDP.ParseText( sPattern[1][0] );
    parserLDP.AnalyzeNote(pNode, pVStaff);
    pVStaff->AddBarline(lm_eBarlineSimple, lmNO_VISIBLE);    //so that accidental doesn't affect 2nd note
    pNode = parserLDP.ParseText( sPattern[1][1] );
    parserLDP.AnalyzeNote(pNode, pVStaff);
    pVStaff->AddSpacer(30);
    pVStaff->AddBarline(lm_eBarlineEnd);

    //compute the right answer
    m_sAnswer = _T("");
    if (oIntv0.GetNumSemitones() > oIntv1.GetNumSemitones())
        m_nRespIndex = 0;   //First is greater
    else if (oIntv0.GetNumSemitones() == oIntv1.GetNumSemitones())
        m_nRespIndex = 2;   //both are equal
    else
        m_nRespIndex = 1;   //second is greater

    //return message to display to introduce the problem
    return _T("");

}

