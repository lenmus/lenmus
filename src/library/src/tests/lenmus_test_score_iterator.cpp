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
#include "lenmus_document_cursor.h"
#include "lenmus_core_table.h"
#include "lenmus_score_iterator.h"

//to delete singletons
#include "lenmus_factory.h"
#include "lenmus_elements.h"


using namespace UnitTest;
using namespace std;
using namespace lenmus;


class ScoreIteratorTestFixture
{
public:

    ScoreIteratorTestFixture()     //SetUp fixture
    {
    }

    ~ScoreIteratorTestFixture()    //TearDown fixture
    {
        delete Factory::instance();
    }

};

SUITE(ScoreIteratorTest)
{

    TEST_FIXTURE(ScoreIteratorTestFixture, ScoreIteratorEmptyScore)
    {
        Document doc;
        doc.from_string("(lenmusdoc (vers 0.0) (content ))" );
        DocIterator cursor(&doc);
        cursor.start_of_content();  //points to score
        ScoreIterator table(&doc, cursor.get_iterator());
        CHECK( table.num_entries() == 0 );
    }

}

#endif  // _LM_DEBUG_

