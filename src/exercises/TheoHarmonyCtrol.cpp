//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 Cecilio Salmeron
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
#pragma implementation "TheoHarmonyCtrol.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "TheoHarmonyCtrol.h"

#include "../score/VStaff.h"
#include "Constrains.h"
#include "Generators.h"
#include "../auxmusic/Conversion.h"

#include "../ldp_parser/LDPParser.h"


#include "../globals/Colors.h"
extern lmColors* g_pColors;

//access to error's logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;

//access to MIDI manager to get default settings for instrument to use
#include "../sound/MidiManager.h"

// access to main frame
#include "../app/MainFrame.h"
extern lmMainFrame* GetMainFrame();


class lmEditorMode;

//------------------------------------------------------------------------------------
// Implementation of lmTheoHarmonyCtrol
//------------------------------------------------------------------------------------

IMPLEMENT_CLASS(lmTheoHarmonyCtrol, lmExerciseCtrol)

BEGIN_EVENT_TABLE(lmTheoHarmonyCtrol, lmExerciseCtrol)

END_EVENT_TABLE()


lmTheoHarmonyCtrol::lmTheoHarmonyCtrol(wxWindow* parent, wxWindowID id,
                            lmHarmonyConstrains* pConstrains, wxSize nDisplaySize, 
                            const wxPoint& pos, const wxSize& size, int style)
    : lmExerciseCtrol(parent, id, pConstrains, nDisplaySize, pos, size, style )
{
    //initializations
    m_pConstrains = pConstrains;
    m_pProblemScore = (lmScore*)NULL;

    SetNewProblem();
    lmMainFrame* pMainFrame = GetMainFrame();
    pMainFrame->NewScoreWindow((lmEditorMode*)NULL, m_pProblemScore);
}

lmTheoHarmonyCtrol::~lmTheoHarmonyCtrol()
{
    //As score ownership is transferred to the Score Editor window, we should
    //do nothing with the problem score.
}

wxDialog* lmTheoHarmonyCtrol::GetSettingsDlg()
{
    //TODO: Create the dialog class and implement it
    //wxDialog* pDlg = new lmDlgCfgTheoHarmony(this, m_pConstrains, m_pConstrains->IsTheoryMode());
    //return pDlg;
    return (wxDialog*)NULL;
}

wxString lmTheoHarmonyCtrol::SetNewProblem()
{
    //This method creates a problem score

    //TODO:
    //Prepare a problem score. The score type will depend on the
    //problem type. Following code is just an example. It prepares a score
    //with a fixed bass.

    // select a random key signature
    lmRandomGenerator oGenerator;
    m_nKey = oGenerator.GenerateKey( m_pConstrains->GetKeyConstrains() );

    //create a score with a bass line
    wxString sPattern;
    lmNote* pNote;
    lmLDPParser parserLDP(_T("en"), _T("utf-8"));
    lmLDPNode* pNode;
    lmVStaff* pVStaff;

    m_pProblemScore = new lmScore();
    lmInstrument* pInstr = m_pProblemScore->AddInstrument(
                                g_pMidi->DefaultVoiceChannel(),
							    g_pMidi->DefaultVoiceInstr(), _T(""));

    pVStaff = pInstr->GetVStaff();
    //m_pProblemScore->SetTopSystemDistance( pVStaff->TenthsToLogical(30, 1) );     // 3 lines
    pVStaff->AddStaff(5);           //add second staff: five lines, standard size
    pVStaff->AddClef( lmE_Sol, 1 );
    pVStaff->AddClef( lmE_Fa4, 2 );
    pVStaff->AddKeySignature( m_nKey );
    pVStaff->AddTimeSignature(2 ,4);

    wxString sNotes[16] = {
        _T("(n c3 q p2 v4 (stem down))"),
        _T("(n e3 q p2 v4 (stem down))"),
        _T("(n f3 q p2 v4 (stem down))"),
        _T("(n a3 q p2 v4 (stem down))"),
        _T("(n g3 q p2 v4 (stem down))"),
        _T("(n e3 q p2 v4 (stem down))"),
        _T("(n c3 q p2 v4 (stem down))"),
        _T("(n b2 q p2 v4 (stem down))"),
        _T("(n g2 q p2 v4 (stem down))"),
        _T("(n a2 q p2 v4 (stem down))"),
        _T("(n c3 q p2 v4 (stem down))"),
        _T("(n e3 q p2 v4 (stem down))"),
        _T("(n e3 q p2 v4 (stem down))"),
        _T("(n d3 q p2 v4 (stem down))"),
        _T("(n g2 q p2 v4 (stem down))"),
        _T("(n c3 q p2 v4 (stem down))")
    };

    //loop the add notes
    for (int iN=0; iN < 16; iN+=2)
    {
        //add barline for previous measure
        if (iN != 0)
            pVStaff->AddBarline(lm_eBarlineSimple);
        else
            pVStaff->AddSpacer(20);

        //two chords per measure (time signature is 2 / 4)
        for (int iM=0; iM < 2; iM++)
        {
            //bass note
            sPattern = sNotes[iN + iM];
            pNode = parserLDP.ParseText( sPattern );
            pNote = parserLDP.AnalyzeNote(pNode, pVStaff);
        }
    }
    //add final barline
    pVStaff->AddBarline(lm_eBarlineEnd);

    //set the name and the title of the score
    m_pProblemScore->SetScoreName(_("Harmony exercise"));
    //m_pProblemScore->AddTitle(_("Harmony exercise"));

    //return text to introduce the problem (not needed)
    return wxEmptyString;
}

void lmTheoHarmonyCtrol::DisplayScoreErrors()
{
    //TODO: This method must check the score and show errors
}

void lmTheoHarmonyCtrol::ClearErrors()
{
    //TODO: This method must remove all error markup from current score
}