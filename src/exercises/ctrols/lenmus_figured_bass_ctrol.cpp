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
//// For compilers that support precompilation, includes "wx.h".
//#include "wx/wxprec.h"
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
//    //m_pScoreProc = LENMUS_NEW lmHarmonyProcessor();
//
//    create_controls();
//
//    //define editor customizations
//    m_pEditMode = LENMUS_NEW lmEditorMode( CLASSINFO(ImoDocumentCanvas), CLASSINFO(lmHarmonyProcessor) );
//    m_pEditMode->ChangeToolPage(lmPAGE_NOTES, CLASSINFO(lmToolPageNotesHarmony) );
//    m_pEditMode->SetModeName("TheoHarmonyCtrol");
//    m_pEditMode->SetModeVers("1");
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
//wxDialog* TheoHarmonyCtrol::get_settings_dialog()
//{
//    //Returns a pointer to the dialog for customizing the exercise.
//
//    //TODO: Create the dialog class and implement it. The uncomment following code:
//    //wxWindow* pParent = dynamic_cast<wxWindow*>(m_pCanvas);
//    //return LENMUS_NEW DlgCfgTheoHarmony(this, m_pConstrains, m_pConstrains->is_theory_mode());
//    return (wxDialog*)nullptr;
//}
//
//void TheoHarmonyCtrol::set_new_problem()
//{
//    //This method creates a problem score, satisfiying the restrictions imposed
//    //by exercise options and user customizations.
//
//    // Carlos  jun-09
//    //  Three types of problem
//    //   1) fixed bass
//    //   2) fixed soprano
//    //   3) chord notation
//
//    // select a random key signature
//    RandomGenerator oGenerator;
//    nHarmonyExcerciseType = oGenerator.random_number(1, 2);
//    wxString sExerciseDescription;
//    wxString sPattern;
//    ImoNote* pNote;
//    lmLDPParser parserLDP("en", "utf-8");
//    lmLDPNode* pNode;
//    lmVStaff* pVStaff;
//    wxString sExerciseTitle;
//
//    if ( nHarmonyExcerciseType == 1 || nHarmonyExcerciseType == 2 )
//    {
//        // Prepare a score with that meets the restrictions
//        // TODO: VER ESTA TONALIDAD
//        m_nKey = oGenerator.generate_key( m_pConstrains->GetKeyConstrains() );
//
//        // TODO: think about exercise options
//        //        note duration?
//        //        ...
//
//
//        int nNumMeasures = 2;
//
//        sExerciseDescription  =  wxString::Format(
//            " Fixed %s; root position. Complete the chord notes."
//            , (nHarmonyExcerciseType == 1? "bass": "soprano") );
//
//        sExerciseTitle = wxString::Format(" Exercise type %d : %s "
//            , nHarmonyExcerciseType, sExerciseDescription.wx_str());
//
//        //create a score with a bass line
//
//        //---- Harmonyexercisedata:
//        // Exercise 1 checks:
//        //   root note
//        //   chord type
//        // Root notes
//        for (int i=0; i < nMAX_E1BCHORDS; i++)
//        {
//            nExerciseBassNotesFPitch[i] = 0;
//            nExercise2NotesFPitch[i] = 0;
//            nExercise1ChordType[i] = ect_Max;
//        }
//
//        m_pProblemScore = LENMUS_NEW ImoScore();
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
//        lmFontInfo tNumeralFont = {"Times New Roman", 12, wxFONTSTYLE_NORMAL,
//                                    wxFONTWEIGHT_BOLD };
//        lmTextStyle* pNumeralStyle = m_pProblemScore->GetStyleName(tNumeralFont);
//        wxString sNotes[7]    = {"c", "d", "e", "f", "g", "a", "b"};
//        // TODO: improve! (calculate numerals from chord info + key signature + mode)
//        //        this is provisional; only for key signature = C Major
//        wxString sNumeralsDegrees[7] =
//        {"I", "II", "III", "IV", "V", "VI", "VII"};
//        wxString sNumerals;
//        EChordType nE1ChordTypes[7] =
//         // TODO: MAKE A GENERIC METHOD to get chord type from: root note + key sig
//        // example: key-sig: DoM
//        //      I             II              III              IV             V             VI             VII
//        {ect_MajorTriad, ect_MinorTriad, ect_MinorTriad, ect_MajorTriad, ect_MajorTriad, ect_MinorTriad, ect_DimTriad, };
//        // For exercise 2, given a numeral (bass note; chord in root poition) : calculate soprano pitch. No inversions
//        FPitch nBassSopranoInterval[2][7] =  {
//        //      I             II              III              IV             V             VI             VII
//        //{ect_MajorTriad, ect_MinorTriad, ect_MinorTriad, ect_MajorTriad, ect_MajorTriad, ect_MinorTriad, ect_DimTriad, };
//          {   lm_M3,        lm_m3,          lm_p5,          lm_M3,          lm_M3,         lm_m3,         lm_m3},
//          {   lm_p5,         lm_p5,         lm_p5,          lm_p5,          lm_p5,         lm_p5,         lm_d5} };
//        // TODO: this info could be get from ChordManager.cpp tData, with a global method:
//        //  { ... { lm_M3, lm_p5, lmNIL }},      //MT        - MajorTriad
//        //  { ... { lm_m3, lm_p5, lmNIL }},      //mT        - MinorTriad
//        //  { ... { lm_m3, lm_d5, lmNIL }},      //dT        - DimTriad
//        //  global method to get interval between voices of a given chord type (specify also nversions)
//
//
//        //loop the add notes
//        int nNoteCount = 0;
//        int nOctave;
//        int nVoice;
//        int nBassNoteStep;
//        int nStaff;
//        int nAccidentals;
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
//                // Process (Exercies 1 and 2)
//                //   Generate bass note
//                //     Get numerals from bass note and display it
//                //     Get chord type from bass note
//                //   Exercise 2: get soprano form bass note
//                //   Display note
//                //      Ex. 1: bass
//                //      Ex. 2: soprano
//
//                // Generate the bass note
//                nBassNoteStep = oGenerator.random_number(0, 6);  // root position: bass note is the chord degree
//                nOctave = oGenerator.random_number(2, 3);
//                nAccidentals = 0;
//
//                //  Calculate pitch of bass note and store bass it for later check
//                nExerciseBassNotesFPitch[nNoteCount] = FPitch(nBassNoteStep, nOctave, 0);
//
//                //   Get numerals from bass note
//                sNumerals = sNumeralsDegrees[nBassNoteStep];
//
//                //   Get chord type from bass note and store it for later check
//                nExercise1ChordType[nNoteCount] = nE1ChordTypes[nBassNoteStep];
//
//                //   Exercise 2: get soprano form bass note and store it for later check
//                if (nHarmonyExcerciseType == 2 )
//                {
//                    nExercise2NotesFPitch[nNoteCount] =
//                        nExerciseBassNotesFPitch[nNoteCount]
//                        + nBassSopranoInterval[1][nBassNoteStep]
//                        + k_interval_p8; //
//                }
//
//                //   Display note
//                //      Ex. 1: bass
//                //      Ex. 2: soprano
//                if ( nHarmonyExcerciseType == 1 )  // bass
//                {
//                    nVoice = 4;
//                    nStaff = 2;
//                    sPattern = wxString::Format("(n %s%d q p%d v%d (stem down))"
//                       , sNotes[nBassNoteStep].wx_str(), nOctave, nStaff, nVoice);
//                }
//                else if (nHarmonyExcerciseType == 2 )  // soprano
//                {
//                    nVoice = 1;
//                    nStaff = 1;
//                    sPattern = wxString::Format("(n %s q p%d v%d (stem down))"
//                       , FPitch_ToAbsLDPName(nExercise2NotesFPitch[nNoteCount]).wx_str(), nStaff, nVoice);
//                }
//                pInstr->add_object(( sPattern );
//                pNote = parserLDP.AnalyzeNote(pNode, pVStaff);
//
//                //    Display the numeral
//                lmTextItem* pNumeralText = LENMUS_NEW lmTextItem(pNote, lmNEW_ID, sNumeralsDegrees[nBassNoteStep],
//                                                          lmHALIGN_DEFAULT, pNumeralStyle);
//                pNote->AttachAuxObj(pNumeralText);
//                pNumeralText->SetUserLocation(0.0f, 230.0f );
//
//                if ( nHarmonyExcerciseType == 1 )  // bass
//                {
//                    wxLogMessage("Ex %d Measure %d Chord %d, BASS: %s%d (%s) FP:%d  pattern:%s"
//                      , nHarmonyExcerciseType , iN, iM,  sNotes[nBassNoteStep].wx_str(), nOctave
//                      , FPitch_ToAbsLDPName(nExerciseBassNotesFPitch[nNoteCount]).wx_str()
//                      , nExerciseBassNotesFPitch[nNoteCount], sPattern.wx_str());
//                }
//                else if (nHarmonyExcerciseType == 2 )  // soprano
//                {
//
//                    wxLogMessage("Ex %d Measure %d Chord %d, BASS: %s%d (%s) FP:%d (I1:%d %s) (I2:%d %s),  SOPRANO:%d %s pattern:%s"
//                      , nHarmonyExcerciseType , iN, iM,  sNotes[nBassNoteStep].wx_str(), nOctave
//                      , FPitch_ToAbsLDPName(nExerciseBassNotesFPitch[nNoteCount]).wx_str()
//                      , nExerciseBassNotesFPitch[nNoteCount]
//                      , nExerciseBassNotesFPitch[nNoteCount] + nBassSopranoInterval[0][nBassNoteStep]
//                      , FPitch_ToAbsLDPName(nExerciseBassNotesFPitch[nNoteCount] + nBassSopranoInterval[0][nBassNoteStep]).wx_str()
//                      , nExerciseBassNotesFPitch[nNoteCount] + nBassSopranoInterval[1][nBassNoteStep]
//                      , FPitch_ToAbsLDPName(nExerciseBassNotesFPitch[nNoteCount] + nBassSopranoInterval[1][nBassNoteStep]).wx_str()
//                      , nExercise2NotesFPitch[nNoteCount]
//                      , FPitch_ToAbsLDPName(nExercise2NotesFPitch[nNoteCount]).wx_str()
//                      , sPattern.wx_str());
//
//                }
//
//                nNoteCount++;
//
//            }
//        }
//        nHarmonyExercise1ChordsToCheck = nNoteCount;
//    }
//
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
