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
#include "lenmus_parser.h"

//to delete singletons
#include "lenmus_factory.h"
#include "lenmus_elements.h"


using namespace UnitTest;
using namespace std;
using namespace lenmus;


class DocumentTestFixture
{
public:

    DocumentTestFixture()     //SetUp fixture
    {
    }

    ~DocumentTestFixture()    //TearDown fixture
    {
        delete Factory::instance();
    }
};

SUITE(DocumentTest)
{
    TEST_FIXTURE(DocumentTestFixture, DocumentEmpty)
    {
        Document doc;
        Document::iterator it = doc.begin();
        //cout << doc.to_string(it) << endl;
        CHECK( doc.to_string(it) == "(lenmusdoc (vers 0.0))" );
        CHECK( doc.is_modified() == false );
    }

    TEST_FIXTURE(DocumentTestFixture, DocumentSetModifiedFlag)
    {
        Document doc;
        Document::iterator it = doc.begin();
        doc.set_modified(true);
        CHECK( doc.is_modified() == true );
        doc.set_modified(false);
        CHECK( doc.is_modified() == false );
    }

    TEST_FIXTURE(DocumentTestFixture, DocumentIteratorAdvance)
    {
        Document doc;
        Document::iterator it = doc.begin();
        ++it; 
        CHECK( doc.to_string(it) == "(vers 0.0)" );
        ++it;
        CHECK( doc.to_string(it) == "0.0" );
        ++it;
        CHECK( it == doc.end() );
    }

    TEST_FIXTURE(DocumentTestFixture, DocumentPushBack)
    {
        Document doc;
        Document::iterator it = doc.begin();
        LdpParser parser(cout);
        SpLdpTree tree = parser.parse_text("(text ''Title of this book'')");
        LdpElement* elm = tree->get_root();
        doc.add_param(it, elm);
        //cout << doc.to_string(it) << endl;
        CHECK( doc.to_string() == "(lenmusdoc (vers 0.0) (text \"Title of this book\"))" );
    }

    TEST_FIXTURE(DocumentTestFixture, DocumentLoad)
    {
        Document doc;
        doc.load("../../test-scores/00011-empty-fill-page.lms");
        Document::iterator it = doc.begin();
        //cout << doc.to_string(it) << endl;
        CHECK( doc.to_string(it) == "(lenmusdoc (vers 0.0) (score (vers 1.6) (language en iso-8859-1) (systemLayout first (systemMargins 0 0 0 2000)) (systemLayout other (systemMargins 0 0 1200 2000)) (opt Score.FillPageWithEmptyStaves true) (opt StaffLines.StopAtFinalBarline false) (instrument (musicData ))))" );
        CHECK( doc.is_modified() == false );
    }

    TEST_FIXTURE(DocumentTestFixture, DocumentDirectLoad)
    {
        Document doc("../../test-scores/00011-empty-fill-page.lms");
        Document::iterator it = doc.begin();
        CHECK( doc.to_string(it) == "(lenmusdoc (vers 0.0) (score (vers 1.6) (language en iso-8859-1) (systemLayout first (systemMargins 0 0 0 2000)) (systemLayout other (systemMargins 0 0 1200 2000)) (opt Score.FillPageWithEmptyStaves true) (opt StaffLines.StopAtFinalBarline false) (instrument (musicData ))))" );
        CHECK( doc.is_modified() == false );
    }

    TEST_FIXTURE(DocumentTestFixture, DocumentFromString)
    {
        Document doc;
        doc.from_string("(lenmusdoc (vers 0.0) (score (vers 1.6) (language en iso-8859-1) (systemLayout first (systemMargins 0 0 0 2000)) (systemLayout other (systemMargins 0 0 1200 2000)) (opt Score.FillPageWithEmptyStaves true) (opt StaffLines.StopAtFinalBarline false) (instrument (musicData ))))" );
        Document::iterator it = doc.begin();
        CHECK( doc.to_string(it) == "(lenmusdoc (vers 0.0) (score (vers 1.6) (language en iso-8859-1) (systemLayout first (systemMargins 0 0 0 2000)) (systemLayout other (systemMargins 0 0 1200 2000)) (opt Score.FillPageWithEmptyStaves true) (opt StaffLines.StopAtFinalBarline false) (instrument (musicData ))))" );
        CHECK( doc.is_modified() == false );
    }

    TEST_FIXTURE(DocumentTestFixture, DocumentScoreFromString)
    {
        Document doc;
        doc.from_string("(score (vers 1.6) (language en iso-8859-1) (systemLayout first (systemMargins 0 0 0 2000)) (systemLayout other (systemMargins 0 0 1200 2000)) (opt Score.FillPageWithEmptyStaves true) (opt StaffLines.StopAtFinalBarline false) (instrument (musicData )))" );
        Document::iterator it = doc.begin();
        CHECK( doc.to_string(it) == "(lenmusdoc (vers 0.0) (score (vers 1.6) (language en iso-8859-1) (systemLayout first (systemMargins 0 0 0 2000)) (systemLayout other (systemMargins 0 0 1200 2000)) (opt Score.FillPageWithEmptyStaves true) (opt StaffLines.StopAtFinalBarline false) (instrument (musicData ))))" );
        CHECK( doc.is_modified() == false );
    }

    TEST_FIXTURE(DocumentTestFixture, DocumentRemoveParam)
    {
        Document doc;
        Document::iterator it = doc.begin();
        ++it;   //vers
        LdpElement* elm = doc.remove(it);
        //cout << doc.to_string() << endl;
        CHECK( doc.to_string() == "(lenmusdoc )" );
        CHECK( doc.is_modified() == false );
        delete elm;
    }

    TEST_FIXTURE(DocumentTestFixture, DocumentInsertParam)
    {
        Document doc;
        LdpParser parser(cout);
        SpLdpTree tree = parser.parse_text("(dx 20)");
        LdpElement* elm = tree->get_root();
        Document::iterator it = doc.begin();
        ++it;   //vers
        doc.insert(it, elm);
        //cout << doc.to_string() << endl;
        CHECK( doc.is_modified() == false );
        CHECK( doc.to_string() == "(lenmusdoc (dx 20) (vers 0.0))" );
    }

    TEST_FIXTURE(DocumentTestFixture, DocumentGetScoreInEmptyDoc)
    {
        Document doc;
        Document::iterator& it = doc.get_score();
        CHECK( it == doc.end() );
    }

    TEST_FIXTURE(DocumentTestFixture, DocumentGetScore)
    {
        Document doc("../../test-scores/00011-empty-fill-page.lms");
        Document::iterator& it = doc.get_score();
        CHECK( doc.to_string(it) == "(score (vers 1.6) (language en iso-8859-1) (systemLayout first (systemMargins 0 0 0 2000)) (systemLayout other (systemMargins 0 0 1200 2000)) (opt Score.FillPageWithEmptyStaves true) (opt StaffLines.StopAtFinalBarline false) (instrument (musicData )))" );
    }

    // undo/redo --------------------------------------------------------

    TEST_FIXTURE(DocumentTestFixture, DocumentPushBackCommandIsStored)
    {
        Document doc;
        Document::iterator it = doc.begin();
        LdpParser parser(cout);
        SpLdpTree tree = parser.parse_text("(text ''Title of this book'')");
        LdpElement* elm = tree->get_root();
        DocCommandExecuter ce(&doc);
        ce.execute( new DocCommandPushBack(it, elm) );
        CHECK( ce.undo_stack_size() == 1 );
        CHECK( doc.to_string() == "(lenmusdoc (vers 0.0) (text \"Title of this book\"))" );
    }

    TEST_FIXTURE(DocumentTestFixture, DocumentUndoPushBackCommand)
    {
        Document doc;
        Document::iterator it = doc.begin();
        LdpParser parser(cout);
        SpLdpTree tree = parser.parse_text("(text ''Title of this book'')");
        LdpElement* elm = tree->get_root();
        DocCommandExecuter ce(&doc);
        ce.execute( new DocCommandPushBack(it, elm) );
        ce.undo();
        CHECK( ce.undo_stack_size() == 0 );
        CHECK( doc.to_string() == "(lenmusdoc (vers 0.0))" );
    }

    TEST_FIXTURE(DocumentTestFixture, DocumentUndoRedoPushBackCommand)
    {
        Document doc;
        Document::iterator it = doc.begin();
        LdpParser parser(cout);
        SpLdpTree tree = parser.parse_text("(text ''Title of this book'')");
        LdpElement* elm = tree->get_root();
        DocCommandExecuter ce(&doc);
        ce.execute( new DocCommandPushBack(it, elm) );
        ce.undo();
        ce.redo();
        CHECK( ce.undo_stack_size() == 1 );
        CHECK( doc.to_string() == "(lenmusdoc (vers 0.0) (text \"Title of this book\"))" );
    }

    TEST_FIXTURE(DocumentTestFixture, DocumentUndoRedoUndoPushBackCommand)
    {
        Document doc;
        Document::iterator it = doc.begin();
        LdpParser parser(cout);
        SpLdpTree tree = parser.parse_text("(text ''Title of this book'')");
        LdpElement* elm = tree->get_root();
        DocCommandExecuter ce(&doc);
        ce.execute( new DocCommandPushBack(it, elm) );
        ce.undo();
        ce.redo();
        ce.undo();
        //cout << doc.to_string() << endl;
        CHECK( ce.undo_stack_size() == 0 );
        CHECK( doc.to_string() == "(lenmusdoc (vers 0.0))" );
    }

    TEST_FIXTURE(DocumentTestFixture, DocumentRemoveCommandIsStored)
    {
        Document doc;
        Document::iterator it = doc.begin();
        ++it;   //vers
        DocCommandExecuter ce(&doc);
        ce.execute( new DocCommandRemove(it) );
        CHECK( ce.undo_stack_size() == 1 );
        CHECK( doc.to_string() == "(lenmusdoc )" );
    }

    TEST_FIXTURE(DocumentTestFixture, DocumentUndoRemoveCommand)
    {
        Document doc;
        Document::iterator it = doc.begin();
        ++it;   //vers
        DocCommandExecuter ce(&doc);
        ce.execute( new DocCommandRemove(it) );
        ce.undo();
        CHECK( ce.undo_stack_size() == 0 );
        //cout << doc.to_string() << endl;
        CHECK( doc.to_string() == "(lenmusdoc (vers 0.0))" );
    }

    TEST_FIXTURE(DocumentTestFixture, DocumentRedoRemoveCommand)
    {
        Document doc;
        Document::iterator it = doc.begin();
        ++it;   //vers
        DocCommandExecuter ce(&doc);
        ce.execute( new DocCommandRemove(it) );
        ce.undo();
        ce.redo();
        //cout << doc.to_string() << endl;
        CHECK( doc.to_string() == "(lenmusdoc )" );
    }

    TEST_FIXTURE(DocumentTestFixture, DocumentUndoRedoUndoRemoveCommand)
    {
        Document doc;
        Document::iterator it = doc.begin();
        ++it;   //vers
        DocCommandExecuter ce(&doc);
        ce.execute( new DocCommandRemove(it) );
        ce.undo();
        ce.redo();
        ce.undo();
        //cout << doc.to_string() << endl;
        CHECK( doc.to_string() == "(lenmusdoc (vers 0.0))" );
    }

    TEST_FIXTURE(DocumentTestFixture, DocumentInsertCommandIsStored)
    {
        Document doc;
        LdpParser parser(cout);
        SpLdpTree tree = parser.parse_text("(dx 20)");
        LdpElement* elm = tree->get_root();
        Document::iterator it = doc.begin();
        ++it;   //vers
        DocCommandExecuter ce(&doc);
        ce.execute( new DocCommandInsert(it, elm) );
        CHECK( ce.undo_stack_size() == 1 );
        //cout << doc.to_string() << endl;
        CHECK( doc.to_string() == "(lenmusdoc (dx 20) (vers 0.0))" );
    }

    TEST_FIXTURE(DocumentTestFixture, DocumentUndoInsertCommandIsStored)
    {
        Document doc;
        LdpParser parser(cout);
        SpLdpTree tree = parser.parse_text("(dx 20)");
        LdpElement* elm = tree->get_root();
        Document::iterator it = doc.begin();
        ++it;   //vers
        DocCommandExecuter ce(&doc);
        ce.execute( new DocCommandInsert(it, elm) );
        ce.undo();
        CHECK( ce.undo_stack_size() == 0 );
        //cout << doc.to_string() << endl;
        CHECK( doc.to_string() == "(lenmusdoc (vers 0.0))" );
    }

    TEST_FIXTURE(DocumentTestFixture, DocumentUndoRedoInsertCommandIsStored)
    {
        Document doc;
        LdpParser parser(cout);
        SpLdpTree tree = parser.parse_text("(dx 20)");
        LdpElement* elm = tree->get_root();
        Document::iterator it = doc.begin();
        ++it;   //vers
        DocCommandExecuter ce(&doc);
        ce.execute( new DocCommandInsert(it, elm) );
        ce.undo();
        ce.redo();
        CHECK( ce.undo_stack_size() == 1 );
        //cout << doc.to_string() << endl;
        CHECK( doc.to_string() == "(lenmusdoc (dx 20) (vers 0.0))" );
    }

    TEST_FIXTURE(DocumentTestFixture, DocumentUndoRedoUndoInsertCommandIsStored)
    {
        Document doc;
        LdpParser parser(cout);
        SpLdpTree tree = parser.parse_text("(dx 20)");
        LdpElement* elm = tree->get_root();
        Document::iterator it = doc.begin();
        ++it;   //vers
        DocCommandExecuter ce(&doc);
        ce.execute( new DocCommandInsert(it, elm) );
        ce.undo();
        ce.redo();
        ce.undo();
        CHECK( ce.undo_stack_size() == 0 );
        //cout << doc.to_string() << endl;
        CHECK( doc.to_string() == "(lenmusdoc (vers 0.0))" );
    }

    TEST_FIXTURE(DocumentTestFixture, DocumentRemoveNotLast)
    {
        Document doc;
        LdpParser parser(cout);
        SpLdpTree tree = parser.parse_text("(musicData (n c4 q)(r e)(n b3 e)(dx 20))");
        LdpElement* elm = tree->get_root();
        Document::iterator it = doc.begin();
        ++it;   //vers
        Document::iterator itNew = doc.insert(it, elm);
        CHECK( doc.to_string() == "(lenmusdoc (musicData (n c4 q) (r e) (n b3 e) (dx 20)) (vers 0.0))" );
        CHECK( doc.to_string( itNew ) == "(musicData (n c4 q) (r e) (n b3 e) (dx 20))" );
        ++itNew;    //n c4
        ++itNew;    //c4
        ++itNew;    //q
        ++itNew;    //r
        CHECK( doc.to_string( itNew ) == "(r e)" );

        DocCommandExecuter ce(&doc);
        ce.execute( new DocCommandRemove(itNew) );
        CHECK( ce.undo_stack_size() == 1 );
        //cout << doc.to_string() << endl;
        CHECK( doc.to_string() == "(lenmusdoc (musicData (n c4 q) (n b3 e) (dx 20)) (vers 0.0))" );
    }

    TEST_FIXTURE(DocumentTestFixture, DocumentUndoRemoveNotLast)
    {
        Document doc;
        LdpParser parser(cout);
        SpLdpTree tree = parser.parse_text("(dx 20)");
        LdpElement* elm = tree->get_root();
        Document::iterator it = doc.begin();
        ++it;   //vers
        Document::iterator itNew = doc.insert(it, elm);
        DocCommandExecuter ce(&doc);
        ce.execute( new DocCommandRemove(itNew) );
        ce.undo();
        CHECK( ce.undo_stack_size() == 0 );
        //cout << doc.to_string() << endl;
        CHECK( doc.to_string() == "(lenmusdoc (dx 20) (vers 0.0))" );
    }

    TEST_FIXTURE(DocumentTestFixture, DocumentUndoRedoRemoveNotLast)
    {
        Document doc;
        LdpParser parser(cout);
        SpLdpTree tree = parser.parse_text("(dx 20)");
        LdpElement* elm = tree->get_root();
        Document::iterator it = doc.begin();
        ++it;   //vers
        Document::iterator itNew = doc.insert(it, elm);
        DocCommandExecuter ce(&doc);
        ce.execute( new DocCommandRemove(itNew) );
        ce.undo();
        ce.redo();
        CHECK( ce.undo_stack_size() == 1 );
        //cout << doc.to_string() << endl;
        CHECK( doc.to_string() == "(lenmusdoc (vers 0.0))" );
    }

    TEST_FIXTURE(DocumentTestFixture, DocumentUndoRedoUndoRemoveNotLast)
    {
        Document doc;
        LdpParser parser(cout);
        SpLdpTree tree = parser.parse_text("(dx 20)");
        LdpElement* elm = tree->get_root();
        Document::iterator it = doc.begin();
        ++it;   //vers
        Document::iterator itNew = doc.insert(it, elm);
        DocCommandExecuter ce(&doc);
        ce.execute( new DocCommandRemove(itNew) );
        ce.undo();
        ce.redo();
        ce.undo();
        CHECK( ce.undo_stack_size() == 0 );
        //cout << doc.to_string() << endl;
        CHECK( doc.to_string() == "(lenmusdoc (dx 20) (vers 0.0))" );
    }

    TEST_FIXTURE(DocumentTestFixture, DocumentRemoveFirst)
    {
        Document doc;
        LdpParser parser(cout);
        SpLdpTree tree = parser.parse_text("(musicData (n c4 q)(r e)(n b3 e)(dx 20))");
        LdpElement* elm = tree->get_root();
        Document::iterator it = doc.begin();
        ++it;   //vers
        Document::iterator itNew = doc.insert(it, elm);
        CHECK( doc.to_string() == "(lenmusdoc (musicData (n c4 q) (r e) (n b3 e) (dx 20)) (vers 0.0))" );
        CHECK( doc.to_string( itNew ) == "(musicData (n c4 q) (r e) (n b3 e) (dx 20))" );
        ++itNew;    //n c4
        CHECK( doc.to_string( itNew ) == "(n c4 q)" );

        DocCommandExecuter ce(&doc);
        ce.execute( new DocCommandRemove(itNew) );
        CHECK( ce.undo_stack_size() == 1 );
        //cout << doc.to_string() << endl;
        CHECK( doc.to_string() == "(lenmusdoc (musicData (r e) (n b3 e) (dx 20)) (vers 0.0))" );
    }

}

#endif  // _LM_DEBUG_

