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

#ifdef _LM_DEBUG_

#include <UnitTest++.h>
#include <iostream>

//classes related to these tests
#include "lenmus_parser.h"
#include "lenmus_syntax_validator.h"

//to delete singletons
#include "lenmus_factory.h"
#include "lenmus_elements.h"


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
        delete Factory::instance();
    }
};

SUITE(SyntaxValidatorTest)
{
    TEST_FIXTURE(SyntaxValidatorTestFixture, SyntaxValidatorValidatesScore)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        expected << "Line 0. score: missing mandatory element 'vers'." << endl;
        SpLdpTree tree = parser.parse_text("(score (language es utf-8))");
        SyntaxValidator v(tree, errormsg);
        v.validate(tree->get_root());  
        //cout << score->get_root()->to_string() << endl;
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( tree->get_root()->to_string() == "(score (language es utf-8))" );
        CHECK( errormsg.str() == expected.str() );
        delete tree->get_root();
    }

    TEST_FIXTURE(SyntaxValidatorTestFixture, SyntaxValidatorReplacesBadPitch)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        expected << "Line 0. Note. Bad pitch 'k5'. Replaced by 'c4'." << endl;
        SpLdpTree note = parser.parse_text("(n k5 e)");
        SyntaxValidator v(note, errormsg);
        v.validate(note->get_root());  
        //cout << note->get_root()->to_string() << endl;
        CHECK( note->get_root()->to_string() == "(n c4 e)" );
        CHECK( errormsg.str() == expected.str() );
        delete note->get_root();
    }

    TEST_FIXTURE(SyntaxValidatorTestFixture, SyntaxValidatorReplacesBadDuration)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        SpLdpTree note = parser.parse_text("(n c4 a)");
        stringstream expected;
        expected << "Line 0. Bad note/rest duration 'a'. Replaced by 'q'." << endl;
        SyntaxValidator v(note, errormsg);
        v.validate(note->get_root());  
        //cout << note->get_root()->to_string() << endl;
        CHECK( note->get_root()->to_string() == "(n c4 q)" );
        CHECK( errormsg.str() == expected.str() );
        delete note->get_root();
    }

    //TEST_FIXTURE(SyntaxValidatorTestFixture, SyntaxValidatorRemovesUnknownElements)
    //{
    //    stringstream errormsg;
    //    LdpParser parser(errormsg);
    //    LdpTree* note = parser.parse_text("(n c4 s (bubble big))");
    //    stringstream expected;
    //    expected << "Line 0. n: too many parameters. Extra parameters removed." << endl;
    //    SyntaxValidator v(note, errormsg);
    //    v.validate(note->get_root());  
    //    //cout << note->get_root()->to_string() << endl;
    //    CHECK( note->get_root()->to_string() == "(n c4 s)" );
    //    CHECK( errormsg.str() == expected.str() );
    //}

    TEST_FIXTURE(SyntaxValidatorTestFixture, SyntaxValidatorReplacesBadLabelValue)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        SpLdpTree clef = parser.parse_text("(clef H)");
        stringstream expected;
        expected << "Line 0. Bad value 'H'. Replaced by 'G'." << endl;
        SyntaxValidator v(clef, errormsg);
        v.validate(clef->get_root());  
        //cout << clef->get_root()->to_string() << endl;
        CHECK( clef->get_root()->to_string() == "(clef G)" );
        CHECK( errormsg.str() == expected.str() );
        delete clef->get_root();
    }

}

#endif  // _LM_DEBUG_

