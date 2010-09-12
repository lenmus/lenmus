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
#include "lenmus_parser.h"
#include "lenmus_analyser.h"
#include "lenmus_internal_model.h"
#include "lenmus_compiler.h"
#include "lenmus_model_builder.h"
#include "lenmus_basic_model.h"

using namespace UnitTest;
using namespace std;
using namespace lenmus;


class ModelBuilderTestFixture
{
public:

    ModelBuilderTestFixture()     //SetUp fixture
    {
        m_scores_path = "../../../../test-scores/";
        m_pLibraryScope = new LibraryScope(cout);
    }

    ~ModelBuilderTestFixture()    //TearDown fixture
    {
        delete m_pLibraryScope;
    }

    LibraryScope* m_pLibraryScope;
    std::string m_scores_path;
};

SUITE(ModelBuilderTest)
{

    TEST_FIXTURE(ModelBuilderTestFixture, ModelBuilderScore)
    {
        DocumentScope documentScope(cout);
        LdpParser* parser = Injector::inject_LdpParser(*m_pLibraryScope, documentScope);
        Analyser* analyser = Injector::inject_Analyser(*m_pLibraryScope, documentScope);
        ModelBuilder* builder = Injector::inject_ModelBuilder(documentScope);
        LdpCompiler compiler(parser, analyser, builder, documentScope.id_assigner());
        ImoDocument* pDoc = compiler.compile_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (musicData (n c4 q) (barline simple))))))" );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
        CHECK( pScore != NULL );
        CHECK( pScore->get_num_instruments() == 1 );
        CHECK( pScore->get_staffobjs_table() != NULL );
    }

}


#endif  // _LM_DEBUG_

