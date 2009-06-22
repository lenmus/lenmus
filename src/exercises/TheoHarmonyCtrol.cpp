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
#include "../app/toolbox/ToolNotes.h"
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

#include "../auxmusic/HarmonyExercisesData.h"

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
    //m_pScoreProc = new lmHarmonyProcessor();

    CreateControls();

    //define editor customizations
    m_pEditMode = new lmEditorMode( CLASSINFO(lmScoreCanvas), CLASSINFO(lmHarmonyProcessor) );
    m_pEditMode->ChangeToolPage(lmPAGE_NOTES, CLASSINFO(lmToolPageNotesHarmony) );
    m_pEditMode->SetModeName(_T("TheoHarmonyCtrol"));
    m_pEditMode->SetModeVers(_T("1"));
}

lmTheoHarmonyCtrol::~lmTheoHarmonyCtrol()
{
    //AWARE: As score and EditMode ownership is transferred to the Score Editor window,
    //they MUST NOT be deleted here.
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

    // Carlos  jun-09
    //  Three types of problem
    //   1) fixed bass
    //   2) fixed soprano
    //   3) chord notation

    // select a random key signature
    lmRandomGenerator oGenerator;
    nHarmonyExcerciseType = oGenerator.RandomNumber(1, 2);
    wxString sExerciseDescription;
    wxString sPattern;
    lmNote* pNote;
    lmLDPParser parserLDP(_T("en"), _T("utf-8"));
    lmLDPNode* pNode;
    lmVStaff* pVStaff;
    wxString sExerciseTitle;

    if ( nHarmonyExcerciseType == 1 || nHarmonyExcerciseType == 2 )
    {
        // Prepare a score with that meets the restrictions
        // TODO: VER ESTA TONALIDAD
        m_nKey = oGenerator.GenerateKey( m_pConstrains->GetKeyConstrains() );

        int nNumMeasures = 2;

        sExerciseDescription  =  wxString::Format(
            _T(" Fixed %s; root position. Complete the chord notes.")
            , (nHarmonyExcerciseType == 1? _T("bass"): _T("soprano")) );

        sExerciseTitle = wxString::Format(_T(" Exercise type %d : %s ")
            , nHarmonyExcerciseType, sExerciseDescription.c_str());

        //create a score with a bass line

        //---- Harmonyexercisedata:
        // Exercise 1 checks:
        //   root note
        //   chord type
        // Root notes
        for (int i=0; i < nMAX_E1BCHORDS; i++)
        {
            nExercise1NotesDPitch[i] = 0;
            nExercise1ChordType[i] = ect_Max;
        }

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


        lmFontInfo tNumeralFont = {_T("Times New Roman"), 12, wxFONTSTYLE_NORMAL,
                                    wxFONTWEIGHT_BOLD };
        lmTextStyle* pNumeralStyle = m_pProblemScore->GetStyleName(tNumeralFont);
        wxString sNotes[7]    = {_T("a"), _T("b"), _T("c"), _T("d"), _T("e"), _T("f"), _T("g")};
        // TODO: improve! (calculate numerals from chord info + key signature + mode)
        //        this is provisional; only for key signature = C Major
        wxString sNumeralsDegrees[7] =
        {_T("VI"), _T("VII"), _T("I"), _T("II"), _T("III"), _T("IV"), _T("V"), };
        wxString sNumerals;
        lmEChordType nE1ChordTypes[7] =
         // TODO: MAKE A GENERIC METHOD to get chord type from: root note + key sig
        // example: key-sig: DoM
        //      VI             VII             I             II              III              IV             V        
        {ect_MinorTriad, ect_DimTriad, ect_MajorTriad, ect_MinorTriad, ect_MinorTriad, ect_MajorTriad, ect_MajorTriad};
        // Flag to indicate to the score processor to check exercise

        //loop the add notes
        int nNoteCount = 0;
        int nOctave;
        int nVoice;
        int nFixedNote;
        int nStaff;
        for (int iN=0; iN < (nNumMeasures*2); iN+=2)
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
                nFixedNote = oGenerator.RandomNumber(0, 6);
                if ( nHarmonyExcerciseType == 1 )  // bass
                {
                    nOctave = oGenerator.RandomNumber(2, 3);
                    nVoice = 4;
                    nStaff = 2;
                }
                else if (nHarmonyExcerciseType == 2 )  // soprano
                {
                    nOctave = oGenerator.RandomNumber(3, 4);
                    nVoice = 1;
                    nStaff = 1;
                }
                sPattern = wxString::Format(_T("(n %s%d q p%d v%d (stem down))")
                    , sNotes[nFixedNote].c_str(), nOctave, nStaff, nVoice);
                pNode = parserLDP.ParseText( sPattern );
                pNote = parserLDP.AnalyzeNote(pNode, pVStaff);

                sNumerals = sNumeralsDegrees[nFixedNote];
                lmTextItem* pNumeralText = new lmTextItem(sNumeralsDegrees[nFixedNote], lmHALIGN_DEFAULT, pNumeralStyle);
                pNote->AttachAuxObj(pNumeralText);
                pNumeralText->SetUserLocation(0.0f, 230.0f );

                nExercise1NotesDPitch[nNoteCount] = pNote->GetDPitch();
                nExercise1ChordType[nNoteCount] = nE1ChordTypes[nFixedNote];
                nNoteCount++;

            }
        }
        nHarmonyExercise1ChordsToCheck = nNoteCount; 
    }



    //add final barline
    pVStaff->AddBarline(lm_eBarlineEnd);

    lmFontInfo tTitleFont = {_T("Times New Roman"), 10, wxFONTSTYLE_NORMAL,
                                wxFONTWEIGHT_BOLD };
    lmTextStyle* pTitleStyle = m_pProblemScore->GetStyleName(tTitleFont);
    lmScoreTitle* pTitle = m_pProblemScore->AddTitle(sExerciseTitle, lmHALIGN_CENTER, pTitleStyle);
    lmLocation tTitlePos = g_tDefaultPos;
    pTitle->SetUserLocation(tTitlePos); // only necessary if wanted to be positioned at a specific point


    //set the name and the title of the score
    m_pProblemScore->SetScoreName( sExerciseTitle );
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
