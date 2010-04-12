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
#include "lenmus_parser.h"

//to delete singletons
#include "lenmus_factory.h"
#include "lenmus_elements.h"


using namespace UnitTest;
using namespace std;
using namespace lenmus;


class LdpParserTestFixture
{
public:

    LdpParserTestFixture()     //SetUp fixture
    {
    }

    ~LdpParserTestFixture()    //TearDown fixture
    {
        delete Factory::instance();
    }
};

SUITE(LdpParserTest)
{
    TEST_FIXTURE(LdpParserTestFixture, ParserReadTokensFromText)
    {
        LdpParser parser(cout);
        SpLdpTree score = parser.parse_text("(score (vers 1.7))");
        //cout << score->get_root()->to_string() << endl;
        CHECK( score->get_root()->to_string() == "(score (vers 1.7))" );
            //"(score (vers 1.6) (language en utf-8) (instrument (musicData )))" );
        delete score->get_root();
    }

    TEST_FIXTURE(LdpParserTestFixture, ParserReadScoreFromFile)
    {
        LdpParser parser(cout);
        SpLdpTree score = parser.parse_file("../../test-scores/00011-empty-fill-page.lms");
        //cout << score->get_root()->to_string() << endl;
        CHECK( score->get_root()->to_string() == "(score (vers 1.6) (language en iso-8859-1) (systemLayout first (systemMargins 0 0 0 2000)) (systemLayout other (systemMargins 0 0 1200 2000)) (opt Score.FillPageWithEmptyStaves true) (opt StaffLines.StopAtFinalBarline false) (instrument (musicData )))" );
        delete score->get_root();
    }

    TEST_FIXTURE(LdpParserTestFixture, ParserFileHasLineNumbers)
    {
        LdpParser parser(cout);
        SpLdpTree score = parser.parse_file("../../test-scores/00011-empty-fill-page.lms");
        if (score->get_root()->to_string() == "(score (vers 1.6) (language en iso-8859-1) (systemLayout first (systemMargins 0 0 0 2000)) (systemLayout other (systemMargins 0 0 1200 2000)) (opt Score.FillPageWithEmptyStaves true) (opt StaffLines.StopAtFinalBarline false) (instrument (musicData )))" )
        {
            LdpElement* elm = score->get_root();
            //cout << elm->get_name() << " in line " << elm->get_line_number() << endl;
            CHECK( elm->get_name() == "score" && elm->get_line_number() == 1 );
            elm = elm->get_first_child();
            CHECK( elm->get_name() == "vers" && elm->get_line_number() == 2 );
            elm = elm->get_next_sibling();
            elm = elm->get_next_sibling();
            CHECK( elm->get_name() == "systemLayout" && elm->get_line_number() == 3 );
            elm = elm->get_next_sibling();
            CHECK( elm->get_name() == "systemLayout" && elm->get_line_number() == 4 );
            elm = elm->get_next_sibling();
            CHECK( elm->get_name() == "opt" && elm->get_line_number() == 5 );
            elm = elm->get_next_sibling();
            CHECK( elm->get_name() == "opt" && elm->get_line_number() == 6 );
            elm = elm->get_next_sibling();
            CHECK( elm->get_name() == "instrument" && elm->get_line_number() == 7 );
        }
        else
            CHECK( false );

        delete score->get_root();
    }

    TEST_FIXTURE(LdpParserTestFixture, ParserReadScoreFromUnicodeFile)
    {
        LdpParser parser(cout);
        SpLdpTree score = parser.parse_file("../../test-scores/00002-unicode-text.lms");
        //cout << score->get_root()->to_string() << endl;
        CHECK( score->get_root()->to_string() == "(score (vers 1.6) (language en iso-8859-1) (systemLayout first (systemMargins 0 0 0 2000)) (systemLayout other (systemMargins 0 0 1200 2000)) (opt Score.FillPageWithEmptyStaves true) (opt StaffLines.StopAtFinalBarline false) (instrument (musicData (clef G) (n c4 q) (text \"Текст на кирилица\" (dx 15) (dy -10) (font normal 10)))))" );
        delete score->get_root();
    }

    TEST_FIXTURE(LdpParserTestFixture, ParserElemenWithId)
    {
        LdpParser parser(cout);
        SpLdpTree score = parser.parse_text("(clef#27 G)");
        //cout << score->get_root()->to_string() << endl;
        CHECK( score->get_root()->to_string() == "(clef G)" );
        CHECK( score->get_root()->GetID() == 27L );
        delete score->get_root();
    }

    TEST_FIXTURE(LdpParserTestFixture, ParserElemenWithoutId)
    {
        LdpParser parser(cout);
        SpLdpTree score = parser.parse_text("(clef G)");
        //cout << score->get_root()->to_string() << endl;
        CHECK( score->get_root()->to_string() == "(clef G)" );
        CHECK( score->get_root()->GetID() == 0L );
        delete score->get_root();
    }

    TEST_FIXTURE(LdpParserTestFixture, ParserElemenWithBadId)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        expected << "Line 0. Bad id in name 'clef#three'." << endl;
        SpLdpTree score = parser.parse_text("(clef#three G)");
        //cout << errormsg.str();
        //cout << expected.str();
        CHECK( score->get_root()->to_string() == "(clef G)" );
        CHECK( score->get_root()->GetID() == 0L );
        CHECK( errormsg.str() == expected.str() );
        delete score->get_root();
    }

}

