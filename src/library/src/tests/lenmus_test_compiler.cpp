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
#include "lenmus_compiler.h"

using namespace UnitTest;
using namespace std;
using namespace lenmus;


class LdpCompilerTestFixture
{
public:

    LdpCompilerTestFixture()     //SetUp fixture
    {
        m_pLibraryScope = new LibraryScope(cout);
    }

    ~LdpCompilerTestFixture()    //TearDown fixture
    {
        delete m_pLibraryScope;
    }

    LibraryScope* m_pLibraryScope;
};

SUITE(LdpCompilerTest)
{
    TEST_FIXTURE(LdpCompilerTestFixture, LdpCompilerCreateEmpty)
    {
        DocumentScope documentScope(cout);
        LdpCompiler compiler(*m_pLibraryScope, documentScope);
        LdpTree* tree = compiler.create_empty();
        CHECK( tree != NULL );
        LdpElement* root = tree->get_root();
        //cout << root->to_string() << endl;
        CHECK( root->to_string() == "(lenmusdoc (vers 0.0) (content ))" );
        delete root;
        delete tree;
    }

    TEST_FIXTURE(LdpCompilerTestFixture, LdpCompilerFromString)
    {
        DocumentScope documentScope(cout);
        LdpCompiler compiler(*m_pLibraryScope, documentScope);
        LdpTree* tree = compiler.compile_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (language en iso-8859-1) (systemLayout first (systemMargins 0 0 0 2000)) (systemLayout other (systemMargins 0 0 1200 2000)) (opt Score.FillPageWithEmptyStaves true) (opt StaffLines.StopAtFinalBarline false) (instrument (musicData )))))" );
        CHECK( tree != NULL );
        LdpElement* root = tree->get_root();
        //cout << root->to_string() << endl;
        CHECK( root->to_string() == "(lenmusdoc (vers 0.0) (content (score (vers 1.6) (systemLayout first (systemMargins 0 0 0 2000)) (systemLayout other (systemMargins 0 0 1200 2000)) (opt Score.FillPageWithEmptyStaves true) (opt StaffLines.StopAtFinalBarline false) (instrument (musicData )))))" );
        delete root;
        delete tree;
    }

    TEST_FIXTURE(LdpCompilerTestFixture, LdpCompilerScoreFromString)
    {
        DocumentScope documentScope(cout);
        LdpCompiler compiler(*m_pLibraryScope, documentScope);
        LdpTree* tree = compiler.compile_string("(score (vers 1.6) (language en iso-8859-1) (systemLayout first (systemMargins 0 0 0 2000)) (systemLayout other (systemMargins 0 0 1200 2000)) (opt Score.FillPageWithEmptyStaves true) (opt StaffLines.StopAtFinalBarline false) (instrument (musicData )))" );
        CHECK( tree != NULL );
        LdpElement* root = tree->get_root();
        //cout << root->to_string() << endl;
        CHECK( root->to_string() == "(lenmusdoc (vers 0.0) (content (score (vers 1.6) (systemLayout first (systemMargins 0 0 0 2000)) (systemLayout other (systemMargins 0 0 1200 2000)) (opt Score.FillPageWithEmptyStaves true) (opt StaffLines.StopAtFinalBarline false) (instrument (musicData )))))" );
        delete root;
        delete tree;
    }

    TEST_FIXTURE(LdpCompilerTestFixture, LdpCompilerScoreIDsFixed)
    {
        DocumentScope documentScope(cout);
        LdpCompiler compiler(*m_pLibraryScope, documentScope);
        LdpTree* tree = compiler.compile_string("(score (vers 1.6) (language en iso-8859-1) (systemLayout first (systemMargins 0 0 0 2000)) (systemLayout other (systemMargins 0 0 1200 2000)) (opt Score.FillPageWithEmptyStaves true) (opt StaffLines.StopAtFinalBarline false) (instrument (musicData )))" );
        CHECK( tree != NULL );
        LdpElement* elm = tree->get_root();
        //cout << elm->to_string() << endl;
        CHECK( elm->get_id() == 0L );   //lenmusdoc
        elm = elm->get_first_child();   //(vers 0.0)
        CHECK( elm->get_id() == 1L );
        elm = elm->get_next_sibling();  //content
        CHECK( elm->get_id() == 2L );
        elm = elm->get_first_child();   //score
        //cout << elm->get_name() << ". ID = " << elm->get_id() << endl;
        CHECK( elm->get_id() == 3L );
        elm = elm->get_first_child();   //(vers 1.6)
        CHECK( elm->get_id() == 4L );
        elm = elm->get_next_sibling();  //systemLayout
        CHECK( elm->get_id() == 6L );
        delete tree->get_root();
        delete tree;
    }

    TEST_FIXTURE(LdpCompilerTestFixture, LdpCompilerFromFile)
    {
        DocumentScope documentScope(cout);
        LdpCompiler compiler(*m_pLibraryScope, documentScope);
        LdpTree* tree = compiler.compile_file("../../test-scores/00011-empty-fill-page.lms");
        CHECK( tree != NULL );
        LdpElement* root = tree->get_root();
        //cout << root->to_string() << endl;
        CHECK( root->to_string() == "(lenmusdoc (vers 0.0) (content (score (vers 1.6) (systemLayout first (systemMargins 0 0 0 2000)) (systemLayout other (systemMargins 0 0 1200 2000)) (opt Score.FillPageWithEmptyStaves true) (opt StaffLines.StopAtFinalBarline false) (instrument (musicData )))))" );
        delete root;
        delete tree;
    }

};

#endif  // _LM_DEBUG_

