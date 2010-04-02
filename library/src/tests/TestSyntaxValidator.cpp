//--------------------------------------------------------------------------------------
//  LenMus Library
//  Copyright (c) 2010 LenMus project
//
//  This program is free software; you can redistribute it and/or modify it under the 
//  terms of the GNU General Public License as published by the Free Software Foundation,
//  either version 3 of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//  PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License along
//  with this library; if not, see <http://www.gnu.org/licenses/> or write to the
//  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
//  MA  02111-1307,  USA.
//
//  For any comment, suggestion or feature request, please contact the manager of
//  the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------

#include <UnitTest++.h>
#include <iostream>

//classes related to these tests
#include "LdpParser.h"


using namespace UnitTest;
using namespace std;
using namespace lenmus;


class SyntaxValidatorTestFixture
{
public:

    SyntaxValidatorTestFixture()     //SetUp fixture
    {
    }

    ~SyntaxValidatorTestFixture()    //TearDown fixture
    {
    }
};

SUITE(SyntaxValidatorTest)
{
    TEST_FIXTURE(SyntaxValidatorTestFixture, ParserValidatesScore)
    {
        LdpParser parser(cout);
        SpLdpTree score = parser.parse_text("(score (language es utf-8))");
        //cout << score->get_root()->to_string() << endl;
        CHECK( score->get_root()->to_string() == "(score (language es utf-8))" );
    }

    TEST_FIXTURE(SyntaxValidatorTestFixture, SyntaxValidatorReplacesBadPitch)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        expected << "Line 0. Note. Bad pitch 'k5'. Replaced by 'c4'." << endl;
        SpLdpTree note = parser.parse_text("(n k5 e)");
        //cout << note->get_root()->to_string() << endl;
        CHECK( note->get_root()->to_string() == "(n c4 e)" );
        CHECK( errormsg.str() == expected.str() );
    }

    TEST_FIXTURE(SyntaxValidatorTestFixture, SyntaxValidatorReplacesBadDuration)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        SpLdpTree note = parser.parse_text("(n c4 a)");
        stringstream expected;
        expected << "Line 0. Bad note/rest duration 'a'. Replaced by 'q'." << endl;
        //cout << note->get_root()->to_string() << endl;
        CHECK( note->get_root()->to_string() == "(n c4 q)" );
        CHECK( errormsg.str() == expected.str() );
    }

    TEST_FIXTURE(SyntaxValidatorTestFixture, SyntaxValidatorRemovesUnknownElements)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        SpLdpTree note = parser.parse_text("(n c4 s (bubble big))");
        stringstream expected;
        expected << "Line 0. n: too many parameters. Extra parameters removed." << endl;
        //cout << note->get_root()->to_string() << endl;
        CHECK( note->get_root()->to_string() == "(n c4 s)" );
        CHECK( errormsg.str() == expected.str() );
    }

    TEST_FIXTURE(SyntaxValidatorTestFixture, SyntaxValidatorReplacesBadLabelValue)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        SpLdpTree clef = parser.parse_text("(clef H)");
        stringstream expected;
        expected << "Line 0. Bad value 'H'. Replaced by 'G'." << endl;
        //cout << clef->get_root()->to_string() << endl;
        CHECK( clef->get_root()->to_string() == "(clef G)" );
        CHECK( errormsg.str() == expected.str() );
    }

}

