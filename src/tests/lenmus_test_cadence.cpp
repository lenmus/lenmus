//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2018 LenMus project
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

#include <UnitTest++.h>
#include <sstream>
#include "lenmus_config.h"

//classes related to these tests
#include "lenmus_injectors.h"
#include "lenmus_paths.h"
#include "lenmus_string.h"
#include "lenmus_cadence.h"
#include "lenmus_chord.h"

//lomse
#include <lomse_doorway.h>
#include <lomse_document.h>
#include <lomse_score_utilities.h>
using namespace lomse;

using namespace UnitTest;
using namespace std;
using namespace lenmus;

////Helper class, to acccess protected members
//class MyCadence : public Cadence
//{
//public:
//    MyCadence() : Cadence() {}
//
//    bool my_is_valid_chord(Cadencelenmus::Chord* pChord, Chord* pBasicChord,
//                           CadenceChord* pPrevChord, bool fExhaustive)
//    {
//        return check_chord(pChord, pBasicChord, pPrevChord, fExhaustive);
//
//    }
//
//
//};

#define CHECK_MESSAGE(message) \
    UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__), (std::string(message)).c_str());

class CadenceTestFixture
{
public:
    ApplicationScope m_appScope;
    LibraryScope* m_pLibScope;
    string m_scores_path;
    stringstream m_msg;

    CadenceTestFixture()     //SetUp fixture
        : m_appScope(cout)
        , m_scores_path(LENMUS_TEST_SCORES_PATH)
    {
        LomseDoorway& door = m_appScope.get_lomse();
        m_pLibScope = door.get_library_scope();
        m_msg << "msg: ";
    }

    ~CadenceTestFixture()    //TearDown fixture
    {
        m_msg.str("");
        m_msg << endl;
    }

    void write_msg()
    {
        m_msg << endl;
        CHECK_MESSAGE( m_msg.str() );
    }

    bool dbg_check_chord(CadenceChord* pChord, const wxString& sFunct,
                         CadenceChord* pPrevChord, EKeySignature nKey, bool fExhaustive)
    {
        int nPrevAlter[4] = {0,0,0,0};
        Cadence::get_chromatic_alterations(pPrevChord, nKey, &nPrevAlter[0]);
        int nStepLeading = KeyUtilities::get_step_for_leading_note(nKey);
        int iLeading =
            Cadence::find_leading_tone_in_previous_chord(pPrevChord, nStepLeading);

        lenmus::Chord* pBasicChord = Cadence::get_basic_chord_for(sFunct, nKey);

        bool fValid = Cadence::check_chord(pChord, pBasicChord, pPrevChord, nKey,
                                           &nPrevAlter[0], nStepLeading, iLeading,
                                           fExhaustive);
        delete pBasicChord;
        return fValid;
    }

};

SUITE(CadenceTest)
{

    TEST_FIXTURE(CadenceTestFixture, cadence_0)
    {
        Cadence cad;
        bool fCreated = cad.create(k_cadence_perfect, k_key_C);

        CHECK( fCreated == true );
        CHECK( cad.get_num_chords() == 2 );
        CHECK( cad.get_function(0) == "V" );
        CHECK( cad.get_function(1) == "I" );

        //m_msg << pScore->to_string();
        //write_msg();
    }

    TEST_FIXTURE(CadenceTestFixture, cadence_001)
    {
        //@001. get_root_note()

        FPitch fp = Cadence::get_root_note("III", k_key_c);
        CHECK( fp.to_abs_ldp_name() == "-e3" );

        fp = Cadence::get_root_note("IIb6", k_key_C);
        CHECK( fp.to_abs_ldp_name() == "d3" );

//        m_msg << fp.to_abs_ldp_name();
//        write_msg();
    }

    TEST_FIXTURE(CadenceTestFixture, cadence_002)
    {
        //@002. All functions in m_aFunction are defined in m_aFunctionToIntervals

        bool fError = false;
        m_msg << endl;
        for (int iF=0; iF < 2; ++iF)
        {
            for (int iC=0; iC < k_cadence_max; ++iC)
            {
                wxString sFunct = dbg_get_function_for_cadence(iF, ECadenceType(iC));
                if (!dbg_function_is_defined(sFunct))
                {
                    fError = true;
                    m_msg << "Function " << to_std_string(sFunct) << " not defined" << endl;
                }
            }
        }
        CHECK( fError );

        if (fError)
            write_msg();
    }

    TEST_FIXTURE(CadenceTestFixture, cadence_003)
    {
        //@003. All functions in m_aImperfect are in m_aFunctionToIntervals

        bool fError = false;
        m_msg << endl;
        for (int iF=0; iF < 2; ++iF)
        {
            for (int iC=0; iC < 4; ++iC)
            {
                wxString sFunct = dbg_get_function_for_imperfect(iF, iC);
                if (!dbg_function_is_defined(sFunct))
                {
                    fError = true;
                    m_msg << "Function " << to_std_string(sFunct) << " not defined" << endl;
                }
            }
        }
        CHECK( fError );

        if (fError)
            write_msg();
    }

    TEST_FIXTURE(CadenceTestFixture, cadence_004)
    {
        //@004. All interval combinations in m_aFunctionToIntervals are valid chords
        //      in table tChordData[ect_Max] (in chords.cpp).

        bool fError = false;
        m_msg << endl;
        for (int i=0; i < 100; ++i)
        {
            wxString sFunct = dbg_get_function_in_conversion_table(i);
            if (sFunct == wxEmptyString)
                break;

            lenmus::Chord* pChord = Cadence::get_basic_chord_for(sFunct, k_key_C);     //major
            if (pChord->get_chord_type() == ect_invalid)
            {
                fError = true;
                m_msg << "major chord for " << to_std_string(sFunct) <<
                    " not defined. Intervals=" << pChord->intervals_to_string() << endl;
            }
            delete pChord;

            pChord = Cadence::get_basic_chord_for(sFunct, k_key_c);     //minor
            if (pChord->get_chord_type() == ect_invalid)
            {
                fError = true;
                m_msg << "minor chord for " << to_std_string(sFunct) <<
                    " not defined. Intervals=" << pChord->intervals_to_string() << endl;
            }
            delete pChord;
        }
        CHECK( fError );

        if (fError)
            write_msg();
    }

    TEST_FIXTURE(CadenceTestFixture, cadence_010)
    {
        //@010. get_basic_chord_for()

        lenmus::Chord* pChord = Cadence::get_basic_chord_for("I", k_key_C);
        CHECK( pChord->intervals_to_string() == "M3,p5" );
        CHECK( pChord->note_steps_to_string() == "c,e,g" );
        delete pChord;

        pChord = Cadence::get_basic_chord_for("IVm6", k_key_d);
        CHECK( pChord->intervals_to_string() == "M3,M6" );
        CHECK( pChord->note_steps_to_string() == "g,b,e" );
//        m_msg << "intervals=" << pChord->intervals_to_string()
//              << ", steps=" << pChord->note_steps_to_string();
//        write_msg();
        delete pChord;
    }

    TEST_FIXTURE(CadenceTestFixture, cadence_020)
    {
        //@020. find_leading_tone_in_previous_chord(). Found ok

        CadenceChord prevChord("g2","b3","d4","g4");

        int stepLeading = KeyUtilities::get_step_for_leading_note(k_key_C);
        int iN = Cadence::find_leading_tone_in_previous_chord(&prevChord, stepLeading);
        CHECK(iN == 1);
        if (iN != 1)
        {
            m_msg << "index to leading tone: " << iN;
            write_msg();
        }
    }

    TEST_FIXTURE(CadenceTestFixture, cadence_021)
    {
        //@020. find_leading_tone_in_previous_chord(). Not found

        CadenceChord prevChord("c2","e3","c4","g4");

        int stepLeading = KeyUtilities::get_step_for_leading_note(k_key_C);
        int iN = Cadence::find_leading_tone_in_previous_chord(&prevChord, stepLeading);
        CHECK( iN == -1 );
        if (iN != -1)
        {
            m_msg << "index to leading tone: " << iN;
            write_msg();
        }
    }

    TEST_FIXTURE(CadenceTestFixture, cadence_050)
    {
        //@050. check_chord(). Cadence ok

        //IV -> I
        CadenceChord prevChord("c3","f3","f4","a4");
        CadenceChord     chord("c3","g3","e4","c5");

        CHECK( dbg_check_chord(&chord, "I", &prevChord, k_key_C, true) );
        if (chord.nSeverity != k_chord_error_0_none)
        {
            m_msg << "Errors: " << endl << Cadence::get_all_errors_reason(chord.nSeverity);
            write_msg();
        }
    }

    TEST_FIXTURE(CadenceTestFixture, cadence_051)
    {
        //@051. Rule 1. Not all chord steps in the chord

        //IV -> I
        CadenceChord prevChord("c3","f3","f4","a4");
        CadenceChord     chord("c3","g3","c4","c5");

        CHECK( !dbg_check_chord(&chord, "I", &prevChord, k_key_C, true) );
        if (chord.nSeverity != k_chord_error_1_not_all_notes)
        {
            m_msg << "Errors: " << endl << Cadence::get_all_errors_reason(chord.nSeverity);
            write_msg();
        }
    }

    TEST_FIXTURE(CadenceTestFixture, cadence_053)
    {
        //@053. Rule 3. No parallel motion of perfect octaves, perfect fifths, and unisons

        //IV -> I
        CadenceChord prevChord("f2","f3","d4","a4");
        CadenceChord     chord("c3","c4","e4","g4");

        CHECK( !dbg_check_chord(&chord, "I", &prevChord, k_key_C, true) );
        if (chord.nSeverity != k_chord_error_3_fifth_octave_motion)
        {
            m_msg << "Errors: " << endl << Cadence::get_all_errors_reason(chord.nSeverity);
            write_msg();
        }
    }

    TEST_FIXTURE(CadenceTestFixture, cadence_055)
    {
        //@055. Rule 5. Fifth tone is not doubled

        //I -> V
        CadenceChord prevChord("e2","e3","c4","g4");
        CadenceChord     chord("d2","g3","d4","b4");

        CHECK( !dbg_check_chord(&chord, "V", &prevChord, k_key_C, true) );
        if (chord.nSeverity != k_chord_error_5_fifth_doubled)
        {
            m_msg << "Errors: " << endl << Cadence::get_all_errors_reason(chord.nSeverity);
            write_msg();
        }
    }

    TEST_FIXTURE(CadenceTestFixture, cadence_056)
    {
        //@056. Rule 6. Leading tone is not doubled

        //TODO: Is correct the failure k_chord_error_7_leading_resolution

        //V -> VII
        CadenceChord prevChord("d3","b3","g4","d5");
        CadenceChord     chord("d3","b3","b4","f5");

        CHECK( !dbg_check_chord(&chord, "VII", &prevChord, k_key_C, true) );
        if (chord.nSeverity != (k_chord_error_6_leading_doubled
                              | k_chord_error_7_leading_resolution) )
        {
            m_msg << "Errors: " << endl << Cadence::get_all_errors_reason(chord.nSeverity);
            write_msg();
        }
    }

    TEST_FIXTURE(CadenceTestFixture, cadence_057)
    {
        //@057. Rule 7. The leading tone should resolve to tonic

        //III -> I
        CadenceChord prevChord("e3","g3","g4","b4");
        CadenceChord     chord("c3","g3","e4","e5");

        CHECK( !dbg_check_chord(&chord, "I", &prevChord, k_key_C, true) );
        if (chord.nSeverity != k_chord_error_7_leading_resolution)
        {
            m_msg << "Errors: " << endl << Cadence::get_all_errors_reason(chord.nSeverity);
            write_msg();
        }
    }

    TEST_FIXTURE(CadenceTestFixture, cadence_059)
    {
        //@059. Rule 9. Intervals not greater than octave except bass-tenor

        //IV -> I
        CadenceChord prevChord("c3","f3","f4","a4");
        CadenceChord     chord("c3","e3","g4","c5");

        CHECK( !dbg_check_chord(&chord, "I", &prevChord, k_key_C, true) );
        if (chord.nSeverity != k_chord_error_9_greater_than_octave)
        {
            m_msg << "Errors: " << endl << Cadence::get_all_errors_reason(chord.nSeverity);
            write_msg();
        }
    }

    TEST_FIXTURE(CadenceTestFixture, cadence_060)
    {
        //@060. Rule 10. No voice crossing

        //IV -> I
        CadenceChord prevChord("c3","f3","f4","a4");
        CadenceChord     chord("c3","e4","c4","g4");

        CHECK( !dbg_check_chord(&chord, "I", &prevChord, k_key_C, true) );
        if (chord.nSeverity != k_chord_error_10_notes_not_ascending)
        {
            m_msg << "Errors: " << endl << Cadence::get_all_errors_reason(chord.nSeverity);
            write_msg();
        }
    }

    TEST_FIXTURE(CadenceTestFixture, cadence_061)
    {
        //@061. Rule 11. No voice overlap

        //IV -> I
        CadenceChord prevChord("c3","f3","c4","a4");
        CadenceChord     chord("c3","e4","g4","c5");

        CHECK( !dbg_check_chord(&chord, "I", &prevChord, k_key_C, true) );
        if (chord.nSeverity != k_chord_error_11_voice_overlap)
        {
            m_msg << "Errors: " << endl << Cadence::get_all_errors_reason(chord.nSeverity);
            write_msg();
        }
    }

    TEST_FIXTURE(CadenceTestFixture, cadence_062)
    {
        //@062. Rule 12. Chromatic accidentals

        //III -> V
        CadenceChord prevChord("e3","g3","e4","b4");
        CadenceChord     chord("d3","g3","g4","b4");

        CHECK( !dbg_check_chord(&chord, "V", &prevChord, k_key_c, true) );
        if (chord.nSeverity != k_chord_error_12_chromatic_acc)
        {
            m_msg << "Errors: " << endl << Cadence::get_all_errors_reason(chord.nSeverity);
            write_msg();
        }
    }

    TEST_FIXTURE(CadenceTestFixture, cadence_064)
    {
        //@064. Rule 14. If bass moves by step all other voices must move in opposite direction

        //IV -> I
        CadenceChord prevChord("f2","f3","d4","a4");
        CadenceChord     chord("e2","c4","e4","g4");

        CHECK( !dbg_check_chord(&chord, "I", &prevChord, k_key_C, true) );
        if (chord.nSeverity != k_chord_error_14_not_contrary_motion)
        {
            m_msg << "Errors: " << endl << Cadence::get_all_errors_reason(chord.nSeverity);
            write_msg();
        }
    }

};

