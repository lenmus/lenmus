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
#include "lenmus_injectors.h"
#include "lenmus_document.h"
#include "lenmus_document_iterator.h"
#include "lenmus_compiler.h"

using namespace UnitTest;
using namespace std;
using namespace lenmus;


class DocIteratorTestFixture
{
public:

    DocIteratorTestFixture()     //SetUp fixture
    {
        m_pLibraryScope = new LibraryScope(cout);
    }

    ~DocIteratorTestFixture()    //TearDown fixture
    {
        delete m_pLibraryScope;
    }

    LibraryScope* m_pLibraryScope;
};

SUITE(DocIteratorTest)
{
    TEST_FIXTURE(DocIteratorTestFixture, DocIteratorPointsToFirst)
    {
        Document doc(*m_pLibraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (language en iso-8859-1) (instrument (musicData (n c4 q) (r q)))) (text \"this is text\")))" );
        DocIterator it(&doc);
        Document::iterator dit = it.get_iterator();
        //cout << doc.to_string(dit) << endl;
        CHECK( doc.to_string(dit) == "(vers 0.0)" );
    }

    TEST_FIXTURE(DocIteratorTestFixture, DocIteratorDereferenceOperator)
    {
        Document doc(*m_pLibraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (language en iso-8859-1) (instrument (musicData (n c4 q) (r q)))) (text \"this is text\")))" );
        DocIterator it(&doc);
        //cout << (*it)->to_string() << endl;
        CHECK( (*it)->to_string() == "(vers 0.0)" );
    }

    TEST_FIXTURE(DocIteratorTestFixture, DocIteratorStartOfContent)
    {
        Document doc(*m_pLibraryScope);
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
        Document doc(*m_pLibraryScope);
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
        Document doc(*m_pLibraryScope);
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
        Document doc(*m_pLibraryScope);
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
        Document doc(*m_pLibraryScope);
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
        Document doc(*m_pLibraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (language en iso-8859-1) (opt StaffLines.StopAtFinalBarline false) (instrument (musicData (n c4 q) (r q)))) (text \"this is text\")))" );
        DocIterator it(&doc);
        it.last_of_content();
        //cout << (*it)->to_string() << endl;
        CHECK( (*it)->to_string() == "(text \"this is text\")" );
    }

    TEST_FIXTURE(DocIteratorTestFixture, ScoreElmIteratorNext)
    {
        Document doc(*m_pLibraryScope);
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

    TEST_FIXTURE(DocIteratorTestFixture, ScoreElmIteratorPrev)
    {
        Document doc(*m_pLibraryScope);
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

    TEST_FIXTURE(DocIteratorTestFixture, ScoreElmIteratorPointToType)
    {
        Document doc(*m_pLibraryScope);
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

    TEST_FIXTURE(DocIteratorTestFixture, ScoreElmIteratorFindInstrument)
    {
        Document doc(*m_pLibraryScope);
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

    TEST_FIXTURE(DocIteratorTestFixture, ScoreElmIteratorStartOfInstrument)
    {
        Document doc(*m_pLibraryScope);
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

    TEST_FIXTURE(DocIteratorTestFixture, ScoreElmIteratorIncrement)
    {
        Document doc(*m_pLibraryScope);
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

    TEST_FIXTURE(DocIteratorTestFixture, ScoreElmIteratorDecrement)
    {
        Document doc(*m_pLibraryScope);
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

}

#endif  // _LM_DEBUG_

