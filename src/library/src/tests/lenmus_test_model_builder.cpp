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
#include "lenmus_core_table.h"
#include "lenmus_parser.h"
#include "lenmus_analyser.h"
#include "lenmus_internal_model.h"
#include "lenmus_im_note.h"
#include "lenmus_document.h"
#include "lenmus_document_cursor.h"
#include "lenmus_score_iterator.h"

//to delete singletons
#include "lenmus_factory.h"
#include "lenmus_elements.h"


using namespace UnitTest;
using namespace std;
using namespace lenmus;


class StaffVoiceLineTableTestFixture
{
public:

    StaffVoiceLineTableTestFixture()     //SetUp fixture
    {
    }

    ~StaffVoiceLineTableTestFixture()    //TearDown fixture
    {
    }
};

SUITE(StaffVoiceLineTableTest)
{

    TEST_FIXTURE(StaffVoiceLineTableTestFixture, StaffVoiceLineTable_Voice0FirstStaff)
    {
        StaffVoiceLineTable table;
        CHECK( table.get_line_assigned_to(0, 0) == 0 );
    }

    TEST_FIXTURE(StaffVoiceLineTableTestFixture, StaffVoiceLineTable_Voice0Staff1)
    {
        StaffVoiceLineTable table;
        //cout << table.get_line_assigned_to(0, 1) << endl;
        CHECK( table.get_line_assigned_to(0, 1) == 1 );
    }

    TEST_FIXTURE(StaffVoiceLineTableTestFixture, StaffVoiceLineTable_FirstVoiceAssignedToDefault)
    {
        StaffVoiceLineTable table;
        table.get_line_assigned_to(0, 1);
        CHECK( table.get_line_assigned_to(3, 1) == 1 );
    }

}

class CoreTableTestFixture
{
public:

    CoreTableTestFixture()     //SetUp fixture
    {
        m_scores_path = "../../../../../test-scores/";
    }

    ~CoreTableTestFixture()    //TearDown fixture
    {
        delete Factory::instance();
    }

    std::string m_scores_path;
};

SUITE(CoreTableTest)
{

    TEST_FIXTURE(CoreTableTestFixture, CoreTableAddEntries)
    {
        Document doc;
        //doc.load(m_scores_path + "00020-space-before-clef.lms");
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (language en iso-8859-1) (instrument (musicData (n c4 q) (barline simple))))))" );
        DocIterator cursor(&doc);
        cursor.start_of_content();  //points to score
        CoreTableBuilder builder(&doc);
        CoreTable* pTable = builder.build_table(&cursor);
        CHECK( pTable->num_entries() == 2 );
        delete pTable;
    }

    TEST_FIXTURE(CoreTableTestFixture, ScoreIteratorPointsFirst)
    {
        Document doc;
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (language en iso-8859-1) (instrument (musicData (n c4 q) (barline simple))))))" );
        DocIterator cursor(&doc);
        cursor.start_of_content();  //points to score
        CoreTableBuilder builder(&doc);
        CoreTable* pTable = builder.build_table(&cursor);
        ScoreIterator it(pTable);
        //(*it)->dump();
        //cout << (*it)->to_string() << endl;
        CHECK( (*it)->to_string() == "(n c4 q)" );
        CHECK( (*it)->segment() == 0 );
        CHECK( (*it)->time() == 0.0f );
        delete pTable;
    }

    TEST_FIXTURE(CoreTableTestFixture, CoreTableFindInstruments)
    {
        Document doc;
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (language en iso-8859-1) (instrument (musicData (n c4 q) (barline simple))) (instrument (musicData (n c4 q) (barline simple))) )))" );
        DocIterator cursor(&doc);
        cursor.start_of_content();  //points to score
        ImScore* pScore = dynamic_cast<ImScore*>( (*cursor)->get_imobj() );
        CHECK( pScore->get_num_instruments() == 0 );
        CoreTableBuilder builder(&doc);
        CoreTable* pTable = builder.build_table(&cursor);
        CHECK( pScore->get_num_instruments() == 2 );
        delete pTable;
    }

    TEST_FIXTURE(CoreTableTestFixture, CoreTableChangeSegment)
    {
        Document doc;
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (language en iso-8859-1) (instrument (musicData (n c4 q) (barline simple) (n d4 e) (barline simple) (n e4 w))))))" );
        DocIterator cursor(&doc);
        cursor.start_of_content();  //points to score
        CoreTableBuilder builder(&doc);
        CoreTable* pTable = builder.build_table(&cursor);
        ScoreIterator it(pTable);
        //(*it)->dump();
        CHECK( pTable->num_entries() == 5 );
        CHECK( (*it)->to_string() == "(n c4 q)" );
        CHECK( (*it)->segment() == 0 );
        ++it;
        CHECK( (*it)->to_string() == "(barline simple)" );
        CHECK( (*it)->segment() == 0 );
        ++it;
        CHECK( (*it)->to_string() == "(n d4 e)" );
        CHECK( (*it)->segment() == 1 );
        ++it;
        CHECK( (*it)->to_string() == "(barline simple)" );
        CHECK( (*it)->segment() == 1 );
        ++it;
        CHECK( (*it)->to_string() == "(n e4 w)" );
        CHECK( (*it)->segment() == 2 );
        delete pTable;
    }

    TEST_FIXTURE(CoreTableTestFixture, CoreTableTimeInSequence)
    {
        Document doc;
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (musicData (n c4 q)(n d4 e.)(n d4 s)(n e4 h)))) ))" );
        DocIterator cursor(&doc);
        cursor.start_of_content();  //points to score
        CoreTableBuilder builder(&doc);
        CoreTable* pTable = builder.build_table(&cursor);
        ScoreIterator it(pTable);
        //pTable->dump();
        CHECK( pTable->num_entries() == 4 );
        CHECK( (*it)->to_string() == "(n c4 q)" );
        CHECK( (*it)->segment() == 0 );
        CHECK( (*it)->time() == 0.0f );
        ++it;
        CHECK( (*it)->to_string() == "(n d4 e.)" );
        CHECK( (*it)->segment() == 0 );
        CHECK( (*it)->time() == 64.0f );
        ++it;
        CHECK( (*it)->to_string() == "(n d4 s)" );
        CHECK( (*it)->segment() == 0 );
        CHECK( (*it)->time() == 112.0f );
        ++it;
        CHECK( (*it)->to_string() == "(n e4 h)" );
        CHECK( (*it)->segment() == 0 );
        CHECK( (*it)->time() == 128.0f );
        delete pTable;
    }

    TEST_FIXTURE(CoreTableTestFixture, CoreTableTimeGoBack)
    {
        Document doc;
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (musicData (n c4 q)(n d4 e.)(n d4 s)(goBack start)(n e4 h)(n g4 q)))) ))" );
        DocIterator cursor(&doc);
        cursor.start_of_content();  //points to score
        CoreTableBuilder builder(&doc);
        CoreTable* pTable = builder.build_table(&cursor);
        ScoreIterator it(pTable);
        //pTable->dump();
        CHECK( pTable->num_entries() == 5 );
        CHECK( (*it)->to_string() == "(n c4 q)" );
        CHECK( (*it)->segment() == 0 );
        CHECK( (*it)->time() == 0.0f );
        ++it;
        CHECK( (*it)->to_string() == "(n d4 e.)" );
        CHECK( (*it)->segment() == 0 );
        CHECK( (*it)->time() == 64.0f );
        ++it;
        CHECK( (*it)->to_string() == "(n d4 s)" );
        CHECK( (*it)->segment() == 0 );
        CHECK( (*it)->time() == 112.0f );
        ++it;
        CHECK( (*it)->to_string() == "(n e4 h)" );
        CHECK( (*it)->segment() == 0 );
        CHECK( (*it)->time() == 0.0f );
        ++it;
        CHECK( (*it)->to_string() == "(n g4 q)" );
        CHECK( (*it)->segment() == 0 );
        CHECK( (*it)->time() == 128.0f );
        delete pTable;
    }

    TEST_FIXTURE(CoreTableTestFixture, CoreTableTimeGoFwd)
    {
        Document doc;
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (musicData (n c4 q)(n d4 e.)(n d4 s)(goBack start)(n e4 q)(goFwd end)(barline)))) ))" );
        DocIterator cursor(&doc);
        cursor.start_of_content();  //points to score
        CoreTableBuilder builder(&doc);
        CoreTable* pTable = builder.build_table(&cursor);
        ScoreIterator it(pTable);
        //pTable->dump();
        CHECK( pTable->num_entries() == 5 );
        CHECK( (*it)->to_string() == "(n c4 q)" );
        CHECK( (*it)->time() == 0.0f );
        ++it;
        CHECK( (*it)->to_string() == "(n d4 e.)" );
        CHECK( (*it)->time() == 64.0f );
        ++it;
        CHECK( (*it)->to_string() == "(n d4 s)" );
        CHECK( (*it)->time() == 112.0f );
        ++it;
        CHECK( (*it)->to_string() == "(n e4 q)" );
        CHECK( (*it)->time() == 0.0f );
        ++it;
        CHECK( (*it)->to_string() == "(barline )" );
        CHECK( (*it)->time() == 128.0f );
        delete pTable;
    }

    TEST_FIXTURE(CoreTableTestFixture, CoreTableStaffAssigned)
    {
        Document doc;
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (musicData (n c4 q p2)(n d4 e.)(n d4 s p3)(n e4 h)))) ))" );
        DocIterator cursor(&doc);
        cursor.start_of_content();  //points to score
        CoreTableBuilder builder(&doc);
        CoreTable* pTable = builder.build_table(&cursor);
        ScoreIterator it(pTable);
        //pTable->dump();
        CHECK( pTable->num_entries() == 4 );
        CHECK( (*it)->to_string() == "(n c4 q p2)" );
        CHECK( (*it)->staff() == 1 );
        ++it;
        CHECK( (*it)->to_string() == "(n d4 e.)" );
        CHECK( (*it)->staff() == 1 );
        ++it;
        CHECK( (*it)->to_string() == "(n d4 s p3)" );
        CHECK( (*it)->staff() == 2 );
        ++it;
        CHECK( (*it)->to_string() == "(n e4 h)" );
        CHECK( (*it)->staff() == 2 );
        delete pTable;
    }

    TEST_FIXTURE(CoreTableTestFixture, CoreTableLineAssigned)
    {
        Document doc;
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (musicData (n c4 q v1)(n d4 e.)(n d4 s v3)(n e4 h)))) ))" );
        DocIterator cursor(&doc);
        cursor.start_of_content();  //points to score
        CoreTableBuilder builder(&doc);
        CoreTable* pTable = builder.build_table(&cursor);
        ScoreIterator it(pTable);
        //pTable->dump();
        CHECK( pTable->num_entries() == 4 );
        CHECK( (*it)->to_string() == "(n c4 q v1)" );
        CHECK( (*it)->line() == 0 );
        ++it;
        CHECK( (*it)->to_string() == "(n d4 e.)" );
        CHECK( (*it)->line() == 0 );
        ++it;
        CHECK( (*it)->to_string() == "(n d4 s v3)" );
        CHECK( (*it)->line() == 1 );
        ++it;
        CHECK( (*it)->to_string() == "(n e4 h)" );
        CHECK( (*it)->line() == 1 );
        delete pTable;
    }

    TEST_FIXTURE(CoreTableTestFixture, CoreTableAssigLineToClef)
    {
        Document doc;
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (musicData (clef G)(n c4 q v2)(n d4 e.)(n d4 s v3)(n e4 h)))) ))" );
        DocIterator cursor(&doc);
        cursor.start_of_content();  //points to score
        CoreTableBuilder builder(&doc);
        CoreTable* pTable = builder.build_table(&cursor);
        ScoreIterator it(pTable);
        //pTable->dump();
        CHECK( pTable->num_entries() == 5 );
        CHECK( (*it)->to_string() == "(clef G)" );
        CHECK( (*it)->line() == 0 );
        ++it;
        CHECK( (*it)->to_string() == "(n c4 q v2)" );
        CHECK( (*it)->line() == 0 );
        ++it;
        CHECK( (*it)->to_string() == "(n d4 e.)" );
        CHECK( (*it)->line() == 0 );
        ++it;
        CHECK( (*it)->to_string() == "(n d4 s v3)" );
        CHECK( (*it)->line() == 1 );
        ++it;
        CHECK( (*it)->to_string() == "(n e4 h)" );
        CHECK( (*it)->line() == 1 );
        delete pTable;
    }

    TEST_FIXTURE(CoreTableTestFixture, CoreTableAssigLineToKey)
    {
        Document doc;
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (staves 2)(musicData (clef G p1)(clef F4 p2)(key D)(n c4 q v2 p1)(n d4 e.)(n d4 s v3 p2)(n e4 h)))) ))" );
        DocIterator cursor(&doc);
        cursor.start_of_content();  //points to score
        CoreTableBuilder builder(&doc);
        CoreTable* pTable = builder.build_table(&cursor);
        ScoreIterator it(pTable);
        //pTable->dump();
        CHECK( pTable->num_entries() == 8 );
        CHECK( (*it)->to_string() == "(clef G p1)" );
        CHECK( (*it)->line() == 0 );
        ++it;
        CHECK( (*it)->to_string() == "(clef F4 p2)" );
        CHECK( (*it)->line() == 1 );
        ++it;
        CHECK( (*it)->to_string() == "(key D)" );
        CHECK( (*it)->line() == 0 );
        ++it;
        CHECK( (*it)->to_string() == "(key D)" );
        CHECK( (*it)->line() == 1 );
        ++it;
        CHECK( (*it)->to_string() == "(n c4 q v2 p1)" );
        CHECK( (*it)->line() == 0 );
        ++it;
        CHECK( (*it)->to_string() == "(n d4 e.)" );
        CHECK( (*it)->line() == 0 );
        ++it;
        CHECK( (*it)->to_string() == "(n d4 s v3 p2)" );
        CHECK( (*it)->line() == 1 );
        ++it;
        CHECK( (*it)->to_string() == "(n e4 h)" );
        CHECK( (*it)->line() == 1 );
        delete pTable;
    }

    TEST_FIXTURE(CoreTableTestFixture, CoreTableFullExample)
    {
        Document doc;
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6)"
                        "(instrument (staves 2)(musicData (clef G p1)(clef F4 p2)"
                        "(key D)(time 2 4)(n f4 w p1)(goBack w)(n c3 e g+ p2)"
                        "(n c3 e g-)(n d3 q)(barline)))"
                        "(instrument (staves 2)(musicData (clef G p1)(clef F4 p2)"
                        "(key D)(time 2 4)(n f4 q. p1)(clef F4 p1)(n a3 e)"
                        "(goBack h)(n c3 q p2)(n c3 e)(clef G p2)(clef F4 p2)"
                        "(n c3 e)(barline)))  )))" );
        DocIterator cursor(&doc);
        cursor.start_of_content();  //points to score
        CoreTableBuilder builder(&doc);
        CoreTable* pTable = builder.build_table(&cursor);
        ScoreIterator it(pTable);
        pTable->dump();
        //CHECK( pTable->num_entries() == 8 );
        //CHECK( (*it)->to_string() == "(clef G p1)" );
        //CHECK( (*it)->line() == 0 );
        //++it;
        //CHECK( (*it)->to_string() == "(clef F4 p2)" );
        //CHECK( (*it)->line() == 1 );
        //++it;
        //CHECK( (*it)->to_string() == "(key D)" );
        //CHECK( (*it)->line() == 0 );
        //++it;
        //CHECK( (*it)->to_string() == "(key D)" );
        //CHECK( (*it)->line() == 1 );
        //++it;
        //CHECK( (*it)->to_string() == "(n c4 q v2 p1)" );
        //CHECK( (*it)->line() == 0 );
        //++it;
        //CHECK( (*it)->to_string() == "(n d4 e.)" );
        //CHECK( (*it)->line() == 0 );
        //++it;
        //CHECK( (*it)->to_string() == "(n d4 s v3 p2)" );
        //CHECK( (*it)->line() == 1 );
        //++it;
        //CHECK( (*it)->to_string() == "(n e4 h)" );
        //CHECK( (*it)->line() == 1 );
        delete pTable;
    }

}


#endif  // _LM_DEBUG_

