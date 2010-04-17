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
#include <sstream>

//classes related to these tests
#include "lenmus_document.h"
#include "lenmus_parser.h"
#include "lenmus_user_command.h"

//to delete singletons
#include "lenmus_factory.h"
#include "lenmus_elements.h"


using namespace UnitTest;
using namespace std;
using namespace lenmus;


class TestUserCommand : public UserCommand
{
public:
    TestUserCommand(Document::iterator& it1, Document::iterator& it2) 
        : UserCommand("test command"), m_it1(it1), m_it2(it2) {}
    ~TestUserCommand() {};

protected:
    bool do_actions(DocCommandExecuter* dce) {
        dce->execute( new DocCommandRemove(m_it1) );
        dce->execute( new DocCommandRemove(m_it2) );
        return true;
    }

    Document::iterator m_it1;
    Document::iterator m_it2;

};



class UserCommandTestFixture
{
public:

    UserCommandTestFixture()     //SetUp fixture
    {
        m_doc = NULL;
    }

    ~UserCommandTestFixture()    //TearDown fixture
    {
        delete_test_data();
        delete Factory::instance();
    }

    Document* m_doc;
    Document::iterator m_it1;
    Document::iterator m_it2;

    void create_test_data() {
        m_doc = new Document();
        Document::iterator it = m_doc->begin();
        LdpParser parser(cout);
        SpLdpTree tree = parser.parse_text("(musicData (text 1)(n c4 q)(clef G)(key e))");
        LdpElement* elm = tree->get_root();
        m_doc->add_param(it, elm);
        it = m_doc->begin();
        ++it;   //vers
        ++it;   //0.0
        ++it;   //musicData
        m_it1 = ++it;   //text
        ++it;   //1
        ++it;   //n
        ++it;   //c4
        ++it;   //q
        ++it;   //clef
        ++it;   //G
        m_it2 = ++it;   //key
    }

    void delete_test_data() {
        if (m_doc) delete m_doc;
        m_doc = NULL;
    }
};

SUITE(UserCommandTest)
{
    TEST_FIXTURE(UserCommandTestFixture, UserCommandExecuteTestCommand)
    {
        create_test_data();
        DocCommandExecuter ce(m_doc);
        UserCommandExecuter executer(&ce);
        TestUserCommand cmd(m_it1, m_it2);
        executer.execute(cmd);
        //cout << m_doc->to_string() << endl;
        CHECK( executer.undo_stack_size() == 1 );
        CHECK( m_doc->to_string() == "(lenmusdoc (vers 0.0) (musicData (n c4 q) (clef G)))" );
        CHECK( m_doc->is_modified() == true );
        delete_test_data();
    }

    TEST_FIXTURE(UserCommandTestFixture, UserCommandUndoTestCommand)
    {
        create_test_data();
        DocCommandExecuter ce(m_doc);
        UserCommandExecuter executer(&ce);
        TestUserCommand cmd(m_it1, m_it2);
        executer.execute(cmd);
        executer.undo();
        //cout << m_doc->to_string() << endl;
        CHECK( executer.undo_stack_size() == 0 );
        CHECK( m_doc->to_string() == "(lenmusdoc (vers 0.0) (musicData (text 1) (n c4 q) (clef G) (key e)))" );
        CHECK( m_doc->is_modified() == false );
        delete_test_data();
    }

    TEST_FIXTURE(UserCommandTestFixture, UserCommandUndoRedoTestCommand)
    {
        create_test_data();
        DocCommandExecuter ce(m_doc);
        UserCommandExecuter executer(&ce);
        TestUserCommand cmd(m_it1, m_it2);
        executer.execute(cmd);
        executer.undo();
        executer.redo();
        //cout << m_doc->to_string() << endl;
        CHECK( executer.undo_stack_size() == 1 );
        CHECK( m_doc->to_string() == "(lenmusdoc (vers 0.0) (musicData (n c4 q) (clef G)))" );
        CHECK( m_doc->is_modified() == true );
        delete_test_data();
    }

    TEST_FIXTURE(UserCommandTestFixture, UserCommandUndoRedoUndoTestCommand)
    {
        create_test_data();
        DocCommandExecuter ce(m_doc);
        UserCommandExecuter executer(&ce);
        TestUserCommand cmd(m_it1, m_it2);
        executer.execute(cmd);
        executer.undo();
        executer.redo();
        executer.undo();
        //cout << m_doc->to_string() << endl;
        CHECK( executer.undo_stack_size() == 0 );
        CHECK( m_doc->to_string() == "(lenmusdoc (vers 0.0) (musicData (text 1) (n c4 q) (clef G) (key e)))" );
        CHECK( m_doc->is_modified() == false );
        delete_test_data();
    }

}
