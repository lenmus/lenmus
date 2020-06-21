//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2015 LenMus project
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
//#pragma implementation "TheoHarmonyCtrol.h"
//#endif
//
////wxWidgets
//#include <wx/wxprec.h>
//
//#ifdef __BORLANDC__
//#pragma hdrstop
//#endif
//
//#include "TheoHarmonyCtrol.h"
//
//#include "../app/Processor.h"
//#include "../app/toolbox/ToolNotes.h"
//#include "../score/VStaff.h"
//#include "../score/Instrument.h"
//#include "lenmus_constrains.h"
//#include "lenmus_generators.h"
//#include "lenmus_document_canvas.h"
//#include "../auxmusic/Conversion.h"
//
//#include "../ldp_parser/LDPParser.h"
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
//
//// access to main frame
//#include "../app/MainFrame.h"
//extern lmMainFrame* GetMainFrame();
//
//#include "../app/toolbox/ToolsBox.h"
//
//#include "../auxmusic/HarmonyExercisesData.h"


//lomse
#include <lomse_doorway.h>
#include <lomse_internal_model.h>
#include <lomse_im_note.h>
#include <lomse_staffobjs_table.h>
#include <lomse_im_factory.h>
using namespace lomse;


namespace lenmus
{

//class lmEditorMode;
//
////------------------------------------------------------------------------------------
//// Implementation of TheoHarmonyCtrol
////------------------------------------------------------------------------------------
//
////IDs for controls
//const int lmID_LINK_SETTINGS = wxNewId();
//const int lmID_LINK_GO_BACK = wxNewId();
//const int lmID_LINK_NEW_PROBLEM = wxNewId();
//
//IMPLEMENT_CLASS(TheoHarmonyCtrol, lmFullEditorExercise)
//
//wxBEGIN_EVENT_TABLE(TheoHarmonyCtrol, lmFullEditorExercise)
//    LM_EVT_URL_CLICK    (lmID_LINK_SETTINGS, lmEBookCtrol::OnSettingsButton)
//    LM_EVT_URL_CLICK    (lmID_LINK_GO_BACK, lmEBookCtrol::OnGoBackButton)
//    LM_EVT_URL_CLICK    (lmID_LINK_NEW_PROBLEM, lmFullEditorExercise::OnNewProblem)
//wxEND_EVENT_TABLE()
//
//
//TheoHarmonyCtrol::TheoHarmonyCtrol(wxWindow* parent, wxWindowID id,
//                            lmHarmonyConstrains* pConstrains, wxSize nDisplaySize,
//                            const wxPoint& pos, const wxSize& size, int style)
//    : lmFullEditorExercise(parent, id, pConstrains, pos, size, style )
//{
//    //initializations
//    m_pConstrains = pConstrains;
//    m_pConstrains = dynamic_cast<....Constrains*>(m_pBaseConstrains);
//    m_pProblemScore = (ImoScore*)nullptr;
//
//    create_controls();
//}
//
//TheoHarmonyCtrol::~TheoHarmonyCtrol()
//{
//    //AWARE: As score and EditMode ownership is transferred to the Score Editor window,
//    //they MUST NOT be deleted here.
//}

//////---------------------------------------------------------------------------------------
////void EarCompareIntvCtrol::initialize_ctrol()
////{
////}

////---------------------------------------------------------------------------------------
//void TheoHarmonyCtrol::get_ctrol_options_from_params()
//{
////    //TODO: reemplazar el tipo de constrains object,
////    //      tomando los datos del constructor de ...ctrol_params.h
//    m_pBaseConstrains = LENMUS_NEW TheoIntervalsConstrains("TheoIntervals", m_appScope);
//    TheoHarmonyCtrolParams builder(m_pBaseConstrains);
//    builder.process_params( m_pDyn->get_params() );
//}
//
//lmEditorMode* TheoHarmonyCtrol::CreateEditMode()
//{
//    //This method is invoked each time a LENMUS_NEW problem is generated, in order to define
//    //editor customizations for the created exercise. Ownership of created lmEditorMode object
//    //is transferred to the document and deleted there.
//
//    lmEditorMode* pEditMode = LENMUS_NEW lmEditorMode( CLASSINFO(ImoDocumentCanvas), CLASSINFO(lmHarmonyProcessor) );
//    pEditMode->ChangeToolPage(lmPAGE_NOTES, CLASSINFO(lmToolPageNotesHarmony) );
//    pEditMode->SetModeName("TheoHarmonyCtrol");
//    pEditMode->SetModeVers("1");
//
//    return pEditMode;
//}
//
//wxDialog* TheoHarmonyCtrol::get_settings_dialog()
//{
//    //Returns a pointer to the dialog for customizing the exercise.
//
//    //TODO: Create the dialog class and implement it. The uncomment following code:
//    //wxWindow* pParent = dynamic_cast<wxWindow*>(m_pCanvas);
//    //return LENMUS_NEW DlgCfgTheoHarmony(pParent, m_pConstrains, m_pConstrains->is_theory_mode());
//    return (wxDialog*)nullptr;
//}
//
//void TheoHarmonyCtrol::set_new_problem()
//{
//    //This method creates a problem score, satisfiying the restrictions imposed
//    //by exercise options and user customizations.
//
//    // TODO: Possible exercise options:
//    //         exercise 1,2:
//    //             inversions allowed
//    //             elision allowed?
//    //          allowed random key signature (if not: use fixed C major)
//    //
//    // TODO: Possible improvements:
//    //         generalize for chords of N notes
//    //         make "number of measures" dependant from the "key signature"?
//    //         calculate numerals from chord info + key signature + mode
//
//
//    //  all-exercises generic data
//    const int lmNUM_HARMONY_EXERCISES = 3;
//    const int nNUM_INTERVALS_IN_N_HARMONY_EXERCISE = 2;
//    int nNumMeasures = 2;
//    //  each-exercise specific data
//    wxString sExerciseDescription;
//    wxString sNote = "q"; // todo: calculate note note duration from time signature
//    wxString sLDPGoBack = wxString::Format("(musicData (goBack %s) )", sNote.wx_str());
//
//    // Carlos  jun-09
//    //  Three types of problem
//    //   1) fixed bass
//    //   2) fixed soprano
//    //   3) chord notation
//
//    // select a random exercise type
//    RandomGenerator oGenerator;
///*@@ cambiado provisionalmente
//    nHarmonyExcerciseType = oGenerator.random_number(1, lmNUM_HARMONY_EXERCISES);
//*/
//    static int nExType = 0;
//    if ( ++nExType > lmNUM_HARMONY_EXERCISES )
//        nExType = 1;
//    nHarmonyExcerciseType = nExType;
//
//    // todo: inversions allowed: make it an exercise option
//    if ( nHarmonyExcerciseType == 3)
//        bInversionsAllowedInHarmonyExercises = true;
//    else
//        bInversionsAllowedInHarmonyExercises = false;
//
//
//    wxString sPattern;
//    lmLDPParser parserLDP;
//    lmLDPNode* pNode;
//    lmVStaff* pVStaff;
//    ImoNote* pNoteToAttach = nullptr;
//    wxString sExerciseTitle;
//    bool bDebugDisplayHiddenNotes = false; // activate only for debug
//
//    if ( nHarmonyExcerciseType >= 1 && nHarmonyExcerciseType <= lmNUM_HARMONY_EXERCISES )
//    {
//        // Prepare a score that meets the restrictions
//
//        m_nKey = oGenerator.generate_key( m_pConstrains->GetKeyConstrains() );
//
//        if (nHarmonyExcerciseType == 3)
//            sExerciseDescription  =  wxString::Format(" Cipher the chords");
//        else
//            sExerciseDescription  =  wxString::Format(
//                " Fixed %s; root position. Complete the chord notes."
//                , (nHarmonyExcerciseType == 1? "bass": "soprano") );
//
//        sExerciseTitle = wxString::Format(" Exercise type %d : %s "
//            , nHarmonyExcerciseType, sExerciseDescription.wx_str());
//
//        //create a score with a bass line
//
//        wxLogMessage(" CLEARING DATA ");
//        for (int nC=0; nC < nMAX_HARMONY_EXERCISE_CHORDS; nC++)
//        {
//            if (pHE_Chords[nC] != nullptr)
//            {
//                wxLogMessage("  deleting chord %d", nC);
//                //@ todo: to do? delete pHE_Chords[nC];
//                pHE_Chords[nC] = nullptr;
//            }
//            for (int nV=0; nV < nNUM_VOICES_IN_HARMONY_EXERCISE; nV++)
//            {
//                if (pHE_Notes[nC][nV] != nullptr)
//                {
//                    wxLogMessage("  deleting note chord %d v:%d", nC, nV);
//                    //@ todo: to do? delete pHE_Notes[nC][nV];
//                    pHE_Notes[nC][nV] = nullptr;
//                }
//                nHE_NotesFPitch[nC][nV] = 0;
//                sHE_Notes[nC][nV] = "";
//                sHE_Pattern[nC][nV] = "";
//            }
//        }
//
//        m_pProblemScore = static_cast<ImoScore*>(ImFactory::inject(k_imo_score, m_pDoc));
//        ImoInstrument* pInstr = m_pProblemScore->add_instrument();    // (
//                                    g_pMidi->get_default_voice_channel(),
//						            g_pMidi->get_default_voice_instr(), "");
//
//        pVStaff = pInstr->GetVStaff();
//        pInstr->add_staff();               //add second staff: five lines, standard size
//        pInstr->add_clef( k_clef_G2, 1 );     //G clef on first staff
//        pInstr->add_clef( k_clef_F4, 2 );     //F clef on second staff
//        pInstr->add_key_signature( m_nKey ); //key signature
//        pInstr->add_time_signature(2 ,4);    //2/4 time signature
//
//
//        lmFontInfo tNumeralFont = {"Times New Roman", 11, wxFONTSTYLE_NORMAL,
//                                    wxFONTWEIGHT_BOLD };
//        lmTextStyle* pNumeralStyle = m_pProblemScore->GetStyleName(tNumeralFont);
//
//        wxString sNumeralsDegrees[7] =
//        {" I", " II", "III", " IV", "  V", " VI", "VII"};
//        const int aCadences[][2] =
//        {
//            { 3, 0}, // plagal    IV I
//            { 4, 0}, // perfect   V I
//            { 4, 3}, // deceptive V IV
//            { 4, 5}, // deceptive V VI
//            { 4, 1}, // deceptive V II
//            { 3, 4}, // half     IV V
//            { 0, 4}, // half     I  V
//            { 1, 4}, // half     II V
//        };
//       const int nTotalNumCadences = sizeof(aCadences) / sizeof(aCadences[0]);
//
//        wxString sNumerals;
//
//        //loop the add notes
//        int nChordCount = 0;
//        int nOctave = 0;
//        int nVoice = 0;
//        int nRootNoteStep = 0;
//        int nStaff;
//        // cadences (TODO: improve)
//        int nCadence = 0;
//        int nInconclusive = 0;
//
//        for (int iN=0; iN < (nNumMeasures*2); iN+=2)
//        {
//            //add barline for previous measure
//            if (iN != 0)
//                pInstr->add_barline(k_barline_simple);
//            else
//                pInstr->add_spacer(20);
//
//            //two chords per measure (time signature is 2 / 4)
//            for (int iM=0; iM < 2; iM++)
//            {
//                // Generate chords with no harmonic progression errors:
//                //  Loop:
//                //     generate a random chord
//                //     create voices in FPitch, obviously matching the chord notes
//                //   Until chord ok: no errors
//                //   With the LENMUS_NEW chord:
//                //     Create figured bass
//                //     Calculate numeral
//                //     Create the score notes for the voices
//                //
//                //   Display
//                //       Exercise 1: bass note and numeral
//                //       Exercise 2: soprano note and numeral
//                //       Exercise 3: all notes and numeral
//                //
//
//                int nNumChordLinkErros = -1;
//                int nAttempts = 0;
//                int nMaxAttempts = 20;
//                int nInversions = 0;
//
//                // try to create a LENMUS_NEW chord until no link error with previous chords
//                pHE_Chords[nChordCount] = 0;
//                wxLogMessage(" ====== START WITH CHORD %d ======= ", nChordCount );
//                while ( nNumChordLinkErros != 0 && nAttempts < nMaxAttempts)
//                {
//                    if (nAttempts)
//                        wxLogMessage("   ***** NEW ATTEMPT (%d) for CHORD %d *****", nAttempts, nChordCount );
//                    // Root note
//                    if (nChordCount > 1 && nChordCount < nMAX_HARMONY_EXERCISE_CHORDS) // 2 last chords: use a cadence
//                    {
//                        if (nChordCount == 2) // choose cadence
//                        {
//                            nInconclusive = oGenerator.random_number(0, 1);
//                            if (nInconclusive)
//                            {
//                                nCadence = oGenerator.random_number(2, nTotalNumCadences-1);
//                            }
//                            else
//                            {
//                                nCadence = oGenerator.random_number(0, 1);
//                            }
//                            wxLogMessage(" @CHORD %d CADENCE:%d", nChordCount, nCadence );
//                        }
//                        nRootNoteStep = aCadences[nCadence][nChordCount-2];
//                        if (nRootNoteStep < 3)
//                            nOctave = 3;
//                        else
//                            nOctave = 2;
//                        wxLogMessage(" @CHORD %d Cadence:%d nInconclusive:%d  rootStep:%d  octave:%d"
//                            , nChordCount, nCadence, nInconclusive, nRootNoteStep, nOctave );
//                    }
//                    else
//                    {
//                        nOctave = oGenerator.random_number(2, 3);
//                        // this is done to make the notes appear more centered in the bass staff
//                        if (nOctave == 3 ) // octave 3 : notes c,d,e
//                           nRootNoteStep = oGenerator.random_number(0, 2);
//                        else // octave 2 : notes f,g,a,b
//                           nRootNoteStep = oGenerator.random_number(3, 6);
//                    }
//                    nInversions = 0;
//                    if (bInversionsAllowedInHarmonyExercises)
//                    {
//                        // Calculate a random number of inversions and apply them
//                        nInversions = oGenerator.random_number(0, nNUM_INTERVALS_IN_N_HARMONY_EXERCISE);
//                    }
//
//                    if (pHE_Chords[nChordCount])
//                    {
//                        if ( nAttempts == 0)
//                        {
//                            wxLogMessage(" @@ ERROR: impossible:  chord no emtpy but 0 previous attempts " );
//                        }
//                        else
//                        {
//                            wxLogMessage(" deleting chord %d of previous attempt ", nChordCount );
//                        }
//                        delete pHE_Chords[nChordCount];
//                        pHE_Chords[nChordCount] = 0;
//                    }
//
//                    //
//                    // create the chord
//                    //
//                    wxLogMessage("Creating_ImoScoreChord: step:%d octave:%d inversions:%d  key:%d"
//                        , nRootNoteStep, nOctave, nInversions, m_nKey );
//                    pHE_Chords[nChordCount] = LENMUS_NEW ImoScoreChord(nRootNoteStep, m_nKey
//                        , nNUM_INTERVALS_IN_N_HARMONY_EXERCISE, nInversions, nOctave);
//
//
//                    // This is the bass voice (root note)
//                    nHE_NotesFPitch[nChordCount][nBassVoiceIndex] = pHE_Chords[nChordCount]->get_note(0);
//                    // but... additional limitation:   bass note might be too high (if there are 2 inversions for example)
//                    //   we set a limit in d3
//                    if (nHE_NotesFPitch[nChordCount][nBassVoiceIndex] > FPitchK(k_step_D, 3, m_nKey) )
//                    {
//                        nHE_NotesFPitch[nChordCount][nBassVoiceIndex] -= k_interval_p8;
//                        wxLogMessage(" Bass reduced one octave to : %d "
//                            , nHE_NotesFPitch[nChordCount][nBassVoiceIndex]
//                            );
//                    }
//                    wxLogMessage(" Bass voice V%d,FINAL: %d (%s)"
//                        , nBassVoice
//                        , nHE_NotesFPitch[nChordCount][nBassVoiceIndex]
//                        , FPitch_ToAbsLDPName(nHE_NotesFPitch[nChordCount][nBassVoiceIndex]).wx_str());
//
//                    // Bass voice final value calculated
//                    // Set the bass voice note in the chord
//                    pHE_Chords[nChordCount]->AddNoteLmFPitch(nHE_NotesFPitch[nChordCount][nBassVoiceIndex]);
//
//                    wxLogMessage("<><>  CHORD %d: bass STEP:%d, octave:%d, key:%d inversions:%d ROOT:%d (%s) ==="
//                        ,nChordCount, nRootNoteStep, nOctave, m_nKey, nInversions
//                        ,FPitchK(nRootNoteStep, nOctave, m_nKey)
//                        ,FPitch_ToAbsLDPName( nHE_NotesFPitch[nChordCount][nBassVoiceIndex]).wx_str()
//                        );
//                    wxLogMessage(_T("\t Chord: [%s]"), pHE_Chords[nChordCount]->ToString().wx_str());
//
//
//                    //
//                    // Create the rest of voices: 3,2,1 (tenor, baritone, soprano)
//                    //   by adding intervals (and octaves) to the bass
//                    //
//
//                    //  There are three possible values, based on intervals,  for each voice:
//                    //    1: bass + N octaves (0 interval: duplicate root)
//                    //    2: bass + N octaves + 1st interval
//                    //    3: bass + N octaves + 2nd interval
//                    //  Notes:
//                    //    if there is 5th elided: do not apply rule 3
//                    //    if root note is NOT duplicated: do not apply rule 1
//                    //  For this exercise we intend to generate 'normal' (not very strange) chords, so
//                    //    better to calculate each voice using all 3 rules and not repeating any of them
//                    //     (it implies duplicate always the root)
//                    FPitch nIntvB[3];
//                    nIntvB[0] = oGenerator.random_number(0, 2);
//                    nIntvB[1] = (nIntvB[0] + oGenerator.random_number(1, 2)) % 3 ;
//                    nIntvB[2] = 3 - (nIntvB[0] + nIntvB[1] ) ;
//                    wxLogMessage(_T("\t\t nIntvB %d %d %d"), nIntvB[0], nIntvB[1], nIntvB[2]);
//
//                    int ni = 0; // index to nIntvB
//                    for (nVoice = nNUM_VOICES_IN_HARMONY_EXERCISE-1; nVoice>=1; nVoice--,ni++)
//                    {
//                        assert(pHE_Chords[nChordCount] != nullptr);
//
//                        int nVoiceIndex = nVoice - 1;
//
//                        // start with BASS NOTE
//                        nHE_NotesFPitch[nChordCount][nVoiceIndex] = nHE_NotesFPitch[nChordCount][nBassVoiceIndex];
//
//                        // Apply the calculated intervals
//                        if (nIntvB[ni])
//                        {
//                            nHE_NotesFPitch[nChordCount][nVoiceIndex] += pHE_Chords[nChordCount]->GetInterval(nIntvB[ni]);
//                        }
//
//                        wxLogMessage(_T("  \tV%d, after applying interval %d : %d (%s), [ni:%d]")
//                            , nVoice
//                            , nIntvB[ni]
//                            , nHE_NotesFPitch[nChordCount][nVoiceIndex]
//                            , FPitch_ToAbsLDPName(nHE_NotesFPitch[nChordCount][nVoiceIndex]).wx_str()
//                            , ni
//                            );
//
//                        // Limitation: a voice can not be lower than the previous...
//                        while ( nHE_NotesFPitch[nChordCount][nVoiceIndex+1] >= nHE_NotesFPitch[nChordCount][nVoiceIndex])
//                        {
//                            nHE_NotesFPitch[nChordCount][nVoiceIndex] += k_interval_p8;
//                            wxLogMessage(" Added octave to voice V%d: %d "
//                                , nVoice
//                                , nHE_NotesFPitch[nChordCount][nVoiceIndex]);
//                        }
//
//                        // additional limitation: increase tenor voice a octave if bass is low and tenor is close to bass
//                        //  to avoid voice distance problems
//                        //  aware: tenor-bass is the only consecutive voices allowed to have a distance higher than octave
//                        if ( nVoiceIndex == nTenorVoiceIndex && nOctave < 3
//                            && (nHE_NotesFPitch[nChordCount][nTenorVoiceIndex] - nHE_NotesFPitch[nChordCount][nBassVoiceIndex]) <= lm_M3 )
//                        {
//                            nHE_NotesFPitch[nChordCount][nTenorVoiceIndex] += k_interval_p8;
//                            wxLogMessage(" Raise Tenor: added octave to voice V%d: %d "
//                                , nVoice
//                                , nHE_NotesFPitch[nChordCount][nTenorVoiceIndex]
//                                );
//                        }
//
//                        // additional limitation: baritone voice should be in upper staff (aprox. octave should be > 3)
//                        const int fUpperStaffLimit = (k_interval_p8*4)-lm_M3;
//                        // aware: do not raise more than one octave; otherwise a rule is broken (octave distance)
//                        if ( nVoiceIndex == nBaritoneVoiceIndex && nHE_NotesFPitch[nChordCount][nBaritoneVoiceIndex] < fUpperStaffLimit )
//                        {
//                            nHE_NotesFPitch[nChordCount][nBaritoneVoiceIndex] += k_interval_p8;
//                            wxLogMessage(" Raise to 2nd staff: added octave to voice V%d: %d (min:%d) "
//                                , nVoice
//                                , nHE_NotesFPitch[nChordCount][nBaritoneVoiceIndex]
//                                , fUpperStaffLimit);
//                        }
//
//                        wxLogMessage("V%d,    FINAL: %d (%s)"
//                            , nVoice
//                            , nHE_NotesFPitch[nChordCount][nVoiceIndex]
//                            , FPitch_ToAbsLDPName(nHE_NotesFPitch[nChordCount][nVoiceIndex]).wx_str());
//
//                       // set this note in the chord
//                       pHE_Chords[nChordCount]->AddNoteLmFPitch(nHE_NotesFPitch[nChordCount][nVoiceIndex]);
//
//                    } // for voice...
//
//                    // check harmonic progression errors
//                    wxLogMessage("###BEFORE %d ANALYSIS OF CHORD LINK ERRORS OF  CHORD %d: %s"
//                         ,nAttempts, nChordCount , pHE_Chords[nChordCount]->ToString().wx_str());
//                    nNumChordLinkErros =  AnalyzeHarmonicProgression(pHE_Chords, nChordCount+1, 0);
//
//                    wxLogMessage("##RESULT: %d LINK ERRORS", nNumChordLinkErros);
//
//                    if (nNumChordLinkErros == 0)
//                         wxLogMessage("<<<<< CHORD %d OK!!! after %d attempts >>>>>>>>>"
//                              , nChordCount , nAttempts, nNumChordLinkErros);
//
//                    nAttempts++;
//
//                    if  ( nAttempts > nMaxAttempts)
//                    {
//                       wxLogMessage(_T("\n **** CHORD %d TO MANY TRIES ******\n"), nChordCount);
//                    }
//
//                }
//
//                // Calculate the figured bass
//                //
//                //  build a chord from a list of notes in LDP source code
//                //    Chord(int nNumNotes, wxString* pNotes, EKeySignature nKey = k_key_C);
//                pHE_FiguredBass[nChordCount] = LENMUS_NEW lmFiguredBass(pVStaff, lmNEW_ID
//                    , pHE_Chords[nChordCount], m_nKey);
//
//                wxLogMessage("  FIGURED BASS:%s"
//                   , pHE_FiguredBass[nChordCount]->GetFiguredBassString().wx_str());
//
//                // At this point we already have the notes in FPitch. Now:
//                //  Create the notes of the score
//                //  Display notes and numerals
//
//                //
//                // Create each ImoNote
//                //
//                wxString sUpDown[2] = {  "up", "down"};
//                nVoice=1; // Voices 1(soprano)(index:0) to 4(bass)(index:3)
//                int nNumDisplayedNotesInChord = 0;
//                for (nStaff=1; nStaff<3; nStaff++)  // V1 (soprano) in P1 (upper staff)
//                {
//                    for (int nVoiceInStaff=1; nVoiceInStaff<3; nVoiceInStaff++,nVoice++)
//                    {
//                        assert(nVoice >= 1 && nVoice <= nNUM_VOICES_IN_HARMONY_EXERCISE);
//
//                        // aware: bass note: voice 4
//                       int nVoiceIndex = nVoice-1;
//                        sHE_Notes[nChordCount][nVoiceIndex] = wxString::Format("%s"
//                           , FPitch_ToAbsLDPName(nHE_NotesFPitch[nChordCount][nVoiceIndex]).wx_str());
//
//                        // Calculate the pattern required to create the note in the score
//                        sHE_Pattern[nChordCount][nVoiceIndex] = wxString::Format("(n %s %s p%d v%d (stem %s))"
//                           , sHE_Notes[nChordCount][nVoiceIndex].wx_str()
//                           , sNote.wx_str()
//                           , nStaff, nVoice, sUpDown[nVoiceInStaff-1].wx_str());
//
//                        wxLogMessage(_T("\t Staff %d, V%d (index:%d) %d [%s], pattern: %s")
//                              , nStaff
//                              , nVoice
//                              , nVoiceIndex
//                              , nHE_NotesFPitch[nChordCount][nVoiceIndex]
//                              , sHE_Notes[nChordCount][nVoiceIndex].wx_str()
//                              , sHE_Pattern[nChordCount][nVoiceIndex].wx_str()
//                              );
//
//
//
//                        // Display the notes in the score
//                        //  Exercise 1: only bass (voice 1)
//                        //  Exercise 2: only soprano (voice 4)
//                        //  Exercise 3: all
//                        // Go back:
//                        // Exercise 1, 2: Never (only one voice is displayed per chord)
//                        // Exercise 3: go back after voices 1,2,3
//
//                        if (
//                            (nHarmonyExcerciseType == 1 && nVoice == nBassVoice ) ||
//                            (nHarmonyExcerciseType == 2 && nVoice == nSopranoVoice ) ||
//                             nHarmonyExcerciseType == 3
//                            )
//                        {
//                            if ( nNumDisplayedNotesInChord > 0 && nNumDisplayedNotesInChord < nNUM_VOICES_IN_HARMONY_EXERCISE)
//                            {
//                                pInstr->add_object(( sLDPGoBack );
//                                parserLDP.AnalyzeMusicData(pNode, pVStaff);
//                            }
//                            pInstr->add_object((  sHE_Pattern[nChordCount][nVoiceIndex] );
//                            pHE_Notes[nChordCount][nVoiceIndex] = parserLDP.AnalyzeNote(pNode, pVStaff);
//                            nNumDisplayedNotesInChord++;
//                            pHE_Notes[nChordCount][nVoiceIndex]->SetVoice(nVoice);
//                            pNoteToAttach = pHE_Notes[nChordCount][nVoiceIndex];
//                            wxLogMessage(" V:%d added pattern: %s ***"
//                                , nVoice, sHE_Pattern[nChordCount][nVoiceIndex].wx_str());
//
//                            // Set the actual ImoNote in the chord
//                            pHE_Chords[nChordCount]->SetLmNote(pNoteToAttach);
//
//                            if (  bDebugDisplayHiddenNotes ) // only for the debug
//                            {
//                                wxColour myBlue( 10, 10, 200);
//                                pNoteToAttach->SetColour(myBlue);
//                                wxLogMessage("    Ej:%d V:%d AZUL", nHarmonyExcerciseType, nVoice);
//                            }
//                            else    wxLogMessage("    Ej:%d V:%d NEGRO", nHarmonyExcerciseType, nVoice);
//                        }
//                        else    wxLogMessage("    Ej:%d V:%d OCULTA", nHarmonyExcerciseType, nVoice);
//
//                    }
//
//                }
//
//                //    Display the numeral, according the root step
////todo remove                StepType nRootStep = (pHE_Chords[nChordCount]->GetNormalizedRoot()).step();
//                StepType nRootStep = pHE_Chords[nChordCount]->GetChordDegree();
//                // todo: use GetDegreeString( pHE_Chords[nChordCount]->GetChordDegree() )
//                lmTextItem* pNumeralText = LENMUS_NEW lmTextItem(
//                    pNoteToAttach, lmNEW_ID, sNumeralsDegrees[nRootStep],
//                    lmHALIGN_DEFAULT, pNumeralStyle);
//                pNoteToAttach->AttachAuxObj(pNumeralText);
//                pNumeralText->SetUserLocation(0.0f, 230.0f );
//
//                wxLogMessage("FINAL_CHORD %d: %s"
//                    , nChordCount, pHE_Chords[nChordCount]->ToString().wx_str());
//
//                nChordCount++;
//            }
//        }
//        nHarmonyExerciseChordsToCheck = nChordCount;
//    }
//    wxLogMessage(" CREATED EXERCISE %d with %d chords"
//        ,nHarmonyExcerciseType, nHarmonyExerciseChordsToCheck);
//    for (int i=0; i<nHarmonyExerciseChordsToCheck; i++)
//        wxLogMessage(" CHORD %d: %s", i, pHE_Chords[i]->lmFPitchChord::ToString().wx_str());
//
//
//    //add final barline
//    pInstr->add_barline(k_barline_end);
//
//    lmFontInfo tTitleFont = {"Times New Roman", 10, wxFONTSTYLE_NORMAL,
//                                wxFONTWEIGHT_BOLD };
//    lmTextStyle* pTitleStyle = m_pProblemScore->GetStyleName(tTitleFont);
//    ImoScoreTitle* pTitle = m_pProblemScore->AddTitle(sExerciseTitle, lmHALIGN_CENTER, pTitleStyle);
//    lmLocation tTitlePos = g_tDefaultPos;
//    pTitle->SetUserLocation(tTitlePos); // only necessary if wanted to be positioned at a specific point
//
//    //set the name and the title of the score
//    m_pProblemScore->SetScoreName( sExerciseTitle );
//}
//
//void TheoHarmonyCtrol::on_settings_changed()
//{
//    //This method is invoked when user clicks on the 'Accept' button in
//    //the exercise setting dialog. You receives control just in case
//    //you would like to do something (i.e. reconfigure exercise displayed
//    //buttons to take into account the LENMUS_NEW exercise options chosen by the user).
//
//    //In this exercise there is no needed to do anything
//}
//
//void TheoHarmonyCtrol::initialize_strings()
//{
//    //This method is invoked only once: at control creation time.
//    //Its purpose is to initialize any variables containing strings, so that
//    //they are translated to the language chosen by user. Take into account
//    //that those strings requiring translation can not be statically initialized,
//    //as at compilation time we know nothing about desired language.
//
//    //In this exercise there is no needed to translate anything
//}


}  //namespace lenmus
