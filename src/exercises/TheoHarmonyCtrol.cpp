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

    CreateControls();
}

lmTheoHarmonyCtrol::~lmTheoHarmonyCtrol()
{
    //AWARE: As score and EditMode ownership is transferred to the Score Editor window,
    //they MUST NOT be deleted here.
}

lmEditorMode* lmTheoHarmonyCtrol::CreateEditMode()
{
    //This method is invoked each time a new problem is generated, in order to define
    //editor customizations for the created exercise. Ownership of created lmEditorMode object
    //is transferred to the document and deleted there.

    lmEditorMode* pEditMode = new lmEditorMode( CLASSINFO(lmScoreCanvas), CLASSINFO(lmHarmonyProcessor) );
    pEditMode->ChangeToolPage(lmPAGE_NOTES, CLASSINFO(lmToolPageNotesHarmony) );
    pEditMode->SetModeName(_T("TheoHarmonyCtrol"));
    pEditMode->SetModeVers(_T("1"));

    return pEditMode;
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

    // TODO: Possible exercise options:
    //         exercise 1,2:
    //             inversions allowed
    //             elision allowed
    //          key signature?
    //
    // TODO: Possible improvements:
    //         generalize for chords of N notes
    //         make "number of measures" dependant from the "key signature"?
    //         calculate numerals from chord info + key signature + mode
    //         function to get chord type from: root note + key sig
    //         calculate note note duration from time signature


    //  all-exercises generic data
    const int lmNUM_HARMONY_EXERCISES = 3; 
    int nNumMeasures = 2;
    //  each-exercise specific data
    wxString sExerciseDescription;
    wxString sNote = _T("q"); // todo: calculate note note duration from time signature
    wxString sLDPGoBack = wxString::Format(_T("(musicData (goBack %s) )"), sNote); 

    // Carlos  jun-09
    //  Three types of problem
    //   1) fixed bass
    //   2) fixed soprano
    //   3) chord notation

    // select a random key signature
    lmRandomGenerator oGenerator;
    nHarmonyExcerciseType = oGenerator.RandomNumber(1, lmNUM_HARMONY_EXERCISES);
    wxString sPattern;
    lmLDPParser parserLDP(_T("en"), _T("utf-8"));
    lmLDPNode* pNode;
    lmVStaff* pVStaff;
    lmNote* pNoteToAttach = NULL;
    wxString sExerciseTitle;
    lmFPitch nIntvB[3] = {0, 0, 0};

    if ( nHarmonyExcerciseType >= 1 && nHarmonyExcerciseType <= lmNUM_HARMONY_EXERCISES )
    {
        // Prepare a score that meets the restrictions

        m_nKey = oGenerator.GenerateKey( m_pConstrains->GetKeyConstrains() );

        if (nHarmonyExcerciseType == 3)
            sExerciseDescription  =  wxString::Format(_(" Cipher the chords"));
        else
            sExerciseDescription  =  wxString::Format(
                _(" Fixed %s; root position. Complete the chord notes.")
                , (nHarmonyExcerciseType == 1? _("bass"): _("soprano")) );

        sExerciseTitle = wxString::Format(_(" Exercise type %d : %s ")
            , nHarmonyExcerciseType, sExerciseDescription.c_str());

        //create a score with a bass line

        for (int i=0; i < nMAX_HARMONY_EXERCISE_CHORDS; i++)
        {
            for (int v=0; v < nNUM_VOICES_IN_HARMONY_EXERCISE; v++)
            {
                nHE_NotesFPitch[v][i] = 0;
                pHE_Notes[v][i] = NULL;
            }
            gnHE_InversionsInChord[i] = 0;
            nHE_ChordType[i] = ect_Max;
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


        lmFontInfo tNumeralFont = {_T("Times New Roman"), 11, wxFONTSTYLE_NORMAL,
                                    wxFONTWEIGHT_BOLD };
        lmTextStyle* pNumeralStyle = m_pProblemScore->GetStyleName(tNumeralFont);
        wxString sNotes[7]    = {_("c"), _("d"), _("e"), _("f"), _("g"), _("a"), _("b")};
        // TODO: improve! (calculate numerals from chord info + key signature + mode)
        //        this is provisional; only for key signature = C Major
        wxString sNumeralsDegrees[7] =
        {_T("I"), _T("II"), _T("III"), _T("IV"), _T("V"), _T("VI"), _T("VII")};
        wxString sNumerals;
        lmEChordType nE1ChordTypes[7] =
         // TODO: MAKE A GENERIC METHOD to get chord type from: root note + key sig
        // example: if key-sig == DoM return this:
        //      I             II              III              IV             V             VI             VII
        {ect_MajorTriad, ect_MinorTriad, ect_MinorTriad, ect_MajorTriad, ect_MajorTriad, ect_MinorTriad, ect_DimTriad, };
        // For exercise 2, given a numeral (bass note; chord in root position) : calculate soprano pitch. No inversions
        //  First index: interval (2 intervals for triad chords)
        //  Second index: degree 
        // TODO: USE A FUNCTION INSTEAD OF THIS ARRAY !!!!
        lmFPitch nIntervalFromBass[3][7] =  {
        //      I             II              III              IV             V             VI             VII
        //{ect_MajorTriad, ect_MinorTriad, ect_MinorTriad, ect_MajorTriad, ect_MajorTriad, ect_MinorTriad, ect_DimTriad, };
          {   0,            0,             0,                0,               0,             0,              0},     // +0 intervals
          {   lm_M3,        lm_m3,          lm_m3,          lm_M3,          lm_M3,         lm_m3,         lm_m3},    // +1 intervals
          {   lm_p5,        lm_p5,          lm_p5,          lm_p5,          lm_p5,         lm_p5,         lm_d5} };  // +2 intervals


        //loop the add notes
        int nChordCount = 0;
        int nOctave;
        int nVoice;
        int nBassNoteStep;
        int nStaff;
        int nAccidentals;
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
                // Process for each chord:
                //
                //   Generate the chord
                //      generate the bass note
                //      generate the rest of notes
                //   Calculate other properties: numerals, chord type, figured bass
                //   Display
                //       Exercise 1: bass note and numeral
                //       Exercise 2: soprano note and numeral
                //       Exercise 3: all notes and numeral
                //

                //
                // Create the chords
                //
                for (nVoice = 1; nVoice<=nNUM_VOICES_IN_HARMONY_EXERCISE; nVoice++)
                {
                    if (nVoice == 1)
                    {
                        // Bass note: generate it
                        nOctave = oGenerator.RandomNumber(2, 3);  
                        // this is done to make the notes appear more centered in the bass staff
                        if (nOctave == 3 ) // octave 3 : notes c,d,e
                           nBassNoteStep = oGenerator.RandomNumber(0, 2);
                        else // octave 2 : notes f,g,a,b
                           nBassNoteStep = oGenerator.RandomNumber(3, 6); 
                        nAccidentals = 0;

                        //  Calculate pitch of bass note and store bass it for later check
                        nHE_NotesFPitch[nChordCount][0] = FPitch(nBassNoteStep, nOctave, 0);

                        wxLogMessage(_T(" Chord %d, V%d, step:%d octave:%d: %d (%s)")
                            , nChordCount
                            , nVoice
                            , nBassNoteStep, nOctave
                            , nHE_NotesFPitch[nChordCount][0]
                            , FPitch_ToAbsLDPName(nHE_NotesFPitch[nChordCount][0]).c_str());

                        //   Get numerals from bass note
                        sNumerals = sNumeralsDegrees[nBassNoteStep];

                        // Specific part for all the voices 2..4: interval
                        // Calculate the 3 possible intervals from the bass : 
                        //   3 different possibilities (0, interval 1, interval 2); no repetition:
                        //     calculate permutations of 0,1,2
                        nIntvB[0] = oGenerator.RandomNumber(0, 2);
                        nIntvB[1] = (nIntvB[0] + oGenerator.RandomNumber(1, 2)) % 3 ;
                        nIntvB[2] = 3 - (nIntvB[0] + nIntvB[1] ) ;
                        wxLogMessage(_T(" nIntvB 0:%d 1:%d 2:%d")
                            , nIntvB[0], nIntvB[1], nIntvB[2]);

                    }
                    else
                    {
                        //
                        // Calculate the rest of voices (tenor, baritone, soprano)
                        //
                        //  There are three possible values for each voice:
                        //    1: bass + N octaves
                        //    2: bass + N octaves + 1st interval
                        //    3: bass + N octaves + 2nd interval
                        //
                        // Strategy:
                        //   1) for each of the 3 voices: Add N octaves 
                        //   2) for each of the 3 voices: Add 0, first or second interval, but
                        //        do not repeat the same interval in different voices
                        // 
                        //   todo: Consider to improve this by allowing elisions.
                        //           This might be an option of the exercise.
                        //

                        // Common part for all the voices 2..4: bass + N octaves
                        // To calculate N, this simple rule may be enough
                        //  tenor: 0,1
                        //  baritone: 1
                        //  soprano: 2
                        int nNumOctavesToAdd = nVoice-2;
                        nHE_NotesFPitch[nChordCount][nVoice-1] = 
                            nHE_NotesFPitch[nChordCount][0] + (lm_p8 * nNumOctavesToAdd);

                        wxLogMessage(_T(" Chord %d, V%d, before applying intv %d (%d): %d (%s)")
                            , nChordCount
                            , nVoice
                            , nIntvB[nVoice-2]
                            , nIntervalFromBass[nIntvB[nVoice-2]][nBassNoteStep]
                            , nHE_NotesFPitch[nChordCount][nVoice-1]
                            , FPitch_ToAbsLDPName(nHE_NotesFPitch[nChordCount][nVoice-1]).c_str());

                        // Specific part for all the voices 2..4: add an interval (different in each voice!!!!)
                        // At this point nIntvB stores a permutation of 0,1,2 such as 2,0,1
                        // For the current voice: add the interval nIntvB[nVoice]
                        //   more precisely: nIntervalFromBass[nIntvB[nVoice-1]][nBassNoteStep]
                        // Apply the calculated intervals
                        nHE_NotesFPitch[nChordCount][nVoice-1] += nIntervalFromBass[nIntvB[nVoice-2]][nBassNoteStep]; 

                        if (nHE_NotesFPitch[nChordCount][nVoice-1] == nHE_NotesFPitch[nChordCount][nVoice-2])
                        {
                            wxLogMessage(_T(" V%d added octave to avoid same note"));
                            nHE_NotesFPitch[nChordCount][nVoice-1] += lm_p8;
                        }
                        wxLogMessage(_T(" Chord %d, V%d, AFTER applying intv %d (%d): %d (%s)")
                            , nChordCount
                            , nVoice
                            , nIntvB[nVoice-2]
                            , nIntervalFromBass[nIntvB[nVoice-2]][nBassNoteStep]
                            , nHE_NotesFPitch[nChordCount][nVoice-1]
                            , FPitch_ToAbsLDPName(nHE_NotesFPitch[nChordCount][nVoice-1]).c_str());
                    }
                }

                // At this point we have the pitch of each voice

                //
                // Apply inversions
                //
                //  Exercise 3: calculate random inversions
                //  other exercises: todo:  inversions as an option
                if (nHarmonyExcerciseType == 3 )
                {
                    // Calculate a random number of inversions and apply them
                    gnHE_InversionsInChord[nChordCount] = oGenerator.RandomNumber(0, 2);
                }
                // for each inversion...
                for (int nInv = 0; nInv < gnHE_InversionsInChord[nChordCount]; nInv++)
                {
                    // move down all the voices except the bass
                    for (int nV=0; nV<nNUM_VOICES_IN_HARMONY_EXERCISE-1; nV++)
                    {
                        nHE_NotesFPitch[nChordCount][nV] = nHE_NotesFPitch[nChordCount][nV+1];
                    }
                    // increase one octave the bass
                    nHE_NotesFPitch[nChordCount][nNUM_VOICES_IN_HARMONY_EXERCISE] += lm_p8;
                    wxLogMessage(_T("  AFTER %d of %d INVERSIONS, Chord Notes: %d %s, %d %s, %d %s, %d %s")
                       , nInv
                       , gnHE_InversionsInChord[nChordCount]
                       , nHE_NotesFPitch[nChordCount][0]
                       , FPitch_ToAbsLDPName(nHE_NotesFPitch[nChordCount][0]).c_str()
                       , nHE_NotesFPitch[nChordCount][1]
                       , FPitch_ToAbsLDPName(nHE_NotesFPitch[nChordCount][1]).c_str()
                       , nHE_NotesFPitch[nChordCount][2]
                       , FPitch_ToAbsLDPName(nHE_NotesFPitch[nChordCount][2]).c_str()
                       , nHE_NotesFPitch[nChordCount][3]
                       , FPitch_ToAbsLDPName(nHE_NotesFPitch[nChordCount][3]).c_str()
                           );
                }

                //
                // Create each lmNote
                //
                wxString sUpDown[2] = { _T("down"), _T("up")};
// TODO: ACLARAR  ¿V1 EN P1 O EN P2? 
//                for (nStaff=1; nStaff<3; nStaff++)  // V1 en P1
//                for (nStaff=2; nStaff>0; nStaff--)  // V1 en P2              
                nVoice=1;
                for (nStaff=2; nStaff>0; nStaff--)
                {
                    for (int nVoiceInStaff=1; nVoiceInStaff<3; nVoiceInStaff++,nVoice++)
                    {
                        wxLogMessage(_T("  Staff %d, V%d "), nStaff, nVoice);
//todo:remove                        nVoice = ((nStaff-1)*2)  + nVoiceInStaff;
                        assert(nVoice >= 1 && nVoice <= 4);

                        sHE_Notes[nChordCount][nVoice-1] = wxString::Format(_T("%s")
                           , FPitch_ToAbsLDPName(nHE_NotesFPitch[nChordCount][nVoice-1]).c_str());

                        sHE_Pattern[nChordCount][nVoice-1] = wxString::Format(_T("(n %s %s p%d v%d (stem %s))")
                           , sHE_Notes[nChordCount][nVoice-1].c_str()
                           , sNote
                           , nStaff, nVoice, sUpDown[nVoiceInStaff-1]);

                        wxLogMessage(_T("   V%d %d %s, pattern: %s")
                              , nVoice
                              , nHE_NotesFPitch[nChordCount][nVoice-1]
                              , sHE_Notes[nChordCount][nVoice-1].c_str()
                              , sHE_Pattern[nChordCount][nVoice-1].c_str()
                              );


                        // Display the notes in the score
                        //  Exercise 1: only bass (voice 1)
                        //  Exercise 2: only soprano (voice 4)
                        //  Exercise 3: all
                        if (    (nHarmonyExcerciseType == 1 && nVoice == 1 ) ||
                                (nHarmonyExcerciseType == 2 && nVoice == 4 ) ||
                                 nHarmonyExcerciseType == 3
                            )
                        {
                            pNode = parserLDP.ParseText(  sHE_Pattern[nChordCount][nVoice-1] );
                            pHE_Notes[nChordCount][nVoice-1] = parserLDP.AnalyzeNote(pNode, pVStaff);
                            pNoteToAttach = pHE_Notes[nChordCount][nVoice-1];
                            wxLogMessage(_T("   (ExType:%d V:%d) added pattern: %s")
                              ,nHarmonyExcerciseType, nVoice, sHE_Pattern[nChordCount][nVoice-1]);
                        }
                        else
                            wxLogMessage(_T("   ExType:%d V:%d NOT added pattern: %s")
                              ,nHarmonyExcerciseType, nVoice, sHE_Pattern[nChordCount][nVoice-1]);



                        // Go back:
                        // Exercise 1, 2: Never (only one note per chord)
                        // Exercise 3: go back after voices 1,2,3
                        if ( nHarmonyExcerciseType == 3 && (nVoice >=1 &&  nVoice <=3))
                        {
                            pNode = parserLDP.ParseText( sLDPGoBack );
                            parserLDP.AnalyzeMusicData(pNode, pVStaff);
                            wxLogMessage(_T("  AnalyzeMusicData: %s"),sLDPGoBack) ;
                        }
                    }

                }

                //    Display the numeral
                lmTextItem* pNumeralText = new lmTextItem(
                    pNoteToAttach, lmNEW_ID, sNumeralsDegrees[nBassNoteStep],
                    lmHALIGN_DEFAULT, pNumeralStyle);
                pNoteToAttach->AttachAuxObj(pNumeralText);
                pNumeralText->SetUserLocation(0.0f, 230.0f );


                //
                // Create the chord
                //
                for (int nV=0; nV<nNUM_VOICES_IN_HARMONY_EXERCISE; nV++)
                    sHE_Notes[nChordCount][nV] =  FPitch_ToAbsLDPName(nHE_NotesFPitch[nChordCount][nV]);

                wxLogMessage(_T("  Chord Notes: %d %s, %d %s, %d %s, %d %s, Inversions: %d")
                   , nHE_NotesFPitch[nChordCount][0]
                   , FPitch_ToAbsLDPName(nHE_NotesFPitch[nChordCount][0]).c_str()
                   , nHE_NotesFPitch[nChordCount][1]
                   , FPitch_ToAbsLDPName(nHE_NotesFPitch[nChordCount][1]).c_str()
                   , nHE_NotesFPitch[nChordCount][2]
                   , FPitch_ToAbsLDPName(nHE_NotesFPitch[nChordCount][2]).c_str()
                   , nHE_NotesFPitch[nChordCount][3]
                   , FPitch_ToAbsLDPName(nHE_NotesFPitch[nChordCount][3]).c_str()
                   , gnHE_InversionsInChord[nChordCount]
                       );

                pHE_Chords[nChordCount] = new lmChord(nNUM_VOICES_IN_HARMONY_EXERCISE, &sHE_Notes[nChordCount][0], m_nKey);
                nHE_ChordType[nChordCount] = pHE_Chords[nChordCount]->GetChordType();

                wxLogMessage(_T("  Chord %d: %s"), nChordCount, pHE_Chords[nChordCount]->ToString());

                // Calculate the figured bass
                //
                //  build a chord from a list of notes in LDP source code
                //    lmChord(int nNumNotes, wxString* pNotes, lmEKeySignatures nKey = earmDo);
                pHE_FiguredBass[nChordCount] = new lmFiguredBass(pVStaff, lmNEW_ID
                    , pHE_Chords[nChordCount], m_nKey);

                wxLogMessage(_T("Exercise %d, Chord %d, FIGURED BASS:%s")
                 , nHarmonyExcerciseType , nChordCount, pHE_FiguredBass[nChordCount]->GetFiguredBassString().c_str());

            nChordCount++;
            }
        }
        nHarmonyExerciseChordsToCheck = nChordCount;
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
