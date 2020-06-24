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

#include <UnitTest++.h>
#include <sstream>
#include "lenmus_config.h"

//classes related to these tests
#include "lenmus_injectors.h"
#include "lenmus_paths.h"
#include "lenmus_string.h"
#include "lenmus_scores_constrains.h"
#include "lenmus_composer.h"

//lomse
#include <lomse_doorway.h>
#include <lomse_document.h>
using namespace lomse;

using namespace UnitTest;
using namespace std;
using namespace lenmus;


#define CHECK_MESSAGE(message) \
    UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__), (std::string(message)).c_str());

class ComposerTestFixture
{
public:
    ApplicationScope m_appScope;
    LibraryScope* m_pLibScope;
    string m_scores_path;
    stringstream m_msg;

    ComposerTestFixture()     //SetUp fixture
        : m_appScope(cout)
        , m_scores_path(LENMUS_TEST_SCORES_PATH)
    {
        LomseDoorway& door = m_appScope.get_lomse();
        m_pLibScope = door.get_library_scope();
    }

    ~ComposerTestFixture()    //TearDown fixture
    {
        m_msg.str("");
        m_msg << endl;
    }

    void write_msg()
    {
        m_msg << endl;
        CHECK_MESSAGE( m_msg.str() );
    }

    void set_constrains_for_lesson_18(ScoreConstrains& constrains)
    {
        //L1_MusicReading_7.xml
        constrains.SetClef(k_clef_G2, true);
        constrains.set_key_signature(k_key_C, true);
        constrains.set_time_signature(k_time_2_4, true);

        TimeSignConstrains* pTimes = LENMUS_NEW TimeSignConstrains();
        pTimes->SetValid(k_time_2_4, true);
        constrains.AddFragment(pTimes,
            "(n * q)(n * q),(n * e)(r e)(n * q)(r e),(n * e g+)(n * e)(n * e)(n * e g-),(n * q)(r q)");

        pTimes = LENMUS_NEW TimeSignConstrains();
        pTimes->SetValid(k_time_2_4, true);
        constrains.AddFragment(pTimes,
            "(n * q l)(n * e g+)(n * e g-),(n * q)(n * e)(r e),(n * e g+)(n * e)(n * e)(n * e g-),(n * h)");

        pTimes = LENMUS_NEW TimeSignConstrains();
        pTimes->SetValid(k_time_2_4, true);
        constrains.AddFragment(pTimes,
            "(n * q)(n * e)(r e),(n * q.)(n * e),(n * e)(r e)(n * e)(r e),(n * h)");

        pTimes = LENMUS_NEW TimeSignConstrains();
        pTimes->SetValid(k_time_2_4, true);
        constrains.AddFragment(pTimes,
            "(n * e)(r e)(n * e g+)(n * e g-),(n * e)(r e)(n * e g+)(n * e g-),(n * q)(n * q)");

        pTimes = LENMUS_NEW TimeSignConstrains();
        pTimes->SetValid(k_time_2_4, true);
        constrains.AddFragment(pTimes,
            "(n * e)(r e)(n * e g+)(n * e g-),(n * e)(r e)(n * e)(r e),(n * q)(r q)");

        pTimes = LENMUS_NEW TimeSignConstrains();
        pTimes->SetValid(k_time_2_4, true);
        constrains.AddFragment(pTimes,
            "(n * e)(r e)(n * e g+)(n * e g-),(n * e g+)(n * e)(n * e)(n * e g-),(n * q)(n * q),(n * h)");

        pTimes = LENMUS_NEW TimeSignConstrains();
        pTimes->SetValid(k_time_2_4, true);
        constrains.AddFragment(pTimes,
            "(n * e g+)(n * e)(n * e)(n * e g-),(n * q l)(n * e g+)(n * e g-),(n * e)(r e)(n * e)(r e),(n * q)(r q)");

        pTimes = LENMUS_NEW TimeSignConstrains();
        pTimes->SetValid(k_time_2_4, true);
        constrains.AddFragment(pTimes,
            "(r q)(n * q),(n * q l)(n * e g+)(n * e g-),(n * e g+)(n * e)(n * e)(n * e g-),(n * e g+)(n * e)(n * e)(n * e g-),(n * q l)(n * e g+)(n * e g-),(n * e)(r e)(n * e)(r e),(n * h)");
    }

};

SUITE(ComposerTest)
{

    TEST_FIXTURE(ComposerTestFixture, composer_1)
    {
        Document theDoc(*m_pLibScope);
        theDoc.create_empty();
        ADocument doc = theDoc.get_document_api();
        Composer composer(doc);

        ScoreConstrains constrains(m_appScope);
        set_constrains_for_lesson_18(constrains);
        constrains.SetClef(k_clef_G2, false);
        constrains.SetClef(k_clef_percussion, true);

        ImoScore* pScore = composer.generate_score(&constrains);

        CHECK( pScore != nullptr );
        CHECK( composer.get_score_clef() == k_clef_percussion );
//        m_msg << pScore->to_string();
//        write_msg();
    }


};

