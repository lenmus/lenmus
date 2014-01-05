//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2014 LenMus project
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

////classes related to these tests
//#include "lenmus_injectors.h"
#include "lenmus_main_frame.h"

using namespace UnitTest;
using namespace std;
using namespace lenmus;


class MainFrameTestFixture
{
public:
//    ProgramScope m_programScope;
    std::string m_scores_path;

    MainFrameTestFixture()     //SetUp fixture
//        : m_programScope(cout)
    {
        m_scores_path = LENMUS_TEST_SCORES_PATH;
    }

    ~MainFrameTestFixture()    //TearDown fixture
    {
    }
};

SUITE(MainFrameTest)
{

    //TEST_FIXTURE(MainFrameTestFixture, TestTest)
    //{
    //    CHECK( true );
    //}

}


