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
#include "lenmus_doc_manager.h"
#include "lenmus_document.h"
#include "lenmus_user_command.h"
#include "lenmus_view.h"

//to delete singletons
#include "lenmus_factory.h"
#include "lenmus_elements.h"


using namespace UnitTest;
using namespace std;
using namespace lenmus;


class MvcModelTestFixture
{
public:

    MvcModelTestFixture()     //SetUp fixture
    {
    }

    ~MvcModelTestFixture()    //TearDown fixture
    {
        delete Factory::instance();
    }
};

SUITE(MvcModelTest)
{
    TEST_FIXTURE(MvcModelTestFixture, MvcBuilderNewDocument)
    {
        MvcCollection docviews;
        MvcBuilder builder(docviews);
        CHECK( docviews.get_num_documents() == 0 );
        builder.new_document();
        CHECK( docviews.get_num_documents() == 1 );
        Document* pDoc = docviews.get_document(0);
        CHECK( pDoc->to_string() == "(lenmusdoc (vers 0.0) (content ))" );
        UserCommandExecuter* pCmdExec = docviews.get_command_executer(0);
        CHECK( pCmdExec );
    }

    TEST_FIXTURE(MvcModelTestFixture, MvcCollectionCloseDocumentByIndex)
    {
        MvcCollection docviews;
        MvcBuilder builder(docviews);
        builder.new_document();
        CHECK( docviews.get_num_documents() == 1 );
        docviews.close_document(0);
        CHECK( docviews.get_num_documents() == 0 );
    }

    TEST_FIXTURE(MvcModelTestFixture, MvcCollectionCloseDocumentByPointer)
    {
        MvcCollection docviews;
        MvcBuilder builder(docviews);
        Document* pDoc = builder.new_document();
        CHECK( docviews.get_num_documents() == 1 );
        docviews.close_document(pDoc);
        CHECK( docviews.get_num_documents() == 0 );
    }

    TEST_FIXTURE(MvcModelTestFixture, MvcBuilderOpenDocument)
    {
        MvcCollection docviews;
        MvcBuilder builder(docviews);
        CHECK( docviews.get_num_documents() == 0 );
        builder.open_document("../../test-scores/00011-empty-fill-page.lms");
        CHECK( docviews.get_num_documents() == 1 );
        Document* pDoc = docviews.get_document(0);
        CHECK( pDoc->to_string() == "(lenmusdoc (vers 0.0) (content (score (vers 1.6) (language en iso-8859-1) (systemLayout first (systemMargins 0 0 0 2000)) (systemLayout other (systemMargins 0 0 1200 2000)) (opt Score.FillPageWithEmptyStaves true) (opt StaffLines.StopAtFinalBarline false) (instrument (musicData )))))" );
        UserCommandExecuter* pCmdExec = docviews.get_command_executer(0);
        CHECK( pCmdExec );
    }

    TEST_FIXTURE(MvcModelTestFixture, MvcCollectionGetCommandExecuter)
    {
        MvcCollection docviews;
        MvcBuilder builder(docviews);
        Document* pDoc = builder.new_document();
        UserCommandExecuter* pExec = docviews.get_command_executer(pDoc);
        CHECK( pExec );
        docviews.close_document(pDoc);
    }

    TEST_FIXTURE(MvcModelTestFixture, MvcCollectionAddView)
    {
        MvcCollection docviews;
        MvcBuilder builder(docviews);
        Document* pDoc = builder.new_document();
        View* pView = new EditView(pDoc);
        CHECK( docviews.get_num_views(pDoc) == 0 );
        docviews.add_view(pDoc, pView);
        CHECK( docviews.get_num_views(pDoc) == 1 );
        docviews.close_document(pDoc);
        delete pView;
    }

    TEST_FIXTURE(MvcModelTestFixture, MvcCollectionViewCursor)
    {
        MvcCollection docviews;
        MvcBuilder builder(docviews);
        Document* pDoc = builder.new_document();
        EditView* pView = new EditView(pDoc);
        docviews.add_view(pDoc, pView);
        DocIterator& cursor = pView->get_cursor();
        cursor.exit_element();
        //cout << (*cursor)->to_string() << endl;
        CHECK( (*cursor)->to_string() == "(content )" );
        docviews.close_document(pDoc);
        delete pView;
    }
}

#endif  // _LM_DEBUG_

