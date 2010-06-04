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
#include <sstream>

//classes related to these tests
#include "lenmus_document.h"
#include "lenmus_document_iterator.h"

//to delete singletons
#include "lenmus_factory.h"
#include "lenmus_elements.h"


using namespace UnitTest;
using namespace std;
using namespace lenmus;


class DocIteratorTestFixture
{
public:

    DocIteratorTestFixture()     //SetUp fixture
    {
    }

    ~DocIteratorTestFixture()    //TearDown fixture
    {
        delete Factory::instance();
    }
};

SUITE(DocIteratorTest)
{
    TEST_FIXTURE(DocIteratorTestFixture, DocIteratorPointsToFirst)
    {
        Document doc;
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (language en iso-8859-1) (instrument (musicData (n c4 q) (r q)))) (text \"this is text\")))" );
        DocIterator it(&doc);
        Document::iterator dit = it.get_iterator();
        //cout << doc.to_string(dit) << endl;
        CHECK( doc.to_string(dit) == "(vers 0.0)" );
    }

    TEST_FIXTURE(DocIteratorTestFixture, DocIteratorDereferenceOperator)
    {
        Document doc;
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (language en iso-8859-1) (instrument (musicData (n c4 q) (r q)))) (text \"this is text\")))" );
        DocIterator it(&doc);
        //cout << (*it)->to_string() << endl;
        CHECK( (*it)->to_string() == "(vers 0.0)" );
    }

    TEST_FIXTURE(DocIteratorTestFixture, DocIteratorStartOfContent)
    {
        Document doc;
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (language en iso-8859-1) (instrument (musicData (n c4 q) (r q)))) (text \"this is text\")))" );
        DocIterator it(&doc);
        it.enter_element();
        //cout << (*it)->to_string() << endl;
        CHECK( (*it)->to_string() == "0.0" );
        it.start_of_content();
        //cout << (*it)->to_string() << endl;
        CHECK( (*it)->to_string() == "(score (vers 1.6) (instrument (musicData (n c4 q) (r q))))" );
    }

    TEST_FIXTURE(DocIteratorTestFixture, DocIteratorNext)
    {
        Document doc;
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (language en iso-8859-1) (instrument (musicData (n c4 q) (r q)))) (text \"this is text\")))" );
        ElementIterator it(&doc);
        CHECK( (*it)->to_string() == "(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (musicData (n c4 q) (r q)))) (text \"this is text\")))" );
        it.enter_element();
        CHECK( (*it)->to_string() == "(vers 0.0)" );
        ++it;
        //cout << (*it)->to_string() << endl;
        CHECK( (*it)->to_string() == "(content (score (vers 1.6) (instrument (musicData (n c4 q) (r q)))) (text \"this is text\"))" );
        ++it;
        CHECK( *it == NULL );
        ++it;
        CHECK( *it == NULL );
    }

    TEST_FIXTURE(DocIteratorTestFixture, DocIteratorPrev)
    {
        Document doc;
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (language en iso-8859-1) (instrument (musicData (n c4 q) (r q)))) (text \"this is text\")))" );
        DocIterator it(&doc);
        ++it;
        CHECK( (*it)->to_string() == "(content (score (vers 1.6) (instrument (musicData (n c4 q) (r q)))) (text \"this is text\"))" );
        --it;
        CHECK( (*it)->to_string() == "(vers 0.0)" );
        --it;
        CHECK( *it == NULL );
        --it;
        CHECK( *it == NULL );
    }

    TEST_FIXTURE(DocIteratorTestFixture, DocIteratorEnterElement)
    {
        Document doc;
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (language en iso-8859-1) (instrument (musicData (n c4 q) (r q)))) (text \"this is text\")))" );
        DocIterator it(&doc);
        //cout << (*it)->to_string() << endl;
        CHECK( (*it)->to_string() == "(vers 0.0)" );
        ++it;
        //cout << (*it)->to_string() << endl;
        CHECK( (*it)->to_string() == "(content (score (vers 1.6) (instrument (musicData (n c4 q) (r q)))) (text \"this is text\"))" );
        it.enter_element();
        CHECK( (*it)->to_string() == "(score (vers 1.6) (instrument (musicData (n c4 q) (r q))))" );
        it.enter_element();
        //cout << (*it)->to_string() << endl;
        CHECK( (*it)->to_string() == "(vers 1.6)" );
    }

    TEST_FIXTURE(DocIteratorTestFixture, DocIteratorExitElement)
    {
        Document doc;
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (language en iso-8859-1) (instrument (musicData (n c4 q) (r q)))) (text \"this is text\")))" );
        DocIterator it(&doc);
        ++it;
        it.enter_element();
        it.enter_element();
        ++it;
        CHECK( (*it)->to_string() == "(instrument (musicData (n c4 q) (r q)))" );
        it.exit_element();
        //cout << (*it)->to_string() << endl;
        CHECK( (*it)->to_string() == "(score (vers 1.6) (instrument (musicData (n c4 q) (r q))))" );
        it.exit_element();
        CHECK( (*it)->to_string() == "(content (score (vers 1.6) (instrument (musicData (n c4 q) (r q)))) (text \"this is text\"))" );
    }

    TEST_FIXTURE(DocIteratorTestFixture, DocIteratorLastOfContent)
    {
        Document doc;
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (language en iso-8859-1) (opt StaffLines.StopAtFinalBarline false) (instrument (musicData (n c4 q) (r q)))) (text \"this is text\")))" );
        DocIterator it(&doc);
        it.last_of_content();
        //cout << (*it)->to_string() << endl;
        CHECK( (*it)->to_string() == "(text \"this is text\")" );
    }

    TEST_FIXTURE(DocIteratorTestFixture, ScoreCursorNext)
    {
        Document doc;
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (language en iso-8859-1) (opt StaffLines.StopAtFinalBarline false) (instrument (musicData (n c4 q) (r q)))) (text \"this is text\")))" );
        DocIterator it(&doc);
        ++it;
        it.enter_element();
        //cout << (*it)->to_string() << endl;
        CHECK( (*it)->to_string() == "(score (vers 1.6) (opt StaffLines.StopAtFinalBarline false) (instrument (musicData (n c4 q) (r q))))" );
        it.enter_element();
        //cout << (*it)->to_string() << endl;
        CHECK( (*it)->to_string() == "(vers 1.6)" );
        ++it;
        CHECK( (*it)->to_string() == "(opt StaffLines.StopAtFinalBarline false)" );
        ++it;
        CHECK( (*it)->to_string() == "(instrument (musicData (n c4 q) (r q)))" );
        ++it;
        CHECK( *it == NULL );
        ++it;
        CHECK( *it == NULL );
    }

    TEST_FIXTURE(DocIteratorTestFixture, ScoreCursorPrev)
    {
        Document doc;
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (language en iso-8859-1) (opt StaffLines.StopAtFinalBarline false) (instrument (musicData (n c4 q) (r q)))) (text \"this is text\")))" );
        DocIterator it(&doc);
        ++it;
        it.enter_element();
        it.enter_element();
        ++it;
        ++it;
        CHECK( (*it)->to_string() == "(instrument (musicData (n c4 q) (r q)))" );
        --it;
        CHECK( (*it)->to_string() == "(opt StaffLines.StopAtFinalBarline false)" );
        --it;
        CHECK( (*it)->to_string() == "(vers 1.6)" );
        --it;
        CHECK( *it == NULL );
        --it;
        CHECK( *it == NULL );
    }

    TEST_FIXTURE(DocIteratorTestFixture, ScoreCursorPointToType)
    {
        Document doc;
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (language en iso-8859-1) (opt StaffLines.StopAtFinalBarline false) (instrument (musicData (n c4 q) (r q)))) (text \"this is text\")))" );
        DocIterator it(&doc);
        ++it;
        it.enter_element();
        it.enter_element();
        it.point_to(k_instrument);
        CHECK( (*it)->to_string() == "(instrument (musicData (n c4 q) (r q)))" );
        it.point_to(k_instrument);
        CHECK( (*it)->to_string() == "(instrument (musicData (n c4 q) (r q)))" );
        it.point_to(k_vers);
        CHECK( it.is_out_of_range() );
    }

    TEST_FIXTURE(DocIteratorTestFixture, ScoreCursorFindInstrument)
    {
        Document doc;
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (language en iso-8859-1) (opt StaffLines.StopAtFinalBarline false) (instrument (musicData (n c4 q) (r q))) (instrument (musicData (n a3 e)))) (text \"this is text\")))" );
        DocIterator it(&doc);
        ++it;
        it.enter_element();
        it.enter_element();
        it.find_instrument(0);
        CHECK( (*it)->to_string() == "(instrument (musicData (n c4 q) (r q)))" );
        it.find_instrument(1);
        CHECK( (*it)->to_string() == "(instrument (musicData (n a3 e)))" );
        it.find_instrument(2);
        CHECK( *it == NULL );
    }

    TEST_FIXTURE(DocIteratorTestFixture, ScoreCursorStartOfInstrument)
    {
        Document doc;
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (language en iso-8859-1) (opt StaffLines.StopAtFinalBarline false) (instrument (musicData (n c4 q) (r q))) (instrument (musicData (n a3 e)))) (text \"this is text\")))" );
        DocIterator it(&doc);
        ++it;
        it.enter_element();
        it.enter_element();
        it.start_of_instrument(0);
        CHECK( (*it)->to_string() == "(n c4 q)" );
        it.start_of_instrument(1);
        //cout << (*it)->to_string() << endl;
        CHECK( (*it)->to_string() == "(n a3 e)" );
        it.start_of_instrument(2);
        CHECK( *it == NULL );
    }

    TEST_FIXTURE(DocIteratorTestFixture, ScoreCursorIncrement)
    {
        Document doc;
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (language en iso-8859-1) (opt StaffLines.StopAtFinalBarline false) (instrument (musicData (n c4 q) (r q)))) (text \"this is text\")))" );
        DocIterator it(&doc);
        ++it;
        it.enter_element();
        it.enter_element();
        it.start_of_instrument(0);
        ++it;
        //cout << (*it)->to_string() << endl;
        CHECK( (*it)->to_string() == "(r q)" );
    }

    TEST_FIXTURE(DocIteratorTestFixture, ScoreCursorDecrement)
    {
        Document doc;
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (language en iso-8859-1) (opt StaffLines.StopAtFinalBarline false) (instrument (musicData (n c4 q) (r q)))) (text \"this is text\")))" );
        DocIterator it(&doc);
        ++it;
        it.enter_element();
        it.enter_element();
        it.start_of_instrument(0);
        ++it;
        --it;
        //cout << (*it)->to_string() << endl;
        CHECK( (*it)->to_string() == "(n c4 q)" );
    }

    //TEST_FIXTURE(DocIteratorTestFixture, ScoreCursorIsAtEnd)
    //{
    //    Document doc;
    //    doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (language en iso-8859-1) (opt StaffLines.StopAtFinalBarline false) (instrument (musicData ))) (text \"this is text\")))" );
    //    DocIterator it(&doc);
    //    ++it;       //point to score
    //    ++it;       //point to text
    //    CHECK( (*it)->to_string() == "(text \"this is text\")" );
    //}

}

#endif  // _LM_DEBUG_

