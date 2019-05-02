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
#include "lenmus_tokenizer.h"
#include "lenmus_string.h"

using namespace UnitTest;
using namespace std;
using namespace lenmus;


class TokenizerTestFixture
{
public:
    TokenizerTestFixture()     //SetUp fixture
    {
    }

    ~TokenizerTestFixture()    //TearDown fixture
    {
    }
};

SUITE(TokenizerTest)
{

    TEST_FIXTURE(TokenizerTestFixture, tokenizer_01)
    {
		//@01. only one token

		Tokenizer tk;

        CHECK( tk.initialize("patata", ", ") == "patata" );
        CHECK( tk.at_end() == true );
    }

    TEST_FIXTURE(TokenizerTestFixture, tokenizer_02)
    {
		//@02. splits at delimiters

		Tokenizer tk;

        CHECK( tk.initialize("this is,a test.", ", ") == "this" );
        CHECK( tk.at_end() == false );
        CHECK( tk.get_next_token() == "is" );
        CHECK( tk.at_end() == false );
        CHECK( tk.get_next_token() == "a" );
        CHECK( tk.at_end() == false );
        CHECK( tk.get_next_token() == "test." );
        CHECK( tk.at_end() == true );
    }

    TEST_FIXTURE(TokenizerTestFixture, tokenizer_03)
    {
		//@03. skips several delimiters

		Tokenizer tk;

        CHECK( tk.initialize("this is, a    test.,", ", ") == "this" );
        CHECK( tk.at_end() == false );
        CHECK( tk.get_next_token() == "is" );
        CHECK( tk.at_end() == false );
        CHECK( tk.get_next_token() == "a" );
        CHECK( tk.at_end() == false );
        CHECK( tk.get_next_token() == "test." );
        CHECK( tk.at_end() == true );
    }

    TEST_FIXTURE(TokenizerTestFixture, tokenizer_04)
    {
		//@04. empty data

		Tokenizer tk;

        CHECK( tk.initialize("", ", ") == "" );
        CHECK( tk.at_end() == true );
    }

};

