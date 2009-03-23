//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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

#include "../app/Processor.h"
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

#include "../app/toolbox/ToolsBox.h"


class lmEditorMode;

//------------------------------------------------------------------------------------
// Implementation of lmTheoHarmonyCtrol
//------------------------------------------------------------------------------------

//IDs for controls
const int lmID_LINK_SETTINGS = wxNewId();
const int lmID_LINK_GO_BACK = wxNewId();
const int lmID_LINK_NEW_PROBLEM = wxNewId();

IMPLEMENT_CLASS(lmTheoHarmonyCtrol, lmFullEditorExercise)

BEGIN_EVENT_TABLE(lmTheoHarmonyCtrol, lmFullEditorExercise)
    LM_EVT_URL_CLICK    (lmID_LINK_SETTINGS, lmEBookCtrol::OnSettingsButton)
    LM_EVT_URL_CLICK    (lmID_LINK_GO_BACK, lmEBookCtrol::OnGoBackButton)
    LM_EVT_URL_CLICK    (lmID_LINK_NEW_PROBLEM, lmFullEditorExercise::OnNewProblem)
END_EVENT_TABLE()


lmTheoHarmonyCtrol::lmTheoHarmonyCtrol(wxWindow* parent, wxWindowID id,
                            lmHarmonyConstrains* pConstrains, wxSize nDisplaySize, 
                            const wxPoint& pos, const wxSize& size, int style)
    : lmFullEditorExercise(parent, id, pConstrains, pos, size, style )
{
    //initializations
    m_pConstrains = pConstrains;
    m_pProblemScore = (lmScore*)NULL;
    m_pScoreProc = new lmHarmonyProcessor();

    CreateControls();
}

lmTheoHarmonyCtrol::~lmTheoHarmonyCtrol()
{
    if (m_pScoreProc)
        delete m_pScoreProc;

    //AWARE: As score ownership is transferred to the Score Editor window, the
    //problem score MUST NOT be deleted.
}

wxDialog* lmTheoHarmonyCtrol::GetSettingsDlg()
{
    //Returns a pointer to the dialog for customizing the exercise.

    //TODO: Create the dialog class and implement it. The uncomment following code:
    //wxDialog* pDlg = new lmDlgCfgTheoHarmony(this, m_pConstrains, m_pConstrains->IsTheoryMode());
    //return pDlg;
    return (wxDialog*)NULL;
}

void lmTheoHarmonyCtrol::SetNewProblem()
{
    //This method creates a problem score, satisfiying the restrictions imposed
    //by exercise options and user customizations.

    //TODO: all.
    //Following code is just an example. It prepares a score
    //with a fixed bass.

    // select a random key signature
    lmRandomGenerator oGenerator;
    m_nKey = oGenerator.GenerateKey( m_pConstrains->GetKeyConstrains() );

    //create a score with a bass line
    wxString sPattern;
   //TODO: dejar version final o quitar:  provlmNote* pNote; lmLDPNode* pNode;
    lmLDPParser parserLDP(_T("en"), _T("utf-8"));
    lmVStaff* pVStaff;

    m_pProblemScore = new lmScore();
    lmInstrument* pInstr = m_pProblemScore->AddInstrument(
                                g_pMidi->DefaultVoiceChannel(),
							    g_pMidi->DefaultVoiceInstr(), _T(""));

    pVStaff = pInstr->GetVStaff();
    pVStaff->AddStaff(5);               //add second staff: five lines, standard size
    pVStaff->AddClef( lmE_Sol, 1 );     //G clef on first staff
    pVStaff->AddClef( lmE_Fa4, 2 );     //F clef on second staff
    pVStaff->AddKeySignature( m_nKey ); //key signature
    pVStaff->AddTimeSignature(2 ,4);    //2/4 time signature


        wxString sNotes[16] = {
        _T("(n c3 q p2 v4 (stem down))"),
        _T("(n e3 q p2 v4 (stem down))"),
        _T("(n g3 q p2 v4 (stem down))"),
        _T("(n d3 q p2 v4 (stem down))"),
        _T("(n f3 q p2 v4 (stem down))"),
        _T("(n a3 q p2 v4 (stem down))"),
        _T("(n e3 q p2 v4 (stem down))"),
        _T("(n g2 q p2 v4 (stem down))"),
        _T("(n b2 q p2 v4 (stem down))"),
        _T("(n f2 q p2 v4 (stem down))"),
        _T("(n a3 q p2 v4 (stem down))"),
        _T("(n c3 q p2 v4 (stem down))"),
        _T("(n f3 q p2 v4 (stem down))"),
        _T("(n f3 q p2 v4 (stem down))"),
        _T("(n f2 q p2 v4 (stem down))"),
        _T("(n f3 q p2 v4 (stem down))")
    };


/*TODO: dejar version final o quitar  @@@@@@@@@@@@@@@carlos
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
*/
    //loop the add notes
    for (int iN=0; iN < 16; iN+=2)
    {
        //add barline for previous measure
        if (iN != 0)
            pVStaff->AddBarline(lm_eBarlineSimple);
        else
            pVStaff->AddSpacer(20);

/*@@@@@@@@@@@@@@@@@@@@@ TODO: dejar version final o quitar 
        //two chords per measure (time signature is 2 / 4)
        for (int iM=0; iM < 2; iM++)
        {
            //bass note
            sPattern = sNotes[iN + iM];
            pNode = parserLDP.ParseText( sPattern );
            pNote = parserLDP.AnalyzeNote(pNode, pVStaff);
        }
*************/
    }
    //add final barline
    pVStaff->AddBarline(lm_eBarlineEnd);

    //set the name and the title of the score
    m_pProblemScore->SetScoreName(_("Harmony exercise"));
    //m_pProblemScore->AddTitle(_("Harmony exercise"));     //To implement !!
}

void lmTheoHarmonyCtrol::OnSettingsChanged()
{
    //This method is invoked when user clicks on the 'Accept' button in
    //the exercise setting dialog. You receives control just in case
    //you would like to do something (i.e. reconfigure exercise displayed
    //buttons to take into account the new exercise options chosen by the user).
    
    //In this exercise there is no needed to do anything
}

void lmTheoHarmonyCtrol::InitializeStrings()
{
    //This method is invoked only once: at control creation time.
    //Its purpose is to initialize any variables containing strings, so that
    //they are translated to the language chosen by user. Take into account
    //that those strings requiring translation can not be statically initialized,
    //as at compilation time we know nothing about desired language.
    
    //In this exercise there is no needed to translate anything
}
