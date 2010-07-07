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
#include "lenmus_document_cursor.h"
#include "lenmus_compiler.h"
#include "lenmus_time.h"

using namespace UnitTest;
using namespace std;
using namespace lenmus;


class ScoreCursorTestFixture
{
public:

    ScoreCursorTestFixture()     //SetUp fixture
    {
        m_scores_path = "../../../../test-scores/";
        m_pLibraryScope = new LibraryScope(cout);
    }

    ~ScoreCursorTestFixture()    //TearDown fixture
    {
        delete m_pLibraryScope;
    }

    void dump_cursor(DocCursor& cursor)
    {
        cout << endl;
        if (cursor.is_at_end_of_child())
            cout << "At end";
        else
            cout << (*cursor)->to_string() << ". Id=" << (*cursor)->get_id();  

        cout << ", time=" << cursor.timepos() << ", instr=" << cursor.instrument()
        << ", staff=" << cursor.staff() << ", segment=" << cursor.segment()
        << ", pointing-obj=" << (cursor.is_pointing_object() ? "yes" : "no") << endl;
    }

    LibraryScope* m_pLibraryScope;
    std::string m_scores_path;
};

SUITE(ScoreCursorTest)
{

    //----------------------------------------------------------------------------
    // ScoreCursor::start() --------------------------------------------------
    //----------------------------------------------------------------------------

    TEST_FIXTURE(ScoreCursorTestFixture, ScoreCursorStart)
    {
        //start: initially in first instrument, first staff, after prolog,
        //timepos 0, segment 0
        Document doc(*m_pLibraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (language en iso-8859-1) (instrument (musicData (n c4 q) (r q)))) (text \"this is text\")))" );
        DocCursor cursor(&doc);
        cursor.enter_element();
        CHECK( (*cursor)->to_string() == "(n c4 q)" );
        CHECK( cursor.instrument() == 0 );
        CHECK( cursor.segment() == 0 );
        CHECK( cursor.staff() == 0 );
        CHECK( is_equal_time(cursor.timepos(), 0.0f) );
    }

    //----------------------------------------------------------------------------
    // ScoreCursor::point_to() --------------------------------------------------
    //----------------------------------------------------------------------------

    TEST_FIXTURE(ScoreCursorTestFixture, ScoreCursorPointTo)
    {
        Document doc(*m_pLibraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (musicData (n c4 q) (r q)))) (text \"this is text\")))" );
        DocCursor cursor(&doc);
        cursor.enter_element();
        cursor.point_to(8L);
        CHECK( (*cursor)->to_string() == "(r q)" );
        CHECK( cursor.instrument() == 0 );
        CHECK( cursor.segment() == 0 );
        CHECK( cursor.staff() == 0 );
        CHECK( is_equal_time(cursor.timepos(), 64.0f) );
    }

    TEST_FIXTURE(ScoreCursorTestFixture, ScoreCursorPointTo_NotFound)
    {
        Document doc(*m_pLibraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (musicData (n c4 q) (r q)))) (text \"this is text\")))" );
        DocCursor cursor(&doc);
        cursor.enter_element();
        cursor.point_to(2L);
        CHECK( *cursor == NULL );
    }

    TEST_FIXTURE(ScoreCursorTestFixture, ScoreCursorPointToBarline)
    {
        Document doc(*m_pLibraryScope);
        doc.from_file(m_scores_path + "90013-two-instruments-four-staves.lms" );
        DocCursor cursor(&doc);
        cursor.enter_element();
        cursor.point_to_barline(41L, 1);
        CHECK( cursor.is_pointing_object() == true );
        CHECK( (*cursor)->get_id() == 41L );
        CHECK( (*cursor)->to_string() == "(barline )" );
        CHECK( cursor.instrument() == 1 );
        CHECK( cursor.segment() == 0 );
        CHECK( cursor.staff() == 1 );
        CHECK( is_equal_time(cursor.timepos(), 128.0f) );
    }

    //----------------------------------------------------------------------------
    // ScoreCursor::move_next() --------------------------------------------------
    //----------------------------------------------------------------------------

    TEST_FIXTURE(ScoreCursorTestFixture, ScoreCursorMoveNext_1)
    {
        //1. to next object. must skip objects in other instruments 
        Document doc(*m_pLibraryScope);
        doc.from_file(m_scores_path + "90013-two-instruments-four-staves.lms" );
        DocCursor cursor(&doc);
        cursor.enter_element();
        cursor.point_to(13L);
        cursor.move_next();
        //dump_cursor(cursor);
        CHECK( cursor.is_pointing_object() == true );
        CHECK( (*cursor)->get_id() == 14L );
        CHECK( cursor.instrument() == 0 );
        CHECK( cursor.segment() == 0 );
        CHECK( cursor.staff() == 0 );
        CHECK( is_equal_time(cursor.timepos(), 96.0f) );
    }

    TEST_FIXTURE(ScoreCursorTestFixture, ScoreCursorMoveNext_2)
    {
        //2. to next time. No object
        Document doc(*m_pLibraryScope);
        doc.from_file(m_scores_path + "90013-two-instruments-four-staves.lms" );
        DocCursor cursor(&doc);
        cursor.enter_element();
        cursor.point_to(33L);
        cursor.move_next();
        //dump_cursor(cursor);
        CHECK( cursor.is_pointing_object() == false );
        CHECK( (*cursor)->get_id() == 39L );
        CHECK( cursor.instrument() == 1 );
        CHECK( cursor.segment() == 0 );
        CHECK( cursor.staff() == 0 );
        CHECK( is_equal_time(cursor.timepos(), 64.0f) );
    }

    TEST_FIXTURE(ScoreCursorTestFixture, ScoreCursorMoveNext_3)
    {
        //3. to next object. Must find right staff 
        Document doc(*m_pLibraryScope);
        doc.from_file(m_scores_path + "90013-two-instruments-four-staves.lms" );
        DocCursor cursor(&doc);
        cursor.enter_element();
        cursor.point_to(17L);
        cursor.move_next();
        //dump_cursor(cursor);
        CHECK( cursor.is_pointing_object() == true );
        CHECK( (*cursor)->get_id() == 18L );
        CHECK( cursor.instrument() == 0 );
        CHECK( cursor.segment() == 0 );
        CHECK( cursor.staff() == 1 );
        CHECK( is_equal_time(cursor.timepos(), 64.0f) );
    }

    TEST_FIXTURE(ScoreCursorTestFixture, ScoreCursorMoveNext_4)
    {
        //4. to next empty time
        Document doc(*m_pLibraryScope);
        doc.from_file(m_scores_path + "90013-two-instruments-four-staves.lms" );
        DocCursor cursor(&doc);
        cursor.enter_element();
        cursor.point_to(33L);
        cursor.move_next();
        cursor.move_next();
        //dump_cursor(cursor);
        CHECK( cursor.is_pointing_object() == false );
        CHECK( (*cursor)->get_id() == 40L );
        CHECK( cursor.instrument() == 1 );
        CHECK( cursor.segment() == 0 );
        CHECK( cursor.staff() == 0 );
        CHECK( is_equal_time(cursor.timepos(), 96.0f) );
    }

    TEST_FIXTURE(ScoreCursorTestFixture, ScoreCursorMoveNext_5)
    {
        //5. traversing a sequence of notes
        Document doc(*m_pLibraryScope);
        doc.from_file(m_scores_path + "90013-two-instruments-four-staves.lms" );
        DocCursor cursor(&doc);
        cursor.enter_element();
        cursor.point_to(35L);

        cursor.move_next();
        CHECK( (*cursor)->get_id() == 36L );
        CHECK( cursor.is_pointing_object() == true );
        CHECK( cursor.instrument() == 1 );
        CHECK( cursor.segment() == 0 );
        CHECK( cursor.staff() == 1 );
        CHECK( is_equal_time(cursor.timepos(), 16.0f) );

        cursor.move_next();
        CHECK( (*cursor)->get_id() == 37L );
        CHECK( cursor.is_pointing_object() == true );
        CHECK( cursor.instrument() == 1 );
        CHECK( cursor.segment() == 0 );
        CHECK( cursor.staff() == 1 );
        CHECK( is_equal_time(cursor.timepos(), 32.0f) );

        cursor.move_next();
        CHECK( (*cursor)->get_id() == 38L );
        CHECK( cursor.is_pointing_object() == true );
        CHECK( cursor.instrument() == 1 );
        CHECK( cursor.segment() == 0 );
        CHECK( cursor.staff() == 1 );
        CHECK( is_equal_time(cursor.timepos(), 48.0f) );

        cursor.move_next();
        CHECK( (*cursor)->get_id() == 39L );
        CHECK( cursor.is_pointing_object() == true );
        CHECK( cursor.instrument() == 1 );
        CHECK( cursor.segment() == 0 );
        CHECK( cursor.staff() == 1 );
        CHECK( is_equal_time(cursor.timepos(), 64.0f) );
    }

    TEST_FIXTURE(ScoreCursorTestFixture, ScoreCursorMoveNext_6)
    {
        //6. cross barline
        Document doc(*m_pLibraryScope);
        doc.from_file(m_scores_path + "90013-two-instruments-four-staves.lms" );
        DocCursor cursor(&doc);
        cursor.enter_element();
        cursor.point_to_barline(41L, 1);
        cursor.move_next();
        //dump_cursor(cursor);
        CHECK( cursor.is_pointing_object() == true );
        CHECK( (*cursor)->get_id() == 46L );
        CHECK( cursor.instrument() == 1 );
        CHECK( cursor.segment() == 1 );
        CHECK( cursor.staff() == 1 );
        CHECK( is_equal_time(cursor.timepos(), 0.0f) );
    }

    TEST_FIXTURE(ScoreCursorTestFixture, ScoreCursorMoveNext_7)
    {
        //7. cross last barline (move to end of staff)
        Document doc(*m_pLibraryScope);
        doc.from_file(m_scores_path + "90013-two-instruments-four-staves.lms" );
        DocCursor cursor(&doc);
        cursor.enter_element();
        cursor.point_to_barline(25L, 1);
        cursor.move_next();
        //dump_cursor(cursor);
        CHECK( cursor.is_pointing_object() == false );
        CHECK( cursor.instrument() == 0 );
        CHECK( cursor.segment() == 2 );
        CHECK( cursor.staff() == 1 );
        CHECK( is_equal_time(cursor.timepos(), 0.0f) );
    }

    TEST_FIXTURE(ScoreCursorTestFixture, ScoreCursorMoveNext_8)
    {
        //8. from end of staff to next staff in same instrument
        Document doc(*m_pLibraryScope);
        doc.from_file(m_scores_path + "90013-two-instruments-four-staves.lms" );
        DocCursor cursor(&doc);
        cursor.enter_element();
        cursor.point_to_barline(25L, 0);
        cursor.move_next();     //move to end of staff
        cursor.move_next();     //now at start of next staff
        //dump_cursor(cursor);
        CHECK( cursor.is_pointing_object() == true );
        CHECK( (*cursor)->get_id() == 9L );
        CHECK( cursor.instrument() == 0 );
        CHECK( cursor.segment() == 0 );
        CHECK( cursor.staff() == 1 );
        CHECK( is_equal_time(cursor.timepos(), 0.0f) );
    }

    TEST_FIXTURE(ScoreCursorTestFixture, ScoreCursorMoveNext_9)
    {
        //9. from end of staff to next instrument
        Document doc(*m_pLibraryScope);
        doc.from_file(m_scores_path + "90013-two-instruments-four-staves.lms" );
        DocCursor cursor(&doc);
        cursor.enter_element();
        cursor.point_to_barline(25L, 1);
        cursor.move_next();     //move to end of staff
        cursor.move_next();     //now at start of next instrument
        //dump_cursor(cursor);
        CHECK( cursor.is_pointing_object() == true );
        CHECK( (*cursor)->get_id() == 29L );
        CHECK( cursor.instrument() == 1 );
        CHECK( cursor.segment() == 0 );
        CHECK( cursor.staff() == 0 );
        CHECK( is_equal_time(cursor.timepos(), 0.0f) );
    }

    TEST_FIXTURE(ScoreCursorTestFixture, ScoreCursorMoveNext_10)
    {
        //10. at end of score: remains there
        Document doc(*m_pLibraryScope);
        doc.from_file(m_scores_path + "90013-two-instruments-four-staves.lms" );
        DocCursor cursor(&doc);
        cursor.enter_element();
        cursor.point_to_barline(48L, 1);
        cursor.move_next();     //move to end of score
        cursor.move_next();     //try to move after end of score
        //dump_cursor(cursor);
        CHECK( cursor.is_pointing_object() == false );
        CHECK( cursor.instrument() == 1 );
        CHECK( cursor.segment() == 2 );
        CHECK( cursor.staff() == 1 );
        CHECK( is_equal_time(cursor.timepos(), 0.0f) );
    }

    TEST_FIXTURE(ScoreCursorTestFixture, ScoreCursorMoveNext_11)
    {
        //11. last note. move forward to end of score
        Document doc(*m_pLibraryScope);
        doc.from_string("(lenmusdoc#0 (vers#1 0.0) (content#2 (score#3 (vers#4 1.6) (instrument#5 (musicData#6 (clef#7 G) (n#8 c4 q))))))" );
        DocCursor cursor(&doc);
        cursor.enter_element();
        cursor.point_to(8L);
        cursor.move_next();     //move to end of score
        //dump_cursor(cursor);
        CHECK( cursor.is_pointing_object() == false );
        CHECK( cursor.instrument() == 0 );
        CHECK( cursor.segment() == 0 );
        CHECK( cursor.staff() == 0 );
        CHECK( is_equal_time(cursor.timepos(), 64.0f) );
    }

    TEST_FIXTURE(ScoreCursorTestFixture, ScoreCursorMoveNext_12)
    {
        //12. at end of score: remains there
        Document doc(*m_pLibraryScope);
        doc.from_string("(lenmusdoc#0 (vers#1 0.0) (content#2 (score#3 (vers#4 1.6) (instrument#5 (musicData#6 (clef#7 G) (n#8 c4 q))))))" );
        DocCursor cursor(&doc);
        cursor.enter_element();
        cursor.point_to(8L);
        cursor.move_next();     //move to end of score
        cursor.move_next();     //try to move after end of score
        //dump_cursor(cursor);
        CHECK( cursor.is_pointing_object() == false );
        CHECK( cursor.instrument() == 0 );
        CHECK( cursor.segment() == 0 );
        CHECK( cursor.staff() == 0 );
        CHECK( is_equal_time(cursor.timepos(), 64.0f) );
    }

    TEST_FIXTURE(ScoreCursorTestFixture, ScoreCursorMoveNext_13)
    {
        //13. advance to empty time
        Document doc(*m_pLibraryScope);
        doc.from_file(m_scores_path + "90013-two-instruments-four-staves.lms" );
        DocCursor cursor(&doc);
        cursor.enter_element();
        cursor.point_to(42L);
        cursor.move_next();
        //dump_cursor(cursor);
        CHECK( cursor.is_pointing_object() == false );
        CHECK( cursor.instrument() == 1 );
        CHECK( cursor.segment() == 1 );
        CHECK( cursor.staff() == 0 );
        CHECK( is_equal_time(cursor.timepos(), 48.0f) );
    }

    TEST_FIXTURE(ScoreCursorTestFixture, ScoreCursorMoveNext_14)
    {
        //14. from empty time to existing time
        Document doc(*m_pLibraryScope);
        doc.from_file(m_scores_path + "90013-two-instruments-four-staves.lms" );
        DocCursor cursor(&doc);
        cursor.enter_element();
        cursor.point_to(42L);
        cursor.move_next();
        cursor.move_next();
        //dump_cursor(cursor);
        CHECK( cursor.is_pointing_object() == false );
        CHECK( cursor.instrument() == 1 );
        CHECK( cursor.segment() == 1 );
        CHECK( cursor.staff() == 0 );
        CHECK( is_equal_time(cursor.timepos(), 64.0f) );
    }

    TEST_FIXTURE(ScoreCursorTestFixture, ScoreCursorMoveNext_15)
    {
        //15. to next object at same time: c29 -> k31 -> t32 -> n33
        Document doc(*m_pLibraryScope);
        doc.from_file(m_scores_path + "90013-two-instruments-four-staves.lms" );
        DocCursor cursor(&doc);
        cursor.enter_element();
        cursor.point_to(29L);

        cursor.move_next();
        CHECK( cursor.is_pointing_object() == true );
        CHECK( (*cursor)->get_id() == 31L );
        CHECK( cursor.instrument() == 1 );
        CHECK( cursor.segment() == 0 );
        CHECK( cursor.staff() == 0 );
        CHECK( is_equal_time(cursor.timepos(), 0.0f) );

        cursor.move_next();
        CHECK( cursor.is_pointing_object() == true );
        CHECK( (*cursor)->get_id() == 32L );
        CHECK( cursor.instrument() == 1 );
        CHECK( cursor.segment() == 0 );
        CHECK( cursor.staff() == 0 );
        CHECK( is_equal_time(cursor.timepos(), 0.0f) );

        cursor.move_next();
        CHECK( cursor.is_pointing_object() == true );
        CHECK( (*cursor)->get_id() == 33L );
        CHECK( cursor.instrument() == 1 );
        CHECK( cursor.segment() == 0 );
        CHECK( cursor.staff() == 0 );
        CHECK( is_equal_time(cursor.timepos(), 0.0f) );
    }

    //----------------------------------------------------------------------------
    // ScoreCursor::move_prev() --------------------------------------------------
    //----------------------------------------------------------------------------

    TEST_FIXTURE(ScoreCursorTestFixture, ScoreCursorMovePrev_1)
    {
        //1. to prev object. must skip objects in other instruments 
        Document doc(*m_pLibraryScope);
        doc.from_file(m_scores_path + "90013-two-instruments-four-staves.lms" );
        DocCursor cursor(&doc);
        cursor.enter_element();
        cursor.point_to(14L);
        cursor.move_prev();
        //dump_cursor(cursor);
        CHECK( cursor.is_pointing_object() == true );
        CHECK( (*cursor)->get_id() == 13L );
        CHECK( cursor.instrument() == 0 );
        CHECK( cursor.segment() == 0 );
        CHECK( cursor.staff() == 0 );
        CHECK( is_equal_time(cursor.timepos(), 64.0f) );
    }

    TEST_FIXTURE(ScoreCursorTestFixture, ScoreCursorMovePrev_2)
    {
        //2. to prev time. No object
        Document doc(*m_pLibraryScope);
        doc.from_file(m_scores_path + "90013-two-instruments-four-staves.lms" );
        DocCursor cursor(&doc);
        cursor.enter_element();
        cursor.point_to_barline(41L, 0);
        cursor.move_prev();
        //dump_cursor(cursor);
        CHECK( cursor.is_pointing_object() == false );
        CHECK( (*cursor)->get_id() == 40L );
        CHECK( cursor.instrument() == 1 );
        CHECK( cursor.segment() == 0 );
        CHECK( cursor.staff() == 0 );
        CHECK( is_equal_time(cursor.timepos(), 96.0f) );
    }

    TEST_FIXTURE(ScoreCursorTestFixture, ScoreCursorMovePrev_3)
    {
        //3. to prev object. Must find right staff 
        Document doc(*m_pLibraryScope);
        doc.from_file(m_scores_path + "90013-two-instruments-four-staves.lms" );
        DocCursor cursor(&doc);
        cursor.enter_element();
        cursor.point_to(18L);
        cursor.move_prev();
        //dump_cursor(cursor);
        CHECK( cursor.is_pointing_object() == true );
        CHECK( (*cursor)->get_id() == 17L );
        CHECK( cursor.instrument() == 0 );
        CHECK( cursor.segment() == 0 );
        CHECK( cursor.staff() == 1 );
        CHECK( is_equal_time(cursor.timepos(), 32.0f) );
    }

    TEST_FIXTURE(ScoreCursorTestFixture, ScoreCursorMovePrev_4)
    {
        //4. to prev empty time
        Document doc(*m_pLibraryScope);
        doc.from_file(m_scores_path + "90013-two-instruments-four-staves.lms" );
        DocCursor cursor(&doc);
        cursor.enter_element();
        cursor.point_to_barline(41L, 0);
        cursor.move_prev();     //move to 40L
        cursor.move_prev();
        //dump_cursor(cursor);
        CHECK( cursor.is_pointing_object() == false );
        CHECK( (*cursor)->get_id() == 39L );
        CHECK( cursor.instrument() == 1 );
        CHECK( cursor.segment() == 0 );
        CHECK( cursor.staff() == 0 );
        CHECK( is_equal_time(cursor.timepos(), 64.0f) );
    }

    TEST_FIXTURE(ScoreCursorTestFixture, ScoreCursorMovePrev_5)
    {
        //5. traversing a sequence of notes
        Document doc(*m_pLibraryScope);
        doc.from_file(m_scores_path + "90013-two-instruments-four-staves.lms" );
        DocCursor cursor(&doc);
        cursor.enter_element();
        cursor.point_to(39L);

        cursor.move_prev();
        CHECK( (*cursor)->get_id() == 38L );
        CHECK( cursor.is_pointing_object() == true );
        CHECK( cursor.instrument() == 1 );
        CHECK( cursor.segment() == 0 );
        CHECK( cursor.staff() == 1 );
        CHECK( is_equal_time(cursor.timepos(), 48.0f) );

        cursor.move_prev();
        CHECK( (*cursor)->get_id() == 37L );
        CHECK( cursor.is_pointing_object() == true );
        CHECK( cursor.instrument() == 1 );
        CHECK( cursor.segment() == 0 );
        CHECK( cursor.staff() == 1 );
        CHECK( is_equal_time(cursor.timepos(), 32.0f) );

        cursor.move_prev();
        CHECK( (*cursor)->get_id() == 36L );
        CHECK( cursor.is_pointing_object() == true );
        CHECK( cursor.instrument() == 1 );
        CHECK( cursor.segment() == 0 );
        CHECK( cursor.staff() == 1 );
        CHECK( is_equal_time(cursor.timepos(), 16.0f) );
    }

    TEST_FIXTURE(ScoreCursorTestFixture, ScoreCursorMovePrev_6)
    {
        //6. to last object at same time (must skip clef)
        Document doc(*m_pLibraryScope);
        doc.from_file(m_scores_path + "90013-two-instruments-four-staves.lms" );
        DocCursor cursor(&doc);
        cursor.enter_element();
        cursor.point_to(36L);
        cursor.move_prev();
        //dump_cursor(cursor);
        CHECK( (*cursor)->get_id() == 35L );
        CHECK( cursor.is_pointing_object() == true );
        CHECK( cursor.instrument() == 1 );
        CHECK( cursor.segment() == 0 );
        CHECK( cursor.staff() == 1 );
        CHECK( is_equal_time(cursor.timepos(), 0.0f) );
    }

    TEST_FIXTURE(ScoreCursorTestFixture, ScoreCursorMovePrev_7)
    {
        //7. to prev object at same time
        Document doc(*m_pLibraryScope);
        doc.from_file(m_scores_path + "90013-two-instruments-four-staves.lms" );
        DocCursor cursor(&doc);
        cursor.enter_element();
        cursor.point_to(35L);
        cursor.move_prev();
        CHECK( (*cursor)->get_id() == 31L );
        CHECK( cursor.is_pointing_object() == true );
        CHECK( cursor.instrument() == 1 );
        CHECK( cursor.segment() == 0 );
        CHECK( cursor.staff() == 1 );
        CHECK( is_equal_time(cursor.timepos(), 0.0f) );

        cursor.move_prev();
        //dump_cursor(cursor);
        CHECK( (*cursor)->get_id() == 30L );
        CHECK( cursor.is_pointing_object() == true );
        CHECK( cursor.instrument() == 1 );
        CHECK( cursor.segment() == 0 );
        CHECK( cursor.staff() == 1 );
        CHECK( is_equal_time(cursor.timepos(), 0.0f) );
    }

    TEST_FIXTURE(ScoreCursorTestFixture, ScoreCursorMovePrev_8)
    {
        //8. cross barline
        Document doc(*m_pLibraryScope);
        doc.from_file(m_scores_path + "90013-two-instruments-four-staves.lms" );
        DocCursor cursor(&doc);
        cursor.enter_element();
        cursor.point_to(46L);
        cursor.move_prev();
        //dump_cursor(cursor);
        CHECK( cursor.is_pointing_object() == true );
        CHECK( (*cursor)->get_id() == 41L );
        CHECK( cursor.instrument() == 1 );
        CHECK( cursor.segment() == 0 );
        CHECK( cursor.staff() == 1 );
        CHECK( is_equal_time(cursor.timepos(), 128.0f) );
    }

    TEST_FIXTURE(ScoreCursorTestFixture, ScoreCursorMovePrev_9)
    {
        //9. cross last barline (move from end of staff)
        Document doc(*m_pLibraryScope);
        doc.from_file(m_scores_path + "90013-two-instruments-four-staves.lms" );
        DocCursor cursor(&doc);
        cursor.enter_element();
        cursor.point_to_barline(25L, 0);
        cursor.move_next();     //move to end of staff
        cursor.move_prev();     //test: move back
        //dump_cursor(cursor);
        CHECK( cursor.is_pointing_object() == true );
        CHECK( (*cursor)->get_id() == 25L );
        CHECK( cursor.instrument() == 0 );
        CHECK( cursor.segment() == 1 );
        CHECK( cursor.staff() == 0 );
        CHECK( is_equal_time(cursor.timepos(), 128.0f) );
    }

    TEST_FIXTURE(ScoreCursorTestFixture, ScoreCursorMovePrev_10)
    {
        //10. from start of staff to prev staff in same instrument
        Document doc(*m_pLibraryScope);
        doc.from_file(m_scores_path + "90013-two-instruments-four-staves.lms" );
        DocCursor cursor(&doc);
        cursor.enter_element();
        cursor.point_to(9L);

        cursor.move_prev();
        CHECK( cursor.is_pointing_object() == false );
        CHECK( cursor.instrument() == 0 );
        CHECK( cursor.segment() == 2 );
        CHECK( cursor.staff() == 0 );
        CHECK( is_equal_time(cursor.timepos(), 0.0f) );

        cursor.move_prev();
        CHECK( cursor.is_pointing_object() == true );
        CHECK( (*cursor)->get_id() == 25L );
        CHECK( cursor.instrument() == 0 );
        CHECK( cursor.segment() == 1 );
        CHECK( cursor.staff() == 0 );
        CHECK( is_equal_time(cursor.timepos(), 128.0f) );
    }

    TEST_FIXTURE(ScoreCursorTestFixture, ScoreCursorMovePrev_11)
    {
        //11. from start of staff to prev instrument:
        //    c29 (instr 1, staff 0) -> eos (instr 0, staff 1) -> b25 (staff 1) 
        Document doc(*m_pLibraryScope);
        doc.from_file(m_scores_path + "90013-two-instruments-four-staves.lms" );
        DocCursor cursor(&doc);
        cursor.enter_element();
        cursor.point_to(29L);

        cursor.move_prev();
        CHECK( cursor.is_pointing_object() == false );
        CHECK( cursor.instrument() == 0 );
        CHECK( cursor.segment() == 2 );
        CHECK( cursor.staff() == 1 );
        CHECK( is_equal_time(cursor.timepos(), 0.0f) );

        cursor.move_prev();
        CHECK( cursor.is_pointing_object() == true );
        CHECK( (*cursor)->get_id() == 25L );
        CHECK( cursor.instrument() == 0 );
        CHECK( cursor.segment() == 1 );
        CHECK( cursor.staff() == 1 );
        CHECK( is_equal_time(cursor.timepos(), 128.0f) );
    }

    TEST_FIXTURE(ScoreCursorTestFixture, ScoreCursorMovePrev_12)
    {
        //12. at start of score: remains there
        Document doc(*m_pLibraryScope);
        doc.from_file(m_scores_path + "90013-two-instruments-four-staves.lms" );
        DocCursor cursor(&doc);
        cursor.enter_element();
        cursor.point_to(8L);
        cursor.move_prev();
        //dump_cursor(cursor);
        CHECK( cursor.is_pointing_object() == true );
        CHECK( cursor.instrument() == 0 );
        CHECK( cursor.segment() == 0 );
        CHECK( cursor.staff() == 0 );
        CHECK( is_equal_time(cursor.timepos(), 0.0f) );
    }

    TEST_FIXTURE(ScoreCursorTestFixture, ScoreCursorMovePrev_13)
    {
        //13. from end of score to last note
        Document doc(*m_pLibraryScope);
        doc.from_string("(lenmusdoc#0 (vers#1 0.0) (content#2 (score#3 (vers#4 1.6) (instrument#5 (musicData#6 (clef#7 G) (n#8 c4 q))))))" );
        DocCursor cursor(&doc);
        cursor.enter_element();
        cursor.point_to(8L);
        cursor.move_next();     //move to end of score
        cursor.move_prev();     //test: move back
        //dump_cursor(cursor);
        CHECK( cursor.is_pointing_object() == true );
        CHECK( (*cursor)->get_id() == 8L );
        CHECK( cursor.instrument() == 0 );
        CHECK( cursor.segment() == 0 );
        CHECK( cursor.staff() == 0 );
        CHECK( is_equal_time(cursor.timepos(), 0.0f) );
    }

    TEST_FIXTURE(ScoreCursorTestFixture, ScoreCursorMovePrev_14)
    {
        //14. back from empty time
        Document doc(*m_pLibraryScope);
        doc.from_file(m_scores_path + "90013-two-instruments-four-staves.lms" );
        DocCursor cursor(&doc);
        cursor.enter_element();
        cursor.point_to(42L);
        cursor.move_next();     //to empty time
        cursor.move_prev();
        //dump_cursor(cursor);
        CHECK( cursor.is_pointing_object() == true );
        CHECK( (*cursor)->get_id() == 42L );
        CHECK( cursor.instrument() == 1 );
        CHECK( cursor.segment() == 1 );
        CHECK( cursor.staff() == 0 );
        CHECK( is_equal_time(cursor.timepos(), 0.0f) );
    }

    TEST_FIXTURE(ScoreCursorTestFixture, ScoreCursorMovePrev_15)
    {
        //15. from object to empty time
        Document doc(*m_pLibraryScope);
        doc.from_file(m_scores_path + "90013-two-instruments-four-staves.lms" );
        DocCursor cursor(&doc);
        cursor.enter_element();
        cursor.point_to_barline(48L, 0);

        cursor.move_prev();
        //dump_cursor(cursor);
        CHECK( cursor.is_pointing_object() == false );
        CHECK( cursor.instrument() == 1 );
        CHECK( cursor.segment() == 1 );
        CHECK( cursor.staff() == 0 );
        CHECK( is_equal_time(cursor.timepos(), 64.0f) );

        cursor.move_prev();
        //dump_cursor(cursor);
        CHECK( cursor.is_pointing_object() == true );
        CHECK( (*cursor)->get_id() == 42L );
        CHECK( cursor.instrument() == 1 );
        CHECK( cursor.segment() == 1 );
        CHECK( cursor.staff() == 0 );
        CHECK( is_equal_time(cursor.timepos(), 0.0f) );
    }

}

#endif  // _LM_DEBUG_

